//-----------------------------------------------------------------------------
// File: ExampleOne.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------

#include "ExampleOne.h"

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CBaseD3DApplication*	g_pApp  = NULL;
HINSTANCE				g_hInst = NULL;

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
// Desc: Constructor for derived application class.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_strWindowTitle            = TEXT("Example: Charcoal Rendering");
	m_bCheckPSsupport			= FALSE;

}

//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Destructor for derived application class.
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{

}


//-----------------------------------------------------------------------------
// Name: CreateShaders()
// Desc: Loads and compiles the shaders, used by this application.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateShaders()
{
	HRESULT				hr;
	LPD3DXBUFFER        pCode;
	LPD3DXBUFFER        pErrorMsgs;
	TCHAR				szShaderPath[512];

	// Compile the vertex shader
	if(FAILED(DXUtil_FindMediaFileCb(szShaderPath, sizeof(szShaderPath), _T("Charcoal.vsh")))) 
		return E_FAIL;

	hr = D3DXCompileShaderFromFile(szShaderPath, NULL, NULL, _T("main"), _T("vs_1_1"), 0, &pCode, &pErrorMsgs , NULL);
	if(FAILED(hr))
	{
		char* szMsg	= (char*)pErrorMsgs->GetBufferPointer();
		OutputDebugString(szMsg);
		return E_FAIL;
	}
	else
	{
		m_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),  &m_pVertexShader );
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsgs);
	}

	// Compile the pixel shader
	if(FAILED(DXUtil_FindMediaFileCb(szShaderPath, sizeof(szShaderPath), _T("Charcoal.psh"))))
		return S_FALSE;

	hr = D3DXCompileShaderFromFile(szShaderPath, NULL, NULL, _T("main"), _T("ps_1_4"), 0, &pCode, &pErrorMsgs , NULL);
	if(FAILED(hr))
	{
		char* szMsg	= (char*)pErrorMsgs->GetBufferPointer();
		OutputDebugString(szMsg);
		return S_FALSE;
	}
	else
	{
		hr = m_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(),  &m_pPixelShader );
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsgs);
		if(FAILED(hr))
		{
			m_pPixelShader = NULL;
			return S_FALSE;
		}
	}


	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitializeLights()
// Desc: Initializes the light settings
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitializeLights()
{
	// Scene light (directional)
//	m_vLight1			= 	D3DXVECTOR4(-0.2f, 1.0f, 0.0f, 0.0f); // First light
	m_vLight1			= 	D3DXVECTOR4(-0.2f, 10.0f, 0.0f, 1.0f); // First light
	m_vLight2			= 	D3DXVECTOR4(-1.5f, 1.0f, 1.0f, 0.0f); // Second light, usage optional in sample
	D3DXVec4Normalize(&m_vLight1, &m_vLight1);
	D3DXVec4Normalize(&m_vLight2, &m_vLight2);

   m_vVSParameters[0]  = 0.1f; // ambient term for phong shading
   m_vVSParameters[1]  = g_fContrastExponent; // constrast exponent

   m_vPSParameters[0]  = 0.6f; // Blend ratio
   m_vPSParameters[1]  = 0.0f; 
   m_vPSParameters[2]  = 0.0f;
   m_vPSParameters[3]  = 0.0f;

    return S_OK;
}

HRESULT CMyD3DApplication::FrameMove()
{
   // Move the light
   if(m_UserInput.bUp)
         m_vLight1.y -= 1.5f  * m_fElapsedTime;
   if(m_UserInput.bDown)
         m_vLight1.y += 1.5f  * m_fElapsedTime;
   if(m_UserInput.bLeft)
         m_vLight1.x += 1.5f  * m_fElapsedTime;
   if(m_UserInput.bRight)
         m_vLight1.x -= 1.5f  * m_fElapsedTime;
   if(m_UserInput.bForward)
         m_vLight1.z += 1.0f  * m_fElapsedTime;
   if(m_UserInput.bBackward)
         m_vLight1.z -= 1.0f  * m_fElapsedTime;

 	D3DXVec4Normalize(&m_vLight1, &m_vLight1);


//   if (m_vLight1.z >= 2.7f) m_vLight1.z = 2.7f;
//	if (m_vLight1.z <= 0.7f) m_vLight1.z = 0.7f;

   // Call to base class
   return CBaseD3DApplication::FrameMove();
}

//-----------------------------------------------------------------------------
// Name: SetShaderConstants()
// Desc: Is called every frame, to provide the shaders with the needed 
//       constants
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::SetShaderConstants()
{

    // Update the position matrix (inverse of the camera transformation)
	D3DXVECTOR4 vCamera(m_matPosition._41, m_matPosition._42, m_matPosition._43, m_matPosition._44);


	// Set the shader constants
	m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&m_matWorldViewProj,		4);
	m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&m_matWorldView,			4);
	m_pd3dDevice->SetVertexShaderConstantF(8, (float*)&m_matWorld		,		4);
	
	m_pd3dDevice->SetVertexShaderConstantF(13, (float*)&vCamera,				1);
	m_pd3dDevice->SetVertexShaderConstantF(14, (float*)&m_vLight1,				1);
	m_pd3dDevice->SetVertexShaderConstantF(15, (float*)&m_vLight2,				1);
	m_pd3dDevice->SetVertexShaderConstantF(16, (float*)&m_vVSParameters,		1);

	m_pd3dDevice->SetPixelShaderConstantF(2 , m_vPSParameters,		1);
	m_pd3dDevice->SetPixelShaderConstantF(3 , (float*)&m_vLight1,	1);

   // Set textures
	m_pd3dDevice->SetTexture(0, m_texRandom);
	m_pd3dDevice->SetTexture(1, m_texContrast);
	m_pd3dDevice->SetTexture(2, m_texPaper);

	// Set texture scaling matrix
/*
	D3DXMATRIX	matTexture;
    D3DXMatrixIdentity(&matTexture);
	D3DXMatrixScaling(&matTexture, 2.5f, 2.50f, 2.0f);
	m_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &matTexture);
*/
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: PreRender()
// Desc: Called before the modell is rendered
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::PreRender()
{
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PostRender()
// Desc: Called after the modell has been rendered
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::PostRender()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(128,128,128,0);
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

    lstrcpy( szMsg, TEXT("Use A to toggle the model rotation.") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Use the arrow keys + HOME/END, to move the light.") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    _stprintf( szMsg, TEXT("Light dir: (%.2f/%.2f/%.2f)"), m_vLight1.x, m_vLight1.y, m_vLight1.z);
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}





