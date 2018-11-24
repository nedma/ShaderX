#include <windows.h>
#include <stdlib.h>
#include "glext.h"
#include "OGLShell.h"
#include "geometry.h"
#include "camera.h"
#include "light.h"
#include "util.h"

int number_of_balls = 80;
BallType Balls[80];
int SphereDetail = 25;
GLuint envmap_texid;
GLuint cubeenvmap_texid[6];
GLUquadric *envmap;

BOOL bDrawPlane = TRUE;
BOOL bDrawWhiteBalls = FALSE;
BOOL bDrawWireframe = FALSE;
BOOL bUseTextures = FALSE;
BOOL bDrawEnvMap = FALSE;

extern BOOL bCubeMapMode;
extern BOOL bEnableSHLightingReduction;
extern int NumberOfLights;
extern int NumberOfPhongLights;
extern LightStruct Lights[8];
extern LightingAlgorithmType PhongLightingAlgorithm;
extern LightingAlgorithmType SHLightingAlgorithm;
extern ProgramType phong_vertex, phong_pixel, sh_vertex, sh_pixel;
extern ProgramType phong_shader, sh_shader;
extern SHCoeffStruct SHCoeff[9];
extern BOOL bSupportGLSL;
extern BOOL bUseAnalyticalLights;

/*****************************************************************************
* Function Name  : PositionBall
* Inputs		  : which_ball
* Returns		  : 
* Description    : Positions the ball into space
*****************************************************************************/
void PositionBall(int which_ball)
{
	PositionCamera();
	glTranslatef(Balls[which_ball].x, Balls[which_ball].y, Balls[which_ball].z);
	glMatrixMode(GL_TEXTURE);
	glTranslatef(Balls[which_ball].x, Balls[which_ball].y, Balls[which_ball].z);

}

/*****************************************************************************
* Function Name  : DrawBall
* Inputs		  : which_ball
* Returns		  : 
* Description    : Draws the ball usign GLU functions
*****************************************************************************/
void DrawBall(int which_ball)
{
	PositionBall(which_ball);
	if(bDrawWhiteBalls)
	{
		glColor3f(1,1,1);
	}
	else
	{
		glColor3f(Balls[which_ball].colorRed, Balls[which_ball].colorGreen, Balls[which_ball].colorBlue);
	}

	if(bDrawWireframe)
	{
		gluQuadricDrawStyle(Balls[which_ball].sphere, GLU_LINE);
	}
	else
	{
		gluQuadricDrawStyle(Balls[which_ball].sphere, GLU_FILL);
	}

	if(bUseTextures)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Balls[which_ball].texture_id);
		gluQuadricTexture(Balls[which_ball].sphere, GL_TRUE);
	}
	else
	{
		gluQuadricTexture(Balls[which_ball].sphere, GL_FALSE);
	}
	gluSphere(Balls[which_ball].sphere, Balls[which_ball].radius, SphereDetail, SphereDetail);
	glDisable(GL_TEXTURE_2D);
}


/*****************************************************************************
* Function Name  : DrawScene
* Inputs		  : 
* Returns		  : 
* Description    : Function responsible for drawing all the balls into the scene
*****************************************************************************/
void DrawScene()
{
	int i;

	PositionCamera();
	glScalef(0.464f, 0.464f, 0.464f);

	if(bDrawWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	for(i=0; i<number_of_balls; i++)
	{
		DrawBall(i);
	}
}

/*****************************************************************************
* Function Name  : Draw
* Inputs		  : 
* Returns		  : 
* Description    : Function responsible for drawing the whole scene and set up
shaders accordingly. Multi pass is performed where required
*****************************************************************************/
void Draw()
{
	int i;
	BOOL bFirstPass = TRUE;

	glDisable(GL_CULL_FACE);

	if(bEnableSHLightingReduction)
	{
		if(!bSupportGLSL)
		{
			if(bUseAnalyticalLights)
			{
				/* first draw the scene using the phong lights */
				/* select where the calculation will go */
				switch(PhongLightingAlgorithm)
				{
				case PHONG_PER_PIXEL:
					SetProgram(phong_pixel);
					break;
				case PHONG_PER_VERTEX:
					SetProgram(phong_vertex);
					break;
				}

				/* 1 pass for each phong light */
				for(i=0; i<NumberOfLights; i++)
				{
					if(Lights[i].bDoPhong)
					{
						if(bFirstPass)
						{
							glDisable(GL_BLEND);
							glDepthFunc(GL_LEQUAL);
							glDepthMask(GL_TRUE);
							bFirstPass = FALSE;
						}
						else
						{
							glEnable(GL_BLEND);
							glDepthFunc(GL_EQUAL);
							glDepthMask(GL_FALSE);
						}
						SetUpLight(i);
						DrawScene();
					}
				}

				if(NumberOfLights != NumberOfPhongLights)
				{
					/* select where the SH calculation will happen */
					switch(SHLightingAlgorithm)
					{
					case SH_PER_PIXEL:
						SetProgram(sh_pixel);
						break;
					case SH_PER_VERTEX:
						SetProgram(sh_vertex);
						SendCoefficientsToProgram(sh_vertex.program_type, SHCoeff);
						break;
					}

					if(bFirstPass)
					{
						glDisable(GL_BLEND);
						glDepthFunc(GL_LEQUAL);
						glDepthMask(GL_TRUE);
						bFirstPass = FALSE;
					}
					else
					{
						glEnable(GL_BLEND);
						glDepthFunc(GL_EQUAL);
						glDepthMask(GL_FALSE);
					}

					DrawScene();
				}
			}
			else
			{
				/* select where the SH calculation will happen */
				switch(SHLightingAlgorithm)
				{
				case SH_PER_PIXEL:
					SetProgram(sh_pixel);
					break;
				case SH_PER_VERTEX:
					SetProgram(sh_vertex);
					SendCoefficientsToProgram(sh_vertex.program_type, SHCoeff);
					break;
				}
				glDisable(GL_BLEND);
				glDepthFunc(GL_LEQUAL);
				glDepthMask(GL_TRUE);
				bFirstPass = FALSE;
				DrawScene();
			}
		}
		else
		{
			if(bUseAnalyticalLights)
			{
				SetShader(phong_shader);
				for(i=0; i<NumberOfLights; i++)
				{
					if(Lights[i].bDoPhong)
					{
						if(bFirstPass)
						{
							glDisable(GL_BLEND);
							glDepthFunc(GL_LEQUAL);
							glDepthMask(GL_TRUE);
							bFirstPass = FALSE;
						}
						else
						{
							glEnable(GL_BLEND);
							glDepthFunc(GL_EQUAL);
							glDepthMask(GL_FALSE);
						}
						SetUpLight(i);
						DrawScene();
					}
				}
				if(NumberOfLights != NumberOfPhongLights)
				{
					SetShader(sh_shader);
					SendCoefficientsToProgram(sh_vertex.program_type, SHCoeff);
					if(bFirstPass)
					{
						glDisable(GL_BLEND);
						glDepthFunc(GL_LEQUAL);
						glDepthMask(GL_TRUE);
						bFirstPass = FALSE;
					}
					else
					{
						glEnable(GL_BLEND);
						glDepthFunc(GL_EQUAL);
						glDepthMask(GL_FALSE);
					}

					DrawScene();
				}
			}
			else
			{
				SetShader(sh_shader);
				SendCoefficientsToProgram(sh_vertex.program_type, SHCoeff);
				glDisable(GL_BLEND);
				glDepthFunc(GL_LEQUAL);
				glDepthMask(GL_TRUE);
				bFirstPass = FALSE;
				DrawScene();
			}
		}
	}
	else
	{
		/* the scene is drawn using normal lighting algorithms */
		if(!bSupportGLSL)
		{
			switch(PhongLightingAlgorithm)
			{
			case PHONG_PER_PIXEL:
				SetProgram(phong_pixel);
				break;
			case PHONG_PER_VERTEX:
				SetProgram(phong_vertex);
				break;
			}
			/* 1 pass for each light */
			for(i=0; i<NumberOfLights; i++)
			{
				if(bFirstPass)
				{
					glDisable(GL_BLEND);
					glDepthFunc(GL_LEQUAL);
					glDepthMask(GL_TRUE);
					bFirstPass = FALSE;
				}
				else
				{
					glEnable(GL_BLEND);
					glDepthFunc(GL_EQUAL);
					glDepthMask(GL_FALSE);
				}
				SetUpLight(i);
				DrawScene();
			}

		}
		else
		{
			SetShader(phong_shader);
			for(i=0; i<NumberOfLights; i++)
			{
				if(bFirstPass)
				{
					glDisable(GL_BLEND);
					glDepthFunc(GL_LEQUAL);
					glDepthMask(GL_TRUE);
					bFirstPass = FALSE;
				}
				else
				{
					glEnable(GL_BLEND);
					glDepthFunc(GL_EQUAL);
					glDepthMask(GL_FALSE);
				}
				SetUpLight(i);
				DrawScene();
			}

		}
	}

	PositionCamera();
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	DisableProgramsAndShaders();
	SendCoefficientsToProgram(sh_vertex.program_type, SHCoeff);

	if(bDrawEnvMap)
	{
		PositionCamera();
		glDisable(GL_VERTEX_PROGRAM_ARB);
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		if(bCubeMapMode)
		{
			/* draw a cube with the cube map as textures */
			/* draw X+ face */
			glBindTexture(GL_TEXTURE_2D, cubeenvmap_texid[0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(100, 100, -100);
			glTexCoord2f(1,0);
			glVertex3f(100, -100, -100);
			glTexCoord2f(1,1);
			glVertex3f(100, -100, 100);
			glTexCoord2f(0,1);
			glVertex3f(100, 100, 100);
			glEnd();
			/* draw X- face */
			glBindTexture(GL_TEXTURE_2D, cubeenvmap_texid[1]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-100, 100, -100);
			glTexCoord2f(1,0);
			glVertex3f(-100, -100, -100);
			glTexCoord2f(1,1);
			glVertex3f(-100, -100, 100);
			glTexCoord2f(0,1);
			glVertex3f(-100, 100, 100);
			glEnd();
			/* draw Y+ face */
			glBindTexture(GL_TEXTURE_2D, cubeenvmap_texid[2]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(100, 100, -100);
			glTexCoord2f(1,0);
			glVertex3f(-100, 100, -100);
			glTexCoord2f(1,1);
			glVertex3f(-100, 100, 100);
			glTexCoord2f(0,1);
			glVertex3f(100, 100, 100);
			glEnd();
			/* draw Y- face */
			glBindTexture(GL_TEXTURE_2D, cubeenvmap_texid[3]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(100, -100, -100);
			glTexCoord2f(1,0);
			glVertex3f(-100, -100, -100);
			glTexCoord2f(1,1);
			glVertex3f(-100, -100, 100);
			glTexCoord2f(0,1);
			glVertex3f(100, -100, 100);
			glEnd();
			/* draw Z+ face */
			glBindTexture(GL_TEXTURE_2D, cubeenvmap_texid[4]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-100, 100, 100);
			glTexCoord2f(1,0);
			glVertex3f(-100, -100, 100);
			glTexCoord2f(1,1);
			glVertex3f(100, -100, 100);
			glTexCoord2f(0,1);
			glVertex3f(100, 100, 100);
			glEnd();
			/* draw Z- face */
			glBindTexture(GL_TEXTURE_2D, cubeenvmap_texid[5]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-100, 100, -100);
			glTexCoord2f(1,0);
			glVertex3f(-100, -100, -100);
			glTexCoord2f(1,1);
			glVertex3f(100, -100, -100);
			glTexCoord2f(0,1);
			glVertex3f(100, 100, -100);
			glEnd();
		}
		else
		{
			/* draw a sphere */
			glBindTexture(GL_TEXTURE_2D, envmap_texid);
			gluSphere(envmap, 200, 100, 100);
		}
		glDisable(GL_TEXTURE_2D);
	}
}

/*****************************************************************************
* Function Name  : InitialiseBallsTable
* Inputs		  : 
* Returns		  : 
* Description    : Initialises the balls randomly in space
*****************************************************************************/
void InitialiseBallsTable()
{
	int i;
	GLuint texture_id;

	OGLShellLoadBMP("Data\\wood.bmp", FALSE, TRUE, GL_RGB, &texture_id);

	for(i=0; i<number_of_balls; i++)
	{
		Balls[i].sphere = gluNewQuadric();
		gluQuadricTexture(Balls[i].sphere, GL_TRUE);
		Balls[i].radius = BALL_DIAMETER/2.0f;

		Balls[i].x = (float)(rand() % 100 - 50);
		Balls[i].z = (float)(rand() % 100 - 50);
		Balls[i].y = (float)(rand() % 100 - 50);

		Balls[i].colorRed = rand() / 65535.0f + 0.2f;
		Balls[i].colorGreen = rand() / 65535.0f + 0.2f;
		Balls[i].colorBlue = rand() / 65535.0f + 0.2f;

		Balls[i].texture_id = texture_id;
	}

	envmap = gluNewQuadric();
	gluQuadricTexture(envmap, GL_TRUE);
}

