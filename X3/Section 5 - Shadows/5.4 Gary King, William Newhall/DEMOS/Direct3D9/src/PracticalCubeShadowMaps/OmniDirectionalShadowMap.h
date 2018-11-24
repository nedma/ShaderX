//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  OmniDirectionalShadowMap.h
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


#ifndef __OMNIDIRECTIONAL_SHADOW_MAP_H_included_
#define __OMNIDIRECTIONAL_SHADOW_MAP_H_included_

#include <d3d9.h>
#include <d3dx9.h>

//--------------------------------------------------------------------------
//  OmnidirectionalShadowMap -- define an abstract interface to a shadowmap
//  class, which may be implemented using either R32F cubemaps or unrolled
//  depth textures (or even dual-parabaloid maps).
//    Objects have a position (center of projection, world-space) and direction
//    (world-space P-E).
// -------------------------------------------------------------------------
class Frustum;

class OmnidirectionalShadowMap
{
public:
   
    virtual ~OmnidirectionalShadowMap() { }

    virtual int     GetNumPasses() const = 0;
    //  these return the matrices for the active pass
    virtual void    GetViewProjectionMatrix(D3DXMATRIX* viewProjection) const = 0;

    //  determines if a specific pass is required or not
    virtual BOOL    NeedsShadowPass( int Pass, const Frustum* viewFrustum ) const = 0;

    //  sets the device's active pixel & vertex shaders, viewport, and constants to
    //  values that will render into the shadow map
    virtual HRESULT SetShadowPass(int Pass) = 0;

    //  sets the device's active pixel & vertex shaders to lambertian diffuse * shadow shader
    virtual HRESULT SetScreenPass() = 0;
    virtual HRESULT SetDebugPass() = 0;
    virtual HRESULT Begin() = 0;
    virtual HRESULT End() = 0;

    //  selects appropriate "mip" level and viewport transform, based on screen-space area
    virtual HRESULT SetLightSourceBoundingRect( const LPRECT bounds ) = 0;
    
    //  clears the entire shadow map
    virtual HRESULT Clear(DWORD Flags, D3DCOLOR Color, float Z) = 0;
    virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;

    virtual LPCTSTR GetName() const = 0;

    virtual void SetWorldMatrices( const D3DXMATRIX* world, const D3DXMATRIX* worldIT ) = 0;
    virtual void SetViewProjectMatrix( const D3DXMATRIX* viewProject ) = 0;

    void GetPosition         (D3DXVECTOR3 *lightPosition) const { if (lightPosition) *lightPosition = m_vPosition; }

    void SetLightColor       (D3DXVECTOR3 vColor) { m_vLightColor_Atten.x = vColor.x; m_vLightColor_Atten.y = vColor.y; m_vLightColor_Atten.z = vColor.z; }
    void SetLightAttenuation (float fAtten) { m_vLightColor_Atten.w = fAtten; }
    void SetZBias            (float fZBias, float fSlopeScale) { m_fZBias = fZBias; m_fSlopeScale = fSlopeScale; }
    void SetDevice           (LPDIRECT3DDEVICE9 device) { m_pd3dDevice = device; }
    void SetPositions        (const D3DXVECTOR3* LightPosition, const D3DXVECTOR3* EyePosition) { m_vPosition = *LightPosition;  m_vEyePosition = *EyePosition; }
    void SetBounds           (float fNear, float fFar) { m_fNear = fNear; m_fFar = fFar; }
    void FilterShadows       (BOOL Filter) { m_bFilter = Filter; }
    void SetDebugMode        (BOOL bDebug) { m_bDebug = bDebug; }

protected:
    OmnidirectionalShadowMap(): 
        m_pd3dDevice(NULL), 
        m_iActivePass(0), 
        m_vLightColor_Atten( 1.0f, 1.0f, 1.0f, 1.0f ),
        m_vPosition(0.f, 0.f, 0.f), 
        m_vEyePosition(0.f, 0.f, 0.f), 
        m_fNear(0.1f), 
        m_fFar(100.f), 
        m_fZBias(0.f),
        m_fSlopeScale(0.f),
        m_bDebug(FALSE),
        m_bFilter(TRUE) { }
    
    int         m_iActivePass;
    D3DXVECTOR4 m_vLightColor_Atten;
    D3DXVECTOR3 m_vPosition;
    D3DXVECTOR3 m_vEyePosition;
    float       m_fNear;
    float       m_fFar;
    float       m_fZBias;
    float       m_fSlopeScale;
    BOOL        m_bDebug;
    BOOL        m_bFilter;

    LPDIRECT3DDEVICE9 m_pd3dDevice;
};

#endif