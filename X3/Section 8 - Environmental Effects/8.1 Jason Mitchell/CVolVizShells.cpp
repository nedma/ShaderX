//=========================================================================================//
// filename: CVolVizShells.cpp                                                             //
//                                                                                         //
// author:   Jason L. Mitchell                                                             //
//           ATI Research, Inc.                                                            //
//           3D Application Research Group                                                 //
//           email: JasonM@ati.com                                                         //
//                                                                                         //
// Description: Shells for visualizing Light Shafts rendering                              //
//                                                                                         //
//=========================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                     //
//=========================================================================================//


#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <d3dx9.h>
#include "DXUtil.h"
#include "CVolVizShells.h"


// Global scene transforms
extern D3DXMATRIX          g_matProj;
extern D3DXMATRIX          g_matWorldViewProj;
extern D3DXMATRIX          g_matWorldView;
extern D3DXMATRIX          g_matITWorldView;
extern D3DXVECTOR3         g_vecSceneLightPos;


//////////////////////////////////////////////////////////////////////////////
// Types /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct ShellVertex
{
    D3DXVECTOR3 m_vecPos;
    DWORD       m_dwColor;

    static const DWORD FVF;

};
const DWORD ShellVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

#pragma pack()

CVolVizShells::CVolVizShells(void)
{
   m_pDevice         = NULL;
   m_pibIndices      = NULL;
   m_pvbVertices     = NULL;

   m_pCookie         = NULL;
   m_pScrollingNoise = NULL;
}

CVolVizShells::~CVolVizShells(void)
{
}

//----------------------------------------------------------------------------
// Intialize non-Direct3D members
//----------------------------------------------------------------------------
HRESULT CVolVizShells::Initialize(void)
{
   m_vWireColor = D3DXVECTOR4(1.0f, 0.2f, 0.2f, 1.0f);

   return S_OK;
}

//----------------------------------------------------------------------------
// Refresh the effect
//----------------------------------------------------------------------------
HRESULT CVolVizShells::RefreshEffect(void)
{
   LPD3DXBUFFER pBufferErrors = NULL;
   LPD3DXEFFECT pDummyEffect  = NULL;
   HRESULT hr;

   hr = D3DXCreateEffectFromFile(m_pDevice, TEXT("effects\\VolVizShells.fx"), NULL, NULL, 0, NULL, &pDummyEffect, &pBufferErrors );

   if (FAILED (hr))
   {
      OutputDebugString ("VolVizShells.fx Effect Refresh Error");
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
HRESULT CVolVizShells::OnCreateDevice(IDirect3DDevice9 *pDevice)
{
   m_pDevice = pDevice;
   return S_OK;
}

DWORD CVolVizShells::MapShellToColorChannel(int shell)
{
   DWORD dwChannel = shell % 4;

   switch (dwChannel)
   {
      default:
      case 0:
         return 0xFF000000;
      case 1:
         return 0x00FF0000;
      case 2:
         return 0x0000FF00;
      case 3:
         return 0x000000FF;
   }
}


//----------------------------------------------------------------------------
// Allocate VB, IB and texture
//----------------------------------------------------------------------------
HRESULT CVolVizShells::OnResetDevice(void)
{
   HRESULT hr;
 
   // Create indices
   if (!m_pibIndices)
   {
      DWORD *pdwIndices;

      m_numShells = 100;  // number of shells
      m_numRows = 1;    // number of rows of triangles (not rows of verts)
      m_numColumns = 1; // number of columns of triangles (not columns of verts)

      m_numTris = m_numShells * m_numRows * m_numColumns * 2;

      m_uVertices = (m_numRows + 1) * (m_numColumns + 1) * m_numShells;
      m_uIndices  = 3 * m_numTris;


      if (FAILED (hr = m_pDevice->CreateIndexBuffer(m_uIndices * sizeof(DWORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pibIndices, NULL)))
      {
         return hr;
      }

      if (FAILED (hr = m_pibIndices->Lock(0, m_uIndices * sizeof(DWORD), (void**) &pdwIndices, 0)))
      {
         return hr;
      }


      int vertsPerShell = (m_numRows + 1) * (m_numColumns + 1);
      int vertsPerRow   = m_numColumns + 1;

      int row, col, shell, curIndex = 0;

      for (shell = 0; shell < m_numShells; shell++)
      {
         for (row = 0; row < m_numRows; row++)
         {
            for (col = 0; col < m_numColumns; col++)
            {
               // Indices for one triangle
               pdwIndices[curIndex++] = shell * vertsPerShell + row * vertsPerRow + col;
               pdwIndices[curIndex++] = shell * vertsPerShell + row * vertsPerRow + col + 1;
               pdwIndices[curIndex++] = shell * vertsPerShell + (row+1) * vertsPerRow + col;

               // Indices for one triangle
               pdwIndices[curIndex++] = shell * vertsPerShell + (row+1) * vertsPerRow + col;
               pdwIndices[curIndex++] = shell * vertsPerShell + row * vertsPerRow + col + 1;
               pdwIndices[curIndex++] = shell * vertsPerShell + (row+1) * vertsPerRow + col + 1;

            }
         }
      }


//      for (UINT i = 0; i < m_uIndices; i+=3)
//      {
//         char buff[256];
//         sprintf(buff, "{%d, %d, %d}\n", pdwIndices[i], pdwIndices[i+1], pdwIndices[i+2]);
//         OutputDebugString(buff);
//      }




      m_pibIndices->Unlock();


   }

    
   // Create vertices
   if (!m_pvbVertices)
   {
      if (FAILED (hr = m_pDevice->CreateVertexBuffer(m_uVertices * sizeof(ShellVertex), D3DUSAGE_WRITEONLY, ShellVertex::FVF, D3DPOOL_DEFAULT, &m_pvbVertices, NULL)))
      {
         return hr;
      }


      ShellVertex *pVertices;

      if (FAILED (hr = m_pvbVertices->Lock(0, m_uVertices * sizeof(ShellVertex), (void**) &pVertices, 0)))
      {
         return hr;
      }

      int row, col, shell, curVertex = 0;
      DWORD dwCurrentColor;

      for (shell = 0; shell < m_numShells; shell++)
      {
         dwCurrentColor = MapShellToColorChannel(shell);

         for (row = 0; row <= m_numRows; row++)          // runs m_numRows + 1 times
         {
            for (col = 0; col <= m_numColumns; col++)    // runs m_numColumns + 1 times
            {
               // Vertices are positioned in 0..1 cube
               // Vertex shader will place them in correct volume in view space

               FLOAT fX =         ((float)col)   / ((float)(m_numColumns));
               FLOAT fY = 1.0f - (((float)row)   / ((float)(m_numRows)));
               FLOAT fZ = 1.0f - (((float)shell) / ((float)(m_numShells-1)));

               pVertices[curVertex].m_vecPos  = D3DXVECTOR3(fX, fY, fZ);
               pVertices[curVertex].m_dwColor = dwCurrentColor;

//               char buff[256];
//               sprintf(buff, "{%2.3f, %2.3f, %2.3f}\n", pVertices[curVertex].m_vecPos.x, pVertices[curVertex].m_vecPos.y, pVertices[curVertex].m_vecPos.z);
//               OutputDebugString(buff);

               curVertex++;
            }
//            OutputDebugString("\n");
         }
      }


      m_pvbVertices->Unlock();
   }


   if (FAILED (hr = D3DXCreateEffectFromFile(m_pDevice, "effects\\VolVizShells.fx", NULL, NULL, 0, NULL, &m_pEffect, NULL)))
   {
      return hr;
   }

   m_pEffect->OnResetDevice();

   return S_OK;
}



HRESULT CVolVizShells::OnLostDevice(void)
{
   m_pEffect->OnLostDevice();

   SAFE_RELEASE (m_pibIndices);
   SAFE_RELEASE (m_pvbVertices);

   // Didn't allocate in here but did AddRef
//   SAFE_RELEASE (m_pCookie);
   
   return S_OK;
}


HRESULT CVolVizShells::OnDestroyDevice(void)
{
   SAFE_RELEASE (m_pEffect);

   m_pDevice = NULL;
   return S_OK;
}


HRESULT CVolVizShells::SetVolVizBounds(D3DXVECTOR4 vMinBounds, D3DXVECTOR4 vMaxBounds, FLOAT fSamplingDelta)
{
   m_vMinBounds = vMinBounds;
   m_vMaxBounds = vMaxBounds;

   m_fSamplingDelta = fSamplingDelta;

   float fZBounds = m_vMaxBounds.z - m_vMinBounds.z;

   // Compute number of shells to actually be drawn
   m_numShellsToDraw = (INT) ((fZBounds / m_fSamplingDelta) + 0.5f);
   m_numShellsToDraw = min(m_numShells, m_numShellsToDraw);

   // Compute how many triangles to actually be drawn
   m_numTrisToDraw = m_numShellsToDraw * m_numRows * m_numColumns * 2;

   return S_OK;
}

HRESULT CVolVizShells::SetLightParameters(D3DXVECTOR4 vLightColor, float fWidth, float fHeight, float fFarPlane)
{
   m_vLightColor        = vLightColor;
   m_fWidth             = fWidth;
   m_fHeight            = fHeight;
   m_fFarPlane          = fFarPlane;

   return S_OK;
}

INT CVolVizShells::GetNumShellsDrawn(void)
{
   return m_numShellsToDraw;
}



HRESULT CVolVizShells::SetFrozenInverseViewMatrix(D3DXMATRIX *matInvView)
{
   m_matInvView = *matInvView;

   return S_OK;
}

HRESULT CVolVizShells::SetViewWorldLightMatrix(D3DXMATRIX *matViewWorldLight)
{
   m_matViewWorldLight = *matViewWorldLight;

   return S_OK;
}



HRESULT CVolVizShells::SetViewWorldLightProjBiasMatrix(D3DXMATRIX *matViewWorldLightProjBias)
{
   m_matViewWorldLightProjBias = *matViewWorldLightProjBias;

   return S_OK;
}

HRESULT CVolVizShells::SetViewWorldLightProjScrollMatrices(D3DXMATRIX *matViewWorldLightProjScroll1, D3DXMATRIX *matViewWorldLightProjScroll2)
{
   m_matViewWorldLightProjScroll1 = *matViewWorldLightProjScroll1;
   m_matViewWorldLightProjScroll2 = *matViewWorldLightProjScroll2;

   return S_OK;
}


HRESULT CVolVizShells::SetViewWorldLightProjMatrix(D3DXMATRIX *matViewWorldLightProj)
{
   m_matViewWorldLightProj = *matViewWorldLightProj;

   return S_OK;
}

HRESULT CVolVizShells::SetTextures(LPDIRECT3DTEXTURE9 pCookie, LPDIRECT3DTEXTURE9 pScrollingNoise, LPDIRECT3DTEXTURE9 pShadowMap)
{
   m_pCookie         = pCookie;
   m_pScrollingNoise = pScrollingNoise;
   m_pShadowMap      = pShadowMap;

   return S_OK;
}

HRESULT CVolVizShells::SetClipPlanes(D3DXPLANE *ClipSpaceFrustumPlanes)
{
   // Copy these in for later use during draw calls
   for (int i=0; i<6; i++)
   {
      m_ClipSpaceLightFrustumPlanes[i] = ClipSpaceFrustumPlanes[i];
   }

   return S_OK;
}


HRESULT CVolVizShells::DrawFrozenShells(void)
{
   HRESULT hr;
   UINT iPass, cPasses;


   // Fraction of shells in VB to actually be drawn
   FLOAT fFractionOfMaxShells = ( ((FLOAT) m_numShellsToDraw) / ((FLOAT) m_numShells));

   m_pEffect->SetMatrixTranspose("matProj",  &g_matProj);
   m_pEffect->SetMatrixTranspose("matInvView",  &m_matInvView);
   m_pEffect->SetMatrixTranspose("matWorldViewProj", &g_matWorldViewProj);

   m_pEffect->SetVector("wireColor",  &m_vWireColor);
   m_pEffect->SetVector("vMinBounds", &m_vMinBounds);
   m_pEffect->SetVector("vMaxBounds", &m_vMaxBounds);

   m_pEffect->SetFloat("fFractionOfMaxShells", fFractionOfMaxShells);

   m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("frozen_wireframe_technique"));

   for (int i=0; i<6; i++)
   {
      m_pDevice->SetClipPlane(i, (float *)(m_ClipSpaceLightFrustumPlanes+i));
   }

   m_pEffect->Begin(&cPasses, 0);

   for (iPass = 0; iPass < cPasses; iPass++)
   {
      m_pEffect->Pass(iPass);

      if (FAILED (hr = m_pDevice->SetFVF(ShellVertex::FVF)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbVertices, 0, sizeof(ShellVertex))))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetIndices(m_pibIndices)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uVertices, 0, m_numTrisToDraw)))
      {
         return hr;
      }
   }

   m_pEffect->End();

   return S_OK;
}


HRESULT CVolVizShells::DrawWireframeShells(void)
{
   HRESULT hr;
   UINT iPass, cPasses;

   m_pEffect->SetMatrixTranspose("matProj",  &g_matProj);

   m_pEffect->SetVector("wireColor",  &m_vWireColor);
   m_pEffect->SetVector("vMinBounds", &m_vMinBounds);
   m_pEffect->SetVector("vMaxBounds", &m_vMaxBounds);

   m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("wireframe_technique"));

   for (int i=0; i<6; i++)
   {
      m_pDevice->SetClipPlane(i, (float *)(m_ClipSpaceLightFrustumPlanes+i));
   }

   m_pEffect->Begin(&cPasses, 0);

   for (iPass = 0; iPass < cPasses; iPass++)
   {
      m_pEffect->Pass(iPass);

      if (FAILED (hr = m_pDevice->SetFVF(ShellVertex::FVF)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbVertices, 0, sizeof(ShellVertex))))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetIndices(m_pibIndices)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uVertices, 0, m_numTris)))
      {
         return hr;
      }
   }

   m_pEffect->End();

   return S_OK;

}

HRESULT CVolVizShells::Draw(DWORD dwFlags)
{
   HRESULT hr;
   UINT iPass, cPasses;
   float fZBounds = m_vMaxBounds.z - m_vMinBounds.z;

   // Compute number of shells to actually be drawn
   m_numShellsToDraw = (INT) ((fZBounds / m_fSamplingDelta) + 0.5f);
   m_numShellsToDraw = min(m_numShells, m_numShellsToDraw);

   // Compute how many triangles to actually be drawn
   m_numTrisToDraw = m_numShellsToDraw * m_numRows * m_numColumns * 2;

   // Fraction of shells in VB to actually be drawn
   FLOAT fFractionOfMaxShells = ( ((FLOAT) m_numShellsToDraw) / ((FLOAT) m_numShells));

   m_pEffect->SetTexture ("tCookie",         m_pCookie);
   m_pEffect->SetTexture ("tScrollingNoise", m_pScrollingNoise);
   m_pEffect->SetTexture ("tShadowMap",      m_pShadowMap);

   // Matrices for getting stuff on the screen
   m_pEffect->SetMatrixTranspose("matProj",          &g_matProj);
   m_pEffect->SetMatrixTranspose("matWorldViewProj", &g_matWorldViewProj);

   // Matrices for working with light space
   m_pEffect->SetMatrixTranspose("matViewWorldLight",         &m_matViewWorldLight);
   m_pEffect->SetMatrixTranspose("matViewWorldLightProj",     &m_matViewWorldLightProj);
   m_pEffect->SetMatrixTranspose("matViewWorldLightProjBias", &m_matViewWorldLightProjBias);
   m_pEffect->SetMatrixTranspose("matViewWorldLightProjScroll1", &m_matViewWorldLightProjScroll1);
   m_pEffect->SetMatrixTranspose("matViewWorldLightProjScroll2", &m_matViewWorldLightProjScroll2);
   
   m_pEffect->SetVector("wireColor",  &m_vWireColor);
   m_pEffect->SetVector("lightColor", &m_vLightColor);
   m_pEffect->SetVector("vMinBounds", &m_vMinBounds);
   m_pEffect->SetVector("vMaxBounds", &m_vMaxBounds);

   m_pEffect->SetFloat("fFractionOfMaxShells", fFractionOfMaxShells);
   m_pEffect->SetFloat ("fDepthScaleFactor", m_fFarPlane);

   if (dwFlags & VOLVIZ_SHELLS_WIREFRAME)
   {
      if (dwFlags & VOLVIZ_SHELLS_CLIP)
      {
         m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("wireframe_technique_clip"));
      }
      else
      {
         m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("wireframe_technique_noclip"));
      }
   }
   else
   {
      if (dwFlags & VOLVIZ_SHELLS_CLIP)
      {
         if (dwFlags & VOLVIZ_SCROLLING_NOISE)
         {
            if (dwFlags & VOLVIZ_SHADOW_MAPPING)
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("clip_noise_shadow_cookie_technique"));
            }
            else
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("clip_noise_noshadow_cookie_technique"));
            }
         }
         else
         {
            if (dwFlags & VOLVIZ_SHADOW_MAPPING)
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("clip_nonoise_shadow_cookie_technique"));
            }
            else
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("clip_nonoise_noshadow_cookie_technique"));
            }
         }
      }
      else
      {
         if (dwFlags & VOLVIZ_SCROLLING_NOISE)
         {
            if (dwFlags & VOLVIZ_SHADOW_MAPPING)
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("noclip_noise_shadow_technique"));
            }
            else
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("noclip_noise_noshadow_technique"));
            }
         }
         else
         {
            if (dwFlags & VOLVIZ_SHADOW_MAPPING)
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("noclip_nonoise_shadow_technique"));
            }
            else
            {
               m_pEffect->SetTechnique (m_pEffect->GetTechniqueByName ("noclip_nonoise_noshadow_technique"));
            }
         }
      }
   }
     


   for (int i=0; i<6; i++)
   {
      m_pDevice->SetClipPlane(i, (float *)(m_ClipSpaceLightFrustumPlanes+i));
   }

   m_pEffect->Begin(&cPasses, 0);

   for (iPass = 0; iPass < cPasses; iPass++)
   {
      m_pEffect->Pass(iPass);

      if (FAILED (hr = m_pDevice->SetFVF(ShellVertex::FVF)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbVertices, 0, sizeof(ShellVertex))))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->SetIndices(m_pibIndices)))
      {
         return hr;
      }

      if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uVertices, 0, m_numTrisToDraw)))
      {
         return hr;
      }
   }

   m_pEffect->End();

   return S_OK;
}
