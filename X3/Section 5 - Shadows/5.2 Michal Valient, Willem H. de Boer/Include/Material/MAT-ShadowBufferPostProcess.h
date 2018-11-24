#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include <vector>
#include "mmgr.h"

#include "material\mat-dx9material.h"

class CBaseTexture;
class CMString;
class CBaseShader;
class CD3SLight;

class CMaterialShadowBufferPostProcess : public CDX9Material {
protected:
    CComPtr<IDirect3DVertexDeclaration9> VSDeclaration;
    CComPtr<IDirect3DVertexBuffer9> VertexBuf;
    CComPtr<ID3DXEffect> Effect;

    D3DXHANDLE  hTechnique;
    D3DXHANDLE  hShadowTexture;

    void DeleteShaders();

    virtual ~CMaterialShadowBufferPostProcess(void);

    DWORD m_dwWidth;
    DWORD m_dwHeight;
public:
    CD3SLight   *pLight;

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    ///Const, dest & more
    CMaterialShadowBufferPostProcess(char *ObjName,CEngine *OwnerEngine);

    virtual bool Clone(CDX9Material **pOut);

    virtual bool Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, CBase3DObject *pObject);
    virtual bool Render(CRenderSettings &RenderSettings, CBase3DObject *pObject);
    virtual bool EndRendering(CRenderSettings &RenderSettings);
    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CMaterialShadowBufferPostProcess);
    MAKE_CLSNAME("CMaterialShadowBufferPostProcess");
    MAKE_DEFAULTGARBAGE()
};
