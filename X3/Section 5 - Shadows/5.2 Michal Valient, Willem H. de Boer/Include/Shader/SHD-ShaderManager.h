// ENG-MaterialManager.h: interface for the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ENG-ResourceManager.h"

//############################################################################
//Forward declarations
class CBaseShader;

class CShaderManager : public CResourceManager {
    CBaseShader *PixelShader;
    CBaseShader *VertexShader;
public:
    CShaderManager(char *ObjName, CEngine *OwnerEngine);
	virtual ~CShaderManager();

    virtual void DeviceLost(CRenderSettings &Settings);

    bool SetPixelShader(CBaseShader *PixelShader, CRenderSettings &Settings);
    bool SetVertexShader(CBaseShader *PixelShader, CRenderSettings &Settings);

    MAKE_CLSCREATE(CShaderManager);
    MAKE_CLSNAME("CShaderManager");
    MAKE_DEFAULTGARBAGE()
};

