#include "stdafx.h"
#include "texturesampler\txs-texture2dsampler_ShadowMapLookup.h"

///Override this one
void CTexture2DSampler_ShadowMapLookup::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    D3DXVECTOR4 tmpVal(0,0,0,0);


    int i = 0;
    if (TexCoord.x>=0.25)
    {
        i = 1;
        tmpVal.x = 1.0f;
        tmpVal.y = 0.0f;
    }
    if (TexCoord.x>=0.5)
    {
        i = 2;
        tmpVal.x = 0.0f;
        tmpVal.y = 1.0f;
    }
    if (TexCoord.x>=0.75)
    {
        i = 3;
        tmpVal.x = 1.0f;
        tmpVal.y = 1.0f;
    }

    int j = 0;
    if (TexCoord.y>=0.25)
    {
        j = 1;
        tmpVal.z = 1.0f;
        tmpVal.w = 0.0f;
    }
    if (TexCoord.y>=0.5)
    {
        j = 2;
        tmpVal.z = 0.0f;
        tmpVal.w = 1.0f;
    }
    if (TexCoord.y>=0.75)
    {
        j = 3;
        tmpVal.z = 1.0f;
        tmpVal.w = 1.0f;
    }

    double In_x = (TexCoord.x - (((double)i) * ((double)0.25))) * 4.0;
    double In_y = (TexCoord.y - (((double)j) * ((double)0.25))) * 4.0;

    float Out_x = 1.0f;

    if      (In_x<0.5 && In_y<0.5)
        Out_x = tmpVal.x;
    else if (In_x>=0.5 && In_y<0.5)
        Out_x = tmpVal.y;
    else if (In_x<0.5 && In_y>=0.5)
        Out_x = tmpVal.z;
    else
        Out_x = tmpVal.w;

    float Out_y = ((i+j)% 2);

    Output = D3DXVECTOR4(Out_x, Out_y, 0, 1);
}

CTexture2DSampler_ShadowMapLookup::CTexture2DSampler_ShadowMapLookup(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
}

CTexture2DSampler_ShadowMapLookup::~CTexture2DSampler_ShadowMapLookup(void) {
}
