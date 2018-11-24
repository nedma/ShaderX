#pragma once
#include "txs-texture2dsampler.h"

///Creates other part of Cook-Torrance lighthing - Fresnel term divided by (n.v) and Pi
///CTF(x,y) = F(x, y) / (x * Pi)
///where x is assumed to be dp3(n,v) and y = index of refraction
///result is in form f(x,y) = (CTF1, CTF2, CTF3, CTF4) where
///
///and CTF1 is CTF in range 0..1
///    CTF2 is CTF in range 1..2
///    CTF3 is CTF in range 2..3
///    CTF4 is CTF in range 3..4
class CTexture2DSampler_Rcp : public CTexture2DSampler {
protected:
public:
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_Rcp(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_Rcp(void);

    MAKE_CLSCREATE(CTexture2DSampler_Rcp);
    MAKE_CLSNAME("CTexture2DSampler_Rcp");
    MAKE_DEFAULTGARBAGE()
};
