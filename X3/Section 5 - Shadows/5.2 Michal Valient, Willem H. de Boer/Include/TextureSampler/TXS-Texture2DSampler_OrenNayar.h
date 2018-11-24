#pragma once
#include "txs-texture2dsampler.h"

///Creates sin(A)*tan(B)
///where A = max(acos(x), acos(y)) and B = min(acos(x), acos(y))
///For part of Oren-Nayar lighting.
class CTexture2DSampler_OrenNayar : public CTexture2DSampler {
protected:
public:
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    //Fill and save texture to file in parameter (as .dds)
    virtual void FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels);

    CTexture2DSampler_OrenNayar(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_OrenNayar(void);

    MAKE_CLSCREATE(CTexture2DSampler_OrenNayar);
    MAKE_CLSNAME("CTexture2DSampler_OrenNayar");
    MAKE_DEFAULTGARBAGE()
};
