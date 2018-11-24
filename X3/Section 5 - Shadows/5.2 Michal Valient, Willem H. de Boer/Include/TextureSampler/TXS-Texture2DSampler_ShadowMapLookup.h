#pragma once
#include "txs-texture2dsampler.h"

///Creates shadow map lookup texture
class CTexture2DSampler_ShadowMapLookup : public CTexture2DSampler {
public:
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_ShadowMapLookup(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_ShadowMapLookup(void);

    MAKE_CLSCREATE(CTexture2DSampler_ShadowMapLookup);
    MAKE_CLSNAME("CTexture2DSampler_ShadowMapLookup");
    MAKE_DEFAULTGARBAGE()
};
