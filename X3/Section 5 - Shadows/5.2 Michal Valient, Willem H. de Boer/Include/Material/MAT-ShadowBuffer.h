#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include <vector>
#include "mmgr.h"

#include "material\mat-dx9material.h"

class CBaseTexture;
class CMString;
class CMaterialShadowBuffer;
class CBaseShader;
class CD3SLight;

class CMaterialShadowBuffer : public CDX9Material {
protected:
    CComPtr<IDirect3DVertexDeclaration9> VSDeclaration;
    CComPtr<ID3DXEffect> Effect;

    D3DXHANDLE  Technique;
    D3DXHANDLE  ToClipMatrix;
    D3DXHANDLE  ToWorld;
    D3DXHANDLE  LightPos;
    D3DXHANDLE  LightRanges;

    void DeleteShaders();

    virtual ~CMaterialShadowBuffer(void);

    typedef struct {
        void *pMesh;
        D3DXMATRIX  ToClip;
        D3DXMATRIX  ToWorld;
        DWORD Index;
    } d3CacheInfo;

    std::vector<d3CacheInfo>    vObjects;
    std::vector<d3CacheInfo>    vMeshes;
public:
    CD3SLight   *pLight;

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    ///Const, dest & more
    CMaterialShadowBuffer(char *ObjName,CEngine *OwnerEngine);

    virtual bool Clone(CDX9Material **pOut);

    virtual bool Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, CBase3DObject *pObject);
    virtual bool Render(CRenderSettings &RenderSettings, CBase3DObject *pObject);
    virtual bool EndRendering(CRenderSettings &RenderSettings);
    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CMaterialShadowBuffer);
    MAKE_CLSNAME("CMaterialShadowBuffer");
    MAKE_DEFAULTGARBAGE()
};
