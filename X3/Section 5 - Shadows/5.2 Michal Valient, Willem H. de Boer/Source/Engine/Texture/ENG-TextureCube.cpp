// ENG-Texture2D.cpp: implementation of the CTextureCube class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-TextureCube.h"

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

CTextureCube::CTextureCube(char *sObjName, CEngine *pOwnerEngine) : CBaseTexture(sObjName, pOwnerEngine) {
    Size = 0;
    MultiSampleType = D3DMULTISAMPLE_NONE;
    OverrideLoadParams = false;
}

void CTextureCube::ResetLoadParams() {
    OverrideLoadParams = false;
}

void CTextureCube::SetLoadParams(UINT Size, UINT MipLevels, 
                               DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, 
                               DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey) {
    OLP_Size = Size;
    OLP_MipLevels = MipLevels;
    OLP_Usage = Usage;
    OLP_Format = Format;
    OLP_Pool = Pool;
    OLP_Filter = Filter;
    OLP_MipFilter = MipFilter;
    OLP_ColorKey = ColorKey;
    OverrideLoadParams = true;
}


bool CTextureCube::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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
    LPDIRECT3DCUBETEXTURE9  Txt;
    PALETTEENTRY palette[256];

    CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();

    if (!OverrideLoadParams) {
        OLP_Size = engDef->TxtCube_Size;
        OLP_MipLevels = engDef->TxtCube_MipLevels;
        OLP_Usage = engDef->TxtCube_Usage;
        OLP_Format = engDef->TxtCube_Format;
        OLP_Pool = engDef->TxtCube_Pool;
        OLP_Filter = engDef->TxtCube_Filter;
        OLP_MipFilter = engDef->TxtCube_MipFilter;
        OLP_ColorKey = engDef->TxtCube_ColorKey;
    }

    res = D3DXCreateCubeTextureFromFileInMemoryEx(OwnerEngine->GetGraphicsManager()->Direct3DDevice, 
        ResBuffer, ResSize, OLP_Size, OLP_MipLevels, OLP_Usage, OLP_Format, 
        OLP_Pool, OLP_Filter, OLP_MipFilter, OLP_ColorKey, NULL, palette, &Txt);
    if (res != D3D_OK) {
         MemMgr->FreeMem(ResBuffer);
         _WARN(this, ErrMgr->TNoSupport,"Ending with failure - Texture not created. D3DXCreateCubeTextureFromFileInMemoryEx returned %i",res);
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
        _WARN(this, ErrMgr->TNoSupport,"Ending with failure - IDirect3DCubeTexture8's GetLevelDesc returnded %i",res);
        Texture->Release();
        MemMgr->FreeMem(ResBuffer);
        Texture = NULL;
        return FALSE;
    }
    LevelCount = Texture->GetLevelCount();
    Pool = desc.Pool;
    Usage = desc.Usage;
    Format = desc.Format;
    Size = desc.Width;
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
    MemMgr->FreeMem(ResBuffer);
    _LOGE(logE,"resource %i assigned to Texture TextureID %i",LoadIndex, GetResourceID());
    return TRUE;
}

bool CTextureCube::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}
