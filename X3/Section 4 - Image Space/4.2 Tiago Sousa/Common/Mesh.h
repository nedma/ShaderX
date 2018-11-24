///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : ShaderX 3
//  File : Mesh.h
//  Desc : Generic mesh class implementation
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class CMaterial;
class CVertexStream;
class CVertexBuffer;
class CIndexBuffer;

class CSubMesh
{
public:
  CSubMesh(): m_iMaterialID(0), m_iFaceCount(0), m_pIB(0)
  {

  }
  ~CSubMesh()
  {
    Release();
  }

  // Creators
  int Create(int iMaterialID, int iCount, const ushort *pIndexList);
  void Release(void);
  
  // Accessors
  int GetMaterialID(void) const
  {
    return m_iMaterialID;
  }

  int GetFaceCount(void) const
  {
    return m_iFaceCount;
  }

  const CIndexBuffer *GetIndexBuffer(void) const
  {
    return m_pIB;
  }

  // Manipulators
  CIndexBuffer *GetIndexBuffer(void)
  {
    return m_pIB;
  }

private:
  int m_iMaterialID, m_iFaceCount;
  CIndexBuffer *m_pIB;
};

class CBaseMesh
{
public:
  CBaseMesh(): m_iMaterialsCount(0), m_pMaterialList(0), m_pVB(0), m_pSubMeshList(0), m_iSubMeshCount(0)
  {

  }

  ~CBaseMesh()
  {
    Release();
  }

  // Load mesh from file
  int Create(const char *pFile);
  // Free resources
  void Release(void);

  // Return materials count
  int GetMaterialsCount(void) const
  {
    return m_iMaterialsCount;
  }  

  // Return mesh material
  const CMaterial *GetMaterialList(void) const
  {
    return m_pMaterialList;
  }

  // Accessors

  int GetSubMeshCount(void) const
  {
    return m_iSubMeshCount;
  }

  const CSubMesh *GetSubMeshes(void) const
  {
    return m_pSubMeshList;
  }

  const CVertexBuffer *GetVB(void) const
  {
    return m_pVB;
  }

  // Manipulators

  CSubMesh *GetSubMeshes(void)
  {
    return m_pSubMeshList;
  }

  CVertexBuffer *GetVB(void)
  {
    return m_pVB;
  }
  
private:
  
  int m_iMaterialsCount;
  CMaterial *m_pMaterialList;
  
  int m_iSubMeshCount;
  // Mesh is contained in a single vertex buffer
  // and Sub-meshes are divided among multiple index buffers
  CVertexBuffer *m_pVB;  
  CSubMesh *m_pSubMeshList;
};


