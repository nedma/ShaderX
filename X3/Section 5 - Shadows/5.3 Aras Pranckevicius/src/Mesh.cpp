#include "stdafx.h"

#include "Mesh.h"

CMesh::CMesh()
:	mMeshPlain(0), mMeshSubdiv(0), mMeshSH(0),
	mAdjacency(0), mUnifiedNormals(0)
{
}

void CMesh::release()
{
	DXSAFE_RELEASE( mMeshPlain );
	DXSAFE_RELEASE( mMeshSubdiv );
	DXSAFE_RELEASE( mMeshSH );
	DXSAFE_RELEASE( mAdjacency );
	DXSAFE_DELETE_ARRAY( mUnifiedNormals );
}

HRESULT CMesh::create( IDirect3DDevice9* dx, const char* filePlain, const char* fileSubdiv )
{
	HRESULT hr;

	// release old meshes
	release();

	//
	// load meshes

	hr = D3DXLoadMeshFromX( filePlain, D3DXMESH_MANAGED, dx, NULL, NULL, NULL, NULL, &mMeshPlain );
	if( FAILED(hr) || (mMeshPlain == NULL) )
		return hr;
	hr = D3DXLoadMeshFromX( fileSubdiv, D3DXMESH_MANAGED, dx, &mAdjacency, NULL, NULL, NULL, &mMeshSubdiv );
	if( FAILED(hr) || (mMeshSubdiv == NULL) )
		return hr;

	//
	// compute unified normals on subdivided mesh

	int nverts = mMeshSubdiv->GetNumVertices();
	DWORD vstride = mMeshSubdiv->GetNumBytesPerVertex();
	const BYTE *vb = 0;
	mMeshSubdiv->LockVertexBuffer( D3DLOCK_READONLY, (void**)&vb );
	mUnifiedNormals = new SVector3[ nverts ];
	int i;
	DWORD *preps = new DWORD[ nverts ];
	mMeshSubdiv->ConvertAdjacencyToPointReps( (const DWORD*)mAdjacency->GetBufferPointer(), preps );
	// set zero normals
	for( i = 0; i < nverts; ++i ) {
		mUnifiedNormals[i].set( 0, 0, 0 );
	}
	// accumulate normals
	for( i = 0; i < nverts; ++i ) {
		mUnifiedNormals[preps[i]] += ((const SVertex*)vb)->n;
		vb += vstride;
	}
	// normalize normals
	for( i = 0; i < nverts; ++i ) {
		mUnifiedNormals[i].normalize();
	}
	// replicate from preps to all vertices
	for( i = 0; i < nverts; ++i ) {
		mUnifiedNormals[i] = mUnifiedNormals[preps[i]];
	}
	mMeshSubdiv->UnlockVertexBuffer();
	delete[] preps;

	//
	// clone subdivided into SH mesh

	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE] = {
		{0,  0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		// SH coeffs
		// first 9
		{0,  24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		{0,  40, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1},
		{0,  56, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 2},
		// last 16
		{0,  60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 3},
		{0,  76, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 4},
		{0,  92, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 5},
		{0,  108, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 6},
		D3DDECL_END()
	};
	
	hr = mMeshSubdiv->CloneMesh( mMeshSubdiv->GetOptions(), decl, dx, &mMeshSH );
	if( FAILED(hr) || !mMeshSH )
		return hr;

	return S_OK;
}
