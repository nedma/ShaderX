//============================================================================//
// filename: CVolVizShells.h                                                  //
//                                                                            //
// author:   Jason L. Mitchell                                                //
//           ATI Research, Inc.                                               //
//           3D Application Research Group                                    //
//           email: JasonM@ati.com                                            //
//                                                                            //
// Description: Shells for visualizing Light Shafts                           //
//                                                                            //
//============================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                        //
//============================================================================//

#ifndef ATI_DX9_CVOLVIZSHELLS_H
#define ATI_DX9_CVOLVIZSHELLS_H


#define VOLVIZ_SHELLS_WIREFRAME (1 << 0)
#define VOLVIZ_SHELLS_CLIP      (1 << 1)
#define VOLVIZ_SCROLLING_NOISE  (1 << 2)
#define VOLVIZ_SHADOW_MAPPING   (1 << 3)
#define VOLVIZ_COOKIE           (1 << 4)

//----------------------------------------------------------------------------------
// Name: CVolVizShells
// Desc: Class for intelligently rendering volviz shells for light shaft rendering
//----------------------------------------------------------------------------------
class CVolVizShells
{
   // For Direct3D rendering
   IDirect3DDevice9       *m_pDevice;

   // Geometry
   UINT                    m_uVertices;
   UINT                    m_uIndices;
   IDirect3DIndexBuffer9  *m_pibIndices;
   IDirect3DVertexBuffer9 *m_pvbVertices;

   int                     m_numShells;
   int                     m_numRows;
   int                     m_numColumns;
   int                     m_numTris;
   int                     m_numShellsToDraw;
   int                     m_numTrisToDraw;

   D3DXVECTOR4             m_vWireColor;
   D3DXVECTOR4             m_vMinBounds;
   D3DXVECTOR4             m_vMaxBounds;
   FLOAT                   m_fSamplingDelta;

   // Light parameters
   D3DXVECTOR4             m_vLightColor;
   FLOAT                   m_fWidth;
   FLOAT                   m_fHeight;
   FLOAT                   m_fFarPlane;

   D3DXMATRIX              m_matInvView;
   D3DXMATRIX              m_matViewWorldLight;
   D3DXMATRIX              m_matViewWorldLightProj;
   D3DXMATRIX              m_matViewWorldLightProjBias;
   D3DXMATRIX              m_matViewWorldLightProjScroll1;
   D3DXMATRIX              m_matViewWorldLightProjScroll2;

   D3DXPLANE               m_ClipSpaceLightFrustumPlanes[6];

   // Textures
   LPDIRECT3DTEXTURE9      m_pCookie;
   LPDIRECT3DTEXTURE9      m_pScrollingNoise;
   LPDIRECT3DTEXTURE9      m_pShadowMap;  // Externally defined

   // Effects/Shaders
   LPD3DXEFFECT            m_pEffect;

public:
   CVolVizShells(void);
  ~CVolVizShells(void);

   // Utility routine
   DWORD MapShellToColorChannel(int shell);

   HRESULT Initialize(void);

   HRESULT RefreshEffect(void);

   HRESULT OnCreateDevice(IDirect3DDevice9 *pDevice);
   HRESULT OnResetDevice(void);
   HRESULT OnLostDevice(void);
   HRESULT OnDestroyDevice(void);

   // Setters
   HRESULT SetVolVizBounds(D3DXVECTOR4 vMinBounds, D3DXVECTOR4 vMaxBounds, FLOAT fSamplingDelta);
   HRESULT SetClipPlanes(D3DXPLANE *ClipSpaceFrustumPlanes);
   HRESULT SetFrozenInverseViewMatrix(D3DXMATRIX *matInvView);
   HRESULT SetViewWorldLightMatrix(D3DXMATRIX *matViewWorldLight);
   HRESULT SetViewWorldLightProjMatrix(D3DXMATRIX *matWorldLightProj);
   HRESULT SetViewWorldLightProjBiasMatrix(D3DXMATRIX *matWorldLightProjBias);
   HRESULT SetViewWorldLightProjScrollMatrices(D3DXMATRIX *matViewWorldLightProjScroll1, D3DXMATRIX *matViewWorldLightProjScroll2);
   HRESULT SetTextures(LPDIRECT3DTEXTURE9 pCookie, LPDIRECT3DTEXTURE9 pScrollingNoise, LPDIRECT3DTEXTURE9 pShadowMap);
   HRESULT SetLightParameters(D3DXVECTOR4 vLightColor, float fWidth, float fHeight, float fFarPlane);

   // Getters
   INT GetNumShellsDrawn(void);

   // Drawing routines
   HRESULT DrawFrozenShells(void);
   HRESULT DrawWireframeShells(void);
   HRESULT Draw(DWORD dwFlags);
};

#endif // ATI_DX9_CVOLVIZSHELLS_H
