//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  VSDCT.h
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

#ifndef __VSDCT_SHADOW_MAP_H_included_
#define __VSDCT_SHADOW_MAP_H_included_

#include "OmniDirectionalShadowMap.h"

class Frustum;

///////////////////////////////////////////////////////////////////////////////////////////////
//  VSDCTShadowMap
//    unrolls a shadow cube map into a 2D depth texture, using an indirection texture to
//    efficiently convert cube coordinates to 2D coordinates.

class VSDCTShadowMap : public OmnidirectionalShadowMap
{
private:
    static const int s_iLogMaxFaceResolution = 9;
    static const int s_iLogMinGreyLevels     = 4;
    static const int s_iDerivativeBits       = 8;
    static const int s_iIndirectionSize      = 1 << max(0, s_iLogMaxFaceResolution + min(s_iDerivativeBits,s_iLogMinGreyLevels) - s_iDerivativeBits);
    static const int s_iDepthDimensionX      = 4*(1<<s_iLogMaxFaceResolution);
    static const int s_iDepthDimensionY      = 2*(1<<s_iLogMaxFaceResolution);

public:
    VSDCTShadowMap(BOOL bResize): OmnidirectionalShadowMap(), 
                            m_pEffect(NULL), 
                            m_fClearZ(1.f), 
                            m_pColorTexture(NULL), 
                            m_pDepthTexture(NULL), 
                            m_pDepthSurface(NULL), 
                            m_pColorSurface(NULL), 
                            m_pIndirectionTexture(NULL), 
                            m_bFullsize(!bResize),                        // use a full-size (1<<s_iLogMaxFaceResolution) indirection cubemap, if not resizing
                            m_pIndirectionTextureSysMem(NULL),
                            m_pFaceSelectionTexture(NULL) ,
                            m_iSelectedLOD(s_iLogMaxFaceResolution),
                            m_bReallocateViewports(bResize)
    {
        m_iIndirectionSize =  (m_bFullsize)?(1<<s_iLogMaxFaceResolution) : s_iIndirectionSize;
    }

    virtual ~VSDCTShadowMap();

    virtual int     GetNumPasses() const { return 6; }
    virtual void    GetViewProjectionMatrix(D3DXMATRIX* viewProjection) const { GetViewProjectionMatrix( viewProjection, m_iActivePass ); }

    virtual void SetWorldMatrices( const D3DXMATRIX* world, const D3DXMATRIX* worldIT );
    virtual void SetViewProjectMatrix( const D3DXMATRIX* viewProject );

    virtual BOOL    NeedsShadowPass( int Pass, const Frustum* viewFrustum ) const;
    virtual HRESULT SetShadowPass(int Pass);
    virtual HRESULT SetDebugPass();
    virtual HRESULT SetScreenPass();

    virtual HRESULT Begin();
    virtual HRESULT End();

    virtual HRESULT SetLightSourceBoundingRect( const LPRECT bounds );

    virtual HRESULT Clear(DWORD Flags, D3DCOLOR Color, float Z);
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();

    virtual LPCTSTR GetName() const { return _T("VSDCT"); }

protected:
    VSDCTShadowMap(): OmnidirectionalShadowMap(), 
                            m_pEffect(NULL), 
                            m_fClearZ(1.f), 
                            m_pColorTexture(NULL), 
                            m_pDepthTexture(NULL), 
                            m_pDepthSurface(NULL), 
                            m_pColorSurface(NULL), 
                            m_pIndirectionTexture(NULL), 
                            m_bFullsize(TRUE),
                            m_pIndirectionTextureSysMem(NULL),
                            m_iIndirectionSize(s_iIndirectionSize),
                            m_pFaceSelectionTexture(NULL),
                            m_iSelectedLOD(s_iLogMaxFaceResolution),
                            m_bReallocateViewports(FALSE) { }

    HRESULT AllocateViewport( int Pass );   // allocates a new viewport for cube face Pass from the unallocated
                                            // depth texture

    HRESULT FillFaceSelectionCube();

    void GetViewProjectionMatrix(D3DXMATRIX* viewProjection, int Pass) const;

    LPDIRECT3DTEXTURE9     m_pDepthTexture;
    LPDIRECT3DSURFACE9     m_pDepthSurface;
    LPDIRECT3DSURFACE9     m_pColorSurface;
    LPDIRECT3DTEXTURE9     m_pColorTexture;
    LPDIRECT3DCUBETEXTURE9 m_pIndirectionTextureSysMem;
    LPDIRECT3DCUBETEXTURE9 m_pIndirectionTexture;
    LPDIRECT3DCUBETEXTURE9 m_pFaceSelectionTexture;

    LPD3DXEFFECT           m_pEffect;
    
    float                  m_fClearZ;
    D3DVIEWPORT9           m_FaceViewport[ 6 ];
    BOOL                   m_bViewportAllocated[ 6 ];
    BOOL                   m_bFullsize;
    BOOL                   m_bReallocateViewports;

    int                    m_iIndirectionSize;
    unsigned int           m_iSelectedLOD;
};

#endif