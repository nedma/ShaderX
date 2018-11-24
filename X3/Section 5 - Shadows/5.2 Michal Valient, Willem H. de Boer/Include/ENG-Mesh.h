#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "ENG-BaseResource.h"


//############################################################################
//Forward declarations
class CRootClassArray;
class CMeshPart;

class CMesh : public CBaseResource {
protected:
    CRootClassArray *Subsets;

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
	virtual ~CMesh();
public:
    D3DXVECTOR3     BoundSphereCenter;
    float           BoundSphereRadius;  //set to 0 if BSphere is not used.
    //###############################################
    //Buffer manipulation
	void ReleaseBuffers(bool Recreate);
    
    //###############################################
    //Subset manipulation methods
    void AddSubset(CMeshPart &Subset);
    void DeleteSubset(int SubsetIndex);
	CMeshPart *GetSubset(int SubsetIndex);
    int GetSubsetCount();

    ///Rendering method
    void Render(CRenderSettings &RenderSettings, int Section);   // -1 = all sections

    virtual void DeviceLost(CRenderSettings &Settings);

    CMesh(char *ObjName, CEngine *OwnerEngine);
    MAKE_CLSNAME("CMesh");
    MAKE_DEFAULTGARBAGE();
};

