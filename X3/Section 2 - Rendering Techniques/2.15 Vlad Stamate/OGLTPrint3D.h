/*****************************************************************************
  Name : OGLTPrint3D.h			v1.0
  Date : November 2000
 
  * Description : 
  Header for OGLPrint3D.c

  This file is part of the TOOLS library (OGLTools.lib) used
  with the SDK demos for PowerVR series 3.

  Ported from D3DTPrint3D.h

  
  Copyright 2000, PowerVR Technologies. 
  PowerVR Technologies is a division of Imagination Technologies Limited. 
  All Rights Reserved.
******************************************************************************/

#ifndef _Print3D_H_
#define _Print3D_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_SYSTEM -1.0f
#define FONT_ARIAL   1.0f

/* dwFlags for OGLTPrint3DSetWindowFlags */
typedef enum {
	PRINT3D_ACTIVATE_WIN		=	0x01,
	PRINT3D_DEACTIVATE_WIN		=	0x02,
	PRINT3D_ACTIVATE_TITLE		=	0x04,
	PRINT3D_DEACTIVATE_TITLE	=	0x08,
	PRINT3D_FULL_OPAQUE			=	0x10,
	PRINT3D_FULL_TRANS			=	0x20,
	PRINT3D_ADJUST_SIZE_ALWAYS	=	0x40,
	PRINT3D_NO_BORDER			=	0x80,
} PRINT3D_FLAGS;

/**************/
/* Prototypes */
/**************/

/* Always in InitView */
BOOL OGLTPrint3DSetTextures(DWORD dwScreenX, DWORD dwScreenY);

/* Always in ReleaseView */
void  OGLTPrint3DReleaseTextures (void);

/* Screen Output */
void OGLTPrint3D(float XPos, float YPos, float fScale,  int Colour, const TCHAR *Format, ...);

/* Creates a default window.
   If Title is NULL the main body will have just one line (for InfoWin) 
*/
DWORD OGLTPrint3DCreateDefaultWindow(float fPosX, float fPosY, int nXSize_LettersPerLine, TCHAR *sTitle, TCHAR *sBody);

/* Creates a window and returns a handle to that window */
DWORD OGLTPrint3DCreateWindow(DWORD dwBufferSizeX, DWORD dwBufferSizeY);

/* Free memory allocated with CreateWindow */
void OGLTPrint3DDeleteWindow(DWORD dwWin);

/* Free memory allocated with CreateWindow for all the windows*/
void OGLTPrint3DDeleteAllWindows(void);

/* Display the window (must be between BeginScene-EndScene)*/
void OGLTPrint3DDisplayWindow(DWORD dwWin);

/* Display All (must be between BeginScene-EndScene)*/
void OGLTPrint3DDisplayAllWindows(void);

/* Feed the text buffer */
void OGLTPrint3DWindowOutput(DWORD dwWin, const TCHAR *Format, ...);

/* Clean the text buffer */
void OGLTPrint3DClearWindowBuffer(DWORD dwWin);

/* User values for the window.
 * Windows position and size are referred to a virtual screen of 100x100,
 * (0,0) is the top-left corner and (100,100) the bottom-right corner.
 * These values are the same for all resolutions.
 */
void  OGLTPrint3DSetWindow(DWORD dwWin, DWORD dwBackgroundColor, DWORD dwFontColor, float fFontSize, 
						   float fPosX, float fPosY, float fSizeX, float fSizeY);

/* User values for the title */
void OGLTPrint3DSetTitle(DWORD dwWin, DWORD dwBackgroundColor, float fFontSize, 
						 DWORD dwFontColorLeft, BYTE *sTitleLeft, 
						 DWORD dwFontColorRigth, BYTE *sTitleRight);
/* Setting flags */
void OGLTPrint3DSetWindowFlags(DWORD dwWin, DWORD dwFlags);

/* Cursor position for the next OutputWindow */
void OGLTPrint3DSetWindowBufferPointer(DWORD dwWin, DWORD dwXPos, DWORD dwYPos);

/* Calculates window size to display everything.
 * (dwMode 0 = Both, dwMode 1 = X only,  dwMode 2 = Y only) 
 */
void OGLTPrint3DAdjustWindowSize(DWORD dwWin, DWORD dwMode);

/* This window on the top of everything */
void OGLTPrint3DSetTopMostWindow(DWORD dwWin);

/* Window position */
void OGLTPrint3DSetWindowPos(DWORD dwWin, float fPosX, float fPosY);

/* Window size */
void OGLTPrint3DSetWindowSize(DWORD dwWin, float fSizeX, float fSizeY);

/* Get resolution */
void OGLTPrint3DGetResolution(DWORD *dwScreenX, DWORD *dwScreenY);

/* Set resolution */
void OGLTPrint3DSetResolution(DWORD dwScreenX, DWORD dwScreenY);

/* Get lenght of a string in pixels */
float OGLTPrint3DGetLength (float fFontSize, BYTE *sString);

#ifdef __cplusplus
}
#endif

#endif

