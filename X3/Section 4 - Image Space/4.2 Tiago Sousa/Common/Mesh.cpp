///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : ShaderX 3
//  File : Mesh.cpp
//  Desc : Generic mesh class implementation
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mesh.h"
#include "Material.h"
#include "ModelLoader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

int CSubMesh::Create(int iMaterialID, int iCount, const ushort *pIndexList)
{
  //Make sure all data released, just in case client misuses
  Release();

  m_iMaterialID=iMaterialID;
  m_iFaceCount=iCount;

  m_pIB=new CIndexBuffer;
  if(FAILED(m_pIB->Create(m_iFaceCount*3, pIndexList)))
  {
    return APP_ERR_INITFAIL;
  }

  return APP_OK;
}

void CSubMesh::Release(void)
{
  m_iMaterialID=0;
  m_iFaceCount=0;
  SAFE_DELETE(m_pIB)
}

int CBaseMesh::Create(const char *pFile)
{
  // Make sure all data released, just in case client misuses
  Release();

  if(!pFile)
  {
    return APP_ERR_INVALIDPARAM;
  }

  // Get file extention
  char *pExtension = strrchr(pFile, '.' );
  if(pExtension) 
  {
    pExtension++;
  }
  else
  {
    return APP_ERR_INVALIDPARAM;
  }

  // Convert to lower case
  _strlwr(pExtension);

  // Check is file type is supported
  if(stricmp(pExtension,"tds")==0)
  {
    // set complete filename
    char  pFileName[256];
    sprintf(pFileName,"%s%s", APP_DATAPATH_MODELS, pFile);

    CModelLoader pModel;
    if(pModel.LoadFromFile(pFileName)==0)
    {
      return APP_ERR_READFAIL;
    }

    // Load materials
    if((m_iMaterialsCount=pModel.GetMaterialCount()))
    {
      // Create and copy material list
      m_pMaterialList=new CMaterial[m_iMaterialsCount];

      TSGMaterial *pMaterialList=pModel.GetMaterials();
      for(int m=0; m<m_iMaterialsCount; m++)
      {
        TSGMaterial *pTmpMat=&pMaterialList[m];

        // Copy material properties
        if(pTmpMat)
        {
          CMaterial *pMaterial=&m_pMaterialList[m];
          pMaterial->SetAmbient(CColor(pTmpMat->cAmbient[0], pTmpMat->cAmbient[1], pTmpMat->cAmbient[2], 1));
          pMaterial->SetDiffuse(CColor(pTmpMat->cDiffuse[0], pTmpMat->cDiffuse[1], pTmpMat->cDiffuse[2], 1));
          pMaterial->SetSpecular(CColor(pTmpMat->cSpecular[0], pTmpMat->cSpecular[1], pTmpMat->cSpecular[2], 1));
          pMaterial->SetEmissive(CColor(pTmpMat->cEmissive[0], pTmpMat->cEmissive[1], pTmpMat->cEmissive[2], 1));
          pMaterial->SetSpecularLevel(pTmpMat->fShininess);
          pMaterial->SetDoubleSided(0);
          pMaterial->SetOpacity(pTmpMat->fOpacity);          
          m_pMaterialList[m].SetDecalTex(pTmpMat->pTextureMap);                      
        }
      }
    }

    // Load groups
    if((m_iSubMeshCount=pModel.GetMeshCount()))
    {
      // Allocate mem for mesh sections
      m_pSubMeshList=new CSubMesh[m_iSubMeshCount];
                  
      // Save mesh data
      TSGMesh *pMeshList=pModel.GetMeshes();
      for(int s=0; s<m_iSubMeshCount; s++)
      {
        m_pSubMeshList[s].Create(pMeshList[s].wMaterialID, pMeshList[s].dwFaceCount, pMeshList[s].pIndices);        
      }
            
      // Create system memory vertex buffer (position + texture coordinates + normal) and copy data into it
      float *pVertexBuffer=new float[pModel.GetVertexCount()*8];

      TSGVertex *pVertexList=pModel.GetVertices();
      for(int v=0;v<pModel.GetVertexCount();v++)
      {
        memcpy(&pVertexBuffer[v*8], &pVertexList[v].vPosition, 3*sizeof(float));
        memcpy(&pVertexBuffer[v*8+3], &pVertexList[v].vTexCoord, 2*sizeof(float));
        memcpy(&pVertexBuffer[v*8+3+2], &pVertexList[v].vNormal, 3*sizeof(float));

        // Invert V coordinate
        pVertexBuffer[v*8+4]=1.0f-pVertexBuffer[v*8+4];
      }

      // Create mesh vertex buffer
      m_pVB=new CVertexBuffer;
      m_pVB->Create(pModel.GetVertexCount(), pVertexBuffer);

      SAFE_DELETE_ARRAY(pVertexBuffer)
    }
  }
  else 
  {
    OutputMsg("Error", "File format %s not supported", pExtension);
    return APP_ERR_NOTSUPPORTED;
  }

  return APP_OK;
}

void CBaseMesh::Release(void)
{  
  m_iMaterialsCount=0;        
  m_iSubMeshCount=0;
  SAFE_DELETE_ARRAY(m_pMaterialList)
  SAFE_DELETE_ARRAY(m_pSubMeshList)
  SAFE_DELETE(m_pVB)
}