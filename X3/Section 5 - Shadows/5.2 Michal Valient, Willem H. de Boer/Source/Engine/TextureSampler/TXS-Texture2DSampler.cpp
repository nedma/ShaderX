#include "stdafx.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-Engine.h"
#include "TextureSampler\txs-texture2dsampler.h"

///Override this one
void CTexture2DSampler::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    Output = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
}

CTexture2DSampler::CTexture2DSampler(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
    _LOG(this, D3_DV_GRP0_LEV0, "Texture sampler created");
}

CTexture2DSampler::~CTexture2DSampler(void) {
    _LOG(this, D3_DV_GRP0_LEV0, "Texture sampler destroyed");
}

void WINAPI CTexture2DSampler::TexFill(D3DXVECTOR4* pOut, CONST D3DXVECTOR2* pTexCoord, CONST D3DXVECTOR2* pTexelSize, LPVOID pData) {
    CTexture2DSampler *Sampler = (CTexture2DSampler *)pData;
    Sampler->FillTexel(*pOut, *pTexCoord, *pTexelSize);
}

void CTexture2DSampler::FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels) {
    int LogE = _LOGB(this, D3_DV_GRP0_LEV0, "Starting sample for %s", TextureFile);
    IDirect3DDevice9 *Device = this->OwnerEngine->GetGraphicsManager()->Direct3DDevice;
    IDirect3DTexture9 *Texture = NULL;
    HRESULT res = D3DXCreateTexture(Device, Width, Height, MipLevels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &Texture);
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

void CTexture2DSampler::FillTexture(IDirect3DTexture9 *Texture) {
    int LogE = _LOGB(this, D3_DV_GRP0_LEV0, "Starting sample for %p", Texture);

    MipLevels = Texture->GetLevelCount();
    D3DSURFACE_DESC levDesc;
    Texture->GetLevelDesc(0,&levDesc);
    Width = levDesc.Width;
    Height = levDesc.Height;

    FilledTexture = Texture;

    HRESULT res = D3DXFillTexture(Texture, TexFill, this);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport, "D3DXFillTexture failed to fill texture");
    }
    _LOGE(LogE, "Texture sampling ended");
}
