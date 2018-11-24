//==================================//
// mesh.h                           //
//                                  //
// Drew Card                        //
// ATI Research                     //
// Software Engineer                //
// 3D Application Research Group    //
// dcard@ati.com                    //
//==================================//
#ifndef MODEL_T_H
#define MODEL_T_H

//==========//
// includes //
//==========//
#include <3ds.h>

//==================//
// link this module //
//==================//
//#ifdef _DEBUG
//#pragma comment (lib, "model_t_d.lib")
//#else
//#pragma comment (lib, "model_t.lib")
//#endif

//=========//
// defines //
//=========//
#define MODEL_MAGIC           ('M' | ('D' << 8) | ('L' << 16) | ('1' << 24))
#define MODEL_OK              0
#define MODEL_ERROR           -1
#define MODEL_MEMORY_ERROR    -2
#define MODEL_FILE_ERROR      -3
#define MODEL_BIG_CHUNK       ('C' | ('H' << 8) | ('N' << 16) | ('K' << 24))

#define MESH_MAGIC            ('M' | ('S' << 8) | ('H' << 16) | ('1' << 24))
#define MESH_OK               0
#define MESH_ERROR            -1
#define MESH_MEMORY_ERROR     -2
#define MESH_FILE_ERROR       -3

#define WRITE_TANGENT_SPACE   1
#define WRITE_FACE_NORMALS    2

//===============================//
// single byte structure packing //
//===============================//
#pragma pack (push)
#pragma pack (1)

typedef struct tanSpace_s
{
   union 
   {
      float    m[9];
      struct
      {
         float    tan[3];
         float    binormal[3];
         float    normal[3];
      };
   };
} tanSpace_t;

typedef struct vertex_s
{
   float    v[3];
   float    n[3];
   float    tc[2];
} vertex_t;

typedef struct material_s
{
   float    ambient[4];
   float    diffuse[4];
   float    specular[4];
   float    emission[4];
   float    shine;
   char     texName[32];
   int      texHandle;
   char     bumpName[32];
   int      bumpHandle;
   int      dl;
} material_t;

typedef struct mesh_s
{
   unsigned long  magic;
   unsigned long  size;
   char           name[32];
 	long	         numVerts;
   long	         numTris;
   unsigned long  dl;
   unsigned long  vertexBuffer;
   unsigned long  indexBuffer;
   material_t     *material;
   vertex_t       *verts;
   tanSpace_t     *tanSpace;
   float          (*faceNormals)[4];
   unsigned long  (*tris)[3];
} mesh_t;

typedef struct model_s
{
   unsigned long  magic;
   unsigned long  size;
   unsigned long  info;
   char           path[32];
   long           numTris;
   long           numVerts;
   long           numMaterials;
   long           numMeshes;
   material_t     *materials;
   mesh_t         **meshes;
} model_t;

#pragma pack (pop)

//====================//
// exported functions //
//====================//
void AllocateMesh(mesh_t* mesh);
void FreeMesh(mesh_t* mesh);
int  Load3ds(char* filename, model_t* model);
int  Load3dsNoTex(char* filename, model_t* model);
void FreeModel(model_t* model);
int  LoadModel(char* filename, model_t* model);
int  LoadModelFast(char* filename, model_t** model);
int  SaveModel(char* filename, model_t* model, unsigned int flags);
void CalculateFaceNormals(mesh_t* mesh);


#endif /* MESH_H */


