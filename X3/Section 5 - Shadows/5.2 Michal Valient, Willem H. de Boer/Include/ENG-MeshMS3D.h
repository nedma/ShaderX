#pragma once
#include "eng-mesh.h"
#include "sc-DynamicArray.h"

#include "nommgr.h"
#include "d3d9.h"
#include "mmgr.h"

//############################################################################
//Forward declarations
class CHashTable2;

class CMeshMS3D : public CMesh {
private:
    //////////////////////////////////////////////////////////////////////
    // MilkShape structures
    //////////////////////////////////////////////////////////////////////
    #pragma pack( push, defs )
    #pragma pack(1)

    //Header for ms3d file
    typedef struct sMS3D_HeaderTag {
        char    id[10];		// always "MS3D000000"
        int     version;	// 3
    } sMS3D_Header; 

    //Vertex
    typedef struct sMS3D_VertexTag {
        BYTE            flags;			// SELECTED | SELECTED2 | HIDDEN
        float           vertex[3];      //
        char            boneId;         // -1 = no bone
        BYTE            referenceCount;
    } sMS3D_Vertex;

    typedef struct sMS3D_FaceTag {
        WORD    flags;                                      // SELECTED | SELECTED2 | HIDDEN
        WORD    vertexIndices[3];                           //
        float   vertexNormals[3][3];                        //
        float   s[3];                                       //
        float   t[3];                                       //
        BYTE    smoothingGroup;                             // 1 - 32
        BYTE    groupIndex;                                 //
    } sMS3D_Face;

    typedef struct sMS3D_GroupTag {
        BYTE            flags;                              // SELECTED | HIDDEN
        char            name[32];                           //
        WORD            numtriangles;                       //
        WORD            *triangleIndices;                   // the groups group the triangles
        char            materialIndex;                      // -1 = no material
    } sMS3D_Group;

    typedef struct sMS3D_MaterialTag {
        char            name[32];                           //
        float           ambient[4];                         //
        float           diffuse[4];                         //
        float           specular[4];                        //
        float           emissive[4];                        //
        float           shininess;                          // 0.0f - 128.0f
        float           transparency;                       // 0.0f - 1.0f
        char            mode;                               // 0, 1, 2 is unused now
        char            texture[128];                        // texture.bmp
        char            alphamap[128];                       // alpha.bmp
    } sMS3D_Material;

    #pragma pack( pop, defs )

    //My directX vertex bufffer struct.
    // Vertex format for ms3d.
    struct MS3D_VERTEX {
        D3DVECTOR   Position; // The 3D position for the vertex
        D3DVECTOR   Normal; // The 3D position for the vertex normal
        FLOAT       U, V;   // The texture coordinates.
    };

    // FVF combination for that vertex format.
    static const DWORD MS3D_FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

    class CImportMilkShape3D_Vertex : public CRootClass {
    public:
        float Position[3];
        float Normal[3];
        float TexCoord[2];

        bool Equals(CImportMilkShape3D_Vertex *Vertex) {
            if ((Position[0] == Vertex->Position[0]) &&
                (Position[1] == Vertex->Position[1]) &&
                (Position[2] == Vertex->Position[2]) &&
                (Normal[0] == Vertex->Normal[0]) &&
                (Normal[1] == Vertex->Normal[1]) &&
                (Normal[2] == Vertex->Normal[2]) &&
                (TexCoord[0] == Vertex->TexCoord[0]) &&
                (TexCoord[1] == Vertex->TexCoord[1]))
                return TRUE;
            else return FALSE;
        }
    };

    class CImportMilkShape3D_Face : public CRootClass {
    public:
        int Vertexes[3];
    };

    class CImportMilkShape3D_Group : public CRootClass {
    public:
        int GroupID;
        CRootClassArray *Faces;
        CImportMilkShape3D_Group() {
            Faces = new CRootClassArray(true);
        }
        virtual ~CImportMilkShape3D_Group() {
            delete Faces;
        }
    };

    class CImportMilkShape3D_Material : public CRootClass {
    public:
        int MaterialID;
        CRootClassArray *Groups;

        CImportMilkShape3D_Material() {
            Groups = new CRootClassArray(true);
        }

        virtual ~CImportMilkShape3D_Material() {
            delete Groups;
        }
    };

    CRootClassArray     *Vertexes;
    CHashTable2          *Materials;

    int IsThereVertex(CImportMilkShape3D_Vertex *Vertex, CRootClassArray *Vertexes);
public:
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    CMeshMS3D(char *ObjName, CEngine *OwnerEngine);
    MAKE_CLSCREATE(CMeshMS3D);
    MAKE_CLSNAME("CMeshMS3D");
    MAKE_DEFAULTGARBAGE()
};
