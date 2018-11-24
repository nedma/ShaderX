#include "stdafx.h"
#include "texturesampler\txs-texture2dsampler_fresnel.h"

///Override this one
void CTexture2DSampler_Fresnel::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    float FresnelForm = powf(1.0f - TexCoord.x - TexelSize.x, 5.0f);
    if (this->Height > 1) {
        Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, TexCoord.y + TexelSize.y, FresnelForm, FresnelForm);
    } else {
        Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, FresnelForm, FresnelForm, FresnelForm);
    }
}

CTexture2DSampler_Fresnel::CTexture2DSampler_Fresnel(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
}

CTexture2DSampler_Fresnel::~CTexture2DSampler_Fresnel(void) {
}
