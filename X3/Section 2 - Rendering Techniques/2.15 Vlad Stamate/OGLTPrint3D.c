/*****************************************************************************
  Name : OGLTPrint3D.c		v1.0
  Date : November 2000

  * Description : 
  Displays a text string using 3D polygons.
  Can be done in two ways: using a window defined by the user
  or writing straight on the screen.

  This file is part of the TOOLS library (OGLTools.lib) used
  with the SDK demos for PowerVR series 3.

  Ported from OGLTPrint3D.c

  Copyright 2000, PowerVR Technologies. 
  PowerVR Technologies is a division of Imagination Technologies Limited. 
  All Rights Reserved.
******************************************************************************/

/*************
** Includes **
*************/
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <crtdbg.h>
#include <gl/gl.h>
#include "OGLTPrint3D.h"		
#include "OGLTPrint3D.dat"		/* Print3D texture data */


/************
** Defines **
************/
#define MAX_LETTERS 5000
#define MAX_WINDOWS 500
#define LINES_SPACING 29.0f

#define Print3D_WIN_EXIST	1
#define Print3D_WIN_ACTIVE	2
#define Print3D_WIN_TITLE	4
#define Print3D_WIN_STATIC	8
#define Print3D_FULL_OPAQUE	16
#define Print3D_FULL_TRANS	32
#define Print3D_ADJUST_SIZE	64
#define Print3D_NO_BORDER	128

#define OPENGL_RGBA(r, g, b, a)   ((GLuint) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))


/*************
** Typedefs **
*************/
typedef struct {
	GLfloat sx, sy, sz;
	GLuint	color;
	GLfloat	tu, tv;
} OGLTLVERTEX;

typedef struct {
	DWORD dwFlags;

	/* Text Buffer */
	TCHAR  *pTextBuffer;
	DWORD  dwBufferSizeX;
	DWORD  dwBufferSizeY;
	DWORD  dwCursorPos;

	/* Title */
	float fTitleFontSize;
	float fTextRMinPos;
	DWORD dwTitleFontColorL;
	DWORD dwTitleFontColorR;
	DWORD dwTitleBaseColor;
	BYTE  *bTitleTextL;
	BYTE  *bTitleTextR;

	/* Window */
	float fWinFontSize;
	DWORD dwWinFontColor;
	DWORD dwWinBaseColor;
	float fWinPos[2];
	float fWinSize[2];
	float fZPos;
	DWORD dwSort;

} OGLTPrint3DWIN;


/************
** Globals **
************/
static OGLTPrint3DWIN	GlobWin[MAX_WINDOWS];
static BOOL				bTexturesReleased = TRUE;

static struct {
	GLuint			uTexture[5];
	OGLTLVERTEX		*pVerticesFont;
	WORD			pFacesFont[MAX_LETTERS*2*3];
	DWORD			dwCreatedWins;
	float			fScreenScale[2];
	DWORD			dwTopMost;
	BOOL			bInitOK;
} Glob;


/***************
** Prototypes **
***************/
static HRESULT	DrawWindow(DWORD dwWin, DWORD Color, float fZPos, float fPosX, float fPosY, float fSizeX, float fSizeY);
static void		LocalPrint3D(DWORD dwWin, float fZPos, float XPos, float YPos, float fScale, int Colour, const TCHAR *Text);
static BOOL		UpLoad4444(DWORD TexID, unsigned char *pSource, DWORD nSize, DWORD nMode);
static float	GetLength(float fFontSize, BYTE *sString);
static void		RenderStates(int nAction);



/*----------------------------------------------------------------------
 *  Function Name   : OGLTPrint3DSetTextures							 
 *  Inputs          : dwScreenX, dwScreenY                                                    
 *  Outputs         : None                                              
 *  Returns         : TRUE if OK.                                       
 *  Globals Used    : Glob                                                  
 *  Description     : Initialization and texture upload.
 *----------------------------------------------------------------------*/
BOOL OGLTPrint3DSetTextures(DWORD dwScreenX, DWORD dwScreenY)
{
	int		i;
	BOOL	bStatus;

	/* Initiation is not OK as long as textures aren't loaded */
	Glob.bInitOK = FALSE;

	/* Compute some values */
	Glob.fScreenScale[0] = (float)dwScreenX/640.0f;
	Glob.fScreenScale[1] = (float)dwScreenY/480.0f;
	Glob.dwTopMost = 255;


	/* Release textures if they haven't been already */
	if (bTexturesReleased==FALSE)
	{
		OGLTPrint3DReleaseTextures();
	}

	/* Textures haven't been released */
	bTexturesReleased = FALSE;
   
	/* Memory allocation for vertices */
	Glob.pVerticesFont=	(OGLTLVERTEX *)malloc(MAX_LETTERS * 4 * sizeof(OGLTLVERTEX));
	if (!Glob.pVerticesFont)
	{
		OutputDebugString ("\nDTPrint3DInit: Memory allocation error.");
		return FALSE;
	}

	/* This is the window background texture.
	   Type 0 because the data comes in TexTool rectangular format. */
	bStatus = UpLoad4444(1, (unsigned char *)WindowBackground, 16, 0);
	if (!bStatus) return FALSE;
	
	bStatus = UpLoad4444(2, (unsigned char *)WindowPlainBackground, 16, 0);
	if (!bStatus) return FALSE;

	bStatus = UpLoad4444(3, (unsigned char *)WindowBackgroundOp, 16, 0);
	if (!bStatus) return FALSE;

	bStatus = UpLoad4444(4, (unsigned char *)WindowPlainBackgroundOp, 16, 0);
	if (!bStatus) return FALSE;

	/* This is the texture with the fonts.
	   Type 1 because there is only alpha component (RGB are white). */
	bStatus = UpLoad4444(0, (unsigned char *)OGLTPrint3DABC_Pixels, 256, 1);
	if (!bStatus) return FALSE;		
	
	/* Vertex indices (inverted compared to OGL as default culling order is different) */
	for (i=0; i<MAX_LETTERS; i++)
	{
		Glob.pFacesFont[i*6+0] = 0+i*4;
		Glob.pFacesFont[i*6+1] = 3+i*4;
		Glob.pFacesFont[i*6+2] = 1+i*4;

		Glob.pFacesFont[i*6+3] = 3+i*4;
		Glob.pFacesFont[i*6+4] = 0+i*4;
		Glob.pFacesFont[i*6+5] = 2+i*4;
	}

	/* Everything is OK */
	Glob.bInitOK = TRUE;

	/* Return OK */
	return TRUE;
}


/*----------------------------------------------------------------------*
 *  Function Name   : OGLTPrint3DReleaseTextures						 
 *  Inputs          : None
 *  Outputs         : None
 *  Returns         : Nothing
 *  Globals Used    : Glob								
 *  Description     : Deallocate the memory allocated in OGLTPrint3DSetTextures.  
 *----------------------------------------------------------------------*/
void OGLTPrint3DReleaseTextures()
{
	/* Delete textures */
	glDeleteTextures(5, Glob.uTexture);

	/* Free vertices allocated in OGLTPrint3DSetTextures */
	if (Glob.pVerticesFont) 
	{
		free(Glob.pVerticesFont);
		Glob.pVerticesFont=NULL;
	}

	/* Set state */
	bTexturesReleased = TRUE;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DDeleteAllWindows
 *  Inputs			: None
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: None
 *  Description		: Delete all windows
 *----------------------------------------------------------------------*/
void OGLTPrint3DDeleteAllWindows()
{
	int i;

	/* Loop through all windows */
	for (i=0; i<MAX_WINDOWS; i++)
	{
		/* Delete window */
		OGLTPrint3DDeleteWindow(i);
	}
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DDeleteWindow
 *  Inputs			: dwWin
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlobWin[]
 *  Description		: Delete window
 *----------------------------------------------------------------------*/
void OGLTPrint3DDeleteWindow(DWORD dwWin)
{
	/* Only delete window if it exists */
	if (GlobWin[dwWin].dwFlags & Print3D_WIN_EXIST)
	{
		/* If a text buffer was allocated, free it */
		if (GlobWin[dwWin].pTextBuffer)
		{
			free(GlobWin[dwWin].pTextBuffer);
			GlobWin[dwWin].pTextBuffer = NULL;
		}

		/* Free buffers corresponding to left and right title strings */
		if (GlobWin[dwWin].bTitleTextL) 
		{
			free(GlobWin[dwWin].bTitleTextL);
			GlobWin[dwWin].bTitleTextL = NULL;
		}
		if (GlobWin[dwWin].bTitleTextR) 
		{
			free(GlobWin[dwWin].bTitleTextR);
			GlobWin[dwWin].bTitleTextR = NULL;
		}
	}

	/* Reset flags */
	GlobWin[dwWin].dwFlags = 0;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DClearWindowBuffer
 *  Inputs			: dwWin
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlobWin[]
 *  Description		: Clear text buffer associated with window
 *----------------------------------------------------------------------*/
void OGLTPrint3DClearWindowBuffer(DWORD dwWin)
{
	/* Check that window exists before doing anything to its buffer */
	if (GlobWin[dwWin].dwFlags & Print3D_WIN_EXIST)
	{
		ZeroMemory(GlobWin[dwWin].pTextBuffer, ((GlobWin[dwWin].dwBufferSizeX) * (GlobWin[dwWin].dwBufferSizeY)) * sizeof(TCHAR));
	}

	/* Reset cursor position */
	GlobWin[dwWin].dwCursorPos	= 0;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetWindowBufferPointer
 *  Inputs			: dwWin, dwXPos, dwYPos
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlobWin
 *  Description		: Set position of window buffer pointer
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetWindowBufferPointer(DWORD dwWin, DWORD dwXPos, DWORD dwYPos)
{
	/* Check that window exists before doing anything to its buffer */
	if (GlobWin[dwWin].dwFlags & Print3D_WIN_EXIST)
	{
		/* Check desired position is within buffer size */
		if (dwXPos <= GlobWin[dwWin].dwBufferSizeX && dwYPos <= GlobWin[dwWin].dwBufferSizeY)
		{
			GlobWin[dwWin].dwCursorPos	= dwXPos + (GlobWin[dwWin].dwBufferSizeX*dwYPos);
		}
	}
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetTopMostWindow
 *  Inputs			: dwWin
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: Glob
 *  Description		: Set visible window
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetTopMostWindow(DWORD dwWin)
{
	/* Set new top most window */
	Glob.dwTopMost = dwWin;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetWindowPos
 *  Inputs			: dwWin, fPosX, fPosY
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlobWin[]
 *  Description		: Set window position
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetWindowPos(DWORD dwWin, float fPosX, float fPosY)
{
	/* Set new window position */
	GlobWin[dwWin].fWinPos[0] = fPosX * 640.0f/100.0f;
	GlobWin[dwWin].fWinPos[1] = fPosY * 480.0f/100.0f;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetWindowSize
 *  Inputs			: dwWin, fSizeX, fSizeY
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlowWin[]
 *  Description		: Set window size
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetWindowSize(DWORD dwWin, float fSizeX, float fSizeY)
{
	/* Set new window size */
	GlobWin[dwWin].fWinSize[0] = fSizeX * 640.0f/100.0f;
	GlobWin[dwWin].fWinSize[1] = fSizeY * 480.0f/100.0f;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DDisplayAllWindows
 *  Inputs			: None
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlobWin[]
 *  Description		: Render all 3D windows
 *----------------------------------------------------------------------*/
void OGLTPrint3DDisplayAllWindows()
{
	DWORD i;

	/* Loop through all windows in the creation order */
	for (i=0; i<MAX_WINDOWS; i++)
	{
		/* Display window (if it exists) */
		if ((GlobWin[i].dwFlags & Print3D_WIN_EXIST) && i!=Glob.dwTopMost)
		{
			OGLTPrint3DDisplayWindow(i);
		}

	}

	/* Finally display top-most window on top of the other ones */
	if (GlobWin[Glob.dwTopMost].dwFlags & Print3D_WIN_EXIST)
	{
		OGLTPrint3DDisplayWindow(Glob.dwTopMost);
	}
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetWindowFlags
 *  Inputs			: dwWin, dwFlags
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlobWin[]
 *  Description		: Set window flags
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetWindowFlags(DWORD dwWin, DWORD dwFlags)
{
	/* Set window flags */
	if (dwFlags & PRINT3D_ACTIVATE_WIN)		GlobWin[dwWin].dwFlags |= Print3D_WIN_ACTIVE;
			
	if (dwFlags & PRINT3D_DEACTIVATE_WIN)	GlobWin[dwWin].dwFlags &= ~Print3D_WIN_ACTIVE;
			
	if (dwFlags & PRINT3D_ACTIVATE_TITLE)	GlobWin[dwWin].dwFlags |= Print3D_WIN_TITLE;
			
	if (dwFlags & PRINT3D_DEACTIVATE_TITLE) GlobWin[dwWin].dwFlags &= ~Print3D_WIN_TITLE;
			
	if (dwFlags & PRINT3D_FULL_OPAQUE)		GlobWin[dwWin].dwFlags |= Print3D_FULL_OPAQUE;
		
	if (dwFlags & PRINT3D_FULL_TRANS)		GlobWin[dwWin].dwFlags |= Print3D_FULL_TRANS;

	if (dwFlags & PRINT3D_ADJUST_SIZE_ALWAYS)
	{
		GlobWin[dwWin].dwFlags |= Print3D_ADJUST_SIZE;
		OGLTPrint3DAdjustWindowSize(dwWin, 0);
	}

	if (dwFlags & PRINT3D_NO_BORDER)		GlobWin[dwWin].dwFlags |= Print3D_NO_BORDER;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetWindow
 *  Inputs			: dwWin, dwWinColor, dwFontCOlor, fFontSize,
 *					  fPosX, fPosY, fSizeX, fSizeY
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlowWin[]
 *  Description		: Set window properties
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetWindow(DWORD dwWin, DWORD dwWinColor, DWORD dwFontColor, float fFontSize, 
						  float fPosX, float fPosY, float fSizeX, float fSizeY)
{
	/* Set window properties */
	GlobWin[dwWin].fWinFontSize		= fFontSize;
	GlobWin[dwWin].dwWinFontColor	= dwFontColor;
	GlobWin[dwWin].dwWinBaseColor	= dwWinColor; 
	GlobWin[dwWin].fWinPos[0]		= fPosX  * 640.0f/100.0f;
	GlobWin[dwWin].fWinPos[1]		= fPosY  * 480.0f/100.0f;
	GlobWin[dwWin].fWinSize[0]		= fSizeX * 640.0f/100.0f;
	GlobWin[dwWin].fWinSize[1]		= fSizeY * 480.0f/100.0f;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DSetTitle
 *  Inputs			: dwWin, dwBackgroundClor, fFontSize, dwFontColorLeft, 
 *					  *sTitleLeft, dwFontColorRight, *sTitleRight
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlowWin[]
 *  Description		: Set titles for specified window
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetTitle(DWORD dwWin, DWORD dwBackgroundColor, float fFontSize, 
						 DWORD dwFontColorLeft, BYTE *sTitleLeft, 
						 DWORD dwFontColorRight, BYTE *sTitleRight)
{
	/* Set new titles (left and right) */
   	if (sTitleLeft)		sprintf(GlobWin[dwWin].bTitleTextL, sTitleLeft);
	if (sTitleRight)	sprintf(GlobWin[dwWin].bTitleTextR, sTitleRight);

	/* Set title properties */
	GlobWin[dwWin].fTitleFontSize	  = fFontSize;
	GlobWin[dwWin].dwTitleFontColorL  = dwFontColorLeft;
	GlobWin[dwWin].dwTitleFontColorR  = dwFontColorRight;
	GlobWin[dwWin].dwTitleBaseColor   = dwBackgroundColor; 
	GlobWin[dwWin].fTextRMinPos       = GetLength(GlobWin[dwWin].fTitleFontSize, GlobWin[dwWin].bTitleTextL) + 10.0f;
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DCreateWindow
 *  Inputs			: dwBufferSizeX, dwBufferSizeY
 *  Outputs			: None
 *  Returns			: Handle to a window.
 *  Globals Used	: GlobWin[]
 *  Description		: Creates a window
 *----------------------------------------------------------------------*/
DWORD OGLTPrint3DCreateWindow(DWORD dwBufferSizeX, DWORD dwBufferSizeY)
{
	DWORD			i;
	static DWORD	dwCurrentWin = 1;

	/* Check that a window with the same window handle doesn't already exist */
	if (GlobWin[dwCurrentWin].dwFlags & Print3D_WIN_EXIST)
	{
		/* It already exists, so delete it */
		OGLTPrint3DDeleteWindow(dwCurrentWin);
	}

	/* First window to be created ? */
	if (dwCurrentWin==1)
	{
		/* Cleaning all the flags when first win is created.*/
		for (i=0; i<MAX_WINDOWS; i++) 
		{
			GlobWin[i].dwFlags = 0;
			GlobWin[i].pTextBuffer = NULL;
		}
	}

	/* Have we reached maximum number of windows ? */
	if (dwCurrentWin == MAX_WINDOWS)
	{
		/* Well... start from the beginning again */
		dwCurrentWin = 1;
		OGLTPrint3DDeleteWindow(dwCurrentWin);
		GlobWin[dwCurrentWin].dwFlags = 0;
		GlobWin[dwCurrentWin].pTextBuffer = NULL;
		OutputDebugString ("\nOGLTPrint3DCreateWindow WARNING: MAX_WINDOWS overflow.\n");
	}

	/* Set default flags */
	GlobWin[dwCurrentWin].dwFlags = Print3D_WIN_TITLE  | Print3D_WIN_EXIST | Print3D_WIN_ACTIVE;

	/* Text Buffer */
	GlobWin[dwCurrentWin].dwBufferSizeX = dwBufferSizeX + 1;
	GlobWin[dwCurrentWin].dwBufferSizeY = dwBufferSizeY;
	GlobWin[dwCurrentWin].pTextBuffer  = (TCHAR *)malloc((dwBufferSizeX+2)*(dwBufferSizeY+2)*sizeof(TCHAR));
	GlobWin[dwCurrentWin].bTitleTextL  = (TCHAR *)malloc(1000*sizeof(TCHAR));
	GlobWin[dwCurrentWin].bTitleTextR  = (TCHAR *)malloc(1000*sizeof(TCHAR));

	/* Memory allocation failed */
	if (!GlobWin[dwCurrentWin].pTextBuffer || !GlobWin[dwCurrentWin].bTitleTextL || !GlobWin[dwCurrentWin].bTitleTextR)
	{
		OutputDebugString ("\nOGLTPrint3DCreateWindow : No memory enough for Text Buffer.\n");
		return 0;
	}

	/* Cleaning the Buffer */
	for (i=0; i<dwBufferSizeX*(dwBufferSizeY+1); i++) *(GlobWin[dwCurrentWin].pTextBuffer+i) = 0;

	/* NULL string for the title */
	*(GlobWin[dwCurrentWin].bTitleTextL)  = 0;
	*(GlobWin[dwCurrentWin].bTitleTextR)  = 0;

	/* Title */
	GlobWin[dwCurrentWin].fTitleFontSize	= 1.0f;
	GlobWin[dwCurrentWin].dwTitleFontColorL = OPENGL_RGBA(0xFF, 0xFF, 0x00, 0xFF);
	GlobWin[dwCurrentWin].dwTitleFontColorR = OPENGL_RGBA(0xFF, 0xFF, 0x00, 0xFF);
	GlobWin[dwCurrentWin].dwTitleBaseColor  = OPENGL_RGBA(0x30, 0x30, 0xFF, 0xFF); /* Dark Blue */

	/* Window */
	GlobWin[dwCurrentWin].fWinFontSize		= 0.5f;
	GlobWin[dwCurrentWin].dwWinFontColor	= OPENGL_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
	GlobWin[dwCurrentWin].dwWinBaseColor	= OPENGL_RGBA(0x80, 0x80, 0xFF, 0xFF); /* Light Blue */
	GlobWin[dwCurrentWin].fWinPos[0]		= 0.0f;
	GlobWin[dwCurrentWin].fWinPos[1]		= 0.0f;
	GlobWin[dwCurrentWin].fWinSize[0]		= 20.0f;
	GlobWin[dwCurrentWin].fWinSize[1]		= 20.0f;
	GlobWin[dwCurrentWin].fZPos		        = 0.0f;
	GlobWin[dwCurrentWin].dwSort		    = 0;
	GlobWin[dwCurrentWin].dwCursorPos		= 0;

	/* Increase window handle count */
	dwCurrentWin++;

	/* Returning the handle */
	return (dwCurrentWin-1);
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DDisplayWindow
 *  Inputs			: dwWin
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: Glob, GlobWin[]
 *  Description		: Render 3D window
 *----------------------------------------------------------------------*/
void OGLTPrint3DDisplayWindow(DWORD dwWin)
{
	DWORD	i, dwPointer=0;
	float	fTitleSize = 0.0f, fRPos, fNewPos;
	  
	/* No textures! so... no window */
	if (!Glob.bInitOK) 
	{
		OutputDebugString("OGLTPrint3DDisplayWindow : You must call OGLTPrint3DSetTextures()\nbefore using this function!!!\n");
		return;
	}

	/* Save current render states */
	RenderStates(0);

	/* TITLE */
	if (GlobWin[dwWin].dwFlags & Print3D_WIN_TITLE)
	{
		/* Set title size */
		fTitleSize = GlobWin[dwWin].fTitleFontSize * 23.5f + 16.0f;
		if (GlobWin[dwWin].fTitleFontSize < 0.0f) fTitleSize = 8.0f + 16.0f;
		
		/* Background */
		if (!(GlobWin[dwWin].dwFlags & Print3D_FULL_TRANS))
		{
			/* Draw title background */
			DrawWindow(dwWin, GlobWin[dwWin].dwTitleBaseColor, 0.0f, GlobWin[dwWin].fWinPos[0], GlobWin[dwWin].fWinPos[1], 
					   GlobWin[dwWin].fWinSize[0], fTitleSize);
		}

		/* Left text */
		LocalPrint3D(dwWin, 0.0f, (GlobWin[dwWin].fWinPos[0] + 6.0f), (GlobWin[dwWin].fWinPos[1] + 7.0f), 
					 GlobWin[dwWin].fTitleFontSize, GlobWin[dwWin].dwTitleFontColorL,  GlobWin[dwWin].bTitleTextL);

		/* Right text */
		if (GlobWin[dwWin].bTitleTextR)
		{
			/* Compute position */
			fRPos = GlobWin[dwWin].fWinSize[0]  - GetLength(GlobWin[dwWin].fTitleFontSize,GlobWin[dwWin].bTitleTextR) - 6.0f;

			/* Check that we're not under minimum position */
			if (fRPos<GlobWin[dwWin].fTextRMinPos) fRPos = GlobWin[dwWin].fTextRMinPos;

			/* Add window position */
			fRPos += GlobWin[dwWin].fWinPos[0];
		
			/* Print text */
			LocalPrint3D(dwWin, 0.0f, fRPos, GlobWin[dwWin].fWinPos[1] + 7.0f, 
											 GlobWin[dwWin].fTitleFontSize,
											 GlobWin[dwWin].dwTitleFontColorR, 
											 GlobWin[dwWin].bTitleTextR);
		}
	}


	/* WINDOW */
	if (GlobWin[dwWin].dwFlags & Print3D_WIN_ACTIVE)
	{
		/* Background */
		if (!(GlobWin[dwWin].dwFlags & Print3D_FULL_TRANS))
		{
			DrawWindow(dwWin, GlobWin[dwWin].dwWinBaseColor, 0.0f, GlobWin[dwWin].fWinPos[0],  (GlobWin[dwWin].fWinPos[1] + fTitleSize), 
							  GlobWin[dwWin].fWinSize[0],GlobWin[dwWin].fWinSize[1]);
		}

		/* Reset pointer */
		dwPointer = 0;

		/* Text, line by line */
		for (i=0; i<GlobWin[dwWin].dwBufferSizeY; i++)
		{
			/* Compute new text position */
			if (GlobWin[dwWin].fWinFontSize < 0.0f) 
			{
				/* New position for alternate font */
				fNewPos = fTitleSize + (float)(i * 12.0f);
			}
			else
			{
				/* New position for normal font */
				fNewPos = fTitleSize + (float)(i * GlobWin[dwWin].fWinFontSize) * LINES_SPACING;
			}

			/* Print window text */
			LocalPrint3D(dwWin, 0.0f, (GlobWin[dwWin].fWinPos[0] + 6.0f), (GlobWin[dwWin].fWinPos[1] + 6.0f + fNewPos), 
							GlobWin[dwWin].fWinFontSize, GlobWin[dwWin].dwWinFontColor, (GlobWin[dwWin].pTextBuffer + dwPointer));

			/* Increase pointer */
			dwPointer += GlobWin[dwWin].dwBufferSizeX;
		}
	}

	/* Restore current render states */
	RenderStates(1);
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DWindowOutput
 *  Inputs			: dwWin, *Format
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlowWin[]
 *  Description		: Set window text
 *----------------------------------------------------------------------*/
void OGLTPrint3DWindowOutput(DWORD dwWin, const TCHAR *Format, ...)
{
	DWORD		i, dwPosBx, dwPosBy, dwSpcPos;
	DWORD		dwBufferSize, dwTotalLength = 0;
	TCHAR		*Text;
	va_list		args;
	TCHAR		bChar;

	/* If window doesn't exist then return from function straight away */
	if (!(GlobWin[dwWin].dwFlags & Print3D_WIN_EXIST)) return;

	/* Compute buffer size */
	dwBufferSize = (GlobWin[dwWin].dwBufferSizeX+1) * (GlobWin[dwWin].dwBufferSizeY+1);

	/* Temporary buffer for the string */
	Text = (TCHAR *)malloc(dwBufferSize * sizeof(TCHAR));

	/* Reading the arguments to create our Text string */
	va_start(args,Format);
	_vsnprintf(Text, dwBufferSize, Format, args);
	va_end(args);

	/* Compute length */
	while (dwTotalLength<dwBufferSize && *(Text+dwTotalLength)!=0) dwTotalLength++;

	/* X and Y pointer position */
	dwPosBx = GlobWin[dwWin].dwCursorPos % GlobWin[dwWin].dwBufferSizeX;
	dwPosBy = GlobWin[dwWin].dwCursorPos / GlobWin[dwWin].dwBufferSizeX;

	/* Process each character */
	for (i=0; i<dwTotalLength; i++)
	{
		/* Get current character in string */
		bChar = *(Text+i);

		/* Space (for word wrap only) */
		if (bChar == ' ') 
		{
			/* Looking for the next space (or return or end) */
			dwSpcPos = 1;
			while (1)
			{
				bChar = *(Text+i+dwSpcPos++);
				if (bChar==' ' || bChar==0x0A || bChar==0) break;
			}
			bChar = ' ';

			/* Humm, if this word is longer than the buffer don't move it. 
			   Otherwise check if it is at the end and create a return.    */
			if (dwSpcPos<GlobWin[dwWin].dwBufferSizeX && (dwPosBx+dwSpcPos)>GlobWin[dwWin].dwBufferSizeX)
			{
				/* Set NULL character */
				*(GlobWin[dwWin].pTextBuffer + GlobWin[dwWin].dwCursorPos++) = 0;
				dwPosBx = 0;
				dwPosBy++;

				/* Set new cursor position */
				GlobWin[dwWin].dwCursorPos = dwPosBy * GlobWin[dwWin].dwBufferSizeX;

				/* Don't go any further */
				continue;
			}
		}

		/* End of line */
		if (dwPosBx == (GlobWin[dwWin].dwBufferSizeX-1))
		{
			*(GlobWin[dwWin].pTextBuffer + GlobWin[dwWin].dwCursorPos++) = 0;
			dwPosBx = 0;
			dwPosBy++;
		}

		/* Vertical Scroll */
		if (dwPosBy >= GlobWin[dwWin].dwBufferSizeY)
		{
			memcpy(GlobWin[dwWin].pTextBuffer, GlobWin[dwWin].pTextBuffer+GlobWin[dwWin].dwBufferSizeX,
				   (GlobWin[dwWin].dwBufferSizeX-1)*GlobWin[dwWin].dwBufferSizeY);

			GlobWin[dwWin].dwCursorPos -= GlobWin[dwWin].dwBufferSizeX;
			
			dwPosBx = 0;
			dwPosBy--;
		}

		/* Return */
		if (bChar == 0x0A) 
		{
			/* Set NULL character */
			*(GlobWin[dwWin].pTextBuffer + GlobWin[dwWin].dwCursorPos++) = 0;

			dwPosBx = 0;
			dwPosBy++;

			GlobWin[dwWin].dwCursorPos = dwPosBy * GlobWin[dwWin].dwBufferSizeX;

			/* Don't go any further */
			continue;
		}

		/* Storing our character */
		if (GlobWin[dwWin].dwCursorPos<dwBufferSize)
		{
			*(GlobWin[dwWin].pTextBuffer + GlobWin[dwWin].dwCursorPos++) = bChar;
		}

		/* Increase position */
		dwPosBx++;
	}

	/* Release our temporary buffer */
	free(Text);

	/* Automatic adjust of the window size */
	if (GlobWin[dwWin].dwFlags & Print3D_ADJUST_SIZE) 
	{
		OGLTPrint3DAdjustWindowSize(dwWin, 0);
	}
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3D
 *  Inputs			: XPos, YPos, fScale, Colour, *Format
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: None
 *  Description		: Display a string of characters on-screen
 *----------------------------------------------------------------------*/
void OGLTPrint3D(float XPos, float YPos, float fScale,  int Colour, const TCHAR *Format, ...)
{
	TCHAR	Text[MAX_LETTERS+1];
	va_list	args;
	
    /* Reading the arguments to create our Text string */
	va_start(args,Format);
	_vsnprintf(Text,MAX_LETTERS,Format,args);
	va_end(args);

	/* Save current render states */
	RenderStates(0);

	/* Adjust input parameters */
    XPos *= 640.0f/100.0f;
	YPos *= 480.0f/100.0f;

	/* Call our local function */
	LocalPrint3D(0, 0.0f, XPos, YPos, fScale, Colour, Text);
									
	/* Restore current render states */
	RenderStates(1);
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DAdjustWindowSize
 *  Inputs			: dwWin, dwMode
 *  Outputs			: None
 *  Returns			: Nothing
 *  Globals Used	: GlowWin[]
 *  Description		: Adjust window size
 *----------------------------------------------------------------------*/
void OGLTPrint3DAdjustWindowSize(DWORD dwWin, DWORD dwMode)
{
	DWORD	i, dwPointer = 0;
	float	fMax = 0.0f, fLength;

	/* Adjustment depends on mode */
	if (dwMode==1 || dwMode==0)
	{
		/* Title horizontal size */
		if (GlobWin[dwWin].dwFlags & Print3D_WIN_TITLE)
		{
			/* Get left title string length */
			fMax = GetLength(GlobWin[dwWin].fTitleFontSize, GlobWin[dwWin].bTitleTextL);

			/* If right title exists, add right title size */
			if (GlobWin[dwWin].bTitleTextR)
			{
				fMax += GetLength (GlobWin[dwWin].fTitleFontSize, GlobWin[dwWin].bTitleTextR) + 12.0f;
			}
		}

		/* Body horizontal size (line by line) */
		for (i=0; i<GlobWin[dwWin].dwBufferSizeY; i++)
		{
			/* Add length of this buffer's line */
			fLength = GetLength(GlobWin[dwWin].fWinFontSize, (GlobWin[dwWin].pTextBuffer + dwPointer));

			/* Check that we don't exceed maximum length */
			if (fLength > fMax) fMax = fLength;

			/* Increase pointer */
			dwPointer += GlobWin[dwWin].dwBufferSizeX;
		}
	
		/* New window size */
		GlobWin[dwWin].fWinSize[0] = fMax - 2.0f + 16.0f;
	}

	/* Vertical size */
	if (dwMode==0 || dwMode==2)
	{
		/* Is vertical size below one line in length ? */
		if (GlobWin[dwWin].dwCursorPos==0 || GlobWin[dwWin].dwBufferSizeY<2) 
		{
			i = 0;
		}
		else
		{
			/* Looking for the last line */
			for (i=GlobWin[dwWin].dwBufferSizeY-1 ; i>=0 ; i--)
			{
				if (*(GlobWin[dwWin].pTextBuffer + GlobWin[dwWin].dwBufferSizeX*i)) break;
			}
		}			 

		/* Size depends on font type used */
		if (GlobWin[dwWin].fWinFontSize>0)
		{
			/* Normal font */
			GlobWin[dwWin].fWinSize[1] = (float)(i+1) * LINES_SPACING * GlobWin[dwWin].fWinFontSize + 16.0f;
		}
		else
		{
			/* Alternate font */
			GlobWin[dwWin].fWinSize[1] = ((float)(i+1) * 12.0f) + 16.0f;
		}
	}
}


/*----------------------------------------------------------------------
 *  Function Name	: OGLTPrint3DCreateDefaultWindow
 *  Inputs			: fPosX, fPosY, nXSize_LettersPerLine, *sTitle, *sBody
 *  Outputs			: None
 *  Returns			: Window handle
 *  Globals Used	: None
 *  Description		: Creates a default window (PowerVR SDK-style)
 *----------------------------------------------------------------------*/
DWORD OGLTPrint3DCreateDefaultWindow(float fPosX, float fPosY, int nXSize_LettersPerLine, TCHAR *sTitle, TCHAR *sBody)
{
	DWORD dwActualWin;
	DWORD dwFlags = PRINT3D_ADJUST_SIZE_ALWAYS;
	DWORD dwBodyTextColor, dwBodyBackgroundColor;

	/* If no text is specified, return an error */
    if (!sBody && !sTitle) return -1;

	/* If no title is specified, body text colours are different */
	if (!sTitle)
	{
		dwBodyTextColor			= OPENGL_RGBA(0xFF, 0xFF, 0x30, 0xFF);
		dwBodyBackgroundColor	= OPENGL_RGBA(0x20, 0x20, 0xB0, 0xE0);
	}
	else
	{
		dwBodyTextColor			= OPENGL_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
		dwBodyBackgroundColor	= OPENGL_RGBA(0x20, 0x30, 0xFF, 0xE0);
	}
	
	/* Set window flags depending on title and body text were specified */
	if (!sBody)		dwFlags |= PRINT3D_DEACTIVATE_WIN;
	if (!sTitle)	dwFlags |= PRINT3D_DEACTIVATE_TITLE;

	/* Create window */
	dwActualWin = OGLTPrint3DCreateWindow(nXSize_LettersPerLine, (sTitle==NULL) ? 1 : 50);

	/* Set window properties */
	OGLTPrint3DSetWindow(dwActualWin, dwBodyBackgroundColor, dwBodyTextColor, 0.5f, fPosX, fPosY, 20.0f, 20.0f);
	
	/* Set title */
	if (sTitle)
	{
		OGLTPrint3DSetTitle(dwActualWin, OPENGL_RGBA(0x20, 0x20, 0xB0, 0xE0), 0.6f, OPENGL_RGBA(0xFF, 0xFF, 0x30, 0xFF), 
							sTitle, OPENGL_RGBA(0xFF, 0xFF, 0x30, 0xFF), "");
	}
	/* Set window text */
	if (sBody)
	{
		OGLTPrint3DWindowOutput(dwActualWin, sBody);
	}

	/* Set window flags */
	OGLTPrint3DSetWindowFlags(dwActualWin, dwFlags);

	/* Return window handle */
	return dwActualWin;
}
/*----------------------------------------------------------------------
 *  Function Name   : OGLTPrint3DSetAspectRatio							 
 *  Inputs          :                                                     
 *  Outputs         : *dwScreenX, *dwScreenY                                                  
 *  Returns         :                                       
 *  Globals Used    :                                                   
 *  Description     : Gets the current aspect ratio/resolution.
 *----------------------------------------------------------------------*/
void OGLTPrint3DGetResolution(DWORD *dwScreenX, DWORD *dwScreenY)
{
	*dwScreenX = (int)(640.0f * Glob.fScreenScale[0]);
	*dwScreenY = (int)(480.0f * Glob.fScreenScale[1]);
}

/*----------------------------------------------------------------------
 *  Function Name   : OGLTPrint3DSetTextures							 
 *  Inputs          : dwScreenX, dwScreenY                                                    
 *  Outputs         : None                                              
 *  Returns         : TRUE if OK.                                       
 *  Globals Used    : Glob                                                  
 *  Description     : Initialization and texture upload.
 *----------------------------------------------------------------------*/
void OGLTPrint3DSetResolution(DWORD dwScreenX, DWORD dwScreenY)
{
	Glob.fScreenScale[0] = (float)dwScreenX/640.0f;
	Glob.fScreenScale[1] = (float)dwScreenY/480.0f;
}
/*----------------------------------------------------------------------
 *  Function Name   : OGLTPrint3DGetLength                                
 *  Inputs          :                                
 *  Outputs         :                                                   
 *  Returns         :                                                   
 *  Globals Used    :			                                        
 *  Description     : Calculates the size in pixels.                                    
 *----------------------------------------------------------------------*/
float OGLTPrint3DGetLength (float fFontSize, BYTE *sString)
{
BYTE Val;
float fScale, fSize;
int i = 0;

		if(sString==NULL) return 0.0f;

		if (fFontSize > 0.0f) /* Arial font */
		{
			fScale = fFontSize;
			fSize  = 0.0f;

			while (Val = sString[i++])
			{
				if (Val==' ') Val = '0';
				if(Val>='0' && Val <= '9') Val = '0'; /* That's for fixing the number width */
				fSize += OGLTPrint3DSize_Bold[Val] * 40.0f * fScale ;

				/* these letters are too narrow due to a bug in the table */
				//if(Val=='i' || Val == 'l' || Val == 'j') fSize += 0.4f* fScale; 

			}
		}
		else /* System font */
		{
			fScale = 255.0f;
			fSize  = 0.0f;

			while (Val = sString[i++])
			{
				if (Val==' ') {fSize += 5.0f; continue;}
				if(Val>='0' && Val <= '9') Val = '0'; /* That's for fixing the number width */
				fSize += OGLTPrint3DSize_System[Val]  * fScale;
			}
		}

		return (fSize);
}

/**************************************************************
**					 INTERNAL FUNCTIONS						 **
**************************************************************/


/*----------------------------------------------------------------------
 *  Function Name   : DrawWindow	                                    
 *  Inputs          :                       							 
 *  Returns         : TRUE if succesful, FALSE otherwise.               
 *  Globals Used    :                                                   
 *  Description     : Draw a generic rectangle (with or without border).	                                    
 *----------------------------------------------------------------------*/
HRESULT DrawWindow(DWORD dwWin, DWORD Color, float fZPos, float fPosX, float fPosY, float fSizeX, float fSizeY)
{
	register	i;
	OGLTLVERTEX	vBox[16];
	BOOL		bIsOp = FALSE;
	float		fU[] = { 0.0f, 0.0f, 6.0f, 6.0f, 10.0f,10.0f, 16.0f,16.0f,10.0f,16.0f,10.0f,16.0f,6.0f,6.0f,0.0f,0.0f};
	float		fV[] = { 0.0f, 6.0f, 0.0f, 6.0f, 0.0f, 6.0f, 0.0f, 6.0f, 10.0f, 10.0f, 16.0f,16.0f, 16.0f, 10.0f, 16.0f, 10.0f};
	WORD		pFaces[] = { 0,1,2, 2,1,3, 2,3,4, 4,3,5, 4,5,6, 6,5,7, 5,8,7, 7,8,9, 8,10,9, 9,10,11, 8,12,10, 8,13,12, 
							 13,14,12, 13,15,14, 13,3,15, 1,15,3, 3,13,5, 5,13,8};

	/* No textures! so... no window */
	if (!Glob.bInitOK) return FALSE;

	/* Full translucent, no window */
    if (!(Color & 0xFF000000)) 
	{
		return FALSE;
	}

	/* Removing the border */
	fSizeX -= 16.0f ;
	fSizeY -= 16.0f ;

	/* Set Z position, color and texture coordinates in array */
    for (i=0; i<16; i++)
	{
		vBox[i].sz =	fZPos;
		vBox[i].color = Color;
		vBox[i].tu =	fU[i]/16.0f;
		vBox[i].tv =	1.0f - fV[i]/16.0f;

	}

	/* Set coordinates in array */
	vBox[0].sx = (fPosX + fU[0]) * Glob.fScreenScale[0];
	vBox[0].sy = (fPosY + fV[0]) * Glob.fScreenScale[1];

	vBox[1].sx = (fPosX + fU[1]) * Glob.fScreenScale[0];
	vBox[1].sy = (fPosY + fV[1]) * Glob.fScreenScale[1];

	vBox[2].sx = (fPosX + fU[2]) * Glob.fScreenScale[0];
	vBox[2].sy = (fPosY + fV[2]) * Glob.fScreenScale[1];

	vBox[3].sx = (fPosX + fU[3]) * Glob.fScreenScale[0];
	vBox[3].sy = (fPosY + fV[3]) * Glob.fScreenScale[1];

	vBox[4].sx = (fPosX + fU[4] + fSizeX) * Glob.fScreenScale[0];
	vBox[4].sy = (fPosY + fV[4]) * Glob.fScreenScale[1];

	vBox[5].sx = (fPosX + fU[5] + fSizeX) * Glob.fScreenScale[0];
	vBox[5].sy = (fPosY + fV[5]) * Glob.fScreenScale[1];

	vBox[6].sx = (fPosX + fU[6] + fSizeX) * Glob.fScreenScale[0];
	vBox[6].sy = (fPosY + fV[6]) * Glob.fScreenScale[1];

	vBox[7].sx = (fPosX + fU[7] + fSizeX) * Glob.fScreenScale[0];
	vBox[7].sy = (fPosY + fV[7]) * Glob.fScreenScale[1];

	vBox[8].sx = (fPosX + fU[8] + fSizeX) * Glob.fScreenScale[0];
	vBox[8].sy = (fPosY + fV[8] + fSizeY) * Glob.fScreenScale[1];

	vBox[9].sx = (fPosX + fU[9] + fSizeX) * Glob.fScreenScale[0];
	vBox[9].sy = (fPosY + fV[9] + fSizeY) * Glob.fScreenScale[1];

	vBox[10].sx = (fPosX + fU[10] + fSizeX) * Glob.fScreenScale[0];
	vBox[10].sy = (fPosY + fV[10] + fSizeY) * Glob.fScreenScale[1];

	vBox[11].sx = (fPosX + fU[11] + fSizeX) * Glob.fScreenScale[0];
	vBox[11].sy = (fPosY + fV[11] + fSizeY) * Glob.fScreenScale[1];

	vBox[12].sx = (fPosX + fU[12]) * Glob.fScreenScale[0];
	vBox[12].sy = (fPosY + fV[12] + fSizeY) * Glob.fScreenScale[1];

	vBox[13].sx = (fPosX + fU[13]) * Glob.fScreenScale[0];
	vBox[13].sy = (fPosY + fV[13] + fSizeY) * Glob.fScreenScale[1];

	vBox[14].sx = (fPosX + fU[14]) * Glob.fScreenScale[0];
	vBox[14].sy = (fPosY + fV[14] + fSizeY) * Glob.fScreenScale[1];

	vBox[15].sx = (fPosX + fU[15]) * Glob.fScreenScale[0];
	vBox[15].sy = (fPosY + fV[15] + fSizeY) * Glob.fScreenScale[1];

	/* Is window opaque ? */
    bIsOp = (GlobWin[dwWin].dwFlags & Print3D_FULL_OPAQUE) ? TRUE : FALSE;

	/* Set texture */
	if (GlobWin[dwWin].dwFlags & Print3D_NO_BORDER)
	{
		glBindTexture(GL_TEXTURE_2D, Glob.uTexture[2 + (bIsOp*2)]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, Glob.uTexture[1 + (bIsOp*2)]);
	}
	
	/* Is window opaque ? */
	if (bIsOp)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		/* Set blending properties */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	/* Set pointers */
	glVertexPointer(3,		GL_FLOAT,			sizeof(OGLTLVERTEX), &vBox[0].sx);
	glColorPointer(4,		GL_UNSIGNED_BYTE,	sizeof(OGLTLVERTEX), &vBox[0].color);
	glTexCoordPointer(2,	GL_FLOAT,			sizeof(OGLTLVERTEX), &vBox[0].tu);

	/* Draw primitive */
	glDrawElements(GL_TRIANGLES, 18*3, GL_UNSIGNED_SHORT, pFaces);
	if (glGetError())
	{
		OutputDebugString("glDrawElements(GL_TRIANGLES, 18*3, GL_UNSIGNED_SHORT, pFaces); failed\n");
	}

	/* No problem occured */
	return TRUE;
}


/*----------------------------------------------------------------------
 *  Function Name   : UpLoad4444	                                    
 *  Inputs          :                       							 
 *  Returns         : TRUE if succesful, FALSE otherwise.               
 *  Globals Used    :                                                   
 *  Description     : Reads texture data from *.dat and loads it in     
 *					  video memory.	                                    
 *----------------------------------------------------------------------*/
static BOOL UpLoad4444(DWORD dwTexID, unsigned char *pSource, DWORD nSize, DWORD nMode)
{
	DWORD			r=0, g=0, b=0, a=0, m=0;
	DWORD			dwMemoryUsedByTexture=0;
	unsigned		ContBMP=0;
	int				i, j;
	int				x=256, y=256;
	BYTE			R, G, B, A;
	unsigned char	*p8888, *pSrcByte, *pDestByte;
	static BOOL		bInit = TRUE;
	
	/* First time initialisation */
	if (bInit)
	{
		/* Find valid texture names */
		glGenTextures(5, Glob.uTexture);
		bInit = FALSE;
	}

	/* Only square textures */
	x = nSize;
	y = nSize;
	
	/* Load texture from data */
	
	/* Format is 4444-packed, expand it into 8888 */
	if (nMode==0)
	{
		/* Allocate temporary memory */
		p8888 = (unsigned char *)malloc(nSize*nSize*4*sizeof(unsigned char));
		pDestByte = p8888;

		/* Set source pointer (after offset of 16) */
		pSrcByte = &pSource[16];

		/* Transfer data */
		for (i=0; i<y; i++)
		{
			for (j=0; j<x; j++) 
			{ 
				/* Get all 4 colour channels (invert A) */
				G =   (*pSrcByte) & 0xF0;
				R = ( (*pSrcByte++) & 0x0F ) << 4;
				A =   (*pSrcByte) ^ 0xF0;
				B = ( (*pSrcByte++) & 0x0F ) << 4;

				/* Set them in 8888 data */
				*pDestByte++ = R;
				*pDestByte++ = G;
				*pDestByte++ = B;
				*pDestByte++ = A;
			}
		}
	}
	else
	{
		/* Set source pointer */
		pSrcByte = pSource;

		/* Allocate temporary memory */
		p8888 = (unsigned char *)malloc(nSize*nSize*4*sizeof(unsigned char));
		if (!p8888) 
		{
			OutputDebugString("Not enough memory!\n");
			return FALSE;
		}

		/* Set destination pointer */
		pDestByte = p8888;

		/* Transfer data */
		for (i=0; i<y; i++)
		{
			for (j=0; j<x; j++) 
			{ 
				/* Get alpha channel */
				A = *pSrcByte++;

				/* Set them in 8888 data */
				*pDestByte++ = 255;
				*pDestByte++ = 255;
				*pDestByte++ = 255;
				*pDestByte++ = A;
			}
		}
	}

	/* Bind texture */
	glBindTexture(GL_TEXTURE_2D, Glob.uTexture[dwTexID]);

	/* Default settings: bilinear */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/* Now load texture */
	glTexImage2D(GL_TEXTURE_2D, 0, 4, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, p8888);
	if (glGetError())
	{
		OutputDebugString("glTexImage2D() failed\n");
		free(p8888);
		return FALSE;
	}

	/* Destroy temporary data */
	free(p8888);

	/* Return status : OK */
    return TRUE;
}


/*----------------------------------------------------------------------
 *  Function Name   : OGLTPrint3DPrint3D                                
 *  Inputs          : PosX PosY Size Text                                
 *  Outputs         :                                                   
 *  Returns         :                                                   
 *  Globals Used    :			                                        
 *  Description     : Displays text.                                    
 *----------------------------------------------------------------------*/
void LocalPrint3D(DWORD dwWin, float fZPos, float XPos, float YPos, float fScale, 
				  int Colour, const TCHAR *Text)
{
	unsigned	i=0, Cont=0, VertexCount=0;
	unsigned	Val;
	float		XSize = 0.0f, XFixBug,	YSize, TempSize;
	float		UPos,	VPos;
	float		USize,	VSize;
	int			StripL[] = {2};
	float		fWinClipX[2],fWinClipY[2];
	float		fScaleX, fScaleY, fPreXPos;

	if (!Glob.bInitOK) return;

	if (fScale>0)
	{
		fScaleX = Glob.fScreenScale[0] * fScale * 255.0f;
		fScaleY = Glob.fScreenScale[1] * fScale * 27.0f;
	}
	else
	{
		fScaleX = Glob.fScreenScale[0] * 255.0f;
		fScaleY = Glob.fScreenScale[1] * 12.0f;
	}


	XPos *= Glob.fScreenScale[0];
	YPos *= Glob.fScreenScale[1];

	fPreXPos = XPos;
	
	if (dwWin<0 || dwWin >MAX_WINDOWS) return;
	if (!(GlobWin[dwWin].dwFlags & Print3D_WIN_EXIST) && dwWin) return;

	/* Calculating our margins */
	if (dwWin)
	{
		fWinClipX[0] = (GlobWin[dwWin].fWinPos[0] + 6.0f) * Glob.fScreenScale[0];
		fWinClipX[1] = (GlobWin[dwWin].fWinPos[0] + GlobWin[dwWin].fWinSize[0] - 6.0f) * Glob.fScreenScale[0];

		fWinClipY[0] = (GlobWin[dwWin].fWinPos[1] + 6.0f) * Glob.fScreenScale[1];
		fWinClipY[1] = (GlobWin[dwWin].fWinPos[1] + GlobWin[dwWin].fWinSize[1]  + 9.0f) * Glob.fScreenScale[1];

		if(GlobWin[dwWin].dwFlags & Print3D_WIN_TITLE)
		{
			if (GlobWin[dwWin].fTitleFontSize>0)
			{
				fWinClipY[0] +=  GlobWin[dwWin].fTitleFontSize * 25.0f  * Glob.fScreenScale[1];
				fWinClipY[1] +=  GlobWin[dwWin].fTitleFontSize * 25.0f *  Glob.fScreenScale[1];
			}
			else
			{
				fWinClipY[0] +=  10.0f * Glob.fScreenScale[1];
				fWinClipY[1] +=  8.0f  * Glob.fScreenScale[1];
			}
		}
	}

	while (TRUE)
	{
		Val = (int)*(Text+i++);

		/* End of the string */
		if (Val==0 || i>MAX_LETTERS) break;

		/* It is SPACE so don't draw and carry on... */
		if (Val==' ') 
		{
			if (fScale>0)	XPos += 10.0f/255.0f * fScaleX; 
				else		XPos += 5.0f * Glob.fScreenScale[0]; 
			continue;
		}

		/* It is RETURN so jump a line */
		if (Val==0x0A) 
		{
			XPos = fPreXPos - XSize; 
			YPos += YSize; 
			continue;
		}

		/* If fScale is negative then select the small set of letters (System) */
		if (fScale < 0.0f)
		{
			XPos    += XSize;
			UPos    =  OGLTPrint3DU_System[Val];
			VPos    =  OGLTPrint3DV_System[Val] - 0.0001f; /* Some cards need this little bit */
			YSize   =  fScaleY;
			XSize   =  OGLTPrint3DSize_System[Val] * fScaleX;
			USize	=  OGLTPrint3DSize_System[Val];
			VSize	=  12.0f/255.0f;
		}
		else /* Big set of letters (Bold) */
		{
			XPos    += XSize;
			UPos    =  OGLTPrint3DU_Bold[Val];
			VPos    =  OGLTPrint3DV_Bold[Val] - 1.0f/230.0f;
			YSize   =  fScaleY;
			XSize   =  OGLTPrint3DSize_Bold[Val] * fScaleX;
			USize	=  OGLTPrint3DSize_Bold[Val];
			VSize	=  29.0f/255.0f;	
		}

		/* CLIPPING */
		XFixBug = XSize;

		if (dwWin) /* for dwWin==0 (screen) no clipping */
		{
			/* Outside */
			if (XPos>fWinClipX[1]  ||  (YPos)>fWinClipY[1])
			{
				continue;
			}

			/* Clip X */
			if (XPos<fWinClipX[1] && XPos+XSize > fWinClipX[1])
			{
				TempSize = XSize;

				XSize = fWinClipX[1] - XPos;

				if (fScale < 0.0f)	USize	=  OGLTPrint3DSize_System[Val] * (XSize/TempSize);
				else				USize	=  OGLTPrint3DSize_Bold[Val] * (XSize/TempSize);
			}

			/* Clip Y */
			if (YPos<fWinClipY[1] && YPos+YSize > fWinClipY[1])
			{
				TempSize = YSize;
				YSize = fWinClipY[1] - YPos;

				if (fScale < 0.0f) 	VSize	=  (YSize/TempSize)*12.0f/255.0f;
				else				VSize	=  (YSize/TempSize)*28.0f/255.0f;
			}
		}

		/* Filling vertex data */
		(Glob.pVerticesFont+VertexCount+0)->sx	= XPos;
		(Glob.pVerticesFont+VertexCount+0)->sy	= YPos;
		(Glob.pVerticesFont+VertexCount+0)->sz	= fZPos;
		(Glob.pVerticesFont+VertexCount+0)->tu	= UPos;
		(Glob.pVerticesFont+VertexCount+0)->tv	= VPos;

		(Glob.pVerticesFont+VertexCount+1)->sx	= XPos+XSize;
		(Glob.pVerticesFont+VertexCount+1)->sy	= YPos;
		(Glob.pVerticesFont+VertexCount+1)->sz	= fZPos;
		(Glob.pVerticesFont+VertexCount+1)->tu	= UPos+USize;
		(Glob.pVerticesFont+VertexCount+1)->tv	= VPos;
  
		(Glob.pVerticesFont+VertexCount+2)->sx	= XPos;
		(Glob.pVerticesFont+VertexCount+2)->sy	= YPos+YSize;
		(Glob.pVerticesFont+VertexCount+2)->sz	= fZPos;
		(Glob.pVerticesFont+VertexCount+2)->tu	= UPos;
		(Glob.pVerticesFont+VertexCount+2)->tv	= VPos-VSize;

		(Glob.pVerticesFont+VertexCount+3)->sx	= XPos+XSize;
		(Glob.pVerticesFont+VertexCount+3)->sy	= YPos+YSize;
		(Glob.pVerticesFont+VertexCount+3)->sz	= fZPos;
		(Glob.pVerticesFont+VertexCount+3)->tu	= UPos+USize;
		(Glob.pVerticesFont+VertexCount+3)->tv	= VPos-VSize;

		(Glob.pVerticesFont+VertexCount+0)->color	= Colour;
		(Glob.pVerticesFont+VertexCount+1)->color	= Colour;
		(Glob.pVerticesFont+VertexCount+2)->color	= Colour;
		(Glob.pVerticesFont+VertexCount+3)->color	= Colour;

		VertexCount+=4;

		XSize = XFixBug;

		/* Fix number width */
		if (Val >='0' && Val <='9')
		{
			if (fScale < 0.0f)	XSize = OGLTPrint3DSize_System['0'] * fScaleX;
				else			XSize = OGLTPrint3DSize_Bold['0'] * fScaleX;
		}
	}

	/* Set Font Texture */
	glBindTexture(GL_TEXTURE_2D, Glob.uTexture[0]);

	/* Set blending mode */
	glEnable(GL_BLEND);

	/* Call DrawIndexedPrimitive */
	if (VertexCount)
	{
		/* Set pointers */
		glVertexPointer(3,		GL_FLOAT,			sizeof(OGLTLVERTEX), &Glob.pVerticesFont[0].sx);
		glColorPointer(4,		GL_UNSIGNED_BYTE,	sizeof(OGLTLVERTEX), &Glob.pVerticesFont[0].color);
		glTexCoordPointer(2,	GL_FLOAT,			sizeof(OGLTLVERTEX), &Glob.pVerticesFont[0].tu);

		/* Draw primitive */
		glDrawElements(GL_TRIANGLES, (VertexCount/2)*3, GL_UNSIGNED_SHORT, Glob.pFacesFont);
/*		Val = glGetError();
		if (Val)
		{
			OutputDebugString("glDrawElements(GL_TRIANGLES, (VertexCount/2)*3, GL_UNSIGNED_SHORT, Glob.pFacesFont); failed\n");
		}*/
	}
}


/*----------------------------------------------------------------------
 *  Function Name   : GetLength                                
 *  Inputs          :                                
 *  Outputs         :                                                   
 *  Returns         :                                                   
 *  Globals Used    :			                                        
 *  Description     : calculates the size in pixels.                                    
 *----------------------------------------------------------------------*/
float GetLength (float fFontSize, BYTE *sString)
{
	BYTE Val;
	float fScale, fSize;

	if (fFontSize>=0) /* Arial font */
	{
		fScale = fFontSize * 255.0f;
		fSize  = 0.0f;

		while (Val = *sString++)
		{
			if (Val==' ') {fSize += 10.0f * fFontSize; continue;}
			if(Val>='0' && Val <= '9') Val = '0'; /* That's for fixing the number width */
			fSize += OGLTPrint3DSize_Bold[Val] * fScale ;
		}
	}
	else /* System font */
	{
		fScale = 255.0f;
		fSize  = 0.0f;

		while (Val = *sString++)
		{
			if (Val==' ') {fSize += 5.0f; continue;}
			if(Val>='0' && Val <= '9') Val = '0'; /* That's for fixing the number width */
			fSize += OGLTPrint3DSize_System[Val]  * fScale;
		}
	}

	return (fSize);
}


/*----------------------------------------------------------------------
 *  Function Name	: RenderStates
 *  Inputs			:
 *  Outputs			:
 *  Returns			:
 *  Globals Used	:
 *  Description		: Stores, writes and restores Render States
 *----------------------------------------------------------------------*/
void RenderStates(int nAction)
{
	static GLboolean	bVertexPointerEnabled, bColorPointerEnabled, bTexCoorPointerEnabled;
	static GLint		nMatrixMode;
	GLfloat				fViewport[4], Matrix[16];
	
	/* Saving or restoring states ? */
	switch (nAction)
	{
		case 0:		/*******************************
					** SAVE CURRENT RENDER STATES **
					*******************************/

					/* Save all attributes */
					glPushAttrib(GL_ALL_ATTRIB_BITS);

					/* Client states */
					glGetBooleanv(GL_VERTEX_ARRAY,			&bVertexPointerEnabled); 
					glGetBooleanv(GL_COLOR_ARRAY,			&bColorPointerEnabled); 
					glGetBooleanv(GL_TEXTURE_COORD_ARRAY,	&bTexCoorPointerEnabled); 

					/* Save matrices */
					glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();


					/******************************
					** SET PRINT3D RENDER STATES **
					******************************/
					
					/* Get viewport dimensions */
					glGetFloatv(GL_VIEWPORT, fViewport);

					/* Set matrix with viewport dimensions */
					ZeroMemory(Matrix, sizeof(Matrix));
					Matrix[0] =	2.0f/fViewport[2];
					Matrix[5] =	-2.0f/fViewport[3];
					Matrix[10] = 1.0f;
					Matrix[12] = -1.0f;
					Matrix[13] = 1.0f;
					Matrix[15] = 1.0f;

					/* Set matrix mode so that screen coordinates can be specified */
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glLoadMatrixf(Matrix);

					/* Disable lighting */
					glDisable(GL_LIGHTING);

					/* Enable texture mapping */
					glEnable(GL_TEXTURE_2D);

					/* Set client states */
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_COLOR_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					/* Blending mode */
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					/* Disable fog */
					glDisable(GL_FOG);

					/* Enable smooth shading */
					glShadeModel(GL_SMOOTH);

					/* Set culling properties */
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					glFrontFace(GL_CCW);

					/* Set Z compare properties */
					glDisable(GL_DEPTH_TEST);
					break;

	case 1:			
					/**********************************
					** RESTORE CURRENT RENDER STATES **
					**********************************/
					if (!bVertexPointerEnabled)		glDisableClientState(GL_VERTEX_ARRAY);
					if (!bColorPointerEnabled)		glDisableClientState(GL_COLOR_ARRAY);
					if (!bTexCoorPointerEnabled)	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

					/* Restore matrix mode & matrix */
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
					glMatrixMode(nMatrixMode);

					/* Restore attributes */
					glPopAttrib();
					break;
	}
}
