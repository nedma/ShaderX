#pragma once

#include "BaseClass.h"
#include "d3d9.h"

//Hardcoded defaults are for GeForce2 GTS class
class CEngineDefaults : public CBaseClass {
public:
    //Vertex buffer defaults
    DWORD       VBuf_Usage;
    D3DPOOL     VBuf_Pool;

    //Index buffer defaults
    DWORD       IBuf_Usage;
    D3DFORMAT   IBuf_Format;
    D3DPOOL     IBuf_Pool;

    //2D Texture creation defaults
    UINT        Txt2D_Width;
    UINT        Txt2D_Height;
    UINT        Txt2D_MipLevels;
    DWORD       Txt2D_Usage;
    D3DFORMAT   Txt2D_Format;
    D3DPOOL     Txt2D_Pool;
    DWORD       Txt2D_Filter;
    DWORD       Txt2D_MipFilter;
    D3DCOLOR    Txt2D_ColorKey;

    //Cube Texture creation defaults
    UINT        TxtCube_Size;
    UINT        TxtCube_MipLevels;
    DWORD       TxtCube_Usage;
    D3DFORMAT   TxtCube_Format;
    D3DPOOL     TxtCube_Pool;
    DWORD       TxtCube_Filter;
    DWORD       TxtCube_MipFilter;
    D3DCOLOR    TxtCube_ColorKey;


    CEngineDefaults(char *ObjName,CEngine *OwnerEngine);
    virtual ~CEngineDefaults(void);

    MAKE_CLSCREATE(CEngineDefaults);
    MAKE_CLSNAME("CEngineDefaults");
    MAKE_DEFAULTGARBAGE();
};
