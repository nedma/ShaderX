// ENG-Texture2D.cpp: implementation of the CTexture2D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-Texture2D.h"

#include "nommgr.h"
#include <D3dx9.h>
#include "mmgr.h"
#include "FIL-FileManager.h"
#include "MGR-DirectGraphicsManager.h"
#include "eng-enginedefaults.h"
#include "eng-rendersettings.h"
#include "ENG-Engine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture2D::CTexture2D(char *sObjName, CEngine *pOwnerEngine) : CBaseTexture(sObjName, pOwnerEngine) {
    Width = 0;
    Height = 0;
    MultiSampleType = D3DMULTISAMPLE_NONE;
    OverrideLoadParams = false;
    CalledCreateTexture = false;
}

void CTexture2D::ResetLoadParams() {
    OverrideLoadParams = false;
}

void CTexture2D::SetLoadParams(UINT Width, UINT Height, UINT MipLevels, 
                               DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, 
                               DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey) {
    OLP_Width = Width;
    OLP_Height = Height;
    OLP_MipLevels = MipLevels;
    OLP_Usage = Usage;
    OLP_Format = Format;
    OLP_Pool = Pool;
    OLP_Filter = Filter;
    OLP_MipFilter = MipFilter;
    OLP_ColorKey = ColorKey;
    OverrideLoadParams = true;
}

bool CTexture2D::CreateTexture(CRenderSettings &RenderSettings, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, 
                   D3DFORMAT Format, D3DPOOL Pool) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Creating texture");

    LPDIRECT3DTEXTURE9  Txt;
    PALETTEENTRY palette[256];

    HRESULT res = D3DXCreateTexture(RenderSettings.Direct3DDevice, Width, Height, MipLevels, Usage, Format, Pool, &Txt);
    if (res != D3D_OK) {
         _WARN(this, ErrMgr->TNoSupport,"Ending with failure - Texture not created. D3DXCreateTexture returned %i",res);
        return FALSE;
    }
    //First if not freed, we need to free current texture.
    if (Texture!=NULL) {
        ULONG   refcnt = Texture->Release();
        if (refcnt>0) _LOG(this,D3_DV_GRP2_LEV2,"Releasing current IDirect3DBaseTexture9 - WARNING - reference count is still %i",refcnt);
        else _LOG(this,D3_DV_GRP2_LEV2,"Releasing current IDirect3DBaseTexture9 - OK");
        Texture = NULL;
    }
    Texture = Txt;
    D3DSURFACE_DESC desc;
    res = Txt->GetLevelDesc(0,&desc);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport,"Ending with failure - IDirect3DTexture8's GetLevelDesc returned %i",res);
        Texture->Release();
        Texture = NULL;
        return FALSE;
    }
    this->LevelCount = Texture->GetLevelCount();
    this->Pool = desc.Pool;
    this->Usage = desc.Usage;
    this->Format = desc.Format;
    this->Width = desc.Width;
    this->Height = desc.Height;
    this->MultiSampleType = desc.MultiSampleType;

    SetRestorable(true);
    //copy a palette
    if (this->Palette!=NULL) MemMgr->FreeMem(this->Palette);
    this->Palette = NULL;
    if (Format == D3DFMT_A8P8 || Format == D3DFMT_P8) { //texture needs palette
        this->Palette = (PALETTEENTRY *)MemMgr->AllocMem(sizeof(PALETTEENTRY) * 256, 0);
        memcpy(this->Palette, palette, sizeof(PALETTEENTRY) * 256);
    }
    this->LoadIndex = 0;
    this->CalledCreateTexture = true;

    _LOGE(logE);
    return true;
}

bool CTexture2D::LoadTexture(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Loading texture from file resource %i",LoadIndex);
    CFileManager *fil = OwnerEngine->GetFileManager();
    int ResSize = fil->GetSize(LoadIndex);
    if (ResSize<0) {
        _WARN(this, ErrMgr->TFile,"Ending with failure - resource not found");
        return FALSE;
    }
    char *ResBuffer = (char *)MemMgr->AllocMem(ResSize,0);
    int res = fil->Load(LoadIndex,ResBuffer);
    if (res<0) {
         MemMgr->FreeMem(ResBuffer);
        _WARN(this, ErrMgr->TFile,"Ending with failure - resource not loaded");
        return FALSE;
    }
    _LOGE(logE);
    logE = _LOGB(this,D3_DV_GRP2_LEV2,"Assigning resource %i to texture TextureID %i",LoadIndex,GetResourceID());
    LPDIRECT3DTEXTURE9  Txt;
    PALETTEENTRY palette[256];

    CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();

    if (!OverrideLoadParams) {
        OLP_Width = engDef->Txt2D_Width;
        OLP_Height = engDef->Txt2D_Height;
        OLP_MipLevels = engDef->Txt2D_MipLevels;
        OLP_Usage = engDef->Txt2D_Usage;
        OLP_Format = engDef->Txt2D_Format;
        OLP_Pool = engDef->Txt2D_Pool;
        OLP_Filter = engDef->Txt2D_Filter;
        OLP_MipFilter = engDef->Txt2D_MipFilter;
        OLP_ColorKey = engDef->Txt2D_ColorKey;
    }

    res = D3DXCreateTextureFromFileInMemoryEx(OwnerEngine->GetGraphicsManager()->Direct3DDevice, 
        ResBuffer, ResSize, OLP_Width, OLP_Height, OLP_MipLevels, OLP_Usage, OLP_Format, 
        OLP_Pool, OLP_Filter, OLP_MipFilter, OLP_ColorKey, NULL, palette, &Txt);
    if (res != D3D_OK) {
         MemMgr->FreeMem(ResBuffer);
         _WARN(this, ErrMgr->TNoSupport,"Ending with failure - Texture not created. D3DXCreateTextureFromFileInMemory returned %i",res);
        return FALSE;
    }
    //First if not freed, we need to free current texture.
    if (Texture!=NULL) {
        ULONG   refcnt = Texture->Release();
        if (refcnt>0) _LOG(this,D3_DV_GRP2_LEV2,"Releasing current IDirect3DBaseTexture9 - WARNING - reference count is still %i",refcnt);
        else _LOG(this,D3_DV_GRP2_LEV2,"Releasing current IDirect3DBaseTexture9 - OK");
        Texture = NULL;
    }
    Texture = Txt;
    D3DSURFACE_DESC desc;
    res = Txt->GetLevelDesc(0,&desc);
    if (res!=D3D_OK) {
        _WARN(this, ErrMgr->TNoSupport,"Ending with failure - IDirect3DTexture8's GetLevelDesc returnded %i",res);
        Texture->Release();
        MemMgr->FreeMem(ResBuffer);
        Texture = NULL;
        return FALSE;
    }
    LevelCount = Texture->GetLevelCount();
    Pool = desc.Pool;
    Usage = desc.Usage;
    Format = desc.Format;
    Width = desc.Width;
    Height = desc.Height;
    MultiSampleType = desc.MultiSampleType;

    SetRestorable(true);
    SetFilled(true);
    //copy a palette
    if (Palette!=NULL) MemMgr->FreeMem(Palette);
    Palette = NULL;
    if (Format == D3DFMT_A8P8 || Format == D3DFMT_P8) { //texture needs palette
        Palette = (PALETTEENTRY *)MemMgr->AllocMem(sizeof(PALETTEENTRY) * 256, 0);
        memcpy(Palette, palette, sizeof(PALETTEENTRY) * 256);
    }
    this->LoadIndex = LoadIndex;
    this->CalledCreateTexture = false;
    MemMgr->FreeMem(ResBuffer);
    _LOGE(logE,"resource %i assigned to Texture TextureID %i",LoadIndex, GetResourceID());
    return TRUE;
}

bool CTexture2D::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    if (CalledCreateTexture) {
        return CreateTexture(RenderSettings, Width, Height, LevelCount, Usage, Format, Pool);
    } else {
        return LoadTexture(LoadIndex,RenderSettings);
    }
}

bool CTexture2D::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

bool CTexture2D::ConvertToNormalMap(CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Creating normal map (resource %i)",LoadIndex);
    IDirect3DTexture9 *NormalMap = NULL;
    HRESULT dres = D3DXCreateTexture(RenderSettings.Direct3DDevice, this->Width, this->Height, D3DX_DEFAULT, this->Usage, D3DFMT_A8R8G8B8,this->Pool, &NormalMap);
    if(FAILED(dres)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot create normal map texture");
        return false;
    }
    dres = D3DXComputeNormalMap(NormalMap,(IDirect3DTexture9 *)this->Texture,NULL,0,D3DX_CHANNEL_RED,2);
    if(FAILED(dres)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot generate normal map texture");
        return false;
    }
    ULONG   refcnt = Texture->Release();
    if (refcnt>0) _WARN(this,ErrMgr->TMemory,"Releasing current IDirect3DBaseTexture9 - WARNING - reference count is still %i",refcnt);
    else _LOG(this,D3_DV_GRP2_LEV2,"Releasing current IDirect3DBaseTexture9 - OK");
    Texture = NormalMap;
    _LOGE(logE);
    return true;
}