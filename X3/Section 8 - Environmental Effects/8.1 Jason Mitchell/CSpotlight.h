//============================================================================//
// filename: CSpotlight.h                                                     //
//                                                                            //
// author:   Jason L. Mitchell                                                //
//           ATI Research, Inc.                                               //
//           3D Application Research Group                                    //
//           email: JasonM@ati.com                                            //
//                                                                            //
// Description: Spotlight Class for Light Shaft rendering                     //
//                                                                            //
//============================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                        //
//============================================================================//

#ifndef ATI_DX9_CSPOTLIGHT_H
#define ATI_DX9_CSPOTLIGHT_H


#define SPOTLIGHT_SHOW_FRUSTUM            (1 << 0)
#define SPOTLIGHT_SHOW_LIGHT              (1 << 1)
#define SPOTLIGHT_SHOW_CLIPPING_FRUSTUM   (1 << 2)


// Controls density of wireframe spotlight
#define NUM_SUPER_ELLIPSE_POINTS       192
#define NUM_SUPER_ELLIPSE_CONNECTORS     8


#define NEAR_COLOR         0xFFFF4444
#define FAR_COLOR          0xFF4444FF
#define CLIP_VOLUME_COLOR  0xFFF68E07



//-----------------------------------------------------------------------------
// Name: CSpotlight
// Desc: Spotlight Class
//-----------------------------------------------------------------------------
class CSpotlight
{

   // Specification of the spotlight frustum
   D3DXVECTOR3    m_vEyePt;
   D3DXVECTOR3    m_vLookAtPt;
   D3DXVECTOR3    m_vUpVec;
   FLOAT          m_fFOV;
   FLOAT          m_fAspect;

   D3DXVECTOR3    m_FrustumVerts[8]; // Light space positions of frustum vertices

   D3DXPLANE      m_FrustumPlanes[6];
   BOOL           m_bFarPlaneIsGroundPlane;

   D3DXVECTOR3    m_vSceneLightPos;
   D3DXVECTOR4    m_vLightColor;

   // Spotlight parameters
   FLOAT          m_fWidth;
   FLOAT          m_fHeight;

   FLOAT          m_fNearWidth;
   FLOAT          m_fNearHeight;
   FLOAT          m_fVerticalFOV;
   FLOAT          m_fFarWidth;
   FLOAT          m_fFarHeight;

   // For Direct3D rendering
   IDirect3DDevice9       *m_pDevice;

   // Light source
   UINT                    m_uFrustumIndices;
   UINT                    m_uFrustumVertices;
   LPDIRECT3DINDEXBUFFER9  m_pibFrustumIndices;
   LPDIRECT3DVERTEXBUFFER9 m_pvbFrustumVertices;

   // Wireframe
   UINT                    m_uWireFrustumIndices;
   UINT                    m_uWireFrustumVertices;
   LPDIRECT3DINDEXBUFFER9  m_pibWireFrustumIndices;
   LPDIRECT3DVERTEXBUFFER9 m_pvbWireFrustumVertices;

   // Transforms
   D3DXMATRIX              m_matView;
   D3DXMATRIX              m_matWorld;
   D3DXMATRIX              m_matProjection;

   // Textures and Surfaces
   LPDIRECT3DTEXTURE9     m_pCookie;            // RGBA

   // Effects/Shaders
   LPD3DXEFFECT           m_pFrustumEffect;
   UINT                   m_iTechnique;

   HRESULT RegenerateWireFrustum(void);

public:
   CSpotlight(void);
  ~CSpotlight(void);

   HRESULT Initialize(void);
   HRESULT OnCreateDevice(IDirect3DDevice9 *pDevice);
   HRESULT OnResetDevice(void);
   HRESULT OnLostDevice(void);
   HRESULT OnDestroyDevice(void);
   HRESULT Update(D3DXVECTOR3 &vecPos, D3DXVECTOR3 &vecLight, BOOL bCalcCaustics);
   HRESULT RefreshEffect(void);

   // Setters
   HRESULT SetView(D3DXVECTOR3 vEyePt, D3DXVECTOR3 vLookAtPt, D3DXVECTOR3 vUpVec, FLOAT fWidth, FLOAT fHeight, FLOAT fZNear, FLOAT fZFar);
   HRESULT SetSceneLightPos(D3DXVECTOR3 vSceneLightPos);
   HRESULT SetCookie(LPDIRECT3DTEXTURE9 pCookie);
   HRESULT SetGroundPlaneClip(BOOL bFarPlaneIsGroundPlane);
   HRESULT SetLightColor(D3DXVECTOR4 vLightColor);
   HRESULT SetWidth(FLOAT fWidth);
   HRESULT SetHeight(FLOAT fHeight);

   // Getters
   D3DXMATRIX *GetViewMatrix(void);
   D3DXMATRIX *GetProjectionMatrix(void);
   HRESULT     GetViewSpaceBounds(D3DXVECTOR4 *vMinBounds, D3DXVECTOR4 *vMaxBounds);
   HRESULT     GetWorldSpaceFrustumPlanes(D3DXPLANE *WorldSpaceFrustumPlanes);
   LPDIRECT3DTEXTURE9 GetCookie(void);
   D3DXVECTOR4 GetLightColor(void);
   FLOAT       GetWidth(void);
   FLOAT       GetHeight(void);

   // Drawing routines
   HRESULT Draw(DWORD dwFlags);
};


#endif // ATI_DX9_CSPOTLIGHT_H
