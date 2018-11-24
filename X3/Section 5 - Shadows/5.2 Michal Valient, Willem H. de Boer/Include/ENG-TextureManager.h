#pragma once

#include "ENG-ResourceManager.h"
#include <D3d9.h>

//############################################################################
//Forward declarations
class CBaseTexture;

class CTextureManager : public CResourceManager {
protected:
    class CTextureInfo : public CRootClass {
    public:
        DWORD                   Stage;
        IDirect3DBaseTexture9  *Textures;

        CTextureInfo();
        ~CTextureInfo();
    };

    CHashTable2 *GuardTextures;
public:
    CTextureManager(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTextureManager();
    virtual void DeviceLost(CRenderSettings &Settings);

    MAKE_CLSCREATE(CTextureManager);
    MAKE_CLSNAME("CTextureManager");
    MAKE_DEFAULTGARBAGE();

    ///GUARDBuffer DX8::SetTexture method
    bool SetTexture(DWORD Stage, CBaseTexture *Texture);
};
