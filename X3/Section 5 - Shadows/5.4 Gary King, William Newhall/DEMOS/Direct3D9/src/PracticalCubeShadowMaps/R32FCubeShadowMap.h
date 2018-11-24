//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  R32FCubeShadowMap.h
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

#ifndef __R32F_CUBE_SHADOW_MAP_H_included_
#define __R32F_CUBE_SHADOW_MAP_H_included_

#include "OmniDirectionalShadowMap.h"

class Frustum;

///////////////////////////////////////////////////////////////////////////////////////////////
//  R32FCubeShadowMap
//    implements OmnidirectionalShadowMap using a R32F floating-point cubemap, and 1
//    Z buffer (shared between faces).
//
//    faces must be completely rendered before switching to the next.  every call to 
//    SetShadowPass() will clear the Z buffer

class R32FCubeShadowMap : public OmnidirectionalShadowMap
{
private:
    static const int s_iCubemapLevels = 9;
    static const int s_iCubemapResolution = 1 << s_iCubemapLevels;
    static const int s_iMaxCubemapTexels = 6 * s_iCubemapResolution * s_iCubemapResolution;

public:
    R32FCubeShadowMap(BOOL bRadeon): OmnidirectionalShadowMap(), m_bRadeonLODBiasWorkaround(bRadeon), m_pEffect(NULL), m_fClearZ(1.f), m_pR32FCubemap(NULL), m_pCurrentFace(NULL), m_pZSurface(NULL), m_iSelectedLOD(0) { }
    virtual ~R32FCubeShadowMap();

    virtual int     GetNumPasses() const { return 6; }
    virtual void    GetViewProjectionMatrix(D3DXMATRIX* viewProjection) const { GetViewProjectionMatrix( viewProjection, m_iActivePass ); }

    virtual void SetWorldMatrices( const D3DXMATRIX* world, const D3DXMATRIX* worldIT );
    virtual void SetViewProjectMatrix( const D3DXMATRIX* viewProject );

    virtual BOOL    NeedsShadowPass( int Pass, const Frustum* viewFrustum ) const;
    virtual HRESULT SetShadowPass(int Pass);
    virtual HRESULT SetDebugPass() { return E_FAIL; }
    virtual HRESULT SetScreenPass();

    virtual HRESULT Begin();
    virtual HRESULT End();

    virtual HRESULT SetLightSourceBoundingRect( const LPRECT bounds );

    virtual HRESULT Clear(DWORD Flags, D3DCOLOR Color, float Z);
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();

    virtual LPCTSTR GetName() const { return _T("R32F Cubemap"); }

protected:
    R32FCubeShadowMap(): OmnidirectionalShadowMap(), m_bRadeonLODBiasWorkaround(FALSE), m_pEffect(NULL), m_fClearZ(1.f), m_pR32FCubemap(NULL), m_pCurrentFace(NULL), m_pZSurface(NULL), m_iSelectedLOD(0) { }

    void GetViewProjectionMatrix(D3DXMATRIX* viewProjection, int Pass) const;

    LPDIRECT3DCUBETEXTURE9 m_pR32FCubemap;
    LPDIRECT3DSURFACE9     m_pCurrentFace;
    LPDIRECT3DSURFACE9     m_pZSurface;
    LPD3DXEFFECT           m_pEffect;
    float                  m_fClearZ;
    D3DVIEWPORT9           m_FaceViewport[ s_iCubemapLevels+1 ];
    BOOL                   m_bRadeonLODBiasWorkaround;

    unsigned int           m_iSelectedLOD;
};

#endif