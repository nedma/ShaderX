//--------------------------------------------------------------------------------------
// File: Topology4.cpp
//
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
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

#define MAX_OBJECTS 32
#define MAX_SHADOWS 2
#define MAX_POINT_LIGHTS 15

#define MAX_LIGHTS 32

#define SHADOW_MAP_FORMAT D3DFMT_R32F
#define SHADOW_MAP_SIZE   1024

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
ID3DXEffect*            g_pEffect2 = NULL;      // D3DX effect interface
CModelViewerCamera      g_Camera;               // A model viewing camera
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls

bool					g_softwareVS30 = true;
class GBuffer*			g_FrameBuffer;
LPDIRECT3DSURFACE9		g_pBackBuffer;
LPDIRECT3DSURFACE9		g_pHDRBuffer;
LPDIRECT3DTEXTURE9		g_pHDRBufferTex;
LPDIRECT3DSURFACE9		g_pMeanLogLuminance;
LPDIRECT3DTEXTURE9		g_pMeanLogLuminanceTex;
LPDIRECT3DSURFACE9		g_pTempLogLuminance = 0;
LPDIRECT3DTEXTURE9		g_pTempLogLuminanceTex = 0;
LPD3DXEFFECT			g_ShowFatFx;
LPD3DXEFFECT			g_ToneMapFx;
class DefMesh*			g_Mesh[MAX_OBJECTS];

BaseLight*				g_ActiveLights[MAX_LIGHTS];
LightHemisphere*		g_LightHemisphere;
LightDirectional*		g_LightDirectional[MAX_SHADOWS];
LightPoint*				g_LightPoint[MAX_POINT_LIGHTS];

LPDIRECT3DTEXTURE9		g_pShadowMap[MAX_SHADOWS];
LPDIRECT3DSURFACE9		g_pShadowMapSurf[MAX_SHADOWS];
LPDIRECT3DSURFACE9		g_pShadowMapZ;

LPDIRECT3DTEXTURE9		g_pDotProductFuncs;
LPDIRECT3DTEXTURE9		g_pMaterialFuncs;
LPDIRECT3DTEXTURE9		g_pAttenuationFuncs;

GlobalMatrices			g_Matrices;
GlobalObjects			g_Objs;
int						g_iXBlocks;
int						g_iYBlocks;

enum
{
	DFT_DISPLAY_BUFFER0,
	DFT_DISPLAY_BUFFER1,
	DFT_DISPLAY_BUFFER2,
	DFT_DISPLAY_BUFFER3,

	DFT_COMBINE_COLOUR,
	DFT_COMBINE_MONO,

	DFT_COMBINE_WIERD,
}					g_DisplayFatType;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_VIEWWHITEPAPER      2
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4



//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl );
void    CALLBACK OnLostDevice();
void    CALLBACK OnDestroyDevice();

void    InitApp();
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText();
void CopyTextureToRenderTarget( IDirect3DDevice9* pd3dDevice );
VOID UpdateCullInfo( CULLINFO* pCullInfo );
void MatrixInfinitePerspectiveFovLH( D3DXMATRIX* matProj, const float fFov, const float fAspect, const float fZNear );
void UpdatePointLights();
HRESULT CreateTestObjects( IDirect3DDevice9* pd3dDevice );
void RenderShadowMap( IDirect3DDevice9* pd3dDevice, unsigned int shadowNum );
void CopyTextureToRenderTarget( IDirect3DDevice9* pd3dDevice );
void LightGBuffer( IDirect3DDevice9* pd3dDevice );
void ApplyFullScreenPixelShader( IDirect3DDevice9* pd3dDevice, const unsigned int width, const unsigned int height );


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Set the callback functions. These functions allow the sample framework to notify
    // the application about device changes, user input, and windows messages.  The 
    // callbacks are optional so you need only set callbacks for events you're interested 
    // in. However, if you don't handle the device reset/lost callbacks then the sample 
    // framework won't be able to reset your device since the application must first 
    // release all device resources before resetting.  Likewise, if you don't handle the 
    // device created/destroyed callbacks then the sample framework won't be able to 
    // recreate your device resources.
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    InitApp();

    // Initialize the sample framework and create the desired Win32 window and Direct3D 
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTCreateWindow( L"DeferredHDR" );
//    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 320, 240, IsDeviceAcceptable, ModifyDeviceSettings );

    // Pass control to the sample framework for handling the message pump and 
    // dispatching render calls. The sample framework will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;
	// check texture formats
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, (D3DUSAGE_QUERY_VERTEXTEXTURE | D3DUSAGE_SOFTWAREPROCESSING), 
                    D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F ) ) )
        return false;

    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE, GBufferMRT::m_FatFormat ) ) )
        return false;
	// check single channel 32 bit float for shadow
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE, D3DFMT_G16R16F ) ) )
        return false;
	// check 2 channel 16 bit signed integer 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_V16U16 ) ) )
        return false;
	// check 4 channel 16 bit unsigned integer 
/*    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16 ) ) )
        return false;
		*/
	// check 4 channel 16 bit floats 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the 
// application to modify the device settings. The supplied pDeviceSettings parameter 
// contains the settings that the framework has selected for the new device, and the 
// application can make any desired changes directly to this structure.  Note however that 
// the sample framework will not correct invalid device settings so care must be taken 
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.  
//--------------------------------------------------------------------------------------
void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps )
{
	// require pixel shader 3.0 (possible this would work in ps2.0b, I don't have a card to check so 
	// for safety go with PS3.0)
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(3,0) )
    {
	    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
    }

    // This application is designed to work on a pure device by not using 
    // IDirect3D9::Get*() methods, so create a pure device if supported and using HWVP.
    if ((pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 )
	{
        pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
	}

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
    // shader debugger. Debugging vertex shaders requires either REF or software vertex 
    // processing, and debugging pixel shaders requires REF.  The 
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile 
    // against the next higher available software target, which ensures that the 
    // unoptimized shaders do not exceed the shader model limitations.  Setting these 
    // flags will cause slower rendering since the shaders will be unoptimized and 
    // forced into software.  See the DirectX documentation for more information about 
    // using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

	D3DCAPS9 fakecaps;
	V_RETURN( pd3dDevice->GetDeviceCaps( &fakecaps ) );

	D3DCAPS9 caps;
	V_RETURN( DXUTGetD3DObject()->GetDeviceCaps( fakecaps.AdapterOrdinal, fakecaps.DeviceType, &caps ) );

	if( caps.VertexShaderVersion >= D3DVS_VERSION(3,0) )
	{
		g_softwareVS30 = false;
	} else
	{
		g_softwareVS30 = true;
	}

	g_DisplayFatType = DFT_COMBINE_COLOUR;
	g_LightHemisphere = 0;
	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		g_pShadowMap[i]	 = NULL;
		g_pShadowMapSurf[i] = NULL;
		g_LightDirectional[i] = NULL;
	}
	g_pShadowMapZ	 = NULL;
	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		g_LightPoint[i] = NULL;
	}

	g_pAttenuationFuncs = NULL;
	g_pDotProductFuncs = NULL;
	g_pMaterialFuncs = NULL;

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		g_Mesh[i] =  NULL;
	}

	for(unsigned int i=0;i < MAX_LIGHTS;i++)
	{
		g_ActiveLights[i] = NULL;
	}

	if( FAILED( hr = D3DXCreateEffectPool( &g_Objs.m_EffectPool) ) )
		return hr;

	if( FAILED( hr = D3DXCreateEffectPool( &g_Objs.m_LightEffectPool) ) )
		return hr;

	g_Objs.m_pD3DDevice = pd3dDevice;

	if( FAILED( hr = CreateTestObjects(pd3dDevice) ) )
		return hr;

	// a pixel shader that show/combines each buffer in the fat framebuffer
	if( FAILED( hr = D3DXCreateEffectFromFile(	pd3dDevice, 
												TEXT("ShowFat.fx"),
												0, // MACROs
												0, // Include
												dwShaderFlags, // flags
												g_Objs.m_LightEffectPool, // Effect Pool
												&g_ShowFatFx,
												0 ) ) )
        return DXTRACE_ERR( L"D3DXCreateEffectFromFile ShowFat.fx", hr );
	g_ShowFatFx->SetTechnique("T0");

	unsigned int currentLight = 0;

	g_LightHemisphere = new LightHemisphere;
	g_LightHemisphere->Create();
	g_ActiveLights[currentLight++] = g_LightHemisphere;

	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		g_LightDirectional[i] = new LightDirectional;
		g_LightDirectional[i]->Create();
		g_ActiveLights[currentLight++] = g_LightDirectional[i];

		if( i == 0 )
		{
			g_LightDirectional[0]->m_LightColour = D3DXVECTOR4(1,0.5f,1,0);
			g_LightDirectional[0]->m_LightIntensity = 10.0f;
		} else
		{
			g_LightDirectional[1]->m_LightColour = D3DXVECTOR4(1,1,0.7f,0);
			g_LightDirectional[1]->m_LightIntensity = 4.0f;
		}
	}

	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		g_LightPoint[i] = new LightPoint;
		g_LightPoint[i]->Create();
		
		g_LightPoint[i]->m_LightIntensity = 2.0f;
		g_LightPoint[i]->m_LightID = (unsigned char) i;
		g_ActiveLights[currentLight++] = g_LightPoint[i];
		g_LightPoint[i]->m_ShowVolume = false;
	}

	// a pixel shader that show/combines each buffer in the fat framebuffer
	if( FAILED( hr = D3DXCreateEffectFromFile(	pd3dDevice, 
												TEXT("ToneMap.fx"),
												0, // MACROs
												0, // Include
												dwShaderFlags, // flags
												g_Objs.m_LightEffectPool, // Effect Pool
												&g_ToneMapFx,
												0 ) ) )
        return DXTRACE_ERR( L"D3DXCreateEffectFromFile ToneMap.fx", hr );
	g_ToneMapFx->SetTechnique("T0");

	// Setup the camera's view parameters
    D3DXVECTOR3 vecEye(-2.0f, 3.5f,	6.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
    g_Camera.SetViewParams( &vecEye, &vecAt );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This function loads the mesh and ensures the mesh has normals; it also optimizes the 
// mesh for the graphics card's vertex cache, which improves performance by organizing 
// the internal triangle list for less cache misses.
//--------------------------------------------------------------------------------------
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh )
{
    ID3DXMesh* pMesh = NULL;
    WCHAR str[MAX_PATH];
    HRESULT hr;

    // Load the mesh with D3DX and get back a ID3DXMesh*.  For this
    // sample we'll ignore the X file's embedded materials since we know 
    // exactly the model we're loading.  See the mesh samples such as
    // "OptimizedMesh" for a more generic mesh loading example.
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );

    V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

    DWORD *rgdwAdjacency = NULL;

    // Make sure there are normals which are required for lighting
    if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        ID3DXMesh* pTempMesh;
        V( pMesh->CloneMeshFVF( pMesh->GetOptions(), 
                                  pMesh->GetFVF() | D3DFVF_NORMAL, 
                                  pd3dDevice, &pTempMesh ) );
        V( D3DXComputeNormals( pTempMesh, NULL ) );

        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
    }

    // Optimize the mesh for this graphics card's vertex cache 
    // so when rendering the mesh's triangle list the vertices will 
    // cache hit more often so it won't have to re-execute the vertex shader 
    // on those vertices so it will improve perf.     
    rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    V( pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
    V( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
    delete []rgdwAdjacency;

    *ppMesh = pMesh;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
    if( g_pEffect )
        V_RETURN( g_pEffect->OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 1000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-350 );
    g_SampleUI.SetSize( 170, 300 );

	D3DCAPS9 d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );

	// MRT support or not?
	if( d3dCaps.NumSimultaneousRTs == 4 )
	{
		g_FrameBuffer = new GBufferMRT( DXUTGetD3DObject(), pd3dDevice );
	} else
	{
		g_FrameBuffer = new GBufferSingle( DXUTGetD3DObject(), pd3dDevice );
	}

	const D3DSURFACE_DESC* pBBSurfaceDesc = DXUTGetBackBufferSurfaceDesc();

	// create all the render-targets
	if( g_FrameBuffer->CreateBuffers( pBBSurfaceDesc->Width, pBBSurfaceDesc->Height ) == false )
		return E_FAIL;

	g_Objs.m_Framebuffer = g_FrameBuffer;

	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		// Create shadow map texture and retrieve surface
		if (FAILED(pd3dDevice->CreateTexture(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, 
			D3DUSAGE_RENDERTARGET, SHADOW_MAP_FORMAT, D3DPOOL_DEFAULT, &g_pShadowMap[i], NULL)))
			return E_FAIL;
		if (FAILED(g_pShadowMap[i]->GetSurfaceLevel(0, &g_pShadowMapSurf[i])))
			return E_FAIL;
	}

	// Create depth buffer for shadow map rendering
	if (FAILED(pd3dDevice->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pShadowMapZ, NULL)))
		return E_FAIL;

	// Create dot product texture
	if (FAILED(pd3dDevice->CreateTexture(	DOTP_WIDTH, 
											NUM_DOTP_FUNCTIONS * BILERP_PROTECTION, 1, 0, 
											D3DFMT_V16U16, D3DPOOL_MANAGED, &g_pDotProductFuncs, NULL)))
		return E_FAIL;
	if ( FAILED(D3DXFillTexture(g_pDotProductFuncs,FillDotProductTexture, 0)) )
		return E_FAIL;

	// Create attenuation texture
	if (FAILED(pd3dDevice->CreateTexture(	ATTENUATION_WIDTH, 
											NUM_ATTENUATION_FUNCTIONS * BILERP_PROTECTION, 1, 0, 
											D3DFMT_V16U16, D3DPOOL_MANAGED, &g_pAttenuationFuncs, NULL)))
		return E_FAIL;
	if ( FAILED(D3DXFillTexture(g_pAttenuationFuncs,FillAttenuationTexture, 0)) )
		return E_FAIL;


	// fill the material list, all materials should have already been registered
	if (FAILED(pd3dDevice->CreateTexture(	NUM_MATERIALS, 1, 
											1, 0, 
											D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED, &g_pMaterialFuncs, NULL)))
		return E_FAIL;
	if ( FAILED(D3DXFillTexture(g_pMaterialFuncs,FillMaterialTexture, 0)) )
		return E_FAIL;


	// set the g-buffer textures into showfat.fx
	g_ShowFatFx->SetTexture( "Buffer0Texture", g_FrameBuffer->GetBufferTexture( FFB_BUFFER0 ) );
	g_ShowFatFx->SetTexture( "Buffer1Texture", g_FrameBuffer->GetBufferTexture( FFB_BUFFER1 ) );
	g_ShowFatFx->SetTexture( "Buffer2Texture", g_FrameBuffer->GetBufferTexture( FFB_BUFFER2 ) );
	g_ShowFatFx->SetTexture( "Buffer3Texture", g_FrameBuffer->GetBufferTexture( FFB_BUFFER3 ) );
	g_ShowFatFx->SetTexture( "GlobalShadowMapTexture", g_pShadowMap[0] );

	g_ShowFatFx->SetFloat( "fRcpFrameWidth", 1.f / g_FrameBuffer->GetWidth() );
	g_ShowFatFx->SetFloat( "fRcpFrameHeight", 1.f / g_FrameBuffer->GetHeight() );

	// set the lightmodel textures
	g_ShowFatFx->SetTexture( "MaterialMapTexture", g_pMaterialFuncs );
	g_ShowFatFx->SetTexture( "DotProductFunctionMapTexture", g_pDotProductFuncs );
	g_ShowFatFx->SetTexture( "AttenuationMapTexture", g_pAttenuationFuncs );

	// store the back buffer surface for later use
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer );

	if (FAILED(pd3dDevice->CreateTexture(g_FrameBuffer->GetWidth(), g_FrameBuffer->GetHeight(), 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &g_pHDRBufferTex, NULL)))
		return E_FAIL;
	if (FAILED(g_pHDRBufferTex->GetSurfaceLevel(0, &g_pHDRBuffer)))
		return E_FAIL;

	// create a 1x1 texture to hold the mean log luminance
	if (FAILED(pd3dDevice->CreateTexture(1, 1, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &g_pMeanLogLuminanceTex, NULL)))
		return E_FAIL;
	if (FAILED(g_pMeanLogLuminanceTex->GetSurfaceLevel(0, &g_pMeanLogLuminance)))
		return E_FAIL;

	// calculate how many passes and how big the temp texture if needed
	int blocksin1pass = d3dCaps.MaxPShaderInstructionsExecuted / 511;
	g_iXBlocks = (int) floor((pBBSurfaceDesc->Width+124.f) / 125.f);
	g_iYBlocks = (int) floor((pBBSurfaceDesc->Height+127.f) / 128);
	if( g_iXBlocks * g_iYBlocks < blocksin1pass )
	{
		// one pass
		g_ToneMapFx->SetTechnique( "T0" );
	} else
	{
		// two pass
		g_ToneMapFx->SetTechnique( "T1" );
		// create a temp texture to hold the log luminance
		if (FAILED(pd3dDevice->CreateTexture(g_iXBlocks, g_iYBlocks, 1, 
				D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &g_pTempLogLuminanceTex, NULL)))
			return E_FAIL;
		if (FAILED(g_pTempLogLuminanceTex->GetSurfaceLevel(0, &g_pTempLogLuminance)))
			return E_FAIL;
	}

	g_ToneMapFx->SetTexture( "BarLwTex", g_pMeanLogLuminanceTex );
	g_ToneMapFx->SetTexture( "HDRTexture", g_pHDRBufferTex );
	g_ToneMapFx->SetFloat( "KeyA", 0.32f );
	g_ToneMapFx->SetFloat( "RecipLwhiteSqrd", 1.f / 1e20f );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

	g_Matrices.m_View = *g_Camera.GetViewMatrix();
	g_Matrices.m_ViewProj = *g_Camera.GetViewMatrix() * *g_Camera.GetProjMatrix();
	g_Matrices.m_Proj = *g_Camera.GetProjMatrix();

	// these shader matrices are shared among all meshes so we just set the first mesh
	if( g_LightHemisphere != 0 )
	{
		// create matrix to takes us from store position into view space position
		D3DXMATRIX matProjectionInvScaled, matScale;
		D3DXMatrixInverse( &matProjectionInvScaled , 0, &g_Matrices.m_Proj );
		D3DXMatrixIdentity( &matScale );
		matScale._11 *= 2;
		matScale._22 *= 2;
		matScale._41 += -1;
		matScale._42 += -1;
		D3DXMatrixMultiply( &matProjectionInvScaled , &matScale, &matProjectionInvScaled );

		// matrix that takes us from G-buffer position to view space position
		g_LightHemisphere->m_Shader->SetMatrix( "matProjectionInvScaled", &matProjectionInvScaled );
	}

}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;
    
    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Get the projection & view matrix from the camera class
        mWorld = *g_Camera.GetWorldMatrix();
        mProj = *g_Camera.GetProjMatrix();
        mView = *g_Camera.GetViewMatrix();

        mWorldViewProjection = mWorld * mView * mProj;

		// render the shadows into the shadow maps
		for(unsigned int i=0;i <MAX_SHADOWS;i++)
		{
			RenderShadowMap(pd3dDevice, i);
		}

		// render the g-buffers, for MRT this should be a single psas
		for(unsigned int passNum = 0; passNum < g_FrameBuffer->GetRenderPassCount();passNum++ )
		{
			if( g_FrameBuffer->GetType() == MT_MRT )
			{
				// Clear render targets
				g_FrameBuffer->Clear( FFB_BUFFER0 | FFB_BUFFER1 | FFB_BUFFER2 | FFB_BUFFER3 | FFB_DEPTH | FFB_STENCIL);
				g_FrameBuffer->SelectBuffersAsRenderTarget( FFB_BUFFER0 | FFB_BUFFER1 | FFB_BUFFER2 | FFB_BUFFER3 | FFB_DEPTH | FFB_STENCIL );
			} else
			{
				// note passNum = 0 to 3 == FFB_BUFFER0 to FFB_BUFFER3
				if( passNum == 0 )
					g_FrameBuffer->Clear( FFB_BUFFER0 | FFB_DEPTH | FFB_STENCIL);
				else
				{
					g_FrameBuffer->Clear( (1 << passNum) );
				}
				g_FrameBuffer->SelectBuffersAsRenderTarget( (1 << passNum) | FFB_DEPTH | FFB_STENCIL );
			}

			// render the objects
			for(unsigned int i=0;i < MAX_OBJECTS;i++)
			{
				if(g_Mesh[i])
					g_Mesh[i]->Render( passNum );
			}
		}

		LightGBuffer(pd3dDevice);

/*		g_ShowFatFx->SetTechnique( "T1" );
		// select just the backbuffer as the target
		pd3dDevice->SetDepthStencilSurface( 0 );
		pd3dDevice->SetRenderTarget( 0, g_pBackBuffer );
		pd3dDevice->SetRenderTarget( 1, 0 );
		pd3dDevice->SetRenderTarget( 2, 0 );
		pd3dDevice->SetRenderTarget( 3, 0 ); 

		pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

		unsigned int numPasses;
		g_ShowFatFx->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
		for(unsigned int i=0;i < numPasses;i++)
		{
			g_ShowFatFx->BeginPass(i);
			ApplyFullScreenPixelShader( pd3dDevice, 320, 240 );
			g_ShowFatFx->EndPass();
		}
		g_ShowFatFx->End();*/


        V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SampleUI.OnRender( fElapsedTime ) );

        V( pd3dDevice->EndScene() );
    }
}

//-----------------------------------------------------------------------------
// Name: BaseLight ApplyFullScreenPixelShader
// Desc: Render a full screen quad with whatever settings are currently set
//-----------------------------------------------------------------------------
void ApplyFullScreenPixelShader( IDirect3DDevice9* pd3dDevice, const unsigned int width, const unsigned int height )
{
	// render pixel shader quad to combine the render target
	struct
	{
		float x,y,z,w;
		float u0,v0;
	} quad[4] = 
	{ 
		{ (float)0,		(float)0,		0.1f,1,		0+(1.f/width)/2.f, 0+(1.f/height)/2.f, }, 
		{ (float)width, (float)0,		0.1f,1,		1+(1.f/width)/2.f, 0+(1.f/height)/2.f, }, 
		{ (float)0,		(float)height,	0.1f,1,		0+(1.f/width)/2.f, 1+(1.f/height)/2.f }, 
		{ (float)width, (float)height,	0.1f,1,		1+(1.f/width)/2.f, 1+(1.f/height)/2.f, } 
	};

	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])  );
}

//-----------------------------------------------------------------------------
// Name: BaseLight ApplyFullScreenPixelShader
// Desc: Render a full screen quad with whatever settings are currently set
//-----------------------------------------------------------------------------
void ApplyFullScreenPixelShader2( IDirect3DDevice9* pd3dDevice, const unsigned int width, const unsigned int height,
																const unsigned int texwidth, const unsigned int texheight)
{
	// render pixel shader quad to combine the render target
	struct
	{
		float x,y,z,w;
		float u0,v0;
	} quad[4] = 
	{ 
		{ (float)0,		(float)0,		0.1f,1,		0+(1.f/texwidth)/2.f, 0+(1.f/texheight)/2.f, }, 
		{ (float)width, (float)0,		0.1f,1,		1+(1.f/texwidth)/2.f, 0+(1.f/texheight)/2.f, }, 
		{ (float)0,		(float)height,	0.1f,1,		0+(1.f/texwidth)/2.f, 1+(1.f/texheight)/2.f }, 
		{ (float)width, (float)height,	0.1f,1,		1+(1.f/texwidth)/2.f, 1+(1.f/texheight)/2.f, } 
	};

	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])  );
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	return;
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    txtHelper.DrawTextLine( L"Put whatever misc status here" );
    
    // Draw help
    if( g_bShowHelp )
    {
        txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-15*6 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Controls (F1 to hide):" );

        txtHelper.SetInsertionPos( 40, pd3dsdBackBuffer->Height-15*5 );
        txtHelper.DrawTextLine( L"Blah: X\n"
                                L"Quit: ESC" );
    }
    else
    {
        txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-15*2 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Press F1 for help" );
    }
    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, the sample framework passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then the sample framework will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing )
{
    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, the sample framework inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke 
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all 
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for 
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice()
{
	// Delete surfaces
	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		SAFE_RELEASE( g_pShadowMapSurf[i] );
		SAFE_RELEASE( g_pShadowMap[i] );
	}
	SAFE_RELEASE( g_pShadowMapZ );

	SAFE_RELEASE( g_pTempLogLuminance );
	SAFE_RELEASE( g_pTempLogLuminanceTex );
	SAFE_RELEASE( g_pMeanLogLuminance );
	SAFE_RELEASE( g_pMeanLogLuminanceTex );
	SAFE_RELEASE( g_pHDRBuffer );
	SAFE_RELEASE( g_pHDRBufferTex );


	SAFE_RELEASE( g_pAttenuationFuncs );

	SAFE_RELEASE( g_pDotProductFuncs );

	SAFE_RELEASE( g_pMaterialFuncs );

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		if(g_Mesh[i])
			g_Mesh[i]->Reset();
	}

	if(g_ShowFatFx)
		g_ShowFatFx->OnLostDevice();

	if(g_ToneMapFx)
		g_ToneMapFx->OnLostDevice();

	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		if(g_LightPoint[i])
			g_LightPoint[i]->Reset();
	}

	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		if(g_LightDirectional[i])
			g_LightDirectional[i]->Reset();
	}

	if(g_LightHemisphere)
		g_LightHemisphere->Reset();

	SAFE_RELEASE( g_pBackBuffer );
	SAFE_DELETE( g_FrameBuffer );


    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
	for(unsigned int i=0;i < MAX_POINT_LIGHTS;i++)
	{
		SAFE_DELETE( g_LightPoint[i] );
	}
	for(unsigned int i=0;i < MAX_SHADOWS;i++)
	{
		SAFE_DELETE( g_LightDirectional[i] );
	}
	SAFE_DELETE( g_LightHemisphere );
	SAFE_RELEASE( g_ShowFatFx );
	SAFE_RELEASE( g_ToneMapFx );

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		SAFE_DELETE( g_Mesh[i] );
	}

	SAFE_RELEASE( g_Objs.m_LightEffectPool );
	SAFE_RELEASE( g_Objs.m_EffectPool );

    SAFE_RELEASE( g_pFont );
}

// assumes render target is already set and that the texture is ready set in stage 0
void CopyTextureToRenderTarget( IDirect3DDevice9* pd3dDevice )
{

	pd3dDevice->SetPixelShader( 0 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_RESULTARG, D3DTA_CURRENT );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
	ApplyFullScreenPixelShader( pd3dDevice, pd3dsdBackBuffer->Width, pd3dsdBackBuffer->Height );
}

void LightGBuffer( IDirect3DDevice9* pd3dDevice )
{
	// simple demo that this is HDR every 5 frame (which on REFRAST is about once a minute...
	// flick the main directional light off
	static int DirectSwitch = 0;
	DirectSwitch++;
	if( DirectSwitch > 5 )
	{
		g_LightDirectional[0]->m_LightIntensity = 1.f / g_LightDirectional[0]->m_LightIntensity;
		DirectSwitch = 0;
	}

	// select just the backbuffer as the target
	// reuse the depth buffer from the fat framebuffer to enable depth cull optizamations
	pd3dDevice->SetDepthStencilSurface( g_FrameBuffer->GetBufferSurface( FFB_DEPTH ) );
	pd3dDevice->SetRenderTarget( 0, g_pHDRBuffer );
	pd3dDevice->SetRenderTarget( 1, 0 );
	pd3dDevice->SetRenderTarget( 2, 0 );
	pd3dDevice->SetRenderTarget( 3, 0 ); 

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	UpdatePointLights();

	// update all active lights
	for(unsigned int j=0;j < MAX_LIGHTS;j++)
	{
		if( g_ActiveLights[j] != NULL)
			g_ActiveLights[j]->Update();
	}

	// render all active lights
	for(unsigned int j=0;j < MAX_LIGHTS;j++)
	{
		if( g_ActiveLights[j] != NULL)
			g_ActiveLights[j]->Render();
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	// select just the backbuffer as the target
	pd3dDevice->SetDepthStencilSurface( 0 );
	pd3dDevice->SetRenderTarget( 1, 0 );
	pd3dDevice->SetRenderTarget( 2, 0 );
	pd3dDevice->SetRenderTarget( 3, 0 ); 

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();

	int numPixels = pd3dsdBackBuffer->Width * pd3dsdBackBuffer->Height;
	D3DXVECTOR4 c0( 1.f/(float)numPixels, 0, 0, 0);
	D3DXVECTOR4 c1( 1.f/(float)pd3dsdBackBuffer->Width, 1.f/(float)pd3dsdBackBuffer->Height, 0, 1);

	unsigned int numPasses;
	g_ToneMapFx->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	int intVec2[4] = { 128,0,0,0 };
	int intVec3[4] = { g_iXBlocks,0,0,0 };
	int intVec4[4] = { g_iYBlocks,0,0,0 };

	HRESULT hr;

	if( numPasses == 2 )
	{
		// one pass generated average log luminance
		V( g_ToneMapFx->BeginPass(0) );
		V( pd3dDevice->SetRenderTarget( 0, g_pMeanLogLuminance) );
		V( pd3dDevice->SetTexture( 0, g_pHDRBufferTex ) );
		V( pd3dDevice->SetPixelShaderConstantF( 0, (float*)&c0, 1 ) );
		V( pd3dDevice->SetPixelShaderConstantF( 1, (float*)&c1, 1 ) );
		V( pd3dDevice->SetPixelShaderConstantI( 2, intVec2, 1 ) );
		V( pd3dDevice->SetPixelShaderConstantI( 3, intVec3, 1 ) );
		V( pd3dDevice->SetPixelShaderConstantI( 4, intVec4, 1 ) );
		ApplyFullScreenPixelShader2( pd3dDevice, 1, 1, pd3dsdBackBuffer->Width, pd3dsdBackBuffer->Height );
		V( g_ToneMapFx->EndPass() );
	} else
	{
		// two pass generated average log luminance
		V( g_ToneMapFx->BeginPass(0) );
		V( pd3dDevice->SetRenderTarget( 0, g_pTempLogLuminance) );
		V( pd3dDevice->SetTexture( 0, g_pHDRBufferTex ) );
		V( pd3dDevice->SetPixelShaderConstantF( 0, (float*)&c0, 1 ) );
		V( pd3dDevice->SetPixelShaderConstantF( 1, (float*)&c1, 1 ) );
		V( pd3dDevice->SetPixelShaderConstantI( 2, intVec2, 1 ) );
		ApplyFullScreenPixelShader2( pd3dDevice, g_iXBlocks, g_iYBlocks, pd3dsdBackBuffer->Width, pd3dsdBackBuffer->Height );
		V( g_ToneMapFx->EndPass() );

		D3DXVECTOR4 c1b( 1.f/(float)g_iXBlocks, 1.f/(float)g_iYBlocks, 0, 1);
		// take the small tmp render target and add and average it into the MeanLogLuminance
		g_ToneMapFx->BeginPass(1);

		pd3dDevice->SetPixelShaderConstantF( 0, (float*)&c0, 1 );
		pd3dDevice->SetPixelShaderConstantF( 1, (float*)&c1b, 1 );
		pd3dDevice->SetPixelShaderConstantI( 3, intVec3, 1 );
		pd3dDevice->SetPixelShaderConstantI( 4, intVec4, 1 );
		pd3dDevice->SetRenderTarget( 0, g_pMeanLogLuminance);
		V( pd3dDevice->SetTexture( 0, g_pTempLogLuminanceTex ) );
		ApplyFullScreenPixelShader2( pd3dDevice, 1, 1, g_iXBlocks, g_iYBlocks );
		g_ToneMapFx->EndPass();
	}

	// now use the mean log luminance to map into LDR
	g_ToneMapFx->BeginPass(numPasses-1);
	pd3dDevice->SetRenderTarget( 0, g_pBackBuffer );
	ApplyFullScreenPixelShader( pd3dDevice, pd3dsdBackBuffer->Width, pd3dsdBackBuffer->Height );
	g_ToneMapFx->EndPass();

	g_ToneMapFx->End();

}

//-----------------------------------------------------------------------------
// Name: RenderShadowMap()
// Desc: Called to render shadow map for the light. Outputs per-pixel distance
//       normalized to 0..1 range to the floating point buffer.
//-----------------------------------------------------------------------------
void RenderShadowMap(  IDirect3DDevice9* pd3dDevice, unsigned int shadowNum )
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

		D3DXMatrixRotationYawPitchRoll( &g_LightDirectional[0]->m_WorldMatrix, testX, -0.5f, 0 );
		const D3DXVECTOR3 zAxis(0,0,1);
		// extract the view direction (faces down the z)
		D3DXVec3TransformNormal( &vLightDir, &zAxis, &g_LightDirectional[0]->m_WorldMatrix );
	} else
	{
		static float testX2 = 0.0f;
		testX2 -= 0.014f;

		D3DXMatrixRotationYawPitchRoll( &g_LightDirectional[1]->m_WorldMatrix, testX2, 0.2, 0 );
		const D3DXVECTOR3 zAxis(0,0,1);
		// extract the view direction (faces down the z)
		D3DXVec3TransformNormal( &vLightDir, &zAxis, &g_LightDirectional[1]->m_WorldMatrix );
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
		g_LightDirectional[0]->m_Shader->SetMatrix( "matShadowWarp", &matWarp );
		g_LightDirectional[0]->m_Shader->SetTexture( "ShadowMapTexture", g_pShadowMap[shadowNum] );
	} else
	{
		g_LightDirectional[1]->m_Shader->SetMatrix( "matShadowWarp", &matWarp );
		g_LightDirectional[1]->m_Shader->SetTexture( "ShadowMapTexture", g_pShadowMap[shadowNum] );
	}

	D3DXVECTOR4 jit0 = D3DXVECTOR4(0.1f / SHADOW_MAP_SIZE, 0.1f / SHADOW_MAP_SIZE, 0, 0 );
	D3DXVECTOR4 jit1 = D3DXVECTOR4(0.1f / SHADOW_MAP_SIZE, 0.9f / SHADOW_MAP_SIZE, 0, 0 );
	D3DXVECTOR4 jit2 = D3DXVECTOR4(0.9f / SHADOW_MAP_SIZE, 0.1f / SHADOW_MAP_SIZE, 0, 0 );
	D3DXVECTOR4 jit3 = D3DXVECTOR4(0.9f / SHADOW_MAP_SIZE, 0.9f / SHADOW_MAP_SIZE, 0, 0 );

	g_LightDirectional[0]->m_Shader->SetVector( "f4ShadowJitter0", &jit0 );
	g_LightDirectional[0]->m_Shader->SetVector( "f4ShadowJitter1", &jit1 );
	g_LightDirectional[0]->m_Shader->SetVector( "f4ShadowJitter2", &jit2 );
	g_LightDirectional[0]->m_Shader->SetVector( "f4ShadowJitter3", &jit3 );

	g_Matrices.m_ShadowView = matLightView;
	g_Matrices.m_ShadowViewProj = matLightViewProj;

	// Save old viewport
	pd3dDevice->GetViewport(&oldViewport);

	// Set new render target
	pd3dDevice->SetRenderTarget(0, g_pShadowMapSurf[shadowNum] );
	pd3dDevice->SetDepthStencilSurface(g_pShadowMapZ);

	// Setup shadow map viewport
	D3DVIEWPORT9 shadowViewport;
	shadowViewport.X = 0;
	shadowViewport.Y = 0;
	shadowViewport.Width  = SHADOW_MAP_SIZE;
	shadowViewport.Height = SHADOW_MAP_SIZE;
	shadowViewport.MinZ = 0.0f;
	shadowViewport.MaxZ = 1.0f;
	pd3dDevice->SetViewport(&shadowViewport);

	// Clear viewport
    pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0L);

	for(unsigned int i=0;i < MAX_OBJECTS;i++)
	{
		if(g_Mesh[i] && g_Mesh[i]->m_CastShadow)
			g_Mesh[i]->RenderShadow();
	}

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Restore old viewport
	pd3dDevice->SetViewport(&oldViewport);
}


HRESULT CreateTestObjects(  IDirect3DDevice9* pd3dDevice )
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
	g_Mesh[0] = new DefMesh;
	if( FAILED( hr = g_Mesh[0]->CreateBuiltIn(	DefMesh::BT_TEAPOT, 
												TEXT("GeomTextured.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomTextured.fx", hr );
	matNum = RegisterMaterial( Material(0.2f, 0.0f, 0.2f ) );
	matVec = D3DXVECTOR4(0.7f,1.0f, MaterialIndexToV(matNum),0 );
	g_Mesh[0]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW128 ), 0, 0 );
	g_Mesh[0]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	LPDIRECT3DTEXTURE9 difTex = 0;
	D3DXCreateTextureFromFile( pd3dDevice, L"diffuse.png", &difTex );
	g_Mesh[0]->m_Shader->SetTexture( "DiffuseMapTexture", difTex );
	SAFE_RELEASE( difTex );

	LPDIRECT3DTEXTURE9 specTex = 0;
	D3DXCreateTextureFromFile( pd3dDevice, L"diffuse.png", &specTex );
	g_Mesh[0]->m_Shader->SetTexture( "SpecularMapTexture", specTex );
	SAFE_RELEASE( specTex );

	LPDIRECT3DTEXTURE9 normTex0 = 0;
	LPDIRECT3DTEXTURE9 normTex1 = 0;

	// load it twice so the second one is set same format, size etc
	D3DXCreateTextureFromFile( pd3dDevice, L"bumps.png", &normTex0 );
	D3DXCreateTextureFromFile( pd3dDevice, L"bumps.png", &normTex1 );

	D3DXComputeNormalMap( normTex0, normTex1, 0, 0, D3DX_CHANNEL_LUMINANCE, 100 );
	g_Mesh[0]->m_Shader->SetTexture( "NormalMapTexture", normTex0 );

	SAFE_RELEASE( normTex0 );
	SAFE_RELEASE( normTex1 );

	// floor
	g_Mesh[1] = new DefMesh;
	if( FAILED( hr = g_Mesh[1]->CreateBuiltIn(	DefMesh::BT_CUBE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 100, 0.1f, 100);
	D3DXMatrixTranslation( &m2, 0, -1, 0 );
	D3DXMatrixMultiply( &g_Mesh[1]->m_WorldMatrix, &m1,&m2);
	g_Mesh[1]->m_CastShadow = false; // self shadowing problems so as the don't shadow anything easier then tweaking

	matNum = MATERIAL_FULL_AMBIENT; // used an already created material
	matVec = D3DXVECTOR4(0.5f,1.0f, MaterialIndexToV(matNum),0 );
	g_Mesh[1]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW32), 0, 0 );
	g_Mesh[1]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	// back wall
	g_Mesh[2] = new DefMesh;
	if( FAILED( hr = g_Mesh[2]->CreateBuiltIn(	DefMesh::BT_CUBE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 100, 100, 0.1f );
	D3DXMatrixTranslation( &m2, 0, 0, 15 );
	D3DXMatrixMultiply( &g_Mesh[2]->m_WorldMatrix, &m1,&m2);
	g_Mesh[2]->m_CastShadow = false; // self shadowing problems so as the don't shadow anything easy then tweaking

	matNum = MATERIAL_FULL_AMBIENT; // used an already created material
	matVec = D3DXVECTOR4(0.5f,1.0f, MaterialIndexToV(matNum),0 );
	g_Mesh[2]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4( GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU(DPF_FRONT_POW32), 0, 0 );
	g_Mesh[2]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	// shadow casting ball
	g_Mesh[3] = new DefMesh;
	if( FAILED( hr = g_Mesh[3]->CreateBuiltIn(	DefMesh::BT_SPHERE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 1,1, 1);
	D3DXMatrixTranslation( &m2, 3, 0.0f, 1 );
	D3DXMatrixMultiply( &g_Mesh[3]->m_WorldMatrix, &m1,&m2);
	// create a oren-nayer style surface with standard specular
	matNum = MATERIAL_STD_SPECULAR; // used an already created material
	matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 );
	g_Mesh[3]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW4 ), 
							GetDotProductFunctionU( DPF_ZERO), 0, 0 );
	g_Mesh[3]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

	// teapot at the back
	g_Mesh[4] = new DefMesh;
	if( FAILED( hr = g_Mesh[4]->CreateBuiltIn(	DefMesh::BT_TEAPOT, TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 1, 1, 1);
	D3DXMatrixTranslation( &m2, -3, 0.0f, -1 );
	D3DXMatrixMultiply( &g_Mesh[4]->m_WorldMatrix, &m1,&m2);

	// create a blinn/phong plastic specular material with low specular exponent
	matNum = RegisterMaterial( Material(0.3f, 0.2f, 1.0f ) );
	matVec = D3DXVECTOR4(0.7f,0.8f, MaterialIndexToV(matNum),0 );
	g_Mesh[4]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW16 ), 0, 0 );
	g_Mesh[4]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );
	col = D3DXVECTOR4(1,1,1,1);
	g_Mesh[4]->m_Shader->SetVector( "f3Colour0", &col );

	// donut
	g_Mesh[5] = new DefMesh;
	if( FAILED( hr = g_Mesh[5]->CreateBuiltIn(	DefMesh::BT_TORUS, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 2, 2, 2);
	D3DXMatrixTranslation( &m2, 1, 0.0f, 5 );
	D3DXMatrixMultiply( &g_Mesh[5]->m_WorldMatrix, &m1,&m2);

	// normal specular 
	col = D3DXVECTOR4(1,1,1,1);
	matNum = RegisterMaterial( Material(0.3f, 0.f, 1.0f ) ); // KAmb=0.3, KEmm=0.f, Phong hilight 
	matVec = D3DXVECTOR4(0.3f,1.0f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
	g_Mesh[5]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1 ), 
							GetDotProductFunctionU( DPF_FRONT_POW256 ), 0, 0 );
	g_Mesh[5]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=POW1, Fb=POW256
	g_Mesh[5]->m_Shader->SetVector( "f3Colour0", &col );

	// ellisoid
	g_Mesh[6] = new DefMesh;
	if( FAILED( hr = g_Mesh[6]->CreateBuiltIn(	DefMesh::BT_SPHERE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 1,3, 1);
	D3DXMatrixTranslation( &m2, 2, 0.0f, 1 );
	D3DXMatrixMultiply( &g_Mesh[6]->m_WorldMatrix, &m1,&m2);
	col = D3DXVECTOR4(1,0,0,1);
	g_Mesh[6]->m_Shader->SetVector( "f3Colour0", &col );
	g_Mesh[6]->m_Shader->SetVector( "f3Colour1", &col );
	// weird
	matNum = RegisterMaterial( Material(0.3f, 0.3f, 0.7f ) ); // KAmb=0.3, KEmm=0.3f, bit of both hilight!
	matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
	g_Mesh[6]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW2 ), 
							GetDotProductFunctionU( DPF_FRONT_POW64 ), 0, 0 );
	g_Mesh[6]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=POW16, Fb=POW64

	g_Mesh[7] = new DefMesh;
	if( FAILED( hr = g_Mesh[7]->CreateBuiltIn(	DefMesh::BT_SPHERE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );

	D3DXMatrixScaling( &m1, 2,1, 1);
	D3DXMatrixTranslation( &m2, -2, 0, 3 );
	D3DXMatrixMultiply( &g_Mesh[7]->m_WorldMatrix, &m1,&m2);
	col = D3DXVECTOR4(1,1,1,1);
	g_Mesh[7]->m_Shader->SetVector( "f3Colour0", &col );
	// very weird
	matNum = RegisterMaterial( Material(1.3f, 0.3f, 0.4f ) ); // KAmb=1.3, KEmm=0.3f, bit of both hilight!
	matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
	g_Mesh[7]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRESNEL_POW4 ), 
							GetDotProductFunctionU( DPF_SHIFTED_FRESNEL_POW8 ), 0, 0 );
	g_Mesh[7]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=???, Fb=???

	g_Mesh[8] = new DefMesh;
	if( FAILED( hr = g_Mesh[8]->CreateBuiltIn(	DefMesh::BT_CUBE, 
												TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 1,1, 1);
	D3DXMatrixTranslation( &m2, 5, -1, 5 );
	D3DXMatrixMultiply( &g_Mesh[8]->m_WorldMatrix, &m1,&m2);
	matNum = MATERIAL_STD_SPECULAR; // use an already created material
	matVec = D3DXVECTOR4(0.5f,1.0f, MaterialIndexToV(matNum),0 );
	g_Mesh[8]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1 ), 
							GetDotProductFunctionU( DPF_FRONT_POW32 ), 0, 0 );
	g_Mesh[8]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );

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

		g_Mesh[i] = new DefMesh;
		if( FAILED( hr = g_Mesh[i]->CreateBuiltIn(	DefMesh::BT_CYLINDER, 
													TEXT("GeomSmoothSolidColour.fx") ) ) )
			return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );
		D3DXVECTOR3 scale( 1, 4, 1);
		D3DXMatrixTransformation( &g_Mesh[i]->m_WorldMatrix, 0, 0, &scale, 0, 0, &pos );

		// normal specular 
		matVec = D3DXVECTOR4(0.5f,0.5f, MaterialIndexToV(matNum),0 ); // Kd=0.5, Ks=0.5, MatIndex
		g_Mesh[i]->m_Shader->SetVector( "f3Material", &matVec );
		dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW2 ), 
								GetDotProductFunctionU( DPF_FRONT_POW32 ), 0, 0 );
		g_Mesh[i]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec ); // Fa=POW2, Fb=POW32

		col = D3DXVECTOR4(0,(float)i/19.f,(float)i/19.f,1);
		g_Mesh[i]->m_Shader->SetVector( "f3Colour0", &col );
	}

	// teapot at the back left
	g_Mesh[19] = new DefMesh;
	if( FAILED( hr = g_Mesh[19]->CreateBuiltIn(	DefMesh::BT_TEAPOT, TEXT("GeomSmoothSolidColour.fx") ) ) )
        return DXTRACE_ERR( L"CreateBuiltIn GeomSmoothSolidColour.fx", hr );
	D3DXMatrixScaling( &m1, 1, 1, 1);
	D3DXMatrixTranslation( &m2, 4, 0.0f, 0 );
	D3DXMatrixMultiply( &g_Mesh[19]->m_WorldMatrix, &m1,&m2);

	// create a blinn/phong specular material coloured surface with low specular exponent
	matNum = RegisterMaterial( Material(0.2f, 0.0f, 0.0f ) );
	matVec = D3DXVECTOR4(0.8f,0.8f, MaterialIndexToV(matNum),0 );
	g_Mesh[19]->m_Shader->SetVector( "f3Material", &matVec );
	dpfVec = D3DXVECTOR4(	GetDotProductFunctionU( DPF_FRONT_POW1), 
							GetDotProductFunctionU( DPF_FRONT_POW128 ), 0, 0 );
	g_Mesh[19]->m_Shader->SetVector( "f2DotProductFuncs", &dpfVec );
	col = D3DXVECTOR4(1,0.675f,0.275f,1);
	g_Mesh[19]->m_Shader->SetVector( "f3Colour0", &col );
	g_Mesh[19]->m_Shader->SetVector( "f3Colour1", &col );


	return S_OK;
}

void UpdatePointLights()
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

		g_LightPoint[j]->m_Radius = scale.x / 2;

		scale = D3DXVECTOR3( radius * 2, radius * 2, radius * 2);
		D3DXMatrixTransformation( &mat, 0, 0, &scale, 0, 0, &pos );
		g_LightPoint[j]->m_WorldMatrix = mat;

		unsigned int colr = j % 4;
		switch(colr)
		{
		case 0 :
			g_LightPoint[j]->m_LightColour = D3DXVECTOR4(1,0,0,0);
			break;
		case 1 :
			g_LightPoint[j]->m_LightColour = D3DXVECTOR4(0,1,0,0);
			break;
		case 2 :
			g_LightPoint[j]->m_LightColour = D3DXVECTOR4(0,0,1,0);
			break;
		case 3 :
			g_LightPoint[j]->m_LightColour = D3DXVECTOR4(1,1,1,0);
			break;
		}

		pos.y += 0.0f;
		theta += 6.28f / MAX_POINT_LIGHTS;
	}
}

void MatrixInfinitePerspectiveFovLH( D3DXMATRIX* matProj, 
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
VOID UpdateCullInfo( CULLINFO* pCullInfo )
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

