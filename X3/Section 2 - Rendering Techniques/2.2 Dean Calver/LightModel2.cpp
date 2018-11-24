//-----------------------------------------------------------------------------
// File: LightModel2.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"

#include "GlobalMatrices.h"
#include "GlobalObjects.h"
#include "DefMesh.h"
#include "GBuffer.h"
#include "GBufferMRT.h"
#include "GBufferSingle.h"

#include "LightModelTexture.h"

#include "BaseLight.h"
#include "LightHemisphere.h"
#include "LightDirectional.h"
#include "LightPoint.h"

#include "LightModel2.h"

#define FORCE_SOFTWARE_VERTEX_PROCESSING 0
#define FORCE_SOFTWARE_PIXEL_PROCESSING 0

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication*	g_pApp  = NULL;
HINSTANCE			g_hInst = NULL;
GlobalMatrices		g_Matrices;
GlobalObjects		g_Objs;

#define SHADOW_MAP_FORMAT D3DFMT_R32F
#define SHADOW_MAP_SIZE   512

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "LightModel2" );
	m_d3dEnumeration.AppUsesDepthBuffer   = FALSE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	m_FrameBuffer = 0;
	m_pBackBuffer = 0;
	m_ShowFatFx = 0;
	m_DisplayFatType = DFT_COMBINE_COLOUR;
	m_LightHemisphere = 0;
	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		m_pShadowMap[i]	 = NULL;
		m_pShadowMapSurf[i] = NULL;
		m_LightDirectional[i] = NULL;
	}
	m_pShadowMapZ	 = NULL;
	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		m_LightPoint[i] = NULL;
	}

	m_pAttenuationFuncs = NULL;
	m_pDotProductFuncs = NULL;
	m_pMaterialFuncs = NULL;

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		m_Mesh[i] =  NULL;
	}

	for(unsigned int i=0;i < MAX_LIGHTS;i++)
	{
		m_ActiveLights[i] = NULL;
	}


    // Read settings from registry
    ReadSettings();
}

//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	HRESULT hr;

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    // Initialize DirectInput
    InitInput( m_hWnd );

	if( FAILED( hr = D3DXCreateEffectPool( &g_Objs.m_EffectPool) ) )
		return hr;

	if( FAILED( hr = D3DXCreateEffectPool( &g_Objs.m_LightEffectPool) ) )
		return hr;

    UpdateCamera( &m_Camera );

    m_bLoadingApp = FALSE;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ReadSettings()
// Desc: Read the app settings from the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::ReadSettings()
{
    HKEY hkey;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Read the stored window width/height.  This is just an example,
        // of how to use DXUtil_Read*() functions.
        DXUtil_ReadIntRegKey( hkey, TEXT("Width"), &m_dwCreationWidth, m_dwCreationWidth );
        DXUtil_ReadIntRegKey( hkey, TEXT("Height"), &m_dwCreationHeight, m_dwCreationHeight );

        RegCloseKey( hkey );
    }
}

//-----------------------------------------------------------------------------
// Name: WriteSettings()
// Desc: Write the app settings to the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::WriteSettings()
{
    HKEY hkey;

    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Write the window width/height.  This is just an example,
        // of how to use DXUtil_Write*() functions.
        DXUtil_WriteIntRegKey( hkey, TEXT("Width"), m_rcWindowClient.right );
        DXUtil_WriteIntRegKey( hkey, TEXT("Height"), m_rcWindowClient.bottom );

        RegCloseKey( hkey );
    }
}

//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

    // Create a IDirectInput8*
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return DXTRACE_ERR( "DirectInput8Create", hr );
    
    // Create a IDirectInputDevice8* for the keyboard
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return DXTRACE_ERR( "CreateDevice", hr );
    
    // Set the keyboard data format
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return DXTRACE_ERR( "SetDataFormat", hr );
    
    // Set the cooperative level on the keyboard
    if( FAILED( hr = m_pKeyboard->SetCooperativeLevel( hWnd, 
                                            DISCL_NONEXCLUSIVE | 
                                            DISCL_FOREGROUND | 
                                            DISCL_NOWINKEY ) ) )
        return DXTRACE_ERR( "SetCooperativeLevel", hr );

    // Acquire the keyboard
    m_pKeyboard->Acquire();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format, LPDIRECT3D9 pD3D, INT adapterOrdinal )
{  
    BOOL bCapsAcceptable;

	UNREFERENCED_PARAMETER( Format );

	// force software emulation via defines at top of file

    bCapsAcceptable = TRUE;

	D3DDEVTYPE devType = pCaps->DeviceType;

	// high precision format as render target
	if( FAILED( pD3D->CheckDeviceFormat(	adapterOrdinal, 
											devType, 
											/*Format*/ D3DFMT_X8R8G8B8,
											D3DUSAGE_RENDERTARGET, 
											D3DRTYPE_TEXTURE,
											GBufferMRT::m_FatFormat ) ) )
	{
		bCapsAcceptable = FALSE;
	}

	// check single channel 32 bit float for shadow
	if( FAILED( pD3D->CheckDeviceFormat(	adapterOrdinal, 
											devType, 
											/*Format*/ D3DFMT_X8R8G8B8,
											D3DUSAGE_RENDERTARGET, 
											D3DRTYPE_TEXTURE,
											D3DFMT_R32F ) ) )
	{
		bCapsAcceptable = FALSE;
	}

	// check 2 channel 16 bit signed integer 
	if( FAILED( pD3D->CheckDeviceFormat(	adapterOrdinal, 
											devType, 
											/*Format*/ D3DFMT_X8R8G8B8,
											0, 
											D3DRTYPE_TEXTURE,
											D3DFMT_V16U16 ) ) )
	{
		bCapsAcceptable = FALSE;
	}

	// check 4 channel 16 bit unsigned integer 
	if( FAILED( pD3D->CheckDeviceFormat(	adapterOrdinal, 
											devType, 
											/*Format*/ D3DFMT_X8R8G8B8,
											0, 
											D3DRTYPE_TEXTURE,
											D3DFMT_A16B16G16R16 ) ) )
	{
		bCapsAcceptable = FALSE;
	}

	// check 4 channel 16 bit floats 
	if( FAILED( pD3D->CheckDeviceFormat(	adapterOrdinal, 
											devType, 
											/*Format*/ D3DFMT_X8R8G8B8,
											0, 
											D3DRTYPE_TEXTURE,
											D3DFMT_A16B16G16R16F ) ) )
	{
		bCapsAcceptable = FALSE;
	}

#if FORCE_SOFTWARE_VERTEX_PROCESSING == 0
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
		if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
			bCapsAcceptable = FALSE;
    }
#else
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
		bCapsAcceptable = FALSE;
    }
#endif

#if FORCE_SOFTWARE_PIXEL_PROCESSING == 1
	if( pCaps->DevCaps & D3DDEVCAPS_HWRASTERIZATION )
		bCapsAcceptable = FALSE;
#endif

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	// Delete surfaces
	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		SAFE_RELEASE(m_pShadowMapSurf[i]);
		SAFE_RELEASE(m_pShadowMap[i]);
	}
	SAFE_RELEASE(m_pShadowMapZ);

	SAFE_RELEASE( m_pAttenuationFuncs );

	SAFE_RELEASE( m_pDotProductFuncs );

	SAFE_RELEASE( m_pMaterialFuncs );

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		if(m_Mesh[i])
			m_Mesh[i]->Reset();
	}

	if(m_ShowFatFx)
		m_ShowFatFx->OnLostDevice();

	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		if(m_LightPoint[i])
			m_LightPoint[i]->Reset();
	}

	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		if(m_LightDirectional[i])
			m_LightDirectional[i]->Reset();
	}

	if(m_LightHemisphere)
		m_LightHemisphere->Reset();

	SAFE_RELEASE( m_pBackBuffer );
	SAFE_DELETE( m_FrameBuffer );

	// TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		SAFE_DELETE( m_LightPoint[i] );
	}
	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		SAFE_DELETE( m_LightDirectional[i] );
	}
	SAFE_DELETE( m_LightHemisphere );
	SAFE_RELEASE( m_ShowFatFx );

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		SAFE_DELETE( m_Mesh[i] );
	}

	m_pFont->DeleteDeviceObjects();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_RELEASE( g_Objs.m_LightEffectPool );
	SAFE_RELEASE( g_Objs.m_EffectPool );

    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    // Cleanup DirectInput
    CleanupDirectInput();

    // Write the settings to the registry
    WriteSettings();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    // Cleanup DirectX input objects
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;

	g_Objs.m_pD3D = m_pD3D;
	g_Objs.m_pD3DDevice = m_pd3dDevice;

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	if( FAILED( hr = CreateTestObjects() ) )
		return hr;

	// a pixel shader that show/combines each buffer in the fat framebuffer
	if( FAILED( hr = D3DXCreateEffectFromFile(	m_pd3dDevice, 
												TEXT("ShowFat.fx"),
												0, // MACROs
												0, // Include
												0, // flags
												g_Objs.m_LightEffectPool, // Effect Pool
												&m_ShowFatFx,
												0 ) ) )
        return DXTRACE_ERR( "D3DXCreateEffectFromFile ShowFat.fx", hr );
	m_ShowFatFx->SetTechnique("T0");

	unsigned int currentLight = 0;

	m_LightHemisphere = new LightHemisphere;
	m_LightHemisphere->Create();
	m_ActiveLights[currentLight++] = m_LightHemisphere;

	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		m_LightDirectional[i] = new LightDirectional;
		m_LightDirectional[i]->Create();
		m_ActiveLights[currentLight++] = m_LightDirectional[i];

		if( i == 0 )
		{
			m_LightDirectional[0]->m_LightColour = D3DXVECTOR4(1,1,1,0);
			m_LightDirectional[0]->m_LightIntensity = 1.0f;
		} else
		{
			m_LightDirectional[1]->m_LightColour = D3DXVECTOR4(0,1,1,0);
			m_LightDirectional[1]->m_LightIntensity = 0.5f;
		}
	}

	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		m_LightPoint[i] = new LightPoint;
		m_LightPoint[i]->Create();
		
		m_LightPoint[i]->m_LightIntensity = 2.0f;
		m_LightPoint[i]->m_LightID = (unsigned char) i;
		m_ActiveLights[currentLight++] = m_LightPoint[i];
		m_LightPoint[i]->m_ShowVolume = false;
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	D3DCAPS9 d3dCaps;
	m_pd3dDevice->GetDeviceCaps( &d3dCaps );

	// MRT support or not?
	if( d3dCaps.NumSimultaneousRTs == 4 )
	{
		m_FrameBuffer = new GBufferMRT( m_pD3D, m_pd3dDevice );
	} else
	{
		m_FrameBuffer = new GBufferSingle( m_pD3D, m_pd3dDevice );
	}

	// create all the render-targets
	if( m_FrameBuffer->CreateBuffers( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height ) == false )
		return E_FAIL;

	g_Objs.m_Framebuffer = m_FrameBuffer;

	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		// Create shadow map texture and retrieve surface
		if (FAILED(m_pd3dDevice->CreateTexture(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, 
			D3DUSAGE_RENDERTARGET, SHADOW_MAP_FORMAT, D3DPOOL_DEFAULT, &m_pShadowMap[i], NULL)))
			return E_FAIL;
		if (FAILED(m_pShadowMap[i]->GetSurfaceLevel(0, &m_pShadowMapSurf[i])))
			return E_FAIL;
	}

	// Create depth buffer for shadow map rendering
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowMapZ, NULL)))
		return E_FAIL;

	// Create dot product texture
	if (FAILED(m_pd3dDevice->CreateTexture(	DOTP_WIDTH, 
											NUM_DOTP_FUNCTIONS * BILERP_PROTECTION, 1, 0, 
											D3DFMT_V16U16, D3DPOOL_MANAGED, &m_pDotProductFuncs, NULL)))
		return E_FAIL;
	if ( FAILED(D3DXFillTexture(m_pDotProductFuncs,FillDotProductTexture, 0)) )
		return E_FAIL;

	// Create attenuation texture
	if (FAILED(m_pd3dDevice->CreateTexture(	ATTENUATION_WIDTH, 
											NUM_ATTENUATION_FUNCTIONS * BILERP_PROTECTION, 1, 0, 
											D3DFMT_V16U16, D3DPOOL_MANAGED, &m_pAttenuationFuncs, NULL)))
		return E_FAIL;
	if ( FAILED(D3DXFillTexture(m_pAttenuationFuncs,FillAttenuationTexture, 0)) )
		return E_FAIL;


	// fill the material list, all materials should have already been registered
	if (FAILED(m_pd3dDevice->CreateTexture(	NUM_MATERIALS, 1, 
											1, 0, 
											D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED, &m_pMaterialFuncs, NULL)))
		return E_FAIL;
	if ( FAILED(D3DXFillTexture(m_pMaterialFuncs,FillMaterialTexture, 0)) )
		return E_FAIL;


	// set the g-buffer textures into showfat.fx
	m_ShowFatFx->SetTexture( "Buffer0Texture", m_FrameBuffer->GetBufferTexture( FFB_BUFFER0 ) );
	m_ShowFatFx->SetTexture( "Buffer1Texture", m_FrameBuffer->GetBufferTexture( FFB_BUFFER1 ) );
	m_ShowFatFx->SetTexture( "Buffer2Texture", m_FrameBuffer->GetBufferTexture( FFB_BUFFER2 ) );
	m_ShowFatFx->SetTexture( "Buffer3Texture", m_FrameBuffer->GetBufferTexture( FFB_BUFFER3 ) );
	m_ShowFatFx->SetTexture( "GlobalShadowMapTexture", m_pShadowMap[0] );

	m_ShowFatFx->SetFloat( "fRcpFrameWidth", 1.f / m_FrameBuffer->GetWidth() );
	m_ShowFatFx->SetFloat( "fRcpFrameHeight", 1.f / m_FrameBuffer->GetHeight() );

	// set the lightmodel textures
	m_ShowFatFx->SetTexture( "MaterialMapTexture", m_pMaterialFuncs );
	m_ShowFatFx->SetTexture( "DotProductFunctionMapTexture", m_pDotProductFuncs );
	m_ShowFatFx->SetTexture( "AttenuationMapTexture", m_pAttenuationFuncs );

	// store the back buffer surface for later use
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer );

	// create camera
	ZeroMemory( &m_Camera, sizeof(m_Camera ) );
	m_Camera.m_vPosition = D3DXVECTOR3( 2.77f, 2.21f, -8.51f );
	m_Camera.m_fPitch = 0.20678f;
	m_Camera.m_fYaw = -0.25366f;

	// Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateCamera()
// Desc: 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::UpdateCamera(Camera* pCamera)
{
    FLOAT fElapsedTime;

    if( m_fElapsedTime > 0.0f )
        fElapsedTime = m_fElapsedTime;
    else
        fElapsedTime = 0.05f;

    FLOAT fSpeed        = 5.0f*fElapsedTime;
    FLOAT fAngularSpeed = 2.0f*fElapsedTime;

    // De-accelerate the camera movement (for smooth motion)
    pCamera->m_vVelocity      *= 0.75f;
    pCamera->m_fYawVelocity   *= 0.75f;
    pCamera->m_fPitchVelocity *= 0.75f;

    // Process keyboard input
	if( m_UserInput.cDown )    pCamera->m_vVelocity.x    += fSpeed; // Slide Right
    if( m_UserInput.zDown )     pCamera->m_vVelocity.x    -= fSpeed; // Slide Left
    if( m_UserInput.sDown )       pCamera->m_vVelocity.y    += fSpeed; // Slide Up
    if( m_UserInput.xDown )     pCamera->m_vVelocity.y    -= fSpeed; // Slide Down
    if( m_UserInput.num8Down )         pCamera->m_vVelocity.z    += fSpeed; // Move Forward
    if( m_UserInput.num2Down )         pCamera->m_vVelocity.z    -= fSpeed; // Move Backward
    if( m_UserInput.num6Down )         pCamera->m_fYawVelocity   += fSpeed; // Turn Right
    if( m_UserInput.num4Down )         pCamera->m_fYawVelocity   -= fSpeed; // Turn Left
    if( m_UserInput.qDown )         pCamera->m_fPitchVelocity += fSpeed; // Turn Down
    if( m_UserInput.aDown )         pCamera->m_fPitchVelocity -= fSpeed; // Turn Up

    // Update the position vector
    D3DXVECTOR3 vT = pCamera->m_vVelocity * fSpeed;
    D3DXVec3TransformNormal( &vT, &vT, &pCamera->m_matOrientation );
    pCamera->m_vPosition += vT;

    // Update the yaw-pitch-rotation vector
    pCamera->m_fYaw   += fAngularSpeed * pCamera->m_fYawVelocity;
    pCamera->m_fPitch += fAngularSpeed * pCamera->m_fPitchVelocity;
    if( pCamera->m_fPitch < -D3DX_PI/2 ) 
        pCamera->m_fPitch = -D3DX_PI/2;
    if( pCamera->m_fPitch > D3DX_PI/2 ) 
        pCamera->m_fPitch = D3DX_PI/2;

    // Set the view matrix
    D3DXQUATERNION qR;
    D3DXQuaternionRotationYawPitchRoll( &qR, pCamera->m_fYaw, pCamera->m_fPitch, 0.0f );
    D3DXMatrixAffineTransformation( &pCamera->m_matOrientation, 1.25f, NULL, &qR, &pCamera->m_vPosition );
    D3DXMatrixInverse( &pCamera->m_matView, NULL, &pCamera->m_matOrientation );

	const float ZNear = 0.1f;

    // Set the projection matrix.
    D3DXMATRIX matProj;
    D3DXMATRIX matViewProjection;

	FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
	MatrixInfinitePerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, ZNear );
	
	D3DXMatrixMultiply( &matViewProjection, &pCamera->m_matView, &matProj );

	g_Matrices.m_View = pCamera->m_matView;
	g_Matrices.m_ViewProj = matViewProjection;
	g_Matrices.m_Proj = matProj;

	// these shader matrices are shared among all meshes so we just set the first mesh
	if( m_LightHemisphere != 0 )
	{
		// create matrix to takes us from store position into view space position
		D3DXMATRIX matProjectionInvScaled, matScale;
		D3DXMatrixInverse( &matProjectionInvScaled , 0, &matProj );
		D3DXMatrixIdentity( &matScale );
		matScale._11 *= 2;
		matScale._22 *= 2;
		matScale._41 += -1;
		matScale._42 += -1;
		D3DXMatrixMultiply( &matProjectionInvScaled , &matScale, &matProjectionInvScaled );

		// matrix that takes us from G-buffer position to view space position
		m_LightHemisphere->m_Shader->SetMatrix( TEXT("matProjectionInvScaled"), &matProjectionInvScaled );
	}

}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: update world

    // Update user input state
    UpdateInput( &m_UserInput );
    UpdateCamera( &m_Camera );
	UpdateCullInfo( &g_Objs.m_CullInfo );

    // Update the world state according to user input
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );

	if( m_Mesh[0] != 0 )
	{
		m_Mesh[0]->m_WorldMatrix = matWorld;
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    HRESULT hr;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    pUserInput->bRotateLeft  = ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );
    pUserInput->bRotateRight = ( (pUserInput->diks[DIK_RIGHT] & 0x80) == 0x80 );
    pUserInput->bRotateUp    = ( (pUserInput->diks[DIK_UP] & 0x80)    == 0x80 );
    pUserInput->bRotateDown  = ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );

	pUserInput->b1Down  = ( (pUserInput->diks[DIK_1] & 0x80) == 0x80 );
    pUserInput->b2Down	= ( (pUserInput->diks[DIK_2] & 0x80) == 0x80 );
    pUserInput->b3Down  = ( (pUserInput->diks[DIK_3] & 0x80) == 0x80 );
    pUserInput->b4Down  = ( (pUserInput->diks[DIK_4] & 0x80) == 0x80 );
    pUserInput->b5Down	= ( (pUserInput->diks[DIK_5] & 0x80) == 0x80 );
    pUserInput->b6Down  = ( (pUserInput->diks[DIK_6] & 0x80) == 0x80 );
    pUserInput->b7Down  = ( (pUserInput->diks[DIK_7] & 0x80) == 0x80 );

    pUserInput->qDown	= ( (pUserInput->diks[DIK_Q] & 0x80) == 0x80 );
    pUserInput->aDown	= ( (pUserInput->diks[DIK_A] & 0x80) == 0x80 );
    pUserInput->sDown	= ( (pUserInput->diks[DIK_S] & 0x80) == 0x80 );
    pUserInput->xDown	= ( (pUserInput->diks[DIK_X] & 0x80) == 0x80 );
    pUserInput->zDown	= ( (pUserInput->diks[DIK_Z] & 0x80) == 0x80 );
    pUserInput->cDown	= ( (pUserInput->diks[DIK_C] & 0x80) == 0x80 );
    pUserInput->num8Down	= ( (pUserInput->diks[DIK_NUMPAD8] & 0x80) == 0x80 );
    pUserInput->num2Down	= ( (pUserInput->diks[DIK_NUMPAD2] & 0x80) == 0x80 );
    pUserInput->num4Down	= ( (pUserInput->diks[DIK_NUMPAD4] & 0x80) == 0x80 );
    pUserInput->num6Down	= ( (pUserInput->diks[DIK_NUMPAD6] & 0x80) == 0x80 );

    pUserInput->bTabDown  = ( (pUserInput->diks[DIK_TAB] & 0x80) == 0x80 );

}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// render the shadows into the shadow maps
		for(unsigned int i=0;i <MAX_SHADOWS;i++)
		{
			RenderShadowMap(i);
		}

		// render the g-buffers, for MRT this should be a single psas
		for(unsigned int passNum = 0; passNum < m_FrameBuffer->GetRenderPassCount();passNum++ )
		{
			if( m_FrameBuffer->GetType() == MT_MRT )
			{
				// Clear render targets
				m_FrameBuffer->Clear( FFB_BUFFER0 | FFB_BUFFER1 | FFB_BUFFER2 | FFB_BUFFER3 | FFB_DEPTH | FFB_STENCIL);
				m_FrameBuffer->SelectBuffersAsRenderTarget( FFB_BUFFER0 | FFB_BUFFER1 | FFB_BUFFER2 | FFB_BUFFER3 | FFB_DEPTH | FFB_STENCIL );
			} else
			{
				// note passNum = 0 to 3 == FFB_BUFFER0 to FFB_BUFFER3
				if( passNum == 0 )
					m_FrameBuffer->Clear( FFB_BUFFER0 | FFB_DEPTH | FFB_STENCIL);
				else
				{
					m_FrameBuffer->Clear( (1 << passNum) );
				}
				m_FrameBuffer->SelectBuffersAsRenderTarget( (1 << passNum) | FFB_DEPTH | FFB_STENCIL );
			}

			// render the objects
			for(unsigned int i=0;i < MAX_OBJECTS;i++)
			{
				if(m_Mesh[i])
					m_Mesh[i]->Render( passNum );
			}
		}

		// display debugger while tab held down
		if( m_UserInput.bTabDown )
			DisplayGBuffer();
		else
		{
			LightGBuffer();
		}

		// Render stats and help text  
		RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}



void CMyD3DApplication::ApplyFullScreenPixelShader( const unsigned int width, const unsigned int height )
{
	// render pixel shader quad to combine the render target
	struct
	{
		float x,y,z,w;
		float u0,v0;
	} quad[4] = 
	{ 
		{ (float)0,		(float)0,			0.1f,1,	0,0, }, 
		{ (float)width, (float)0,			0.1f,1,	1,0, }, 
		{ (float)0,		(float)height,		0.1f,1,	0,1, }, 
		{ (float)width, (float)height,		0.1f,1,	1,1, } 
	};

	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])  );
}

// assumes render target is already set and that the texture is ready set in stage 0
void CMyD3DApplication::CopyTextureToRenderTarget()
{

	m_pd3dDevice->SetPixelShader( 0 );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_RESULTARG, D3DTA_CURRENT );

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	ApplyFullScreenPixelShader( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );
}

void CMyD3DApplication::DisplayGBuffer()
{
	if( m_UserInput.b1Down )
	{
		m_ShowFatFx->SetTechnique( "T0" );
	} else if( m_UserInput.b2Down )
	{
		m_ShowFatFx->SetTechnique( "T1" );
	} else if( m_UserInput.b3Down )
	{
		m_ShowFatFx->SetTechnique( "T2" );
	} else if( m_UserInput.b4Down )
	{
		m_ShowFatFx->SetTechnique( "T3" );
	} else if( m_UserInput.b5Down )
	{
		m_ShowFatFx->SetTechnique( "T4" );
	} else if( m_UserInput.b6Down )
	{
		m_ShowFatFx->SetTechnique( "T5" );
	} else if( m_UserInput.b7Down )
	{
		m_ShowFatFx->SetTechnique( "T6" );
	}

	// select just the backbuffer as the target
	m_pd3dDevice->SetDepthStencilSurface( 0 );
	m_pd3dDevice->SetRenderTarget( 0, m_pBackBuffer );
	m_pd3dDevice->SetRenderTarget( 1, 0 );
	m_pd3dDevice->SetRenderTarget( 2, 0 );
	m_pd3dDevice->SetRenderTarget( 3, 0 ); 

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );


	unsigned int numPasses;
	m_ShowFatFx->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	for(unsigned int i=0;i < numPasses;i++)
	{
		m_ShowFatFx->Pass(i);
		ApplyFullScreenPixelShader( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );
	}

	m_ShowFatFx->End();
}

void CMyD3DApplication::LightGBuffer()
{
	// select just the backbuffer as the target
	// reuse the depth buffer from the fat framebuffer to enable depth cull optizamations
	m_pd3dDevice->SetDepthStencilSurface( m_FrameBuffer->GetBufferSurface( FFB_DEPTH ) );
	m_pd3dDevice->SetRenderTarget( 0, m_pBackBuffer );
	m_pd3dDevice->SetRenderTarget( 1, 0 );
	m_pd3dDevice->SetRenderTarget( 2, 0 );
	m_pd3dDevice->SetRenderTarget( 3, 0 ); 

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	UpdatePointLights();

	// update all active lights
	for(unsigned int j=0;j < MAX_LIGHTS;j++)
	{
		if( m_ActiveLights[j] != NULL)
			m_ActiveLights[j]->Update();
	}

	// render all active lights
	for(unsigned int j=0;j < MAX_LIGHTS;j++)
	{
		if( m_ActiveLights[j] != NULL)
			m_ActiveLights[j]->Render();
	}

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

//-----------------------------------------------------------------------------
// Name: RenderShadowMap()
// Desc: Called to render shadow map for the light. Outputs per-pixel distance
//       normalized to 0..1 range to the floating point buffer.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RenderShadowMap( unsigned int shadowNum )
{
	D3DVIEWPORT9 oldViewport;

	// Light direction
    // Set the transform matrices
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3(0.0f, 0.0f,  0.0f);
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3(0.0f, 1.0f,  0.0f);

	FLOAT		depthRange = 50.0f;
	D3DXMATRIX	matLightView, matLightProj,matLightViewProj;
	D3DXMATRIX	matView, matInvView, matWarp;

	D3DXVECTOR3 vLightDir;
	if( shadowNum == 0 )
	{
		static float testX = 0.0f;
		testX += 0.01f;

		D3DXMatrixRotationYawPitchRoll( &m_LightDirectional[0]->m_WorldMatrix, testX, -0.1f, 0 );
		const D3DXVECTOR3 zAxis(0,0,1);
		// extract the view direction (faces down the z)
		D3DXVec3TransformNormal( &vLightDir, &zAxis, &m_LightDirectional[0]->m_WorldMatrix );
	} else
	{
		static float testX2 = 0.0f;
		testX2 -= 0.014f;

		D3DXMatrixRotationYawPitchRoll( &m_LightDirectional[1]->m_WorldMatrix, testX2, 0, 0 );
		const D3DXVECTOR3 zAxis(0,0,1);
		// extract the view direction (faces down the z)
		D3DXVec3TransformNormal( &vLightDir, &zAxis, &m_LightDirectional[1]->m_WorldMatrix );
	}

	D3DXVec3Normalize(&vLightDir, &vLightDir);

	// Setup shadow map transform
	vEyePt = vLookatPt + (vLightDir * 15);
    D3DXMatrixLookAtLH(&matLightView, &vEyePt, &vLookatPt, &vUpVec);

	// Projection for directional light
    D3DXMatrixOrthoLH(&matLightProj, 20, 20, 0.0f, depthRange * 1);

	D3DXMatrixMultiply( &matLightViewProj, &matLightView, &matLightProj );

	// we need to move position from eye space to light space and then account for texel offset
	// invViewMat * lightViewMat * matTexAdj;
	// Biblo : A Traditionalist View of 3-D Image Warping, Hansong Zhang
	D3DXMatrixInverse( &matInvView, 0, &g_Matrices.m_View );

	D3DXMatrixMultiply( &matWarp, &matInvView, &matLightViewProj );
		// Texture adjustment matrix
	FLOAT fTexelOffs = (0.5f / SHADOW_MAP_SIZE);
	D3DXMATRIX matTexAdj(0.5f,      0.0f,							0.0f,			0.0f,
						 0.0f,     -0.5f,							0.0f,			0.0f,
						 0.0f,      0.0f,							1.0f,			0.0f,
						 0.5f + fTexelOffs,	0.5f + fTexelOffs,		-0.0065f,		1.0f);

	D3DXMatrixMultiply( &matWarp, &matWarp, &matTexAdj );

	if( shadowNum == 0 )
	{
		m_LightDirectional[0]->m_Shader->SetMatrix( "matShadowWarp", &matWarp );
		m_LightDirectional[0]->m_Shader->SetTexture( "ShadowMapTexture", m_pShadowMap[shadowNum] );
	} else
	{
		m_LightDirectional[1]->m_Shader->SetMatrix( "matShadowWarp", &matWarp );
		m_LightDirectional[1]->m_Shader->SetTexture( "ShadowMapTexture", m_pShadowMap[shadowNum] );
	}

	D3DXVECTOR4 jit0 = D3DXVECTOR4(0.1f / SHADOW_MAP_SIZE, 0.1f / SHADOW_MAP_SIZE, 0, 0 );
	D3DXVECTOR4 jit1 = D3DXVECTOR4(0.1f / SHADOW_MAP_SIZE, 0.9f / SHADOW_MAP_SIZE, 0, 0 );
	D3DXVECTOR4 jit2 = D3DXVECTOR4(0.9f / SHADOW_MAP_SIZE, 0.1f / SHADOW_MAP_SIZE, 0, 0 );
	D3DXVECTOR4 jit3 = D3DXVECTOR4(0.9f / SHADOW_MAP_SIZE, 0.9f / SHADOW_MAP_SIZE, 0, 0 );

	m_LightDirectional[0]->m_Shader->SetVector( TEXT("f4ShadowJitter0"), &jit0 );
	m_LightDirectional[0]->m_Shader->SetVector( TEXT("f4ShadowJitter1"), &jit1 );
	m_LightDirectional[0]->m_Shader->SetVector( TEXT("f4ShadowJitter2"), &jit2 );
	m_LightDirectional[0]->m_Shader->SetVector( TEXT("f4ShadowJitter3"), &jit3 );

	g_Matrices.m_ShadowView = matLightView;
	g_Matrices.m_ShadowViewProj = matLightViewProj;

	// Save old viewport
	m_pd3dDevice->GetViewport(&oldViewport);

	// Set new render target
	m_pd3dDevice->SetRenderTarget(0, m_pShadowMapSurf[shadowNum] );
	m_pd3dDevice->SetDepthStencilSurface(m_pShadowMapZ);

	// Setup shadow map viewport
	D3DVIEWPORT9 shadowViewport;
	shadowViewport.X = 0;
	shadowViewport.Y = 0;
	shadowViewport.Width  = SHADOW_MAP_SIZE;
	shadowViewport.Height = SHADOW_MAP_SIZE;
	shadowViewport.MinZ = 0.0f;
	shadowViewport.MaxZ = 1.0f;
	m_pd3dDevice->SetViewport(&shadowViewport);

	// Clear viewport
    m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0L);

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		if(m_Mesh[i] && m_Mesh[i]->m_CastShadow)
			m_Mesh[i]->RenderShadow();
	}

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Restore old viewport
	m_pd3dDevice->SetViewport(&oldViewport);
}

HRESULT CMyD3DApplication::CreateTestObjects()
{
	D3DXMATRIX m1,m2;
	HRESULT hr;
	unsigned int matNum;
	D3DXVECTOR4 matVec(0,0,0,0);
	D3DXVECTOR4 dpfVec(0,0,0,0);
	D3DXVECTOR4 col;

	// reset the material list
	EmptyMaterialList();

	// arrow keyed controllable teapot in the center of the screen
	m_Mesh[0] = new DefMesh;
	if( FAILED( hr = m_Mesh[0]->CreateBuiltIn(	DefMesh::BT_TEAPOT, 
												TEXT("GeomTextured.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomTextured.fx", hr );
	matNum = RegisterMaterial( Material(0.2f, 0.0f, 0.2f ) );
	matVec = D3DXVECTOR4(0.7f,1.0f, MaterialIndexToV(matNum),0 );
	m_Mesh[0]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW128 ), 0, 0 );
	m_Mesh[0]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	LPDIRECT3DTEXTURE9 difTex = 0;
	D3DXCreateTextureFromFile( m_pd3dDevice, "diffuse.png", &difTex );
	m_Mesh[0]->m_Shader->SetTexture( "DiffuseMapTexture", difTex );
	SAFE_RELEASE( difTex );

	LPDIRECT3DTEXTURE9 specTex = 0;
	D3DXCreateTextureFromFile( m_pd3dDevice, "diffuse.png", &specTex );
	m_Mesh[0]->m_Shader->SetTexture( "SpecularMapTexture", specTex );
	SAFE_RELEASE( specTex );

	LPDIRECT3DTEXTURE9 normTex0 = 0;
	LPDIRECT3DTEXTURE9 normTex1 = 0;

	// load it twice so the second one is set same format, size etc
	D3DXCreateTextureFromFile( m_pd3dDevice, "bumps.png", &normTex0 );
	D3DXCreateTextureFromFile( m_pd3dDevice, "bumps.png", &normTex1 );

	D3DXComputeNormalMap( normTex0, normTex1, 0, 0, D3DX_CHANNEL_LUMINANCE, 100 );
	m_Mesh[0]->m_Shader->SetTexture( "NormalMapTexture", normTex0 );

	SAFE_RELEASE( normTex0 );
	SAFE_RELEASE( normTex1 );

	// floor
	m_Mesh[1] = new DefMesh;
	if( FAILED( hr = m_Mesh[1]->CreateBuiltIn(	DefMesh::BT_CUBE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 100, 0.1f, 100);
	D3DXMatrixTranslation( &m2, 0, -1, 0 );
	D3DXMatrixMultiply( &m_Mesh[1]->m_WorldMatrix, &m1,&m2);
	m_Mesh[1]->m_CastShadow = false; // self shadowing problems so as the don't shadow anything easier then tweaking

	matNum = MATERIAL_FULL_AMBIENT; // used an already created material
	matVec = D3DXVECTOR4(0.5f,1.0f, MaterialIndexToV(matNum),0 );
	m_Mesh[1]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW32), 0, 0 );
	m_Mesh[1]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	// back wall
	m_Mesh[2] = new DefMesh;
	if( FAILED( hr = m_Mesh[2]->CreateBuiltIn(	DefMesh::BT_CUBE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 100, 100, 0.1f );
	D3DXMatrixTranslation( &m2, 0, 0, 15 );
	D3DXMatrixMultiply( &m_Mesh[2]->m_WorldMatrix, &m1,&m2);
	m_Mesh[2]->m_CastShadow = false; // self shadowing problems so as the don't shadow anything easy then tweaking

	matNum = MATERIAL_FULL_AMBIENT; // used an already created material
	matVec = D3DXVECTOR4(0.5f,1.0f, MaterialIndexToV(matNum),0 );
	m_Mesh[2]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4( GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU(DPF_FRONT_POW32), 0, 0 );
	m_Mesh[2]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	// shadow casting ball
	m_Mesh[3] = new DefMesh;
	if( FAILED( hr = m_Mesh[3]->CreateBuiltIn(	DefMesh::BT_SPHERE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 1,1, 1);
	D3DXMatrixTranslation( &m2, 3, 0.0f, 1 );
	D3DXMatrixMultiply( &m_Mesh[3]->m_WorldMatrix, &m1,&m2);
	// create a oren-nayer style surface with full ambient
	matNum = MATERIAL_FULL_AMBIENT; // used an already created material
	matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 );
	m_Mesh[3]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW4 ), 
							GetDotProductFunctionU( DPF_ZERO), 0, 0 );
	m_Mesh[3]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	// teapot at the back
	m_Mesh[4] = new DefMesh;
	if( FAILED( hr = m_Mesh[4]->CreateBuiltIn(	DefMesh::BT_TEAPOT, TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 1, 1, 1);
	D3DXMatrixTranslation( &m2, -3, 0.0f, -1 );
	D3DXMatrixMultiply( &m_Mesh[4]->m_WorldMatrix, &m1,&m2);

	// create a blinn/phong plastic specular material with low specular exponent
	matNum = RegisterMaterial( Material(0.3f, 0.2f, 1.0f ) );
	matVec = D3DXVECTOR4(0.7f,0.8f, MaterialIndexToV(matNum),0 );
	m_Mesh[4]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW16 ), 0, 0 );
	m_Mesh[4]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );
	col = D3DXVECTOR4(1,1,1,1);
	m_Mesh[4]->m_Shader->SetVector( "f3Colour0", &col );

	// donut
	m_Mesh[5] = new DefMesh;
	if( FAILED( hr = m_Mesh[5]->CreateBuiltIn(	DefMesh::BT_TORUS, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 2, 2, 2);
	D3DXMatrixTranslation( &m2, 1, 0.0f, 5 );
	D3DXMatrixMultiply( &m_Mesh[5]->m_WorldMatrix, &m1,&m2);

	// normal specular 
	col = D3DXVECTOR4(1,1,1,1);
	matNum = RegisterMaterial( Material(0.3f, 0.f, 1.0f ) ); // KAmb=0.3, KEmm=0.f, Phong hilight 
	matVec = D3DXVECTOR4(0.3f,1.0f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
	m_Mesh[5]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1 ), 
							GetDotProductFunctionU( DPF_FRONT_POW256 ), 0, 0 );
	m_Mesh[5]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=POW1, Fb=POW256
	m_Mesh[5]->m_Shader->SetVector( "f3Colour0", &col );

	// ellisoid
	m_Mesh[6] = new DefMesh;
	if( FAILED( hr = m_Mesh[6]->CreateBuiltIn(	DefMesh::BT_SPHERE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 1,3, 1);
	D3DXMatrixTranslation( &m2, 2, 0.0f, 1 );
	D3DXMatrixMultiply( &m_Mesh[6]->m_WorldMatrix, &m1,&m2);
	col = D3DXVECTOR4(1,0,0,1);
	m_Mesh[6]->m_Shader->SetVector( "f3Colour0", &col );
	m_Mesh[6]->m_Shader->SetVector( "f3Colour1", &col );
	// weird
	matNum = RegisterMaterial( Material(0.3f, 0.3f, 0.7f ) ); // KAmb=0.3, KEmm=0.3f, bit of both hilight!
	matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
	m_Mesh[6]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW2 ), 
							GetDotProductFunctionU( DPF_FRONT_POW64 ), 0, 0 );
	m_Mesh[6]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=POW16, Fb=POW64

	m_Mesh[7] = new DefMesh;
	if( FAILED( hr = m_Mesh[7]->CreateBuiltIn(	DefMesh::BT_SPHERE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 2,1, 1);
	D3DXMatrixTranslation( &m2, -2, 0, 3 );
	D3DXMatrixMultiply( &m_Mesh[7]->m_WorldMatrix, &m1,&m2);
	col = D3DXVECTOR4(1,1,1,1);
	m_Mesh[7]->m_Shader->SetVector( "f3Colour0", &col );
	// very weird
	matNum = RegisterMaterial( Material(1.3f, 0.3f, 0.4f ) ); // KAmb=1.3, KEmm=0.3f, bit of both hilight!
	matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
	m_Mesh[7]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRESNEL_POW4 ), 
							GetDotProductFunctionU( DPF_SHIFTED_FRESNEL_POW8 ), 0, 0 );
	m_Mesh[7]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=???, Fb=???

	m_Mesh[8] = new DefMesh;
	if( FAILED( hr = m_Mesh[8]->CreateBuiltIn(	DefMesh::BT_CUBE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 1,1, 1);
	D3DXMatrixTranslation( &m2, 5, -1, 5 );
	D3DXMatrixMultiply( &m_Mesh[8]->m_WorldMatrix, &m1,&m2);
	matNum = MATERIAL_FULL_AMBIENT; // use an already created material
	matVec = D3DXVECTOR4(0.5f,1.0f, MaterialIndexToV(matNum),0 );
	m_Mesh[8]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1 ), 
							GetDotProductFunctionU( DPF_FRONT_POW32 ), 0, 0 );
	m_Mesh[8]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	matNum = RegisterMaterial( Material(0.3f, 0.f, 0.0f ) ); // KAmb=0.3, KEmm=0.f, metal hilight 

	// henge
	float theta = 0.f;
	matNum = RegisterMaterial( Material(0.3f, 0.f, 0.0f ) ); // KAmb=0.3, KEmm=0.f, metal hilight 

	for( unsigned int i=9;i < 19;i++)
	{
		D3DXVECTOR3 pos(0,1,0);
		pos.x = sinf( theta ) * 7;
		pos.z = -cosf( theta ) * 7;
		theta += 6.14f / 10;

		m_Mesh[i] = new DefMesh;
		if( FAILED( hr = m_Mesh[i]->CreateBuiltIn(	DefMesh::BT_CYLINDER, 
													TEXT("GeomSmoothSolidColour.fx") ) ) )
			return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );
		D3DXVECTOR3 scale( 1, 4, 1);
		D3DXMatrixTransformation( &m_Mesh[i]->m_WorldMatrix, 0, 0, &scale, 0, 0, &pos );

		// normal specular 
		matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
		m_Mesh[i]->m_Shader->SetVector( "f3Material", &matVec );
		dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW2 ), 
								GetDotProductFunctionU( DPF_FRONT_POW32 ), 0, 0 );
		m_Mesh[i]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=POW2, Fb=POW32

		col = D3DXVECTOR4(0,(float)i/19.f,(float)i/19.f,1);
		m_Mesh[i]->m_Shader->SetVector( "f3Colour0", &col );
	}

	// teapot at the back left
	m_Mesh[19] = new DefMesh;
	if( FAILED( hr = m_Mesh[19]->CreateBuiltIn(	DefMesh::BT_TEAPOT, TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( "CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 1, 1, 1);
	D3DXMatrixTranslation( &m2, 4, 0.0f, 0 );
	D3DXMatrixMultiply( &m_Mesh[19]->m_WorldMatrix, &m1,&m2);

	// create a blinn/phong specular material coloured surface with low specular exponent
	matNum = RegisterMaterial( Material(0.2f, 0.0f, 0.0f ) );
	matVec = D3DXVECTOR4(0.8f,0.8f, MaterialIndexToV(matNum),0 );
	m_Mesh[19]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW128 ), 0, 0 );
	m_Mesh[19]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );
	col = D3DXVECTOR4(1,0.675f,0.275f,1);
	m_Mesh[19]->m_Shader->SetVector( "f3Colour0", &col );
	m_Mesh[19]->m_Shader->SetVector( "f3Colour1", &col );


	return S_OK;
}

void CMyD3DApplication::UpdatePointLights()
{

	D3DXVECTOR3 pos(0,0,-2);
	static float starttheta = 3.14f;
	float theta = starttheta;
	starttheta += 0.015f;

	for(unsigned int j=0;j < MAX_POINT_LIGHTS;j++)
	{
		float val = (j/5.f) + 1.f;

		pos.x = sinf( theta ) * (1.5f + (MAX_POINT_LIGHTS-(float)j));
		pos.z = -cosf( theta ) * (0.5f + (float)j/4);

		D3DXVECTOR3 scale(val,val,val);

		D3DXMATRIXA16 mat;
		D3DXMatrixTransformation( &mat, 0, 0, &scale, 0, 0, 0 );

		D3DXVECTOR3 Sph[3];
		Sph[0] = D3DXVECTOR3(1,0,0);
		Sph[1] = D3DXVECTOR3(0,1,0);
		Sph[2] = D3DXVECTOR3(0,0,1);

		D3DXVec3TransformCoordArray( Sph, sizeof(D3DXVECTOR3), Sph, sizeof(D3DXVECTOR3), &mat, 3 );
		float x = D3DXVec3Length( &Sph[0] );
		float y = D3DXVec3Length( &Sph[1] );
		float z = D3DXVec3Length( &Sph[2] );

		float radius = x;
		if( y > radius )
			radius = y;
		if( z > radius )
			radius = z;

		m_LightPoint[j]->m_Radius = scale.x / 2;

		scale = D3DXVECTOR3( radius * 2, radius * 2, radius * 2);
		D3DXMatrixTransformation( &mat, 0, 0, &scale, 0, 0, &pos );
		m_LightPoint[j]->m_WorldMatrix = mat;

		unsigned int colr = j % 4;
		switch(colr)
		{
		case 0 :
			m_LightPoint[j]->m_LightColour = D3DXVECTOR4(1,0,0,0);
			break;
		case 1 :
			m_LightPoint[j]->m_LightColour = D3DXVECTOR4(0,1,0,0);
			break;
		case 2 :
			m_LightPoint[j]->m_LightColour = D3DXVECTOR4(0,0,1,0);
			break;
		case 3 :
			m_LightPoint[j]->m_LightColour = D3DXVECTOR4(1,1,1,0);
			break;
		}

		pos.y += 0.0f;
		theta += 6.28f / MAX_POINT_LIGHTS;
	}
}

void CMyD3DApplication::MatrixInfinitePerspectiveFovLH( D3DXMATRIX* matProj, 
														const float fFov, 
														const float fAspect, 
														const float fZNear )

{

	// use D3D function with a zfar that we will overwrite in a minute
	D3DXMatrixPerspectiveFovLH( matProj, fFov, fAspect, fZNear, 60.f );

	// this moves the far plane to infinity
	matProj->_33 = 1;
	matProj->_43 = -fZNear;
}

//-----------------------------------------------------------------------------
// Name: UpdateCullInfo()
// Desc: Sets up the frustum planes, endpoints, and center for the frustum
//       defined by a given view matrix and projection matrix.  This info will 
//       be used when culling each object in CullObject().
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::UpdateCullInfo( CULLINFO* pCullInfo )
{
    D3DXMATRIXA16 mat;

	D3DXMatrixInverse( &mat, NULL, &g_Matrices.m_ViewProj );

    pCullInfo->vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
    pCullInfo->vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
    pCullInfo->vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
    pCullInfo->vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
    pCullInfo->vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  0.99999f); // xyZ (0.99999f so we don't freak out at infinity)
    pCullInfo->vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  0.99999f); // XyZ
    pCullInfo->vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  0.99999f); // xYZ
    pCullInfo->vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  0.99999f); // XYZ

    for( INT i = 0; i < 8; i++ )
        D3DXVec3TransformCoord( &pCullInfo->vecFrustum[i], &pCullInfo->vecFrustum[i], &mat );

    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[CP_NEAR], &pCullInfo->vecFrustum[0], 
        &pCullInfo->vecFrustum[1], &pCullInfo->vecFrustum[2] ); // Near
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[CP_FAR], &pCullInfo->vecFrustum[6], 
        &pCullInfo->vecFrustum[7], &pCullInfo->vecFrustum[5] ); // Far
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[CP_LEFT], &pCullInfo->vecFrustum[2], 
        &pCullInfo->vecFrustum[6], &pCullInfo->vecFrustum[4] ); // Left
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[CP_RIGHT], &pCullInfo->vecFrustum[7], 
        &pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[5] ); // Right
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[CP_TOP], &pCullInfo->vecFrustum[2], 
        &pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[6] ); // Top
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[CP_BOTTOM], &pCullInfo->vecFrustum[1], 
        &pCullInfo->vecFrustum[0], &pCullInfo->vecFrustum[4] ); // Bottom
}

