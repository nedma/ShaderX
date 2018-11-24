#pragma once
#include "baseclass.h"
#include "D3Format\d3f_mesh.h"

class CD3F_Quat : public CRootClass {
    typedef struct {
        DWORD   TAG;
        DWORD   IsSet;
        double  x;
        double  y;
        double  z;
        double  w;
    } SD3F_Quat;
public:
    bool    isSet;
    double  x;
    double  y;
    double  z;
    double  w;

    CD3F_Quat() {
        x = y = z = w = 0;
        isSet = false;
    }
    CD3F_Quat(double pX, double pY, double pZ, double pW) {
        Set(pX,pY,pZ,pW);
    }
    void Set(double pX, double pY, double pZ, double pW) {
        x = pX;
        y = pY;
        z = pZ;
        w = pW;
        isSet = true;
    }

    void SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_PosRotScale : public CRootClass {
    typedef struct {
        DWORD   TAG;
    } SD3F_PosRotScale;
public:
    CD3F_Coord      Pos;
    CD3F_Quat       Rot;
    CD3F_Coord      Scale;

    void SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_Object : public CBaseClass {
    typedef struct {
        DWORD   TAG;
        char    Name[512];
        int     ID;
        int     ParentID;
        DWORD   IsInstance;
        DWORD   IsTarget;
        int     MasterID;
        int     MeshID;
    } SD3F_Object;
public:
    int         Index;  //index of object in scene list
    int         ID;     //ID of object
    int         ParentID;   //ID of parent object
    bool        IsInstance;    //true if object is instance of other object
    bool        IsTarget;      //true if it is target
    int         MasterID;       //ID of master object (if instanced)
    int         MeshID;         //ID of mesh (valid also for instances)
    CMString    Name;      //name of object

    CD3F_PosRotScale    LocalPivot;     //Pivot in local space
    CD3F_PosRotScale    WorldPivot;     //Pivot in world space

    CD3F_Object(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Object(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Object");
    MAKE_CLSCREATE(CD3F_Object);
};
