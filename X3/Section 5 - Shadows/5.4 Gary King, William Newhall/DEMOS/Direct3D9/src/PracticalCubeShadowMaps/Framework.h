//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  Framework.h
// 
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//-----------------------------------------------------------------------------


#ifndef __FRAMEWORK_H_included_
#define __FRAMEWORK_H_included_

////////////////////////////////////////////////////////////////////////////////
//    various functions related to the D3D9 sample framework setup, that aren't
//    fundamental to the algorithms used in the demo.
//    moved here, so that PracticalCubeShadowMaps.cpp is primarily an algorithm-driven file



///////////////////////////////////////////////////////////////////////////////
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
HRESULT CPracticalCubeShadowMaps::OneTimeSceneInit()
{
    m_pMouseUI = new MouseFPSUI( m_hWnd );
    m_pMouseUI->Reset();
    m_pMouseUI->SetTranslationalSensitivityFactor( 1.5f );
    m_pMouseUI->SetTranslate(0.f, 0.1875f, 0.f);
    // Drawing loading status message until app finishes loading
    SendMessage(m_hWnd, WM_PAINT, 0, 0);

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
HRESULT CPracticalCubeShadowMaps::ConfirmDevice(D3DCAPS9* pCaps, DWORD /*dwBehavior*/,
                                         D3DFORMAT adapterFormat, D3DFORMAT /*backBufferFormat*/)
{
    BOOL bCapsAcceptable = TRUE;

    //  device must support 2.0 pixel shaders for this demo to run
    if (pCaps->PixelShaderVersion<D3DPS_VERSION(2,0))
    {
        MessageBox( NULL, "Device does not support Shader Model 2.0", "ERROR", MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TOPMOST);
        return E_FAIL;
    }

    BOOL bR32F, bDepth, bSigned;

    //  device supports mipmapped R32f cubemaps as render targets
    bR32F =
        (CheckResourceFormatSupport( D3DFMT_R32F, D3DRTYPE_CUBETEXTURE, D3DUSAGE_RENDERTARGET, adapterFormat, pCaps )==S_OK) &&
        ((pCaps->TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP)==D3DPTEXTURECAPS_MIPCUBEMAP);

    //  device supports 2D depth/stencil textures
    bDepth =
        (CheckResourceFormatSupport( D3DFMT_D24X8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL, adapterFormat, pCaps )==S_OK);

    //  device supports signed cube maps
    bSigned =
        (CheckResourceFormatSupport( D3DFMT_Q8W8V8U8, D3DRTYPE_CUBETEXTURE, 0, adapterFormat, pCaps )==S_OK);

    //  device must support either R32F cubemaps or depth textures for this demo to run
    if ( !(bR32F || bDepth) )
    {
        MessageBox( NULL, "Device does not support either depth textures or floating-point cube maps", "ERROR", MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TOPMOST);
        return E_FAIL;
    }

    //  HACKHACKHACK -- to work around mipmap LOD biasing on radeons, check the vendor ID of the GPU in use.
    //  if it is an ATI GPU, use texCUBEbias, instead
    if ( m_pD3D )
    {
        D3DADAPTER_IDENTIFIER9 adapter;
        m_pD3D->GetAdapterIdentifier( pCaps->AdapterOrdinal, 0, &adapter );
        m_caps.m_bIsRadeon = (adapter.VendorId == 0x1002);
    }

    m_caps.m_bSupportsDepthTexture |= (bDepth && bSigned);
    m_caps.m_bSupportsR32F |= bR32F;
    m_tweakables.m_ShadowTechnique = (m_caps.m_bSupportsDepthTexture) ? SHADOW_VSDCT : SHADOW_R32FCUBE;

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
HRESULT CPracticalCubeShadowMaps::InitDeviceObjects()
{
    HRESULT hr;
    // Initialize the font
    HDC hDC = GetDC(NULL);
    int nHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    ReleaseDC(NULL, hDC);
    if (FAILED(hr = D3DXCreateFont(m_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE, 
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
                                   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                                   TEXT("Arial"), &m_pFont)))
        return DXTRACE_ERR("D3DXCreateFont", hr);

    m_bLoadingApp = FALSE;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
HRESULT CPracticalCubeShadowMaps::RestoreDeviceObjects()
{
    HRESULT hr;

    //  RestoreDeviceObjects shouldn't be called unless we are initializing the app for the first time,
    //  or after a reset (which destroys all of these variables)
    if ( m_pScene!=NULL || m_pShadowMap!=NULL || m_pDeclaration!=NULL || m_pBackBuffer!=NULL || m_pZBuffer!=NULL )
    {
        MessageBox( NULL, "RestoreDeviceObjects() called unexpectedly", "ERROR", MB_OK|MB_ICONHAND|MB_TOPMOST|MB_SETFOREGROUND);
        return E_FAIL;
    }

    m_pScene = new NVBScene();
    if ( FAILED(hr = m_pScene->Load(std::string("adobe_mar13.nvb"), m_pd3dDevice, GetFilePath::GetFilePath)) )
    {
        MessageBox( NULL, "Unable to load adobe_mar13.nvb from media directory", "ERROR", MB_OK|MB_ICONHAND|MB_TOPMOST|MB_SETFOREGROUND);
        return hr;
    }

    //  grab bounding boxes
    BoundingBox sceneBox;
    m_modelBoxen.clear();
    GetSceneBoundingBox( &sceneBox, &m_modelBoxen, m_pScene );

    if (m_pFont)
        m_pFont->OnResetDevice();

    //  create shadow map raises its own error message
    if ( FAILED(hr = CreateShadowMap()) )
        return hr;

    D3DVERTEXELEMENT9 decl[] = 
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 }, 
        { 0, 60, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 }, 
        D3DDECL_END()
    };

    m_pd3dDevice->GetDepthStencilSurface( &m_pZBuffer );
    m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer );

    if (FAILED(hr = D3DXCreateEffectFromFile(m_pd3dDevice, GetFilePath::GetFilePath("programs\\PracticalCubeShadowMaps\\PracticalCubeShadowMaps.fx").c_str(),
                             NULL, NULL, 0, NULL, &m_pDepthFirstEffect, NULL)))
    {
        MessageBox ( NULL, TEXT("Unable to load PracticalCubeShadowMaps.fx"), TEXT("ERROR"), MB_OK|MB_ICONHAND|MB_TOPMOST|MB_SETFOREGROUND);
        return hr;
    }

    if (FAILED(hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pDeclaration )))
    {
        MessageBox( NULL, TEXT("unable to create vertex declaration"), TEXT("ERROR"), MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
        return hr;
    }

    //  create an event query -- if the creation fails, don't use it.
    hr = m_pd3dDevice->CreateQuery( D3DQUERYTYPE_EVENT, &m_pLimitQueuedFrames );
    m_caps.m_bSupportsEventQuery = (hr==D3D_OK);
    m_pScreenQuad = new ScreenMesh( m_pd3dDevice );

    RECT windowRect;
    ::GetClientRect( m_hWnd, &windowRect );

    LONG width  = windowRect.right - windowRect.left;
    LONG height = windowRect.bottom - windowRect.top;

    m_viewport.X = m_viewport.Y = 0;
    m_viewport.Height = height; m_viewport.Width = width; m_viewport.MinZ = 0.f; m_viewport.MaxZ = 1.f;

    float fAspect = ((float)width / (float)height);

    D3DXMatrixPerspectiveFovLH ( &m_mProjection, D3DXToRadian(60.f), fAspect, 0.1f, 400.f );

    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CPracticalCubeShadowMaps::MsgProc(HWND hWnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam)
{
    switch(msg)
    {
    case WM_CHAR:
        HandleKey( (DWORD)wParam, FALSE );
        break;
    case WM_KEYDOWN:
        HandleKey( (DWORD)wParam, TRUE );
        break;
    case WM_MOUSEMOVE:
        if ( m_pMouseUI ) m_pMouseUI->OnMouseMove( LOWORD(lParam), HIWORD(lParam) );
        break;
    case WM_LBUTTONDOWN:
        if ( m_pMouseUI ) m_pMouseUI->OnLButtonDown( LOWORD(lParam), HIWORD(lParam) );
        break;
    case WM_LBUTTONUP:
        if ( m_pMouseUI ) m_pMouseUI->OnLButtonUp( LOWORD(lParam), HIWORD(lParam) );
        break;
    case WM_PAINT:
    {
        if (m_bLoadingApp)
        {
            // Draw on the window tell the user that the app is loading
            // TODO: change as needed
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
    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pDeclaration );
    SAFE_RELEASE( m_pBackBuffer );
    SAFE_RELEASE( m_pZBuffer );
    SAFE_RELEASE( m_pLimitQueuedFrames );
    SAFE_RELEASE( m_pDepthFirstEffect );
    SAFE_DELETE( m_pScene );
    SAFE_DELETE( m_pShadowMap );
    SAFE_DELETE( m_pScreenQuad );

    if (m_pFont)
        m_pFont->OnLostDevice();

    m_bQueryOut = FALSE;  // reset state

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::DeleteDeviceObjects()
{
    SAFE_RELEASE(m_pFont);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::FinalCleanup()
{
    // TODO: Perform any final cleanup needed
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CheckResourceFormatSupport()
// Desc: Checks if the provided resource and usage is supported on the current
//       device
//-----------------------------------------------------------------------------

HRESULT CPracticalCubeShadowMaps::CheckResourceFormatSupport(D3DFORMAT fmt, D3DRESOURCETYPE resType, 
                                                             DWORD dwUsage, D3DFORMAT adapterFormat,
                                                             D3DCAPS9* pCaps)
{
    return m_pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, adapterFormat, dwUsage, resType, fmt);   
}


//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::RenderText()
{
    D3DCOLOR fontColor    = D3DCOLOR_ARGB(255, 255, 255, 0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    RECT rct;
    ZeroMemory(&rct, sizeof(rct));       

    rct.left   = 2;
    rct.right  = m_d3dsdBackBuffer.Width - 20;

    // Output display stats
    INT nNextLine = 0; 

    wsprintf(szMsg, TEXT("%s"), (m_tweakables.m_bShowHelp)?TEXT("Press F1 to Hide Help"):TEXT("Press F1 to Show Help"));
    rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
    m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

    wsprintf(szMsg, TEXT("Shadow Mode [%s]    Caster Culling [%s]    Filtering [%s]    Dynamic Resize [%s]"), 
                            m_pShadowMap->GetName(),
                            (m_tweakables.m_bCullFromCasterFrustum)?TEXT("enabled"):TEXT("disabled"),
                            (m_tweakables.m_bFilter)?TEXT("yes"):TEXT("no"),
                            (m_tweakables.m_bDynamicSizing)?TEXT("on"):TEXT("off"));
    rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
    m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

    sprintf(szMsg, TEXT("Intensity [%.2f]     ZBias[%.5f]     Slope Scale Bias[%.2f]"),
                           m_tweakables.m_fIntensity,
                           m_tweakables.m_fZBias,
                           m_tweakables.m_fSlopeScale);
    rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
    m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

    // Output help
    if ( m_tweakables.m_bShowHelp )
    {
        nNextLine += 20;
        wsprintf(szMsg, TEXT("F1: Toggle Help    F2: Toggle Stats    F3: Toggle Filter%s"),
            (m_caps.m_bSupportsDepthTexture)?TEXT("    ~: Debug View (VSDCT only)"):TEXT(""));
        rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);
        
        wsprintf(szMsg, TEXT("F4: Toggle Scissor Rect    F5: Toggle Caster Culling    F6: Toggle Dynamic Sizing"));
        rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

        wsprintf(szMsg, TEXT("[/]: Change Intensity    +/-: Change ZBias    </>: Change Slop Scale (VSDCT only)"));
        rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

        wsprintf(szMsg, TEXT("SPACE: Stop Animation%s%s"),
            (m_caps.m_bSupportsR32F)?TEXT("    1: Use R32F Shadow Map"):TEXT(""),
            (m_caps.m_bSupportsDepthTexture)?TEXT("    2: Use VSDCT Shadow Map"):TEXT(""));
        rct.top = nNextLine; nNextLine += 20; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);
    }

    // Output stats (if necessary)
    
    if ( m_tweakables.m_bShowStats )
    {
        nNextLine = m_d3dsdBackBuffer.Height; 
        wsprintf(szMsg, TEXT("shadow passes rendered: %d of %d"), m_stats.shadowPassesRendered, m_pShadowMap->GetNumPasses());
        nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

        float percentCull = float(m_stats.totalObjectsCulled) / float(m_stats.totalObjects) * 100.f;

        wsprintf(szMsg, TEXT("frustum culling killed %d%% of %d objects"), (int)percentCull, m_stats.totalObjects);
        nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

        percentCull = float(m_stats.shadowCastersCulled) / float(m_stats.shadowCasters) * 100.f;
        wsprintf(szMsg, TEXT("shadow caster culling killed %d%% of %d possible casters"), (int)percentCull, m_stats.shadowCasters);
        nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);

        lstrcpy(szMsg, m_strFrameStats);
        nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
        m_pFont->DrawText(NULL, szMsg, -1, &rct, DT_NOCLIP, fontColor);
    }


    return S_OK;
}

//------------------------------------------------------------------------------
//  Name: HandleKey()
//  Desc: Processes key presses
//------------------------------------------------------------------------------

void CPracticalCubeShadowMaps::HandleKey( DWORD wParam, BOOL bIsVirtKey )
{
    //  W,A,S,D are swallowed by MouseUI, for movement
    if ( !bIsVirtKey )
    {
        switch ((CHAR)wParam)
        {
        case ' ':
            m_tweakables.m_bAnimateLight = !m_tweakables.m_bAnimateLight;
            break;

        case '.':
        case '>':
            m_tweakables.m_fSlopeScale += 0.01f; 
            break;
        case ',':
        case '<':
            m_tweakables.m_fSlopeScale -= 0.01f;
            break;

        case '{':
        case '[':
            m_tweakables.m_fIntensity *= 0.5f;
            break;
        
        case '}':
        case ']':
            m_tweakables.m_fIntensity *= 2.f;
            break;

        case '=':
        case '+':
            switch (m_tweakables.m_ShadowTechnique)
            {
            case SHADOW_R32FCUBE: m_tweakables.m_fZBias += 0.005f; break;
            case SHADOW_VSDCT:    m_tweakables.m_fZBias += 0.00005f; break;
            }
            break;
        case '-':
        case '_':
            switch (m_tweakables.m_ShadowTechnique)
            {
            case SHADOW_R32FCUBE: m_tweakables.m_fZBias -= 0.005f; break;
            case SHADOW_VSDCT:    m_tweakables.m_fZBias -= 0.00005f; break;
            }
            break;

        case '1':  // change to r32f shadow maps, iff we aren't already using r32f shadow maps & r32f is supported
        case '!':
            if ( m_caps.m_bSupportsR32F && (m_tweakables.m_ShadowTechnique!=SHADOW_R32FCUBE) )
            {
                SAFE_DELETE( m_pShadowMap );
                m_tweakables.m_ShadowTechnique = SHADOW_R32FCUBE;
                if ( FAILED(CreateShadowMap()) )
                {
                    MessageBox(NULL, "Failed to set shadow technique R32F cube", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
                    SendMessage(NULL, WM_QUIT, 0, 0);
                }
            }
            break;
        case '2':
        case '@':
            if ( m_caps.m_bSupportsDepthTexture && (m_tweakables.m_ShadowTechnique!=SHADOW_VSDCT))
            {
                SAFE_DELETE( m_pShadowMap );
                m_tweakables.m_ShadowTechnique = SHADOW_VSDCT;
                if ( FAILED(CreateShadowMap()) )
                {
                    MessageBox(NULL, "Failed to set shadow technique VSDCT", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
                    SendMessage(NULL, WM_QUIT, 0, 0);
                }
            }
            break;
      
        case '`':
        case '~':
            if (m_tweakables.m_ShadowTechnique==SHADOW_VSDCT)
                m_tweakables.m_bDebug = !m_tweakables.m_bDebug;
            break;
        }
    }
    else
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;

        case VK_F1:  // help
            m_tweakables.m_bShowHelp = !m_tweakables.m_bShowHelp;
            break;

        case VK_F2:  // stat reporting
            m_tweakables.m_bShowStats = !m_tweakables.m_bShowStats;
            break;

        case VK_F3:   // filter
            m_tweakables.m_bFilter = !m_tweakables.m_bFilter;
            m_pShadowMap->FilterShadows( m_tweakables.m_bFilter );
            break;
            
        case VK_F4:   // show scissor rect
            m_tweakables.m_bDrawScissor = !m_tweakables.m_bDrawScissor;
            break;

        case VK_F5:   // fancy culling
            m_tweakables.m_bCullFromCasterFrustum = !m_tweakables.m_bCullFromCasterFrustum;
            break;

        case VK_F6:   // dynamic shadow map resizing
            m_tweakables.m_bDynamicSizing = !m_tweakables.m_bDynamicSizing;
            SAFE_DELETE( m_pShadowMap );
            if ( FAILED(CreateShadowMap()) )
            {
                MessageBox(NULL, "Failed to set shadow technique unroll depth", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
                SendMessage(NULL, WM_QUIT, 0, 0);
            }
            break;
        }
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ScreenMesh
//    all of the various functions for handling screenmeshes
//    pretty simple stuff
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ScreenMesh::ScreenMesh(LPDIRECT3DDEVICE9 dev)
{
    ScreenVertex mesh[4] = {
        { -1.f, -1.f, 0.f },
        {  1.f, -1.f, 0.f },
        { -1.f,  1.f, 0.f },
        {  1.f,  1.f, 0.f } };

    unsigned short indices[4] = { 0, 1, 2, 3 };

    dwFVF = D3DFVF_XYZ;
    pVB = NULL;
    pIB = NULL;
    pVD = NULL;
    dwNumVerts = 4;
    dwNumFaces = 2;

    D3DVERTEXELEMENT9 declaration[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
        D3DDECL_END()
    };

    if (FAILED(dev->CreateVertexBuffer( 4*sizeof(ScreenVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL )))
    {
        MessageBox(NULL, "unable to create vertex buffer", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
    }
    if (FAILED(dev->CreateIndexBuffer ( 4*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB, NULL )))
    {
        MessageBox(NULL, "unable to create index buffer", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
    }

    dev->CreateVertexDeclaration( declaration, &pVD );

    unsigned short* pDataIndex = NULL;
    pIB->Lock( 0, 0, (void**) &pDataIndex, 0 );
    memcpy(pDataIndex, indices, 4*sizeof(unsigned short));
    pIB->Unlock();

    float* pDataVertex = NULL;
    pVB->Lock(0,0,(void**)&pDataVertex, 0);
    memcpy(pDataVertex, mesh, 4*sizeof(ScreenVertex));
    pVB->Unlock();
}

ScreenMesh::~ScreenMesh()
{
    SAFE_RELEASE( pVB );
    SAFE_RELEASE( pIB );
    SAFE_RELEASE( pVD );
}


#endif