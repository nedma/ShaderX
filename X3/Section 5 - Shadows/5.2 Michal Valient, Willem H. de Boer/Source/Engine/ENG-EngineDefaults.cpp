#include "stdafx.h"
#include "eng-enginedefaults.h"

#include "nommgr.h"
#include <D3dx9.h>
#include "mmgr.h"

CEngineDefaults::CEngineDefaults(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    //Vertex buffer defaults
    VBuf_Usage = D3DUSAGE_WRITEONLY;
//    VBuf_Usage = D3DUSAGE_SOFTWAREPROCESSING;
    VBuf_Pool = D3DPOOL_MANAGED;

    //Index buffer defaults
    IBuf_Usage = D3DUSAGE_WRITEONLY;
//    IBuf_Usage = D3DUSAGE_SOFTWAREPROCESSING;
    IBuf_Format = D3DFMT_INDEX16;
    IBuf_Pool = D3DPOOL_MANAGED;

    //2D Texture creation defaults
    Txt2D_Width = D3DX_DEFAULT;
    Txt2D_Height = D3DX_DEFAULT;
    Txt2D_MipLevels = D3DX_DEFAULT;
    Txt2D_Usage = 0;
    Txt2D_Format = D3DFMT_UNKNOWN;
    Txt2D_Pool = D3DPOOL_MANAGED;
    Txt2D_Filter = D3DX_FILTER_TRIANGLE;
    Txt2D_MipFilter = D3DX_FILTER_TRIANGLE;
    Txt2D_ColorKey = 0;

    //Cube Texture creation defaults
    TxtCube_Size = D3DX_DEFAULT;
    TxtCube_MipLevels = D3DX_DEFAULT;
    TxtCube_Usage = 0;
    TxtCube_Format = D3DFMT_UNKNOWN;
    TxtCube_Pool = D3DPOOL_MANAGED;
    TxtCube_Filter = D3DX_FILTER_TRIANGLE;
    TxtCube_MipFilter = D3DX_FILTER_TRIANGLE;
    TxtCube_ColorKey = 0;
}

CEngineDefaults::~CEngineDefaults(void) {
}
