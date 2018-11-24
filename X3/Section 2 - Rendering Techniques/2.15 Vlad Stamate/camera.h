
//motion constants
#define MOVE_FRONT		1
#define MOVE_BACK		2
#define STRAFE_LEFT		4
#define STRAFE_RIGHT	8
#define ROTATE_LEFT		16
#define ROTATE_RIGHT	32
#define LOOK_UP			64
#define LOOK_DOWN		128
#define BOB_UP			256
#define BOB_DOWN		512
#define FLY_UP			1024
#define FLY_DOWN		2048

#define MOVING_STEP		5.0f
#define ROTATION_STEP	2.0f

#define ANGLE			ROTATION_STEP*3.1415/180

#define MOUSE_SENSITIVITY 5

typedef struct _Vector_T
{
	float x,y,z;
}VectorType;

typedef struct _Camera_T
{
	float eyex, eyey, eyez;
	float centerx, centery, centerz;
	float upx, upy, upz;
	VectorType position;
	VectorType direction;
	float thetax,thetay,thetaz;
}PoolCameraType;

void PositionCamera();
void CameraMoveFront();
void CameraMoveBack();
void CameraMoveUp();
void CameraMoveDown();
void CameraStrafeLeft();
void CameraStrafeRight();
void CameraRotateVertical(int ammount);
void CameraRotateHorizontal(int ammount);
void RotateAroundCenter();