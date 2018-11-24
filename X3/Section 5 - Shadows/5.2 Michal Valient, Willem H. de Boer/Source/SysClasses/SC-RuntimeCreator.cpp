#include "stdafx.h"
#include "sc-runtimecreator.h"
#include "sc-StrHashTable.h"

CRuntimeCreator::CRuntimeCreator(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP0_LEV0, "Creating");
    Classes = new CStrHashTable(true, 0);
}

CRuntimeCreator::~CRuntimeCreator(void) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0, "Destructing");
    delete Classes;
    _LOGE(logE, "Destructed");
}

bool CRuntimeCreator::RegisterClass(CBaseClass *Class, void *RegisterInfo) {
    int NameSize = Class->GetClsName(NULL,0);
    char *Name = (char *)MemMgr->AllocMem(NameSize+1,0);
    NameSize = Class->GetClsName(Name,NameSize+1);
    int logE = _LOGB(this,D3_DV_GRP0_LEV1, "Registering class %s",Name);
    CRuntimeInfo *Nfo = new CRuntimeInfo();
    Nfo->ExistingInstance = Class;
    Nfo->RegisterInfo = RegisterInfo;
    CMString ClassName(Name);
    Classes->Add(ClassName.c_str(), ClassName.Length(), Nfo);
    MemMgr->FreeMem(Name);
    _LOGE(logE);
    return true;
}

CBaseClass *CRuntimeCreator::CreateClass(char *ClassName, char *ObjectName, void *Parameters) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV4, "Creating object %s from class %s", ObjectName, ClassName);
    CBaseClass *result = NULL;
    CMString Name(ClassName);
    CRuntimeInfo *Origin = (CRuntimeInfo *)Classes->Get(Name.c_str(), Name.Length());
    if (Origin!=NULL) {
        result = Origin->ExistingInstance->Create(ObjectName,this->OwnerEngine, Origin->RegisterInfo, Parameters);
        if (result==NULL) _LOGE(logE,"NOT CREATED - Classes Create method failed");
        else {
            char on[128],cn[128];
            result->GetObjName(on,128);
            result->GetClsName(cn,128);
            _LOGE(logE,"Created class %s.%s",on,cn);
        }
    } else {
        _LOGE(logE,"NOT CREATED - Runtime class not found");
    }
    return result;
}
