//
// Direct3D Extension Mesh functions (for ShaderX³)
//
//	written 2004 by Ronny Burkersroda
//

#ifndef						_MESH_FUNCTIONS_H__INCLUDED_
#define						_MESH_FUNCTIONS_H__INCLUDED_


#pragma once


// header includes
#include					<d3d9.h>									// Direct3D
#include					<d3dx9.h>									// Direct3D extensions


// D3DX mesh functions namespace
namespace					D3DXMeshFunctions
{


	HRESULT						BuildMorphingMesh(							// create a body with mesh from the bodies of a scene hierarchy and project onto the meshes of another hierarchy
		ID3DXMesh*					pmeshSource,								// pointer to interface of the mesh including source vertex positions
		ID3DXMesh*					pmeshTarget,								// pointer to interface of the mesh including target vertex positions
		const D3DVECTOR*			pvctCenter,									// pointer to center vector
		ID3DXMesh**					ppmeshMorphing								// pointer to store morphing mesh interface pointer to
		);


}


#endif