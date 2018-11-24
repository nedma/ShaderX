#pragma once
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "baseclass.h"

///Texture sampler classes are used to create textures from functions
///i.e. Fresnel term or specular power term.
class CTexture2DSampler : public CBaseClass {
protected:
    static void WINAPI TexFill(D3DXVECTOR4* pOut, CONST D3DXVECTOR2* pTexCoord, CONST D3DXVECTOR2* pTexelSize, LPVOID pData);

    UINT    Width;
    UINT    Height;
    UINT    MipLevels;

    IDirect3DTexture9 *FilledTexture;
public:
    ///Override this function in descendants to get results you want.
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    //Fill and save texture to file in parameter (as .dds)
    virtual void FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels);
    
    //Fill texture specified as parameter
    virtual void FillTexture(IDirect3DTexture9 *Texture);

    CTexture2DSampler(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler(void);

    MAKE_CLSCREATE(CTexture2DSampler);
    MAKE_CLSNAME("CTexture2DSampler");
    MAKE_DEFAULTGARBAGE()
};
