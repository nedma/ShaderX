//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  PracticalCubeShadowMaps.h
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

#ifndef __PRACTICAL_CUBESHADOWMAPS_H_included_
#define __PRACTICAL_CUBESHADOWMAPS_H_included_

#include <DX9SDKSampleFramework/DX9SDKSampleFramework.h>

#include <vector>
#include "Bounding.h"


//  forward class declarations
class NVBScene;
class OmnidirectionalShadowMap;
class Frustum;


///////////////////////////////////////////////////////////////////
//  ScreenVertex & ScreenMesh primarily used for storing simple geometry (e.g., full screen quads)
struct ScreenVertex
{
    float x, y, z;
};

struct ScreenMesh
{
    LPDIRECT3DVERTEXBUFFER9 pVB;
    LPDIRECT3DINDEXBUFFER9  pIB;
    LPDIRECT3DVERTEXDECLARATION9 pVD;
    DWORD dwNumVerts;
    DWORD dwNumFaces;
    DWORD dwFVF;

    ScreenMesh(): pVB(NULL), pIB(NULL), dwNumVerts(0), dwNumFaces(0), pVD(NULL), dwFVF(D3DFVF_XYZ) { }
    ScreenMesh(LPDIRECT3DDEVICE9);
    ~ScreenMesh();
};

///////////////////////////////////////////////////////////////////////////////////
//  CLASS CPRACTICALCUBESHADOWMAPS
//    Main application class
//////////////////////////////////////////////////////////////////////////////////
class CPracticalCubeShadowMaps : public CD3DApplication
{
public:
    LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    CPracticalCubeShadowMaps();
    virtual ~CPracticalCubeShadowMaps();

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice(D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT);

    HRESULT RenderText();

    HRESULT CheckResourceFormatSupport(D3DFORMAT fmt, D3DRESOURCETYPE resType, DWORD dwUsage, D3DFORMAT adapterFormat, D3DCAPS9* pCaps);

private:

    HRESULT CreateShadowMap( );
    HRESULT DrawAllObjects( INT sampler, const Frustum* frustum, BOOL createShadowFrustum, const Frustum* viewerFrustum );
    void HandleKey( DWORD wParam, BOOL bIsVirtKey );
    void ComputeLightSourceBounds();                   // computes the bounding rectangle of the light source

    BOOL                      m_bLoadingApp;           // TRUE, if the app is loading
    BOOL                      m_bNeedsLight;
    BOOL                      m_bQueryOut;             // outstanding query
    RECT                      m_ScissorRect;
    ID3DXFont*                m_pFont;                 // D3DX font

    struct  // a structure for storing sanitized hardware capabilities
    {
        BOOL m_bIsRadeon;
        BOOL m_bSupportsR32F;           // supports 32-bit floating-point cube maps
        BOOL m_bSupportsDepthTexture;   // supports depth shadow maps
        BOOL m_bSupportsEventQuery;     // supports DX9 event query
    } m_caps;

    enum {
        SHADOW_R32FCUBE,
        SHADOW_VSDCT
    };

    struct  // a structure for storing user-tweakable settings
    {
        unsigned int m_ShadowTechnique;         //  type of shadow map to render
        float        m_fIntensity;              //  light intensity
        float        m_fZBias;                  //  constant Z bias
        float        m_fSlopeScale;             //  slope-scale depth bias (depth textures only)
        BOOL         m_bShowHelp;               //  show help text
        BOOL         m_bShowStats;              //  report culling & fps stats
        BOOL         m_bCullFromCasterFrustum;  //  use fancy shadow caster culling?
        BOOL         m_bAnimateLight;           //  animate the light source?
        BOOL         m_bDrawScissor;            //  display the scissor rectangle?
        BOOL         m_bFilter;                 //  filter the shadow comparison?
        BOOL         m_bDebug;                  //  display debug view (only works in depth texture mode)?
        BOOL         m_bDynamicSizing;          //  allow dynamic resizing of VSDCT?
    } m_tweakables;

    struct  // a structure for storing various per-frame performance stats
    {
        int shadowPassesRendered;
        int shadowCasters;
        int shadowCastersCulled;
        int totalObjects;
        int totalObjectsCulled;
    } m_stats;

    MouseFPSUI*               m_pMouseUI;             // mouse UI
    ScreenMesh*               m_pScreenQuad;          // screen quad
    NVBScene*                 m_pScene;               // the scene
    OmnidirectionalShadowMap* m_pShadowMap;           // the shadow map (either R32F or depth)

    D3DXMATRIX                m_mProjection;          // projection matrix
    D3DXMATRIX                m_mView;                // view matrix

    LPD3DXEFFECT              m_pDepthFirstEffect;
    LPDIRECT3DVERTEXDECLARATION9 m_pDeclaration;
    LPDIRECT3DSURFACE9        m_pBackBuffer;
    LPDIRECT3DSURFACE9        m_pZBuffer;
    LPDIRECT3DQUERY9          m_pLimitQueuedFrames;   // evens out framerate...

    D3DVIEWPORT9              m_viewport;


    std::vector< BoundingBox > m_modelBoxen;
};

#endif