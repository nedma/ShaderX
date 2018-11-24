// ENG-MaterialManager.h: interface for the CMaterialManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ENG-ResourceManager.h"

//############################################################################
//Forward declarations
class CBaseMaterial;
class CBaseMaterialContext;

class CMaterialManager : public CResourceManager {
    CBaseMaterial           *CurrentMaterial;
    DWORD                   CurrentPass;
    CBaseMaterialContext    *CurrentContext;
public:
    CMaterialManager(char *ObjName, CEngine *OwnerEngine);
	virtual ~CMaterialManager();

    virtual void DeviceLost(CRenderSettings &Settings);

    MAKE_CLSCREATE(CMaterialManager);
    MAKE_CLSNAME("CMaterialManager");
    MAKE_DEFAULTGARBAGE()
};

