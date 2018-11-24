#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include <math.h>
#include "camera.h"

#define DISTANCE 5	/* how much will the camera move back and front */

PoolCameraType Camera = {0,40,-100, /* eye position */
						0,0,0,		/* center */
						0,1,0,		/* UP vector */
						{0,40,100},	/* position */
						{0,0,1},	/* direction */
						-30,0,0};	/* angles */
int movement = 0;

VectorType CameraModifyPos(int dir);

void PositionCamera()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef( -Camera.thetax, 1.0f, 0.0f, 0.0f );
	glRotatef( -Camera.thetay, 0.0f, 1.0f, 0.0f );
	glRotatef( -Camera.thetaz, 0.0f, 0.0f, 1.0f );
	glTranslatef( -Camera.position.x, -Camera.position.y, -Camera.position.z );
}

void RotateAroundCenter()
{
	VectorType tv;

	tv.x=Camera.position.x*(float)cos(-ANGLE)-Camera.position.z*(float)sin(-ANGLE);
	tv.y=Camera.position.y;
	tv.z=Camera.position.x*(float)sin(-ANGLE)+Camera.position.z*(float)cos(-ANGLE);
	Camera.position = tv;
	Camera.thetay+=ROTATION_STEP;

	tv.x=Camera.direction.x*(float)cos(-ANGLE)-Camera.direction.z*(float)sin(-ANGLE);
	tv.y=Camera.direction.y;
	tv.z=Camera.direction.x*(float)sin(-ANGLE)+Camera.direction.z*(float)cos(-ANGLE);
	Camera.direction=tv;
}

void CameraMoveFront()
{
	Camera.position = CameraModifyPos(MOVE_FRONT);
	movement = MOVE_FRONT;
}

void CameraMoveBack()
{
	Camera.position = CameraModifyPos(MOVE_BACK);
	movement = MOVE_BACK;
}

void CameraMoveUp()
{
	Camera.position = CameraModifyPos(FLY_UP);
	movement = FLY_UP;
}

void CameraMoveDown()
{
	Camera.position = CameraModifyPos(FLY_DOWN);
	movement = FLY_DOWN;
}

void CameraStrafeLeft()
{
	Camera.position = CameraModifyPos(STRAFE_LEFT);
	movement = STRAFE_LEFT;
}

void CameraStrafeRight()
{
	Camera.position = CameraModifyPos(STRAFE_RIGHT);
	movement = STRAFE_RIGHT;
}

void CameraRotateVertical(int ammount)
{
	if(ammount > 0)
	{
		Camera.position = CameraModifyPos(LOOK_UP);
		movement = LOOK_UP;
	}
	else
	{
		Camera.position = CameraModifyPos(LOOK_DOWN);
		movement = LOOK_DOWN;
	}

}

void CameraRotateHorizontal(int ammount)
{
	if(ammount > 0)
	{
		Camera.position = CameraModifyPos(ROTATE_LEFT);
		movement = ROTATE_LEFT;
	}
	else
	{
		Camera.position = CameraModifyPos(ROTATE_RIGHT);
		movement = ROTATE_RIGHT;
	}
}

/*****************************************************************************
 * Function Name  : CameraModifyPos
 * Inputs		  : dir
 * Returns		  : VectorType
 * Description    : Main camera movememtn funtion, given a type of movement 
					this function will compute the next position and orientation
					of the camera
 *****************************************************************************/
VectorType CameraModifyPos(int dir)
{
	VectorType tv,tv1;
	VectorType _position = Camera.position;
	float movingStep=MOVING_STEP;
	
	switch(dir)
	{
		case MOVE_FRONT:
			_position.x-=movingStep*Camera.direction.x;
			_position.y-=movingStep*Camera.direction.y;
			_position.z-=movingStep*Camera.direction.z;
			break;
		case MOVE_BACK:
			_position.x+=movingStep*Camera.direction.x;
			_position.y+=movingStep*Camera.direction.y;
			_position.z+=movingStep*Camera.direction.z;
			break;
		case ROTATE_LEFT:
			tv.x=Camera.direction.x*(float)cos(ANGLE)-Camera.direction.z*(float)sin(ANGLE);
			tv.y=Camera.direction.y;
			tv.z=Camera.direction.x*(float)sin(ANGLE)+Camera.direction.z*(float)cos(ANGLE);
			Camera.direction=tv;
			Camera.thetay-=ROTATION_STEP;
			break;
		case ROTATE_RIGHT:
			tv.x=Camera.direction.x*(float)cos(-ANGLE)-Camera.direction.z*(float)sin(-ANGLE);
			tv.y=Camera.direction.y;
			tv.z=Camera.direction.x*(float)sin(-ANGLE)+Camera.direction.z*(float)cos(-ANGLE);
			Camera.direction=tv;
			Camera.thetay+=ROTATION_STEP;
			break;
		case LOOK_UP:
			tv.x=Camera.direction.x;
			tv.y=Camera.direction.y*(float)cos(-ANGLE)-Camera.direction.z*(float)sin(-ANGLE);
			tv.z=Camera.direction.y*(float)sin(-ANGLE)+Camera.direction.z*(float)cos(-ANGLE);
			Camera.direction=tv;
			Camera.thetax-=ROTATION_STEP;
			break;
		case LOOK_DOWN:
			tv.x=Camera.direction.x;
			tv.y=Camera.direction.y*(float)cos(ANGLE)-Camera.direction.z*(float)sin(ANGLE);
			tv.z=Camera.direction.y*(float)sin(ANGLE)+Camera.direction.z*(float)cos(ANGLE);
			Camera.direction=tv;
			Camera.thetax+=ROTATION_STEP;
			break;
		case STRAFE_RIGHT:
			tv1=Camera.direction;
			tv=Camera.direction;
			tv.x=tv1.x*(float)cos(90*3.1415/180)-tv1.z*(float)sin(90*3.1415/180);
			tv.y=0;
			tv.z=tv1.x*(float)sin(90*3.1415/180)+tv1.z*(float)cos(90*3.1415/180);
			_position.x-=movingStep*tv.x;
			_position.y-=movingStep*tv.y;
			_position.z-=movingStep*tv.z;
			break;
		case STRAFE_LEFT:
			tv1=Camera.direction;
			tv=Camera.direction;
			tv.x=tv1.x*(float)cos(-90*3.1415/180)-tv1.z*(float)sin(-90*3.1415/180);
			tv.y=0/*tv1.y*/;
			tv.z=tv1.x*(float)sin(-90*3.1415/180)+tv1.z*(float)cos(-90*3.1415/180);
			_position.x-=movingStep*tv.x;
			_position.y-=movingStep*tv.y;
			_position.z-=movingStep*tv.z;
			break;
		case FLY_UP:
			_position.y-=movingStep;
			break;
		case FLY_DOWN:
			_position.y+=movingStep;
			break;
	}
	return _position;
};