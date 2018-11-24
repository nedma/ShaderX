#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "eng-base3dobject.h"

class CMesh;
class CHashTable2;
class CXMLNode;

class CD3SObject : public CBase3DObject {

    class CMatSubset : public CRootClass {
    public:
        CBaseMaterial           *Material;        ///If not null, this is material of this object

        CMatSubset();
        ~CMatSubset();
    };
    CMesh  *Mesh;                   ///If not null, this is mesh of this object

    CHashTable2    *SubSetMaterials; ///Materials for subsets (CMatSubset class)

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
	virtual ~CD3SObject();
public:
    int     MeshID;                 ///ID of the mesh

    CD3SObject(char *ObjName,CEngine *OwnerEngine);

    //Renders object to screen allowing to use own material.
    virtual void *Render(CRenderSettings &RenderSettings);

    virtual bool EndRendering(CRenderSettings &RenderSettings);

    //Renders object to screen without setting explicit material - just render the geometry.
    virtual void *RenderGeometry(CRenderSettings &RenderSettings);

    virtual void PrepareGeometry(CRenderSettings &RenderInfo);
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool AssignMesh(DWORD MeshID);
    virtual bool AssignMaterial(DWORD SubsetID, DWORD MaterialID);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CD3SObject);
    MAKE_CLSNAME("CD3SObject");
    MAKE_DEFAULTGARBAGE()
};
