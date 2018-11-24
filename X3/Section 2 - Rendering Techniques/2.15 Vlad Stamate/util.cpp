#include <stdio.h>
#include <math.h>
#include "glext.h"
#include "oglshell.h"
#include "util.h"
#include "light.h"
#include "camera.h"
#include "OGLTPrint3D.h"

ProgramType	phong_vertex, phong_pixel, sh_vertex, sh_pixel;
ProgramType phong_shader, sh_shader;

extern HWND gHWND;
extern BOOL bNoFragmentProgram;
extern PoolCameraType Camera;
extern SHCoeffStruct SHCoeff[9];
extern AUX_RGBImageRec *EnvMap;
extern GLuint envmap_texid;
extern AUX_RGBImageRec *CubeEnvMap[6];
extern GLuint cubeenvmap_texid[6];
extern BOOL bEnableSHLightingReduction;
extern int NumberOfLights;
extern int NumberOfPhongLights;
extern int number_of_balls;
extern int SphereDetail;
extern BOOL bSupportGLSL;
extern BOOL bEnvMapLoaded;
extern BOOL bDrawWireframe;

/*****************************************************************************
 * Function Name  : ReportError
 * Inputs		  : string
 * Returns		  : Nothing
 * Description    : Reports an error by displaying a message box on the screen.
 *****************************************************************************/
void ReportError(char *string)
{
	MessageBox(gHWND, string, "Error", MB_OK | MB_ICONERROR);
}

/*****************************************************************************
 * Function Name  : printInfoLog
 * Inputs		  : string
 * Returns		  : Nothing
 * Description    : Reports an error by displaying a message box on the screen.
 *****************************************************************************/
void ReportGLSLLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLcharARB *infoLog;
	char string[1024];

    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                         &infologLength);
    if (infologLength > 0)
    {
        infoLog = (GLcharARB*)malloc(infologLength);
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer\n");
            exit(1);
        }
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		if(strlen(infoLog))
		{
			sprintf(string, "InfoLog:\n%s\n\n", infoLog);
			OutputDebugString(string);
		}
        free(infoLog);
    }
}


/*****************************************************************************
 * Function Name  : ReadProgramFile
 * Inputs		  : *filename
 * Returns		  : string
 * Description    : Reads a vertex/fragment program from a file given its name.
 *****************************************************************************/
char *ReadProgramFile(char *filename)
{
	FILE *file;
	int length;
	char *program = 0;

	file = fopen(filename, "rb");

	if(file)
	{
		fseek(file,  0, SEEK_END);
		length = ftell(file);
		fseek(file,  0, SEEK_SET);

		program = (char*)malloc((length + 1)*sizeof(char));
		fread(program, length, 1, file);
		program[length] = 0;

		fclose(file);
	}

	return program;
}

/*****************************************************************************
 * Function Name  : GLLoadVertexProgram
 * Inputs		  : *file_name
 * Returns		  : Error, program_id
 * Description    : Loads a vertex program (ARB_vertex_program) to OpenGL.
 *****************************************************************************/
Error GLLoadVertexProgram(char *file_name, GLuint *program_id)
{
	char *program;
	GLenum error = 0;
	char *errorString;
	int errorPosition;
	char string[128];

	program = ReadProgramFile(file_name);
	if(!program)
	{
		sprintf(string, "Program file %s not found!", file_name);
		ReportError(string);
		return ERROR_PROGRAM_NOT_FOUND;
	}

	glGenProgramsARB(1, program_id);

	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, *program_id);
	error = glGetError();
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)strlen(program), program);
	error = glGetError();
	if(error)
	{
		errorString = (char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, (GLint*)&errorPosition);
		sprintf(string, "Vertex Program Error: %s at position %d", errorString, errorPosition);
		ReportError(string);
		return ERROR_IN_PROGRAM;
	}
	free(program);
	return NO_ERROR_PROGRAM;
}

/*****************************************************************************
 * Function Name  : GLLoadFragmentProgram
 * Inputs		  : *file_name
 * Returns		  : Error, program_id
 * Description    : Loads a fragment program (ARB_fragment_program) to OpenGL.
 *****************************************************************************/
Error GLLoadFragmentProgram(char *file_name, GLuint *program_id)
{
	char *program;
	GLenum error = 0;
	char *errorString;
	int errorPosition;
	char string[128];

	program = ReadProgramFile(file_name);
	
	if(!program)
	{
		sprintf(string, "Program file %s not found!", file_name);
		ReportError(string);
		return ERROR_PROGRAM_NOT_FOUND;
	}

	glGenProgramsARB(1, program_id);

	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, *program_id);
	error = glGetError();
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)strlen(program), program);
	error = glGetError();
	if(error)
	{
		errorString = (char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, (GLint*)&errorPosition);
		sprintf(string, "Fragment Program Error: %s at line %d", errorString, errorPosition);
		ReportError(string);
		return ERROR_IN_PROGRAM;
	}
	free(program);
	return NO_ERROR_PROGRAM;
}

/*****************************************************************************
 * Function Name  : GLLoadShader
 * Inputs		  : *vertex_shader_file, *fragment_shader_file
 * Returns		  : Error, *program_handle
 * Description    : Loads a combination of vertex and fragment shader (GLSL) to OpenGL.
 *****************************************************************************/
Error GLLoadShader(char *vertex_shader_file, char *fragment_shader_file, GLhandleARB *program_handle)
{
	const char *vertex_shader, *fragment_shader;
    GLhandleARB fs, vs;   // handles to objects
    GLint       vertCompiled, fragCompiled, linked;    // status values
	char string[256];

	/* first deal with the vertex shader */
	vertex_shader = ReadProgramFile(vertex_shader_file);
	fragment_shader = ReadProgramFile(fragment_shader_file);
	
	if(!vertex_shader || !fragment_shader)
	{
		sprintf(string, "One of the shader files (%s or %s) not found!", vertex_shader_file, fragment_shader_file);
		ReportError(string);
		return ERROR_PROGRAM_NOT_FOUND;
	}


    // Create a vertex shader object and a fragment shader object

    vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    // Load source code strings into shaders

    glShaderSourceARB(vs, 1, &vertex_shader, NULL);
    glShaderSourceARB(fs, 1, &fragment_shader, NULL);

    glCompileShaderARB(vs);
    glGetObjectParameterivARB(vs,
                GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    ReportGLSLLog(vs);

    glCompileShaderARB(fs);
    glGetObjectParameterivARB(fs,
                GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
    ReportGLSLLog(fs);

    if (!vertCompiled || !fragCompiled)
        return ERROR_IN_PROGRAM;

    *program_handle = glCreateProgramObjectARB();
    glAttachObjectARB(*program_handle, vs);
    glAttachObjectARB(*program_handle, fs);


    glLinkProgramARB(*program_handle);
    glGetObjectParameterivARB(*program_handle,
                GL_OBJECT_LINK_STATUS_ARB, &linked);
    ReportGLSLLog(*program_handle);

    if (!linked)
        return ERROR_IN_PROGRAM;

	free((void*)vertex_shader);
	free((void*)fragment_shader);

	return NO_ERROR_PROGRAM;

}

/*****************************************************************************
 * Function Name  : SendCoefficientsToProgram
 * Inputs		  : program, *Coeff
 * Returns		  : 
 * Description    : Loads the nine SH coefficients to the vertex/fragment 
					shader/program (as constants).
 *****************************************************************************/
void SendCoefficientsToProgram(GLenum program, SHCoeffStruct *Coeff)
{
	int i;
	int position;

	if(bSupportGLSL)
	{
		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[0]");
	    glUniform3fARB(position, Coeff[0].red, Coeff[0].green, Coeff[0].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[1]");
	    glUniform3fARB(position, Coeff[1].red, Coeff[1].green, Coeff[1].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[2]");
	    glUniform3fARB(position, Coeff[2].red, Coeff[2].green, Coeff[2].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[3]");
	    glUniform3fARB(position, Coeff[3].red, Coeff[3].green, Coeff[3].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[4]");
	    glUniform3fARB(position, Coeff[4].red, Coeff[4].green, Coeff[4].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[5]");
	    glUniform3fARB(position, Coeff[5].red, Coeff[5].green, Coeff[5].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[6]");
	    glUniform3fARB(position, Coeff[6].red, Coeff[6].green, Coeff[6].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[7]");
	    glUniform3fARB(position, Coeff[7].red, Coeff[7].green, Coeff[7].blue);

		position = glGetUniformLocationARB(sh_shader.program_handle, "Coefficients[8]");
	    glUniform3fARB(position, Coeff[8].red, Coeff[8].green, Coeff[8].blue);
	}
	else
	{
		for(i=0; i<9; i++)
			glProgramEnvParameter4fARB(program, i, Coeff[i].red, Coeff[i].green, Coeff[i].blue, 1);
	}
}

/*****************************************************************************
 * Function Name  : LoadPrograms
 * Inputs		  : 
 * Returns		  : 
 * Description    : Loads all the shaders and programs from disk to OpenGL.
 *****************************************************************************/
BOOL LoadPrograms()
{
	if(bSupportGLSL)
	{
		GLLoadShader("Data\\phong_lighting.vs", "Data\\phong_lighting.fs", &phong_shader.program_handle);
		GLLoadShader("Data\\sh_lighting.vs", "Data\\sh_lighting.fs", &sh_shader.program_handle);
	}
	else
	{
		if(GLLoadVertexProgram("Data\\phong_lighting_1light.vp", &phong_vertex.program_id) != NO_ERROR_PROGRAM)
			return FALSE;
		phong_vertex.program_type = GL_VERTEX_PROGRAM_ARB;

		if(GLLoadVertexProgram("Data\\sh_lighting.vp", &sh_vertex.program_id) != NO_ERROR_PROGRAM)
			return FALSE;
		sh_vertex.program_type = GL_VERTEX_PROGRAM_ARB;

		if(!bNoFragmentProgram)
		{
			if(GLLoadFragmentProgram("Data\\phong_lighting.fp", &phong_pixel.program_id) != NO_ERROR_PROGRAM)
				return FALSE;
			phong_pixel.program_type = GL_FRAGMENT_PROGRAM_ARB;

			if(GLLoadFragmentProgram("Data\\sh_lighting.fp", &sh_pixel.program_id) != NO_ERROR_PROGRAM)
				return FALSE;
			sh_pixel.program_type = GL_FRAGMENT_PROGRAM_ARB;
		}
	}
	return TRUE;
}

/*****************************************************************************
 * Function Name  : SetProgram
 * Inputs		  : 
 * Returns		  : 
 * Description    : Sets which program is currently active. Also sends the camera
					position to the loaded program.
 *****************************************************************************/
void SetProgram(ProgramType program)
{
	if(bSupportGLSL)
		glUseProgramObjectARB(0);

	if(program.program_id < 0)
	{
		return;
	}


	switch(program.program_type)
	{
	case GL_VERTEX_PROGRAM_ARB:
		glEnable(GL_VERTEX_PROGRAM_ARB);
		break;
	case GL_FRAGMENT_PROGRAM_ARB:
		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		break;
	}

	glBindProgramARB(program.program_type, program.program_id);
	/* send the camera position to the program */
	glProgramEnvParameter4fARB(program.program_type, 9, 
		Camera.position.x, Camera.position.y, Camera.position.z, 1);
	
}

/*****************************************************************************
 * Function Name  : SetShader
 * Inputs		  : 
 * Returns		  : 
 * Description    : Sets which shader is currently active. Also sends the camera
					position to the loaded shader.
 *****************************************************************************/
void SetShader(ProgramType shader)
{
	int position;

	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);

    glUseProgramObjectARB(shader.program_handle);

	/* send the camera position to the program */
	position = glGetUniformLocationARB(phong_shader.program_handle, "CameraPosition");
    glUniform4fARB(position, Camera.position.x, Camera.position.y, Camera.position.z, 1);

}

/*****************************************************************************
 * Function Name  : DisableProgramsAndShaders
 * Inputs		  : 
 * Returns		  : 
 * Description    : Disables all vertex/fragment programs/shaders.
 *****************************************************************************/
void DisableProgramsAndShaders()
{
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);

	if(bSupportGLSL)
	{
		glUseProgramObjectARB(0);
	}

}


/*****************************************************************************
 * Function Name  : square
 * Inputs		  : val
 * Returns		  : float
 * Description    : Squares the float incomming value
 *****************************************************************************/
float square(float val)
{
	return val * val;
}

/*****************************************************************************
 * Function Name  : Normalise
 * Inputs		  : *source
 * Returns		  : *destination
 * Description    : Normalises the incomming 4 component vector
 *****************************************************************************/
void Normalise(float *source, float *destination)
{
	float length;

	length = (float)sqrt(square(source[0]) + square(source[1]) + square(source[2]));

	destination[0] = source[0] / length;
	destination[1] = source[1] / length;
	destination[2] = source[2] / length;
	destination[3] = 1;
}

/*****************************************************************************
 * Function Name  : InitCoefficients
 * Inputs		  : 
 * Returns		  : 
 * Description    : Sets to 0 all the nine SH coefficients
 *****************************************************************************/
void InitCoefficients()
{
	int i;

	for(i=0; i<9; i++)
	{
		SHCoeff[i].red = 0;
		SHCoeff[i].green = 0;
		SHCoeff[i].blue = 0;
	}
}

/*****************************************************************************
 * Function Name  : LoadImage
 * Inputs		  : *filename
 * Returns		  : AUX_RGBImageRec *
 * Description    : Loads a bitmap image from disk
 *****************************************************************************/
AUX_RGBImageRec *LoadImage(char *filename)
{
	AUX_RGBImageRec *bitmap;

	if(!filename)									
		return 0;

	bitmap = auxDIBImageLoad(filename);	

	return bitmap;
}

/*****************************************************************************
 * Function Name  : LoadFile
 * Inputs		  : *title
 * Returns		  : char *
 * Description    : Displays the load-file dialog box and returns the path to 
					the file selected by the user
 *****************************************************************************/
BOOL LoadFile(char *title, char *filename)
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = gHWND;
	ofn.lpstrFile = szFile;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Bitmap\0*.BMP\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(title)
		ofn.lpstrTitle = title;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn)==TRUE) 
	{
		strcpy(filename, ofn.lpstrFile);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*****************************************************************************
 * Function Name  : LoadEnvMap
 * Inputs		  : 
 * Returns		  : BOOL
 * Description    : Loads an environment map from the disk
 *****************************************************************************/
BOOL LoadEnvMap()
{
	char filename[128];
	
	if(LoadFile(0, filename) == FALSE)
		return FALSE;

	EnvMap = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &envmap_texid);

	if(EnvMap)
		return TRUE;
	else
		return FALSE;
}

/*****************************************************************************
 * Function Name  : LoadCubeEnvMap
 * Inputs		  : 
 * Returns		  : BOOL
 * Description    : Loads an cube map (serie of 6 images) from the disk
 *****************************************************************************/
BOOL LoadCubeEnvMap()
{
	char filename[128];
	
	if(LoadFile("Cube Map Face X+", filename) == FALSE)
		return FALSE;
	CubeEnvMap[0] = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &cubeenvmap_texid[4]);

	if(LoadFile("Cube Map Face X-", filename) == FALSE)
		return FALSE;
	CubeEnvMap[1] = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &cubeenvmap_texid[5]);

	if(LoadFile("Cube Map Face Y+", filename) == FALSE)
		return FALSE;
	CubeEnvMap[2] = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &cubeenvmap_texid[0]);

	if(LoadFile("Cube Map Face Y-", filename) == FALSE)
		return FALSE;
	CubeEnvMap[3] = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &cubeenvmap_texid[1]);

	if(LoadFile("Cube Map Face Z+", filename) == FALSE)
		return FALSE;
	CubeEnvMap[4] = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &cubeenvmap_texid[2]);

	if(LoadFile("Cube Map Face Z-", filename) == FALSE)
		return FALSE;
	CubeEnvMap[5] = LoadImage(filename);
	OGLShellLoadBMP(filename, FALSE, TRUE, GL_RGB, &cubeenvmap_texid[3]);


	return TRUE;
}

/*****************************************************************************
 * Function Name  : DisplayHUD
 * Inputs		  : 
 * Returns		  : 
 * Description    : Displays different usefull text information on the screen
 *****************************************************************************/
void DisplayHUD()
{
	float frameRate;

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	DisableProgramsAndShaders();

	if(bDrawWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	OGLShellGetVariable(FRAME_RATE, &frameRate);

	OGLTPrint3D(0,0,0.5f,(DWORD)0xFFFFFFFF,"FPS: %.2f", frameRate);
	OGLTPrint3D(0,3,0.5f,(DWORD)0xFFFFFFFF,"POLYS: %d", number_of_balls * SphereDetail * SphereDetail);
	OGLTPrint3D(0,6,0.5f,(DWORD)0xFFFFFFFF,"Number Of Lights: %d", NumberOfLights);
	if(bEnableSHLightingReduction)
	{
		OGLTPrint3D(0,9,0.5f,(DWORD)0xFFFFFFFF,"SH Lighting");
		OGLTPrint3D(0,12,0.5f,(DWORD)0xFFFFFFFF,"Number Of Phong Lights: %d", NumberOfPhongLights);
	}
	else
	{
		OGLTPrint3D(0,9,0.5f,(DWORD)0xFFFFFFFF,"Phong Lighting");
	}

	if(bDrawWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

}

/*****************************************************************************
 * Function Name  : max3
 * Inputs		  : a, b, c
 * Returns		  : float
 * Description    : Calculates and returns the max value between 3 floating 
					point numbers
 *****************************************************************************/
float max3(float a, float b, float c)
{
	if(a > b)
	{
		if(a > c)
			return a;
		else
			return c;
	}
	else
	{
		if(b > c)
			return b;
		else
			return c;
	}
}

/*****************************************************************************
 * Function Name  : FindMaxInArray
 * Inputs		  : array[MAX_NUMBER_OF_LIGHTS]
 * Returns		  : int
 * Description    : Finds the id of the maximum value in a given array
 *****************************************************************************/
int FindMaxInArray(float array[MAX_NUMBER_OF_LIGHTS])
{
	float max_value = 0;
	int max_id = 0;
	int i;

	for(i=0; i<MAX_NUMBER_OF_LIGHTS; i++)
	{
		if(array[i] >= max_value)
		{
			max_value = array[i];
			max_id = i;
		}
	}

	array[max_id] = 0;

	return max_id;
}

/*****************************************************************************
 * Function Name  : KeyboardHandling
 * Inputs		  : 
 * Returns		  : 
 * Description    : Deals with immediate keyboard interraction
 *****************************************************************************/
void KeyboardHandling()
{
	#define KEYDOWN(name,key) (name[key] & 0x80)
	BYTE	Keys[256];	

	/* Get keyboard state */
	GetKeyboardState(Keys);

	if (KEYDOWN(Keys, VK_UP) )
	{
		CameraMoveFront();
	}
	if (KEYDOWN(Keys, VK_DOWN) )
	{
		CameraMoveBack();
	}
	if (KEYDOWN(Keys, VK_LEFT) )
	{
		CameraStrafeLeft();
	}
	if (KEYDOWN(Keys, VK_RIGHT) )
	{
		CameraStrafeRight();
	}
}