#pragma once
#include "nommgr.h"
#include "atlcomcli.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "mat-basematerial.h"

class CMString;
class CRenderSettings;
class CXMLNode;

class CDX9MaterialContext : public CBaseMaterialContext {
public:
};

class CDX9Material : public CBaseMaterial {
protected:
    virtual ~CDX9Material(void);
public:
    CDX9Material(char *ObjName,CEngine *OwnerEngine);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings) = 0;

    ///Create new material. Assign an ID for it, register it to the manager
    virtual bool Clone(CDX9Material **pOut) = 0;

    HRESULT LoadFromSource(const char *pszFileName, CRenderSettings &RenderSettings, ID3DXEffect **pOut);
    HRESULT LoadFromBuffer(char *pszBuffer, DWORD dwSize, CRenderSettings &RenderSettings, ID3DXEffect **pOut);

    MAKE_CLSNAME("CDX9Material");
};
