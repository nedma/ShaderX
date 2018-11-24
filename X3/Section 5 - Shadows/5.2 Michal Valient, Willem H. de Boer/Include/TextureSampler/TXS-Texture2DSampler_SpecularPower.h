#pragma once
#include "txs-texture2dsampler.h"

///Creates complete shininess power texture
///in form f(x,y) = (x,y, pow(x,sy), pow(x,sy))
///where sy = MinPower + y * (MaxPower - MinPower)
class CTexture2DSampler_SpecularPower : public CTexture2DSampler {
public:
    float   MinPower;
    float   MaxPower;

    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_SpecularPower(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_SpecularPower(void);

    MAKE_CLSCREATE(CTexture2DSampler_SpecularPower);
    MAKE_CLSNAME("CTexture2DSampler_SpecularPower");
    MAKE_DEFAULTGARBAGE()
};
