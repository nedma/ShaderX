#ifndef __VISIBILITY_CALC_H
#define __VISIBILITY_CALC_H

#include "Mesh.h"
#include "SphericalSamples.h"
#include "SHProjection.h"
#include <time.h>
#include "Opcode/Opcode.h"

/// Trim max visibility distance...
#define MAX_VIS_DISTANCE 15.0f


/**
 *  Calculates visibility distance functions for the vertices of finely
 *  tesselated mesh. Can save/load them from file once they are calculated.
 */
class CVisibilityCalculator {
public:
	enum { SQRT_SAMPLES = 50 };

public:
	CVisibilityCalculator()
		: mSphereSamples(SQRT_SAMPLES), mSHSamples(CMesh::SH_ORDER, mSphereSamples)
	{
	}

	/**
	 *  Calculate!
	 */
	void calculate( CMesh& mesh ) {

		// try load from file...
		bool okLoad = tryLoadData( mesh );
		if( okLoad ) {
			OutputDebugString( "Loaded SH data\n" );
			return;
		}
		
		double shcoeffs[CMesh::SH_COEFFS];

		const BYTE* vbSubdiv;
		BYTE* vbSH;
		DWORD strideSubdiv = mesh.getMeshSubdiv().GetNumBytesPerVertex();
		DWORD strideSH = mesh.getMeshSH().GetNumBytesPerVertex();
		mesh.getMeshSubdiv().LockVertexBuffer( D3DLOCK_READONLY, (void**)&vbSubdiv );
		mesh.getMeshSH().LockVertexBuffer( 0, (void**)&vbSH );

		char buf[200];

		int n = mesh.getMeshSubdiv().GetNumVertices();
		mCurrMesh = &mesh;
		
		sprintf( buf, "Total verts: %i\n", n );
		OutputDebugString( buf );

		clock_t t1 = clock();

		// build Opcode collision model from plain (non-subdivided) mesh
		Opcode::MeshInterface opMesh;
		opMesh.SetNbTriangles( mesh.getMeshPlain().GetNumFaces() );
		opMesh.SetNbVertices( mesh.getMeshPlain().GetNumVertices() );
		const BYTE* vbPlain;
		const WORD* ibPlain;
		DWORD stridePlain = mesh.getMeshPlain().GetNumBytesPerVertex();
		mesh.getMeshPlain().LockVertexBuffer( D3DLOCK_READONLY, (void**)&vbPlain );
		mesh.getMeshPlain().LockIndexBuffer( D3DLOCK_READONLY, (void**)&ibPlain );
		SVector3* opVB = new SVector3[opMesh.GetNbVertices()];
		DWORD* opIB = new DWORD[opMesh.GetNbTriangles()*3];
		for( int iv = 0; iv < opMesh.GetNbVertices(); ++iv ) {
			opVB[iv] = *(const SVector3*)vbPlain;
			vbPlain += stridePlain;
		}
		for( int ii = 0; ii < opMesh.GetNbTriangles()*3; ++ii ) {
			opIB[ii] = *ibPlain;
			++ibPlain;
		}
		mesh.getMeshPlain().UnlockVertexBuffer();
		mesh.getMeshPlain().UnlockIndexBuffer();
		opMesh.SetPointers( (const IceMaths::IndexedTriangle*)opIB, (const IceMaths::Point*)opVB );

		Opcode::OPCODECREATE opCreate;
		opCreate.mIMesh = &opMesh;
		opCreate.mSettings.mRules = Opcode::SPLIT_LARGEST_AXIS | Opcode::SPLIT_SPLATTER_POINTS | Opcode::SPLIT_GEOM_CENTER;
		opCreate.mNoLeaf = true;
		opCreate.mQuantized = false;
		opCreate.mKeepOriginal = false;
		opCreate.mCanRemap = true;
		Opcode::Model opModel;
		opModel.Build( opCreate );

		Opcode::RayCollider opRayCol;
		struct Local {
			static void ClosestContact( const Opcode::CollisionFace& hit, void* user_data ) {
				Opcode::CollisionFace* CF = (Opcode::CollisionFace*)user_data;
				if( hit.mDistance < CF->mDistance) *CF = hit;
			}
		};
		opRayCol.SetMaxDist( MAX_VIS_DISTANCE );
		opRayCol.SetTemporalCoherence( false );
		opRayCol.SetCulling( false );
		opRayCol.SetFirstContact( false );
		opRayCol.SetHitCallback( Local::ClosestContact );
		opRayCol.SetUserData( &mCurrFace );

		mCurrRayCol = &opRayCol;
		mCurrModel = &opModel;

		FILE* f = fopen( "shdata.dat", "wb" );
		SFileHeader fhead;
		fhead.mNumVerts = n;
		fhead.mSHOrder = CMesh::SH_ORDER;
		fhead.mSqrtSamples = SQRT_SAMPLES;
		fwrite( &fhead, 1, sizeof(fhead), f );

		// for each vertex...
		for( int i = 0; i < n; ++i ) {
			// output some progress
			if( !(i&127) ) {
				sprintf( buf, "vert %i...\n", i );
				OutputDebugString( buf );
			}
			CMesh::SVertex& svert = *(CMesh::SVertex*)vbSubdiv;
			mCurrPos = svert.p;
			mCurrPos += mesh.getUnifiedNormal(i) * 0.05f;
			sh::projectXYZFunction( *this, svert.n, mSHSamples, shcoeffs );
			CMesh::SSHVertex& shvert = *(CMesh::SSHVertex*)vbSH;
			shvert.p = svert.p;
			shvert.n = svert.n;
			float sh32[CMesh::SH_COEFFS];
			for( int j = 0; j < CMesh::SH_COEFFS; ++j ) {
				float c = (float)shcoeffs[j];
				shvert.sh[j] = c;
				sh32[j] = c;
			}
			// write as float16
			D3DXFLOAT16 sh16[CMesh::SH_COEFFS];
			D3DXFloat32To16Array( sh16, sh32, CMesh::SH_COEFFS );
			fwrite( sh16, 2, CMesh::SH_COEFFS, f );

			vbSubdiv += strideSubdiv;
			vbSH += strideSH;
		}

		clock_t t2 = clock();
		sprintf( buf, "Time: %f\n", (double)(t2-t1)/CLOCKS_PER_SEC );
		OutputDebugString( buf );

		fclose( f );

		delete[] opVB;
		delete[] opIB;

		mesh.getMeshSubdiv().UnlockVertexBuffer();
		mesh.getMeshSH().UnlockVertexBuffer();
	}

	// SHProjection
	double eval( const SVector3& xyz ) {
		// Opcode
		mCurrFace.mDistance = MAX_VIS_DISTANCE;
		IceMaths::Ray ray( *(IceMaths::Point*)&mCurrPos, *(IceMaths::Point*)&xyz );
		mCurrRayCol->Collide( ray, *mCurrModel, NULL, NULL );
		return mCurrFace.mDistance;
		// D3DX
		/*
		BOOL rHit;
		DWORD rFace;
		float rU, rV, rDist = MAX_FLOAT;
		D3DXIntersect( &mCurrMesh->getMeshPlain(), &mCurrPos, &xyz, &rHit, &rFace, &rU, &rV, &rDist, 0, 0 );
		if( rDist > 50.0f ) rDist = 50.0f;
		return rDist;
		*/
	}

private:
	struct SFileHeader {
		int	mNumVerts;
		int	mSHOrder;
		int mSqrtSamples;
	};
	static bool tryLoadData( CMesh& mesh )
	{
		FILE* f = fopen( "shdata.dat", "rb" );
		if( !f )
			return false;
		SFileHeader head;
		fread( &head, 1, sizeof(head), f );
		if( head.mNumVerts != mesh.getMeshSubdiv().GetNumVertices() ||
			head.mSHOrder != CMesh::SH_ORDER ||
			head.mSqrtSamples != SQRT_SAMPLES )
		{
			fclose( f );
			return false;
		}

		BYTE* vbSubdiv;
		BYTE* vbSH;
		DWORD strideSubdiv = mesh.getMeshSubdiv().GetNumBytesPerVertex();
		DWORD strideSH = mesh.getMeshSH().GetNumBytesPerVertex();
		mesh.getMeshSubdiv().LockVertexBuffer( D3DLOCK_READONLY, (void**)&vbSubdiv );
		mesh.getMeshSH().LockVertexBuffer( 0, (void**)&vbSH );

		D3DXFLOAT16 flt16[CMesh::SH_COEFFS];
		int n = mesh.getMeshSubdiv().GetNumVertices();
		for( int i = 0; i < n; ++i ) {
			CMesh::SVertex& svert = *(CMesh::SVertex*)vbSubdiv;
			CMesh::SSHVertex& shvert = *(CMesh::SSHVertex*)vbSH;
			shvert.p = svert.p;
			shvert.n = svert.n;
			// read as float16
			fread( flt16, 2, CMesh::SH_COEFFS, f );
			D3DXFloat16To32Array( shvert.sh, flt16, CMesh::SH_COEFFS );
			vbSubdiv += strideSubdiv;
			vbSH += strideSH;
		}
		mesh.getMeshSubdiv().UnlockVertexBuffer();
		mesh.getMeshSH().UnlockVertexBuffer();

		fclose( f );
		return true;
	}

private:
	CSphericalSamples	mSphereSamples;
	CSHSamples			mSHSamples;

	CMesh*	mCurrMesh;
	SVector3	mCurrPos;
	Opcode::CollisionFace	mCurrFace;
	Opcode::RayCollider*	mCurrRayCol;
	Opcode::Model*			mCurrModel;
};


#endif
