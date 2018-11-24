#pragma once
#include "d3f_object.h"

class CD3F_Texture : public CBaseClass {
    typedef struct {
        DWORD   TAG;
        int     ID;
        char    FileName[512];    
    } SD3F_Texture;
public:
    int         ID;
    CMString    FileName;

    CD3F_Texture(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Texture(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Texture");
    MAKE_CLSCREATE(CD3F_Texture);
};
