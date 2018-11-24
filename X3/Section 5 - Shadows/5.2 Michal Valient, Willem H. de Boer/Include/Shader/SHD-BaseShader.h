#pragma once

#include "nommgr.h"
#include "d3d9.h"
#include "mmgr.h"

#include "eng-baseresource.h"

class CShaderManager;

class CBaseShader : public CBaseResource {
protected:
    ///Shader specific flags
    DWORD ShaderFlags;

    virtual ~CBaseShader(void);
public:
    static const DWORD Shader_Flag_Set         = 1;

    ///returns IsSet state
    ///If IsSet is true, shader is set as current
    D3_INLINE bool GetIsSet() { 
        return (ShaderFlags & Shader_Flag_Set) ? true : false;
    }
    ///Sets IsSetStatus.
    D3_INLINE void SetIsSet(bool Status) {
        if (Status) ShaderFlags = ShaderFlags | Shader_Flag_Set;
        else ShaderFlags = ShaderFlags & (~Shader_Flag_Set);
    }

    ///Set this sader as current
    virtual bool Set(CShaderManager *ShaderManager, bool VertexShader, CRenderSettings &Settings) = 0;
    
    ///Const, dest & more
    CBaseShader(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSNAME("CBaseShader");
};
