#ifndef __TERRAIN_H
#define __TERRAIN_H

#include <dingus/renderer/RenderableMesh.h>
#include <dingus/gfx/Vertices.h>
#include "Resources.h"


enum {
	CHUNK_SEGMENTS = 32,
	TERRAIN_SEGMENTS = 512,
	TERRAIN_VERTS = TERRAIN_SEGMENTS+1,
	TERRAIN_CHUNKS = TERRAIN_SEGMENTS / CHUNK_SEGMENTS,
	TOTAL_CHUNKS = TERRAIN_CHUNKS * TERRAIN_CHUNKS,
	LOD_COUNT = 6, // whatever 2^N is CHUNK_SEGMENTS, plus one
};

class CTerrain;


class CTerrainRenderable : public CRenderable {
public:
	CTerrainRenderable( CTerrain& terrain ) : mTerrain(&terrain), mLOD(0) { };
	void	setLOD( int lod ) { mLOD = lod; }

	virtual void render( const CRenderContext& ctx );
	virtual const CD3DVertexBuffer*	getUsedVB() const;
	virtual const CD3DIndexBuffer*	getUsedIB() const;

private:
	CTerrain*	mTerrain;
	int			mLOD;
};



class CTerrain {
public:
	typedef SVertexXyzDiffuse	TVertex;
public:
	CTerrain( const char* hmapTex, float height );
	~CTerrain();
	
	void render( int lod, eRenderMode renderMode );

	SVector3 getNormal( int y, int x ) const;

	CEffectParams& getParams( eRenderMode rm ) { return mTerrRenderable[rm]->getParams(); }

private:
	CTerrainRenderable*	mTerrRenderable[MAX_MODES-1]; // base, color, normal/z

public:
	float		mHeightMap[TERRAIN_VERTS][TERRAIN_VERTS];
	CAABox		mChunkAABBs[TERRAIN_CHUNKS][TERRAIN_CHUNKS];
	SVector3	mChunkCenters[TERRAIN_CHUNKS][TERRAIN_CHUNKS];
	int			mTrisPerStaticLOD[LOD_COUNT];
	int			mStartIndexForStaticLOD[LOD_COUNT];
};


#endif
