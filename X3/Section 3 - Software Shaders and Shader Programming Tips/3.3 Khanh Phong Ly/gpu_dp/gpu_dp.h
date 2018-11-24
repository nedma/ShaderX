//-----------------------------------------------------------------------------
// File: gpu_dp.h
//
// Desc: Header file gpu_dp sample app
//-----------------------------------------------------------------------------
#pragma once
#include <fstream>
#include "dpGPU.h"
#include "dpSoft.h"
#include "worldtoscreen.h"
//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\gpu_dp")

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08


// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 
    BYTE diksPrev[256];   // DirectInput keyboard state buffer 

	bool bScaleInc;
	bool bScaleDec;
	bool bItCountInc;
	bool bItCountDec;
	bool bToggleSoft;
	bool bToggleDisplayImage;

//	bool bSetDisp_ActionMap;
//	bool bSetDisp_BoundaryMap;
//	bool bSetDisp_CurrValMap;
//	bool bSetDisp_OrigValMap;

	bool bRestartIterations;

	POINT                 m_ptLastMousePosition;  // Last absolute postion of mouse cursor
	bool                  m_bMouseLButtonDown;    // True if left button is down 
	bool                  m_bMouseMButtonDown;    // True if middle button is down 
	bool                  m_bMouseRButtonDown;    // True if right button is down 
	int                   m_nCurrentButtonMask;   // mask of which buttons are down

	bool bValueModInc;
	bool bValueModDec;
	bool bAddValueModToMap;
	bool bSubValueModToMap;
};



class CParticleSystem;



//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
//    LPDIRECT3DVERTEXBUFFER9 m_pVB;                  // for data iteration
	LPDIRECT3DVERTEXBUFFER9 m_pVB2;                  // to display results.
	LPDIRECT3DVERTEXBUFFER9 m_pVB3;                  // to display results.

    CD3DFont*               m_pFont;                // Font for drawing text

    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();
	HRESULT Pick();


	DPGPU				m_DPGPU;
	DPSoft				m_DPSoft;
	DPGPU_Params		m_DPGPU_Params; // intialised to communicate with DP classes
	DPSoft_Params		m_DPSoft_Params;

	typedef enum
	{
		SHOW_ORIG_VAL_MAP=0,
		SHOW_BOUNDARY_MAP,
		SHOW_CURR_VAL_MAP,
		SHOW_ACTION_MAP,
		//SHOW_ORIG_IMAGE,
		//SHOW_CURR_IMAGE,
		SHOW_MAX,
	} MAP_SHOW;
	MAP_SHOW		m_MapShow;
	bool			m_bSoft;		// flat to calc in software 
	int				m_ItCount;

	LARGE_INTEGER	m_countspersec;
	LARGE_INTEGER	m_perfcounterprev;
	LARGE_INTEGER	m_perfcounter;

	LPDIRECT3DPIXELSHADER9	m_pDefaultPixelShader;
	LPDIRECT3DPIXELSHADER9	m_pActionMapDispPixelShader;
	LPDIRECT3DPIXELSHADER9	m_pLogScalerPixelShader;
	LPDIRECT3DSURFACE9		m_pBackBufChain;

	WorldScreenTransform	m_WorldScreenTransform;
	CParticleSystem			*m_pParticleSystem;

	int					m_TotalIterations;
	float				m_ModValue;
	float				m_ModValueDelta;

	float					m_CurrValScale;
	float					m_OrigValScale;


	

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

