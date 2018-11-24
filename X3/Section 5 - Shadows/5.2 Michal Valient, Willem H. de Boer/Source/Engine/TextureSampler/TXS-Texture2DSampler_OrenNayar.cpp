#include "stdafx.h"
#include "texturesampler\TXS-Texture2DSampler_OrenNayar.h"
#include "SC-MString.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-Engine.h"


///Override this one
void CTexture2DSampler_OrenNayar::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    double PositionX = TexCoord.x + TexelSize.x;
    PositionX = min(1.0, max(0.0, PositionX));
    double PositionY = TexCoord.y + TexelSize.y;
    PositionY = min(1.0, max(0.0, PositionY));

    //PositionX = PositionX * 2.0 - 1.0;  //from 0..1 to -1,1
    //PositionY = PositionY * 2.0 - 1.0;  //from 0..1 to -1,1

    double aLight = acos(PositionX);
    double aEye = acos(PositionY);

    double aMax = max(aLight, aEye);
    double aMin = min(aLight, aEye);

    double result = sin(aMax)*tan(aMin);

    if (1) {
        Output.x = float(result);
    } else {    //visualization
        if (result>=0) {
            Output.x = Output.y = Output.z = float(result) / 9.0f;
        } else {
            Output.x = Output.y = Output.z = Output.w = 0.0f; 
        }
    }
}

void CTexture2DSampler_OrenNayar::FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels) {
    int LogE = _LOGB(this, D3_DV_GRP0_LEV0, "Starting sample for %s", TextureFile);
    IDirect3DDevice9 *Device = this->OwnerEngine->GetGraphicsManager()->Direct3DDevice;
    IDirect3DTexture9 *Texture = NULL;
    HRESULT res = D3DXCreateTexture(Device, Width, Height, MipLevels, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &Texture);
    //HRESULT res = D3DXCreateTexture(Device, Width, Height, MipLevels, 0, D3DFMT_X8B8G8R8, D3DPOOL_SYSTEMMEM, &Texture);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport, "D3DXCreateTexture failed to create texture");
    } else {
        FillTexture(Texture);
        res = D3DXSaveTextureToFile(TextureFile, D3DXIFF_DDS, Texture, NULL);
        if (res!=D3D_OK) {
            _WARN(this, ErrMgr->TNoSupport, "D3DXSaveTextureToFile failed to save texture to %s", TextureFile);
        }
    }
    Texture->Release();
    _LOGE(LogE, "Texture sampling ended");
}

CTexture2DSampler_OrenNayar::CTexture2DSampler_OrenNayar(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
}

CTexture2DSampler_OrenNayar::~CTexture2DSampler_OrenNayar(void) {
}
