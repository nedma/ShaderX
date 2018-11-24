#pragma once

#include "shader\shd-baseshader.h"

class CVertexShader : public CBaseShader {
protected:
    bool                    TryLoadBinaryFirst;     ///try to load binary version first (default is false)

    IDirect3DVertexShader9   *Shader;                ///Shader object

    virtual ~CVertexShader(void);                    ///destroyer
    ///load binary version of shader
    IDirect3DVertexShader9 *Load_Bin(void *Buffer, int BufferSize, CRenderSettings &RenderSettings);
    ///load and compile source version of shader
    IDirect3DVertexShader9 *Load_Src(void *Buffer, int BufferSize, CRenderSettings &RenderSettings);
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

    CVertexShader(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CVertexShader);
    MAKE_CLSNAME("CVertexShader");
    MAKE_DEFAULTGARBAGE()
};
