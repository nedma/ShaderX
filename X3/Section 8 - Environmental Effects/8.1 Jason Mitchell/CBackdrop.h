//==================================================================================//
// filename: CBackdrop.h                                                            //
//                                                                                  //
// author:   Jason L. Mitchell                                                      //
//           ATI Research, Inc.                                                     //
//           3D Application Research Group                                          //
//           email: JasonM@ati.com                                                  //
//                                                                                  //
// Description: Simple Backdrop                                                     //
//                                                                                  //
//==================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                              //
//==================================================================================//

#ifndef ATI_DX9_CBACKDROP_H
#define ATI_DX9_CBACKDROP_H


#define BACKDROP_DRAW_DEPTH      (1 << 0)
#define BACKDROP_SCROLLING_NOISE (1 << 1)
#define BACKDROP_SHADOW_MAPPING  (1 << 2)
#define BACKDROP_COOKIE          (1 << 3)


//-----------------------------------------------------------------------------
// Name: CBackdrop
// Desc: Backdrop Class
//-----------------------------------------------------------------------------
class CBackdrop
{
   // For Direct3D rendering
   LPDIRECT3DDEVICE9       m_pDevice;

   // Geometry
   UINT                    m_uVertices;
   UINT                    m_uIndices;
   LPDIRECT3DINDEXBUFFER9  m_pibIndices;
   LPDIRECT3DVERTEXBUFFER9 m_pvbVertices;

   // Pointers to externally-defined cookie, noise and shadow maps
   LPDIRECT3DTEXTURE9      m_pCookie;
   LPDIRECT3DTEXTURE9      m_pScrollingNoise;
   LPDIRECT3DTEXTURE9      m_pShadowMap;

   // Transforms
   D3DXMATRIX              m_matWorldViewProj;
   D3DXMATRIX              m_matWorldView;
   D3DXMATRIX              m_matITWorldView;

   D3DXMATRIX              m_matWorldLight;
   D3DXMATRIX              m_matWorldLightProj;
   D3DXMATRIX              m_matWorldLightProjBias;
   D3DXMATRIX              m_matWorldLightProjScroll1;
   D3DXMATRIX              m_matWorldLightProjScroll2;
   D3DXVECTOR3             m_vSceneLightPos;

   // Light parameters
   D3DXVECTOR4             m_vLightColor;
   FLOAT                   m_fWidth;
   FLOAT                   m_fHeight;

   // Textures
   LPDIRECT3DTEXTURE9      m_pTexture;

   // Effects/Shaders
   LPD3DXEFFECT            m_pEffect;        // Full-precision effect

public:
   CBackdrop(void);
  ~CBackdrop(void);

   HRESULT Initialize(void);

   HRESULT RefreshEffect(void);

   HRESULT OnCreateDevice(IDirect3DDevice9 *pDevice);
   HRESULT OnResetDevice(void);
   HRESULT OnLostDevice(void);
   HRESULT OnDestroyDevice(void);

   HRESULT SetITWorldViewMatrix(D3DXMATRIX   *matITWorldView);
   HRESULT SetWorldViewProjMatrix(D3DXMATRIX *matWorldViewProj);
   HRESULT SetWorldViewMatrix(D3DXMATRIX     *matWorldView);

   HRESULT SetWorldLightMatrix(D3DXMATRIX *matWorldLight);
   HRESULT SetWorldLightProjMatrix(D3DXMATRIX *matWorldLightProj);
   HRESULT SetWorldLightProjBiasMatrix(D3DXMATRIX *matWorldLightProjBias);
   HRESULT SetWorldLightProjScrollMatrices(D3DXMATRIX *matWorldLightProjScroll1, D3DXMATRIX *matWorldLightProjScroll2);

   HRESULT SetSceneLightPos(D3DXVECTOR3 vSceneLightPos);

   HRESULT SetTextureHandles(LPDIRECT3DTEXTURE9 pCookie, LPDIRECT3DTEXTURE9 pScrollingNoise, LPDIRECT3DTEXTURE9 pShadowMap);
   HRESULT SetLightParameters(D3DXVECTOR4 vLightColor, float fWidth, float fHeight);

   // Drawing routine
   HRESULT Draw(DWORD dwFlags);
};

#endif // ATI_DX9_CBACKDROP_H
