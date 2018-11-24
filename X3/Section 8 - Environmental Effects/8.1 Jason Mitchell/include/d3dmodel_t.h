//==================================//
// glmodel_t.h                      //
//                                  //
// Drew Card                        //
// ATI Research                     //
// Software Engineer                //
// 3D Application Research Group    //
// dcard@ati.com                    //
//==================================//
#ifndef GLMODEL_T_H
#define GLMODEL_T_H

//==========//
// includes //
//==========//
#include <d3d9.h>
#include <model_t.h>

//==================//
// link this module //
//==================//
//#ifdef _DEBUG
//#pragma comment (lib, "d3dmodel_t_d.lib")
//#else
//#pragma comment (lib, "d3dmodel_t.lib")
//#endif

void d3dActivateMaterial(material_t* mat, IDirect3DDevice9* d3d);
void d3dInitializeModel(model_t* model, IDirect3DDevice9* d3d);
void d3dInitializeModelNoMat(model_t* model, IDirect3DDevice9* d3d);  // New - JasonM
void d3dFreeModel(model_t* model);
void d3dRenderMesh(mesh_t* mesh, IDirect3DDevice9* d3d);
void d3dRenderModel(model_t* model, IDirect3DDevice9* d3d);
void d3dRenderMeshNoMat(mesh_t* mesh, IDirect3DDevice9* d3d);
void d3dRenderModelNoMat(model_t* model, IDirect3DDevice9* d3d);
void d3dRenderMeshVS(mesh_t* mesh, IDirect3DDevice9* d3d);
void d3dRenderModelVS(model_t* model, IDirect3DDevice9* d3d);
void d3dRenderNormals(model_t* mesh, IDirect3DDevice9* d3d);
void d3dRenderMeshNormals(mesh_t* mesh, IDirect3DDevice9* d3d);

#endif /* MESH_H */


