//=====================================================================================//
// filename: CBackdrop.cpp                                                             //
//                                                                                     //
// author:   Jason L. Mitchell                                                         //
//           ATI Research, Inc.                                                        //
//           3D Application Research Group                                             //
//           email: JasonM@ati.com                                                     //
//                                                                                     //
// Description: Simple Backdrop                                                        //
//                                                                                     //
//=====================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                 //
//=====================================================================================//


#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <d3dx9.h>
#include "DXUtil.h"
#include "CBackdrop.h"

#define FLOOR_TINT               0x007DD5FA
#define BACK_WALL_TINT           0x00F39B9B
#define LEFT_WALL_TINT           0x0095FCB0

#define ROOM_SIZE                    200.0f
#define ROOM_TEXTURE_REPEAT_RATE       2.0f


//////////////////////////////////////////////////////////////////////////////
// Types /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct BackdropVertex
{
    D3DXVECTOR3 m_vecPos;
    D3DXVECTOR3 m_vecNorm;
    DWORD       m_dwColor;
    D3DXVECTOR2 m_vecTex;

    static const DWORD FVF;
};
const DWORD BackdropVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;

#pragma pack()

CBackdrop::CBackdrop(void)
{
   m_pDevice        = NULL;
   m_pibIndices     = NULL;
   m_pvbVertices    = NULL;
   m_pTexture       = NULL;
}

CBackdrop::~CBackdrop(void)
{
}

//----------------------------------------------------------------------------
// Intialize non-Direct3D members
//----------------------------------------------------------------------------
HRESULT CBackdrop::Initialize(void)
{
   m_uVertices = 12;
   m_uIndices  = 18;
   return S_OK;
}

//----------------------------------------------------------------------------
// Refresh the effect
//----------------------------------------------------------------------------
HRESULT CBackdrop::RefreshEffect(void)
{
   LPD3DXBUFFER pBufferErrors = NULL;
   LPD3DXEFFECT pDummyEffect  = NULL;
   HRESULT hr;

   hr = D3DXCreateEffectFromFile(m_pDevice, TEXT("effects\\Spotlight_Backdrop.fx"), NULL, NULL, 0, NULL, &pDummyEffect, &pBufferErrors );

   if (FAILED (hr))
   {
      OutputDebugString ("effects\\Spotlight_Backdrop.fx Effect Refresh Error");
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
HRESULT CBackdrop::OnCreateDevice(IDirect3DDevice9 *pDevice)
{
   m_pDevice = pDevice;
   return S_OK;
}

//----------------------------------------------------------------------------
// Allocate VB, IB and texture
//----------------------------------------------------------------------------
HRESULT CBackdrop::OnResetDevice(void)
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

      // Six triangles for backdrop
      pwIndices[0]  = 0; pwIndices[1]  = 1;  pwIndices[2]  = 2;
      pwIndices[3]  = 0; pwIndices[4]  = 2;  pwIndices[5]  = 3;

      pwIndices[6]  = 4; pwIndices[7]  = 5;  pwIndices[8]  = 6;
      pwIndices[9]  = 4; pwIndices[10] = 6;  pwIndices[11] = 7;

      pwIndices[12] = 8; pwIndices[13] = 9;  pwIndices[14] = 10;
      pwIndices[15] = 8; pwIndices[16] = 10; pwIndices[17] = 11;

      m_pibIndices->Unlock();
   }

    
   // Create vertices
   if (!m_pvbVertices)
   {
      if (FAILED (hr = m_pDevice->CreateVertexBuffer(m_uVertices * sizeof(BackdropVertex), D3DUSAGE_WRITEONLY, BackdropVertex::FVF, D3DPOOL_DEFAULT, &m_pvbVertices, NULL)))
      {
         return hr;
      }


      BackdropVertex *pVertices;

      if (FAILED (hr = m_pvbVertices->Lock(0, m_uVertices * sizeof(BackdropVertex), (void**) &pVertices, 0)))
      {
         return hr;
      }

      // Floor
      pVertices[0].m_vecPos  = D3DXVECTOR3(-ROOM_SIZE, 0.0f, -ROOM_SIZE);
      pVertices[0].m_vecNorm = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
      pVertices[0].m_vecTex  = D3DXVECTOR2(-ROOM_TEXTURE_REPEAT_RATE, -ROOM_TEXTURE_REPEAT_RATE);
      pVertices[0].m_dwColor = FLOOR_TINT;

      pVertices[1].m_vecPos  = D3DXVECTOR3(-ROOM_SIZE, 0.0f, ROOM_SIZE);
      pVertices[1].m_vecNorm = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
      pVertices[1].m_vecTex  = D3DXVECTOR2(-ROOM_TEXTURE_REPEAT_RATE, ROOM_TEXTURE_REPEAT_RATE);
      pVertices[1].m_dwColor = FLOOR_TINT;

      pVertices[2].m_vecPos  = D3DXVECTOR3(ROOM_SIZE, 0.0f, ROOM_SIZE);
      pVertices[2].m_vecNorm = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
      pVertices[2].m_vecTex  = D3DXVECTOR2(ROOM_TEXTURE_REPEAT_RATE, ROOM_TEXTURE_REPEAT_RATE);
      pVertices[2].m_dwColor = FLOOR_TINT;

      pVertices[3].m_vecPos  = D3DXVECTOR3(ROOM_SIZE, 0.0f, -ROOM_SIZE);
      pVertices[3].m_vecNorm = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
      pVertices[3].m_vecTex  = D3DXVECTOR2(ROOM_TEXTURE_REPEAT_RATE, -ROOM_TEXTURE_REPEAT_RATE);
      pVertices[3].m_dwColor = FLOOR_TINT;

      // +z wall (back wall)
      pVertices[4].m_vecPos  = D3DXVECTOR3(-ROOM_SIZE,  0.0f,  ROOM_SIZE);
      pVertices[4].m_vecNorm = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
      pVertices[4].m_vecTex  = D3DXVECTOR2(-ROOM_TEXTURE_REPEAT_RATE, -ROOM_TEXTURE_REPEAT_RATE);
      pVertices[4].m_dwColor = BACK_WALL_TINT;

      pVertices[5].m_vecPos  = D3DXVECTOR3(-ROOM_SIZE,  ROOM_SIZE,  ROOM_SIZE);
      pVertices[5].m_vecNorm = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
      pVertices[5].m_vecTex  = D3DXVECTOR2(-ROOM_TEXTURE_REPEAT_RATE,  ROOM_TEXTURE_REPEAT_RATE);
      pVertices[5].m_dwColor = BACK_WALL_TINT;

      pVertices[6].m_vecPos  = D3DXVECTOR3( ROOM_SIZE,  ROOM_SIZE,  ROOM_SIZE);
      pVertices[6].m_vecNorm = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
      pVertices[6].m_vecTex  = D3DXVECTOR2( ROOM_TEXTURE_REPEAT_RATE,  ROOM_TEXTURE_REPEAT_RATE);
      pVertices[6].m_dwColor = BACK_WALL_TINT;

      pVertices[7].m_vecPos  = D3DXVECTOR3( ROOM_SIZE,  0.0f,  ROOM_SIZE);
      pVertices[7].m_vecNorm = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
      pVertices[7].m_vecTex  = D3DXVECTOR2( ROOM_TEXTURE_REPEAT_RATE, -ROOM_TEXTURE_REPEAT_RATE);
      pVertices[7].m_dwColor = BACK_WALL_TINT;

      // -x wall (left wall)
      pVertices[8].m_vecPos   = D3DXVECTOR3(-ROOM_SIZE,  0.0f, -ROOM_SIZE);
      pVertices[8].m_vecNorm  = D3DXVECTOR3( 1.0f,  0.0f,  0.0f);
      pVertices[8].m_vecTex   = D3DXVECTOR2(-ROOM_TEXTURE_REPEAT_RATE, -ROOM_TEXTURE_REPEAT_RATE);
      pVertices[8].m_dwColor  = LEFT_WALL_TINT;

      pVertices[9].m_vecPos   = D3DXVECTOR3(-ROOM_SIZE,  ROOM_SIZE, -ROOM_SIZE);
      pVertices[9].m_vecNorm  = D3DXVECTOR3( 1.0f,  0.0f,  0.0f);
      pVertices[9].m_vecTex   = D3DXVECTOR2(-ROOM_TEXTURE_REPEAT_RATE,  ROOM_TEXTURE_REPEAT_RATE);
      pVertices[9].m_dwColor  = LEFT_WALL_TINT;

      pVertices[10].m_vecPos  = D3DXVECTOR3(-ROOM_SIZE,  ROOM_SIZE,  ROOM_SIZE);
      pVertices[10].m_vecNorm = D3DXVECTOR3( 1.0f,  0.0f,  0.0f);
      pVertices[10].m_vecTex  = D3DXVECTOR2( ROOM_TEXTURE_REPEAT_RATE,  ROOM_TEXTURE_REPEAT_RATE);
      pVertices[10].m_dwColor = LEFT_WALL_TINT;

      pVertices[11].m_vecPos  = D3DXVECTOR3(-ROOM_SIZE,  0.0f,  ROOM_SIZE);
      pVertices[11].m_vecNorm = D3DXVECTOR3( 1.0f,  0.0f,  0.0f);
      pVertices[11].m_vecTex  = D3DXVECTOR2( ROOM_TEXTURE_REPEAT_RATE, -ROOM_TEXTURE_REPEAT_RATE);
      pVertices[11].m_dwColor = LEFT_WALL_TINT;

      m_pvbVertices->Unlock();
   }

   // Create the effect
   if (FAILED (hr = D3DXCreateEffectFromFile(m_pDevice, "effects\\Spotlight_Backdrop.fx", NULL, NULL, 0, NULL, &m_pEffect, NULL)))
   {
      return hr;
   }

   m_pEffect->OnResetDevice();

   D3DXCreateTextureFromFileEx(m_pDevice, "textures\\Backdrop.dds", D3DX_DEFAULT, D3DX_DEFAULT, 
   D3DX_DEFAULT, 0, D3DFMT_DXT1, D3DPOOL_MANAGED, D3DX_DEFAULT, 
   D3DX_DEFAULT, 0, NULL, NULL, &m_pTexture);

   return S_OK;
}



HRESULT CBackdrop::OnLostDevice(void)
{
   m_pEffect->OnLostDevice();

   SAFE_RELEASE (m_pibIndices);
   SAFE_RELEASE (m_pvbVertices);

   SAFE_RELEASE (m_pTexture);
   
   return S_OK;
}


HRESULT CBackdrop::OnDestroyDevice(void)
{
   SAFE_RELEASE (m_pEffect);

   m_pDevice = NULL;

   return S_OK;
}

HRESULT CBackdrop::SetWorldLightProjBiasMatrix(D3DXMATRIX *matWorldLightProjBias)
{
   // If we have a null pointer, report it
   if (matWorldLightProjBias == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetWorldLightProjBiasMatrix (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_matWorldLightProjBias = *matWorldLightProjBias;

   return S_OK;
}

HRESULT CBackdrop::SetWorldLightProjScrollMatrices(D3DXMATRIX *matWorldLightProjScroll1, D3DXMATRIX *matWorldLightProjScroll2)
{
   // If we have a null pointer, report it
   if ((matWorldLightProjScroll1 == NULL) || (matWorldLightProjScroll2 == NULL))
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetWorldLightProjScrollMatrices (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_matWorldLightProjScroll1 = *matWorldLightProjScroll1;
   m_matWorldLightProjScroll2 = *matWorldLightProjScroll2;

   return S_OK;
}

HRESULT CBackdrop::SetWorldViewProjMatrix(D3DXMATRIX *matWorldViewProj)
{
   // If we have a null pointer, report it
   if (matWorldViewProj == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetWorldViewProjMatrix (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_matWorldViewProj = *matWorldViewProj;

   return S_OK;
}

HRESULT CBackdrop::SetWorldViewMatrix(D3DXMATRIX *matWorldView)
{
   // If we have a null pointer, report it
   if (matWorldView == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetWorldViewMatrix (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_matWorldView = *matWorldView;

   return S_OK;
}

HRESULT CBackdrop::SetWorldLightMatrix(D3DXMATRIX *matWorldLight)
{
   // If we have a null pointer, report it
   if (matWorldLight == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetWorldLightMatrix (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }
   
   m_matWorldLight = *matWorldLight;

   return S_OK;
}



HRESULT CBackdrop::SetITWorldViewMatrix(D3DXMATRIX *matITWorldView)
{
   // If we have a null pointer, report it
   if (matITWorldView == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetITWorldViewMatrix (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }
   
   m_matITWorldView = *matITWorldView;
   return S_OK;
}

HRESULT CBackdrop::SetWorldLightProjMatrix(D3DXMATRIX *matWorldLightProj)
{
   // If we have a null pointer, report it
   if (matWorldLightProj == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad pointer in CBackdrop::SetWorldLightProjMatrix (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_matWorldLightProj = *matWorldLightProj;

   return S_OK;
}

HRESULT CBackdrop::SetSceneLightPos(D3DXVECTOR3 vSceneLightPos)
{
   m_vSceneLightPos = vSceneLightPos;

   return S_OK;
}

HRESULT CBackdrop::SetTextureHandles(LPDIRECT3DTEXTURE9 pCookie,
                                     LPDIRECT3DTEXTURE9 pScrollingNoise,
                                     LPDIRECT3DTEXTURE9 pShadowMap)
{
   // If any handle is NULL, report an error
   if ((pCookie == NULL) || (pScrollingNoise == NULL) || (pShadowMap == NULL))
   {
      char strError[256];
      sprintf(strError, "Bad Texture handle in CBackdrop::SetTextureHandles (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_pCookie = pCookie;
   m_pScrollingNoise = pScrollingNoise;
   m_pShadowMap = pShadowMap;

   return S_OK;
}

HRESULT CBackdrop::SetLightParameters(D3DXVECTOR4 vLightColor, float fWidth, float fHeight)
{
   m_vLightColor        = vLightColor;
   m_fWidth             = fWidth;
   m_fHeight            = fHeight;

   return S_OK;
}


HRESULT CBackdrop::Draw(DWORD dwFlags)
{
   HRESULT hr;
   UINT iPass, cPasses;
   D3DXVECTOR4 Leye;

   // Matrices for getting primitives lit and on screen
   m_pEffect->SetMatrixTranspose("matWorldViewProj",         &m_matWorldViewProj);
//   m_pEffect->SetMatrixTranspose("matITWorldView",           &m_matITWorldView);
//   m_pEffect->SetMatrixTranspose("matWorldView",             &m_matWorldView);

   // Matrices for working with light space 
   m_pEffect->SetMatrixTranspose("matWorldLight",            &m_matWorldLight);
   m_pEffect->SetMatrixTranspose("matWorldLightProj",        &m_matWorldLightProj);
   m_pEffect->SetMatrixTranspose("matWorldLightProjBias",    &m_matWorldLightProjBias);
   m_pEffect->SetMatrixTranspose("matWorldLightProjScroll1", &m_matWorldLightProjScroll1);
   m_pEffect->SetMatrixTranspose("matWorldLightProjScroll2", &m_matWorldLightProjScroll2);

   // Transform light position into eye space and pass to effect
//   D3DXVec3Transform (&Leye, &m_vSceneLightPos, &m_matWorldView);
//   m_pEffect->SetVector("g_Leye",  &Leye);
  
   // Select the proper technique based on draw flags
   if (dwFlags & BACKDROP_DRAW_DEPTH)
   {
      m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("depth_technique"));
   }
   else
   {

      if (dwFlags & BACKDROP_SCROLLING_NOISE)
      {
         if (dwFlags & BACKDROP_SHADOW_MAPPING)
         {
         
            m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("noise_shadow_technique"));
         }
         else // no shadow
         {
            m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("noise_noshadow_technique"));
         }
      }
      else // no noise
      {
         if (dwFlags & BACKDROP_SHADOW_MAPPING)
         {
            m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("nonoise_shadow_technique"));
         }
         else // no shadow
         {
            m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("nonoise_noshadow_technique"));
         }
      }

      m_pEffect->SetTexture ("tBase",           m_pTexture);
      m_pEffect->SetTexture ("tCookie",         m_pCookie);
      m_pEffect->SetTexture ("tShadowMap",      m_pShadowMap);
      m_pEffect->SetTexture ("tScrollingNoise", m_pScrollingNoise);
   }

   // Run through all necesary passes for selected technique
   m_pEffect->Begin(&cPasses, 0);

   for (iPass = 0; iPass < cPasses; iPass++)
   {
      m_pEffect->Pass (iPass);

      if (FAILED (hr = m_pDevice->SetFVF (BackdropVertex::FVF)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetStreamSource (0, m_pvbVertices, 0, sizeof(BackdropVertex))))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetIndices (m_pibIndices)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->DrawIndexedPrimitive (D3DPT_TRIANGLELIST, 0, 0, m_uVertices, 0, 6)))
      {
         return hr;
      }
   }

   m_pEffect->End();

   return S_OK;
}
