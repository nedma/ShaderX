//============================================================================//
// filename: COverlayQuad.cpp                                                 //
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


#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <d3dx9.h>
#include "DXUtil.h"
#include "COverlayQuad.h"


//////////////////////////////////////////////////////////////////////////////
// Types /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct OverlayVertex
{
    D3DXVECTOR4 m_vecPos;
    D3DXVECTOR2 m_vecTex;

    static const DWORD FVF;

};
const DWORD OverlayVertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

#pragma pack()

COverlayQuad::COverlayQuad(void)
{
   m_pDevice         = NULL;
   m_pibIndices      = NULL;
   m_pvbVertices     = NULL;
   m_pOverlayTexture = NULL;
}

COverlayQuad::~COverlayQuad(void)
{
}

//----------------------------------------------------------------------------
// Intialize non-Direct3D members
//----------------------------------------------------------------------------
HRESULT COverlayQuad::Initialize(float fHeight, float fWidth, float fTop, float fLeft)
{
   m_fHeight = fHeight;
   m_fWidth = fWidth;
   m_fTop  = fTop;
   m_fLeft = fLeft;

   m_uVertices = 4;
   m_uIndices  = 6;

   return S_OK;
}

//----------------------------------------------------------------------------
// Refresh the effect
//----------------------------------------------------------------------------
HRESULT COverlayQuad::RefreshEffect(void)
{
   LPD3DXBUFFER pBufferErrors = NULL;
   LPD3DXEFFECT pDummyEffect  = NULL;
   HRESULT hr;

   hr = D3DXCreateEffectFromFile(m_pDevice, TEXT("effects\\OverlayQuad.fx"), NULL, NULL, 0, NULL, &pDummyEffect, &pBufferErrors );

   if (FAILED (hr))
   {
      OutputDebugString ("OverlayQuad.fx Effect Refresh Error");
      OutputDebugString ( (char *) pBufferErrors->GetBufferPointer());
   }
   else
   {
      SAFE_RELEASE(m_pEffect);

      m_pEffect = pDummyEffect;
   }

   return S_OK;
}


//----------------------------------------------------------------------------
// Stash the device
//----------------------------------------------------------------------------
HRESULT COverlayQuad::OnCreateDevice(IDirect3DDevice9 *pDevice)
{
   m_pDevice = pDevice;
   return S_OK;
}

//----------------------------------------------------------------------------
// Allocate VB, IB and texture
//----------------------------------------------------------------------------
HRESULT COverlayQuad::OnResetDevice(void)
{
   HRESULT hr;
 
   // Create indices
   if (!m_pibIndices)
   {
      WORD *pwIndices;

      if (FAILED (hr = m_pDevice->CreateIndexBuffer(m_uIndices * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pibIndices, NULL)))
      {
         return hr;
      }

      if (FAILED (hr = m_pibIndices->Lock(0, m_uIndices * sizeof(WORD), (void**) &pwIndices, 0)))
      {
         return hr;
      }

      // Two triangles for overlay quad
      pwIndices[0] = 0;
      pwIndices[1] = 1;
      pwIndices[2] = 2;

      pwIndices[3] = 0;
      pwIndices[4] = 2;
      pwIndices[5] = 3;

      m_pibIndices->Unlock();
   }

   // Create vertices
   if (!m_pvbVertices)
   {
      if (FAILED (hr = m_pDevice->CreateVertexBuffer(m_uVertices * sizeof(OverlayVertex), D3DUSAGE_WRITEONLY, OverlayVertex::FVF, D3DPOOL_DEFAULT, &m_pvbVertices, NULL)))
      {
         return hr;
      }

      OverlayVertex *pVertices;

      if (FAILED (hr = m_pvbVertices->Lock(0, m_uVertices * sizeof(OverlayVertex), (void**) &pVertices, 0)))
      {
         return hr;
      }

      FLOAT fBottom = m_fTop  + m_fHeight;
      FLOAT fRight  = m_fLeft + m_fWidth;

      pVertices[0].m_vecPos  = D3DXVECTOR4(m_fLeft, fBottom, 1.0f, 1.0f);
      pVertices[0].m_vecTex  = D3DXVECTOR2(0.0f, 1.0f);

      pVertices[1].m_vecPos  = D3DXVECTOR4(m_fLeft, m_fTop, 1.0f, 1.0f);
      pVertices[1].m_vecTex  = D3DXVECTOR2(0.0f, 0.0f);

      pVertices[2].m_vecPos  = D3DXVECTOR4(fRight, m_fTop, 1.0f, 1.0f);
      pVertices[2].m_vecTex  = D3DXVECTOR2(1.0f, 0.0f);

      pVertices[3].m_vecPos  = D3DXVECTOR4(fRight, fBottom, 1.0f, 1.0f);
      pVertices[3].m_vecTex  = D3DXVECTOR2(1.0f, 1.0f);

      m_pvbVertices->Unlock();
   }


   if (FAILED (hr = D3DXCreateEffectFromFile(m_pDevice, "effects\\OverlayQuad.fx", NULL, NULL, 0, NULL, &m_pEffect, NULL)))
   {
      return hr;
   }

   m_pEffect->OnResetDevice();

   return S_OK;
}



HRESULT COverlayQuad::OnLostDevice(void)
{
   m_pEffect->OnLostDevice();

   SAFE_RELEASE (m_pibIndices);
   SAFE_RELEASE (m_pvbVertices);

   return S_OK;
}


HRESULT COverlayQuad::OnDestroyDevice(void)
{
   SAFE_RELEASE (m_pEffect);

   m_pDevice = NULL;
   return S_OK;
}


HRESULT COverlayQuad::SetClearColor(D3DXVECTOR4 vClearColor)
{
   m_vClearColor = vClearColor;

   return S_OK;
}


HRESULT COverlayQuad::SetOverlayTexture(LPDIRECT3DTEXTURE9 pOverlayTexture)
{
   m_pOverlayTexture = pOverlayTexture;

   return S_OK;
}


HRESULT COverlayQuad::Draw(DWORD dwFlags)
{
   HRESULT hr;
   UINT iPass, cPasses;
   D3DXVECTOR4 Leye;
 
   if (dwFlags & OVERLAY_QUAD_SHOW_TEXTURE)
   {
      m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("show_texture_technique"));

      m_pEffect->SetTexture ("tOverlayTexture", m_pOverlayTexture);
   }
   else if (dwFlags & OVERLAY_QUAD_BLEND_TEXTURE)
   {
      m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("blend_texture_technique"));

      m_pEffect->SetTexture ("tOverlayTexture", m_pOverlayTexture);
   }
   else if (dwFlags & OVERLAY_QUAD_SHADOW_BLUR)
   {
      m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("shadow_blur_technique"));

      m_pEffect->SetTexture ("tOverlayTexture", m_pOverlayTexture);
   }
   else if (dwFlags & OVERLAY_QUAD_COMPOSITE_FOG_BUFFER)
   {
      m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("composite_fog_technique"));

      m_pEffect->SetTexture ("tOverlayTexture", m_pOverlayTexture);
   }
   else
   {
      m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("clear_technique"));

      m_pEffect->SetVector ("vClearColor", &m_vClearColor);
   }


   m_pEffect->Begin(&cPasses, 0);

   for (iPass = 0; iPass < cPasses; iPass++)
   {
      m_pEffect->Pass(iPass);

      if (FAILED (hr = m_pDevice->SetFVF(OverlayVertex::FVF)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbVertices, 0, sizeof(OverlayVertex))))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetIndices(m_pibIndices)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uVertices, 0, 2)))
      {
         return hr;
      }
  
   }

   m_pEffect->End();

   return S_OK;
}
