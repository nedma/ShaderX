#include <math.h>
#include <stdio.h>
#include <windows.h>
#include "glext.h"
#include "oglshell.h"
#include "util.h"
#include "light.h"
#include "sh_env.h"

extern float fConst1, fConst2, fConst3, fConst4, fConst5;
extern SHCoeffStruct SHCoeff[9];
extern AUX_RGBImageRec *EnvMap;
extern AUX_RGBImageRec *CubeEnvMap[6];
extern SHCoeffStruct SHCoeff[9];
extern int NumberOfLights;
extern LightStruct Lights[1];
extern int SizeOf2DEnvMap;


/*****************************************************************************
 * Function Name  : SampleCubeMap
 * Inputs		  : x, y, which, bitmap
 * Returns		  : color (float value)
 * Description    : Given a coordinate and a color component this function
					will sample from the bitmap and return a float value 
					representing the required channel (R, G or B)
 *****************************************************************************/
static float SampleCubeMap(int x, int y, int which, AUX_RGBImageRec *bitmap)
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
					an SH coefficient from a cube map. The input arguments
					l and m decide which coeficient to calculate
 *****************************************************************************/
static SHCoeffStruct CalculateL(int l, int m)
{
	float xp,yp; /* xplane and yplane are coordinates of the 2D environment map */
	int i, j, faceid;
	float x, y, z;		/* cartesian coordinates */
	SHCoeffStruct integral = {0,0,0}; /* 3 because we have r g and b */
	float Ylm;
	float size;

	for(faceid=0; faceid<6; faceid++)
	{
		for(i=0; i<SizeOf2DEnvMap; i++)
		{
			for(j=0; j<SizeOf2DEnvMap; j++)
			{

				/* build 2D cube map coordinates */
				xp = ((float)i - (float)SizeOf2DEnvMap/2.0f)/(float)SizeOf2DEnvMap;
				yp = ((float)j - (float)SizeOf2DEnvMap/2.0f)/(float)SizeOf2DEnvMap;


				/* build the 3D coordiantes out of 2D cube map coordinates */
				switch(faceid)
				{
				case 0: /* X+ */
					x = 1;
					y = yp;
					z = xp;
					break;
				case 1: /* X- */
					x = -1;
					y = yp;
					z = xp;
					break;
				case 2: /* Y+ */
					x = xp;
					y = 1;
					z = yp;
					break;
				case 3: /* Y- */
					x = xp;
					y =-1;
					z = yp;
					break;
				case 4: /* Z+ */
					x = xp;
					y = yp;
					z = 1;
					break;
				case 5: /* Z- */
					x = xp;
					y = yp;
					z = -1;
					break;
				}

				/* normalize the vector */
				size = (float)sqrt(x * x + y * y + z * z);
				x = x / size;
				y = y / size;
				z = z / size;

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

				integral.red += Sample(i, j, 0, CubeEnvMap[faceid]) * Ylm;
				integral.green += Sample(i, j, 1, CubeEnvMap[faceid]) * Ylm;
				integral.blue += Sample(i, j, 2, CubeEnvMap[faceid]) * Ylm;
			}
		}
	}
	/* normalise the integral */
	integral.red = PI * integral.red / ( SizeOf2DEnvMap * SizeOf2DEnvMap * 6);
	integral.green = PI * integral.green / ( SizeOf2DEnvMap * SizeOf2DEnvMap * 6);
	integral.blue = PI * integral.blue / ( SizeOf2DEnvMap * SizeOf2DEnvMap * 6);

	return integral;
}

/*****************************************************************************
 * Function Name  : CalculateCoefficientsCubeEnv
 * Inputs		  : 
 * Returns		  : 
 * Description    : Given a cube-map will project it into SH 
 *****************************************************************************/
void CalculateCoefficientsCubeEnv()
{
	int l, m;
	int index;
	char string[128];

	InitCoefficients();

	SizeOf2DEnvMap = CubeEnvMap[0]->sizeX;

	for(l=0; l<3; l++) 
	{
		for(m=-l; m<=l; m++) 
		{
			index = l*(l+1)+m;
			SHCoeff[index] = CalculateL(l, m); 
			sprintf(string, "L%d%d = %f %f %f\n", l, m, SHCoeff[index].red, SHCoeff[index].green, SHCoeff[index].blue);
			OutputDebugString(string);
		}
	}
}