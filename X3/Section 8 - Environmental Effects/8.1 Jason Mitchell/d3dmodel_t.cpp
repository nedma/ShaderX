//==================================//
// d3dmodel_t.cpp                    //
//                                  //
// Drew Card                        //
// ATI Research                     //
// Software Engineer                //
// 3D Application Research Group    //
// dcard@ati.com                    //
//==================================//

//==========//
// includes //
//==========//
#include "d3dmodel_t.h"
//#include <image_t.h>
#include <math.h>
#include <stdio.h>
#include <index_t.h>
#include <d3dx9tex.h>

//========//
// macros //
//========//
#define VEC_Add(a, b, c)			((c)[0] = (a)[0] + (b)[0], (c)[1] = (a)[1] + (b)[1], (c)[2] = (a)[2] + (b)[2])
#define VEC_AddTo(a, b)				((a)[0] += (b)[0], (a)[1] += (b)[1], (a)[2] += (b)[2])					
#define VEC_AddScalar(a, b, c)	((c)[0] = (a)[0] + (b), (c)[1] = (a)[1] + (b), (c)[2] = (a)[2] + (b))
#define VEC_AddScalarTo(a, b)		((a)[0] += (b), (a)[1] += (b), (a)[2] += (b))					
#define VEC_Copy(a, b)				((b)[0] = (a)[0], (b)[1] += (a)[1], (b)[2] += (a)[2])					
#define VEC_Cross(a, b, c)			((c)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1], (c)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2], (c)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0])
#define VEC_Divide(a, b, c)		((c)[0] = (a)[0] / (b)[0], (c)[1] = (a)[1] / (b)[1], (c)[2] = (a)[2] / (b)[2])
#define VEC_DivideBy(a, b)			((a)[0] /= (b)[0], (a)[1] /= (b)[1], (a)[2] /= (b)[2])
#define VEC_DivideByScalar(v, s)	((v)[0] /= s, (v)[1] /= s, (v)[2] /= s)	
#define VEC_DivideScalar(v, s, o)	((o)[0] = (v)[0] / s, (o)[1] = (v)[1] / s, (o)[2] = v[2] / s)				
#define VEC_DotProduct(a, b)		((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define VEC_Dot4(a, b)		      ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])
#define VEC_MidPoint(a, b, c)		((c)[0] = (b)[0] + ((a)[0] - (b)[0]) * 0.5f, (c)[1] = (b)[1] + ((a)[1] - (b)[1]) * 0.5f, (c)[2] = (b)[2] + ((a)[2] - (b)[2]) * 0.5f)
#define VEC_Mult(a, b, c)			((c)[0] = (a)[0] * (b)[0], (c)[1] = (a)[1] * (b)[1], (c)[2] = (a)[2] * (b)[2])
#define VEC_MultBy(a, b)			((a)[0] *= (b)[0], (a)[1] *= (b)[1], (a)[2] *= (b)[2])
#define VEC_MultByScalar(v, s)	((v)[0] *= s, (v)[1] *= s, (v)[2] *= s)	
#define VEC_MultScalar(v, s, o)	((o)[0] = (v)[0] * s, (o)[1] = (v)[1] * s, (o)[2] = v[2] * s)				
#define VEC_Negate(a, b)			((b)[0] = -(a)[0], (b)[1] = -(a)[1], (b)[2] = -(a)[2])
#define VEC_Scale(v, s, o)			((o)[0] = (v)[0] * s, (o)[1] = (v)[1] * s, (o)[2] = v[2] * s)				
#define VEC_ScaleBy(v, s)			((v)[0] *= s, (v)[1] *= s, (v)[2] *= s)	
#define VEC_Set(v, x, y, z)		((v)[0] = x, (v)[1] = y, (v)[2] = z)
#define VEC_Subtract(a, b, c)		((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])
#define VEC_SubtractFrom(a, b)	((a)[0] -= (b)[0], (a)[1] -= (b)[1], (a)[2] -= (b)[2])
#define VEC_Magnitude(v)			(sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2]))
#define VEC_Normalize(v)         { double __mag = 1.0 / VEC_Magnitude(v); \
                                   (v)[0] *= __mag; (v)[1] *= __mag; (v)[2] *= __mag; }
#define VEC_Normalizef(v)        { float __mag = 1.0f / (float)VEC_Magnitude(v); \
                                   (v)[0] *= __mag; (v)[1] *= __mag; (v)[2] *= __mag; }

#define FREE(p)                  if (p) { free(p); p = NULL; }
#define PI                       3.14159265358979f
#define D3DFVF_MODEL_T_VERTEX    (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DFVF_FAT_VERTEX        (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2)) 
#define D3DFVF_MODEL_T_NORM_COLOR_VERTEX    (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2 )

//===========//
// functions //
//===========//
void d3dCreateVertexBuffer(mesh_t* mesh, IDirect3DDevice9* d3d);
void d3dCreateIndexBuffer(mesh_t* mesh, IDirect3DDevice9* d3d);

typedef struct 
{
   float    vert[3];
   float    norm[3];
   float    tc[2];
   float    tan[3];
   float    binorm[3];
} fatVertex;

//=================//
// local variables //
//=================//
static D3DMATERIAL9  g_defaultMaterial = 
{
   { 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
   { 0.1f, 0.1f, 0.1f, 1.0f }, // ambient
   { 1.0f, 1.0f, 1.0f, 1.0f }, // specular
   { 0.0f, 0.0f, 0.0f, 1.0f }, // emission
   90.0f                       // shineness
};

void d3dFreeMesh(mesh_t* mesh)
{
   if (mesh->vertexBuffer != 0)
   {
      ((IDirect3DVertexBuffer9*)mesh->vertexBuffer)->Release();
      mesh->vertexBuffer = 0;
   }

   if (mesh->indexBuffer != 0)
   {
      ((IDirect3DIndexBuffer9*)mesh->indexBuffer)->Release();
      mesh->indexBuffer = 0;
   }
}

void d3dFreeMaterial(material_t* material)
{
   if (material->texHandle != 0)
   {
      ((IDirect3DTexture9*)material->texHandle)->Release();
      material->texHandle = 0;
   }
}

//===================================================================//
// d3dFreeModel                                                      //
// frees d3d resources associated with the model                     //
//===================================================================//
void d3dFreeModel(model_t* model)
{
   int      i;

   if (model)
	{
      for (i = 0; i < model->numMaterials; i++)
      {
         d3dFreeMaterial(&model->materials[i]);
      }

      for (i = 0; i < model->numMeshes; i++)
      {
         d3dFreeMesh(model->meshes[i]);
      }
   }
}

void d3dInitializeModel(model_t* model, IDirect3DDevice9* d3d)
{
   int   i;
   char  filename[64];

   for (i = 0; i < model->numMaterials; i++)
   {
      //==============//
      // load texture //
      //==============//
      if (model->materials[i].texName[0] != '\0')
      {
         sprintf(filename, "%s%s", model->path, model->materials[i].texName); 
         D3DXCreateTextureFromFile(d3d, filename, (IDirect3DTexture9**)&model->materials[i].texHandle);
      }
   }

   for (i = 0; i < model->numMeshes; i++)
   {
      d3dCreateVertexBuffer(model->meshes[i], d3d);
      d3dCreateIndexBuffer(model->meshes[i], d3d);
   }
}

void d3dInitializeModelNoMat(model_t* model, IDirect3DDevice9* d3d)
{
   int   i;

   for (i = 0; i < model->numMeshes; i++)
   {
      d3dCreateVertexBuffer(model->meshes[i], d3d);
      d3dCreateIndexBuffer(model->meshes[i], d3d);
   }
}



void d3dActivateMaterial(material_t* mat, IDirect3DDevice9* d3d)
{
   if (mat != NULL)
   {
      D3DMATERIAL9 material;
      memset(&material, 0, sizeof(D3DMATERIAL9));

      memcpy(&material.Diffuse, mat->diffuse, sizeof(float) * 4);
      memcpy(&material.Specular, mat->specular, sizeof(float) * 4);
      memcpy(&material.Ambient, mat->ambient, sizeof(float) * 4);
      memcpy(&material.Emissive, mat->emission, sizeof(float) * 4);
      material.Power = mat->shine; 
      d3d->SetMaterial(&material);

      //======================//
      // activate the texture //
      //======================//
      if (mat->texHandle)
      {
         d3d->SetTexture(0, (IDirect3DBaseTexture9*)mat->texHandle);
         d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
         d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
         d3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
         d3d->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);

         d3d->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
         d3d->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
         d3d->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
      }
      else
      {
         d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
         d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
         d3d->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
         d3d->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
      }
   }
   else
   {
      d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
      d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
      d3d->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
      d3d->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
      d3d->SetMaterial(&g_defaultMaterial);
   }
}

//===================================================================//
// d3dRenderModel                                                    //
// renders the model                                                 //
//===================================================================//
void d3dRenderModel(model_t* model, IDirect3DDevice9* d3d)
{
   int      i;

   if (model)
	{
      for (i = 0; i < model->numMeshes; i++)
      {
         d3dRenderMesh(model->meshes[i], d3d);
      }
   }
}


//===================================================================//
// d3dRenderMesh                                                     //
// renders the mesh using d3d                                        //
//===================================================================//
void d3dRenderMesh(mesh_t* mesh, IDirect3DDevice9* d3d)
{
   if (mesh)
	{
      //======================//
      // let d3d know the fvf //
      //======================//
      //d3d->SetVertexShader(D3DFVF_MODEL_T_VERTEX);
      d3d->SetFVF(D3DFVF_FAT_VERTEX);
      d3d->SetVertexShader(NULL);

      //==========================//
      // set up the vertex stream //
      //==========================//
      //d3d->SetStreamSource(0, (IDirect3DVertexBuffer9*)mesh->vertexBuffer, 0, sizeof(vertex_t));
      d3d->SetStreamSource(0, (IDirect3DVertexBuffer9*)mesh->vertexBuffer, 0, sizeof(fatVertex));

      //====================//
      // set up the indices //
      //====================//
      d3d->SetIndices((IDirect3DIndexBuffer9*)mesh->indexBuffer);

      d3dActivateMaterial(mesh->material, d3d);
      
      //======================//
      // render the triangles //
      //======================//
      d3d->DrawIndexedPrimitive(
         D3DPT_TRIANGLELIST,
         0,
         0,
         mesh->numVerts,
         0,
         mesh->numTris);
   }
}

//===================================================================//
// d3dRenderModelNoMat                                               //
// renders the model                                                 //
//===================================================================//
void d3dRenderModelNoMat(model_t* model, IDirect3DDevice9* d3d)
{
   int      i;

   if (model)
	{
      for (i = 0; i < model->numMeshes; i++)
      {
         d3dRenderMeshNoMat(model->meshes[i], d3d);
      }
   }
}


//===================================================================//
// d3dRenderMeshNoMat                                                //
// renders the mesh using opengl                                     //
//===================================================================//
void d3dRenderMeshNoMat(mesh_t* mesh, IDirect3DDevice9* d3d)
{
   if (mesh)
	{
      //======================//
      // let d3d know the fvf //
      //======================//
      //d3d->SetVertexShader(D3DFVF_MODEL_T_VERTEX);
      d3d->SetFVF(D3DFVF_FAT_VERTEX);
//      d3d->SetVertexShader(NULL);

      //==========================//
      // set up the vertex stream //
      //==========================//
      //d3d->SetStreamSource(0, (IDirect3DVertexBuffer9*)mesh->vertexBuffer, 0, sizeof(vertex_t));
      d3d->SetStreamSource(0, (IDirect3DVertexBuffer9*)mesh->vertexBuffer, 0, sizeof(fatVertex));

      //====================//
      // set up the indices //
      //====================//
      d3d->SetIndices((IDirect3DIndexBuffer9*)mesh->indexBuffer);

      //======================//
      // render the triangles //
      //======================//
      d3d->DrawIndexedPrimitive(
         D3DPT_TRIANGLELIST,
         0,
         0,
         mesh->numVerts,
         0,
         mesh->numTris);
   }
}

//===================================================================//
// d3dRenderModelVS                                                  //
// renders the model                                                 //
//===================================================================//
void d3dRenderModelVS(model_t* model, IDirect3DDevice9* d3d)
{
   int      i;

   if (model)
	{
      for (i = 0; i < model->numMeshes; i++)
      {
         d3dRenderMeshVS(model->meshes[i], d3d);
      }
   }
}


//===================================================================//
// d3dRenderMeshVS                                                   //
// renders the mesh using opengl                                     //
//===================================================================//
void d3dRenderMeshVS(mesh_t* mesh, IDirect3DDevice9* d3d)
{
   if (mesh)
	{
      //==========================//
      // set up the vertex stream //
      //==========================//
      //d3d->SetStreamSource(0, (IDirect3DVertexBuffer9*)mesh->vertexBuffer, 0, sizeof(vertex_t));
      d3d->SetStreamSource(0, (IDirect3DVertexBuffer9*)mesh->vertexBuffer, 0, sizeof(fatVertex));

      //====================//
      // set up the indices //
      //====================//
      d3d->SetIndices((IDirect3DIndexBuffer9*)mesh->indexBuffer);

      //======================//
      // render the triangles //
      //======================//
      d3d->DrawIndexedPrimitive(
         D3DPT_TRIANGLELIST,
         0,
         0,
         mesh->numVerts,
         0,
         mesh->numTris);
   }
}

/*
void d3dCreateVertexBuffer(mesh_t* mesh, IDirect3DDevice9* d3d)
{
   //======================//
   // create vertex buffer //
   //======================//
   d3d->CreateVertexBuffer(
      mesh->numVerts * sizeof(vertex_t),
      D3DUSAGE_WRITEONLY,
      D3DFVF_MODEL_T_VERTEX,
      D3DPOOL_MANAGED,
      (IDirect3DVertexBuffer9**)&mesh->vertexBuffer);

   vertex_t* pVertices;
   if (FAILED (((IDirect3DVertexBuffer9*)mesh->vertexBuffer)->Lock(0, mesh->numVerts * sizeof(vertex_t), (BYTE**)&pVertices, 0)))
   {
      return;
   }

   memcpy(pVertices, mesh->verts, sizeof(vertex_t) * mesh->numVerts);

   ((IDirect3DVertexBuffer9*)mesh->vertexBuffer)->Unlock();
}
*/

void d3dCreateVertexBuffer(mesh_t* mesh, IDirect3DDevice9* d3d)
{
   int   i;

   //======================//
   // create vertex buffer //
   //======================//
   d3d->CreateVertexBuffer(
      mesh->numVerts * sizeof(fatVertex),
      D3DUSAGE_WRITEONLY,
      D3DFVF_FAT_VERTEX,
      D3DPOOL_MANAGED,
      (IDirect3DVertexBuffer9**)&mesh->vertexBuffer,
      NULL);

   fatVertex* pVertices;
   if (FAILED (((IDirect3DVertexBuffer9*)mesh->vertexBuffer)->Lock(0, mesh->numVerts * sizeof(fatVertex), (void**)&pVertices, 0)))
   {
      return;
   }

   for (i = 0; i < mesh->numVerts; i++)
   {
      memcpy(&pVertices[i], &mesh->verts[i], sizeof(vertex_t));
      memcpy(pVertices[i].tan, &mesh->tanSpace[i].tan, sizeof(float) * 6);
   }
   
   ((IDirect3DVertexBuffer9*)mesh->vertexBuffer)->Unlock();
}

void d3dCreateIndexBuffer(mesh_t* mesh, IDirect3DDevice9* d3d)
{
   //=====================//
   // create index buffer //
   //=====================//
   if (FAILED (d3d->CreateIndexBuffer(
         mesh->numTris * 3 * sizeof(long), 
         D3DUSAGE_WRITEONLY, 
         D3DFMT_INDEX32,
         D3DPOOL_MANAGED,
         (IDirect3DIndexBuffer9**)&mesh->indexBuffer,
         NULL)))
   {
      return;
   }
   
   long (*indices)[3];
   if (FAILED (((IDirect3DIndexBuffer9*)mesh->indexBuffer)->Lock (0, mesh->numTris * 3 * sizeof(long), (void**)&indices, 0)))
   {
      return;
   }
   memcpy(indices, mesh->tris, mesh->numTris * 3 * sizeof(long));
   ((IDirect3DIndexBuffer9*)mesh->indexBuffer)->Unlock();
}