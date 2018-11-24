#pragma once
#include "math.h"

#include "baseclass.h"
#include "SC-MString.h"

///Forwards
class CRootClassArray;

static const double D3F_FUZZY_EQUAL = 0.00001;

class CD3F_Coord : public CRootClass {
    typedef struct {
        DWORD   TAG;
        DWORD   IsSet;
        double  x;
        double  y;
        double  z;
    } SD3F_Coord;
public:
    bool IsSet;
    double x;
    double y;
    double z;

    void Set(double pX, double pY, double pZ) { 
        x = pX;
        y = pY;
        z = pZ;
        IsSet = true;
    }
    CD3F_Coord() {
        Set(0,0,0);
        IsSet = false;
    }
    CD3F_Coord(double pX, double pY, double pZ) {
        Set(pX,pY,pZ);
    }
    bool Equals(CD3F_Coord &Coord) {
        if ((fabs(Coord.x - x) < D3F_FUZZY_EQUAL) && (fabs(Coord.y - y) < D3F_FUZZY_EQUAL) && (fabs(Coord.z - z) < D3F_FUZZY_EQUAL)) return true;
        else return false;
    }
    virtual void SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_MapCoord : public CRootClass {
    typedef struct {
        DWORD   TAG;
        DWORD   IsSet;
        double  u;
        double  v;
    } SD3F_MapCoord;
public:
    bool IsSet;
    double u;
    double v;

    void Set(double pU, double pV) {
        u = pU;
        v = pV;
        IsSet = true;
    }
    CD3F_MapCoord() {
        Set(0,0);
        IsSet = false;
    }
    CD3F_MapCoord(double pU, double pV) {
        Set(pU,pV);
    }
    bool Equals(CD3F_MapCoord &Coord) {
        if ((fabs(Coord.u - u) < D3F_FUZZY_EQUAL) && (fabs(Coord.v - v) < D3F_FUZZY_EQUAL)) return true;
        else return false;
    }
    virtual void SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_Vertex : public CRootClass {
    typedef struct {
        DWORD   TAG;
    } SD3F_Vertex;
public:
    DWORD           Index;
    CD3F_Coord      Pos;
    CD3F_MapCoord   Map;
    CD3F_Coord      Norm;

    bool Equals(CD3F_Vertex *V) {
        if (V == NULL) return false;
        if ((Pos.Equals(V->Pos)) && (Map.Equals(V->Map)) && (Norm.Equals(V->Norm)))  return true;
        else return false;
    }
    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_Face : public CRootClass {
    typedef struct {
        DWORD               TAG;
        DWORD   VertexIndex[3];
    } SD3F_Face;
public:
    DWORD       Index;
    __int64       VertexIndex[3];
    CD3F_Face() {
        VertexIndex[0] = VertexIndex[1] = VertexIndex[2] = -1;
    }
    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_Submesh : public CRootClass {
    typedef struct {
        DWORD       TAG;
        DWORD       MinVertIndex;      
        DWORD       MaxVertIndex;      
        DWORD       StartFaceIndex;    
        DWORD       FaceCount;
        int         MaterialID;
    } SD3F_Submesh;
public:
    DWORD       Index;
    DWORD       MinVertIndex;       ///Minimum vertex index in Vertexes array
    DWORD       MaxVertIndex;       ///Maximum vertex index in Vertexes array
    DWORD       StartFaceIndex;     ///Starting face
    DWORD       FaceCount;          ///Ending face
    int         MaterialID;

    CD3F_Submesh() {
        MinVertIndex = MAXDWORD;
        MaxVertIndex = 0;
        StartFaceIndex = 0;
        FaceCount = 0;
    }
    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);
};

class CD3F_Mesh : public CBaseClass {
    typedef struct {
        char                TAG[64];
        int                 Version;
        char                Name[512];
        double              BSphereRadius;
        DWORD               VertexCount;
        DWORD               FaceCount;
        DWORD               SubCount;
    } SD3F_Mesh;
public:
    DWORD               Index;
    CMString            Name;
    CMString            FileName;       ///Filled in export. used in scene save.

    double              BSphereRadius;
    CD3F_Coord          BSphereCenter;
    CRootClassArray     *Vertexes;
    CRootClassArray     *Faces;
    CRootClassArray     *SubMeshes;

    CD3F_Mesh(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Mesh(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Mesh");
    MAKE_CLSCREATE(CD3F_Mesh);
};
