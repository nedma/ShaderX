/*****************************************************************************
  Name : OGLShell.h v1.9.10
  Date : October 2003
  Platform : ANSI compatible

  Header file to be used with OGLShell.cpp

  Copyright : 2003 by Imagination Technologies. All rights reserved.
******************************************************************************/

#ifndef _OGLShell_H_
#define _OGLShell_H_

/* The 3D menus of OGLShell requires linking with the OGLTools library (OGLTools.h and OGLTools.lib).
   You may comment out the following line to disable the compilation of 3D menus-related functions. In that 
   case OGLTools.h and OGLTools.lib are not required to the build process. */
#undef OGLSHELL_USE_3D_MENUS

#ifdef __cplusplus
extern "C" {
#endif

/* Enum */
typedef enum _OGLShellPrefs
{
	DEFAULT_CLEAR_ON		=0x00000001,	// Viewport clear is On by default
	DEFAULT_FULLSCREEN		=0x00000002,	// Application will start FullScreen by default
	DEFAULT_NO_VSYNC		=0X00000004,	// Vertical Synchronisation will be OFF by default
	FORCE_CLEAR_ON			=0x00000008,	// Viewport clear will be forced On and will not appear in the menu
	FORCE_CLEAR_OFF			=0x00000010,	// Viewport clear will be forced Off and will not appear in the menu
	FORCE_FULLSCREEN		=0x00000020,	// FullScreen mode will be used all the time (Unable to get to Window mode)
	DISABLE_VERY_HIGH_RES	=0x00000040,	// Disable resolution above 1024x768
	DISABLE_Z_CLEAR			=0x00000080,	// Z Clear will not be performed.
	START_WINDOW_320X240	=0x00000100,	// Starting window resolution will be 320x240 (Cannot be used with FULLSCREEN)
	USE_INSTANTANEOUS_FPS	=0x00000200,	// Frame rate will be computed on an instantaneous basis (each frame) rather then average (default)
	ENABLE_CLEAN_EXIT		=0x00000400,	// OGLShell will directly exit when FALSE is returned from InitView() or RenderScene()
	ENABLE_STENCIL_BUFFER	=0x00000800,	// OGLShell will select a stencil format by default (if available)
	USE_3D_MENUS			=0x00001000,	// 3D scaling menus will be used
	DISPLAY_PRINT3D_ICONS	=0x00002000		// The menu icons for page management will be displayed. 
} OGLShellPrefs;

/* Enum */
typedef enum _OGLShellVariables
{
	APPLICATION_INSTANCE_HANDLE	=1,	// Handle of application (HINSTANCE type)
	PREVIOUS_INSTANCE_HANDLE,		// Handle of previous application (HINSTANCE type)
	WINDOW_HANDLE,					// Application window handle (HWND type)
	FULLSCREEN_MODE,				// Is application in FullScreen mode (BOOL type)
	FRAME_BUFFER_BIT_DEPTH,			// Rendering surface bit depth (DWORD type)
	FRAME_RATE						// Current frame rate (DWORD type)
} OGLShellVariables;


/*******************************************
** OGLShell helper functions declarations **
*******************************************/

/* This function is used to pass preferences to the OGLShell
   If used, it must be called from InitApplication() only. */
void OGLShellSetPreferences(const char *pszApplicationName, 
							const HMENU	hUserMenuID, 
							const HACCEL hUserAccel, 
							const HICON	hUserIcon, 
							const DWORD	dwFlags);

/* This function loads a BMP texture and returns its name.
   format is used to specify the texture format as used in the glTexImage2D() OpenGL function */
BOOL OGLShellLoadBMP(const char *lpName, const BOOL bTranslucent, const BOOL bMIPMap, 
					 const GLenum format, unsigned int *pnTextureName);

/* This function enables the user to retrieve rendering variables if needed */
void OGLShellGetVariable(DWORD dwVariableName, 
						 void *pVariablePointer);


/*******************************
** User interaction functions **
*******************************/

/* These functions have to exist in the scene file (e.g. scene.c).
   They are used to interact with OGLShell */

void InitApplication(HINSTANCE hInstance, char *pszCommandLine);
/*
This function will be called by OGLShell ONCE at the beginning of the OGLShell 
WinMain() function. This function enables the user to perform any initialisation 
before any 3D is rendered.
From this function the user can call OGLShellSetPreferences() to set default
values or submit a menu to OGLShell. See above for the prototype of this function.
*/


void QuitApplication();
/*
This function will be called by OGLShell just before finishing the program.
It enables the user to release any memory allocated in InitApplication().
*/


void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
/*
This function is the user Window Procedure function. It enables the user to retrieve
menu choices, keystrokes or other messages (WM_TIMER for instance).
If you don't want to use this function, put nothing in it :
    
void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Nothing !
}

OGLShell processes many window messages. If the user needs to 
process these messages in their application, they should make sure
NOT to return DefWindowProc() as it will prevent the OGLShell WindowProc
to do its own processing for these messages.
The window messages processed by OGLShell are :

WM_ENTERMENULOOP, WM_EXITMENULOOP,
WM_ACTIVATEAPP,
WM_SYSCOMMAND,
WM_SIZE, WM_MOVE, WM_SIZING, WM_MOVING,
WM_PAINT,
WM_DESTROY, WM_QUIT

Note : do NOT process the VK_ESCAPE key, as it is already used by  
  	   OGLShell to quit the application.
*/


BOOL RenderScene(HDC hDC, HGLRC hRC);
/*
That's the main user function in which you have to do your own rendering.
The current Device Context handle (hDC) and OpenGL Render Context handles (hRC)
are passed to this function so you may use them to control your rendering.
This function is called every frame by OGLShell and enable the user to
create the scene for this frame.
*/


BOOL InitView(HWND hWindow, HDC hDC, HGLRC hRC, DWORD dwWidth, DWORD dwHeight);
/*
This function enables the user to create display lists, materials, set 
rendering values, etc... This function will be called each time the rendering 
context has to be reinitialised (i.e. switching to FullScreen, changing Z-Buffer 
mode,  etc...).
You may need the window handle (hWindow), the Device Context handle (hDC), the
OpenGL Rendering Context (hRC) and the current window dimensions (dwWidth and dwHeight)
to create your rendering variables.
*/

void ReleaseView();
/*
This function enables the user to release any variables they created in IniView().
The function will be called each time the OpenGL Rendering Context has to be recreated. 
*/

#ifdef OGLSHELL_USE_3D_MENUS

BOOL OGLShellDisplay3DMenus();
/*
If 3D menus are to be used, a single call to this function must be made by 
the user within the BeginScene to EndScene pair. It returns true if successful, 
and false otherwise. 
*/

BOOL OGLShellTick3DMenus(UINT identifier, BOOL tick);
/*
When updating menus this function allows you to tick a menu item by passing its
identifier and a boolean to determine whether to tick or untick this item.
*/

BOOL OGLShellGhost3DMenus(UINT identifier, BOOL tick);
/*
When updating menus this function allows you to ghost a menu item by passing its
identifier and a boolean to determine whether to ghost or unghost this item.
*/

#endif

#ifdef __cplusplus
}
#endif

#endif
