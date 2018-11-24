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

class CDX9_MaterialShadowPlain : public CDX9Material {
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    CComPtr<IDirect3DVertexDeclaration9> VSDeclaration;
    CComPtr<ID3DXEffect> Effect;

    CBaseTexture    *DefaultTexture;
    CBaseTexture    *DefaultSpotLight;
    CBaseTexture    *DefaultNormalMap;

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

    void ReleaseBuffers();

    virtual ~CDX9_MaterialShadowPlain(void);

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

public:
    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool Clone(CDX9Material **pOut);

    virtual bool Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, CBase3DObject *pObject);
    virtual bool Render(CRenderSettings &RenderSettings, CBase3DObject *pObject);
    virtual bool EndRendering(CRenderSettings &RenderSettings);
    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    ///Const, dest & more
    CDX9_MaterialShadowPlain(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CDX9_MaterialShadowPlain);
    MAKE_CLSNAME("CDX9_MaterialShadowPlain");
    MAKE_DEFAULTGARBAGE()
};
