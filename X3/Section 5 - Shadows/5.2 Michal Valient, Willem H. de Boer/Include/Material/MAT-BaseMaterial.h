#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "eng-baseresource.h"

class CBase3DObject;
class CMesh;

class CBaseMaterialContext : public CRootClass {
    public:
        CBase3DObject   *Object;
};

class CBaseMaterial : public CBaseResource {
protected:
    virtual ~CBaseMaterial(void);
public:
    virtual bool Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, CBase3DObject *pObject) = 0;

    virtual bool Render(CRenderSettings &RenderSettings, CBase3DObject *pObject) = 0;

    virtual bool EndRendering(CRenderSettings &RenderSettings) = 0;

    ///Const, dest & more
    CBaseMaterial(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSNAME("CBaseMaterial");
};
