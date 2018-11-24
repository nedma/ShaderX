//-----------------------------------------------------------------------------
// File: BasicLighting.h
//
// Desc: Header file BasicLighting sample app
//-----------------------------------------------------------------------------
#pragma once


#define MAX_OBJECTS 32
#define MAX_SHADOWS 2
#define MAX_POINT_LIGHTS 15

#define MAX_LIGHTS 32

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\GBuffer")

// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 

    // TODO: change as needed
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;

	BOOL b1Down;
	BOOL b2Down;
	BOOL b3Down;
	BOOL b4Down;
	BOOL b5Down;
	BOOL b6Down;
	BOOL b7Down;

	BOOL qDown;
	BOOL aDown;
	BOOL sDown;
	BOOL xDown;
	BOOL zDown;
	BOOL cDown;

	BOOL num8Down;
	BOOL num2Down;
	BOOL num4Down;
	BOOL num6Down;

	BOOL bTabDown;
};

//-----------------------------------------------------------------------------
// Name: struct Camera
// Desc: 
//-----------------------------------------------------------------------------
struct Camera
{
    D3DXMATRIXA16         m_matView;
    D3DXMATRIXA16         m_matOrientation;
    D3DXVECTOR3        m_vPosition;
    D3DXVECTOR3        m_vVelocity;
    FLOAT              m_fYaw;
    FLOAT              m_fYawVelocity;
    FLOAT              m_fPitch;
    FLOAT              m_fPitchVelocity;
};


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
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
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, LPDIRECT3D9, INT );

	// borrowed from D3D sample Cull
	Camera	m_Camera;
	VOID    UpdateCamera(Camera* pCamera);
	VOID UpdateCullInfo( CULLINFO* pCullInfo );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();
    VOID    ReadSettings();
    VOID    WriteSettings();

	void ApplyFullScreenPixelShader( const unsigned int width, const unsigned int height );
	void CopyTextureToRenderTarget();
	void DisplayGBuffer();
	void LightGBuffer();
	void RenderShadowMap( unsigned int shadowNum );
	HRESULT CreateTestObjects();
	void UpdatePointLights();
	void MatrixInfinitePerspectiveFovLH( D3DXMATRIX* matProj, 
														const float fFov, 
														const float fAspect, 
														const float fZNear );

	class GBuffer*		m_FrameBuffer;
	LPDIRECT3DSURFACE9	m_pBackBuffer;
	LPD3DXEFFECT		m_ShowFatFx;
	class DefMesh*		m_Mesh[MAX_OBJECTS];

	BaseLight*			m_ActiveLights[MAX_LIGHTS];
	LightHemisphere*	m_LightHemisphere;
	LightDirectional*	m_LightDirectional[MAX_SHADOWS];
	LightPoint*			m_LightPoint[MAX_POINT_LIGHTS];

	LPDIRECT3DTEXTURE9	m_pShadowMap[MAX_SHADOWS];
	LPDIRECT3DSURFACE9	m_pShadowMapSurf[MAX_SHADOWS];
	LPDIRECT3DSURFACE9	m_pShadowMapZ;

	LPDIRECT3DTEXTURE9	m_pDotProductFuncs;
	LPDIRECT3DTEXTURE9	m_pMaterialFuncs;
	LPDIRECT3DTEXTURE9	m_pAttenuationFuncs;

	enum
	{
		DFT_DISPLAY_BUFFER0,
		DFT_DISPLAY_BUFFER1,
		DFT_DISPLAY_BUFFER2,
		DFT_DISPLAY_BUFFER3,

		DFT_COMBINE_COLOUR,
		DFT_COMBINE_MONO,

		DFT_COMBINE_WIERD,
	} m_DisplayFatType;

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};


