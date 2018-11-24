//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  VSDCT.cpp
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


#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <assert.h>
#include <string>

#include <shared/GetFilePath.h>
#include "VSDCT.h"
#include "d3dutil.h"
#include "Bounding.h"


VSDCTShadowMap::~VSDCTShadowMap()
{
    InvalidateDeviceObjects();
}

//--------------------------------------------------------------------
//  RestoreDeviceObjects()
//    Creates the render targets used by a VSDCT shadow map
//--------------------------------------------------------------------

HRESULT VSDCTShadowMap::RestoreDeviceObjects()
{
    assert( m_pd3dDevice != NULL );

    //  this is the actual shadow map (D24X8 texture, created with usage DEPTHSTENCIL)
    if (FAILED(m_pd3dDevice->CreateTexture(s_iDepthDimensionX, s_iDepthDimensionY, 1,
                                 D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24X8, D3DPOOL_DEFAULT, &m_pDepthTexture, NULL)))
    {
        MessageBox( NULL, "Unable to create depth texture", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  grab the surface, so we can use the depth texture as a depth buffer for rendering
    if ( FAILED(m_pDepthTexture->GetSurfaceLevel(0, &m_pDepthSurface)) )
    {
        MessageBox( NULL, "Unable to get surface 0 from depth texture", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  create a color buffer render target, corresponding to the depth buffer
    //  don't need a lot of color precision here...
    if ( FAILED(m_pd3dDevice->CreateTexture(s_iDepthDimensionX, s_iDepthDimensionY, 1, D3DUSAGE_RENDERTARGET,
                                 D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_pColorTexture, NULL)) )
    {
        MessageBox( NULL, "Unable to create color render target", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  and cache the surface pointer, for use with SetRenderTarget
    if ( FAILED(m_pColorTexture->GetSurfaceLevel(0, &m_pColorSurface)) )
    {
        MessageBox( NULL, "Unable to get surface 0 from color texture", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  the indirection texture is allocated in vidmem
    if (FAILED(m_pd3dDevice->CreateCubeTexture(m_iIndirectionSize, 1, 0, 
                                 D3DFMT_G16R16, D3DPOOL_DEFAULT, &m_pIndirectionTexture, NULL)))
    {
        MessageBox( NULL, "Unable to create G16R16 vidmem cubemap", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  so a corresponding texture needs to be allocated in sys mem, so that we can update the vidmem version
    if (FAILED(m_pd3dDevice->CreateCubeTexture(m_iIndirectionSize, 1, 0, 
                                 D3DFMT_G16R16, D3DPOOL_SYSTEMMEM, &m_pIndirectionTextureSysMem, NULL)))
    {
        MessageBox( NULL, "Unable to create G16R16 sysmem cubemap", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  create a cubemap for FaceSelectionTexture, if we are using limited indirection
    if ( m_bReallocateViewports && FAILED(FillFaceSelectionCube()) )
        return E_FAIL;

    HRESULT hr = D3DXCreateEffectFromFile(m_pd3dDevice, GetFilePath::GetFilePath("programs\\PracticalCubeShadowMaps\\VSDCT.fx").c_str(),
                                          NULL, NULL, 0, NULL, &m_pEffect, NULL);
    if (FAILED(hr))
    {
        ::MessageBox(NULL, "Failed to load VSDCT.fx", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_ICONHAND);
        return hr;
    }

    for (int i=0; i<6; i++)
        m_bViewportAllocated[i] = FALSE;

    return S_OK;
}

//--------------------------------------------------------------------
//  InvalidateDeviceObjects()
//    Destroys objects
//--------------------------------------------------------------------

HRESULT VSDCTShadowMap::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pDepthSurface );
    SAFE_RELEASE( m_pDepthTexture );
    SAFE_RELEASE( m_pColorSurface );
    SAFE_RELEASE( m_pColorTexture );
    SAFE_RELEASE( m_pIndirectionTexture );
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pIndirectionTextureSysMem );
    SAFE_RELEASE( m_pFaceSelectionTexture );
    return S_OK;
}

//--------------------------------------------------------------------
//  SetLightSourceBoundingRect()
//    Selects the appropriate mipmap LOD for rendering & shadow test,
//    based on the screen-space area of the light source
//--------------------------------------------------------------------

HRESULT VSDCTShadowMap::SetLightSourceBoundingRect( const LPRECT bounds )
{
    LONG width  = bounds->right  - bounds->left; width = min( 8192, width );
    LONG height = bounds->bottom - bounds->top;  height = min( 8192, height );
    LONG numPixels = min(s_iDepthDimensionX * s_iDepthDimensionY, width*height);
    LONG mipLevel = 0;

    while ( (6*(1<<mipLevel)*(1<<mipLevel)) < 2*numPixels )  // sample shadow structure the frequency we will display it
        mipLevel++;                        // only true for uniform sampling from view, but this works reasonably well

    mipLevel = s_iLogMaxFaceResolution - mipLevel;     // L0 is finest in mip pyramid, not N
    mipLevel = (mipLevel<0) ? 0 : mipLevel;     // clamp to finest LOD (shadow will be magnified, if this happens)
    m_iSelectedLOD = (m_bReallocateViewports)?min(3, mipLevel):0;
    return S_OK;
}

//--------------------------------------------------------------------
//  Clear()
//    Clears the Z and color buffers, and deallocates all indirect viewports
//    doesn't preserve previous state
//--------------------------------------------------------------------

HRESULT VSDCTShadowMap::Clear(DWORD Flags, D3DCOLOR Color, float Z)
{
    assert( m_pd3dDevice != NULL );
    assert( m_pDepthSurface != NULL );
    assert( m_pColorSurface != NULL );

    m_fClearZ = Z;

    if ( m_bDebug )
    {
        m_pd3dDevice->SetDepthStencilSurface( m_pDepthSurface );
        m_pd3dDevice->SetRenderTarget( 0, m_pColorSurface );
        DWORD dwFlags = (m_bDebug) ? D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET : D3DCLEAR_ZBUFFER;
        m_pd3dDevice->Clear( 0, NULL, dwFlags, D3DCOLOR_ARGB(0xff, 0x0, 0xff, 0x0), m_fClearZ, 0 );
    }

    if ( m_bReallocateViewports )
        ZeroMemory( m_bViewportAllocated, sizeof(m_bViewportAllocated) );

    return S_OK;
}

//------------------------------------------------------------------------------
//  GetViewProjectionMatrix
//    Returns the world->cubemap clip space matrix for the active pass(face)
//    This is to allow an application to perform frustum culling with world-space
//    bounding boxes/spheres

void VSDCTShadowMap::GetViewProjectionMatrix(D3DXMATRIX *viewProjection, int Pass) const
{
    D3DXMATRIX cubeProject, cubeView, lightTranslate;

    //  positions are specified in eye space
    D3DXMatrixTranslation(&lightTranslate, -m_vPosition.x, -m_vPosition.y, -m_vPosition.z);

    cubeView = D3DUtil_GetCubeMapViewMatrix( (DWORD) Pass );
    D3DXMatrixPerspectiveFovLH( &cubeProject, D3DXToRadian(90.f), 1.f, m_fNear, m_fFar );

    D3DXMatrixMultiply (viewProjection, &lightTranslate, &cubeView);
    D3DXMatrixMultiply (viewProjection,  viewProjection, &cubeProject);
}

//-----------------------------------------------------------------------------
//  SetWorldMatrices
//    Sets the World and WorldIT matrix for the effect

void VSDCTShadowMap::SetWorldMatrices( const D3DXMATRIX *world, const D3DXMATRIX *worldIT )
{
    if (world)
        m_pEffect->SetMatrix( "cWorld", world );

    if (worldIT)
        m_pEffect->SetMatrix( "cWorldIT", worldIT );

    if ( world || worldIT )
        m_pEffect->CommitChanges();
}

//-----------------------------------------------------------------------------
//  SetViewProjectMatrix
//    Sets the screen view/projection matrix

void VSDCTShadowMap::SetViewProjectMatrix( const D3DXMATRIX *viewProject )
{
    if (viewProject)
    {
        m_pEffect->SetMatrix( "cScreenViewProjection", viewProject );
        m_pEffect->CommitChanges();
    }
}

//-----------------------------------------------------------------------------
//  Begin
//    Passes Begin to m_pEffect
HRESULT VSDCTShadowMap::Begin()
{
    UINT uPasses;

    if (!FAILED(m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE)))
    {
        if (FAILED(m_pEffect->BeginPass(0)))
            return E_FAIL;
        m_pEffect->CommitChanges();
        return S_OK;
    }
    else
    {
        MessageBox( NULL, "Error beginning effect", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }
}

//-----------------------------------------------------------------------------
//  End
//    Passes End to m_pEffect
HRESULT VSDCTShadowMap::End()
{
	m_pEffect->EndPass();
    m_pEffect->End();
    return S_OK;
}

//-----------------------------------------------------------------------------
//  SetShadowPass
//    Sets the active technique and render target to one appropriate for the
//    shadowmap face we want to render into

HRESULT VSDCTShadowMap::SetShadowPass( int Pass )
{
    if ( Pass<0 || Pass>5 || !m_pDepthSurface || !m_pColorSurface )
        return E_FAIL;

    //  compute a viewport transform for the current shadow pass.  find a good viewport if we don't have one already
    if ( !m_bViewportAllocated[ Pass ] )
    {
        if (FAILED(AllocateViewport( Pass )))
            return E_FAIL;
    }

    //  render the color buffer if debugging, too.  otherwise, just depth
    std::string techniqueName = (m_bDebug) ? std::string( "GenShadowMap_Color" ) : 
                                             std::string( "GenShadowMap_Nocolor" );

    if ( FAILED(m_pEffect->SetTechnique(techniqueName.c_str())) )
    {
        MessageBox( NULL, "Failed to set technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    m_iActivePass = Pass;

    HRESULT hr = m_pd3dDevice->SetDepthStencilSurface( m_pDepthSurface );
    if (SUCCEEDED(hr))
        hr = m_pd3dDevice->SetRenderTarget( 0, m_pColorSurface );

    if (FAILED(hr))
    {
        MessageBox( NULL, "Unable to set render target to cubemap", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return hr;
    }

    m_pd3dDevice->SetViewport(&m_FaceViewport[Pass]);
    
    D3DRECT clearRect;
    clearRect.x1 = m_FaceViewport[Pass].X;
    clearRect.x2 = m_FaceViewport[Pass].X + m_FaceViewport[Pass].Width;
    clearRect.y1 = m_FaceViewport[Pass].Y;
    clearRect.y2 = m_FaceViewport[Pass].Y + m_FaceViewport[Pass].Height;

    if (!m_bDebug)  // in debug mode, clear the entire zbuffer -- otherwise, just clear the portion we are going to use
    {
        m_pd3dDevice->Clear( 1, &clearRect, D3DCLEAR_ZBUFFER, 0x00000000, m_fClearZ, 0 );
    }

    m_pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, *(DWORD*)&m_fZBias );
    m_pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&m_fSlopeScale );

    D3DXMATRIX cubeView, cubeProject, cubeViewProjection;

    cubeView = D3DUtil_GetCubeMapViewMatrix( (DWORD)Pass );
    D3DXMatrixPerspectiveFovLH( &cubeProject, D3DXToRadian(90.f), 1.f, m_fNear, m_fFar );

    D3DXMatrixMultiply( &cubeViewProjection, &cubeView, &cubeProject );

    m_pEffect->SetMatrix( "cCubemapViewProjection", &cubeViewProjection );

    D3DXVECTOR4 vLightPos4( m_vPosition.x, m_vPosition.y, m_vPosition.z, 0.f );
    m_pEffect->SetVector( "cLightPositionWorld", &vLightPos4 );

    //  a fudge factor, so that all the texel clamping works out correctly (see VSDCT.fx for more details)
    D3DXVECTOR4 vIndirectScaleBias( (float)m_iIndirectionSize / (float)(m_iIndirectionSize+1), 0.f, 0.f, 0.f );
    m_pEffect->SetVector( "cIndirectionScaleBias", &vIndirectScaleBias );
    
    return S_OK;
}

//-----------------------------------------------------------------------------
//  SetScreenPass
//    Sets the active technique to the appropriate one for the screen, and
//    sets any render states necessary for this

HRESULT VSDCTShadowMap::SetScreenPass()
{
    //  there are 2 different VSDCT techniques:
    //  1 for full-res indirection cubemaps
    //  1 for reduced-res
    std::string techniqueName = (m_bFullsize) ? std::string("UseShadowMap_Full") :
                                                std::string("UseShadowMap_Limited");

    if ( FAILED(m_pEffect->SetTechnique(techniqueName.c_str())) )
    {
        MessageBox( NULL, "Failed to set technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    //  shadowmap texture
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, (m_bFilter)?D3DTEXF_LINEAR : D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, (m_bFilter)?D3DTEXF_LINEAR : D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

    //  indirection texture
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

    //  limited indirection requires an extra FaceSelectionSampler (contains +- 1.0 corresponding to MA face)
    if (!m_bFullsize)
    {
        m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
        m_pd3dDevice->SetTexture( 3, m_pFaceSelectionTexture );
    }
    
    m_pd3dDevice->SetTexture( 0, m_pDepthTexture );
    m_pd3dDevice->SetTexture( 2, m_pIndirectionTexture );

    D3DXVECTOR4 vLightPos4( m_vPosition.x, m_vPosition.y, m_vPosition.z, 0.f );
    D3DXVECTOR4 vLightQ( 0.f, 0.f, 0.f, 0.f );

    //  compute the projection parameters for the shadow map
    vLightQ.y = m_fFar / (m_fFar - m_fNear);
    vLightQ.x = vLightQ.y * m_fNear;

    m_pEffect->SetVector( "cLightPositionWorld", &vLightPos4 );
    m_pEffect->SetVector( "cLightColor_Atten", &m_vLightColor_Atten );
    m_pEffect->SetVector( "cLightQ", &vLightQ );

    return S_OK;
}

//-----------------------------------------------------------------------------
//  NeedsShadowPass
//    computes if the light is inside the view frustum -- if yes, then all passes
//    need to be rendered.
//    if not, then test if the shadow frustum is inside the view frustum for 
//    the current pass

BOOL VSDCTShadowMap::NeedsShadowPass( int Pass, const Frustum *viewFrustum ) const
{
    if (viewFrustum->TestPoint( &m_vPosition ))  // light is inside the view frustum
    {
        return TRUE;
    }

    D3DXMATRIX cubeViewProjection;
    GetViewProjectionMatrix( &cubeViewProjection, Pass );

    Frustum cubeFrustum( &cubeViewProjection );
   
    return cubeFrustum.TestFrustum( viewFrustum );
}


//------------------------------------------------------------------------------
//  AllocateViewport

HRESULT VSDCTShadowMap::AllocateViewport( int Pass )
{
    if (m_bViewportAllocated[Pass])
        return S_OK;

    //  viewport size depends on the LOD.
    //  viewport size could vary per-face; however, this demo does not support that functionality (for simplicity)
    m_FaceViewport[Pass].Height = m_FaceViewport[Pass].Width = (1 << (s_iLogMaxFaceResolution-m_iSelectedLOD));
    m_FaceViewport[Pass].MinZ = 0.f;
    m_FaceViewport[Pass].MaxZ = 1.f;

    //  now, scan through all blocks of the viewport size, compare against previous passes' viewports,
    //  and find an open 2D block 
    for ( DWORD viewX=0; viewX<s_iDepthDimensionX && !m_bViewportAllocated[Pass]; viewX+=m_FaceViewport[Pass].Width )
    {
        for ( DWORD viewY=0; viewY<s_iDepthDimensionY && !m_bViewportAllocated[Pass]; viewY+=m_FaceViewport[Pass].Height )
        {
            bool clearTile = true;  // is the current tile available?
            for (int prevPass=0; prevPass<6 && clearTile; prevPass++)
            {
                if (m_bViewportAllocated[prevPass] && (prevPass!=Pass))
                {
                    if ( (viewY < m_FaceViewport[prevPass].Y+m_FaceViewport[prevPass].Height) &&
                         (viewX < m_FaceViewport[prevPass].X+m_FaceViewport[prevPass].Width) &&
                         (viewY+m_FaceViewport[Pass].Height > m_FaceViewport[prevPass].Y) &&
                         (viewX+m_FaceViewport[Pass].Height > m_FaceViewport[prevPass].X) )
                         clearTile = false;
                }
            }
            if (clearTile)
            {
                m_bViewportAllocated[Pass]=TRUE;
                m_FaceViewport[Pass].X = viewX;
                m_FaceViewport[Pass].Y = viewY;
            }
        }
    }
    
    if (!m_bViewportAllocated[Pass])
        return E_FAIL;

    D3DLOCKED_RECT data;
    if ( FAILED(m_pIndirectionTextureSysMem->LockRect((D3DCUBEMAP_FACES)Pass, 0, &data, NULL, 0)) )
    {
        MessageBox(NULL, "Unable to lock surface in indirection texture", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return E_FAIL;
    }

    LPBYTE bits = (LPBYTE)data.pBits;

    //  compute the indirection cubemap texels.
    //  the first texel (0.0 coord) should map to 0.5/DepthDimension in the shadow map
    //  the last texel (1.0 coord) should map to (DepthDimension-0.5)/(DepthDimension) in the shadow map
    //  this way, there are no clamping artifacts at the cubemap borders
    for ( int indirectY=0; indirectY<m_iIndirectionSize; indirectY++ )
    {
        unsigned short* texels = (unsigned short*)bits;
        bits += data.Pitch;

        float t = ((float)indirectY/(float)(m_iIndirectionSize-1))*(float)(m_FaceViewport[Pass].Height-1);
        t = (0.5f+t+(float)m_FaceViewport[Pass].Y) / (float)s_iDepthDimensionY;
        unsigned short ucoord[2];
        ucoord[1] = (unsigned short)(t*65535.f+0.5f);

        for ( int indirectX=0; indirectX<m_iIndirectionSize; indirectX++, texels+=2 )
        {
            float s = ((float)indirectX/(float)(m_iIndirectionSize-1))*(float)(m_FaceViewport[Pass].Width-1);
            s = (0.5f+s+(float)m_FaceViewport[Pass].X) / (float)s_iDepthDimensionX;
            ucoord[0] = (unsigned short)(s*65535.f+0.5f);
            texels[0] = ucoord[0];
            texels[1] = ucoord[1];
        }
    }
    m_pIndirectionTextureSysMem->UnlockRect((D3DCUBEMAP_FACES)Pass, 0);

    m_pd3dDevice->UpdateTexture( m_pIndirectionTextureSysMem, m_pIndirectionTexture );
    return S_OK;
}


//-----------------------------------------------------------------------------------------------
//  setup to draw quad with debug texture.
HRESULT VSDCTShadowMap::SetDebugPass()
{
    if (!m_bDebug)
        return E_FAIL;

    m_pd3dDevice->SetTexture( 0, m_pColorTexture );
    m_pd3dDevice->SetTexture( 1, NULL );
    m_pd3dDevice->SetTexture( 2, NULL );
    m_pd3dDevice->SetTexture( 3, NULL );

    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pEffect->SetTechnique( "DebugTechnique" );
    return S_OK;
}

//-------------------------------------------------------------------------------------------------
//   fill the face selection cube with data
HRESULT VSDCTShadowMap::FillFaceSelectionCube()
{
    //  make it small -- 4x4 is sufficient
    const int FaceTextureSize = 4;
    if ( FAILED(m_pd3dDevice->CreateCubeTexture(FaceTextureSize, 1, 0, D3DFMT_Q8W8V8U8,
                                                D3DPOOL_MANAGED, &m_pFaceSelectionTexture, NULL)) )
    {
        MessageBox( NULL, "Unable to create face selection texture", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

#define D3DCOLOR_UVWQ(u,v,w,q) D3DCOLOR_ARGB(q,w,v,u)
    for (int i=0; i<6; i++)
    {
        D3DLOCKED_RECT data;
        if (FAILED(m_pFaceSelectionTexture->LockRect((D3DCUBEMAP_FACES)i, 0, &data, NULL, 0)))
        {
            MessageBox( NULL, "Unable to lock face selection texture", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
            return E_FAIL;
        }

        D3DCOLOR faceColor;
        switch (i)
        {
        case 0: faceColor = D3DCOLOR_UVWQ(127,0,0,0); break;
        case 1: faceColor = D3DCOLOR_UVWQ(-127,0,0,0); break;
        case 2: faceColor = D3DCOLOR_UVWQ(0,127,0,0); break;
        case 3: faceColor = D3DCOLOR_UVWQ(0,-127,0,0); break;
        case 4: faceColor = D3DCOLOR_UVWQ(0,0,127,0); break;
        case 5: faceColor = D3DCOLOR_UVWQ(0,0,-127,0); break;
        }

        unsigned char* bits = (unsigned char*)data.pBits;
        for (int y=0;y<FaceTextureSize;y++, bits+=data.Pitch)
        {
            DWORD* row = (DWORD*)bits;
            for (int x=0; x<FaceTextureSize; x++, row++)
            {
                *row = (DWORD)faceColor;
            }
        }

        m_pFaceSelectionTexture->UnlockRect((D3DCUBEMAP_FACES)i, 0);
    }
#undef D3DCOLOR_UVWQ
    return S_OK;
}