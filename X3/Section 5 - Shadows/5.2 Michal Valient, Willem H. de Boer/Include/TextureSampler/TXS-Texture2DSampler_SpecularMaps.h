#pragma once
#include "txs-texture2dsampler.h"


///This class creates specular power lightmaps
///for one power one lightmap.
///if texture is 1D (Height == 1) sampling f(x,y) gives (x,pow(x,shi), pow(x,shi), pow(x,shi))
///if texture is 1D (Height > 1) sampling f(x,y) gives with enabled linear interpolation 
///    (x,y, pow(x,shi), pow(x,shi))
class CTexture2DSampler_SpecularMaps : public CTexture2DSampler {
public:
    UINT    MinPower;
    UINT    MaxPower;

    float   CurrentPower;

    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    ///Specify file without extension
    virtual void FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels);

    CTexture2DSampler_SpecularMaps(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_SpecularMaps(void);

    MAKE_CLSCREATE(CTexture2DSampler_SpecularMaps);
    MAKE_CLSNAME("CTexture2DSampler_SpecularMaps");
    MAKE_DEFAULTGARBAGE()
};
