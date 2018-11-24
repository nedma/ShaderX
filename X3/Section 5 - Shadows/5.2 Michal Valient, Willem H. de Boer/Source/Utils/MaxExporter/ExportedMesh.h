#pragma once
#include "afx.h"

#pragma pack(1)

static const char   MeshHeader[] = "Dimension3 Mesh File";
static const float  MeshVersion = 2.0f;

typedef struct {
    UINT    MaterialID;
    UINT    MinVertIndex;
    UINT    MaxVertIndex;
    UINT    StartFaceIndex;
    UINT    FaceCount;
} ExportedMeshSubmesh;

typedef struct {
    UINT    Vertex[3];
} ExportedMeshFace;

typedef struct {
    float       Position[3];
    float       Normal[3];
    float       Map[2];
} ExportedMeshVertex;

typedef struct {
    char        Header[32];             //Header magic string "Dimension3 Mesh File" padded to 32 with zeroes
    float       Version;                //Version of mesh
    float       BSphereRadius;          //Center of bounding sphere
    float       BSphereCenter[3];       //x,y,z coordinates of bounding sphere
    UINT        NameLength;             //length of zero terminated name buffer (including zero at the end! strlen(Name) = NameLength-1)
    UINT        VertexCount;            //Number of saved verticles
    UINT        FaceCount;              //Number of saved faces
    UINT        SubMeshCount;           //Number of saved submeshes
} ExportedMeshHeader;

class CExportedMesh : public CObject {
public:
    ExportedMeshHeader      Header;
    char                   *Name;
    ExportedMeshVertex     *Vertex;
    ExportedMeshFace       *Face;
    ExportedMeshSubmesh    *SubMesh;

    CString                 FileName;

    CExportedMesh(void);
    virtual ~CExportedMesh(void);

    bool CreateFromFlexporter(const MeshDescriptor& mesh);
    bool Save(CString *FileName);
};
