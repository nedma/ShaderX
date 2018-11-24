#include <math.h>
#include "glext.h"
#include "light.h"
#include "camera.h"
#include "util.h"

int NumberOfLights = 4; /* total number of lights */
int NumberOfPhongLights = 2; /* how many of the lights are phong lights */
LightStruct Lights[8] = {
						{{0.7f,0.3f,0.0f,1},{1,0,0,1},{50,10,50,1}, 2.5f, TRUE, TRUE, TRUE},
						{{0.5f,0.5f,0.5f,1},{0,1,0,1},{-50,50,70,1}, 3.5f, TRUE, TRUE, TRUE},
						{{0.5f,0.3f,0.7f,1},{0,0,1,1},{-50,30,-20,1}, 4.1f, TRUE, FALSE, TRUE},
						{{0.5f,0.3f,0.5f,1},{1,0,0,1},{50,50,-50,1}, 2.8f, TRUE, FALSE, TRUE},
						{{0.4f,0.3f,0.5f,1},{1,0,0,1},{50,10,50,1}, -1.3f, TRUE, FALSE, TRUE},
						{{0.5f,0.5f,0.3f,1},{0,1,0,1},{-50,10,50,1}, 2.7f, TRUE, FALSE, TRUE},
						{{0.5f,0.3f,0.3f,1},{0,0,1,1},{-50,10,-50,1}, -3.1f, TRUE, FALSE, TRUE},
						{{0.3f,0.5f,0.6f,1},{1,0,0,1},{50,10,-50,1}, 3.7f, TRUE, FALSE, TRUE},
						};

float lightOff[4] = {0,0,0,1};

extern BOOL bSupportGLSL;
extern ProgramType phong_shader;

/*****************************************************************************
 * Function Name  : SetupLights
 * Inputs		  : 
 * Returns		  : 
 * Description    : Sets up the lights
 *****************************************************************************/
void SetupLights()
{
	float ambient[4] = {0.3f, 0.3f, 0.3f, 1};
	int i;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	for(i=0; i<NumberOfLights; i++)
	{
		/* we need to normalise the light position, and store this value into light direction */
		Normalise(Lights[i].position, Lights[i].direction);

		glLightfv(GL_LIGHT0 + i, GL_POSITION, Lights[i].position);
		if(Lights[i].bEnabled)
		{
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, Lights[i].colour);
		}
		else
		{
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lightOff);
		}
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);

	}

}

/*****************************************************************************
 * Function Name  : DrawLights
 * Inputs		  : 
 * Returns		  : 
 * Description    : Draws the lights on the screen
 *****************************************************************************/
void DrawLights()
{
	int i;

	PositionCamera();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPointSize(8.0f);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	
	DisableProgramsAndShaders();

	for(i=0; i<NumberOfLights; i++)
	{
		if(Lights[i].bEnabled)
		{
			glColor3f(Lights[i].colour[0], Lights[i].colour[1], Lights[i].colour[2]);
			glBegin(GL_POINTS);
			glVertex3f(Lights[i].position[0], Lights[i].position[1], Lights[i].position[2]);
			glEnd();
		}
	}
}

/*****************************************************************************
 * Function Name  : EnableLight
 * Inputs		  : bEnable
 * Returns		  : 
 * Description    : Enables or disables one more light
 *****************************************************************************/
void EnableLight(BOOL bEnable)
{
	if(bEnable)
	{
		if(NumberOfLights < MAX_NUMBER_OF_LIGHTS)
			NumberOfLights++;
	}
	else
	{
		if(NumberOfLights > 1)
			NumberOfLights--;
	}

	if(NumberOfPhongLights > NumberOfLights)
		NumberOfPhongLights = NumberOfLights;
}

/*****************************************************************************
 * Function Name  : TurnPhongLight
 * Inputs		  : bOn
 * Returns		  : 
 * Description    : Turn a phong light on or off
 *****************************************************************************/
void TurnPhongLight(BOOL bOn)
{
	if(bOn)
	{
		if(NumberOfPhongLights < NumberOfLights)
			NumberOfPhongLights++;
	}
	else
	{
		if(NumberOfPhongLights > 0)
			NumberOfPhongLights--;
	}
}

/*****************************************************************************
 * Function Name  : RotateLight
 * Inputs		  : which
 * Returns		  : 
 * Description    : animate 1 light
 *****************************************************************************/
void RotateLight(int which)
{
	VectorType tv;
	float Angle = Lights[which].rotationSpeed * 3.1415f/180;
	tv.x=Lights[which].position[0]*(float)cos(-Angle)-Lights[which].position[2]*(float)sin(-Angle);
	tv.y=Lights[which].position[1];
	tv.z=Lights[which].position[0]*(float)sin(-Angle)+Lights[which].position[2]*(float)cos(-Angle);

	Lights[which].position[0] = tv.x;
	Lights[which].position[1] = tv.y;
	Lights[which].position[2] = tv.z;
}

/*****************************************************************************
 * Function Name  : RotateLights
 * Inputs		  : 
 * Returns		  : 
 * Description    : do 1 frame of animation for all lights that need to be animated
 *****************************************************************************/
void RotateLights()
{
	int i;

	for(i=0; i<NumberOfLights; i++)
	{
		if(Lights[i].bDoAnimate)
			RotateLight(i);
	}
	SetupLights();
}

/*****************************************************************************
 * Function Name  : SetUpLight
 * Inputs		  : which
 * Returns		  : 
 * Description    : Sets up information about the light so that the vertex 
					program can pick it up
 *****************************************************************************/
void SetUpLight(int which)
{
	if(bSupportGLSL)
	{
		float ambient[4] = {0.1f, 0.1f, 0.1f, 0.0f};
		int position1,position2, position3;
		position1 = glGetUniformLocationARB(phong_shader.program_handle, "LightPosition");
		position2 = glGetUniformLocationARB(phong_shader.program_handle, "LightColor");
		position3 = glGetUniformLocationARB(phong_shader.program_handle, "LightAmbient");

		glUniform3fARB(position1, Lights[which].position[0], Lights[which].position[1], Lights[which].position[2]);
	    glUniform3fARB(position2, Lights[which].colour[0], Lights[which].colour[1], Lights[which].colour[2]);
	    glUniform3fARB(position3, ambient[0], ambient[1], ambient[2]);
	}
	else
	{
		float ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, Lights[which].position);
		if(Lights[which].bEnabled)
		{
			glLightfv(GL_LIGHT0, GL_DIFFUSE, Lights[which].colour);
		}
		else
		{
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightOff);
		}
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	}
}

/*****************************************************************************
 * Function Name  : DetermineImportantLights
 * Inputs		  : 
 * Returns		  : 
 * Description    : this function looks at all the lights and determines for 
					which lights to do Phong model illumination based on the 
					Shaderx3 paper
 *****************************************************************************/
void DetermineImportantLights()
{
	int i;
	float Lf[MAX_NUMBER_OF_LIGHTS] = {0};

	if(NumberOfLights <= NumberOfPhongLights)
	{
		/* if the number of lights is equal or smaller than the number of
		enabled lights then do Phong lighting for all the enabled lights */
		for(i=0; i<NumberOfLights; i++)
		{
			Lights[i].bDoPhong = TRUE;
		}
		return;
	}
	/* for each of the lights enabled */
	for(i=0; i<NumberOfLights; i++)
	{
		float Drg, Drb, Dgb;
		float Cf, If;

		Drg = (float)fabs(Lights[i].colour[0] - Lights[i].colour[1]);
		Drb = (float)fabs(Lights[i].colour[0] - Lights[i].colour[2]);
		Dgb = (float)fabs(Lights[i].colour[1] - Lights[i].colour[2]);

		Cf = max3(Drg, Drb, Dgb);

		If = (float)sqrt(Lights[i].colour[0] * Lights[i].colour[0] + 
			Lights[i].colour[1] * Lights[i].colour[1] + 
			Lights[i].colour[2] * Lights[i].colour[2]);

		Lf[i] = C1 * Cf + C2 * If;
	}

	for(i=0; i<NumberOfLights; i++)
	{
		Lights[i].bDoPhong = FALSE;
	}

	for(i=0; i<NumberOfPhongLights; i++)
	{
		int id = FindMaxInArray(Lf);
		Lights[id].bDoPhong = TRUE;
	}

}