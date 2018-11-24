#pragma once
#include "d3f_object.h"

class CD3F_Camera : public CD3F_Object {
    typedef struct {
        DWORD   TAG;
        DWORD   Ortho;
        double  FOV;
        double  NearClip;
        double  FarCLip;
        int     TargetID;
    } SD3F_Camera;
public:
    bool    Ortho;
    double  FOV;
    double  NearClip;
    double  FarCLip;
    int     TargetID;

    CD3F_Camera(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Camera(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Camera");
    MAKE_CLSCREATE(CD3F_Camera);
};
