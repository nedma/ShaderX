#include <gl\glaux.h>
#include "glext.h"
#include "light.h"

#define PI 3.1415f

typedef struct
{
	GLuint program_id;			/* for ARB vertex/fragment program usage */
	GLenum program_type;		/* for ARB vertex/fragment program usage */
	GLhandleARB program_handle; /* for GLSL usage */
}ProgramType;

typedef enum
{
	NO_ERROR_PROGRAM		= 1,
	ERROR_PROGRAM_NOT_FOUND = -1,
	ERROR_IN_PROGRAM		= -2
}Error;

/* algorithm choice */
typedef enum
{
	PHONG_PER_PIXEL,
	PHONG_PER_VERTEX,
	SH_PER_PIXEL,
	SH_PER_VERTEX
}LightingAlgorithmType;

void SendCoefficientsToProgram(GLenum program, SHCoeffStruct *Coeff);
void SetProgram(ProgramType program);
void SetShader(ProgramType shader);
BOOL LoadPrograms();
void Normalise(float source[4], float destination[4]);
float square(float val);
void InitCoefficients();
AUX_RGBImageRec *LoadImage(char *filename);
BOOL LoadEnvMap();
BOOL LoadCubeEnvMap();
void DisplayHUD();
float max3(float a, float b, float c);
int FindMaxInArray(float array[MAX_NUMBER_OF_LIGHTS]);
void DisableProgramsAndShaders();
void KeyboardHandling();
