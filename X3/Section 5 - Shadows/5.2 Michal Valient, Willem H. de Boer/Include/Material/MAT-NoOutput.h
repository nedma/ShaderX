#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include <vector>
#include "mmgr.h"

#include "material\mat-dx9material.h"

class CBaseTexture;
class CMString;
class CMaterialNoOutput;
class CBaseShader;

class CMaterialNoOutput : public CDX9Material {
protected:
    CComPtr<IDirect3DVertexDeclaration9> VSDeclaration;
    CComPtr<ID3DXEffect> Effect;

    D3DXHANDLE  Technique;
    D3DXHANDLE  ToClipMatrix;
    D3DXHANDLE  OutColor;

    void DeleteShaders();

    virtual ~CMaterialNoOutput(void);
    
    typedef struct {
        void *pMesh;
        D3DXMATRIX  ToClip;
        DWORD Index;
    } d3CacheInfo;

    std::vector<d3CacheInfo>    vObjects;
    std::vector<d3CacheInfo>    vMeshes;
public:
    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    ///Const, dest & more
    CMaterialNoOutput(char *ObjName,CEngine *OwnerEngine);

    virtual bool Clone(CDX9Material **pOut);

    virtual bool Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, CBase3DObject *pObject);
    virtual bool Render(CRenderSettings &RenderSettings, CBase3DObject *pObject);
    virtual bool EndRendering(CRenderSettings &RenderSettings);
    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CMaterialNoOutput);
    MAKE_CLSNAME("CMaterialNoOutput");
    MAKE_DEFAULTGARBAGE()
};
