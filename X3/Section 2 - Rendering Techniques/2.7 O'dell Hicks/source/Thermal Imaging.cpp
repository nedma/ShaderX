//-----------------------------------------------------------------------------
// File: Thermal Imaging.cpp
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
#include "Thermal Imaging.h"


//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//vertex shader.  

//C0 is the transposed world*view*projection matrix
//C4 is the transposed world matrix
//C8 is the eye/camera's 'look at' vector
const static char thermalVertexShader[] =
{ 
"vs.1.1 \n \
 dcl_position v0 \n \
 dcl_normal0   v1 \n \
 dcl_texcoord0 v2 \n \
 \
 m4x4 oPos, v0, c0 ; transform pos to homogenous clip space \n \
 \
 ; Transform normal to world space \n \
 m4x4 r0, v1, c4 \n \
 ; Normalize the normal \n \
 dp3 r0.w, r0, r0 \n \
 rsq r0.w, r0.w \n \
 mul r0, r0, r0.w \n \
 \
 ; Intensity of infrared emittance relative to the eye \n \
 dp3 r1, c8, r0 \n \
 \
 ;Texture coords  \n \
 \
 mov oT0.xy, v2 ; thermal map \n \
 mov oT1, r1 ; color lookup" 
};


//pixel shader
//multiply the generated texture coordinate with the thermal map sample
//then sample that color lookup with that value

const static char thermalPixelShader [] = {
"ps.1.4         \n \
 texld r0, t0 ; thermal map \n \
 texcrd r1.rgb, t1 ; generated texture coordinate \n \
 mul r1.xyz, r0, r1 \n \
 \
 phase ; now sample the color lookup with the coordinate in r1 \n \
 texld r1, r1 \n \
 mov r0, r1"
};

//declaration for the vertex type
const static D3DVERTEXELEMENT9 g_VertexDeclaration[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 },
    { 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0 },
    { 0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0 },
    D3DDECL_END()
};

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

    //InitCommonControls();
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
    m_dwCreationWidth           = 800;
    m_dwCreationHeight          = 600;
    m_strWindowTitle            = TEXT( "Thermal Imaging" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;    
	m_bDrawThermal				= TRUE;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fCenterDist                = 5.0f;
    m_fWorldRotY                = 4.71f;

	m_pVertexBuffer = NULL;	
	m_pIndexBuffer = NULL;
	m_pVertexDeclaration = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pBaseTexture = NULL;
	m_pHeatTexture = NULL;	

	m_CurrentTime = m_PreviousTime = timeGetTime();
	m_DeltaTime = 0;
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

bool warnedShader = false; //to just throw up the messagebox once
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable = TRUE;

    //PS 1.4 or better is required
	if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,4) )
	{
		bCapsAcceptable = FALSE;    
		
		if(warnedShader == false)
		{
			MessageBox(NULL, "To run this sample, you need a video card capable of PS 1.4 or higher",
				"Inadequate video card.", MB_OK);

			warnedShader = true;
		}
	}

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
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: create device objects
  
    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );    

	if( FAILED( m_Mesh.Create( m_pd3dDevice, _T("tiny.x") ) ) )
		return D3DAPPERR_MEDIANOTFOUND;	

	//force mesh's vertex format
	m_Mesh.SetFVF( m_pd3dDevice, MESH_FVF );	

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
    // TODO: setup render states

    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


	//clamp coordinates for thermal map... very important!
	m_pd3dDevice->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    //view matrix
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_MatView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_MatView );

    // Set the projection matrix
    
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_MatProj, D3DX_PI/4, fAspect, 1.0f, 10000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_MatProj );
    
    m_pd3dDevice->LightEnable( 0, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();
	
	//restore mesh	
	m_Mesh.RestoreDeviceObjects( m_pd3dDevice );

	//create the shaders and vertex data
	createVertexData();
	createShaders();

	//base texture, temperature map, thermal color lookup
	D3DXCreateTextureFromFile( m_pd3dDevice, "tiny_skin.bmp", &m_pBaseTexture );
	D3DXCreateTextureFromFile( m_pd3dDevice, "tiny_heat.bmp", &m_pHeatTexture );
	D3DXCreateTextureFromFile( m_pd3dDevice, "thermal.bmp", &m_pThermalLookupTexture );	

	//noise overlay
	m_pNoiseOverlay = new NoiseOverlay(m_pd3dDevice, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height);

    return S_OK;
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

    // Update the world state according to user input
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

	if( m_UserInput.bSpaceBarToggled)
	{		
		if(m_bDrawThermal)
			m_bDrawThermal = FALSE;
		else
			m_bDrawThermal = TRUE;
	}

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_DeltaTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_DeltaTime;

    if( m_UserInput.bMoveForward && !m_UserInput.bMoveBackward )
	{
        m_fCenterDist -= m_DeltaTime * 35.0f;

		if(m_fCenterDist < 3.0f)
			m_fCenterDist = 3.0f;
	}
    else if( m_UserInput.bMoveBackward && !m_UserInput.bMoveForward )
	{
        m_fCenterDist += m_DeltaTime * 35.0f;

		if(m_fCenterDist > 35.0f)
			m_fCenterDist = 35.0f;
	}
	
    D3DXVECTOR3 vFromPt  = D3DXVECTOR3( 0.0f, 2.0f, 0.0f );

	vFromPt.x -= m_fCenterDist * cosf(m_fWorldRotY);
	vFromPt.z -= m_fCenterDist * sinf(m_fWorldRotY);

    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 1.5f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_MatView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_MatView );

	//time update
	m_PreviousTime = m_CurrentTime;
	m_CurrentTime = timeGetTime();
	
	m_DeltaTime = (m_CurrentTime - m_PreviousTime) * 0.001f; //convert from milliseconds to seconds
	
	D3DXMATRIX matTranspose;

	D3DXMatrixMultiply(&matTranspose, &m_MatView, &m_MatProj);
	D3DXMatrixTranspose(&matTranspose, &matTranspose);

	m_pd3dDevice->SetVertexShaderConstantF(  0, (float*)&matTranspose, 4 );

	//world matrix
	//identity for now
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranspose(&matWorld, &matWorld);

	m_pd3dDevice->SetVertexShaderConstantF(  4, (float*)&matWorld, 4 );

	//eye position
	D3DXVECTOR3 lookDir = vLookatPt - vFromPt;
	D3DXVec3Normalize(&lookDir, &lookDir);

	//clunky...
	D3DXVECTOR4 eyeDir(-lookDir.x, -lookDir.y, -lookDir.z, 0.0f);	
	m_pd3dDevice->SetVertexShaderConstantF(  8, (float*)&eyeDir, 1 );	

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
	memcpy(pUserInput->diksOld, pUserInput->diks, sizeof(pUserInput->diks));

    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    // TODO: Process user input as needed
    pUserInput->bRotateLeft  = ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );
    pUserInput->bRotateRight = ( (pUserInput->diks[DIK_RIGHT] & 0x80) == 0x80 );
    pUserInput->bMoveForward   = ( (pUserInput->diks[DIK_UP] & 0x80)    == 0x80 );
    pUserInput->bMoveBackward  = ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );
	
	pUserInput->bSpaceBarToggled  = ( (pUserInput->diks[DIK_SPACE] & 0x80) == 0x80
		&& (pUserInput->diksOld[DIK_SPACE] & 0x80) == 0);
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // Clear the viewport
	DWORD clearColor;

	if(m_bDrawThermal)
		clearColor = 0x00000055;
	else
		clearColor = 0x00555555;

    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         clearColor, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {

		if(m_bDrawThermal == FALSE)
		{
			m_pd3dDevice->SetVertexShader( NULL );		
			m_pd3dDevice->SetPixelShader(NULL);

			
			m_Mesh.Render( m_pd3dDevice );
		}
		else
		{
			m_pd3dDevice->SetVertexDeclaration( m_pVertexDeclaration ); 
			m_pd3dDevice->SetVertexShader(m_pVertexShader );		
			m_pd3dDevice->SetPixelShader(m_pPixelShader );

			m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(VERTEX) );
			m_pd3dDevice->SetIndices(m_pIndexBuffer);		
			

			//set the transformation matrix ( world(identity) * view * projection ), transposed
			D3DXMATRIX matTransposedTransformation;

			D3DXMatrixIdentity( &matTransposedTransformation );
			
			D3DXMatrixMultiply( &matTransposedTransformation, &m_MatView, &m_MatProj );

			D3DXMatrixTranspose(&matTransposedTransformation, &matTransposedTransformation);	

					
			//m_pd3dDevice->SetTexture(0, m_pBaseTexture);				
			m_pd3dDevice->SetTexture(0, m_pHeatTexture);				
			m_pd3dDevice->SetTexture(1, m_pThermalLookupTexture);				

			//fog off
			m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
			
			m_pd3dDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST ,0,0,m_dwNumVerts,
				0, m_dwNumFaces);		 
		}

		//noise overlay
		if(m_bDrawThermal)
			m_pNoiseOverlay->Render(m_pd3dDevice, m_DeltaTime);

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

	lstrcpy( szMsg, TEXT("Press SPACE to toggle thermal/normal rendering") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Use left and right arrows to rotate, up and down to zoom") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

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
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

	//m_pSkyBox->InvalidateDeviceObjects();
	m_Mesh.InvalidateDeviceObjects();


	//release stuff for reset
	SAFE_RELEASE(m_pVertexBuffer);	
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexDeclaration);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pBaseTexture);
	SAFE_RELEASE(m_pThermalLookupTexture);
	SAFE_RELEASE(m_pHeatTexture);
	
	delete m_pNoiseOverlay;

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
	
	m_Mesh.Destroy();	

	//release everything
	SAFE_RELEASE(m_pVertexBuffer);	
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexDeclaration);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pHeatTexture);
	SAFE_RELEASE(m_pThermalLookupTexture);	
	SAFE_RELEASE(m_pBaseTexture);	

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

    // Cleanup DirectInput
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
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}

//Note that no error handling is done in this method
HRESULT CMyD3DApplication::createVertexData()
{
	LPDIRECT3DVERTEXBUFFER9 pMeshSourceVB;
    LPDIRECT3DINDEXBUFFER9  pMeshSourceIB;
	VERTEX*              pSrc;
    VERTEX*              pDst;

	// Get the number of vertices and faces for the meshes
    m_dwNumVerts = m_Mesh.GetSysMemMesh()->GetNumVertices();
    m_dwNumFaces = m_Mesh.GetSysMemMesh()->GetNumFaces();    

    // Create the dolphin and seafloor vertex and index buffers
    m_pd3dDevice->CreateVertexBuffer( m_dwNumVerts * sizeof(VERTEX),
                                      D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
                                      &m_pVertexBuffer, NULL );
    
    m_pd3dDevice->CreateIndexBuffer( m_dwNumFaces * 3 * sizeof(WORD),
                                      D3DUSAGE_WRITEONLY,
                                      D3DFMT_INDEX16, D3DPOOL_MANAGED,
                                      &m_pIndexBuffer, NULL );	

    // Copy vertices from the mesh and calculate tangent and binormal
    m_Mesh.GetSysMemMesh()->GetVertexBuffer( &pMeshSourceVB );

    m_pVertexBuffer->Lock( 0, 0, (void**)&pDst, 0 );
    pMeshSourceVB->Lock( 0, 0, (void**)&pSrc, 0 );
    //memcpy( pDst, pSrc, m_dwNumVerts * sizeof(VERTEX) );

	for(unsigned int i = 0; i < m_dwNumVerts; i++)
	{
		pDst[i].position = pSrc[i].position;
		pDst[i].normal = pSrc[i].normal;
		pDst[i].texCoord = pSrc[i].texCoord;
	}

    m_pVertexBuffer->Unlock();
    pMeshSourceVB->Unlock();
    pMeshSourceVB->Release();    

    // Copy indices for the mesh
    m_Mesh.GetSysMemMesh()->GetIndexBuffer( &pMeshSourceIB );
    m_pIndexBuffer->Lock( 0, 0, (void**)&pDst, 0 );
    pMeshSourceIB->Lock( 0, 0, (void**)&pSrc, 0 );
    memcpy( pDst, pSrc, 3 * m_dwNumFaces * sizeof(WORD) );
    m_pIndexBuffer->Unlock();
    pMeshSourceIB->Unlock();
    pMeshSourceIB->Release();    

	return S_OK;
}

void CMyD3DApplication::createShaders()
{
	HRESULT      hr;

	//vertex declaration
	hr = m_pd3dDevice->CreateVertexDeclaration( g_VertexDeclaration, &m_pVertexDeclaration );

	//create vertex shader	
    LPD3DXBUFFER errorBuf = NULL;
	LPD3DXBUFFER shaderBuf = NULL;    	

    // Build a DWORD array of opcodes from the text string
    hr = D3DXAssembleShader( thermalVertexShader, lstrlen(thermalVertexShader), 0, NULL, 0, //last 0 is flags, new for dx9
        &shaderBuf, &errorBuf );     
    
	if(SUCCEEDED(hr))
    {
        if(errorBuf)
			errorBuf->Release();

        // Create new pixel shader
        hr = m_pd3dDevice->CreateVertexShader(
			(DWORD*)shaderBuf->GetBufferPointer(), &m_pVertexShader);

        if( FAILED(hr) )
        {            
			if(shaderBuf)
				shaderBuf->Release();            
        }
        else
        {
			if(shaderBuf)
				shaderBuf->Release();						
        }
    }
	else
	{
		char* errorMsg = (char*)errorBuf->GetBufferPointer();
	}


	//create pixel shader	

    // Build a DWORD array of opcodes from the text string
    hr = D3DXAssembleShader( thermalPixelShader, lstrlen(thermalPixelShader), 0, NULL, 0, //last 0 is flags, new for dx9
        &shaderBuf, &errorBuf );    
    
	if(SUCCEEDED(hr))
    {
        if(errorBuf)
			errorBuf->Release();

        // Create new pixel shader
        hr = m_pd3dDevice->CreatePixelShader((DWORD*)shaderBuf->GetBufferPointer(),
            &m_pPixelShader );

        if( FAILED(hr) )
        {            
			if(shaderBuf)
				shaderBuf->Release();         
        }
        else
        {
			if(shaderBuf)
				shaderBuf->Release();            
        }
    }
	else
	{
		char* errorMsg = (char*)errorBuf->GetBufferPointer();
	}
}


