#pragma once
#include "txs-texture2dsampler.h"

///Creates fresnel lookup texture
///if texture is 1D (Height == 1) sampling f(x,y) gives (x,Fresnel(x), Fresnel(x), Fresnel(x))
///if texture is 1D (Height > 1) sampling f(x,y) gives with enabled linear interpolation 
///    (x,y, Fresnel(x), Fresnel(x))
class CTexture2DSampler_Fresnel : public CTexture2DSampler {
public:
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_Fresnel(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_Fresnel(void);

    MAKE_CLSCREATE(CTexture2DSampler_Fresnel);
    MAKE_CLSNAME("CTexture2DSampler_Fresnel");
    MAKE_DEFAULTGARBAGE()
};
