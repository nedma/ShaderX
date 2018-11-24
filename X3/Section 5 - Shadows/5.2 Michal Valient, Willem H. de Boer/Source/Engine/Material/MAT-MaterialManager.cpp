// ENG-MaterialManager.cpp: implementation of the CMaterialManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Material\Mat-MaterialManager.h"
#include "Material\Mat-BaseMaterial.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaterialManager::CMaterialManager(char *ObjName, CEngine *OwnerEngine) : CResourceManager(ObjName,OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Creating");
    this->CurrentMaterial = NULL;
    this->CurrentPass = 0;
    _LOGE(logE);
}

CMaterialManager::~CMaterialManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Destroying");
    if (CurrentMaterial!=NULL) {
        _WARN(this,ErrMgr->TMemory,"Material %u (%s) was left as current. Possible memory leaks in Direct3DDevice", CurrentMaterial->GetResourceID(), CurrentMaterial->GetObjName());
    }
    _LOGE(logE);
}

void CMaterialManager::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    CResourceManager::DeviceLost(Settings);
    _LOGE(logE,"Sended device lost");
}

