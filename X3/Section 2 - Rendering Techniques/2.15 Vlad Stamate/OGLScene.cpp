/*****************************************************************************
  Name : OGLScene.c
  Date : January 2001
  
  Copyright : 1999 by Imagination Technologies. All rights reserved.
******************************************************************************/
/*************
** Includes **
*************/
#ifdef _DEBUG
//#include <crtdbg.h>
#endif
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "OGLShell.h"	/* OGLShell for header */
#include "OGLTPrint3D.h"
#include "resource.h"
#include "camera.h"
#include "geometry.h"
#include "glext.h"
#include "util.h"
#include "light.h"
#include "dialogs.h"
#include "sh_analytical.h"
#include "sh_env.h"
#include "sh_cubeenv.h"

float winAspectRatio;
int mousePosX = -1;
int mousePosY = -1;
BOOL rotateMouse = FALSE;
BOOL rotateAroundCenter = FALSE;
HMENU hMenu;
HWND gHWND = NULL;
HINSTANCE gHINSTANCE;
SHCoeffStruct SHCoeff[9];
int SizeOf2DEnvMap;
BOOL bUseAnalyticalLights = TRUE;
AUX_RGBImageRec *EnvMap;
AUX_RGBImageRec *CubeEnvMap[6];
float fConst1, fConst2, fConst3, fConst4, fConst5;
BOOL bCubeMapMode = TRUE;		/* if we are having surface ligths (env maps) have we loaded
								sphere projected env maps or cube maps? */
BOOL bEnableSHLightingReduction = FALSE;
BOOL bSupportGLSLQuestionAsked = FALSE;
BOOL bSupportGLSLAnswer = FALSE;
BOOL bEnvMapLoaded = FALSE;


LightingAlgorithmType PhongLightingAlgorithm = PHONG_PER_VERTEX;
LightingAlgorithmType SHLightingAlgorithm = SH_PER_VERTEX;

extern BallType Balls[1];
extern LightStruct Lights[3];
extern ProgramType phong_vertex, phong_pixel, sh_vertex, sh_pixel;
extern ProgramType phong_shader;
extern BOOL bNoFragmentProgram;
extern BOOL bDrawPlane;
extern BOOL bDrawWhiteBalls;
extern BOOL bDrawWireframe;
extern BOOL bUseTextures;
extern BOOL bDrawEnvMap;
extern int SphereDetail;
extern BOOL bSupportGLSL;

BOOL InitialiseOpenGL()
{
	glClearColor(0,0,0,0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,winAspectRatio, 0.1f, 1000);

	PositionCamera();

	glEnable(GL_DEPTH_TEST);
	
	glBlendFunc(GL_ONE, GL_ONE);
//	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	glEnable(GL_BLEND);

	return InitExtensions();
}

/****************************************************************************
** InitApplication() is called by OGLShell to enable user to initialise	   **
** his/her application													   **
****************************************************************************/
void InitApplication(HINSTANCE hInstance, char *pszCommandLine)
{
	gHINSTANCE = hInstance;

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	/* You can use OGLShellSetPreferences(...) to set preferences for your application. 
	   You can specify a name, pass a menu, an accelerator table, an application icon 
	   and specify default rendering variables that will be used with your program :*/

	OGLShellSetPreferences("Spherical Harmonics Algorithms", // Title that will appear in window
							hMenu,		// Menu handle (if required)
							LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1)),	// Accelerator table handle (if required)
							LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)),			// Icon handle (if required)
							DEFAULT_CLEAR_ON	// List of rendering flags (if required)
							);	

}


/****************************************************************************
** QuitApplication() is called by OGLShell to enable user to release      **
** any memory before quitting the program.								   **
****************************************************************************/
void QuitApplication()
{
	/* Release any memory or instances allocated in InitApplication() */
}


/*******************************************************************************
** UserWindowProc(...) is the application's window messages handler.
** From that function you can process menu inputs, keystrokes, timers, etc...
** When processing keystrokes, DO NOT process ESCAPE key 
** (VK_ESCAPE), as it is already used by OGLShell.
********************************************************************************/
void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/* You can process normal window messages from that function (WM_COMMAND
	   to retrieve menu inputs, WM_KEYDOWN to retrieve keystrokes, etc...) */

	int xPos, yPos;

	switch(message)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'W':
		case 'w':
			CameraMoveFront();
			break;
		case 'S':
		case 's':
			CameraMoveBack();
			break;
		case 'Q':
		case 'q':
			CameraMoveUp();
			break;
		case 'E':
		case 'e':
			CameraMoveDown();
			break;
		case 'A':
		case 'a':
			CameraStrafeLeft();
			break;
		case 'D':
		case 'd':
			CameraStrafeRight();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		/* enter move mode */
		xPos = LOWORD(lParam); 
		yPos = HIWORD(lParam); 
		mousePosX = xPos;
		mousePosY = yPos;
		rotateMouse = TRUE;
		break;
	case WM_LBUTTONUP:
		rotateMouse = FALSE;
		break;
	case WM_MOUSEMOVE:
		if(rotateMouse)
		{
			xPos = LOWORD(lParam); 
			yPos = HIWORD(lParam); 
			if(abs(yPos - mousePosY) > MOUSE_SENSITIVITY)
			{
				CameraRotateVertical(yPos - mousePosY);
				mousePosY = yPos;
			}
			if(abs(xPos - mousePosX) > MOUSE_SENSITIVITY)
			{
				CameraRotateHorizontal(xPos - mousePosX);
				mousePosX = xPos;
			}
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_LIGHT_ADDLIGHT:
			EnableLight(TRUE);
			SetupLights();
			if(bUseAnalyticalLights)
				CalculateCoefficientsAnalytical();
			break;
		case ID_LIGHT_REMOVELIGHT:
			EnableLight(FALSE);
			SetupLights();
			if(bUseAnalyticalLights)
				CalculateCoefficientsAnalytical();
			break;
		case ID_LIGHTS_INCREASEPHONGLIGHTS:
			TurnPhongLight(TRUE);
			break;
		case ID_LIGHTS_DECREASEPHONGLIGHTS:
			TurnPhongLight(FALSE);
			break;
		case ID_LIGHT_PHONGLIGHTS:
			DialogBox(gHINSTANCE, MAKEINTRESOURCE(IDD_DIALOG1), gHWND, DlgProcPhongLights);
			break;
		case ID_LIGHTS_DRAWENVMAP:
			if(bDrawEnvMap)
			{
				CheckMenuItem(hMenu, ID_LIGHTS_DRAWENVMAP, MF_UNCHECKED);
				bDrawEnvMap = !bDrawEnvMap;
			}
			else
			{
				if(bEnvMapLoaded)
				{
					CheckMenuItem(hMenu, ID_LIGHTS_DRAWENVMAP, MF_CHECKED);
					bDrawEnvMap = !bDrawEnvMap;
				}
				else
				{
					MessageBox(gHWND, "Environment map not loaded. Please load an environment map first", "Draw Env Map Error", MB_OK);
				}
			}
			break;
		case ID_ALGORITHM_ANALYTICALLIGHTS:
			CheckMenuItem(hMenu, ID_ALGORITHM_ANALYTICALLIGHTS, MF_CHECKED);
			CheckMenuItem(hMenu, ID_ENVIRONMENTMAP_HDR, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_ENVIRONMENTMAP_RGB, MF_UNCHECKED);
			bUseAnalyticalLights = TRUE;
			CalculateCoefficientsAnalytical();
			break;
		case ID_ENVIRONMENTMAP_RGB:
			if(LoadEnvMap())
			{
				bCubeMapMode = FALSE;
				CheckMenuItem(hMenu, ID_ALGORITHM_ANALYTICALLIGHTS, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ENVIRONMENTMAP_HDR, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ENVIRONMENTMAP_RGB, MF_CHECKED);
				bUseAnalyticalLights = FALSE;
				CalculateCoefficientsEnv();
				bEnvMapLoaded = TRUE;
			}
			else
			{
				bEnvMapLoaded = FALSE;
			}
			break;
		case ID_ALGORITHM_CUBEENVIRONMENTMAP:
			if(LoadCubeEnvMap())
			{
				bCubeMapMode = TRUE;
				CheckMenuItem(hMenu, ID_ALGORITHM_ANALYTICALLIGHTS, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ENVIRONMENTMAP_HDR, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ENVIRONMENTMAP_RGB, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ALGORITHM_CUBEENVIRONMENTMAP, MF_CHECKED);
				bUseAnalyticalLights = FALSE;
				CalculateCoefficientsCubeEnv();
				bEnvMapLoaded = TRUE;
			}
			else
			{
				bEnvMapLoaded = FALSE;
			}
			break;
		case ID_ALGORITHM_SHPERVERTEX:
			SHLightingAlgorithm = SH_PER_VERTEX;
			CheckMenuItem(hMenu, ID_ALGORITHM_SHPERVERTEX, MF_CHECKED);
			CheckMenuItem(hMenu, ID_ALGORITHM_SHPERPIXEL, MF_UNCHECKED);
			if(bUseAnalyticalLights)
				CalculateCoefficientsAnalytical();
			else
				if(bCubeMapMode)
				{
					CalculateCoefficientsCubeEnv();
				}
				else
				{
					CalculateCoefficientsEnv();
				}
			break;
		case ID_ALGORITHM_SHPERPIXEL:
			if(!bNoFragmentProgram)
			{
				SHLightingAlgorithm = SH_PER_PIXEL;
				CheckMenuItem(hMenu, ID_ALGORITHM_SHPERVERTEX, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ALGORITHM_SHPERPIXEL, MF_CHECKED);
				if(bUseAnalyticalLights)
					CalculateCoefficientsAnalytical();
				else
					if(bCubeMapMode)
					{
						CalculateCoefficientsCubeEnv();
					}
					else
					{
						CalculateCoefficientsEnv();
					}
			}
			break;
		case ID_ALGORITHM_PHONGPERVERTEX:
			PhongLightingAlgorithm = PHONG_PER_VERTEX;
			CheckMenuItem(hMenu, ID_ALGORITHM_PHONGPERVERTEX, MF_CHECKED);
			CheckMenuItem(hMenu, ID_ALGORITHM_PHONGPERPIXEL, MF_UNCHECKED);
			break;
		case ID_ALGORITHM_PHONGPERPIXEL:
			if(!bNoFragmentProgram)
			{
				PhongLightingAlgorithm = PHONG_PER_PIXEL;
				CheckMenuItem(hMenu, ID_ALGORITHM_PHONGPERVERTEX, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_ALGORITHM_PHONGPERPIXEL, MF_CHECKED);
			}
			break;
		case ID_ALGORITHM_ENABLESHLIGHTINGREDUCTION:
			if(bEnableSHLightingReduction)
			{
				CheckMenuItem(hMenu, ID_ALGORITHM_ENABLESHLIGHTINGREDUCTION, MF_UNCHECKED);
			}
			else
			{
				CheckMenuItem(hMenu, ID_ALGORITHM_ENABLESHLIGHTINGREDUCTION, MF_CHECKED);
				if(bUseAnalyticalLights)
					CalculateCoefficientsAnalytical();
			}
			bEnableSHLightingReduction = !bEnableSHLightingReduction;
			break;
		case ID_GEOMETRY_INCREASEGEOMETRYCOMPLEXITY:
			SphereDetail += 5;
			break;
		case ID_GEOMETRY_DECREASEGEOMETRYCOMPLEXITY:
			if(SphereDetail > 10)
				SphereDetail -= 5;
			break;
		case ID_GEOMETRY_SWITCHPLANEPOOLTABLE:
			bDrawPlane = !bDrawPlane;
			break; 
		case ID_GEOMETRY_SWITCHWHITE:
			bDrawWhiteBalls = !bDrawWhiteBalls;
			break;
		case ID_GEOMETRY_SWITCHROTATEMODE:
			rotateAroundCenter = !rotateAroundCenter;
			break;
		case ID_GEOMETRY_SWITCHWIREFRAME:
			bDrawWireframe = !bDrawWireframe;
			break;
		case ID_GEOMETRY_SWITCHTEXTUREON:
			bUseTextures = !bUseTextures;
			break;
		case ID_HELP_ABOUT:
			DialogBox(gHINSTANCE, MAKEINTRESOURCE(IDD_DIALOG2), gHWND, DlgProcAbout);
			break;
		}
		break;	
	}
}


/*******************************************************************************
** RenderScene(...) is the main rendering function. It is called by OGLShell
** for every frame.
** This function should contain the rendering functions to call (glBegin/glEnd,
** glVertex, etc...)
*******************************************************************************/
BOOL RenderScene(HDC hDC, HGLRC hRC)
{
	/* Do your actual 3D rendering here */

	KeyboardHandling();

	/* do light segregation */
	DetermineImportantLights();

	/* move camera */
	if(rotateAroundCenter)
	{
		RotateAroundCenter();
	}

	glDepthMask(GL_TRUE);
	Draw();
	DrawLights();
	RotateLights();

	if(bUseAnalyticalLights)
		CalculateCoefficientsAnalytical();

	DisplayHUD();

	/* The function should ALWAYS return TRUE if the rendering was successful.
	   If OGLShell receives FALSE from this function, then rendering will be
	   stopped and the application will be terminated */
	return TRUE;
}
	

/*******************************************************************************
** InitView() is called by OGLShell each time a rendering variable is changed
** in the Shell menu (resolution change, z-buffer mode...)
** In this function one should initialise all variables that are dependant on
** general rendering variables (screen mode, 3D driver, etc...)
*******************************************************************************/
BOOL InitView(HWND hWindow, HDC hDC, HGLRC hRC, DWORD dwWidth, DWORD dwHeight)
{
	/* From this function you can create your rendering variables (Lights, 
	   Materials, Textures, etc...) */

	winAspectRatio = dwWidth/(float)dwHeight;
	if(InitialiseOpenGL() == FALSE)
		return FALSE;

	if(bSupportGLSL)
	{
		if(!bSupportGLSLQuestionAsked)
		{
			if(MessageBox(hWindow, "This videocard supports GLSL extensions. Do you want to use GLSL instead of ARB_vertex/fragment_programs?",
				"Enable GLSL support", MB_YESNO | MB_ICONQUESTION) == IDNO)
			{
				bSupportGLSL = FALSE;
			}
			bSupportGLSLQuestionAsked = TRUE;
			bSupportGLSLAnswer = bSupportGLSL;
		}
		else
			bSupportGLSL = bSupportGLSLAnswer;

	}

	InitialiseBallsTable();

	/* load all the fragment and vertex program, if any problems stop */
	if(!LoadPrograms())
		return FALSE;

	SetupLights();

	SHAnalyticalInitConstants();
	SHEnvInitConstants();

	CheckMenuItem(hMenu, ID_ALGORITHM_PHONGPERVERTEX, MF_CHECKED);
	CheckMenuItem(hMenu, ID_ALGORITHM_SHPERVERTEX, MF_CHECKED);
	if(bNoFragmentProgram)
	{
		EnableMenuItem(hMenu, ID_ALGORITHM_PHONGPERPIXEL, MF_GRAYED);
		EnableMenuItem(hMenu, ID_ALGORITHM_SHPERPIXEL, MF_GRAYED);
	}

	/* Initially we are doing only analytical lights */
	CheckMenuItem(hMenu, ID_ALGORITHM_ANALYTICALLIGHTS, MF_CHECKED);

	/* set up fonts */
	OGLTPrint3DSetTextures(dwWidth, dwHeight);

	/* The function should ALWAYS return TRUE if the initialisation was successful.
	   If OGLShell receives FALSE from this function, then rendering will be
	   stopped and the application will be terminated */
	return TRUE;
}


/*******************************************************************************
** ReleaseView(...) is called by OGLShell each time a rendering variable is
** changed. It is the direct companion to InitView(...), as its purpose is to
** release or destroy any variables or processes allocated in InitView.
** NOTE : Textures created with the OGLShellLoadBMP(...) function do *NOT* need
** to be released, as it is already taken care of by OGLShell.
*******************************************************************************/
void ReleaseView()
{
	/* Release any interfaces or variables created in InitView() */

	OGLTPrint3DReleaseTextures();
}

