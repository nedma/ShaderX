#pragma once

#include "baseclass.h"
#include "SC-MString.h"

class CHashTable2;
class CRootClassArray;
class CD3F_Mesh;

///list of These classes is after load in Meshes array of scene.
///Warn - when saving, there is correct list of meshes (CD3F_Mesh).
class CD3F_MeshLink : public CRootClass {
public:
    int         MeshID;
    CMString    MeshName;
    CMString    MeshFile;
};

class CD3F_Scene : public CBaseClass {
    typedef struct {
        DWORD   TAG;
        int     MeshID;
        char    MeshName[512];
        char    MeshFile[512];
    } SD3F_MeshLink;

    typedef struct {
        char    TAG[64];
        int     Version;
        int     MeshCount;
        int     ObjCount;
        int     LightCount;
        int     CameraCount;
        int     TextureCount;
        int     MaterialCount;
    } SD3F_Scene;
public:
    CRootClassArray *Meshes;
    CHashTable2 *Objects;
    CHashTable2 *Lights;
    CHashTable2 *Cameras;
    CHashTable2 *Textures;
    CHashTable2 *Materials;

    CD3F_Scene(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Scene(void);

    virtual void SaveXML(char *Filename);
    virtual void SaveBIN(char *Filename);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Scene");
    MAKE_CLSCREATE(CD3F_Scene);
};
