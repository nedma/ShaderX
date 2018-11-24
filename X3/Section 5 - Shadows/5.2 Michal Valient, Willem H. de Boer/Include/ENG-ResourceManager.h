#pragma once

#include "MGR-BaseManager.h"

//############################################################################
//Forward declarations
class CHashTable2;
class CBaseResource;
class CRenderSettings;

class CResourceManager : public CBaseManager {
protected:
    CHashTable2  *Resources;
public:
    ///With this method you query for specified resource. If it is managed by manager, it's
    ///reference count is increased and caller can use it.
    ///@retval NULL if resource is not manager by manager.
    virtual CBaseResource *Query(DWORD ResourceID);
    ///Decrements reference count of Resource. If that drops to zero, Release is called.
    ///After you add resource to manager, do not destroy resource directly. Use this method,
    ///because of that there can be a lot of other objects that refer to this resource.
    ///@return New reference count of resource. Zero means resource was freed.
    ///@retval -1 means error (or resource was not found)
	virtual int Delete(DWORD ResourceID);
    ///Add new resource to manager.
    ///@retval FALSE if another resource with the same ID exists in manager
	virtual bool Add(DWORD ResourceID, CBaseResource &Resource);
    ///Add new temporary resource to manager. This resource receives it's own ID
    ///@retval FALSE if another resource with the same ID exists in manager
	virtual bool AddTemporary(CBaseResource &Resource);
    ///Find specified resource
    ///@retval NULL on error
	virtual CBaseResource *Find(DWORD ResourceID);
    ///Delete all resources (in meaning of release).
    virtual void ReleaseAll();
    ///Send DeviceLost to all resources
	virtual void DeviceLost(CRenderSettings &Settings);
    ///Send Restore to all resources
	virtual bool Restore(CRenderSettings &Settings);

    CResourceManager(char *ObjName,CEngine *OwnerEngine);
	virtual ~CResourceManager();

    MAKE_CLSCREATE(CResourceManager);
    MAKE_CLSNAME("CResourceManager");
    MAKE_DEFAULTGARBAGE();
};
