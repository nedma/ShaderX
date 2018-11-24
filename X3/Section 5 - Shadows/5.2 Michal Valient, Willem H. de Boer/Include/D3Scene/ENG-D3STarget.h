#pragma once
#include "eng-base3dobject.h"

class CD3STarget : public CBase3DObject {
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
    virtual ~CD3STarget(void);
public:
    CD3STarget(char *ObjName,CEngine *OwnerEngine);

    void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext);

    //Renders object to screen allowing to use own material.
    virtual void *Render(CRenderSettings &RenderSettings) { return NULL; };

    virtual bool EndRendering(CRenderSettings &RenderSettings) {return true;};

    //Renders object to screen without setting explicit material - just render the geometry.
    virtual void *RenderGeometry(CRenderSettings &RenderSettings) { return NULL; };

    void PrepareGeometry(CRenderSettings &RenderInfo);

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    MAKE_CLSCREATE(CD3STarget);
    MAKE_CLSNAME("CD3STarget");
    MAKE_DEFAULTGARBAGE()
};

