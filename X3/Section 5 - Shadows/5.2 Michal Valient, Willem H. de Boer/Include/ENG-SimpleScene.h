#pragma once
#include "baseclass.h"

//############################################################################
//Forward declarations
class CHashTable2;
class CRenderSettings;
class CRenderGroup;
class CBase3DObject;
class CD3SCamera;
class CXMLNode;

class CSimpleScene : public CBaseClass {
protected:
    CHashTable2  *Groups;
    CHashTable2  *Objects;
    CHashTable2  *Cameras;
    CHashTable2  *Lights;
public:
    CD3SCamera  *CurrentCamera;
    //If device is lost, everything has to be set to lost state.
    virtual void DeviceLost(CRenderSettings &Settings);    

    //Restore from lost state.
    virtual void Restore(CRenderSettings &Settings);

    void AddGroupByID(DWORD GroupID, CRenderGroup &Group);
    void AddObject(DWORD GroupID, CBase3DObject &Object);
    void AddCamera(CBase3DObject &Camera);
    void AddLight(CBase3DObject &Light);

    void Render(CRenderSettings &Settings);

    CSimpleScene(char *ObjName,CEngine *OwnerEngine);
    virtual ~CSimpleScene(void);

    bool Load(int LoadIndex, CRenderSettings &Settings);
    bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CSimpleScene);
    MAKE_CLSNAME("CSimpleScene");
    MAKE_DEFAULTGARBAGE()
};
