///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : ShaderX 3
//  File : Material.cpp
//  Desc : Simple material class
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Material.h"

int CMaterial::SetDecalTex(const char *pDecal)
{
  if(!pDecal)
  {
    return APP_ERR_INVALIDPARAM;
  }

  // Check if texture is same
  if(strcmp(pDecal, m_pTextureList.GetTextureFileName())==0)
  {
    return APP_OK;
  }
  m_pTextureList.Create(pDecal, 0);
  return APP_OK;
}