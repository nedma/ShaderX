#include <math.h>
#include <stdio.h>
#include <windows.h>
#include "glext.h"
#include "oglshell.h"
#include "util.h"
#include "light.h"

extern float fConst1, fConst2, fConst3, fConst4, fConst5;
extern SHCoeffStruct SHCoeff[9];
extern AUX_RGBImageRec *EnvMap;
extern SHCoeffStruct SHCoeff[9];
extern int NumberOfLights;
extern LightStruct Lights[1];
extern int SizeOf2DEnvMap;

#define USE_OLD_COORDINATE_TRANSFORM 1

/*****************************************************************************
 * Function Name  : SHEnvInitConstants
 * Inputs		  : 
 * Returns		  : 
 * Description    : Initialises the constants 
 *****************************************************************************/
void SHEnvInitConstants()
{
	float  fNormalisation = PI*16/17.0f;
	fConst1 = square(0.282095f) * fNormalisation * 1;
	fConst2 = square(0.488603f) * fNormalisation * (2/3.0f);
	fConst3 = square(1.092548f) * fNormalisation * (1/4.0f);
	fConst4 = square(0.315392f) * fNormalisation * (1/4.0f);
	fConst5 = square(0.546274f) * fNormalisation * (1/4.0f);
}

/*****************************************************************************
 * Function Name  : Sample
 * Inputs		  : x, y, which, bitmap
 * Returns		  : color (float value)
 * Description    : Given a coordinate and a color component this function
					will sample from the bitmap and return a float value 
					representing the required channel (R, G or B)
 *****************************************************************************/
float Sample(int x, int y, int which, AUX_RGBImageRec *bitmap)
{
	float fvalue = 0;
	int ivalue = 0;
	ivalue = bitmap->data[x * 3 + which + y * bitmap->sizeX * 3];
 	fvalue = ivalue / 255.0f;
	return fvalue;
}

/*****************************************************************************
 * Function Name  : CalculateL
 * Inputs		  : l, m
 * Returns		  : SHCoeffStruct
 * Description    : Given the degree of the SH this function will calculate
					an SH coefficient from a environment map. The input arguments
					l and m decide which coeficient to calculate
 *****************************************************************************/
static SHCoeffStruct CalculateL(int l, int m)
{
	float xp,yp; /* xplane and yplane are coordinates of the 2D environment map */
	int i, j;
	float theta, phi; /* spherical coordiantes */
	float x, y, z;		/* cartesian coordinates */
	SHCoeffStruct integral = {0,0,0}; /* 3 because we have r g and b */
	float Ylm;

	for(i=0; i<SizeOf2DEnvMap; i++)
	{
		for(j=0; j<SizeOf2DEnvMap; j++)
		{
#if USE_OLD_COORDINATE_TRANSFORM
			/* build 2D coordinates */
			xp = (float)i/(float)SizeOf2DEnvMap;
			yp = (float)j/(float)SizeOf2DEnvMap;

			/* build spherical coordinates out of 2D coordinates */
			theta = 2.0f * (float)acos(sqrt(1.0f - xp));
			phi = 2.0f * PI * yp;

			/* build the 3D coordiantes out of spherical coordinates */
			x = (float)(sin(theta)*cos(phi));
			y = (float)(sin(theta)*sin(phi));
			z = (float)cos(theta);
#else
			GLfloat matrix[16];
			GLfloat vector[4] = {0,0,-1,1};
			GLfloat vector1[4], vector2[4];

			/* build 2D coordinates */
			xp = 2 * (float)i/(float)SizeOf2DEnvMap - 1;
			yp = 2 * (float)j/(float)SizeOf2DEnvMap - 1;

			theta = atan2(yp, xp);
			phi=PI * sqrt(xp*xp + yp*yp);

			OGLTMatrixRotateY(matrix, phi);
			OGLTVectorMult(vector, vector1, matrix);
			OGLTMatrixRotateZ(matrix, theta);
			OGLTVectorMult(vector1, vector2, matrix);

			x = vector2[0];
			y = vector2[1];
			z = vector2[2];
#endif
			/* now compute Ylm */
			if((l==0)&&(m==0))
			{
				Ylm = fConst1;
			}

			if(l==1)
			{
				if(m==-1)
				{
					Ylm = fConst2 * y;
				}
				if(m==0)
				{
					Ylm = fConst2 * z;
				}
				if(m==1)
				{
					Ylm = fConst2 * x;
				}
			}
			if(l==2)
			{
				if(m==1)
				{
					Ylm = fConst3 * x * z;
				}
				if(m==-1)
				{
					Ylm = fConst3 * y * z;
				}
				if(m==-2)
				{
					Ylm = fConst3 * x * y;
				}
				if(m==0)
				{
					Ylm = fConst4 * ( 3 * z * z - 1);
				}
				if(m==2)
				{
					Ylm = fConst5 * ( x * x - y * y );
				}
			}

			/* now we integrate across the whole map */

			integral.red += Sample(i, j, 0, EnvMap) * Ylm;
			integral.green += Sample(i, j, 1, EnvMap) * Ylm;
			integral.blue += Sample(i, j, 2, EnvMap) * Ylm;
		}
	}

	/* normalise the integral */
	integral.red = PI * integral.red / ( SizeOf2DEnvMap * SizeOf2DEnvMap);
	integral.green = PI * integral.green / ( SizeOf2DEnvMap * SizeOf2DEnvMap);
	integral.blue = PI * integral.blue / ( SizeOf2DEnvMap * SizeOf2DEnvMap);

	return integral;
}

/*****************************************************************************
 * Function Name  : CalculateCoefficientsEnv
 * Inputs		  : 
 * Returns		  : 
 * Description    : Given a environment map will project it into SH 
 *****************************************************************************/
void CalculateCoefficientsEnv()
{
	int l, m;
	int index;
	char string[128];

	InitCoefficients();

	SizeOf2DEnvMap = EnvMap->sizeX;

	for(l=0; l<3; ++l) 
	{
		for(m=-l; m<=l; ++m) 
		{
			index = l*(l+1)+m;
			SHCoeff[index] = CalculateL(l, m); 
			sprintf(string, "L%d%d = %f %f %f\n", l, m, SHCoeff[index].red, SHCoeff[index].green, SHCoeff[index].blue);
			OutputDebugString(string);
		}
	}

}