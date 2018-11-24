//-----------------------------------------------------------------------------
// File: EZEffect.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
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
#include "SXEffect.h"
#include "EZEffect.h"

#include <crtdbg.h>
#define new new(_CLIENT_BLOCK,__FILE__,__LINE__)	// Detect memory leaks by default

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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

HRESULT CMyD3DApplication::CreateGeometry(PCTSTR pszVBFile,PCTSTR pszIBFile,CMyD3DApplication::MESH& mshObject,bool bTextured)
{
	// Load VB
	HANDLE hFile = CreateFile(pszVBFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwFileSize = GetFileSize(hFile,NULL);

	mshObject.dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|(bTextured?D3DFVF_TEX1:0);
	mshObject.uStride = D3DXGetFVFVertexSize(mshObject.dwFVF);
	mshObject.dwVertsCount = dwFileSize/mshObject.uStride;

	HRESULT hRetval = m_pd3dDevice->CreateVertexBuffer(dwFileSize,D3DUSAGE_WRITEONLY,mshObject.dwFVF,D3DPOOL_DEFAULT,&mshObject.pVB,NULL);
	if (FAILED(hRetval))
	{
		CloseHandle(hFile);
		return DXTRACE_ERR("CreateVertexBuffer",hRetval);
	}
	void *pData = NULL;
	mshObject.pVB->Lock(0,0,&pData,0);
	DWORD dwBytesRead;
	ReadFile(hFile,pData,dwFileSize,&dwBytesRead,NULL);
	mshObject.pVB->Unlock();
	CloseHandle(hFile);


	// Load IB
	hFile = CreateFile(pszIBFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	dwFileSize = GetFileSize(hFile,NULL);
	mshObject.dwTrisCount = dwFileSize/(sizeof(WORD)*3);

	hRetval = m_pd3dDevice->CreateIndexBuffer(dwFileSize,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&mshObject.pIB,NULL);
	if (FAILED(hRetval))
	{
		CloseHandle(hFile);
		return DXTRACE_ERR("CreateIndexBuffer",hRetval);
	}
	pData = NULL;
	mshObject.pIB->Lock(0,0,&pData,0);
	ReadFile(hFile,pData,dwFileSize,&dwBytesRead,NULL);
	mshObject.pIB->Unlock();
	CloseHandle(hFile);

	return D3D_OK;
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication() :
	m_pTexFace(NULL),
	m_fTimeWeight(0.0f)
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "Effects Manipulation Framework" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = -0.44453f;
    m_fWorldRotY                = 0.0f;
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

    m_bLoadingApp = FALSE;
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
	bCapsAcceptable = pCaps->VertexShaderVersion >= D3DVS_VERSION(1,0);

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
//       CreateOffScreenPlainSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: create device objects
    HRESULT hr;

    // Init the font
    hr = m_pFont->InitDeviceObjects(m_pd3dDevice);
    if( FAILED( hr ) )
        return DXTRACE_ERR( "m_pFont->InitDeviceObjects", hr );

	hr = D3DXCreateTextureFromFile(m_pd3dDevice,TEXT("scene\\happySDKface.jpg"),&m_pTexFace);
	if (FAILED(hr))
        return DXTRACE_ERR( "D3DXCreateTextureFromFile", hr );
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
	// Load all scene stuff
	HRESULT hr = m_Effect.CreateFromFile(m_pd3dDevice,TEXT("scene\\Squish.fx"));
	if (FAILED(hr))
		return DXTRACE_ERR("m_Effect.CreateFromFile",hr);

	m_Effect("fAnimTime").Animate(0.0f,15.0f);
	m_Effect("fHandsRot").Animate(D3DXToRadian(-30.0f),D3DXToRadian(25.0f));

	// Load geometry
	hr = CreateGeometry(TEXT("scene\\Face.vb"),TEXT("scene\\Face.ib"),m_mshFace,true);
	if (FAILED(hr))
		return DXTRACE_ERR("CreateGeometry",hr);

	hr = CreateGeometry(TEXT("scene\\LeftHand.vb"),TEXT("scene\\LeftHand.ib"),m_mshLHand,false);
	if (FAILED(hr))
		return DXTRACE_ERR("CreateGeometry",hr);

	hr = CreateGeometry(TEXT("scene\\RightHand.vb"),TEXT("scene\\RightHand.ib"),m_mshRHand,false);
	if (FAILED(hr))
		return DXTRACE_ERR("CreateGeometry",hr);

    // TODO: setup render states

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0 );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

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

    // Update the camera according to user input
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;
	D3DXMATRIX matTemp;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX*2 );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY*2 );
	matTemp = matRotX*matRotY;

	D3DXVECTOR3 vec3Eye(0,5,-1);
	D3DXVECTOR3 vec3UpDir(0,1,0);
	D3DXVECTOR3 vec3Center(0,5,0);

	D3DXVec3TransformCoord(&vec3Eye,&vec3Eye,&matTemp);
	D3DXVec3TransformNormal(&vec3UpDir,&vec3UpDir,&matTemp);

	D3DXVec3Normalize(&vec3Eye,&vec3Eye);
	D3DXVec3Normalize(&vec3UpDir,&vec3UpDir);
	vec3Eye *= 35.0f;	// Eye point

	D3DXMatrixLookAtLH(&m_matViewProj,&vec3Eye,&vec3Center,&vec3UpDir);
	m_pd3dDevice->SetTransform(D3DTS_VIEW,&m_matViewProj);

	// Set the projection matrix
    float fAspect = ((float)m_d3dsdBackBuffer.Width) / ((float)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH(&matTemp,D3DX_PI/4,fAspect,1.0f,100.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matTemp);

	m_matViewProj *= matTemp;	// Save it for our shader

	static UINT uKeysCount = m_Effect("aKeys").GetElemsCount();

	float fOldNormWeight = fabsf(sinf(m_fTimeWeight));
	fOldNormWeight *= (float)uKeysCount;

	m_fTimeWeight += m_fElapsedTime*2.0f;
	float fNormWeight = fabsf(sinf(m_fTimeWeight));
	fNormWeight *= (float)uKeysCount;
	if ((UINT)fNormWeight >= uKeysCount)
		fNormWeight = (float)(uKeysCount-1);

	if (((int)fNormWeight == 7) && (fOldNormWeight>fNormWeight))
		PlaySound(TEXT("scene\\squish.wav"),NULL,SND_ASYNC|SND_FILENAME|SND_NOWAIT|SND_NOSTOP);

	m_Effect.SetAnimWeight(m_Effect("aKeys")[(int)fNormWeight]);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
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
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00000000, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: render world
		RenderHappyScene();

        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}


HRESULT CMyD3DApplication::RenderHappyScene()
{
	// Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 0.0f );
	mtrl.Ambient.r = mtrl.Ambient.g = mtrl.Ambient.b = .3f;
    m_pd3dDevice->SetMaterial( &mtrl );

	float fHeight = m_Effect("fAnimTime");

	static UINT uKeysCount = m_Effect("aHandsKeys").GetElemsCount();
	float fNormWeight = fabsf(sinf(m_fTimeWeight+0.3f));
	fNormWeight *= (float)uKeysCount;
	if ((UINT)fNormWeight >= uKeysCount)
		fNormWeight = (float)(uKeysCount-1);
	m_Effect.SetAnimWeight(m_Effect("aHandsKeys")[(int)fNormWeight]);

	D3DXMATRIX matRot,matTransform;

	// Hands use FFP
	m_pd3dDevice->SetTexture(0,NULL);
	m_pd3dDevice->SetVertexShader(NULL);

	m_pd3dDevice->SetIndices(m_mshLHand.pIB);
	m_pd3dDevice->SetStreamSource(0,m_mshLHand.pVB,0,m_mshLHand.uStride);
	m_pd3dDevice->SetFVF(m_mshLHand.dwFVF);

	D3DXMatrixRotationZ(&matRot,-(float)m_Effect("fHandsRot"));
	D3DXMatrixTranslation(&matTransform,0,fHeight,0);
	matTransform = matRot*matTransform;
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&matTransform);
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_mshLHand.dwVertsCount,0,m_mshLHand.dwTrisCount);


	m_pd3dDevice->SetIndices(m_mshRHand.pIB);
	m_pd3dDevice->SetStreamSource(0,m_mshRHand.pVB,0,m_mshRHand.uStride);
	m_pd3dDevice->SetFVF(m_mshRHand.dwFVF);

	D3DXMatrixRotationZ(&matRot,m_Effect("fHandsRot"));
	D3DXMatrixTranslation(&matTransform,0,fHeight,0);
	matTransform = matRot*matTransform;
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&matTransform);
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_mshLHand.dwVertsCount,0,m_mshLHand.dwTrisCount);
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_mshRHand.dwVertsCount,0,m_mshRHand.dwTrisCount);


	// Draw face using the effect
	D3DXMatrixTranslation(&matTransform,0,fHeight,0);

	m_Effect("matWorld") = matTransform;
	m_Effect("matViewProj") = m_matViewProj;
	m_Effect("vec4LightDir") = D3DXVECTOR4(0,0,1,0);
	m_Effect("vec4Diffuse") = D3DXVECTOR4(1,1,1,1);

	m_Effect("fSquishFactor") = 0.3f;

	m_Effect.GetD3DXEffect()->Begin(NULL,0);
	m_Effect.GetD3DXEffect()->Pass(0);

	m_pd3dDevice->SetTexture(0,m_pTexFace);
	m_pd3dDevice->SetIndices(m_mshFace.pIB);
	m_pd3dDevice->SetStreamSource(0,m_mshFace.pVB,0,m_mshFace.uStride);
	m_pd3dDevice->SetFVF(m_mshFace.dwFVF);
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_mshFace.dwVertsCount,0,m_mshFace.dwTrisCount);

	m_Effect.GetD3DXEffect()->End();
	return D3D_OK;
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
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), 
              m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
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

	SAFE_RELEASE(m_mshFace.pVB);
	SAFE_RELEASE(m_mshFace.pIB);

	SAFE_RELEASE(m_mshLHand.pVB);
	SAFE_RELEASE(m_mshLHand.pIB);

	SAFE_RELEASE(m_mshRHand.pVB);
	SAFE_RELEASE(m_mshRHand.pIB);

	m_Effect.Clear();
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
	SAFE_RELEASE(m_pTexFace);
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

    return S_OK;
}




