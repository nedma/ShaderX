//-----------------------------------------------------------------------------
// File: ExampleBase.h
//
// Desc: Header file for example base stuff
//-----------------------------------------------------------------------------
#pragma once

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "d3dx9shader.h"

extern float g_fContrastExponent;

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
const int	C_STEP_SIZE			= 16;
const float	C_AUTO_CAM_TIME		= 999.0f;

//-----------------------------------------------------------------------------
// Vertex format for models
//-----------------------------------------------------------------------------
struct ModelVertex
{
    D3DXVECTOR3 pos;		// vertex position
    D3DXVECTOR3 normal;		// vertex normal
//	DWORD		color;		// vertex color
	float u,v;				// texture coords

};
#define D3DFVF_MODELVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL/*|D3DFVF_DIFFUSE*/|D3DFVF_TEX1)

//-----------------------------------------------------------------------------
// Struct to store the current input state
//-----------------------------------------------------------------------------
struct UserInput
{
	BOOL bToggleScrPos;
	BOOL bToggleModel;
	BOOL bForward;
	BOOL bBackward;
   BOOL bLeft;
   BOOL bRight;
   BOOL bUp;
   BOOL bDown;
};

typedef struct s_BGVertex
{
	float x,y,z,w;
	float s,t;
} BGVertex_t;


//-----------------------------------------------------------------------------
// Name: class CBaseD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the
//       generic functionality needed in all Direct3D samples. CBaseD3DApplication
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CBaseD3DApplication : public CD3DApplication
{
    BOOL						m_bLoadingApp;          // TRUE, if the app is loading
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;	// Vertex declaration
	LPDIRECT3DVERTEXDECLARATION9	m_pModelVertexDeclaration ;	// Vertex declaration

	BOOL					m_bAutoCam;				// Toggle flag for auto cam
	BOOL					m_bScrPos;
	BOOL					m_szKeys[256];

protected:
    CD3DFont*               m_pFont;                // Font for drawing text

    LPDIRECT3DTEXTURE9			m_texRandom;			// Random texture
	LPDIRECT3DTEXTURE9			m_texContrast;			// Contrast texture
	LPDIRECT3DTEXTURE9			m_texPaper;				// Paper texture

    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT PreRender();
    virtual HRESULT Render();
    virtual HRESULT PostRender();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

	virtual HRESULT CreateShaders();
	virtual HRESULT InitializeLights();
	virtual HRESULT SetShaderConstants();
   virtual HRESULT RenderText();

	
	HRESULT RenderBackground();
	HRESULT	CalcMovement();
	HRESULT CalcAutoCam();
	HRESULT SetScrShotPos();
	HRESULT createTexture(int nWidth, int nHeight, LPDIRECT3DTEXTURE9* pTexture);
	HRESULT createRandomTexture();
	HRESULT createContrastMap(float fDensity, float fExp);
	HRESULT loadPaperTexture(char* szFileName);
	HRESULT loadContrastTexture(char* szFileName);
	HRESULT loadXFile(const char* szFileName, DWORD fvf);
	BYTE	getHeight(int x, int y);
    void    UpdateInput( UserInput* pUserInput );

    D3DXMATRIX				m_matView;				// ViewMatrix
    D3DXMATRIX				m_matProj;				// ProjectionMatrix
    D3DXMATRIX				m_matWorld;				// WorldMatrix
    D3DXMATRIX				m_matWorldViewProj;
    D3DXMATRIX				m_matWorldView;
    D3DXMATRIX				m_matWorldViewIT;
	D3DXMATRIX				m_matPosition;
	D3DXVECTOR3		        m_vVelocity;
	D3DXVECTOR3		        m_vAngularVelocity;
	D3DXVECTOR3				m_vecScale;
	float					m_fAutoTime;
	float					m_fAutoCamSpeed;			// Speed factor for auto cam
   BOOL              m_bRotateModel;

	BOOL					m_bCheckPSsupport;		// Enables/Disables pixelshader support check
	LPDIRECT3DVERTEXSHADER9	m_pVertexShader;		// VertexShader
	LPDIRECT3DPIXELSHADER9	m_pPixelShader;			// PixelShader

	// Mesh infos
	float					m_fRadius;
	int						m_nVertices;
	int						m_nFaces;
	DWORD					m_dwFVF;
	DWORD					m_dwNumSections;
	D3DXVECTOR3				m_vecCenter;
	D3DMATERIAL9*			m_pMeshMaterials;
	D3DXATTRIBUTERANGE*		m_pAttributes;
	IDirect3DIndexBuffer9*	m_pIB;
	IDirect3DVertexBuffer9*	m_pVB;
	BGVertex_t m_arBackground[4];
   UserInput               m_UserInput;            // Struct for storing user input


public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CBaseD3DApplication();
    virtual ~CBaseD3DApplication();
};

