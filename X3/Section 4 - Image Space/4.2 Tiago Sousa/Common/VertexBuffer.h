///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : ShaderX 3
//  File : VertexBuffer.h
//  Desc : Simple vertex buffer class
//  Note:
//  - For demo simplicity all vertices share same properties (position, texture coordinates, and normal)
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class CVertexBuffer
{
public:
  CVertexBuffer(): m_iFormat(0), m_iSize(0), m_iCount(0), m_pVB(0)
  {
  };
  ~CVertexBuffer()
  {
    Release();
  };

  // Create a vertex buffer
  int Create(int iCount, const float *pVertexList);  
  // Release resources
  void Release(void);
  // Activates vertex buffer
  void Enable(void);

  // Manipulators
  IDirect3DVertexBuffer9 *Get(void)
  {
    return m_pVB; 
  }

  // Accessors
  const IDirect3DVertexBuffer9 *Get(void) const
  {
    return m_pVB; 
  }

  int GetFormat(void) const
  {
    return m_iFormat;
  }

  int GetSize(void) const
  {
    return m_iSize;
  }

  int GetCount(void) const
  {
    return m_iCount;
  }

private:

  // Vertex data
  int m_iFormat, m_iSize, m_iCount;
  IDirect3DVertexBuffer9 *m_pVB; 
};