#pragma once

#include "material\mat-dx9material.h"

class CBaseTexture;
class CBaseShader;
class CMString;
class CRootClassArray;
class CD3SLight;

#include "nommgr.h"
#include <vector>
#include <hash_map>
#include "mmgr.h"

using namespace std;
using namespace stdext;

class CDX9_MaterialShadowSoft : public CDX9Material {
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    CComPtr<IDirect3DVertexDeclaration9> VSDeclaration;
    CComPtr<ID3DXEffect> Effect;

    static CComPtr<IDirect3DTexture9>    spRTTex;
    static CComPtr<ID3DXRenderToSurface> spRenderToTex;

    CBaseTexture    *DefaultTexture;
    CBaseTexture    *DefaultSpotLight;
    CBaseTexture    *DefaultNormalMap;
    CBaseTexture    *NoiseTexture;

    float           SpecularShininess;      //shininess of material
    D3DXCOLOR       SpecularColor;          //Material's specular color modifier
    D3DXCOLOR       DiffuseColor;           //Material's diffuse color modifier
    DWORD           dwNormalID;
    DWORD           dwDiffuseID;
    CBaseTexture    *DiffuseTexture;        //Diffuse texture with specular strength in alpha
    CBaseTexture    *NormalTexture;         //Normal texture

    D3DXHANDLE  Technique;
    D3DXHANDLE  ToClipMatrix;
    D3DXHANDLE  ToWorldMatrix;
    D3DXHANDLE  SpotlightTexture;
    D3DXHANDLE  ShadowMapTexture;
    D3DXHANDLE  LightPosition;
    D3DXHANDLE  EyePosition;
    D3DXHANDLE  ToLightMatrix;
    D3DXHANDLE  LightRanges;
    D3DXHANDLE  hNoiseTexture;


    typedef struct {
        CMesh           *pMesh;
        CBase3DObject   *pObject;
        D3DXMATRIX      ToClip;
        D3DXMATRIX      ToWorld;
        DWORD           Index;
    } d3CacheInfo;

    typedef vector<d3CacheInfo> cachevector;

    typedef struct {
        cachevector objects;
        CD3SLight *pLight;
    } d3lightInfo;
    
    typedef hash_map<DWORD, d3lightInfo> lightMap;
    static lightMap lightSort;

    //The shadow accum processing things
    D3DXHANDLE  hAccumTechnique;
    D3DXHANDLE  hAccumSource;
    CComPtr<IDirect3DVertexDeclaration9>    VSAccumDeclaration;
    CComPtr<IDirect3DVertexBuffer9>         AccumVertexBuf;

    //The phong rendering code
    D3DXHANDLE  hPhongTechnique;

    void ReleaseBuffers();

    virtual ~CDX9_MaterialShadowSoft(void);

    bool AccumShadows(CRenderSettings &RenderSettings, CD3SLight *pLight);

    bool RenderPhong(CRenderSettings &RenderSettings, d3lightInfo *LI);

public:
    static bool bUsePureRandom;
    static bool bUse1D;

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool Clone(CDX9Material **pOut);

    virtual bool Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, CBase3DObject *pObject);
    virtual bool Render(CRenderSettings &RenderSettings, CBase3DObject *pObject);
    virtual bool EndRendering(CRenderSettings &RenderSettings);
    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    ///Const, dest & more
    CDX9_MaterialShadowSoft(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CDX9_MaterialShadowSoft);
    MAKE_CLSNAME("CDX9_MaterialShadowSoft");
    MAKE_DEFAULTGARBAGE()
};
