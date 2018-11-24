// Engine's Runtime classes initialisation
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-Engine.h"

///This method registers all classes derived from CBaseClass that engine knows for dynamic creation
void CEngine::RegisterRuntimeClasses(void) {
    /*
    int logE = _LOGB("Engine",NULL,D3_DV_GRP0_LEV0,"Registering runtime classes");
    CBaseClass *RClass;
    char RClassName[1024];
    //CDirTreeDirectory
    RClass = new CDirTreeDirectory("RuntimeClass",this);
    if (!RuntimeCreator->RegisterClass(RClass,NULL)) {
        RClass->GetClsName(RClassName,1024);
        _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"Failed to register class %s",RClassName);
    }
    _LOGE(logE,"Registered");
    */
}

