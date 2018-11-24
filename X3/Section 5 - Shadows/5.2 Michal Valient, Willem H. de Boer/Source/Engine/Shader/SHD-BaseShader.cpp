#include "stdafx.h"

#include "shader\shd-baseshader.h"

CBaseShader::CBaseShader(char *ObjName,CEngine *OwnerEngine) : CBaseResource(ObjName,OwnerEngine) {
    ShaderFlags = 0;
}

CBaseShader::~CBaseShader(void) {
}
