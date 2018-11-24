#pragma once
#include "eng-base3dobject.h"

class CXMLNode;

class CD3SCamera : public CBase3DObject {
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
    virtual ~CD3SCamera(void);
public:
    bool            Ortho;
    float           FOV;
    float           NearClip;
    float           FarCLip;
    int             TargetID;
    CBase3DObject   *Target;
    D3DXVECTOR3     Up;     //Vector to UP

    CD3SCamera(char *ObjName,CEngine *OwnerEngine);

    virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext);

    //Renders object to screen allowing to use own material.
    virtual void *Render(CRenderSettings &RenderSettings) { return NULL; };

    virtual bool EndRendering(CRenderSettings &RenderSettings) {return true;};

    //Renders object to screen without setting explicit material - just render the geometry.
    virtual void *RenderGeometry(CRenderSettings &RenderSettings) { return NULL; };
    
    ///This implementation Calls Target's PrepareGeometry (if not null).
    ///Then prepares own Geometry by calling ancestor's PrepareGeometry.
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);
    virtual void PrepareGeometry(CRenderSettings &RenderInfo);

    ///Instructs camera to apply it's settings to rendersettings.
    virtual void Apply(CRenderSettings &Settings);

    ///Camera manipulation
    ///Recomputes UP vector and other thingies.
    virtual void Recompute();
    ///Rotate Yaw Pitch and Roll
    virtual void RotateLR(float Angle);
    virtual void RotateUD(float Angle);
    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CD3SCamera);
    MAKE_CLSNAME("CD3SCamera");
    MAKE_DEFAULTGARBAGE()
};
