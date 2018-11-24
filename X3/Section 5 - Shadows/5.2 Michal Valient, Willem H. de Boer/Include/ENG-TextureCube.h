#pragma once

#include "ENG-BaseTexture.h"

//############################################################################
//Forward declarations


class CTextureCube : public CBaseTexture {
protected:
    bool                OverrideLoadParams;
    UINT                OLP_Size;   //Width and height
    UINT                OLP_MipLevels;
    DWORD               OLP_Usage;
    D3DFORMAT           OLP_Format;
    D3DPOOL             OLP_Pool;
    DWORD               OLP_Filter;
    DWORD               OLP_MipFilter;
    D3DCOLOR            OLP_ColorKey;

public:
    UINT                Size;
    D3DMULTISAMPLE_TYPE MultiSampleType;

    ///Load data into resource
    ///This implementation uses standard D3DX method D3DXCreateCubeTextureFromFileInMemoryEx
    ///to load texture.
    ///@param LoadIndex - index of data in FileManager.
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);

    ///Resets loading parameters override
    virtual void ResetLoadParams();

    ///Sets loading parameters override
    ///Parameter descriptions are identical to those in D3DXCreateCubeTextureFromFileInMemoryEx function.
    virtual void SetLoadParams(UINT Size, UINT MipLevels, DWORD Usage, D3DFORMAT Format, 
        D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey);

    ///This implementation does nothing
    virtual bool Update(CRenderSettings &RenderSettings);
    
    CTextureCube(char *sObjName, CEngine *pOwnerEngine);

    MAKE_CLSCREATE(CTextureCube);
    MAKE_CLSNAME("CTextureCube");
    MAKE_DEFAULTGARBAGE()
};
