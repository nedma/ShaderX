//-----------------------------------------------------------------------------
// File: gpu_dp.cpp
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
#include <assert.h>
#include <fstream>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "resource.h"
#include "gpu_dp.h"
#include "pixelshaderutil.h"
#include "neighbourhoodFuncs.h"
#include "dropper.h"
#include "debugFuncs.h"

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	// seed the random number generator
	srand((int)DXUtil_Timer(TIMER_GETABSOLUTETIME));

	//g_poutfile= new std::fstream( "resultlog.txt", std::ios::out );
	//if ( !g_poutfile )
	//{	assert(0);	}

    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    INT res = d3dApp.Run();

	//if (g_poutfile)
	//	g_poutfile->close();
	return res;
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
#ifdef _DEB_PS
	m_dwCreationWidth           = 300; // to be set when data texture loaded.
	m_dwCreationHeight          = 240;
#else
    m_dwCreationWidth           = 800; // to be set when data texture loaded.
    m_dwCreationHeight          = 600;
#endif
    m_strWindowTitle            = TEXT( "gpusort" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

	m_pVB2						= NULL;
	m_pVB3						= NULL;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

		//user
	QueryPerformanceFrequency( &m_countspersec );

	m_MapShow					= SHOW_CURR_VAL_MAP;
	m_bSoft						= false;
	m_ItCount					= 2;
	m_pBackBufChain				= NULL;

	m_pDefaultPixelShader		= NULL;
	m_pActionMapDispPixelShader	= NULL;
	m_pLogScalerPixelShader		= NULL;

	m_TotalIterations			= 0;
	m_pParticleSystem			= new CParticleSystem( 512, 2048 );
	m_ModValue					= 0;
	m_ModValueDelta				= 0;

	m_CurrValScale				= 0; // set after data texture loaded
	m_OrigValScale				= 25;

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
    // TODO: perform one time initialization

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    // Initialize DirectInput
    InitInput( m_hWnd );

	m_DPGPU_Params.m_pd3dDevice = NULL;
#ifdef _DEB_PS
	m_DPGPU_Params.m_pDataFileName = "valuemap8x8.bmp";
	m_DPGPU_Params.m_pBoundaryMapFileName = "boundarymap8x8.bmp";
#else
	m_DPGPU_Params.m_pDataFileName = "valuemap128.bmp";
	m_DPGPU_Params.m_pBoundaryMapFileName = "boundarymap128.bmp";
//	m_DPGPU_Params.m_pDataFileName = "valuemap8x8.bmp";
//	m_DPGPU_Params.m_pBoundaryMapFileName = "boundarymap8x8.bmp";
#endif
	m_DPGPU_Params.m_pImageFileName = "image.bmp";


	m_DPGPU.OneTimeSceneInit( &m_DPGPU_Params );
	m_DPSoft_Params.m_pDPGPU = &m_DPGPU;
	m_DPSoft.OneTimeSceneInit( &m_DPSoft_Params );

    m_bLoadingApp = FALSE;
	return S_OK;
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
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;
		// check if desired texture format is compatible with current adapter format.

	HRESULT hr;
    hr = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                          D3DDEVTYPE_HAL,
                                          Format,
                                          0,
                                          D3DRTYPE_TEXTURE,
                                          m_DPGPU.GetDataTextureFormat() );

	if ( FAILED(hr) )
		bCapsAcceptable = FALSE;

    if( bCapsAcceptable )         
	{
//		DXTRACE_ERR( "Hurray!", hr );
        return S_OK;
	}
    else
        return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateOffScreenPlainSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: create device objects
    HRESULT hr;
	int j;

	m_DPGPU_Params.m_pd3dDevice = m_pd3dDevice;
	if ( FAILED (hr= m_DPGPU.InitDeviceObjects(&m_DPGPU_Params)) )
	{
        DXTRACE_ERR( "m_DPGPU.InitDeviceObjects", hr );
		return E_ABORT;
	}

	if ( FAILED( m_DPSoft.InitDeviceObjects(&m_DPSoft_Params) ) )
	{
		DXTRACE_ERR( "m_DPSoft.InitDeviceObjects", hr );
		return E_ABORT;
	}

	m_ModValue = 10.0f * sqrt( float(m_DPGPU.GetDataHeight() * m_DPGPU.GetDataWidth()) ); //arbitary
	m_ModValueDelta = m_ModValue/10.0f;
	m_CurrValScale = m_ModValue/20.0f;

    // Init the font
    hr = m_pFont->InitDeviceObjects( m_pd3dDevice );
    if( FAILED( hr ) )
        return DXTRACE_ERR( "m_pFont->InitDeviceObjects", hr );


	// create geometry to display the results....
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 6*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB2, NULL ) ) )
        return DXTRACE_ERR( "CreateVertexBuffer", hr );
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 6*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB3, NULL ) ) )
        return DXTRACE_ERR( "CreateVertexBuffer", hr );

	CUSTOMVERTEX* pVertices;
    // Fill the vertex buffer with 2 triangles (to make quad)
    if( FAILED( hr = m_pVB2->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
        return DXTRACE_ERR( "Lock", hr );

	float width, height;
	float tscalex, tscaley;
	width = float(m_dwCreationWidth);//m_DPGPU.GetDataWidth() * (int)tscalex;
	height = float(m_dwCreationHeight); //m_DPGPU.GetDataHeight() * (int)tscaley;
	tscalex = width/m_DPGPU.GetDataWidth(); // scale poly up to show texture better.
	tscaley = height/m_DPGPU.GetDataHeight(); // scale poly up to show texture better.

	pVertices[0].position = D3DXVECTOR4( 0.0f,  0.0f, 0.0f, 1.0f );
	pVertices[0].texcoords0 = D3DXVECTOR2( 0.0f,  0.0f );
    pVertices[1].position = D3DXVECTOR4(  (float)width, 0, 0.0f, 1.0f );
	pVertices[1].texcoords0 = D3DXVECTOR2( 1.0f,  0.0f );
	pVertices[2].position = D3DXVECTOR4(  0.0f, (float)height, 0.0f, 1.0f );
	pVertices[2].texcoords0 = D3DXVECTOR2( 0.0f,  1.0f );

    pVertices[3].position = D3DXVECTOR4(  (float)width, 0, 0.0f, 1.0f );
	pVertices[3].texcoords0 = D3DXVECTOR2( 1.0f,  0.0f );
    pVertices[4].position = D3DXVECTOR4(  0.0f, (float)height, 0.0f, 1.0f );
	pVertices[4].texcoords0 = D3DXVECTOR2( 0.0f,  1.0f );
	pVertices[5].position = D3DXVECTOR4(  (float)width, (float)height, 0.0f, 1.0f );
	pVertices[5].texcoords0 = D3DXVECTOR2( 1.0f,  1.0f );

	float offx= 0.0f;
	float offy= 0.0f;
	D3DXVECTOR4 offvec = D3DXVECTOR4( offx,  offy, 0.0f, 0.0f );
	for ( j=0; j<6; j++ )
	{	pVertices[j].position += offvec;	}
    
	m_pVB2->Unlock();

	hr = LoadShaderFile(m_pd3dDevice, "DefaultShader20.psh", &m_pDefaultPixelShader);
	if ( FAILED(hr) )
		return DXTRACE_ERR( "cannot load pixel shader", hr );

	hr = LoadShaderFile(m_pd3dDevice, "ActionMapDisplayer20.psh", &m_pActionMapDispPixelShader);
	if ( FAILED(hr) )
		return DXTRACE_ERR( "cannot load pixel shader", hr );

	hr = LoadShaderFile(m_pd3dDevice, "LogScaler20.psh", &m_pLogScalerPixelShader);
	if ( FAILED(hr) )
		return DXTRACE_ERR( "cannot load pixel shader", hr );
	
	hr = m_WorldScreenTransform.InitDeviceObjects( offx, offy, tscalex, tscaley, 
													m_DPGPU.GetDataWidth(), m_DPGPU.GetDataHeight() );

	// init the particle system.
	hr = m_pParticleSystem->InitDeviceObjects( &m_WorldScreenTransform, &m_DPGPU, m_hWnd );
	m_TotalIterations = 0;

    return S_OK;
}


// -----------------------------------------------------------------------------------
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
    // TODO: setup render states
	HRESULT		retres= S_OK;
    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures

    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 4, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 4, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 4, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	m_pd3dDevice->SetSamplerState( 4, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState( 4, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,        D3DCMP_ALWAYS );
//    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE ,        FALSE );

	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	HRESULT hr;
	if ( FAILED(hr=m_DPGPU.RestoreDeviceObjects(NULL) ))
		retres = S_FALSE;
	
	if ( FAILED(hr=m_DPSoft.RestoreDeviceObjects(NULL) ))
		retres = S_FALSE;


	hr = m_pd3dDevice->GetRenderTarget( 0,	&m_pBackBufChain );
	if ( FAILED(hr) )
		retres = S_FALSE;

	if ( FAILED(hr=m_pParticleSystem->RestoreDeviceObjects(m_pd3dDevice) ))
		retres = S_FALSE;

	m_UserInput.bRestartIterations = true;

	// Restore the font
    m_pFont->RestoreDeviceObjects();

//    m_bLoadingApp = FALSE;
    return retres;
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

	static float scaleDelta = float(m_DPGPU.GetDataHeight()*m_DPGPU.GetDataWidth())/100000.0f;

    if( m_UserInput.bScaleInc && !m_UserInput.bScaleDec )
	{
		if ( m_MapShow == SHOW_ORIG_VAL_MAP )
			m_OrigValScale += 0.05f;
		else
			m_CurrValScale += scaleDelta;
	}
    if( !m_UserInput.bScaleInc && m_UserInput.bScaleDec )
	{
		if ( m_MapShow == SHOW_ORIG_VAL_MAP )
		{
			m_OrigValScale -= 0.05f;
			if ( m_OrigValScale <=0 )
				m_OrigValScale = 0.05f;
		}
		else
		{
        	m_CurrValScale -= scaleDelta;
			if ( m_CurrValScale <=0 )
				m_CurrValScale = scaleDelta;
		}
	}
	if( m_UserInput.bToggleSoft )
	{
		m_bSoft ^= 1;
		m_UserInput.bToggleSoft= 0;
	}

	if( m_UserInput.bToggleDisplayImage )
	{
		m_MapShow = MAP_SHOW(int(m_MapShow) +  1);
		if (m_MapShow>=SHOW_MAX)
			m_MapShow = MAP_SHOW(0);
	}

	if( m_UserInput.bItCountInc && !m_UserInput.bItCountDec )
	{
		m_ItCount += 2;
	}
	if( !m_UserInput.bItCountInc && m_UserInput.bItCountDec )
	{
		m_ItCount -= 2;
		if ( m_ItCount <= 0)
			m_ItCount = 2;
	}

	if ( m_UserInput.bValueModInc )
	{
		m_ModValue += m_ModValueDelta;
	}
	if ( m_UserInput.bValueModDec )
	{
		m_ModValue -= m_ModValueDelta;
		if ( m_ModValue < 0.0f )
			m_ModValue = 0.0f;
	}

	if ( m_fFPS!=0 )
	{
		m_pParticleSystem->Update(1.0f/m_fFPS);
	}

	Pick();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    HRESULT hr;

	memcpy( pUserInput->diksPrev, pUserInput->diks, 256*sizeof(BYTE) );

    // Get the input's device state, and put the state in dims
    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    // TODO: Process user input as needed
	pUserInput->bScaleInc	= ( (pUserInput->diks[DIK_UP] & 0x80)  == 0x80 );
	pUserInput->bScaleDec	= ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );
	pUserInput->bItCountInc	= ( (pUserInput->diks[DIK_RIGHT] & 0x80)  == 0x80 );
	pUserInput->bItCountDec	= ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );

	pUserInput->bToggleSoft = ( (pUserInput->diks[DIK_S] & 0x80)  == 0x80 );
	pUserInput->bToggleSoft = pUserInput->bToggleSoft && !((pUserInput->diksPrev[DIK_S] & 0x80)  == 0x80 );

	
	pUserInput->bToggleDisplayImage = ( (pUserInput->diks[DIK_I] & 0x80)  == 0x80 );
	pUserInput->bToggleDisplayImage = pUserInput->bToggleDisplayImage && !( (pUserInput->diksPrev[DIK_I] & 0x80)  == 0x80 );

	pUserInput->bRestartIterations = ( (pUserInput->diks[DIK_R] & 0x80)  == 0x80 );

	pUserInput->bValueModInc = ( (pUserInput->diks[DIK_RBRACKET] & 0x80)  == 0x80 );
	pUserInput->bValueModDec = ( (pUserInput->diks[DIK_LBRACKET] & 0x80)  == 0x80 );
	pUserInput->bAddValueModToMap = ( (pUserInput->diks[DIK_EQUALS] & 0x80)  == 0x80 );
	pUserInput->bAddValueModToMap = pUserInput->bAddValueModToMap&& !( (pUserInput->diksPrev[DIK_EQUALS] & 0x80)  == 0x80 );
	pUserInput->bSubValueModToMap = ( (pUserInput->diks[DIK_MINUS] & 0x80)  == 0x80 );
	pUserInput->bSubValueModToMap = pUserInput->bSubValueModToMap && !( (pUserInput->diksPrev[DIK_MINUS] & 0x80)  == 0x80 );
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	D3DXVECTOR4		dbvec1(0,1,2,3);
	D3DXVECTOR4		dbvec2(4,5,6,7);

	if (m_bSoft)
	{
		QueryPerformanceCounter( &m_perfcounterprev );
		m_DPSoft.IterateVals(m_ItCount,m_UserInput.bRestartIterations);
		QueryPerformanceCounter( &m_perfcounter );
	}
	else
	{
		QueryPerformanceCounter( &m_perfcounterprev );
		m_DPGPU.IterateVals(m_ItCount,m_UserInput.bRestartIterations);
		QueryPerformanceCounter( &m_perfcounter );
    }
	if ( m_UserInput.bRestartIterations )
		m_TotalIterations = m_ItCount+1;
	else
		m_TotalIterations += m_ItCount;

		// render to back buffer
	D3DXVECTOR4 v,o;
	o =  D3DXVECTOR4( 1.f,1.f,1.f,1.f );

	m_pd3dDevice->SetRenderTarget( 0, m_pBackBufChain );
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	0x00000000, 1.0f, 0L );
	
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		if (!m_bSoft)
		{

			m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			switch (m_MapShow)
			{
				case SHOW_ORIG_VAL_MAP:
					v = D3DXVECTOR4( 1.0f/(m_OrigValScale), 0,0,1 );
					m_pd3dDevice->SetPixelShaderConstantF(25,(float*)&v,1);
					m_pd3dDevice->SetPixelShaderConstantF(18, (float*)&o, 1 );
					m_pd3dDevice->SetPixelShader( m_pLogScalerPixelShader );
					m_pd3dDevice->SetTexture( 0, m_DPGPU.m_pDataTexture );
					m_pd3dDevice->SetStreamSource( 0, m_pVB2, 0, sizeof(CUSTOMVERTEX) );
					m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
					break;
				case SHOW_BOUNDARY_MAP:
					m_pd3dDevice->SetPixelShader( m_pDefaultPixelShader );
					m_pd3dDevice->SetTexture( 0, m_DPGPU.m_pBoundaryMapTex );
					m_pd3dDevice->SetStreamSource( 0, m_pVB2, 0, sizeof(CUSTOMVERTEX) );
					m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
					break;
				case SHOW_CURR_VAL_MAP:
					v = D3DXVECTOR4( 1.0f/(m_CurrValScale), 0,0,1 );
					m_pd3dDevice->SetPixelShaderConstantF(25,(float*)&v,1);
					m_pd3dDevice->SetPixelShaderConstantF(18, (float*)&o, 1 );
					m_pd3dDevice->SetPixelShader( m_pLogScalerPixelShader );
					m_pd3dDevice->SetTexture( 0, m_DPGPU.m_pRenderTargTex[0] );
					m_pd3dDevice->SetStreamSource( 0, m_pVB2, 0, sizeof(CUSTOMVERTEX) );
					m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
					break;
				case SHOW_ACTION_MAP:
					m_pd3dDevice->SetPixelShader( m_pActionMapDispPixelShader );
					m_pd3dDevice->SetTexture( 0, m_DPGPU.m_pActionTex );
					m_pd3dDevice->SetStreamSource( 0, m_pVB2, 0, sizeof(CUSTOMVERTEX) );
					m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
					break;
				default:
					assert(0);
			}

			m_pParticleSystem->Render(m_pd3dDevice);
		}
			// Render stats and help text  
        RenderText();


    }
    // End the scene.
    m_pd3dDevice->EndScene();

    return S_OK;
}
//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,50,0,255);
    D3DCOLOR fontColor2       = D3DCOLOR_ARGB(255,0,255,100);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 6.0f; 
    lstrcpy( szMsg, m_strDeviceStats );
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine += 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	switch( m_MapShow )
	{
	case SHOW_ORIG_VAL_MAP:
		sprintf( szMsg, "original values");
		break;
	case SHOW_BOUNDARY_MAP:
		sprintf( szMsg, "boundary map");
		break;
	case SHOW_CURR_VAL_MAP:
		sprintf( szMsg, "current values " );
		break;
	case SHOW_ACTION_MAP:
		sprintf( szMsg, "action map " );
		break;
	default:
		assert(0);
	}
	fNextLine += 20.0f;
	m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	int bips =  (m_perfcounter.LowPart-m_perfcounterprev.LowPart);///countspersec.QuadPart; 
	float dtime= ((float)bips)/(float)(m_countspersec.LowPart);
	if ( m_bSoft )
		sprintf( szMsg, "Software Mode: time for %d iterations= %f s ", m_ItCount, dtime );
	else
		sprintf( szMsg, "GPU Mode: time for %d iterations= %f s ", m_ItCount, dtime );
    fNextLine += 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );


	if ( m_MapShow == SHOW_ORIG_VAL_MAP )
        sprintf( szMsg, " Orig Value scale down factor: %f", m_OrigValScale );
	else
		sprintf( szMsg, " Value scale down factor: %f", m_CurrValScale );
	fNextLine += 20.0f;
	m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	int row, col;
	if ( !FAILED(m_WorldScreenTransform.ScreenToWorld(m_UserInput.m_ptLastMousePosition.x,
		m_UserInput.m_ptLastMousePosition.y, row, col )	) )
	{
		float dispVal=-1;
		switch( m_MapShow )
		{
		case SHOW_ORIG_VAL_MAP:
			dispVal = m_DPGPU.GetOrigValue(row,col);
			break;
		case SHOW_BOUNDARY_MAP:
			dispVal = m_DPGPU.IsBoundary(row,col)? 0.0f : 1.0f;
			break;
		case SHOW_CURR_VAL_MAP:
			dispVal = m_DPGPU.GetCurrValue(row,col);
			break;
		case SHOW_ACTION_MAP:
			dispVal = (float)m_DPGPU.GetAction(row, col);
			break;
			break;
		default:
			assert(0);
		}

		sprintf( szMsg, " [row,col] = [%d,%d] Val:= %g", row,col, dispVal );
		fNextLine += 20.0f;
		m_pFont->DrawText( 2, fNextLine, fontColor2, szMsg );
	}

	sprintf( szMsg, "ModVal = %f", m_ModValue );
	fNextLine += 20.0f;
	m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	sprintf( szMsg, "Total Iterations = %d", m_TotalIterations );
	fNextLine += 20.0f;
	m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );


	// Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 

    lstrcpy( szMsg, TEXT(" I = cycle texture to display. S = Toggle Software/GPU mode") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT(" Up/Down Arrow = Intensity scale factor for Value Maps, lower factor = brighter image") );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT(" Left/Right Arrow = Dec/Inc Num Iterations Per Frame ") );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT(" R = Reset cell values to original costs") );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT(" [ / ] = change Modval ") );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT(" - / + = Sub/Add Modval to original costs at area under mouse") );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT(" LMouse = drop a particle. RightMouse = kill all particles.") );
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

		case WM_RBUTTONDOWN: 
		case WM_MBUTTONDOWN: 
		case WM_LBUTTONDOWN: 
			{
				if( msg == WM_LBUTTONDOWN ) { m_UserInput.m_bMouseLButtonDown = true; m_UserInput.m_nCurrentButtonMask |= MOUSE_LEFT_BUTTON; }
				if( msg == WM_MBUTTONDOWN ) { m_UserInput.m_bMouseMButtonDown = true; m_UserInput.m_nCurrentButtonMask |= MOUSE_MIDDLE_BUTTON; }
				if( msg == WM_RBUTTONDOWN ) { m_UserInput.m_bMouseRButtonDown = true; m_UserInput.m_nCurrentButtonMask |= MOUSE_RIGHT_BUTTON; }
				SetCapture(hWnd);
				GetCursorPos( &m_UserInput.m_ptLastMousePosition ); 
				ScreenToClient( hWnd, &m_UserInput.m_ptLastMousePosition );
				break;
			}

		case WM_RBUTTONUP: 
		case WM_MBUTTONUP: 
		case WM_LBUTTONUP:   
			{
				if( msg == WM_LBUTTONUP ) { m_UserInput.m_bMouseLButtonDown = false; m_UserInput.m_nCurrentButtonMask &= ~MOUSE_LEFT_BUTTON; }
				if( msg == WM_MBUTTONUP ) { m_UserInput.m_bMouseMButtonDown = false; m_UserInput.m_nCurrentButtonMask &= ~MOUSE_MIDDLE_BUTTON; }
				if( msg == WM_RBUTTONUP ) { m_UserInput.m_bMouseRButtonDown = false; m_UserInput.m_nCurrentButtonMask &= ~MOUSE_RIGHT_BUTTON; }

				if( !m_UserInput.m_bMouseLButtonDown  && 
					!m_UserInput.m_bMouseRButtonDown &&
					!m_UserInput.m_bMouseMButtonDown )
				{
					ReleaseCapture();
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
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

	SAFE_RELEASE( m_pBackBufChain );

	m_DPGPU.InvalidateDeviceObjects();
	m_DPSoft.InvalidateDeviceObjects();

	m_pParticleSystem->InvalidateDeviceObjects();

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
    // TODO: Cleanup any objects created in InitDeviceObjects()


    m_pFont->DeleteDeviceObjects();
	SAFE_RELEASE( m_pVB2 );
	SAFE_RELEASE( m_pVB3 );
	m_DPGPU.DeleteDeviceObjects();
	m_DPSoft.DeleteDeviceObjects();
	m_pParticleSystem->DeleteDeviceObjects();

	SAFE_RELEASE( m_pDefaultPixelShader );
	SAFE_RELEASE( m_pActionMapDispPixelShader );
	SAFE_RELEASE( m_pLogScalerPixelShader );

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
    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );
    
	SAFE_DELETE( m_pParticleSystem );
	m_DPGPU.FinalCleanup();
	m_DPSoft.FinalCleanup();

	CleanupDirectInput();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    // Cleanup DirectX input objects
    if( m_pKeyboard )
        m_pKeyboard->Unacquire();
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}

//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Pick()
{
		// check if particle is to be instanced or if map is to be altered.
	int row, col;
	POINT ptCursor;
	if( GetCapture() )
	{
		ptCursor = m_UserInput.m_ptLastMousePosition;
		if ( !FAILED( m_WorldScreenTransform.ScreenToWorld( ptCursor.x, ptCursor.y, row, col ))  )
		{
			if ( m_UserInput.m_bMouseLButtonDown )
			{
				m_pParticleSystem->InstanceNewParticle(row, col, D3DXCOLOR( 1.0f,   0.5f,   0.5f,   1.0f ));
			}
			else if ( m_UserInput.m_bMouseRButtonDown )
			{
				m_pParticleSystem->KillAllParticles();
			}
		}
	}
	if ( m_UserInput.bAddValueModToMap || m_UserInput.bSubValueModToMap )
	{
		GetCursorPos( &ptCursor ); 
		ScreenToClient( m_hWnd, &ptCursor );
		if ( !FAILED( m_WorldScreenTransform.ScreenToWorld( ptCursor.x, ptCursor.y, row, col ))  )
		{
			if ( m_UserInput.bAddValueModToMap  )
			{ // in a 3x3 around the mouse, increment the map values by the mod val.
				m_DPGPU.ModifyMap3x3( row, col, m_ModValue );
			}
			else if ( m_UserInput.bSubValueModToMap )
			{
				m_DPGPU.ModifyMap3x3( row, col, -m_ModValue );
			}
		}
	}

	return S_OK;
}
