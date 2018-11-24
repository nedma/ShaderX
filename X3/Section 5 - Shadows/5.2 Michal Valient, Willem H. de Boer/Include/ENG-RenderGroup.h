#pragma once
#include "baseclass.h"

//############################################################################
//Forward declarations
class CBase3DObject;
class CRootClassArray;
class CRenderSettings;
class CBaseMaterial;

/**
  This class groups objects with the same rendering ordering
  This implementation uses no special sorting.
  For example group for transparent objects will use inverted Z ordering.
**/

class CRenderGroup : public CBaseClass {
protected:
    DWORD GroupID;
    CRootClassArray *Visible;   ///All objects in group
    CBaseMaterial   *NoOutput;  ///Material for no output
public:
    CRootClassArray *Objects;   ///All objects in group

    D3_INLINE void SetGroupID(DWORD GroupID) {this->GroupID = GroupID; }
    D3_INLINE DWORD GetGroupID() { return GroupID; }


    void Render(CRenderSettings &Settings);

    void AddVisible(CBase3DObject &Object);
    void ClearVisible();
    DWORD   GetVisibleCount();
    CBase3DObject *GetVisible(DWORD Index);
    void AddObject(CBase3DObject &Object);

    CRenderGroup(char *ObjName,CEngine *OwnerEngine);
    virtual ~CRenderGroup(void);

    MAKE_CLSCREATE(CRenderGroup);
    MAKE_CLSNAME("CRenderGroup");
    MAKE_DEFAULTGARBAGE()
};
