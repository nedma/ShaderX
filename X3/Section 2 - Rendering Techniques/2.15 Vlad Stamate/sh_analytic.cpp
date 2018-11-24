#include "util.h"
#include "light.h"

static float fConst1, fConst2, fConst3, fConst4, fConst5;

extern SHCoeffStruct SHCoeff[9];
extern int NumberOfLights;
extern LightStruct Lights[1];

/*****************************************************************************
 * Function Name  : SHAnalyticalInitConstants
 * Inputs		  : 
 * Returns		  : 
 * Description    : Initialises the constants 
 *****************************************************************************/
void SHAnalyticalInitConstants()
{
	float  fNormalisation = PI*16/17.0f;
	fConst1 = square(0.282095f) * fNormalisation * 1;
	fConst2 = square(0.488603f) * fNormalisation * (2/3.0f);
	fConst3 = square(1.092548f) * fNormalisation * (1/4.0f);
	fConst4 = square(0.315392f) * fNormalisation * (1/4.0f);
	fConst5 = square(0.546274f) * fNormalisation * (1/4.0f);
}


/*****************************************************************************
 * Function Name  : CalculateCoefficientsPerLight
 * Inputs		  : light
 * Returns		  : 
 * Description    : Given a light information will project it into SH 
 *****************************************************************************/
static void CalculateCoefficentsPerLight(LightStruct *light)
{
	SHCoeff[0].red += light->colour[0] * fConst1;
	SHCoeff[0].green += light->colour[1] * fConst1;
	SHCoeff[0].blue += light->colour[2] * fConst1;

	SHCoeff[1].red += light->colour[0] * fConst2 * light->direction[0];
	SHCoeff[1].green += light->colour[1] * fConst2 * light->direction[0];
	SHCoeff[1].blue += light->colour[2] * fConst2 * light->direction[0];

	SHCoeff[2].red += light->colour[0] * fConst2 * light->direction[1];
	SHCoeff[2].green += light->colour[1] * fConst2 * light->direction[1];
	SHCoeff[2].blue += light->colour[2] * fConst2 * light->direction[1];

	SHCoeff[3].red += light->colour[0] * fConst2 * light->direction[2];
	SHCoeff[3].green += light->colour[1] * fConst2 * light->direction[2];
	SHCoeff[3].blue += light->colour[2] * fConst2 * light->direction[2];

	SHCoeff[4].red += light->colour[0] * fConst3 * (light->direction[0] * light->direction[2]);
	SHCoeff[4].green += light->colour[1] * fConst3 * (light->direction[0] * light->direction[2]);
	SHCoeff[4].blue += light->colour[2] * fConst3 * (light->direction[0] * light->direction[2]);

	SHCoeff[5].red += light->colour[0] * fConst3 * (light->direction[2] * light->direction[1]);
	SHCoeff[5].green += light->colour[1] * fConst3 * (light->direction[2] * light->direction[1]);
	SHCoeff[5].blue += light->colour[2] * fConst3 * (light->direction[2] * light->direction[1]);

	SHCoeff[6].red += light->colour[0] * fConst3 * (light->direction[1] * light->direction[0]);
	SHCoeff[6].green += light->colour[1] * fConst3 * (light->direction[1] * light->direction[0]);
	SHCoeff[6].blue += light->colour[2] * fConst3 * (light->direction[1] * light->direction[0]);

	SHCoeff[7].red += light->colour[0] * fConst4 * (3.0f * light->direction[2] * light->direction[2] - 1.0f);
	SHCoeff[7].green += light->colour[1] * fConst4 * (3.0f * light->direction[2] * light->direction[2] - 1.0f);
	SHCoeff[7].blue += light->colour[2] * fConst4 * (3.0f * light->direction[2] * light->direction[2] - 1.0f);

	SHCoeff[8].red += light->colour[0] * fConst5 * (light->direction[0] * light->direction[0] - light->direction[1] * light->direction[1]);
	SHCoeff[8].green += light->colour[1] * fConst5 * (light->direction[0] * light->direction[0] - light->direction[1] * light->direction[1]);
	SHCoeff[8].blue += light->colour[2] * fConst5 * (light->direction[0] * light->direction[0] - light->direction[1] * light->direction[1]);

}

/*****************************************************************************
 * Function Name  : CalculateCoefficientsAnalytical
 * Inputs		  : 
 * Returns		  : 
 * Description    : This function will compute the SH coefficients for all the
					lights that need to have their coefficients computed
 *****************************************************************************/
void CalculateCoefficientsAnalytical()
{
	int i;

	InitCoefficients();

	for(i=0; i<NumberOfLights; i++)
	{
		if(Lights[i].bEnabled && !Lights[i].bDoPhong)
			CalculateCoefficentsPerLight(&Lights[i]);
	}
}
