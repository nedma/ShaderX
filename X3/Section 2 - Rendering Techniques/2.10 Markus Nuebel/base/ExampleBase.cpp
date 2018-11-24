//-----------------------------------------------------------------------------
// File: ExampleBase.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include <time.h>
#include "ExampleBase.h"

float g_fContrastExponent = 2.5f;

//-----------------------------------------------------------------------------
// Name: CBaseD3DApplication()
// Desc: Application constructor.   Paired with ~CBaseD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CBaseD3DApplication::CBaseD3DApplication()
{
    m_dwCreationWidth           = 640;
    m_dwCreationHeight          = 480;
    m_strWindowTitle            = TEXT("ExampleBase");
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
    m_bStartFullscreen			   =  false;
    m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 10, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_bRotateModel              = FALSE;

    ZeroMemory(&m_UserInput, sizeof(m_UserInput) );
	 ZeroMemory(&m_szKeys, sizeof(m_szKeys));
    m_vVelocity			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vAngularVelocity	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vecScale			= D3DXVECTOR3(3.0f, 4.0f, 3.0f);


	m_fAutoTime					= 0.0f;
	m_fAutoCamSpeed				= 0.1f;
	m_bAutoCam					= TRUE;
	m_bScrPos					= FALSE;
	m_bCheckPSsupport			= TRUE;

	// Init shaders
	m_pVertexShader = NULL;
	m_pPixelShader	= NULL;

	// Init mesh settings
	m_fRadius			= 0.0f;
	m_dwNumSections		= 0;
	m_vecCenter			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pMeshMaterials	= NULL;
	m_pAttributes		= NULL;
	m_pIB				= NULL;
	m_pVB				= NULL;
	m_pModelVertexDeclaration	= NULL;

	// Init textures
	m_texRandom			= NULL;
	m_texContrast		= NULL;
	m_texPaper			= NULL;

	srand( (unsigned)time(NULL));
}




//-----------------------------------------------------------------------------
// Name: ~CBaseD3DApplication()
// Desc: Application destructor.  Paired with CBaseD3DApplication()
//-----------------------------------------------------------------------------
CBaseD3DApplication::~CBaseD3DApplication()
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
HRESULT CBaseD3DApplication::OneTimeSceneInit()
{
    // TODO: perform one time initialization

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;


    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
	{
		if(!( pCaps->VertexShaderVersion >= D3DVS_VERSION(1,1) ))
			return E_FAIL;
	}

	if(m_bCheckPSsupport)
	{
		if(!( pCaps->PixelShaderVersion >= D3DPS_VERSION(1,4) ))
			return E_FAIL;
	}

   if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		return E_FAIL;
	}

    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
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
HRESULT CBaseD3DApplication::InitDeviceObjects()
{
	float fWidth = (float)m_rcWindowClient.right - m_rcWindowClient.left;
	float fHeight = (float)m_rcWindowClient.bottom - m_rcWindowClient.top;

   BGVertex_t vertSource = {0.0f, 0.0f, 1.0f, 1.0f,									0.0f, 0.0f};
	memcpy(&m_arBackground[0], 	&vertSource, sizeof(vertSource));
   BGVertex_t vertSource2 = {0.0f, fHeight, 1.0f, 1.0f,					0.0f, 1.0f};
	memcpy(&m_arBackground[1], 	&vertSource2, sizeof(vertSource2));
   BGVertex_t vertSource3 = {fWidth , 0.0f, 1.0f, 1.0f,					1.0f, 0.0f};
	memcpy(&m_arBackground[2], 	&vertSource3, sizeof(vertSource3));
   BGVertex_t vertSource4 = {fWidth , fHeight, 1.0f, 1.0f,		1.0f, 1.0f};
	memcpy(&m_arBackground[3], 	&vertSource4, sizeof(vertSource4));

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	// Vertex declaration
	D3DVERTEXELEMENT9 decl[] = 
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,    0 },
		D3DDECL_END()
	};

	m_pd3dDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration );

	// Load the model
//	if(FAILED(loadXFile("..\\media\\bigship1.x", D3DFVF_MODELVERTEX)))
	if(FAILED(loadXFile("..\\media\\chameleon.x", D3DFVF_MODELVERTEX)))
		return E_FAIL;

	// Create random texture
	if(FAILED(createRandomTexture()))
		return E_FAIL;
//D3DXSaveTextureToFile("c:/temp/random.bmp", D3DXIFF_BMP, m_texRandom, NULL);

	// Create contratst texture
	if(FAILED(createContrastMap(0.5f, g_fContrastExponent)))
		return E_FAIL;
D3DXSaveTextureToFile("c:/temp/contrast.bmp", D3DXIFF_BMP, m_texContrast, NULL);

	// Load papertexture
	if(FAILED(loadPaperTexture("..\\media\\paper.dds")))
		return E_FAIL;
//D3DXSaveTextureToFile("c:/temp/paper.bmp", D3DXIFF_BMP, m_texPaper, NULL);

	// Call methods in derived class
	if(FAILED(CreateShaders()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBaseD3DApplication::CreateShaders()
{
	return S_OK;
}


HRESULT CBaseD3DApplication::InitializeLights()
{
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
HRESULT CBaseD3DApplication::RestoreDeviceObjects()
{
	float fWidth = (float)m_rcWindowClient.right - m_rcWindowClient.left;
	float fHeight = (float)m_rcWindowClient.bottom - m_rcWindowClient.top;

   BGVertex_t vertSource = {0.0f, 0.0f, 1.0f, 1.0f,									0.0f, 0.0f};
	memcpy(&m_arBackground[0], 	&vertSource, sizeof(vertSource));
   BGVertex_t vertSource2 = {0.0f, fHeight, 1.0f, 1.0f,					0.0f, 1.0f};
	memcpy(&m_arBackground[1], 	&vertSource2, sizeof(vertSource2));
   BGVertex_t vertSource3 = {fWidth , 0.0f, 1.0f, 1.0f,					1.0f, 0.0f};
	memcpy(&m_arBackground[2], 	&vertSource3, sizeof(vertSource3));
   BGVertex_t vertSource4 = {fWidth , fHeight, 1.0f, 1.0f,		1.0f, 1.0f};
	memcpy(&m_arBackground[3], 	&vertSource4, sizeof(vertSource4));

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE,   FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,        TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,		FALSE); 
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING,		FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT,        0x000F0F0F);

    // Set up the textures
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP ,D3DTOP_MODULATE2X );

    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );



    // Set the world matrix
    D3DXMATRIX     matR;
	 D3DXQUATERNION qR;
    D3DXMatrixIdentity(&m_matWorld);
    D3DXMATRIX matIdentity;
    D3DXQuaternionRotationYawPitchRoll(&qR, 2.0f, 0.0f, 0.0f);
    D3DXMatrixRotationQuaternion(&matR, &qR);
    D3DXMatrixMultiply(&m_matWorld, &matR, &m_matWorld);

    m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);


    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. 
    D3DXVECTOR3 vFromPt   =  D3DXVECTOR3( m_fRadius*1.2f , m_fRadius*0.5f, -m_fRadius*1.8f );
    D3DXVECTOR3 vLookatPt = m_vecCenter;
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&m_matView, &vFromPt, &vLookatPt, &vUpVec);
    m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixInverse(&m_matPosition, NULL, &m_matView);

    // Set the projection matrix
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

	// Setup lights
	InitializeLights();

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}

// Called in AutoCamMode, to calculate the next view matrix
// for the next camera postion
HRESULT CBaseD3DApplication::CalcAutoCam()
{
	float		fAutoCamPos;
	float		fRadius;
	D3DXVECTOR3 vCamera;
	D3DXVECTOR3	vCenter;

	fRadius	= 600.0f;
	vCenter	= D3DXVECTOR3(512, 100, 512);
	vCenter.x	*= m_vecScale.x;
	vCenter.y	*= m_vecScale.y;
	vCenter.z	*= m_vecScale.z;


	if(m_fAutoTime > C_AUTO_CAM_TIME)
		m_fAutoTime = 0.0f;
	else
		m_fAutoTime += m_fElapsedTime*m_fAutoCamSpeed;

	float	fsin	= (float)sin(m_fAutoTime);
	float	fcos	= (float)cos(m_fAutoTime);
	// Current time point in autocam loop
	fAutoCamPos = (m_fAutoTime/C_AUTO_CAM_TIME)*2*D3DX_PI; // Results in a range between 0 and PI

	// Depending on the current rotation direction the next point is calculated
	vCamera = D3DXVECTOR3(	vCenter.x + fRadius*fcos , 
						/*vCenter.y + sin(fAutoCamPos)*fRadius*0.5f+*/600.0f, 
						vCenter.z + fRadius*fsin );
 
	// Setup of the view matrix
	D3DXVECTOR3 vAt		= D3DXVECTOR3(vCenter.x + fRadius/4.0f*fcos, vCenter.y + fRadius/8.0f*fsin, vCenter.z + fRadius/4.0f*fcos);
    D3DXVECTOR3 vRight  = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&m_matView, &vCamera, &vAt, &vUp );
	D3DXMatrixTranslation(&m_matPosition, vCamera.x, vCamera.y, vCamera.z);

	D3DXMATRIX     matR;
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll( &qR, -1.5f, .5f, 0.0f);
	D3DXMatrixRotationQuaternion( &matR, &qR );
	D3DXMatrixMultiply( &m_matPosition, &matR, &m_matPosition );

	return S_OK;
}

// Called in AutoCamMode, to calculate the next view matrix
// for the next camera postion
HRESULT CBaseD3DApplication::SetScrShotPos()
{
	float		fRadius;
	D3DXVECTOR3 vCamera;
	D3DXVECTOR3	vCenter;

	fRadius	= 600.0f;
	vCenter	= D3DXVECTOR3(512, 100, 512);
	vCenter.x	*= m_vecScale.x;
	vCenter.y	*= m_vecScale.y;
	vCenter.z	*= m_vecScale.z;

	float	fsin	= (float)sin(m_fAutoTime);
	float	fcos	= (float)cos(m_fAutoTime);

	// Depending on the current rotation direction the next point is calculated
	vCamera = D3DXVECTOR3(	vCenter.x + fRadius*fcos , 
						/*vCenter.y + sin(fAutoCamPos)*fRadius*0.5f+*/600.0f, 
						vCenter.z + fRadius*fsin );
 
	// Setup of the view matrix
	D3DXVECTOR3 vAt		= D3DXVECTOR3(vCenter.x + fRadius/4.0f*fcos, vCenter.y + fRadius/8.0f*fsin, vCenter.z + fRadius/4.0f*fcos);
    D3DXVECTOR3 vRight  = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&m_matView, &vCamera, &vAt, &vUp );
	D3DXMatrixTranslation(&m_matPosition, vCamera.x, vCamera.y, vCamera.z);

	D3DXMATRIX     matR;
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll( &qR, -1.5f, .5f, 0.0f);
	D3DXMatrixRotationQuaternion( &matR, &qR );
	D3DXMatrixMultiply( &m_matPosition, &matR, &m_matPosition );

	return S_OK;
}

// Processes keyboard input and updates the view matrix and the position 
HRESULT	CBaseD3DApplication::CalcMovement()
{
    FLOAT fSecsPerFrame = m_fElapsedTime;

	// Init rotation and translation
	D3DXVECTOR3 vT( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vR( 0.0f, 0.0f, 0.0f );


	// Scale the input, to get more responsiveness
	vT *= 200.0f;
	vR *= 50.0f;

	// Apply some friciton (dumping) to the current velocity
	// and add the new impulse
	m_vVelocity        = m_vVelocity * 0.9f			+ vT * 0.01f;
	m_vAngularVelocity = m_vAngularVelocity * 0.9f	+ vR * 0.01f;

	// Frame movement is calculated base on the frame duration
	// This is not totally accurate, since the calculation assume 
	// the velocity of the whole frame to be deterimned by the speed
	// at the end of the frame.
	D3DXVECTOR3  vTFrame = m_vVelocity			* fSecsPerFrame * .5f;
	D3DXVECTOR3  vRFrame = m_vAngularVelocity	* fSecsPerFrame * .5f;

	// Calculate the new position, based on this frames translation
	D3DXMATRIX     matT;
	D3DXMatrixTranslation( &matT, vTFrame.x, vTFrame.y, vTFrame.z);
	D3DXMatrixMultiply( &m_matPosition, &matT, &m_matPosition );

	// Rotation, is calculated by using quaternions
	D3DXMATRIX     matR;
	D3DXQUATERNION qR;

   // The resulting view matrix is just the inverse of the camera transformation
	D3DXMatrixInverse( &m_matView, NULL, &m_matPosition );

	if(m_bRotateModel)
   {
	   // Spin the model
	   D3DXQuaternionRotationYawPitchRoll(&qR, fSecsPerFrame/5.0f, 0.0f, 0.0f);
	   D3DXMatrixRotationQuaternion(&matR, &qR);
	   D3DXMatrixMultiply(&m_matWorld, &matR, &m_matWorld);
  }

   return S_OK;
}

HRESULT CBaseD3DApplication::SetShaderConstants()
{
	return S_OK;

}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::FrameMove()
{
    // TODO: update world

    // Update user input state
    UpdateInput( &m_UserInput );

    if(m_UserInput.bToggleModel)
       m_bRotateModel = ! m_bRotateModel;

	// Check for screenshot position
	if(m_UserInput.bToggleScrPos)
		m_bScrPos	= !m_bScrPos;

	if(m_bScrPos)
		SetScrShotPos();
	else
			CalcMovement();
	
	// Update the view matrix
    m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	// Recalcualte some matrices
    m_matWorldViewProj	= m_matWorld * m_matView * m_matProj;
    m_matWorldView		= m_matWorld * m_matView;
	D3DXMatrixInverse(&m_matWorldViewIT, NULL, &m_matWorldView);

	D3DXMatrixTranspose(&m_matWorldView, &m_matWorldView);
	D3DXMatrixTranspose(&m_matWorldViewProj, &m_matWorldViewProj);
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
#define KeyPressed(x) (GetAsyncKeyState((x)) & 0x8000)
#define KeyReleased(x) \
(m_szKeys[x] && !(GetAsyncKeyState((x)) & 0x8000))? TRUE : FALSE; m_szKeys[x] = (GetAsyncKeyState((x)) & 0x8000);
void CBaseD3DApplication::UpdateInput( UserInput* pUserInput )
{
	// If the application is not active, do not query inputs
	if(!m_bActive)
	{
		memset(pUserInput, 0, sizeof(UserInput));
		return;
	}

    pUserInput->bToggleModel  = KeyReleased('A');
    pUserInput->bForward		= KeyPressed(VK_HOME);
    pUserInput->bBackward		= KeyPressed(VK_END);
    pUserInput->bLeft         = KeyPressed(VK_LEFT);
    pUserInput->bRight        = KeyPressed(VK_RIGHT);
    pUserInput->bUp           = KeyPressed(VK_UP);
    pUserInput->bDown         = KeyPressed(VK_DOWN);

}


HRESULT CBaseD3DApplication::RenderBackground()
{

	m_pd3dDevice->SetPixelShader(NULL);
	m_pd3dDevice->SetVertexShader(NULL);

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pd3dDevice->SetRenderState (D3DRS_ZENABLE, D3DZB_FALSE);
	m_pd3dDevice->SetRenderState (D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTexture(0, m_texPaper);
	m_pd3dDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEX1);
	m_pd3dDevice->DrawPrimitiveUP (D3DPT_TRIANGLESTRIP, 2, m_arBackground, sizeof(BGVertex_t));
	m_pd3dDevice->SetRenderState (D3DRS_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}



HRESULT CBaseD3DApplication::PreRender()
{
    return S_OK;
}

HRESULT CBaseD3DApplication::PostRender()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::Render()
{
	// Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(128,128,128), 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: render world

		RenderBackground();

		SetShaderConstants();

		PreRender();
		if(NULL != m_pVertexShader)
			m_pd3dDevice->SetVertexShader(m_pVertexShader);
		if(NULL != m_pPixelShader)
			m_pd3dDevice->SetPixelShader(m_pPixelShader);

		// Model rendering
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_pd3dDevice->SetVertexDeclaration(m_pModelVertexDeclaration);
        m_pd3dDevice->SetIndices(m_pIB);
        m_pd3dDevice->SetStreamSource(0, m_pVB, 0, D3DXGetFVFVertexSize(m_dwFVF));
		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// Draw the model using the material
			m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
											 0,
											 0,
											 m_nVertices, 
							  				 0, 
											 m_nFaces);
		PostRender();

        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::RenderText()
{

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CBaseD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
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
HRESULT CBaseD3DApplication::InvalidateDeviceObjects()
{
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
HRESULT CBaseD3DApplication::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
	SAFE_RELEASE(m_texRandom);
	SAFE_RELEASE(m_texContrast);
	SAFE_RELEASE(m_texPaper);
	SAFE_RELEASE(m_pVertexDeclaration);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);

	SAFE_DELETE_ARRAY(m_pMeshMaterials);
	SAFE_DELETE_ARRAY(m_pAttributes);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pModelVertexDeclaration);

    m_pFont->DeleteDeviceObjects();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: LoadXFile()
// Desc: Loads the model file and fills in the vertex buffer, 
//       index buffer and the material members
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::loadXFile(const char* szFileName, const DWORD dwFVF)
{
	ID3DXMesh *tempMesh	= NULL;
	ID3DXMesh *tempMeshFVF = NULL;
	ID3DXMesh *tempMeshOpt = NULL;
    LPD3DXBUFFER pD3DXMtrlBuffer = NULL;
    D3DXMATERIAL* d3dxMaterials;

	HRESULT hr;
	//  Load the mesh
	hr = D3DXLoadMeshFromX(const_cast<char*>(szFileName), D3DXMESH_SYSTEMMEM , m_pd3dDevice, 
									NULL, &pD3DXMtrlBuffer, NULL, &m_dwNumSections, &tempMesh);
    if (FAILED(hr))
        return hr;

	// Retrieve the verex data
    ModelVertex* pBuff = NULL;
    hr = tempMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pBuff);
    if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		return hr;
	}

	// Compute some bounds information
	hr = D3DXComputeBoundingSphere((LPD3DXVECTOR3)pBuff, tempMesh->GetNumVertices(), D3DXGetFVFVertexSize(tempMesh->GetFVF()), 
								&m_vecCenter, &m_fRadius);
	tempMesh->UnlockVertexBuffer();
	if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		return hr;
	}

	// Prepare the mesh
	hr = tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);
	if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		return hr;
	}

	// Convert to our format
	hr = tempMeshOpt->CloneMeshFVF(tempMeshOpt->GetOptions()|D3DXMESH_WRITEONLY, dwFVF, m_pd3dDevice, &tempMeshFVF);
	if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		SAFE_RELEASE(tempMeshOpt);
		return hr;
	}


    // Get the array of materials out of the returned buffer
    d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    m_pMeshMaterials = new D3DMATERIAL9[m_dwNumSections];
    for(DWORD i=0; i<m_dwNumSections; i++ )
    {
        m_pMeshMaterials[i]				= d3dxMaterials[i].MatD3D;
        m_pMeshMaterials[i].Ambient		= m_pMeshMaterials[i].Diffuse;
    }

	// Get some mesh infos
	m_nVertices = tempMeshFVF->GetNumVertices();
	m_nFaces	= tempMeshFVF->GetNumFaces();
	m_dwFVF		= tempMeshFVF->GetFVF();

	// Retrieve the vertex and index buffers
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	tempMeshFVF->GetVertexBuffer(&m_pVB);
	tempMeshFVF->GetIndexBuffer(&m_pIB);

	// Retrieve the mesh attributes
	hr = tempMeshFVF->GetAttributeTable(NULL, &m_dwNumSections);
	if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		SAFE_RELEASE(tempMeshFVF);
		SAFE_RELEASE(tempMeshOpt);
		return hr;
	}

	m_pAttributes = new D3DXATTRIBUTERANGE[m_dwNumSections];
	hr = tempMeshFVF->GetAttributeTable(m_pAttributes, &m_dwNumSections);
	if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		SAFE_RELEASE(tempMeshFVF);
		SAFE_RELEASE(tempMeshOpt);
		return hr;
	}

	// Retrieve the vertex declaration
	D3DVERTEXELEMENT9 decl[7];
	hr = tempMeshFVF->GetDeclaration(decl);
	if(FAILED(hr))
	{
		SAFE_RELEASE(tempMesh);
		SAFE_RELEASE(tempMeshFVF);
		SAFE_RELEASE(tempMeshOpt);
		return hr;
	}
	m_pd3dDevice->CreateVertexDeclaration(decl, &m_pModelVertexDeclaration );

	// Free some resources
	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshFVF);
	SAFE_RELEASE(tempMeshOpt);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed

    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Creates a new DX9 texture.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::createTexture(int nWidth, int nHeight, LPDIRECT3DTEXTURE9* pTexture)
 {
	HRESULT hr;

	hr = D3DXCreateTexture(m_pd3dDevice, nWidth, nHeight, D3DUSAGE_AUTOGENMIPMAP, 0, D3DFMT_X8R8G8B8,
						D3DPOOL_MANAGED, pTexture);


	if(FAILED(hr))
		return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: createRandomTexture()
// Desc: Creates a random texture
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::createRandomTexture()
{
	int nWidth	= 512;
	int nHeight	= 512;

	SAFE_RELEASE(m_texRandom);
	if(FAILED(createTexture(nWidth, nHeight, &m_texRandom)))
		return E_FAIL;


	D3DLOCKED_RECT	rect;
	BYTE*	pSurf;
	int		nPitch;

	// Lock the Surface
	m_texRandom->LockRect(0, &rect, NULL, D3DLOCK_NOSYSLOCK );
	pSurf	= (BYTE*)rect.pBits;
	nPitch	= rect.Pitch/4; // We are using 32-bit color information
	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			*pSurf++ = (BYTE)(rand() % 256);
			*pSurf++ = (BYTE)(rand() % 256);
			*pSurf++ = (BYTE)(rand() % 256);
			*pSurf++ = 0;
		}
		pSurf+=nPitch-nWidth;
	}
	m_texRandom->UnlockRect(0);


	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: createContrastMap()
// Desc: Creates a random texture
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::createContrastMap(float fDensity, float fExp)
{
	int nWidth	= 256 * 2;
	int nHeight	= 256 * 2;

	PDIRECT3DSURFACE9 surfContrast = NULL;


	HRESULT hr = m_pd3dDevice->CreateOffscreenPlainSurface(nWidth, nHeight, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT , &surfContrast, NULL);
	if(FAILED(hr))
		return E_FAIL;

	D3DLOCKED_RECT	rect;
	BYTE*	pSurf;
	int		nPitch;


	// Create a white texture
	surfContrast->LockRect(&rect, NULL, 0);
	pSurf	= (BYTE*)rect.pBits;
	nPitch	= rect.Pitch/4; // We are using 32-bit color information
	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			*pSurf++ = 255;
			*pSurf++ = 255;
			*pSurf++ = 255;
			*pSurf++ = 255;
		}
		pSurf+=nPitch-nWidth;
	}

	// Calculate the number of noise sprinkels
	int nSprinkelCount	= (int)((float)(nWidth/2) * (float)(nHeight/2) * fDensity);

	for(i=0 ; i<nSprinkelCount; i++)
	{
		// Sprinkel black dots onto the texure, while applying the contrast enhancement
		// operator to the y-coordinate.

		// CEO works in the range of 0 ... 1

		// Create a random x value
		rand();
		double x = (double)rand()/(double)RAND_MAX;
		rand();
		double y = (double)rand()/(double)RAND_MAX;

		// Apply CEO
		y = pow(y, fExp);

		// Compute texture position
		int nXCoord = (int)(x * (double)(nWidth-1));
		int nYCoord = (int)(y * (double)(nHeight-1));

		// Set the x,y texel to be black
		pSurf = (BYTE*)rect.pBits + (rect.Pitch * nYCoord) + (nXCoord * 4);
		// ARGB texture
		*pSurf++ = 0;
		*pSurf++ = 0;
		*pSurf++ = 0;
		*pSurf++ = 255;
	}

	surfContrast->UnlockRect();

	// Generate mipmaps
	SAFE_RELEASE(m_texContrast);
	if(FAILED(createTexture(nWidth/2, nHeight/2, &m_texContrast)))
		return E_FAIL;
	
	// 
	int nLevels = m_texContrast->GetLevelCount();
	for(i=0 ; i<nLevels; i++)
	{
		PDIRECT3DSURFACE9 pMipSurface;
		hr  = m_texContrast->GetSurfaceLevel(i, &pMipSurface);
		if(FAILED(hr))
			return E_FAIL;

		D3DXLoadSurfaceFromSurface(pMipSurface, NULL, NULL, surfContrast, NULL, NULL, D3DX_DEFAULT, 0);
		pMipSurface->Release();
	}
	surfContrast->Release ();


	hr = D3DXFilterTexture(m_texContrast, NULL, 0, D3DX_FILTER_BOX);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: loadPaperTexture()
// Desc: Loads the paper texture
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::loadPaperTexture(char* szFileName)
{
	// Cleanup old texture
	SAFE_RELEASE(m_texPaper);

	// Load the paper texture
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, szFileName, &m_texPaper)))
		return E_FAIL;

	return S_OK;
}
	
//-----------------------------------------------------------------------------
// Name: loadContrastTexture()
// Desc: Loads the contrast texture
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::loadContrastTexture(char* szFileName)
{
	// Cleanup old texture
	SAFE_RELEASE(m_texContrast);

	// Load the paper texture
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, szFileName, &m_texContrast)))
		return E_FAIL;

	return S_OK;
}
