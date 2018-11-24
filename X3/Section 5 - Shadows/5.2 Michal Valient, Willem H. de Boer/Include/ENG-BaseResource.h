#pragma once

#include "baseclass.h"

//############################################################################
//Forward declarations
class CRenderSettings;

class CBaseResource : public CBaseClass {
public:
    friend class CResourceManager;
protected:
    DWORD   Flags;
    DWORD   ReferenceCount;
    DWORD   CacheInCount;
    DWORD   ResourceID;
    ///Index of resource on disk used to load.
    DWORD   LoadIndex;

    ///Responsible for actual cache in
    ///Default implementation calls Load method if mesh is not filled
    ///Then it calls Update method (always).
    virtual void DoCacheIn(CRenderSettings &RenderSettings);

    ///Responsible for actual cache out
    ///Default implementation does nothing
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
    ///Protected destructor - call Delete(true) to force deletion instead!
    virtual ~CBaseResource(void);
public:
    static const DWORD Flag_Lost            = 1;
    static const DWORD Flag_Restorable      = 2;
    static const DWORD Flag_Filled          = 4;
    static const DWORD Flag_PerFrameUpdate  = 8;
    static const DWORD Flag_OnRenderUpdate  = 16;

    ///Returns cache in count
    D3_INLINE DWORD GetCacheInCount() { return CacheInCount; }

    ///Returns reference count
    D3_INLINE DWORD GetReferenceCount() { return ReferenceCount; }

    ///Increases reference count of resource
    D3_INLINE DWORD AddReferenceCount() { return ++ReferenceCount; }

    ///Returns LoadIndex
    D3_INLINE int GetLoadIndex() { return LoadIndex; }
    ///Sets resource ID
    D3_INLINE void SetLoadIndex(DWORD LoadIndex) { this->LoadIndex = LoadIndex; }

    ///Returns resource ID
    D3_INLINE int GetResourceID() { return ResourceID; }
    ///Sets resource ID
    D3_INLINE void SetResourceID(DWORD ResourceID) { this->ResourceID = ResourceID; }

    ///Returns resource flags
    D3_INLINE UINT GetFlags() { return Flags; }

    ///returns lost status
    D3_INLINE bool GetLost() { return (Flags & Flag_Lost); }
    ///sets (or resets) lost status
    D3_INLINE void SetLost(bool Status) { 
        if (Status) Flags = Flags | Flag_Lost;
        else Flags = Flags & (~Flag_Lost);
    }

    ///Returns restorable status
    D3_INLINE bool GetRestorable() { return (Flags & Flag_Restorable)? true : false; }
    ///Sets restorable status
    D3_INLINE void SetRestorable(bool Status) {
        if (Status) Flags = Flags | Flag_Restorable;
        else Flags = Flags & (~Flag_Restorable);
    }

    ///Returns filled status for this mesh.
    ///If filled status is true, load is not called during cache in
    D3_INLINE bool GetFilled() { return (Flags & Flag_Filled)? true : false; }
    ///Set filled status
    D3_INLINE void SetFilled(bool Status) { 
        if (Status) Flags = Flags | Flag_Filled;
        else Flags = Flags & (~Flag_Filled);
    }

    ///Returns per frame update status
    ///If PerFrameUpdate is true, Update method is called every render loop
    D3_INLINE bool GetPerFrameUpdate() { return (Flags & Flag_PerFrameUpdate)? true : false; }
    ///Sets per frame update status
    D3_INLINE void SetPerFrameUpdate(bool Status) {
        if (Status) Flags = Flags | Flag_PerFrameUpdate;
        else Flags = Flags & (~Flag_PerFrameUpdate);
    }

    ///Returns OnRenderUpdate status
    ///If OnRenderUpdate is true, Update method is called every render loop resource is used.
    D3_INLINE bool GetOnRenderUpdate() { return (Flags & Flag_OnRenderUpdate)? true : false; }
    ///Sets OnRenderUpdate status
    D3_INLINE void SetOnRenderUpdate(bool Status) {
        if (Status) Flags = Flags | Flag_OnRenderUpdate;
        else Flags = Flags & (~Flag_OnRenderUpdate);
    }

    ///Restore method
    virtual bool Restore(CRenderSettings &Settings);

    ///Device lost method
    virtual void DeviceLost(CRenderSettings &Settings) = 0;

    ///Cache in method
    ///Do not override this method, override DoCacheIn() instead.
    virtual void CacheIn(CRenderSettings &RenderSettings);

    ///Cache out method
    ///Do not override this method, override DoCacheOut() instead.
    virtual void CacheOut(CRenderSettings &RenderSettings);

    ///Load data into resource
    ///@param LoadIndex - index of data in FileManager.
    ///@warning all temporary data should be freed on end of this method.
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings) = 0;

    ///Update data in resource
    virtual bool Update(CRenderSettings &RenderSettings) = 0;

    ///Delete method decrements reference count and if it drops to zero, it releases object.
    ///@return new reference count of resource. Zero means resource was freed.
    ///@param ForceDelete If ForceDelete is true, object is forced to destroy, even if referecne count is not zero.
    ///@retval -1 means error.
    virtual int Delete(bool ForceDelete);

    CBaseResource(char *ObjName, CEngine *OwnerEngine);
    MAKE_CLSNAME("CBaseResource");
};
