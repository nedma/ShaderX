#pragma once

#include "shader\shd-baseshader.h"

class CPixelShader : public CBaseShader {
protected:
    bool                    TryLoadBinaryFirst;     ///try to load binary version first (default is false)

    IDirect3DPixelShader9   *Shader;                ///Shader object

    virtual ~CPixelShader(void);                    ///destroyer
    ///load binary version of shader
    IDirect3DPixelShader9 *Load_Bin(void *Buffer, int BufferSize, CRenderSettings &RenderSettings);
    ///load and compile source version of shader
    IDirect3DPixelShader9 *Load_Src(void *Buffer, int BufferSize, CRenderSettings &RenderSettings);
public:
    ///set value of TryLoadBinaryFirst
    void SetLoadBinaryFirst(bool TryLoadBinaryFirst) {
        this->TryLoadBinaryFirst = TryLoadBinaryFirst;
    }
    ///get value of TryLoadBinaryFirst
    bool GetLoadBinaryFirst() {
        return this->TryLoadBinaryFirst;
    }

    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);
    virtual void DeviceLost(CRenderSettings &Settings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    virtual bool Set(CShaderManager *ShaderManager, bool VertexShader, CRenderSettings &Settings);

    CPixelShader(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CPixelShader);
    MAKE_CLSNAME("CPixelShader");
    MAKE_DEFAULTGARBAGE()
};
