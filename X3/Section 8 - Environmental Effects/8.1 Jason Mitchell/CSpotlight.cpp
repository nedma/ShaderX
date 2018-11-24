//======================================================================================//
// filename: CSpotlight.cpp                                                             //
//                                                                                      //
// author:   Jason L. Mitchell                                                          //
//           ATI Research, Inc.                                                         //
//           3D Application Research Group                                              //
//           email: JasonM@ati.com                                                      //
//                                                                                      //
// Description: Spotlight Class                                                         //
//                                                                                      //
//======================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                  //
//======================================================================================//

#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <d3dx9.h>
#include "DXUtil.h"
#include "CSpotlight.h"


// Global scene transforms
extern D3DXMATRIX          g_matWorldViewProj;
extern D3DXMATRIX          g_matWorldView;
extern D3DXMATRIX          g_matITWorldView;


//////////////////////////////////////////////////////////////////////////////
// Types /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct FrustumVertex
{
    D3DXVECTOR3 m_vecPos;
    D3DXVECTOR3 m_vecNorm;
    D3DXVECTOR2 m_vecTex;

    static const DWORD FVF;
};
const DWORD FrustumVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

struct WireFrustumVertex
{
    D3DXVECTOR3 m_vecPos;
    D3DCOLOR    m_dwColor;

    static const DWORD FVF;
};
const DWORD WireFrustumVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

#pragma pack()



//////////////////////////////////////////////////////////////////////////////
// CSpotlight implementation /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CSpotlight::CSpotlight()
{
   m_pDevice                = NULL;
   m_pFrustumEffect         = NULL;

   // Light source geometry
   m_pibFrustumIndices      = NULL;
   m_pvbFrustumVertices     = NULL;

   // Wireframe frustum geometry
   m_pibWireFrustumIndices  = NULL;
   m_pvbWireFrustumVertices = NULL;

   // Texture and surface pointers
   m_pCookie                = NULL;

   // BOOL to tell if we're using ground plane instead of far frustum plane
   m_bFarPlaneIsGroundPlane = TRUE;
}


CSpotlight::~CSpotlight(void)
{
}


//----------------------------------------------------------------------------
//
// Intialize non-Direct3D structures
//
//----------------------------------------------------------------------------
HRESULT CSpotlight::Initialize(void)
{
    return S_OK;
}




HRESULT CSpotlight::OnCreateDevice(IDirect3DDevice9 *pDevice)
{
   // If  the device is NULL, report an error
   if (pDevice == NULL)
   {
      char strError[256];
      sprintf (strError, "Bad Direct3D Device in CSpotlight::OnCreateDevice (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString (strError);

      return S_FALSE;
   }


   m_pDevice = pDevice;
   return S_OK;
}




//----------------------------------------------------------------------------
// Allocate VB, IB and texture
// JasonM
// Fill in index buffer data
//----------------------------------------------------------------------------
HRESULT CSpotlight::OnResetDevice(void)
{
   HRESULT hr;
   char strError[256];
   WORD *pwIndices;
   int i;

   // Create indices
   if (m_pibFrustumIndices == NULL)
   {
      m_uFrustumIndices = 18;

      if (FAILED (hr = m_pDevice->CreateIndexBuffer(m_uFrustumIndices * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pibFrustumIndices, NULL)))
      {
         sprintf (strError, "Failed to create Index Buffer in CSpotlight::OnResetDevice (%s line %d)\n", __FILE__, __LINE__);
         OutputDebugString (strError);
         return hr;
      }

      if (FAILED(hr = m_pibFrustumIndices->Lock(0, m_uFrustumIndices * sizeof(WORD), (void**) &pwIndices, 0)))
      {
         sprintf (strError, "Index buffer lock failed in CSpotlight::OnResetDevice (%s line %d)\n", __FILE__, __LINE__);
         OutputDebugString (strError);
         return hr;
      }
      
      for (i = 0; i < 18; i++)
      {
         pwIndices[i] = i;
      }

      if (FAILED (hr = m_pibFrustumIndices->Unlock()))
      {
         sprintf (strError, "Index buffer unlock failed in CSpotlight::OnResetDevice (%s line %d)\n", __FILE__, __LINE__);
         OutputDebugString (strError);
         return hr;
      }
   }
    
   // Create vertices
   if (m_pvbFrustumVertices == NULL)
   {
      m_uFrustumVertices = 18;

      if (FAILED(hr = m_pDevice->CreateVertexBuffer(m_uFrustumVertices * sizeof(FrustumVertex), D3DUSAGE_WRITEONLY, FrustumVertex::FVF, D3DPOOL_DEFAULT, &m_pvbFrustumVertices, NULL)))
      {
         sprintf (strError, "Vertex buffer creation failed in CSpotlight::OnResetDevice (%s line %d)\n", __FILE__, __LINE__);
         OutputDebugString (strError);
         return hr;
      }
   }



   // Create indices for wireframe frustum
   if (m_pibWireFrustumIndices == NULL)
   {
      m_uWireFrustumIndices = 24;

      if (FAILED(hr = m_pDevice->CreateIndexBuffer(m_uWireFrustumIndices * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pibWireFrustumIndices, NULL)))
      {
         return hr;
      }

      if (FAILED(hr = m_pibWireFrustumIndices->Lock(0, m_uWireFrustumIndices * sizeof(WORD), (void**) &pwIndices, 0)))
      {
         return hr;
      }

      pwIndices[0]  = 0;      pwIndices[1]  = 1;
      pwIndices[2]  = 1;      pwIndices[3]  = 2;
      pwIndices[4]  = 2;      pwIndices[5]  = 3;
      pwIndices[6]  = 0;      pwIndices[7]  = 3;

      pwIndices[8]  = 4;      pwIndices[9]  = 5;
      pwIndices[10] = 5;      pwIndices[11] = 6;
      pwIndices[12] = 6;      pwIndices[13] = 7;
      pwIndices[14] = 4;      pwIndices[15] = 7;

      pwIndices[16] = 0;      pwIndices[17] = 4;
      pwIndices[18] = 1;      pwIndices[19] = 5;
      pwIndices[20] = 2;      pwIndices[21] = 6;
      pwIndices[22] = 3;      pwIndices[23] = 7;

      m_pibWireFrustumIndices->Unlock();
   }


   // Create vertices
   if (m_pvbWireFrustumVertices == NULL)
   {
      m_uWireFrustumVertices = 8;

      if (FAILED(hr = m_pDevice->CreateVertexBuffer(m_uWireFrustumVertices * sizeof(WireFrustumVertex), D3DUSAGE_WRITEONLY, WireFrustumVertex::FVF, D3DPOOL_DEFAULT, &m_pvbWireFrustumVertices, NULL)))
      {
         return hr;
      }

      RegenerateWireFrustum();
   }


   if (FAILED(hr = D3DXCreateEffectFromFile(m_pDevice, "effects\\Spotlight.fx", NULL, NULL, 0, NULL, &m_pFrustumEffect, NULL)))
   {
      return hr;
   }

   m_pFrustumEffect->OnResetDevice();


   return S_OK;
}



HRESULT CSpotlight::OnLostDevice()
{
//   m_pFrustumEffect->OnLostDevice();

   SAFE_RELEASE (m_pibFrustumIndices);
   SAFE_RELEASE (m_pvbFrustumVertices);

   SAFE_RELEASE (m_pibWireFrustumIndices);
   SAFE_RELEASE (m_pvbWireFrustumVertices);


   return S_OK;
}


HRESULT CSpotlight::OnDestroyDevice(void)
{
   SAFE_RELEASE(m_pFrustumEffect);

   m_pDevice = NULL;
   return S_OK;
}

D3DXMATRIX *CSpotlight::GetViewMatrix(void)
{
   return &m_matView;
}


D3DXMATRIX *CSpotlight::GetProjectionMatrix(void)
{
   return &m_matProjection;
}

LPDIRECT3DTEXTURE9 CSpotlight::GetCookie(void)
{
   return m_pCookie;
}

D3DXVECTOR4 CSpotlight::GetLightColor(void)
{
   return m_vLightColor;
}

//----------------------------------------------------------------------------
// Get the spotlight width
//----------------------------------------------------------------------------
FLOAT CSpotlight::GetWidth(void)
{
   return m_fWidth;
}


//----------------------------------------------------------------------------
// Get the spotlight height
//----------------------------------------------------------------------------
FLOAT CSpotlight::GetHeight(void)
{
   return m_fHeight;
}


HRESULT CSpotlight::GetViewSpaceBounds(D3DXVECTOR4 *vMinBounds, D3DXVECTOR4 *vMaxBounds)
{
   D3DXMATRIX  matFrustumWorld, matFrustumWorldView;
   D3DXVECTOR4 vert;

   // Init mins and maxes
   *vMinBounds =  D3DXVECTOR4( FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX);
   *vMaxBounds =  D3DXVECTOR4(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

   // Transformation from light space to view space
   D3DXMatrixInverse (&matFrustumWorld, NULL, &m_matView);
   D3DXMatrixMultiply (&matFrustumWorldView, &matFrustumWorld, &g_matWorldView);

   // For each of the eight verts of the light frustum...
   for (int i=0; i<8; i++)
   {
      // Transform to view space
      D3DXVec3Transform (&vert, &m_FrustumVerts[i], &matFrustumWorldView);

      if (vert.x < vMinBounds->x)
      {
         vMinBounds->x = vert.x;
      }

      if (vert.y < vMinBounds->y)
      {
         vMinBounds->y = vert.y;
      }

      if (vert.z < vMinBounds->z)
      {
         vMinBounds->z = vert.z;
      }

      if (vert.x > vMaxBounds->x)
      {
         vMaxBounds->x = vert.x;
      }

      if (vert.y > vMaxBounds->y)
      {
         vMaxBounds->y = vert.y;
      }

      if (vert.z > vMaxBounds->z)
      {
         vMaxBounds->z = vert.z;
      }
   }

   return S_OK;
}

HRESULT CSpotlight::GetWorldSpaceFrustumPlanes(D3DXPLANE *WorldSpaceFrustumPlanes)
{
   D3DXMATRIX  matITLightToWorld;

   // Plane transformation from light space to world space
   D3DXMatrixTranspose (&matITLightToWorld, &m_matView);

   D3DXPlaneTransformArray (WorldSpaceFrustumPlanes, sizeof(D3DXPLANE), m_FrustumPlanes, sizeof(D3DXPLANE), &matITLightToWorld, 6);


   // If we're using the ground plane instead of the far plane
   if (m_bFarPlaneIsGroundPlane)
   {
      // This plane is far/ground plane
      D3DXVECTOR3 GroundPlaneVert[3];
      GroundPlaneVert[0] = D3DXVECTOR3(0.0f, -0.02f, 0.0f); // Hard-coded to y=-epsilon plane
      GroundPlaneVert[1] = D3DXVECTOR3(1.0f, -0.02f, 1.0f);
      GroundPlaneVert[2] = D3DXVECTOR3(1.0f, -0.02f, 0.0f);

      D3DXPlaneFromPoints (&WorldSpaceFrustumPlanes[1], &GroundPlaneVert[0], &GroundPlaneVert[1], &GroundPlaneVert[2]);
   }

   return S_OK;
}


HRESULT CSpotlight::RegenerateWireFrustum(void)
{
   HRESULT hr;

   // If this is called before VBs are allocated, just return
   if (!m_pvbWireFrustumVertices)
   {
      return S_OK;
   }

   WireFrustumVertex *pWireVertices;

   if (FAILED (hr = m_pvbWireFrustumVertices->Lock(0, m_uWireFrustumVertices * sizeof(WireFrustumVertex), (void**) &pWireVertices, 0)))
   {
      return hr;
   }

#define NEAR_DIST 10.0f
#define FAR_DIST  400.0f

   // Compute Field of View as function of width at z==1
   m_fFOV = 2 * atanf(m_fWidth);
   m_fVerticalFOV = 2 * atanf(m_fHeight);
   float fAspect = m_fHeight / m_fWidth;

   // Size of near and far quads
   m_fNearWidth  = NEAR_DIST * tanf(m_fFOV/2.0f);
   m_fNearHeight = NEAR_DIST * tanf(m_fVerticalFOV/2.0f);

   m_fVerticalFOV = 2 * atanf(m_fHeight);
   m_fFarWidth  = FAR_DIST * tanf(m_fFOV/2.0f);
   m_fFarHeight = FAR_DIST * tanf(m_fVerticalFOV/2.0f);

   int curVert = 0;

   // Near quad of wireframe frustum
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[0] = D3DXVECTOR3(-m_fNearWidth,  m_fNearHeight, NEAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[1] = D3DXVECTOR3( m_fNearWidth,  m_fNearHeight, NEAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[2] = D3DXVECTOR3( m_fNearWidth, -m_fNearHeight, NEAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[3] = D3DXVECTOR3(-m_fNearWidth, -m_fNearHeight, NEAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;

   // Far quad of wireframe frustum 
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[4] = D3DXVECTOR3(-m_fFarWidth,  m_fFarHeight, FAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[5] = D3DXVECTOR3( m_fFarWidth,  m_fFarHeight, FAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[6] = D3DXVECTOR3( m_fFarWidth, -m_fFarHeight, FAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;
   pWireVertices[curVert].m_vecPos = m_FrustumVerts[7] = D3DXVECTOR3(-m_fFarWidth, -m_fFarHeight, FAR_DIST);
   pWireVertices[curVert].m_dwColor = CLIP_VOLUME_COLOR;
   curVert++;

   m_pvbWireFrustumVertices->Unlock();

   // If we have valid VBs and the frustum just changed...
   if (m_pvbFrustumVertices )
   {
      // Regen the little light-source geometry here...

      FrustumVertex *pVertices;

      if (FAILED (hr = m_pvbFrustumVertices->Lock(0, m_uFrustumVertices * sizeof(FrustumVertex), (void**) &pVertices, 0)))
      {
         return hr;
      }

      D3DXVECTOR3 vNormal;

      //
      // Pyramid faces +z with tip at origin
      //

      float fLightCapDistance = 10.0f;
      float fLightNearWidth  = fLightCapDistance * tanf(m_fFOV/2.0f);
      float fLightNearHeight = fLightCapDistance * tanf(m_fVerticalFOV/2.0f);

      // +x face of frustum body
      pVertices[0].m_vecPos = D3DXVECTOR3(0.0f,  0.0f, 0.0f);
      pVertices[1].m_vecPos = D3DXVECTOR3(fLightNearWidth,  fLightNearHeight, fLightCapDistance);
      pVertices[2].m_vecPos = D3DXVECTOR3(fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      D3DXVec3Cross (&vNormal, &pVertices[1].m_vecPos, &pVertices[2].m_vecPos);
      D3DXVec3Normalize(&vNormal, &vNormal);
      pVertices[0].m_vecNorm = pVertices[1].m_vecNorm = pVertices[2].m_vecNorm = vNormal;

      // -y face of frustum body
      pVertices[3].m_vecPos = D3DXVECTOR3( 0.0f,  0.0f, 0.0f);
      pVertices[4].m_vecPos = D3DXVECTOR3( fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      pVertices[5].m_vecPos = D3DXVECTOR3(-fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      D3DXVec3Cross (&vNormal, &pVertices[4].m_vecPos, &pVertices[5].m_vecPos);
      D3DXVec3Normalize(&vNormal, &vNormal);
      pVertices[3].m_vecNorm = pVertices[4].m_vecNorm = pVertices[5].m_vecNorm = vNormal;

      // -x face of frustum body
      pVertices[6].m_vecPos = D3DXVECTOR3( 0.0f,  0.0f, 0.0f);
      pVertices[7].m_vecPos = D3DXVECTOR3(-fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      pVertices[8].m_vecPos = D3DXVECTOR3(-fLightNearWidth,  fLightNearHeight, fLightCapDistance);
      D3DXVec3Cross (&vNormal, &pVertices[8].m_vecPos, &pVertices[7].m_vecPos);
      D3DXVec3Normalize(&vNormal, &vNormal);
      pVertices[6].m_vecNorm = pVertices[7].m_vecNorm = pVertices[8].m_vecNorm = vNormal;

      // -y face of frustum body
      pVertices[9].m_vecPos  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);
      pVertices[10].m_vecPos = D3DXVECTOR3(-fLightNearWidth, fLightNearHeight, fLightCapDistance);
      pVertices[11].m_vecPos = D3DXVECTOR3( fLightNearWidth, fLightNearHeight, fLightCapDistance);
      D3DXVec3Cross (&vNormal, &pVertices[10].m_vecPos, &pVertices[11].m_vecPos);
      D3DXVec3Normalize(&vNormal, &vNormal);
      pVertices[9].m_vecNorm = pVertices[10].m_vecNorm = pVertices[11].m_vecNorm = vNormal;


      // Cookie capping the light frustum
      pVertices[12].m_vecPos = D3DXVECTOR3(-fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      pVertices[12].m_vecTex = D3DXVECTOR2(0.0f, 1.0f);
      pVertices[13].m_vecPos = D3DXVECTOR3( fLightNearWidth,  fLightNearHeight, fLightCapDistance);
      pVertices[13].m_vecTex = D3DXVECTOR2(1.0f, 0.0f);
      pVertices[14].m_vecPos = D3DXVECTOR3(-fLightNearWidth,  fLightNearHeight, fLightCapDistance);
      pVertices[14].m_vecTex = D3DXVECTOR2(0.0f, 0.0f);

      pVertices[15].m_vecPos = D3DXVECTOR3(-fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      pVertices[15].m_vecTex = D3DXVECTOR2(0.0f, 1.0f);
      pVertices[16].m_vecPos = D3DXVECTOR3( fLightNearWidth, -fLightNearHeight, fLightCapDistance);
      pVertices[16].m_vecTex = D3DXVECTOR2(1.0f, 1.0f);
      pVertices[17].m_vecPos = D3DXVECTOR3( fLightNearWidth,  fLightNearHeight, fLightCapDistance);
      pVertices[17].m_vecTex = D3DXVECTOR2(1.0f, 0.0f);

      pVertices[12].m_vecNorm = pVertices[13].m_vecNorm = pVertices[14].m_vecNorm = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
      pVertices[15].m_vecNorm = pVertices[16].m_vecNorm = pVertices[17].m_vecNorm = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

      m_pvbFrustumVertices->Unlock();
   }

   //
   // Update light-space planes defining the frustum
   //
   D3DXPlaneFromPoints (&m_FrustumPlanes[0], &m_FrustumVerts[2], &m_FrustumVerts[1], &m_FrustumVerts[3]);
   D3DXPlaneFromPoints (&m_FrustumPlanes[1], &m_FrustumVerts[7], &m_FrustumVerts[4], &m_FrustumVerts[6]);
   D3DXPlaneFromPoints (&m_FrustumPlanes[2], &m_FrustumVerts[0], &m_FrustumVerts[1], &m_FrustumVerts[4]);
   D3DXPlaneFromPoints (&m_FrustumPlanes[3], &m_FrustumVerts[3], &m_FrustumVerts[0], &m_FrustumVerts[7]);
   D3DXPlaneFromPoints (&m_FrustumPlanes[4], &m_FrustumVerts[3], &m_FrustumVerts[6], &m_FrustumVerts[2]);
   D3DXPlaneFromPoints (&m_FrustumPlanes[5], &m_FrustumVerts[1], &m_FrustumVerts[2], &m_FrustumVerts[5]);

//   for (int i = 0; i < 6; i++)
//   {
//      char buff[256];
//      sprintf(buff, "{Plane %d: %2.2f, %2.2f, %2.2f, %2.2f}\n", i, m_FrustumPlanes[i].a, m_FrustumPlanes[i].b, m_FrustumPlanes[i].c, m_FrustumPlanes[i].d);
//      OutputDebugString(buff);
//   }


   return S_OK;

}

//----------------------------------------------------------------------------
// Set Parameters which define the view
//----------------------------------------------------------------------------
HRESULT CSpotlight::SetView(D3DXVECTOR3 vEyePt, D3DXVECTOR3 vLookAtPt, D3DXVECTOR3 vUpVec, FLOAT fWidth, FLOAT fHeight, FLOAT fZNear, FLOAT fZFar)
{
   // Stash inputs away in private members
   m_vEyePt       = vEyePt;
   m_vLookAtPt    = vLookAtPt;
   m_vUpVec       = vUpVec;

   // Compute Field of View as function of width at z==1
   m_fFOV = 2 * atanf(fHeight);
   float fAspect = fWidth / fHeight;

   // Construct View and Projection matrices
   D3DXMatrixLookAtLH (&m_matView, &vEyePt, &vLookAtPt, &vUpVec);
   D3DXMatrixPerspectiveFovLH (&m_matProjection, m_fFOV, fAspect, fZNear, fZFar);

   return S_OK;
}


//----------------------------------------------------------------------------
// Set the light color
//----------------------------------------------------------------------------
HRESULT CSpotlight::SetLightColor(D3DXVECTOR4 vLightColor)
{
   m_vLightColor = vLightColor;

   return S_OK;
}


//----------------------------------------------------------------------------
// Set the spotlight width
//----------------------------------------------------------------------------
HRESULT CSpotlight::SetWidth(FLOAT fWidth)
{
   m_fWidth = fWidth;

   RegenerateWireFrustum();

   return S_OK;
}


//----------------------------------------------------------------------------
// Set the spotlight height
//----------------------------------------------------------------------------
HRESULT CSpotlight::SetHeight(FLOAT fHeight)
{
   m_fHeight = fHeight;

   RegenerateWireFrustum();

   return S_OK;
}


HRESULT CSpotlight::SetSceneLightPos(D3DXVECTOR3 vSceneLightPos)
{
   m_vSceneLightPos = vSceneLightPos;

   return S_OK;
}


//----------------------------------------------------------------------------
// Set the cookie
//----------------------------------------------------------------------------
HRESULT CSpotlight::SetCookie(LPDIRECT3DTEXTURE9 pCookie)
{
   // If  the texture point is NULL, report an error
   if (pCookie == NULL)
   {
      char strError[256];
      sprintf(strError, "Bad texture pointer in CSpotlight::SetCookie (%s line %d)\n", __FILE__, __LINE__);
      OutputDebugString(strError);

      return S_FALSE;
   }

   m_pCookie = pCookie;

   return S_OK;
}


//----------------------------------------------------------------------------
// Decide whether or not the far plane gets replaced by a ground plane
//----------------------------------------------------------------------------
HRESULT CSpotlight::SetGroundPlaneClip(BOOL bFarPlaneIsGroundPlane)
{
   m_bFarPlaneIsGroundPlane = bFarPlaneIsGroundPlane;

   return S_OK;
}


//----------------------------------------------------------------------------
// Refresh effects
//----------------------------------------------------------------------------
HRESULT CSpotlight::RefreshEffect(void)
{
   LPD3DXBUFFER pBufferErrors = NULL;
   LPD3DXEFFECT pDummyEffect  = NULL;
   HRESULT hr;

   hr = D3DXCreateEffectFromFile(m_pDevice, TEXT("effects\\Spotlight.fx"), NULL, NULL, 0, NULL, &pDummyEffect, &pBufferErrors );

   // Report failed recreation of effect
   if (FAILED (hr))
   {
      OutputDebugString ("Spotlight.fx Effect Refresh Error");
      OutputDebugString ( (char *) pBufferErrors->GetBufferPointer());
   }
   else // effect was recreated successfully so use it
   {
      SAFE_RELEASE(m_pFrustumEffect);

      m_pFrustumEffect = pDummyEffect;
   }

   return S_OK;
}

//----------------------------------------------------------------------------
// Draw the light and/or frustum
//----------------------------------------------------------------------------
HRESULT CSpotlight::Draw(DWORD dwFlags)
{
   HRESULT hr;
   UINT iPass, cPasses;
   D3DXVECTOR4 Leye;
   D3DXMATRIX  matFrustumWorld, matFrustumWorldView, matITFrustumWorldView, matFrustumWorldViewProj;

   // Set up proper frustum transformations
   D3DXMatrixInverse (&matFrustumWorld, NULL, &m_matView);
   D3DXMatrixMultiply (&matFrustumWorldViewProj, &matFrustumWorld, &g_matWorldViewProj);
   D3DXMatrixMultiply (&matFrustumWorldView, &matFrustumWorld, &g_matWorldView);
   D3DXMatrixMultiply (&matITFrustumWorldView, &matFrustumWorld, &g_matITWorldView);

   m_pFrustumEffect->SetMatrixTranspose("matWorldViewProj", &matFrustumWorldViewProj);
   m_pFrustumEffect->SetMatrixTranspose("matWorldView",     &matFrustumWorldView);
   m_pFrustumEffect->SetMatrixTranspose("matITWorldView",   &matITFrustumWorldView);

   // Transform light position into eye space and pass to effect
   D3DXVec3Transform (&Leye, &m_vSceneLightPos, &g_matWorldView);
   m_pFrustumEffect->SetVector("g_Leye",  &Leye);
   m_pFrustumEffect->SetVector("lightColor",  &m_vLightColor);

   // Set the cookie texture
   m_pFrustumEffect->SetTexture("tCookie",  m_pCookie);
   

   if (dwFlags & SPOTLIGHT_SHOW_LIGHT)
   {
      //
      // Draw the cookie capping the frustum
      //
      m_pFrustumEffect->SetTechnique (m_pFrustumEffect->GetTechniqueByName ("frustum_front_technique"));

      m_pFrustumEffect->Begin(&cPasses, 0);

      for (iPass = 0; iPass < cPasses; iPass++)
      {
         m_pFrustumEffect->Pass(iPass);

         if (FAILED (hr = m_pDevice->SetFVF(FrustumVertex::FVF)))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbFrustumVertices, 0, sizeof(FrustumVertex))))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->SetIndices(m_pibFrustumIndices)))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 12, 0, m_uFrustumVertices, 0, 2)))
         {
            return hr;
         }
      }

      m_pFrustumEffect->End();


      //
      // Draw the body of the frustum
      //
      m_pFrustumEffect->SetTechnique (m_pFrustumEffect->GetTechniqueByName ("frustum_technique"));

      m_pFrustumEffect->Begin(&cPasses, 0);

      for (iPass = 0; iPass < cPasses; iPass++)
      {
         m_pFrustumEffect->Pass(iPass);

         if (FAILED (hr = m_pDevice->SetFVF(FrustumVertex::FVF)))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbFrustumVertices, 0, sizeof(FrustumVertex))))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->SetIndices(m_pibFrustumIndices)))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uFrustumVertices, 0, 4)))
         {
            return hr;
         }
      }

      m_pFrustumEffect->End();
   }

   if (dwFlags & SPOTLIGHT_SHOW_CLIPPING_FRUSTUM)
   {
      //
      // Draw the wireframe frustum
      //
      m_pFrustumEffect->SetTechnique (m_pFrustumEffect->GetTechniqueByName ("wire_frustum_technique"));

      m_pFrustumEffect->Begin(&cPasses, 0);

      for (iPass = 0; iPass < cPasses; iPass++)
      {
         m_pFrustumEffect->Pass(iPass);

         if (FAILED (hr = m_pDevice->SetFVF(WireFrustumVertex::FVF)))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->SetStreamSource(0, m_pvbWireFrustumVertices, 0, sizeof(WireFrustumVertex))))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->SetIndices(m_pibWireFrustumIndices)))
         {
            return hr;
         }

         if (FAILED (hr = m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 8, 0, 12)))
         {
            return hr;
         }
      }

      m_pFrustumEffect->End();
   }

   return S_OK;
}

