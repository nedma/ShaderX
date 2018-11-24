#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "baseclass.h"

class CXMLNode;
class CRenderSettings;
class CBase3DObject;

class CPRSLinearController : public CBaseClass {
    typedef struct KeyFrame {
        D3DXVECTOR3     Position;
        D3DXQUATERNION  Rotation;
        D3DXVECTOR3     Scale;
    } KeyFrame;

    KeyFrame        *Keyframes;
    UINT             KeyframeCount;
public:
    CPRSLinearController(char *ObjName,CEngine *OwnerEngine);
    virtual ~CPRSLinearController(void);

    bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);
    bool Animate(CBase3DObject *Object, float Time, CRenderSettings &Settings);

    MAKE_CLSCREATE(CPRSLinearController);
    MAKE_CLSNAME("CPRSLinearController");
    MAKE_DEFAULTGARBAGE()
};
