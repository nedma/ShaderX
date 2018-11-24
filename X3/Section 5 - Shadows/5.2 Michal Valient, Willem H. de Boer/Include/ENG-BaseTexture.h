#pragma once

#include "ENG-BaseResource.h"
#include <D3d9.h>

//############################################################################
//Forward declarations

class CBaseTexture : public CBaseResource {
protected:
    virtual void DoCacheIn(CRenderSettings &Settings);
    virtual void DoCacheOut(CRenderSettings &Settings);
    virtual ~CBaseTexture();
public:
    D3DPOOL                 Pool;       //D3DPOOL_XXXX parameter
    DWORD                   Usage;      //D3DUSAGE_XXXX parameter
    D3DFORMAT               Format;     //Format of texture
    int                     LevelCount; //how many mip map levels we have.

    IDirect3DBaseTexture9  *Texture;
    PALETTEENTRY           *Palette;

    //D3DPOOL_XXXX parameter
    D3_INLINE D3DPOOL GetPool() { return Pool; }
    //D3DUSAGE_XXXX parameter
    D3_INLINE UINT GetUsage() { return Usage; }
    //Format of texture
    D3_INLINE D3DFORMAT GetFormat() { return Format; }
    //how many mip map levels we have.
    D3_INLINE int GetLevelCount() { return LevelCount; }

    //Sended, when device was lost. If Texture!=NULL and it needs to be killed, we do it for safe.
    //Descendants should do all other extra processing here.
    virtual void DeviceLost(CRenderSettings &Settings);

    CBaseTexture(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSNAME("CBaseTexture");
};
