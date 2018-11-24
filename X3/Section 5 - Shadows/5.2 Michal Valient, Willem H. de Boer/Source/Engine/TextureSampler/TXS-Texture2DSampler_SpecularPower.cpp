#include "stdafx.h"
#include "texturesampler\txs-texture2dsampler_specularpower.h"

///Override this one
void CTexture2DSampler_SpecularPower::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    float distance = MaxPower - MinPower;

    float shininess = MinPower + (TexCoord.y + TexelSize.y) * distance;
    Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, TexCoord.y + TexelSize.y, powf(TexCoord.x + TexelSize.x, shininess), powf(TexCoord.x + TexelSize.x, shininess));
}

CTexture2DSampler_SpecularPower::CTexture2DSampler_SpecularPower(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
    MinPower = 10.0f;
    MaxPower = 100.0f;
}

CTexture2DSampler_SpecularPower::~CTexture2DSampler_SpecularPower(void) {
}
