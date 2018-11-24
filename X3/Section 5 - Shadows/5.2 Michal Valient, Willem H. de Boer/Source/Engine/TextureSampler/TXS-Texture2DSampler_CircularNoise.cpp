#include "stdafx.h"
#include "texturesampler\TXS-Texture2DSampler_CircularNoise.h"
#include "SC-MString.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-Engine.h"

#include "nommgr.h"
#include "PoissonDisk.h"
#include <algorithm>
#include "mmgr.h"

/*
inline D3DXVECTOR4 NoiseMe() {
    bool bCanReturn = false;
    D3DXVECTOR4 retVal;
    while (!bCanReturn)
    {
        retVal.x = ((float)rand()) / ((float)RAND_MAX);
        retVal.y = ((float)rand()) / ((float)RAND_MAX);
        retVal.z = ((float)rand()) / ((float)RAND_MAX);
        retVal.w = ((float)rand()) / ((float)RAND_MAX);

        retVal.x = 2.0f * retVal.x - 1.0f;
        retVal.y = 2.0f * retVal.y - 1.0f;
        retVal.z = 2.0f * retVal.z - 1.0f;
        retVal.w = 2.0f * retVal.w - 1.0f;

        float c1 = retVal.x * retVal.x + retVal.y * retVal.y;
        float c2 = retVal.y * retVal.y + retVal.z * retVal.z;
        float c3 = retVal.z * retVal.z + retVal.x * retVal.x;
        float c4 = retVal.w * retVal.w + retVal.z * retVal.z;

        if ((c1<=1) && (c2<=1) && (c3<=1) && (c4<=1))
            bCanReturn = true;
    }
    (retVal * 0.5f) + D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.5f);
    return retVal;
}
*/

inline D3DXVECTOR4 NoiseMe() {
    bool bCanReturn = false;
    D3DXVECTOR4 retVal;
    retVal.z = ((float)rand()) / ((float)RAND_MAX);
    retVal.w = ((float)rand()) / ((float)RAND_MAX);
    while (!bCanReturn)
    {
        retVal.x = ((float)rand()) / ((float)RAND_MAX);
        retVal.y = ((float)rand()) / ((float)RAND_MAX);

        retVal.x = 2.0f * retVal.x - 1.0f;
        retVal.y = 2.0f * retVal.y - 1.0f;

        float c1 = retVal.x * retVal.x + retVal.y * retVal.y;

        if (c1<=1)
            bCanReturn = true;
    }
    return retVal;
}

///Override this one
void CTexture2DSampler_CircularNoise::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
//    Output = NoiseMe();
    float fI = TexCoord.x * 256.0f;
    float fJ = TexCoord.y * 256.0f;
    int i = fI;
    int j = fJ;
    Output = vRandoms[i][j];

    if (!bSigned) {
        Output.x = Output.x * 0.5f + 0.5f;
        Output.y = Output.y * 0.5f + 0.5f;
    }
}

void CTexture2DSampler_CircularNoise::FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels) {
    int LogE = _LOGB(this, D3_DV_GRP0_LEV0, "Starting sample for %s", TextureFile);

    //Fill the array
    for (int j=0; j<256; j++)
    {
//        if (j % 16 == 0)
//        {
//            printf("## New srand\n");
//            srand(GetTickCount());
//        }
        printf("## Row %u\n", j);
        vector<Point2> spVec;
        GenPMain(32, 64, spVec);
        printf("# 1 # ");
        random_shuffle(spVec.begin(), spVec.end());
        printf("# 2 # ");
        int cnt = 0;
        for (int i=0; i<256; i++)
        {
            vRandoms[i][j].x = spVec[cnt].u;
            vRandoms[i][j].y = spVec[cnt].v;
            vRandoms[i][j].z = ((float)rand()) / ((float)RAND_MAX);
            vRandoms[i][j].w = ((float)rand()) / ((float)RAND_MAX);
            cnt++;
            if (cnt >= spVec.size())
            {
//                random_shuffle(spVec.begin(), spVec.end());
                cnt = 0;
            }
        }
    }

    IDirect3DDevice9 *Device = this->OwnerEngine->GetGraphicsManager()->Direct3DDevice;
    IDirect3DTexture9 *Texture = NULL;
    HRESULT res = S_OK;
    res = D3DXCreateTexture(Device, Width, Height, MipLevels, 0, D3DFMT_A8B8G8R8, D3DPOOL_SYSTEMMEM, &Texture);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport, "D3DXCreateTexture failed to create texture");
    } else {
        bSigned = false;
        CMString TextureFile = "C:\\D3F_CircularNoise.dds";
        FillTexture(Texture);
        res = D3DXSaveTextureToFile(TextureFile.c_str(), D3DXIFF_DDS, Texture, NULL);
        if (res!=D3D_OK) {
            _WARN(this, ErrMgr->TNoSupport, "D3DXSaveTextureToFile failed to save texture to %s", TextureFile);
        }
    }
    Texture->Release();
    Texture = NULL;
    
    res = D3DXCreateTexture(Device, Width, Height, MipLevels, 0, D3DFMT_V16U16, D3DPOOL_SYSTEMMEM, &Texture);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport, "D3DXCreateTexture failed to create texture");
    } else {
        bSigned = true;
        CMString TextureFile = "C:\\D3F_CircularNoiseSigned.dds";
        FillTexture(Texture);
        res = D3DXSaveTextureToFile(TextureFile.c_str(), D3DXIFF_DDS, Texture, NULL);
        if (res!=D3D_OK) {
            _WARN(this, ErrMgr->TNoSupport, "D3DXSaveTextureToFile failed to save texture to %s", TextureFile);
        }
    }
    Texture->Release();
    Texture = NULL;

    //1D
    res = D3DXCreateTexture(Device, Width, 1, MipLevels, 0, D3DFMT_A8B8G8R8, D3DPOOL_SYSTEMMEM, &Texture);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport, "D3DXCreateTexture failed to create texture");
    } else {
        bSigned = false;
        CMString TextureFile = "C:\\D3F_CircularNoise1D.dds";
        FillTexture(Texture);
        res = D3DXSaveTextureToFile(TextureFile.c_str(), D3DXIFF_DDS, Texture, NULL);
        if (res!=D3D_OK) {
            _WARN(this, ErrMgr->TNoSupport, "D3DXSaveTextureToFile failed to save texture to %s", TextureFile);
        }
    }
    Texture->Release();
    Texture = NULL;

    res = D3DXCreateTexture(Device, Width, 1, MipLevels, 0, D3DFMT_V16U16, D3DPOOL_SYSTEMMEM, &Texture);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport, "D3DXCreateTexture failed to create texture");
    } else {
        bSigned = true;
        CMString TextureFile = "C:\\D3F_CircularNoiseSigned1D.dds";
        FillTexture(Texture);
        res = D3DXSaveTextureToFile(TextureFile.c_str(), D3DXIFF_DDS, Texture, NULL);
        if (res!=D3D_OK) {
            _WARN(this, ErrMgr->TNoSupport, "D3DXSaveTextureToFile failed to save texture to %s", TextureFile);
        }
    }
    Texture->Release();
    Texture = NULL;

    //Special include file for HLSL shader
    int fMaxI = 8;
    int fMaxJ = 8;
    FILE *pFile = fopen("C:\\D3F_PoissonDiskArray.hlsl.h", "wt");
    fprintf(pFile, "static float2 vPoisson[%u] = {\n", fMaxI * fMaxJ);
    for (int j=0; j<fMaxJ; j++)
    {
        fprintf(pFile, "    ");
        for (int i=0; i<fMaxI; i++)
        {
            fprintf(pFile, "{%.12f, %.12f}", vRandoms[i + j * fMaxI][0].x, vRandoms[i + j * fMaxI][0].y);
            if ((i!=fMaxI-1) && (j!=fMaxJ))
                fprintf(pFile, ", ");
        }
        fprintf(pFile, "\n");
    }
    fprintf(pFile, "};\n");
    fclose (pFile);
    _LOGE(LogE, "Texture sampling ended");
}

CTexture2DSampler_CircularNoise::CTexture2DSampler_CircularNoise(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
        srand(GetTickCount());
}

CTexture2DSampler_CircularNoise::~CTexture2DSampler_CircularNoise(void) {
}
