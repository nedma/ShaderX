#ifndef _LIGHT_H_
#define _LIGHT_H_

#define MAX_NUMBER_OF_LIGHTS 8

#define C1	0.7f
#define C2  0.3f

typedef struct Light_Rec
{
	float colour[4];
	float direction[4];
	float position[4];
	float rotationSpeed;
	BOOL bEnabled;
	BOOL bDoPhong;
	BOOL bDoAnimate;
}LightStruct;

typedef struct SHCoeff_Rec
{
	float red, green, blue;

}SHCoeffStruct;

void DrawLights();
void SetupLights();
void EnableLight(BOOL bEnable);
void RotateLights();
void SetUpLight(int which);
void DetermineImportantLights();
void TurnPhongLight(BOOL bOn);

#endif