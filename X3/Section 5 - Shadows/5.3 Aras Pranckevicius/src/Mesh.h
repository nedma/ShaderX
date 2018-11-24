#ifndef __MESH_H
#define __MESH_H

#include "math/Vector3.h"

// --------------------------------------------------------------------------

// Well, actually "whole scene stuff"...
class CMesh {
public:
	/// Use 5th SH order, so 25 coeffs
	enum { SH_ORDER = 5, SH_COEFFS=SH_ORDER*SH_ORDER };
	/// Simple vertex: position, normal
	struct SVertex {
		SVector3	p;
		SVector3	n;
	};
	/// SH vertex: position, normal, SH coeffs
	struct SSHVertex {
		SVector3	p;
		SVector3	n;
		float		sh[SH_COEFFS];
	};

public:
	CMesh();
	~CMesh() { release(); }

	void release();
	HRESULT create( IDirect3DDevice9* dx, const char* filePlain, const char* fileSubdiv );

	/// Plain low-poly mesh
	ID3DXMesh&	getMeshPlain() { return *mMeshPlain; }
	/// Subdivided mesh
	ID3DXMesh&	getMeshSubdiv() { return *mMeshSubdiv; }
	/// Subdivided mesh, with SH coeffs
	ID3DXMesh&	getMeshSH() { return *mMeshSH; }

	/// Get unified normal of subdivided mesh
	const SVector3& getUnifiedNormal( int i ) const { return mUnifiedNormals[i]; }

private:
	// non-subdivided mesh
	ID3DXMesh*		mMeshPlain;
	// subdivided mesh
	ID3DXMesh*		mMeshSubdiv;
	ID3DXBuffer*	mAdjacency;
	SVector3*		mUnifiedNormals;
	// SH mesh
	ID3DXMesh*		mMeshSH;
};


#endif
