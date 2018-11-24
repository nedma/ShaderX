#pragma once
#include "txs-texture2dsampler.h"

///This object generates the circular noise pattern coordinates in X,Y and
///standard noise in Z,W.
///
///Circle noise is generated in ranges (-1,1) standard noise in 0-1
///
///TODO: Try to generate cache friendly noise 
///(i.e. sort texels into some sort of buckets so this fits into the cache of the GPU)!
class CTexture2DSampler_CircularNoise : public CTexture2DSampler {
protected:
    bool bSigned;

public:
    D3DXVECTOR4 vRandoms[256][256];
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    //Fill and save texture to file in parameter (as .dds)
    virtual void FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels);

    CTexture2DSampler_CircularNoise(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_CircularNoise(void);

    MAKE_CLSCREATE(CTexture2DSampler_CircularNoise);
    MAKE_CLSNAME("CTexture2DSampler_CircularNoise");
    MAKE_DEFAULTGARBAGE()
};
