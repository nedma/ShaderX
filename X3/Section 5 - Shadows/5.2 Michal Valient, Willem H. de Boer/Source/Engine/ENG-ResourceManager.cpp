#include "stdafx.h"
#include "ENG-ResourceManager.h"
#include "ENG-Engine.h"
#include "eng-baseresource.h"
#include "SC-HashTable2.h"

CResourceManager::CResourceManager(char *ObjName,CEngine *OwnerEngine) : CBaseManager(ObjName,OwnerEngine) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV0, "Creating");
    Resources = new CHashTable2(true, 0);
    _LOGE(logE,"Created");
}

CResourceManager::~CResourceManager() {
    int logE = _LOGB(this, D3_DV_GRP2_LEV0, "Destroying");
    ReleaseAll();
    delete Resources;
    _LOGE(logE,"Destroyed");
}

void CResourceManager::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    if (Resources->GetItemCount()>0) {
        UINT ret = Resources->GetFirstItem();
        while (1) {
            CBaseResource *Resource = (CBaseResource *)Resources->GetValueAtIndex(ret);
            if (Resource!=NULL) {
                Resource->DeviceLost(Settings);
            }
            UINT ret2 = Resources->GetNextItem(ret);
            if (ret2==ret) break;
            else ret = ret2;
        }
    }
    _LOGE(logE,"Sended device lost");
}

bool CResourceManager::Restore(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending restore");
    bool result = TRUE;
    if (Resources->GetItemCount()>0) {
        UINT ret = Resources->GetFirstItem();
        while (1) {
            CBaseResource *Resource = (CBaseResource *)Resources->GetValueAtIndex(ret);
            if (Resource!=NULL) {
                result = result && Resource->Restore(Settings);
            }
            UINT ret2 = Resources->GetNextItem(ret);
            if (ret2==ret) break;
            else ret = ret2;
        }
    }
    _LOGE(logE,"Sended restore");
    return result;
}

void CResourceManager::ReleaseAll() {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Releasing all items");
    if (Resources->GetItemCount()>0) {
        UINT ret = Resources->GetFirstItem();
        while (1) {
            CBaseResource *Resource = (CBaseResource *)Resources->GetValueAtIndex(ret);
            if (Resource!=NULL) {
                _WARN(this,ErrMgr->TMemory,"Resource %u (%s) left in manager", Resource->GetResourceID(), Resource->GetObjName());
                Resources->Erase(Resource->GetResourceID());
                Resource->Delete(TRUE);
            }
            if (Resources->GetItemCount()>0) {
                ret = Resources->GetFirstItem();
            } else {
                break;
            }
        }
    }
    Resources->Clear(true);
    _LOGE(logE,"All items deleted");
}

CBaseResource *CResourceManager::Find(DWORD ResourceID) {
    CBaseResource *result = (CBaseResource *)Resources->Get(ResourceID);
    if (result!=NULL) _LOG(this,D3_DV_GRP2_LEV4,"Found Resource with ID %i",ResourceID);
    else _LOG(this,D3_DV_GRP2_LEV4,"Not found Resource with ID %i",ResourceID);
    return result;
}

bool CResourceManager::Add(DWORD ResourceID, CBaseResource &Resource) {
    if (ResourceID>=D3_GRI_TemporaryStart) _WARN(this, ErrMgr->TSystem, "ID %u is in range of temporary ID's", ResourceID);
    bool fncresult = FALSE;
    CBaseResource *result = Find(ResourceID);
    if (result!=NULL) {
        _LOG(this,D3_DV_GRP2_LEV4, "Add failed, resource %u already exists", ResourceID);
    } else {
        Resources->Add(ResourceID, &Resource);
        Resource.SetResourceID(ResourceID);
        _LOG(this,D3_DV_GRP2_LEV4, "Add success, resource %u is new", ResourceID);
        fncresult = TRUE;
    }
    return fncresult;
}

bool CResourceManager::AddTemporary(CBaseResource &Resource) {
    bool fncresult = FALSE;
    DWORD ResourceID = OwnerEngine->GetUniqueTemporaryNumber();
    CBaseResource *result = Find(ResourceID);
    if (result!=NULL) {
        _LOG(this,D3_DV_GRP2_LEV4, "Add failed, temporary resource %u already exists", ResourceID);
    } else {
        Resources->Add(ResourceID, &Resource);
        Resource.SetResourceID(ResourceID);
        _LOG(this,D3_DV_GRP2_LEV4, "Add success, temporary resource %u is new", ResourceID);
        fncresult = TRUE;
    }
    return fncresult;
}

int CResourceManager::Delete(DWORD ResourceID) {
    CBaseResource *result = Find(ResourceID);
    if (result!=NULL) {
        int logE = _LOGB(this,D3_DV_GRP2_LEV4, "Deleting Resource with ID %i, new RefCount will be %i",result->GetResourceID(),result->GetReferenceCount()-1);
        if (result->GetReferenceCount()<=1) Resources->Erase(ResourceID);
        DWORD res = result->Delete(FALSE);
        _LOGE(logE, "Deleted");
        return res;
    }
    return -1;
}

CBaseResource *CResourceManager::Query(DWORD ResourceID) {
    CBaseResource *result = Find(ResourceID);
    if (result!=NULL) result->AddReferenceCount();
    return result;
}