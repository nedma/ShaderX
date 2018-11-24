#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "dxfile.h"
#include <DX9SDKSampleFramework/DX9SDKSampleFramework.h>
#include "nv_RainbowEffect.h"
#include "shared\GetFilePath.h"
#include "resource.h"
#include "HLSL_RainbowFogbow.h"
 
//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
RainbowFogbowSDKDemo* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    RainbowFogbowSDKDemo d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if (FAILED(d3dApp.Create(hInst)))
        return 0;

    return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: RainbowFogbowSDKDemo()
// Desc: Application constructor.   Paired with ~RainbowFogbowSDKDemo()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
RainbowFogbowSDKDemo::RainbowFogbowSDKDemo()
{
    m_dwCreationWidth           = 512;
    m_dwCreationHeight          = 512;
    m_strWindowTitle            = TEXT("HLSL_RainbowFogbow");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;

    m_bStartFullscreen          = false;
    m_bShowCursorWhenFullscreen = false;

    m_pFont                     = NULL;
    m_bLoadingApp               = TRUE;

    ZeroMemory(&m_UserInput, sizeof(m_UserInput));

	//NVIDIA effect//////////////////////////////////
	m_d3dEnumeration.AppMinAlphaChannelBits = 8;	//we need a backbuffer with ALPHA!
	
	m_pSkyBoxMesh						= new CD3DMesh();
	m_pSkyBoxMoistureMesh				= new CD3DMesh();
	m_pTerrainMesh						= new CD3DMesh();
	m_pNVRainbowEffect					= new nv_RainbowEffect();
	m_rainbowDropletRadius				= 0.81f;
	m_rainbowIntensity					= 1.0f;
	m_pObjectEffects					= NULL;
	m_pSkyBoxRainEffect					= NULL;
	m_hTechniqueRenderObjectsNormal		= NULL;
	m_hTechniqueRenderObjectsBlack		= NULL; 
	m_hTechniqueRenderSkyBoxMoisture	= NULL;
	m_VisualizeRenderSteps				= false;

	m_RainbowDataTextures.push_back("Textures\\2D\\r200_800_Artist.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r5.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r10.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r50.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r100.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r300.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r500.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r800.tga");
	m_RainbowDataTextures.push_back("Textures\\2D\\r1000.tga");
	m_currentRainbowDataTexture = 0;
	

}

//-----------------------------------------------------------------------------
// Name: ~RainbowFogbowSDKDemo()
// Desc: Application destructor.  Paired with RainbowFogbowSDKDemo()
//-----------------------------------------------------------------------------
RainbowFogbowSDKDemo::~RainbowFogbowSDKDemo()
{
	SAFE_DELETE(m_pSkyBoxMesh);
	SAFE_DELETE(m_pSkyBoxMoistureMesh);
	SAFE_DELETE(m_pTerrainMesh);
	SAFE_DELETE(m_pNVRainbowEffect);
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::OneTimeSceneInit()
{
	//NVIDIA effect//////////////////////////////////

	//these vectors are hand tweaked to position the initial viewer nicely,
	//and to match the sun in our shader with the sun in the skybox.
	D3DXVECTOR3 vecEye(0.0f, -5.0f, 80.0f);
    D3DXVECTOR3 vecAt (-20.0f, 18.7f,-1.0f);
	m_sunLightDirection = D3DXVECTOR4( -1.0f , -0.69f , -1.1f , 0.0f );

	m_camera.SetViewParams( &vecEye, &vecAt );

	/////////////////////////////////////////////////

    // Drawing loading status message until app finishes loading
    SendMessage(m_hWnd, WM_PAINT, 0, 0);

    m_bLoadingApp = FALSE;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior, 
                                         D3DFORMAT adapterFormat, 
                                         D3DFORMAT backBufferFormat)
{
    UNREFERENCED_PARAMETER(pCaps);
    UNREFERENCED_PARAMETER(dwBehavior);
    UNREFERENCED_PARAMETER(adapterFormat);

	if(backBufferFormat != D3DFMT_A8R8G8B8)
		return E_FAIL;

 
    return S_OK;
}


//helper to set proper directories and find paths and such
HRESULT	RainbowFogbowSDKDemo::LoadMeshHelperFunction(CD3DMesh* mesh, std::string meshFile)
{

	HRESULT hr;
	std::string path;
	std::string mediaFileDir;
	std::string::size_type lastSlashPos;

	path = GetFilePath::GetFilePath(meshFile.c_str(), false );
	lastSlashPos = path.find_last_of("\\", path.size());
	if (lastSlashPos != path.npos)
		mediaFileDir = path.substr(0, lastSlashPos);
	else
		mediaFileDir = ".";

	if(path.empty())
		return D3DAPPERR_MEDIANOTFOUND;

	char currDir[512];
	GetCurrentDirectory(512,currDir);

	//note the mesh needs the current working directory to be set so that it
	//can properly load the textures
	SetCurrentDirectory(mediaFileDir.c_str());
	hr = mesh->Create( m_pd3dDevice, path.c_str() );
	SetCurrentDirectory(currDir);

	if( FAILED( hr ) )
        return D3DAPPERR_MEDIANOTFOUND;
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
HRESULT RainbowFogbowSDKDemo::InitDeviceObjects()
{
	HRESULT hr;

	//skybox
	hr = LoadMeshHelperFunction(m_pSkyBoxMesh, "models\\RainbowFogbowModels\\rainbowFogBow_skyBox.x");
	if( FAILED( hr ) )
        return hr;

	//skybox with noise textures
	hr = LoadMeshHelperFunction(m_pSkyBoxMoistureMesh, "models\\RainbowFogbowModels\\rainbowFogBow_skyBox_Noise.x");
	if( FAILED( hr ) )
        return hr;

	//a ground plane just to show it in the scene
	hr = LoadMeshHelperFunction(m_pTerrainMesh, "models\\RainbowFogbowModels\\rainbowFogBow_terrain.x");
	if( FAILED( hr ) )
        return hr;


	//our rainbow effect
	m_pNVRainbowEffect->Create(m_pd3dDevice);

	std::string path = GetFilePath::GetFilePath("HLSL\\RainbowFogbowSDKHelpers.fx", false );
	if(path.empty())
		return D3DAPPERR_MEDIANOTFOUND;
	hr = D3DXCreateEffectFromFile(m_pd3dDevice, path.c_str(), NULL, NULL, 0, NULL, &m_pObjectEffects, NULL );
    if (FAILED(hr))
        m_pObjectEffects = NULL;

	path = GetFilePath::GetFilePath("HLSL\\RainbowFogbowNoiseScroll.fx", false );
	if(path.empty())
		return D3DAPPERR_MEDIANOTFOUND;
	hr = D3DXCreateEffectFromFile(m_pd3dDevice, path.c_str(), NULL, NULL, 0, NULL, &m_pSkyBoxRainEffect, NULL );
    if (FAILED(hr))
        m_pSkyBoxRainEffect = NULL;



    // Initialize the font
    HDC hDC = GetDC(NULL);
    int nHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    ReleaseDC(NULL, hDC);
    if (FAILED(hr = D3DXCreateFont(m_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE, 
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
                                   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                                   TEXT("Arial"), &m_pFont)))
        return DXTRACE_ERR("D3DXCreateFont", hr);


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
HRESULT RainbowFogbowSDKDemo::RestoreDeviceObjects()
{

	m_camera.SetProjParams( D3DXToRadian(60.0f), m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height, 1.0f, 505.0f );
	m_camera.SetScalers(0.01f, 15.0f);

	m_pSkyBoxMesh->RestoreDeviceObjects(m_pd3dDevice);
	m_pSkyBoxMoistureMesh->RestoreDeviceObjects(m_pd3dDevice);
	m_pTerrainMesh->RestoreDeviceObjects(m_pd3dDevice);
	m_pNVRainbowEffect->RestoreDeviceObjects(m_pd3dDevice, m_RainbowDataTextures[m_currentRainbowDataTexture].c_str());
	if(m_pObjectEffects)
		m_pObjectEffects->OnResetDevice();
	if(m_pSkyBoxRainEffect)
		m_pSkyBoxRainEffect->OnResetDevice();

	m_hTechniqueRenderObjectsNormal     = m_pObjectEffects->GetTechniqueByName("texturedWithFogFactor");
	m_hTechniqueRenderObjectsBlack      = m_pObjectEffects->GetTechniqueByName("blackWithFogFactor");
	m_hTechniqueRenderSkyBoxMoisture	= m_pObjectEffects->GetTechniqueByName("skyBoxMoisture");

	D3DXHANDLE hTechnique = m_pSkyBoxRainEffect->GetTechniqueByName("ScrollingNoise");
	m_pSkyBoxRainEffect->SetTechnique(hTechnique);

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
  
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );



	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Set the world matrix
    D3DXMatrixIdentity( &m_worldTransform );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_worldTransform );

    m_pd3dDevice->SetTransform( D3DTS_VIEW, m_camera.GetViewMatrix() );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, m_camera.GetProjMatrix() );


    if (m_pFont)
        m_pFont->OnResetDevice();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::FrameMove()
{
	m_camera.FrameMove(m_fElapsedTime);

    // Update user input state
    UpdateInput(&m_UserInput);

    // Update the world state according to user input
    m_pd3dDevice->SetTransform( D3DTS_VIEW, m_camera.GetViewMatrix() );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, m_camera.GetProjMatrix() );

	D3DXMATRIX projInv;
	D3DXMatrixInverse(&projInv, NULL, m_camera.GetProjMatrix());

	m_pNVRainbowEffect->SetViewMatrix(m_camera.GetViewMatrix());
	m_pNVRainbowEffect->SetProjInvMatrix(&projInv);
	m_pNVRainbowEffect->SetDropletRadius(m_rainbowDropletRadius);
	m_pNVRainbowEffect->SetRainbowIntensity(m_rainbowIntensity);



    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void RainbowFogbowSDKDemo::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = (m_bActive && KeyPressed(VK_UP));
    pUserInput->bRotateDown  = (m_bActive && KeyPressed(VK_DOWN));
    pUserInput->bRotateLeft  = (m_bActive && KeyPressed(VK_LEFT));
    pUserInput->bRotateRight = (m_bActive && KeyPressed(VK_RIGHT));

    if (m_bActive && KeyPressed(VK_ESCAPE))
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}
void RainbowFogbowSDKDemo::SetUpMatriceeesForEffect( D3DXMATRIX* view, D3DXMATRIX* world, D3DXMATRIX* proj)
{
	//set up matrices
	D3DXMATRIX tmp1, tmp2;
	D3DXMatrixMultiply(&tmp1, world,view);
	D3DXMatrixMultiply(&tmp2, &tmp1, proj);
	m_pObjectEffects->SetMatrix("worldViewProj", &tmp2);
	D3DXMatrixInverse(&tmp1, NULL, view);
	m_pObjectEffects->SetMatrix("viewInverse", &tmp1);
	m_pObjectEffects->SetMatrix("world", world);
	D3DXMatrixInverse(&tmp1, NULL, world);
	D3DXMatrixTranspose(&tmp2, &tmp1);
	m_pObjectEffects->SetMatrix("worldInverseTranspose", &tmp2);

}

//helper function to modify the translation of a matrix
//made to move the viewer to be inside the skybox when rendering a skybox
void SetD3DXMatrixTranslationTo(float x, float y, float z, D3DXMATRIX* pMat)
{
	assert(pMat && "null matrix");
	pMat->_41 = x; 
	pMat->_42 = y; 
	pMat->_43 = z;
}

void RainbowFogbowSDKDemo::RenderMoisturePass()
{
	float moistureDensity = 0.5f;
	D3DXCOLOR fogMoisture( moistureDensity , m_rainbowDropletRadius , 0.1f , 1.0f );


	// Clear the viewport
	m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						fogMoisture, 1.0f, 0L);

	//render objects
	SetUpMatriceeesForEffect(	m_camera.GetViewMatrix(), 
						&m_worldTransform ,
						m_camera.GetProjMatrix()	);
	m_pObjectEffects->SetTechnique( m_hTechniqueRenderObjectsBlack );
	UINT passes;
	UINT i;
	m_pObjectEffects->Begin( &passes , 0 );
	for(i = 0 ; i < passes ; ++i )
	{
		m_pObjectEffects->BeginPass(i);
		m_pTerrainMesh->Render(m_pd3dDevice);
		m_pObjectEffects->EndPass();
	}
	m_pObjectEffects->End();

	// Render the Skybox

	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_DESTALPHA  );
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA  );

	D3DXMATRIXA16 save_matView =  *(m_camera.GetViewMatrix());
	D3DXMATRIXA16 matView = save_matView;
	SetD3DXMatrixTranslationTo(0.0f, -3.0f, 0.0f, &matView);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,      &matView );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	m_pObjectEffects->SetTechnique( m_hTechniqueRenderSkyBoxMoisture );
	SetUpMatriceeesForEffect(&matView,&m_worldTransform,m_camera.GetProjMatrix());

	// Render the skybox
	m_pObjectEffects->Begin(&passes, 0 );
	for( i = 0 ; i < passes ; ++i)
	{
		m_pObjectEffects->BeginPass(i);
		m_pSkyBoxMesh->Render( m_pd3dDevice );
		m_pObjectEffects->EndPass();
	}
	m_pObjectEffects->End();

	//render rainsheets noise skybox--------------
	//multiply this noise times our existing moisture
	D3DXMATRIX tmp1, tmp2;
	D3DXMatrixMultiply(&tmp1, &m_worldTransform, &matView);
	D3DXMatrixMultiply(&tmp2, &tmp1, m_camera.GetProjMatrix());
	m_pSkyBoxRainEffect->SetMatrix( "worldViewProj", &tmp2 );
	m_pSkyBoxRainEffect->SetMatrix( "world", &m_worldTransform );

  	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );

	m_pSkyBoxRainEffect->SetFloat("speed", 10.0f);
	m_pSkyBoxRainEffect->SetFloat("time", m_fTime);
	
	D3DXVECTOR4 rainVector( 0.0, -1.0, 0.0, 0.0);
	m_pSkyBoxRainEffect->SetVector( "rainVec", &rainVector );

	m_pSkyBoxRainEffect->Begin(&passes, 0 );
	for( i = 0 ; i < passes ; ++i)
	{
		m_pSkyBoxRainEffect->BeginPass(i);
		m_pSkyBoxMoistureMesh->Render( m_pd3dDevice );
		m_pSkyBoxRainEffect->EndPass();
	}
	m_pSkyBoxRainEffect->End();


	// Restore the render states
	m_pd3dDevice->SetTransform( D3DTS_VIEW, m_camera.GetViewMatrix() );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

}

void RainbowFogbowSDKDemo::RenderSceneNormally()
{

	//since our skybox will blend based on alpha we have to clear the backbuffer to this alpha value
	D3DXCOLOR fogColor( 0.0f , 0.0f , 0.0f , 1.0f ); 
	m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						fogColor , 1.0f, 0L);


	SetUpMatriceeesForEffect(	m_camera.GetViewMatrix(), 
								&m_worldTransform ,
								m_camera.GetProjMatrix()	);
	m_pObjectEffects->SetTechnique( m_hTechniqueRenderObjectsNormal );

	UINT passes;
	UINT i;
	//render terrain
	m_pObjectEffects->Begin( &passes , 0 );
	for(i = 0 ; i < passes ; ++i )
	{
		m_pObjectEffects->BeginPass(i);
		m_pTerrainMesh->Render(m_pd3dDevice);
		m_pObjectEffects->EndPass();
	}
	m_pObjectEffects->End();


	// Render the Skybox
		
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_DESTALPHA  );
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA  );

	D3DXMATRIXA16 save_matView =  *(m_camera.GetViewMatrix());
	D3DXMATRIXA16 matView = save_matView;
	SetD3DXMatrixTranslationTo(0.0f, -3.0f, 0.0f, &matView);

	m_pd3dDevice->SetTransform( D3DTS_VIEW,      &matView );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	m_pSkyBoxMesh->Render( m_pd3dDevice );

	// Restore the render states
	m_pd3dDevice->SetTransform( D3DTS_VIEW, m_camera.GetViewMatrix() );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	

}

void RainbowFogbowSDKDemo::SetViewPortHelper(DWORD x, DWORD y, DWORD w, DWORD h)
{
	D3DVIEWPORT9 vp;
	vp.X      = x;
	vp.Y      = y;
	vp.Width  = w;
	vp.Height = h;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f; 
	m_pd3dDevice->SetViewport(&vp);
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::Render()
{
	static lastDataFile = m_currentRainbowDataTexture;
	if(m_currentRainbowDataTexture!= lastDataFile)
	{
		m_pNVRainbowEffect->InvalidateDeviceObjects();
		m_pNVRainbowEffect->RestoreDeviceObjects(m_pd3dDevice, m_RainbowDataTextures[m_currentRainbowDataTexture].c_str());
		lastDataFile = m_currentRainbowDataTexture;
	}

	/**
		The Rainbow is rendered in a few distinct steps
		1. Generate a moisture texture
		2. Render the scene as you would normally
		3. Render the rainbow effect using the moisture texture
	*/

	m_pObjectEffects->SetFloat("fogNear", 100.0f);
	m_pObjectEffects->SetFloat("fogFar", 400.0f);
	m_pObjectEffects->SetVector( "lightDir", &m_sunLightDirection );
	m_pNVRainbowEffect->SetLightDirection(&m_sunLightDirection);


    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		if(!m_VisualizeRenderSteps)//render normally
		{
			
			m_pNVRainbowEffect->BeginMoistureTextureRendering(m_pd3dDevice);
			RenderMoisturePass();
			m_pNVRainbowEffect->EndMoistureTextureRendering(m_pd3dDevice);

			RenderSceneNormally();
			
			m_pNVRainbowEffect->RenderRainbow(m_pd3dDevice);
	        
		}
		else //render steps to different viewports.
		{
			//clear the main viewport
			SetViewPortHelper(0,0,m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height);
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
								0x00000000, 1.0f, 0L );


			//lets make a white moisture texture so we can just get a plain rainbow
			m_pNVRainbowEffect->BeginMoistureTextureRendering(m_pd3dDevice);
			m_pSkyBoxMesh->Render( m_pd3dDevice );

			float moistureDensity = 0.5f;
			D3DXCOLOR fogMoisture( moistureDensity , m_rainbowDropletRadius , 0.1f , 1.0f );
			m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
								fogMoisture, 1.0f, 0L);
			m_pNVRainbowEffect->EndMoistureTextureRendering(m_pd3dDevice);

			//now render just the plain rainbow
			SetViewPortHelper(0,0, m_d3dsdBackBuffer.Width/3, m_d3dsdBackBuffer.Height/2);
			m_pNVRainbowEffect->RenderRainbow(m_pd3dDevice);


			//update the moisture texture to be the real moisture texture
			m_pNVRainbowEffect->BeginMoistureTextureRendering(m_pd3dDevice);
			RenderMoisturePass();
			m_pNVRainbowEffect->EndMoistureTextureRendering(m_pd3dDevice);

			//render the moisture texture (just copy it to the backbuffer is fine.)
			LPDIRECT3DSURFACE9 ppSurface;
			LPDIRECT3DSURFACE9 pBackBuffer;
			RECT viewPortRect;
			viewPortRect.left = m_d3dsdBackBuffer.Width/3;
			viewPortRect.top = 0;
			viewPortRect.right = viewPortRect.left+ m_d3dsdBackBuffer.Width/3;
			viewPortRect.bottom =m_d3dsdBackBuffer.Height/2;
			m_pNVRainbowEffect->GetMoistureTexture()->GetSurfaceLevel(0,&ppSurface);
			m_pd3dDevice->GetRenderTarget(0, &pBackBuffer);
			m_pd3dDevice->StretchRect(ppSurface, NULL, pBackBuffer, &viewPortRect, D3DTEXF_NONE);
			ppSurface->Release();
			pBackBuffer->Release();

			//render Moisture X Rainbow
			SetViewPortHelper(2*m_d3dsdBackBuffer.Width/3,0, m_d3dsdBackBuffer.Width/3, m_d3dsdBackBuffer.Height/2);
			m_pNVRainbowEffect->RenderRainbow(m_pd3dDevice);


			//render the normal scene
			SetViewPortHelper(0,m_d3dsdBackBuffer.Height/2,m_d3dsdBackBuffer.Width/3, m_d3dsdBackBuffer.Height/2);
			RenderSceneNormally();


			//render the normal scene with the rainbow
			SetViewPortHelper(m_d3dsdBackBuffer.Width/3,m_d3dsdBackBuffer.Height/2, m_d3dsdBackBuffer.Width/3, m_d3dsdBackBuffer.Height/2);
			RenderSceneNormally();
			m_pNVRainbowEffect->RenderRainbow(m_pd3dDevice);
	      
		
		}
		SetViewPortHelper(0,0, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height);
		RenderText();
		m_pd3dDevice->EndScene();
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::RenderText()
{


	D3DCOLOR fontColor    = D3DCOLOR_ARGB(255, 255, 255, 0);
	TCHAR szMsg[MAX_PATH] = TEXT("");

	RECT rct;
	ZeroMemory(&rct, sizeof(rct));       

	if(!m_VisualizeRenderSteps)//render normally
	{
		rct.left   = 2;
		rct.right  = m_d3dsdBackBuffer.Width - 20;

		// Output display stats
		INT nNextLine = 40; 

		lstrcpy(szMsg, m_strDeviceStats);
		nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

		lstrcpy(szMsg, m_strFrameStats);
		nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

		// Output help
		nNextLine = m_d3dsdBackBuffer.Height; 

		lstrcpy(szMsg, TEXT("Use WASD keys and mouse to move"));
		nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

		// Output statistics & help
		float lookupTextureMinRadius = 5.0f;//microns
		float lookupTextureMaxRadius = 800.0f;//microns
		sprintf( szMsg, TEXT("Water Droplet Radius: approx %0.3f microns"), m_rainbowDropletRadius *(lookupTextureMaxRadius - lookupTextureMinRadius)  + lookupTextureMinRadius);
		nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
		if(m_currentRainbowDataTexture == 0) //only this data file supports multiple radii
		{
			m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);
		}

		sprintf( szMsg, TEXT("Rainbow Intensity: %0.3f"), m_rainbowIntensity);
		nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

		sprintf( szMsg, TEXT("Rainbow Data: %s"), m_RainbowDataTextures[this->m_currentRainbowDataTexture].c_str());
		nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);



	}
	else //render in visualization mode
	{

		rct.left   = 0;
		rct.top = 0;
		rct.right  = rct.left + m_d3dsdBackBuffer.Width/3;
		rct.bottom = rct.top + 20;
		lstrcpy(szMsg, TEXT("rainbow: R"));
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

		rct.left   = m_d3dsdBackBuffer.Width/3;
		rct.top = 0;
		rct.right  = rct.left + m_d3dsdBackBuffer.Width/3;
		rct.bottom = rct.top + 20;
		lstrcpy(szMsg, TEXT("moisture: M"));
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

		rct.left   = 2*m_d3dsdBackBuffer.Width/3;
		rct.top = 0;
		rct.right  = rct.left + m_d3dsdBackBuffer.Width/3;
		rct.bottom = rct.top + 20;
		lstrcpy(szMsg, TEXT("M * R"));
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);


		rct.left   = 0;
		rct.top = m_d3dsdBackBuffer.Height/2;
		rct.right  = rct.left + m_d3dsdBackBuffer.Width/3;
		rct.bottom = rct.top + 20;
		lstrcpy(szMsg, TEXT("Scene: S"));
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);


		rct.left   = m_d3dsdBackBuffer.Width/3;
		rct.top = m_d3dsdBackBuffer.Height/2;
		rct.right  = rct.left + m_d3dsdBackBuffer.Width/3;
		rct.bottom = rct.top + 20;
		lstrcpy(szMsg, TEXT("S + (M * R)"));
		m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

	}
	return S_OK;

}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT RainbowFogbowSDKDemo::MsgProc(HWND hWnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam)
{
    switch(msg)
    {
        case WM_PAINT:
        {
            if (m_bLoadingApp)
            {
                // Draw on the window tell the user that the app is loading
                HDC hDC = GetDC(hWnd);
                TCHAR strMsg[MAX_PATH];
                wsprintf(strMsg, TEXT("Loading... Please wait"));

                RECT rct;
                GetClientRect(hWnd, &rct);

                DrawText(hDC, strMsg, -1, &rct, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                ReleaseDC(hWnd, hDC);
            }
            break;
        }
				case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case  ID_VISUALIZERENDERINGSTEPS_SHOWNORMAL:
				m_VisualizeRenderSteps = false;
				break;
			case  ID_VISUALIZERENDERINGSTEPS_SHOWALL:
				m_VisualizeRenderSteps = true;
				break;
			case ID_RAINBOW_INCREASEDROPLETRADIUS:
				{
					if(m_currentRainbowDataTexture == 0)
					{
						m_rainbowDropletRadius += 0.01f;
						if(m_rainbowDropletRadius > 1.0f)
							m_rainbowDropletRadius = 1.0f;
					}
				}
				break;
			case ID_RAINBOW_DECREASEDROPLETRADIUS:
				{
					if(m_currentRainbowDataTexture == 0)
					{
						m_rainbowDropletRadius -= 0.01f;
						if(m_rainbowDropletRadius < 0.0f)
							m_rainbowDropletRadius = 0.0f;
					}
				}
				break;
			case ID_RAINBOW_INCREASEINTENSITY:
				{
					m_rainbowIntensity += 0.01f;
					if(m_rainbowIntensity > 10.0f)
						m_rainbowIntensity = 10.0f;
				}
				break;
			case ID_RAINBOW_DECREASEINTENSITY:
				{
					m_rainbowIntensity -= 0.01f;
					if(m_rainbowIntensity < 0.0f)
						m_rainbowIntensity = 0.0f;
				}
				break;
			case ID_DATAFILES_NEXT:
				{
					m_currentRainbowDataTexture ++;
					if(m_currentRainbowDataTexture > m_RainbowDataTextures.size()-1 )
					{
						m_currentRainbowDataTexture = 0;
					}
				}
				break;
			case ID_DATAFILES_PREVIOUS:
				{
					m_currentRainbowDataTexture --;
					if(m_currentRainbowDataTexture < 0 )
					{
						m_currentRainbowDataTexture = m_RainbowDataTextures.size()-1;
					}
				}
				break;
			}
			break;
		}

    }

	m_camera.HandleMessages(hWnd,msg,wParam,lParam);

	return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::InvalidateDeviceObjects()
{
	m_pSkyBoxMesh->InvalidateDeviceObjects();
	m_pSkyBoxMoistureMesh->InvalidateDeviceObjects();
	m_pTerrainMesh->InvalidateDeviceObjects();
	m_pNVRainbowEffect->InvalidateDeviceObjects();
	if( m_pObjectEffects)
		m_pObjectEffects->OnLostDevice();
	if( m_pSkyBoxRainEffect )
		m_pSkyBoxRainEffect->OnLostDevice();


	if (m_pFont)
        m_pFont->OnLostDevice();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::DeleteDeviceObjects()
{
	m_pSkyBoxMesh->Destroy();
	m_pSkyBoxMoistureMesh->Destroy();
	m_pTerrainMesh->Destroy();
	m_pNVRainbowEffect->Destroy();
	SAFE_RELEASE(m_pObjectEffects);
	SAFE_RELEASE(m_pSkyBoxRainEffect);

	SAFE_RELEASE(m_pFont);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT RainbowFogbowSDKDemo::FinalCleanup()
{
	return S_OK;
}




