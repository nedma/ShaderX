//============================================================================//
// filename: COverlayQuad.h                                                   //
//                                                                            //
// author:   Jason L. Mitchell                                                //
//           ATI Research, Inc.                                               //
//           3D Application Research Group                                    //
//           email: JasonM@ati.com                                            //
//                                                                            //
// Description: Simple quad overlay for displaying textures                   //
//                                                                            //
//============================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                        //
//============================================================================//

#ifndef ATI_DX9_COVERLAY_QUAD_H
#define ATI_DX9_COVERLAY_QUAD_H

#define OVERLAY_QUAD_SHOW_TEXTURE         (1 << 0)
#define OVERLAY_QUAD_BLEND_TEXTURE        (1 << 1)
#define OVERLAY_QUAD_SHADOW_BLUR          (1 << 2)
#define OVERLAY_QUAD_COMPOSITE_FOG_BUFFER (1 << 3)



//-----------------------------------------------------------------------------
// Name: COverlayQuad
// Desc: Ground plane Class
//-----------------------------------------------------------------------------
class COverlayQuad
{
   // For Direct3D rendering
   LPDIRECT3DDEVICE9       m_pDevice;

   // Geometry
   FLOAT                   m_fHeight;
   FLOAT                   m_fWidth;
   FLOAT                   m_fTop;
   FLOAT                   m_fLeft;
   UINT                    m_uVertices;
   UINT                    m_uIndices;
   LPDIRECT3DINDEXBUFFER9  m_pibIndices;
   LPDIRECT3DVERTEXBUFFER9 m_pvbVertices;

   // Clear color for using this quad to clear high-precision textures
   D3DXVECTOR4             m_vClearColor;

   // Pointer to externally-defined textures
   LPDIRECT3DTEXTURE9      m_pOverlayTexture;

   // Effects/Shaders
   LPD3DXEFFECT            m_pEffect;

public:
   COverlayQuad(void);
  ~COverlayQuad(void);

   HRESULT Initialize(float fHeight, float fWidth, float fTop, float fLeft);

   HRESULT RefreshEffect(void);

   HRESULT OnCreateDevice(IDirect3DDevice9 *pDevice);
   HRESULT OnResetDevice(void);
   HRESULT OnLostDevice(void);
   HRESULT OnDestroyDevice(void);

   HRESULT SetOverlayTexture(LPDIRECT3DTEXTURE9 pOverlayTexture);
   HRESULT SetClearColor(D3DXVECTOR4 vClearColor);

   // Drawing routines
   HRESULT Draw(DWORD dwFlags);
};

#endif // ATI_DX9_COVERLAY_QUAD_H
