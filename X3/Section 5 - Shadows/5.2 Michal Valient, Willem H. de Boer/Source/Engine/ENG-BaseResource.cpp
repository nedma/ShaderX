#include "stdafx.h"

#include "eng-baseresource.h"

CBaseResource::CBaseResource(char *ObjName, CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    Flags = 0;
    ReferenceCount = 1;
    CacheInCount = 0;
    ResourceID = 0;
    LoadIndex = 0;
}

CBaseResource::~CBaseResource(void) {
}

bool CBaseResource::Restore(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Restoring Resource");
    bool result = TRUE;
    if (!GetLost()) {
        _LOG(this,D3_DV_GRP2_LEV0,"Does not need to be restored");
    } else {
        if (!GetRestorable()) {
            result = FALSE;
            _WARN(this,ErrMgr->TNoSupport,"Not a restorable resource");
        } else {
            if (CacheInCount>0) {   //mesh was cached in, we need to cache it in again.
                DoCacheIn(Settings);
            } else {    //mesh is not cached in, we need to cache it out
                DoCacheOut(Settings);
            }
            SetLost(FALSE);
        }
    }
    _LOGE(logE,"End of restoration");
    return result;
}

void CBaseResource::DoCacheIn(CRenderSettings &RenderSettings) {
    if (!GetFilled()) { //needed to be loaded
        if (!this->Load(LoadIndex, RenderSettings)) {
            _WARN(this,ErrMgr->TNoSupport,"Cannot load resource");
        }
    }
    this->Update(RenderSettings);
}

void CBaseResource::CacheIn(CRenderSettings &RenderSettings) {
    if (!CacheInCount) {    //needs to be cached in
        int logE = _LOGB(this, D3_DV_GRP2_LEV2, "Caching in resource, CacheIn count is 1");
        DoCacheIn(RenderSettings);
        _LOGE(logE,"Cached in");
    } else {
        _LOG(this,D3_DV_GRP2_LEV3, "Cache in - new CacheIn count is %u",CacheInCount+1);
    }
    this->CacheInCount++;
}

void CBaseResource::DoCacheOut(CRenderSettings &RenderSettings) {
}

void CBaseResource::CacheOut(CRenderSettings &RenderSettings) {
    if (CacheInCount>1) {
        _LOG(this,D3_DV_GRP2_LEV3, "Cache out - new cache in count is %u",CacheInCount-1);
        CacheInCount--;
    } else {
        int logE = _LOGB(this, D3_DV_GRP2_LEV2, "Caching out resource, cache in count dropped to zero");
        DoCacheOut(RenderSettings);
        this->CacheInCount = 0;
        _LOGE(logE,"Cached out");
    }
}

int CBaseResource::Delete(bool ForceDelete) {
    if (ReferenceCount>1 && !ForceDelete) {
        ReferenceCount--;
        _LOG(this,D3_DV_GRP2_LEV1, "Delete - new reference count is %u",ReferenceCount);
        return ReferenceCount;
    } else {
        if (this->ReferenceCount>1) 
            _WARN(this,ErrMgr->TMemory, "Delete - Resource %u (%s) Reference count is %u, but object is being destroyed!",ResourceID, GetObjName(), ReferenceCount);
        else 
            _LOG(this,D3_DV_GRP2_LEV0, "Delete - reference count is 0, destroying");
        delete this;
        return 0;
    }
}