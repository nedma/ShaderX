#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>

/* the units used are centimeters */


#define BALL_DIAMETER	7
#define TABLE_LENGTH	400
#define TABLE_WIDTH		200

typedef struct _Ball_T
{
	float colorRed, colorGreen, colorBlue;
	GLuint texture_id;
	float radius;
	GLUquadric *sphere;
	float x, y, z;			/* the position of the sphere */
}BallType;

void Draw();
void InitialiseBallsTable();
