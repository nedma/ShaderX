//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  R32FCubeShadowMap.cpp
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
#include "R32FCubeShadowMap.h"
#include "d3dutil.h"
#include "Bounding.h"

R32FCubeShadowMap::~R32FCubeShadowMap()
{
    InvalidateDeviceObjects();
}

//--------------------------------------------------------------------
//  RestoreDeviceObjects()
//    Creates the render targets used by an R32F cubemap shadow map
//--------------------------------------------------------------------

HRESULT R32FCubeShadowMap::RestoreDeviceObjects()
{
    assert( m_pd3dDevice != NULL );

    //  generate with all MIP maps -- based on screen-space area of light source, we will use
    //  smaller render targets when appropriate
    if (FAILED(m_pd3dDevice->CreateCubeTexture(s_iCubemapResolution, 0, D3DUSAGE_RENDERTARGET, 
                                 D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pR32FCubemap, NULL)))
    {
        MessageBox( NULL, "Unable to create R32F cubemap", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(s_iCubemapResolution, s_iCubemapResolution,
                                 D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pZSurface, NULL)))
    {
        MessageBox( NULL, "Unable to create depth/stencil surface", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    HRESULT hr;

    //  switch between texCUBEbias and texCUBE based on whether or not an ATI GPU is detected
    if ( m_bRadeonLODBiasWorkaround )
        hr = D3DXCreateEffectFromFile(m_pd3dDevice, GetFilePath::GetFilePath("programs\\PracticalCubeShadowMaps\\R32FCubemapRadeon.fx").c_str(), NULL, NULL, 0, NULL, &m_pEffect, NULL);
    else
        hr = D3DXCreateEffectFromFile(m_pd3dDevice, GetFilePath::GetFilePath("programs\\PracticalCubeShadowMaps\\R32FCubemap.fx").c_str(), NULL, NULL, 0, NULL, &m_pEffect, NULL);
    
    if (FAILED(hr))
    {
        ::MessageBox(NULL, "Failed to load R32FCubemap.fx", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_ICONHAND);
        return hr;
    }

    for (int i=0; i<=s_iCubemapLevels; i++)
    {
        DWORD res = 1 << (s_iCubemapLevels-i);
        m_FaceViewport[i].X = m_FaceViewport[i].Y = 0;
        m_FaceViewport[i].Width = m_FaceViewport[i].Height = res;
        m_FaceViewport[i].MinZ = 0.f;
        m_FaceViewport[i].MaxZ = 1.f;
    }

    return S_OK;
}

//--------------------------------------------------------------------
//  InvalidateDeviceObjects()
//    Destroys objects
//--------------------------------------------------------------------

HRESULT R32FCubeShadowMap::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pCurrentFace );
    SAFE_RELEASE( m_pR32FCubemap );
    SAFE_RELEASE( m_pZSurface );
    SAFE_RELEASE( m_pEffect );
    return S_OK;
}

//--------------------------------------------------------------------
//  SetLightSourceBoundingRect()
//    Selects the appropriate mipmap LOD for rendering & shadow test,
//    based on the screen-space area of the light source
//--------------------------------------------------------------------

HRESULT R32FCubeShadowMap::SetLightSourceBoundingRect( const LPRECT bounds )
{
    LONG width  = bounds->right  - bounds->left; width = min( 8192, width );
    LONG height = bounds->bottom - bounds->top;  height = min( 8192, height );
    LONG numPixels = min(s_iMaxCubemapTexels, width*height);
    LONG mipLevel = 0;

    while ( (6*(1<<mipLevel)*(1<<mipLevel)) < 2*numPixels )  // sample shadow structure the frequency we will display it
        mipLevel++;                        // only true for uniform sampling from view, but this works reasonably well

    mipLevel = s_iCubemapLevels - mipLevel;     // L0 is finest in mip pyramid, not N
    mipLevel = (mipLevel<0) ? 0 : mipLevel;     // clamp to finest LOD (shadow will be magnified, if this happens)
    m_iSelectedLOD = mipLevel;
    return S_OK;
}

//--------------------------------------------------------------------
//  Clear()
//    Clears the Z and color buffers
//    doesn't preserve previous state
//--------------------------------------------------------------------

HRESULT R32FCubeShadowMap::Clear(DWORD Flags, D3DCOLOR Color, float Z)
{
    assert( m_pd3dDevice != NULL );
    assert( m_pZSurface != NULL );
    assert( m_pR32FCubemap != NULL );

    m_fClearZ = Z;

    SAFE_RELEASE( m_pCurrentFace );

    //  set new viewport & depth/stencil surface
    m_pd3dDevice->SetViewport(&m_FaceViewport[m_iSelectedLOD]);
    m_pd3dDevice->SetDepthStencilSurface(m_pZSurface);
    for (int i=0; (i<6) && (Flags); i++)
    {
        LPDIRECT3DSURFACE9 cubeFace;
        m_pR32FCubemap->GetCubeMapSurface((D3DCUBEMAP_FACES)i, m_iSelectedLOD, &cubeFace);
        m_pd3dDevice->SetRenderTarget(0, cubeFace);
        m_pd3dDevice->Clear(0, NULL, Flags, Color, m_fClearZ, 0);
        //  only clear Z/stencil on first pass
        Flags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
        SAFE_RELEASE(cubeFace);
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//  GetViewProjectionMatrix
//    Returns the world->cubemap clip space matrix for the active pass(face)
//    This is to allow an application to perform frustum culling with world-space
//    bounding boxes/spheres

void R32FCubeShadowMap::GetViewProjectionMatrix(D3DXMATRIX *viewProjection, int Pass) const
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

void R32FCubeShadowMap::SetWorldMatrices( const D3DXMATRIX *world, const D3DXMATRIX *worldIT )
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

void R32FCubeShadowMap::SetViewProjectMatrix( const D3DXMATRIX *viewProject )
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
HRESULT R32FCubeShadowMap::Begin()
{
    UINT uPasses;

    if (!FAILED(m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE)))
    {
        m_pEffect->BeginPass(0);
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
HRESULT R32FCubeShadowMap::End()
{
	m_pEffect->EndPass();
    m_pEffect->End();
    return S_OK;
}

//-----------------------------------------------------------------------------
//  SetShadowPass
//    Sets the active technique and render target to one appropriate for the
//    shadowmap face we want to render into

HRESULT R32FCubeShadowMap::SetShadowPass( int Pass )
{
    if ( Pass<0 || Pass>5 )
        return E_FAIL;

    if ( FAILED(m_pEffect->SetTechnique("GenShadowMap")) )
    {
        MessageBox( NULL, "Failed to set technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    SAFE_RELEASE( m_pCurrentFace );

    if ( !m_pR32FCubemap || !m_pZSurface )
        return E_FAIL;

    m_pR32FCubemap->GetCubeMapSurface( (D3DCUBEMAP_FACES)Pass, m_iSelectedLOD, &m_pCurrentFace );

    HRESULT hr = m_pd3dDevice->SetRenderTarget( 0, m_pCurrentFace );
    if (SUCCEEDED(hr))
        hr = m_pd3dDevice->SetDepthStencilSurface( m_pZSurface );

    if (FAILED(hr))
    {
        MessageBox( NULL, "Unable to set render target to cubemap", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return hr;
    }

    m_pd3dDevice->SetViewport(&m_FaceViewport[m_iSelectedLOD]);
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0x00000000, m_fClearZ, 0 );

    D3DXMATRIX cubeView, cubeProject, cubeViewProjection;

    m_iActivePass = Pass;

    cubeView = D3DUtil_GetCubeMapViewMatrix( (DWORD)Pass );
    D3DXMatrixPerspectiveFovLH( &cubeProject, D3DXToRadian(90.f), 1.f, m_fNear, m_fFar );

    D3DXMatrixMultiply( &cubeViewProjection, &cubeView, &cubeProject );

    m_pEffect->SetMatrix( "cCubemapViewProjection", &cubeViewProjection );

    D3DXVECTOR4 vLightPos4( m_vPosition.x, m_vPosition.y, m_vPosition.z, 0.f );
    m_pEffect->SetVector( "cLightPositionWorld", &vLightPos4 );
    m_pEffect->SetFloat( "cZBias", m_fZBias );
    
    return S_OK;
}

//-----------------------------------------------------------------------------
//  SetScreenPass
//    Sets the active technique to the appropriate one for the screen, and
//    sets any render states necessary for this

HRESULT R32FCubeShadowMap::SetScreenPass()
{

    std::string techniqueName = (m_bFilter) ? std::string( "UseShadowMap_Filtered" ) :
                                              std::string( "UseShadowMap_Unfiltered" );

    if ( FAILED(m_pEffect->SetTechnique(techniqueName.c_str())) )
    {
        MessageBox( NULL, "Failed to set technique in effect file", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
        return E_FAIL;
    }

    SAFE_RELEASE( m_pCurrentFace );

    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, m_iSelectedLOD );
    //  for some reason, Radeons don't respect this sampler state, so we need to use texCUBEbias in the shader
    if ( !m_bRadeonLODBiasWorkaround )
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS, (DWORD)(-s_iCubemapLevels) );
    else
        m_pEffect->SetFloat( "cLODBias", (FLOAT)(-s_iCubemapLevels) );
    
    m_pd3dDevice->SetTexture( 0, m_pR32FCubemap );

    D3DXVECTOR4 vLightPos4( m_vPosition.x, m_vPosition.y, m_vPosition.z, 0.f );
    m_pEffect->SetVector( "cLightPositionWorld", &vLightPos4 );

    float fRes = (float) (1 << (s_iCubemapLevels - m_iSelectedLOD));

    D3DXVECTOR4 vShadowResolution( fRes, 1.f/fRes, 0.f, 0.f );

    m_pEffect->SetVector( "cShadowResolution", &vShadowResolution );
    m_pEffect->SetVector( "cLightColor_Atten", &m_vLightColor_Atten );   
  
    return S_OK;
}

//-----------------------------------------------------------------------------
//  NeedsShadowPass
//    computes if the light is inside the view frustum -- if yes, then all passes
//    need to be rendered.
//    if not, then test if the shadow frustum is inside the view frustum for 
//    the current pass

BOOL R32FCubeShadowMap::NeedsShadowPass( int Pass, const Frustum *viewFrustum ) const
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