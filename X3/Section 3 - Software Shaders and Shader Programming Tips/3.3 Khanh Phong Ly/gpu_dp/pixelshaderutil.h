#ifndef __PIXELSHADERUTIL_H__
#define __PIXELSHADERUTIL_H__


#include <d3dx9.h>
#include <tchar.h>


HRESULT LoadShaderFile(LPDIRECT3DDEVICE9 pd3dDevice, char *filename, IDirect3DPixelShader9 ** shader);


//HRESULT	SetPixelShaderConstF_TEX_DXDY(LPDIRECT3DDEVICE9 pd3dDevice, float texWidth, float texHeight);
HRESULT SetPixelOffsetConstants(LPDIRECT3DDEVICE9 pd3dDevice, int imageWidth, int imageHeight);

#endif //__PIXELSHADERUTIL_H__