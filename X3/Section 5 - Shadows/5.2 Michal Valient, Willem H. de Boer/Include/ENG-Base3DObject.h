// ENG-Base3DObject.h: interface for the CBase3DObject class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "ENG-BaseResource.h"

//############################################################################
//Forward declarations
class CRootClassArray;
class CXMLNode;
class CPRSLinearController;
class CBaseMaterial;
class CBaseMaterialContext;

class CBase3DObject : public CBaseResource {
protected:
    UINT            TransformRenderID;  ///In this number, render ID is stored 
                                        ///when first called prepareBounding geometry. Every other call is
                                        ///ignored if here is stored the same value as in RenderSettings.
public:
    CPRSLinearController *PRSController;///If not null, object is animated with this controller.

    CRootClassArray *Lights;            ///Here are stored lights, that affects this object.

    bool            Visible;            ///If object is visible (means if it is worth calling render method)
    D3DXVECTOR3     Translation;
    D3DXQUATERNION  Rotation;
    D3DXVECTOR3     Scale;

    ///World object position
    D3DXVECTOR3     WorldObjectPos;

    //Bounding sphere - original parameters
    D3DXVECTOR3     BSCenter;
    float           BSRadius;

    //Bounding sphere - after transform to world space (after prepareGeometry phase)
    D3DXVECTOR3     WorldBSCenter;
    float           WorldBSRadius;

    //Transformation matrixes
    D3DXMATRIX      WorldTransform;
    D3DXMATRIX      LocalTransform;

    int             ParentID;       ///ID of the parent object.
    CBase3DObject  *Parent;         ///if not null, this is parent object

    CBase3DObject(char *ObjName,CEngine *OwnerEngine);
	virtual ~CBase3DObject();

    //Renders object to screen.
    //virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) = 0;

    //Renders object to screen allowing to use own material.
    virtual void *Render(CRenderSettings &RenderSettings) = 0;

    //End the rendering of the object - applicable only after Render method. Does not affect the RenderGeometry method.
    virtual bool EndRendering(CRenderSettings &RenderSettings) = 0;

    //Renders object to screen without setting explicit material - just render the geometry.
    //Implementation should not cache the call - immediate Rendering has to occur.
    virtual void *RenderGeometry(CRenderSettings &RenderSettings) = 0;

    //This method is called in first phase of rendering - object is requested to 
    //transform its bounding objects to its final position, size and shape.
    //All *Matrix structure have to be valid after call of this method.
    //Default implementation forces parent to compute bound geometry,
    //then computes local transform and then objects world transform.
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);

    //object is requested to prepare its geometry for rendering. Changes to vertex 
    //buffers are done in this phase
    virtual void PrepareGeometry(CRenderSettings &RenderInfo) = 0;

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSNAME("CBase3DObject");
};

