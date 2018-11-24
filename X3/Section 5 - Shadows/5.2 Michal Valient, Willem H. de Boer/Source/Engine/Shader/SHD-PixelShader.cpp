#include "stdafx.h"

#include "shader\shd-pixelshader.h"
#include "Shader\SHD-ShaderManager.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "FIL-FileManager.h"

CPixelShader::CPixelShader(char *ObjName,CEngine *OwnerEngine) : CBaseShader(ObjName,OwnerEngine) {
    Shader = NULL;
    TryLoadBinaryFirst = false;
}

CPixelShader::~CPixelShader(void) {
    if (Shader) {
        UINT refCount = Shader->Release();
        if (refCount) {
            _WARN(this, ErrMgr->TMemory, "Shader %p being released, but reference count is still %u", Shader, refCount);
        }
    }
}

bool CPixelShader::Set(CShaderManager *ShaderManager, bool VertexShader, CRenderSettings &Settings) {
    if (VertexShader) {
        _WARN(this, ErrMgr->TNoSupport, "Pixel shader cannot be set to vertex pipeline");
        return false;
    } else {
        HRESULT res = Settings.Direct3DDevice->SetPixelShader(Shader);
        if (res == D3D_OK) {
            return true;
        } else {
            _WARN(this, ErrMgr->TSystem, "Pixel shader cannot be set - error %i", res);
            return false;
        }
    }
}

bool CPixelShader::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading pixel shader from resource %i", ResourceID);
    bool WasBinary = false;
    CFileManager *Fil = OwnerEngine->GetFileManager();
    int ResourceSize = Fil->GetSize(ResourceID);
    char *Buffer = (char *)MemMgr->AllocMem(ResourceSize,0);
    if (Fil->Load(ResourceID, Buffer)<0) {
        _WARN(this,ErrMgr->TFile, "Cannot load pixel shader from resource - %i", ResourceID);
    }
    if (TryLoadBinaryFirst) {
        Shader = Load_Bin(Buffer, ResourceSize, RenderSettings);
        if (Shader == NULL) Shader = Load_Src(Buffer, ResourceSize, RenderSettings);
        else {
            WasBinary = true;
        }
        if (Shader == NULL) {
            _WARN(this, ErrMgr->TNoSupport, "Cannot compile pixel shader");
        }
    } else {
        Shader = Load_Src(Buffer, ResourceSize, RenderSettings);
        if (Shader == NULL) Shader = Load_Bin(Buffer, ResourceSize, RenderSettings);
        if (Shader == NULL) {
            _WARN(this, ErrMgr->TNoSupport, "Cannot compile pixel shader");
        } else {
            WasBinary = true;
        }
    }
    MemMgr->FreeMem(Buffer);
    this->LoadIndex = LoadIndex;
    this->TryLoadBinaryFirst = WasBinary;
    SetFilled(true);
    SetRestorable(true);
    _LOGE(logE);
    return true;
}

bool CPixelShader::Update(CRenderSettings &RenderSettings) {
    return true;
}

void CPixelShader::DeviceLost(CRenderSettings &Settings) {
    this->SetLost(false);
}

void CPixelShader::DoCacheOut(CRenderSettings &RenderSettings) {
    if (Shader) {
        UINT refCount = Shader->Release();
        if (refCount) {
            _WARN(this, ErrMgr->TMemory, "Shader %p being released, but reference count is still %u", Shader, refCount);
        }
    }
    this->SetFilled(false);
}

IDirect3DPixelShader9 *CPixelShader::Load_Bin(void *Buffer, int BufferSize, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating binary pixel shader");
    IDirect3DPixelShader9 *OutputShader = NULL;
    // Create the pixel shader
	HRESULT rc = RenderSettings.Direct3DDevice->CreatePixelShader((DWORD *)Buffer, &OutputShader);
	if (FAILED(rc)) {
        _LOGE(logE, "Failed");
        OutputShader = NULL;
    } else {
        _LOGE(logE);
    }
    return OutputShader;
}

IDirect3DPixelShader9 *CPixelShader::Load_Src(void *Buffer, int BufferSize, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating pixel shader from source code");
    IDirect3DPixelShader9 *OutputShader = NULL;
    ID3DXBuffer *ShaderBuffer = NULL;
    ID3DXBuffer *ErrorBuffer = NULL;
    HRESULT rc = D3DXAssembleShader((char *)Buffer, 
                                    BufferSize, 
                                    NULL, 
                                    NULL, 
                                    D3DXSHADER_DEBUG, 
                                    &ShaderBuffer, 
                                    &ErrorBuffer);
    if (FAILED(rc)) {
        _LOGE(logE, "Cannot assemble pixel shader - %s",ErrorBuffer->GetBufferPointer());
        OutputShader = NULL;
    } else {
        // Create the pixel shader
        rc = RenderSettings.Direct3DDevice->CreatePixelShader((DWORD *)ShaderBuffer->GetBufferPointer(), &OutputShader);
        if (FAILED(rc)) {
            _LOGE(logE, "Failed");
            OutputShader = NULL;
        } else {
            _LOGE(logE);
        }
    }
    if (ShaderBuffer) ShaderBuffer->Release();
    if (ErrorBuffer) ErrorBuffer->Release();
    return OutputShader;
}

