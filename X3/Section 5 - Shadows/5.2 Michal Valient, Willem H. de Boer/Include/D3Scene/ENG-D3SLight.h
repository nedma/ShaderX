#pragma once
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "eng-base3dobject.h"

class CFrustum;
class CXMLNode;
class CBaseTexture;
class CRootClassArray;
class CBaseMaterial;

class CD3SLight : public CBase3DObject {
private:
    CFrustum        *Frustum;
    D3DXMATRIX      SpotProjection;
    D3DXMATRIX      SpotView;
    CBaseMaterial   *ShadowBuffer;  ///Material for no output
    CBaseMaterial   *PostProcess;   ///Material for post processing
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
    virtual ~CD3SLight(void);
public:
    ///Type of light
    static const int TTypeOmni = 1;
    static const int TTypeSpot = 2;
    static const int TTypeDirect = 3;

    ///Type of decay
    static const int TDecayNone = 1;
    static const int TDecayInverse = 2;
    static const int TDecayInverseSquare = 3;

    CBaseTexture    *SpotlightTexture;
    
    //Shadow related
    CBaseTexture    *ShadowTexture;
    CBaseTexture    *ShadowTextureIntermediate;
    ID3DXRenderToSurface *RenderToShadow;
    CRootClassArray *LitObjects;            //objects lit by this light.

    int             Type;       //convert from Flex types
    float           Hotspot;
    float           Intensity;
    float           Contrast;
    bool            UseAttenuationNear;
    bool            UseAttenuationFar;
    float           AttenuationNearStart;
    float           AttenuationNearEnd;
    float           AttenuationFarStart;
    float           AttenuationFarEnd;
    int             DecayType;  //convert from Flex types
    int             ShadowMapSize;
    int             TargetID;
    CBase3DObject  *Target;

    D3DXCOLOR       LightColor;

    D3DXMATRIX      SpotViewProjection;

    CD3SLight(char *ObjName,CEngine *OwnerEngine);

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

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    //Specific light functions
    bool TestObjectLit(CBase3DObject *Obj);
    void AddLitObject(CBase3DObject *Obj);
    bool RenderShadowBuffer(CRenderSettings &Settings);

    bool RenderShadowBuffer1(CRenderSettings &Settings);
    bool RenderShadowBuffer2(CRenderSettings &Settings);


    MAKE_CLSCREATE(CD3SLight);
    MAKE_CLSNAME("CD3SLight");
    MAKE_DEFAULTGARBAGE()
};
