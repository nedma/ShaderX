#include "stdafx.h"
#include "texturesampler\TXS-Texture2DSampler_Rcp.h"

///Override this one
void CTexture2DSampler_Rcp::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    //double Math_Pi = 3.141592653589793238462643383279502884197169399375105820974944592;
    double Math_Pi = 1.0;

    double PositionX = TexCoord.x + TexelSize.x;
    PositionX = min(1.0, max(0.0, PositionX));
    double PositionY = TexCoord.y + TexelSize.y;
    PositionY = min(1.0, max(0.0, PositionY));

    double Fresnel = PositionY + (1.0 - PositionY)*pow(1.0 - PositionX, 5.0);
    double Result = Fresnel / (PositionX * Math_Pi);
    if (1) {    //if we want it expanded to 0-8 range
        float RealResult1 = min(1.0f, max(0.0f, (float)Result));
        Result = Result - 1.0f;
        float RealResult2 = min(1.0f, max(0.0f, (float)Result / 1.5f));
        Result = Result - 1.5f;
        float RealResult3 = min(1.0f, max(0.0f, (float)Result / 2.0f));
        Result = Result - 2.0f;
        float RealResult4 = min(1.0f, max(0.0f, (float)Result / 2.5f));
        Output = D3DXVECTOR4(RealResult1, RealResult2, RealResult3, RealResult4);
    } else {
        Output = D3DXVECTOR4((float)PositionX, (float)PositionY, (float)Result, (float)Result);
    }
}

CTexture2DSampler_Rcp::CTexture2DSampler_Rcp(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
}

CTexture2DSampler_Rcp::~CTexture2DSampler_Rcp(void) {
}
