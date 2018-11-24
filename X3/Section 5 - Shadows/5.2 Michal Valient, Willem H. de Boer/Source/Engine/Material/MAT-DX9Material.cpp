#include "stdafx.h"
#include "material\mat-dx9material.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "FIL-FileManager.h"
#include "SC-MString.h"
#include "sc-xmlparser.h"
#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"

#include "nommgr.h"
#include "atlcomcli.h"
#include "mmgr.h"

CDX9Material::CDX9Material(char *ObjName,CEngine *OwnerEngine) : CBaseMaterial(ObjName,OwnerEngine) {
}

CDX9Material::~CDX9Material(void) {
}

HRESULT CDX9Material::LoadFromSource(const char *pszFileName, CRenderSettings &RenderSettings, ID3DXEffect **pOut) 
{
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating Vertex shader from source code '%s'", pszFileName);

    CMString szName = pszFileName;
    CFileManager *Fil = OwnerEngine->GetFileManager();
    int ShaderIndex = Fil->Find(szName);
    int ResourceSize = Fil->GetSize(ShaderIndex);
    char *Buffer = (char *)MemMgr->AllocMem(ResourceSize,0);
    if (Fil->Load(ShaderIndex, Buffer)<0) {
        _WARN(this,ErrMgr->TFile, "Cannot load pixel shader from resource - %i", ShaderIndex);
    }

    CComPtr<ID3DXEffect> pEffect;
    CComPtr<ID3DXBuffer> ErrorBuffer;

    HRESULT rc = D3DXCreateEffect(
        RenderSettings.Direct3DDevice, 
        Buffer, 
        ResourceSize, 
        NULL, 
        NULL, 
        0, 
        NULL, 
        &pEffect, 
        &ErrorBuffer);
    if (FAILED(rc)) {
        _LOG(this, D3_DV_GRP2_LEV0, "Cannot assemble effect - %s",ErrorBuffer->GetBufferPointer());
        pEffect.Release();
    }
    pEffect.CopyTo(pOut);
    MemMgr->FreeMem(Buffer);
    SetFilled(true);
    SetRestorable(true);

    _LOGE(logE);
    return rc;
}

HRESULT CDX9Material::LoadFromBuffer(char *pszBuffer, DWORD dwSize, CRenderSettings &RenderSettings, ID3DXEffect **pOut) 
{
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating Vertex shader from source buffer");

    int ResourceSize = dwSize;
    char *Buffer = pszBuffer;

    CComPtr<ID3DXEffect> pEffect;
    CComPtr<ID3DXBuffer> ErrorBuffer;

    HRESULT rc = D3DXCreateEffect(
        RenderSettings.Direct3DDevice, 
        Buffer, 
        ResourceSize, 
        NULL, 
        NULL, 
        0, 
        NULL, 
        &pEffect, 
        &ErrorBuffer);
    if (FAILED(rc)) {
        _LOG(this, D3_DV_GRP2_LEV0, "Cannot assemble effect - %s",ErrorBuffer->GetBufferPointer());
        pEffect.Release();
    }
    pEffect.CopyTo(pOut);
    SetFilled(true);
    SetRestorable(true);

    _LOGE(logE);
    return rc;
}
