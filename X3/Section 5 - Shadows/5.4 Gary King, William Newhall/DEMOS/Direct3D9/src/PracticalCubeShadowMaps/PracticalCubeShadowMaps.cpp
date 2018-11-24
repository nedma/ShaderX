//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  PracticalCubeShadowMaps.cpp
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
// Note: This code uses the D3D Framework helper library.
// Link to: DX9SDKSampleFramework.lib d3dxof.lib dxguid.lib d3d9.lib winmm.lib comctl32.lib
//
//-----------------------------------------------------------------------------

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <shared/GetFilePath.h>
#include <shared/NVBScene9.h>
#include <DX9SDKSampleFramework/DX9SDKSampleFramework.h>

#include "MouseUIs.h"
#include "OmniDirectionalShadowMap.h"
#include "R32FCubeShadowMap.h"
#include "VSDCT.h"
#include "PracticalCubeShadowMaps.h"
#include "Bounding.h"

//  The extrusion test requires a reasonable number of FLOPS, so in scenes with lots of shadow casters, doing
//  it for every tiny one is overkill.  here's a system-dependent value that defines the crossover point
//  between just drawing the shadow caster, and actually testing it.  2.5 works well on my PC
const FLOAT ExtrusionTestThreshold = 2.5f;

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CPracticalCubeShadowMaps* g_pApp  = NULL;
HINSTANCE                 g_hInst = NULL;

//  a lot of code is in this header file, to keep the main file focused on the algorithmic implementations
#include "Framework.h"

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    CPracticalCubeShadowMaps d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if (FAILED(d3dApp.Create(hInst)))
        return 0;

    return d3dApp.Run();
}


//-----------------------------------------------------------------------------
// Name: CPracticalCubeShadowMaps()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CPracticalCubeShadowMaps::CPracticalCubeShadowMaps()
{
    m_dwCreationWidth           = 768;
    m_dwCreationHeight          = 512;
    m_strWindowTitle            = TEXT("PracticalCubeShadowMaps");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_bStartFullscreen          = false;
    m_bShowCursorWhenFullscreen = false;

    m_pFont                     = NULL;
    m_pScene                    = NULL;
    m_pMouseUI                  = NULL;
    m_pShadowMap                = NULL;
    m_pDeclaration              = NULL;
    m_pZBuffer                  = NULL;
    m_pBackBuffer               = NULL;
    m_pLimitQueuedFrames        = NULL;
    m_pDepthFirstEffect         = NULL;
    m_bQueryOut                 = FALSE;
    m_bNeedsLight               = TRUE;
    m_pScreenQuad               = NULL;  
    m_bLoadingApp               = TRUE;
    
    m_caps.m_bSupportsDepthTexture  = FALSE;
    m_caps.m_bSupportsR32F          = FALSE;
    m_caps.m_bSupportsEventQuery    = FALSE;
    m_caps.m_bIsRadeon              = FALSE;
    
    m_tweakables.m_ShadowTechnique        = SHADOW_R32FCUBE;
    m_tweakables.m_fIntensity             = 4.f;
    m_tweakables.m_fZBias                 = 0.f;
    m_tweakables.m_fSlopeScale            = 0.f;
    m_tweakables.m_bShowHelp              = FALSE;
    m_tweakables.m_bShowStats             = FALSE;
    m_tweakables.m_bCullFromCasterFrustum = FALSE;
    m_tweakables.m_bAnimateLight          = TRUE;
    m_tweakables.m_bDrawScissor           = FALSE;
    m_tweakables.m_bFilter                = FALSE;
    m_tweakables.m_bDebug                 = FALSE;

    m_d3dEnumeration.AppMinDepthBits = 24;
    m_d3dEnumeration.AppMinAlphaChannelBits = 8;
    m_d3dEnumeration.AppMinStencilBits = 0;

}

//-----------------------------------------------------------------------------
// Name: ~CPracticalCubeShadowMaps()
// Desc: Application destructor.  Paired with CPracticalCubeShadowMaps()
//-----------------------------------------------------------------------------
CPracticalCubeShadowMaps::~CPracticalCubeShadowMaps()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
    FinalCleanup();
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::FrameMove()
{
    float time = 0.f;
    static float lasttime = 0.f;
    static float theta = 0.f;
    static bool  first = true;

    time = (float)::timeGetTime() * 0.001f;
    
    if ( first )
    {
        lasttime = time;
        first = false;
    }

    if ( m_pMouseUI )
    {
        m_pMouseUI->UpdateKeyState();
        m_pMouseUI->Tick( time - lasttime );
        m_pMouseUI->GetViewMatrix( &m_mView );
    }

    if ( m_tweakables.m_bAnimateLight )
    {
        theta += (time - lasttime) * 0.25f;
        if (theta>=1.f) 
            theta = 0.f;
    }

    D3DXVECTOR3 vLP ( 1.f*cos(2.f*D3DX_PI*theta), 2.f, 1.f*sin(2.f*D3DX_PI*theta) );
    D3DXVECTOR3 vEP ( -m_mView._41, -m_mView._42, -m_mView._43 );

    m_pShadowMap->SetPositions( &vLP, &vEP );
    m_pShadowMap->SetZBias( m_tweakables.m_fZBias, m_tweakables.m_fSlopeScale );
    m_pShadowMap->SetLightAttenuation( m_tweakables.m_fIntensity );
    ComputeLightSourceBounds();

    lasttime = time;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::Render()
{
    BOOL bRenderShadowPass[6];   // 6 is the most passes used for a shadow map (cubemap, 1 per face)

    // Begin the scene
    int nPasses = m_pShadowMap->GetNumPasses();
    m_stats.shadowPassesRendered = 0;
    m_stats.shadowCasters = 0;
    m_stats.shadowCastersCulled = 0;
    m_stats.totalObjects = 0;
    m_stats.totalObjectsCulled = 0;

    D3DXMATRIX viewProjection;
    D3DXMatrixMultiply( &viewProjection, &m_mView, &m_mProjection );

    m_pDepthFirstEffect->SetMatrix( "cScreenViewProjection", &viewProjection );
    m_pShadowMap->SetViewProjectMatrix( &viewProjection );

    //  grab the view frustum
    Frustum viewFrustum(&viewProjection);
    m_pShadowMap->SetDebugMode( m_tweakables.m_bDebug );

    //  iterate through the shadow map passes, determining whether or not we need to render it this frame
    //  only render the faces of the shadowmap that are needed...
    for (int pass=0; pass<nPasses;pass++)
        bRenderShadowPass[pass] = m_pShadowMap->NeedsShadowPass(pass, &viewFrustum);

    //  wait for the previous frame to finish rendering, before we start rendering this one
    //  WARNING:  although this gets rid of mouse lag (and evens out frame rate fluctuations),
    //  forcing 0 queued frames hurts performance on multi-gpu systems.  there are better solutions
    //  available, but for the purposes of this demo, 0 queued frames will suffice
    if ( m_caps.m_bSupportsEventQuery && m_bQueryOut )
    {
        BOOL bFrameDone = FALSE;
        while (!bFrameDone)
        {
            m_pLimitQueuedFrames->GetData( &bFrameDone, sizeof(BOOL), D3DGETDATA_FLUSH );
        }
    }

    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTexture( 1, NULL );
    m_pd3dDevice->SetTexture( 2, NULL );
    m_pd3dDevice->SetTexture( 3, NULL );
    m_pShadowMap->Clear( D3DCLEAR_ZBUFFER, 0x00000000, 1.f );

    for (int pass=0; pass<nPasses; pass++)
    {
        if (!bRenderShadowPass[pass])
            continue;

        if (FAILED(m_pShadowMap->SetShadowPass(pass)))
            return E_FAIL;

        D3DXMATRIX shadowViewProjection;
        m_pShadowMap->GetViewProjectionMatrix( &shadowViewProjection );

        Frustum shadowFrustum( &shadowViewProjection );

        if (SUCCEEDED(m_pd3dDevice->BeginScene()))  // render shadow map
        {
            m_pd3dDevice->SetVertexDeclaration( m_pDeclaration );

            if ( m_pShadowMap->Begin()==S_OK )
            {
                if (FAILED(DrawAllObjects(-1, &shadowFrustum, TRUE, &viewFrustum )))
                    return E_FAIL;
                m_pShadowMap->End();
            }
            
            m_pd3dDevice->EndScene();
            m_stats.shadowPassesRendered++;
        }
    }

    m_pd3dDevice->SetRenderTarget( 0, m_pBackBuffer );
    m_pd3dDevice->SetDepthStencilSurface( m_pZBuffer );
    //  only clear the z buffer -- we should be overwriting the entire color buffer every frame
    m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER, 0x000000ff, 1.0f, 0L);

    //m_pd3dDevice->SetViewport( &m_viewport );

    if (SUCCEEDED(m_pd3dDevice->BeginScene()))   // render frame
    {
        m_pd3dDevice->SetVertexDeclaration( m_pDeclaration );
        m_pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, 0 );
        m_pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0 );

        //  draw depth first pass
        {
            if ( FAILED(m_pDepthFirstEffect->SetTechnique("TransformAmbient")) )
            {
                MessageBox( NULL, "Failed to set technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
                return E_FAIL;
            }

            UINT uPasses;

            if ( SUCCEEDED(m_pDepthFirstEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE)) )
            {
                m_pDepthFirstEffect->BeginPass(0);
                if (FAILED(DrawAllObjects( 1, &viewFrustum, FALSE, &viewFrustum )))
                    return E_FAIL;
				m_pDepthFirstEffect->EndPass();
                m_pDepthFirstEffect->End();
            }
        }

        if (m_bNeedsLight)
        {
            //  now, render lighting..  clamp to scissorrect
            m_pd3dDevice->SetScissorRect(&m_ScissorRect);
            m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );

            if (FAILED(m_pShadowMap->SetScreenPass()))
                return E_FAIL;
            
            if ( m_pShadowMap->Begin()==S_OK )
            {
                if (FAILED(DrawAllObjects( 1, &viewFrustum, FALSE, &viewFrustum )))
                    return E_FAIL;
                m_pShadowMap->End();
            }
            else
            {
                MessageBox(NULL, "Unable to set technique", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
            }
        }

        m_pd3dDevice->SetTexture( 0, NULL );
        m_pd3dDevice->SetTexture( 1, NULL );
        m_pd3dDevice->SetTexture( 2, NULL );
        m_pd3dDevice->SetTexture( 3, NULL );

        if ( m_tweakables.m_bDrawScissor && m_bNeedsLight )
        {
            if ( SUCCEEDED(m_pDepthFirstEffect->SetTechnique("GreenQuad")) )
            {
                UINT uPasses;
                m_pd3dDevice->SetVertexDeclaration(m_pScreenQuad->pVD);
                m_pd3dDevice->SetIndices(m_pScreenQuad->pIB);
                if ( SUCCEEDED(m_pDepthFirstEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE)) )
                {
                    m_pDepthFirstEffect->BeginPass(0);
                    m_pDepthFirstEffect->CommitChanges();
                    m_pd3dDevice->SetStreamSource(0, m_pScreenQuad->pVB, 0, sizeof(ScreenVertex));
                    if (FAILED(m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, 4, 0, 2 )))
                        return E_FAIL;
					m_pDepthFirstEffect->EndPass();
                    m_pDepthFirstEffect->End();
                }
            }
        }

        m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
      
        if ( SUCCEEDED(m_pShadowMap->SetDebugPass()))
        {
            m_pd3dDevice->SetVertexDeclaration(m_pScreenQuad->pVD);
            m_pd3dDevice->SetIndices(m_pScreenQuad->pIB);
            if ( m_pShadowMap->Begin() == S_OK )
            {
                m_pd3dDevice->SetStreamSource(0, m_pScreenQuad->pVB, 0, sizeof(ScreenVertex));
                if (FAILED(m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, 4, 0, 2 )))
                    return E_FAIL;
                m_pShadowMap->End();
            }
        }
        
        // Render stats and help text  
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xf );
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    if ( m_caps.m_bSupportsEventQuery )  // put a marker in the command stream indicating the end of the frame
    {
        m_pLimitQueuedFrames->Issue(D3DISSUE_END);
        m_bQueryOut = TRUE;
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////
//  Name: ComputeLightSourceBounds
//  Desc: Informs the shadow maps how big the light source is (used to dynamically resize the shadow maps),
//        and computes the scissor rectangle
/////////////////////////////////////////////////////////////////////////////////

void CPracticalCubeShadowMaps::ComputeLightSourceBounds()
{
    const float lightRadius = sqrt( m_tweakables.m_fIntensity / .02f ); //  cutoff light sphere where energy <2% of total
    D3DXVECTOR3 eyeVec( -m_mView._41, -m_mView._42, -m_mView._43 );

    D3DXVECTOR3 lightPosition;
    m_pShadowMap->GetPosition( &lightPosition );
    
    D3DXVECTOR3 radiusVec( lightRadius, lightRadius, lightRadius );
    
    BoundingBox lightBox;
    lightBox.minPt = lightPosition - radiusVec;
    lightBox.maxPt = lightPosition + radiusVec;

    D3DXMATRIX viewProjection;
    D3DXMatrixMultiply( &viewProjection, &m_mView, &m_mProjection );
    Frustum viewFrustum( &viewProjection );


    D3DXVECTOR3 viewportOffsetVec( (float)m_viewport.X, (float)m_viewport.Y, 0.f );

    RECT windowRect;

    if ( !viewFrustum.TestSphere(&lightPosition, lightRadius) )
    {
        windowRect.left = 0;
        windowRect.right = 1;
        windowRect.top = 0;
        windowRect.bottom = 1;
        m_ScissorRect.left = 0;
        m_ScissorRect.right = 0;
        m_ScissorRect.top = 0;
        m_ScissorRect.bottom = 0;
        m_bNeedsLight = FALSE;
    }
    //  if inside the light bounds (or dynamic resizing is disabled), set light to maximal area
    else if ( (D3DXVec3Length(&(lightPosition-eyeVec))<=lightRadius) )
    {
        m_ScissorRect.left = m_viewport.X;
        m_ScissorRect.right = m_viewport.X + m_viewport.Width;
        m_ScissorRect.top = m_viewport.Y;
        m_ScissorRect.bottom = m_viewport.Y + m_viewport.Height;
        windowRect.left = -2048;
        windowRect.right = 2048;
        windowRect.top = -2048;
        windowRect.bottom  = 2048;
        m_bNeedsLight = TRUE;
    }
    else
    {
        XFormBoundingBox( &lightBox, &lightBox, &viewProjection );
        lightBox.Clamp( &D3DXVECTOR3(-1.f, -1.f, 0.f), &D3DXVECTOR3(1.f, 1.f, 1.f) );
        lightBox.minPt = lightBox.minPt*0.5f + D3DXVECTOR3(0.5f, 0.5f, 0.5f);
        lightBox.maxPt = lightBox.maxPt*0.5f + D3DXVECTOR3(0.5f, 0.5f, 0.5f);            
        lightBox.minPt.x *= m_viewport.Width;  lightBox.maxPt.x *= m_viewport.Width;
        lightBox.minPt.y *= m_viewport.Height; lightBox.maxPt.y *= m_viewport.Height;
        lightBox.minPt += viewportOffsetVec;   lightBox.maxPt += viewportOffsetVec;

        windowRect.left   = (INT)lightBox.minPt.x;
        windowRect.top    = m_viewport.Height - (INT)(lightBox.maxPt.y+0.5f);
        windowRect.right  = (INT)(lightBox.maxPt.x+0.5f);
        windowRect.bottom = m_viewport.Height - (INT)lightBox.minPt.y;

        m_ScissorRect.left   = windowRect.left;
        m_ScissorRect.right  = windowRect.right;
        m_ScissorRect.top    = windowRect.top;
        m_ScissorRect.bottom = windowRect.bottom;

        m_bNeedsLight = TRUE;
    }
    
    //  HACKHACKHACK -- just force window to maximum size, if we don't want to dynamically size the shadow map
    if ( !m_tweakables.m_bDynamicSizing && m_bNeedsLight )
    {
        windowRect.left = -2048;
        windowRect.right = 2048;
        windowRect.top = -2048;
        windowRect.bottom = 2048;
    }

    m_pShadowMap->SetLightSourceBoundingRect( &windowRect );
    //  multiply by sqrt(3) to account for square feature
    //  multiply by sqrt(2) to increase energy cutoff to 1%
    m_pShadowMap->SetBounds(0.1f, lightRadius*1.4142f*1.73205f); 
}

//------------------------------------------------------------------------------
//  Name: CreateShadowMap
//  Desc: Creates the shadow map object.  Called by RestoreDeviceObjects and HandleKey
//------------------------------------------------------------------------------

HRESULT CPracticalCubeShadowMaps::CreateShadowMap()
{
    HRESULT hr;

    switch ( m_tweakables.m_ShadowTechnique )
    {
    case SHADOW_R32FCUBE:
        m_pShadowMap = static_cast<OmnidirectionalShadowMap*>(new R32FCubeShadowMap(m_caps.m_bIsRadeon));
        m_tweakables.m_fZBias = 0.05f;     // use a larger constant bias, since we are using world-space units
        m_tweakables.m_fSlopeScale = 0.f;  // slope-scale not supported
        break;
    case SHADOW_VSDCT:
        m_pShadowMap = static_cast<OmnidirectionalShadowMap*>(new VSDCTShadowMap(m_tweakables.m_bDynamicSizing));
        m_tweakables.m_fZBias = 0.00015f;  // in Z-buffer units
        m_tweakables.m_fSlopeScale = 1.1f;
        break;
    default:
        MessageBox( NULL, "Unsupported shadow type", "ERROR", MB_OK|MB_TOPMOST|MB_SETFOREGROUND );
        return E_FAIL;
    }

    m_pShadowMap->SetDevice( m_pd3dDevice );
    
    if ( FAILED(hr = m_pShadowMap->RestoreDeviceObjects()) )
        return hr;

    m_pShadowMap->FilterShadows( m_tweakables.m_bFilter );
    
    return S_OK;
}

//---------------------------------------------------------------------------
//  Name: DrawAllObjects
//  Desc: Frustum culls and draws all objects in the list
//------------------------------------------------------------------------------
HRESULT CPracticalCubeShadowMaps::DrawAllObjects( INT sampler, const Frustum* frustum, BOOL createShadowFrustum, const Frustum* viewerFrustum )
{
    D3DXVECTOR3 lightPos;
    m_pShadowMap->GetPosition(&lightPos);

    const float lightRadius = sqrt( m_tweakables.m_fIntensity / .01f ); //  cutoff light sphere where energy <1% of total

    for ( unsigned int iMesh=0; iMesh<m_pScene->m_NumMeshes; iMesh++ )
    {
        m_stats.totalObjects++;
        const NVBScene::Mesh& mesh = m_pScene->m_Meshes[iMesh];

        //  model boxes are already stored in world space (transformed in GetSceneBoundingBox)
        const BoundingBox& modelBox = m_modelBoxen[iMesh];
        BoundingBox lightBox;
        lightBox.minPt = lightPos - D3DXVECTOR3(lightRadius, lightRadius, lightRadius);
        lightBox.maxPt = lightPos + D3DXVECTOR3(lightRadius, lightRadius, lightRadius);

        if ( !frustum->TestBox( &modelBox ) )
        {
            //  basic frustum culling against the current projection matrix
            m_stats.totalObjectsCulled++;
            continue; 
        }

        if ( createShadowFrustum )
            m_stats.shadowCasters++;

        //  for tiny objects, it's probably faster just to blast them straight to the shadowmap than to
        //  do fancy CPU intersection tests -- the right value is system-dependent (CPU vs GPU speed)
        D3DXVECTOR3 fDiagVec = modelBox.maxPt - modelBox.minPt;
        float fDiagSqr = D3DXVec3Dot( &fDiagVec, &fDiagVec );

        if ( createShadowFrustum && m_tweakables.m_bCullFromCasterFrustum && (fDiagSqr>ExtrusionTestThreshold) )
        {
            if ( modelBox.TestPoint(&lightPos) )
            {
                //  light inside object bounding box: cull view frustum against light bounding box
                if ( !viewerFrustum->TestBox(&lightBox) )
                {
                    m_stats.shadowCastersCulled++;
                    continue;
                }
            }
            else
            {
                //  standard test -- build a frustum for the shadow caster, and perform intersection test with view frustum
                //  multiply by sqrt(3) to account for square features
                ExtrudedBoundingBox shadowVolume( &modelBox, &lightPos, lightRadius*1.73205f );
                
                if ( !viewerFrustum->TestExtrudedBox(&shadowVolume) )
                {
                    m_stats.shadowCastersCulled++;
                    continue;
                }
            }
        }

        HRESULT hr;
        D3DXMATRIX modelMatrix, modelITMatrix;
        modelMatrix = mesh.m_Transform;
        D3DXMatrixInverse( &modelITMatrix, NULL, &modelMatrix );
        D3DXMatrixTranspose( &modelITMatrix, &modelITMatrix );

        m_pDepthFirstEffect->SetMatrix( "cWorld", &modelMatrix );
        m_pDepthFirstEffect->SetMatrix( "cWorldIT", &modelITMatrix );
        m_pDepthFirstEffect->CommitChanges();
        m_pShadowMap->SetWorldMatrices( &modelMatrix, &modelITMatrix );

        if (sampler!=-1) m_pd3dDevice->SetTexture( sampler, mesh.m_DiffuseMap );

        if (FAILED(hr = mesh.Draw()))
            return hr;
    }
    return S_OK;
}