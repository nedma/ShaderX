#include "stdafx.h"
#include "Terrain.h"


// --------------------------------------------------------------------------

class CTerrainMeshCreator : public IMeshCreator {
public:
	CTerrainMeshCreator( CTerrain& terrain ) : mTerrain(&terrain) { }
	
	virtual void createMesh( CMesh& mesh ) {
		assert( !mesh.isCreated() );

		// single IB for all static chunks, and all LODs
		// Each cell of a grid is composed of 2 triangles this way:
		//		0----13
		// 		|   / |
		// 		| /   |
		// 		25----4
		// Vertices are assumed to be laid out in rows.
		
		mTerrain->mTrisPerStaticLOD[0] = CHUNK_SEGMENTS * CHUNK_SEGMENTS * 2;
		mTerrain->mStartIndexForStaticLOD[0] = 0;
		int ntris = mTerrain->mTrisPerStaticLOD[0];
		int lod;
		for( lod = 1; lod < LOD_COUNT; ++lod ) {
			mTerrain->mTrisPerStaticLOD[lod] = mTerrain->mTrisPerStaticLOD[lod-1] / 4;
			mTerrain->mStartIndexForStaticLOD[lod] = ntris * 3;
			ntris += mTerrain->mTrisPerStaticLOD[lod];
		}
		int nindices = ntris * 3;
		//hr = dx->CreateIndexBuffer( nindices * sizeof(int), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIB, 0 );

		int vertsPerChunk = (CHUNK_SEGMENTS+1)*(CHUNK_SEGMENTS+1);
		int totalVerts = vertsPerChunk * TOTAL_CHUNKS;

		CVertexFormat vfmt(
			CVertexFormat::V_POSITION |
			//CVertexFormat::V_NORMALDEC
			CVertexFormat::V_NORMALCOL
		);
		mesh.createResource( totalVerts, nindices, vfmt, 2, *RGET_VDECL(vfmt), CMesh::BUF_STATIC );

		initIB( mesh );
		initVB( mesh );
	}

private:
	void initIB( CMesh& mesh );
	void initVB( CMesh& mesh );
	CTerrain*	mTerrain;
};


void CTerrainMeshCreator::initIB( CMesh& mesh )
{
	unsigned short* pib = (unsigned short*)mesh.lockIB( false );
	assert( pib );
	for( int lod = 0; lod < LOD_COUNT; ++lod ) {
		int step = 1<<lod;
		for( int i = 0; i < CHUNK_SEGMENTS; i += step ) {
			for( int j = 0; j < CHUNK_SEGMENTS; j += step ) {
				short base = i * (CHUNK_SEGMENTS+1) + j;
				pib[0] = base;
				pib[1] = base+step;
				pib[2] = base+(CHUNK_SEGMENTS+1)*step;
				pib[3] = base+step;
				pib[4] = base+(CHUNK_SEGMENTS+1)*step+step;
				pib[5] = base+(CHUNK_SEGMENTS+1)*step;
				pib += 6;
			}
		}
	}
	mesh.unlockIB();
};


void CTerrainMeshCreator::initVB( CMesh& mesh )
{
	// VB. each chunk is laid out flat, so chunks in VB are one after
	// another.
	CTerrain::TVertex* pvb = (CTerrain::TVertex*)mesh.lockVB( false );
	assert( pvb );
	
	// chunks
	for( int cy = 0; cy < TERRAIN_CHUNKS; ++cy ) {
		int chunkY = cy * CHUNK_SEGMENTS;
		for( int cx = 0; cx < TERRAIN_CHUNKS; ++cx ) {
			int chunkX = cx * CHUNK_SEGMENTS;
			CAABox& aabb = mTerrain->mChunkAABBs[cy][cx];
			mTerrain->mChunkCenters[cy][cx].set(
				chunkX+CHUNK_SEGMENTS/2-TERRAIN_SEGMENTS/2,
				0.0f,
				-(chunkY+CHUNK_SEGMENTS/2-TERRAIN_SEGMENTS/2)
			);

			// chunk vertices
			for( int y = 0; y <= CHUNK_SEGMENTS; ++y ) {
				int vertY = chunkY + y;
				for( int x = 0; x <= CHUNK_SEGMENTS; ++x ) {
					int vertX = chunkX + x;
					SVector3 p;
					p.x = (vertX - TERRAIN_SEGMENTS/2);
					p.y = mTerrain->mHeightMap[vertY][vertX];
					p.z = -(vertY - TERRAIN_SEGMENTS/2);
					aabb.extend( p );
					pvb->p = p;
					
					// normal dec3n encoded
					/*
					SVector3 n = mTerrain->getNormal( vertY, vertX ) * 511.0f;
					int nx = (int)n.x & 1023;
					int ny = (int)n.y & 1023;
					int nz = (int)n.z & 1023;
					pvb->diffuse = (nx<<0) | (ny<<10) | (nz<<20);
					*/
					// normal color encoded
					SVector3 n = mTerrain->getNormal( vertY, vertX );
					n.x += 1.0f;
					n.y += 1.0f;
					n.z += 1.0f;
					n *= 127.5f;
					int nx = int(n.x) & 255;
					int ny = int(n.y) & 255;
					int nz = int(n.z) & 255;
					pvb->diffuse = (nx<<16) | (ny<<8) | (nz<<0);
					++pvb;
				}
			}
		}
	}

	mesh.unlockVB();
}


// --------------------------------------------------------------------------

void CTerrainRenderable::render( const CRenderContext& ctx )
{
	SVector3 flatEye = G_RENDERCTX->getCamera().getEye3();
	flatEye.y = 0.0f;

	CD3DDevice& dx = CD3DDevice::getInstance();
	CMesh& mesh = *RGET_S_MESH(RID_TERRAIN);

	dx.setIndexBuffer( &mesh.getIB() );
	dx.setVertexBuffer( 0, &mesh.getVB(), 0, mesh.getVertexStride() );
	dx.setDeclaration( mesh.getVertexDecl() );

	// cull and render all chunks
	const int vertsPerChunk = (CHUNK_SEGMENTS+1)*(CHUNK_SEGMENTS+1);
	int chunkBaseVertex = 0;
	for( int y = 0; y < TERRAIN_CHUNKS; ++y ) {
		for( int x = 0; x < TERRAIN_CHUNKS; ++x ) {
			if( !mTerrain->mChunkAABBs[y][x].frustumCull( G_RENDERCTX->getCamera().getViewProjMatrix() ) ) {
				//int lod = 3;
				/*
				const float chd = CHUNK_SEGMENTS*CHUNK_SEGMENTS;
				const float d = SVector3(mChunkCenters[y][x] - flatEye).lengthSq();
				static const float ds[LOD_COUNT] = { 0.0f, 1.5f, 3.0f, 4.7f, 6.7f, 9.0f };
				for( lod = LOD_COUNT-1; lod >= 0; --lod ) {
					if( d > ds[lod]*ds[lod]*chd )
						break;
				}
				*/
				int prims = mTerrain->mTrisPerStaticLOD[mLOD];
				dx.getDevice().DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST,
					chunkBaseVertex, 0, vertsPerChunk,
					mTerrain->mStartIndexForStaticLOD[mLOD], prims
				);
				dx.getStats().incDrawCalls();
				dx.getStats().incPrimsRendered( prims );
				dx.getStats().incVerticesRendered( vertsPerChunk );
			}
			chunkBaseVertex += vertsPerChunk;
		}
	}
}

const CD3DVertexBuffer*	CTerrainRenderable::getUsedVB() const
{
	return &RGET_S_MESH(RID_TERRAIN)->getVB();
}

const CD3DIndexBuffer*	CTerrainRenderable::getUsedIB() const
{
	return &RGET_S_MESH(RID_TERRAIN)->getIB();
}


// --------------------------------------------------------------------------

CTerrain::CTerrain( const char* hmapTex, float height )
{
	HRESULT hr;

	// load heightmap texture
	IDirect3DTexture9* hmapTexture = 0;
	hr = D3DXCreateTextureFromFileEx(
		&CD3DDevice::getInstance().getDevice(),
		hmapTex, TERRAIN_VERTS, TERRAIN_VERTS, 1, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, D3DX_DEFAULT, D3DX_DEFAULT,
		0, NULL, 0, &hmapTexture
	);
	assert( SUCCEEDED(hr) );
	assert( hmapTexture );

	// read heights
	D3DLOCKED_RECT lr;
	hr = hmapTexture->LockRect( 0, &lr, NULL, D3DLOCK_READONLY );
	assert( SUCCEEDED(hr) );
	const BYTE* p = (const BYTE*)lr.pBits;
	float* d = &mHeightMap[0][0];
	for( int y = 0; y < TERRAIN_VERTS; ++y ) {
		for( int x = 0; x < TERRAIN_VERTS; ++x )
			*d++ = ((float)p[x*4+1]) * height / 255.0f;
		p += lr.Pitch;
	}
	hr = hmapTexture->UnlockRect( 0 );
	assert( SUCCEEDED(hr) );

	// release heightmap
	hmapTexture->Release();

	// register terrain mesh
	CSharedMeshBundle::getInstance().registerMesh( RID_TERRAIN, new CTerrainMeshCreator( *this ) );

	mTerrRenderable[BASE] = new CTerrainRenderable( *this );
	mTerrRenderable[COLOR] = new CTerrainRenderable( *this );
	mTerrRenderable[NORMALZ] = new CTerrainRenderable( *this );
}

CTerrain::~CTerrain()
{
}

void CTerrain::render( int lod, eRenderMode renderMode )
{
	mTerrRenderable[renderMode]->setLOD( lod );
	G_RENDERCTX->attach( *mTerrRenderable[renderMode] );
}

SVector3 CTerrain::getNormal( int y, int x ) const
{
	const int F = 3;
	SVector3 n1, n2;
	const float* v = &mHeightMap[y][x];

	if( y<F || x<F || y>=TERRAIN_VERTS-F || x>=TERRAIN_VERTS-F )
		return SVector3(0,1,0);

	float v0 = v[ F]				- v[0];
	float v1 = v[ TERRAIN_VERTS*F]	- v[0];
	float v2 = v[-F]				- v[0];
	float v3 = v[-TERRAIN_VERTS*F]	- v[0];
	
	n1  = SVector3(F,v0,0).cross(SVector3(0,v1,-F)).getNormalized();
	n1 += SVector3(0,v1,-F).cross(SVector3(-F,v2,0)).getNormalized();
	n1 += SVector3(-F,v2,0).cross(SVector3(0,v3,F)).getNormalized();
	n1 += SVector3(0,v3,F).cross(SVector3(F,v0,0)).getNormalized();
	
	n1.normalize();

	const int F2 = 1;

	v0 = v[ F]					- v[0];
	v1 = v[ TERRAIN_VERTS*F]	- v[0];
	v2 = v[-F]					- v[0];
	v3 = v[-TERRAIN_VERTS*F]	- v[0];
	
	n2  = SVector3(F2,v0,0).cross(SVector3(0,v1,-F2)).getNormalized();
	n2 += SVector3(0,v1,-F2).cross(SVector3(-F2,v2,0)).getNormalized();
	n2 += SVector3(-F2,v2,0).cross(SVector3(0,v3,F2)).getNormalized();
	n2 += SVector3(0,v3,F2).cross(SVector3(F2,v0,0)).getNormalized();

	n2.normalize();

	n1.y *= 0.8f;
	n2.y *= 0.8f;

	return SVector3(n1*0.5f+n2*1.0f).getNormalized();
}
