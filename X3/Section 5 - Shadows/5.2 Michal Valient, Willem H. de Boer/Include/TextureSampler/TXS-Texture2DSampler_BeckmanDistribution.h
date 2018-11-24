#pragma once
#include "txs-texture2dsampler.h"

///Creates beckman distribution texture for Cook-Tortance specular model
///in form f(x,y) = (D1, D2, D3, D4).
///where x is assumed to be cosine of an angle _A_
///and y is meant as _m_ from ...
///
///D(A, m) = (e^(-(tan A / m)^2)) / (m^2 * (cos A)^4)
///
///and D1 is D in range 0..1
///    D2 is D in range 1..2
///    D3 is D in range 2..3
///    D4 is D in range 3..4
class CTexture2DSampler_BeckmanDistribution : public CTexture2DSampler {
protected:
    double MinUsedRough;
    double MinUsedCos;
public:
    float   MinRoughness;
    float   MaxRoughness;

    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_BeckmanDistribution(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_BeckmanDistribution(void);

    MAKE_CLSCREATE(CTexture2DSampler_BeckmanDistribution);
    MAKE_CLSNAME("CTexture2DSampler_BeckmanDistribution");
    MAKE_DEFAULTGARBAGE()
};
