//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  Bounding.h
// 
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//-----------------------------------------------------------------------------


#ifndef __BOUNDING_H_included_
#define __BOUNDING_H_included_

#include <d3dx9.h>
#include <d3dx9math.h>
#include <math.h>
#include <vector>
#include <shared/NVBScene9.h>

class BoundingBox;
class ExtrudedBoundingBox;

/////////////////////////////////////////////////////////////////////////////////////////
//  CLASS FRUSTUM
//    Defines a view frustum (6 planes), capable of performing culling against a variety of
//    different primitives -- points, edges, axis-aligned bounding boxes, spheres,
//    other frusta, and extruded AABBs
//
//    a frustum can be built from an arbitrary matrix
/////////////////////////////////////////////////////////////////////////////////////////
class Frustum
{
public:
    Frustum();
    Frustum( const D3DXMATRIX* matrix ) { BuildFrustumFromMatrix( matrix ); }

    int  TestBox        ( const BoundingBox* box ) const;
    BOOL TestPoint      ( const D3DXVECTOR3* pnt ) const { return TestSphere( pnt, 0.f ); }
    BOOL TestEdge       ( const D3DXVECTOR3* a, const D3DXVECTOR3* b ) const;
    BOOL TestFrustum    ( const Frustum* other ) const;
    BOOL TestSphere     ( const D3DXVECTOR3* pnt, float rad ) const;
	BOOL TestExtrudedBox( const ExtrudedBoundingBox* box ) const;
   
    D3DXVECTOR3 pntList[8];
    D3DXPLANE camPlanes[6];
    int nVertexLUT[6];
protected:
    void BuildFrustumFromMatrix( const D3DXMATRIX* matrix );
};

///////////////////////////////////////////////////////////////////////////////////////////
//  CLASS BOUNDINGBOX
//    Defines an axis-aligned bounding box class, with a minimum and maximum point.
///////////////////////////////////////////////////////////////////////////////////////////
class BoundingBox
{
public:
    D3DXVECTOR3 minPt;
    D3DXVECTOR3 maxPt;

    BoundingBox(): minPt(1.e20f, 1.e20f, 1.e20f), maxPt(-1.e20f, -1.e20f, -1.e20f) { }
    BoundingBox( const BoundingBox& other ): minPt(other.minPt), maxPt(other.maxPt) { }
    
    bool TestPoint( const D3DXVECTOR3* _pt ) const;
    void Clamp( const D3DXVECTOR3* _min, const D3DXVECTOR3* _max );
    void Centroid( D3DXVECTOR3* vec) const { *vec = 0.5f*(minPt+maxPt); }
    void Merge( const D3DXVECTOR3* vec );
};

//////////////////////////////////////////////////////////////////////////////////////////////
//  CLASS EXTRUDEDBOUNDINGBOX
//    Defines an extruded bounding box -- a bounding box, and an emanation point.  the result is
//    similar to a shadow volume.  capable of intersection testing against points and edges.
//
//    an extruded AABB is built from an AABB, an emanation point, and an emanation length.
//////////////////////////////////////////////////////////////////////////////////////////////
class ExtrudedBoundingBox
{
public:
    BoundingBox srcBox;
	D3DXVECTOR3 projectionPoint;
	float length;
	int nEdges;
	D3DXPLANE extrudedPlanes[6];
	DWORD silhouetteEdges[6][2];


	ExtrudedBoundingBox(): srcBox(), nEdges(0), length(0.f), projectionPoint(0.f, 0.f, 0.f) { }
	ExtrudedBoundingBox( const ExtrudedBoundingBox& other );
	ExtrudedBoundingBox( const BoundingBox* box, const D3DXVECTOR3* pnt, float len );

	BOOL TestPoint( const D3DXVECTOR3* _pt ) const;
	BOOL TestEdge( const D3DXVECTOR3* a, const D3DXVECTOR3* b ) const;
};




void GetSceneBoundingBox( BoundingBox* box, std::vector<BoundingBox>* modelBoxen, const NVBScene* scene );
void GetModelBoundingBox( BoundingBox* box, const NVBScene::Mesh* mesh );
void XFormBoundingBox( BoundingBox* result, const BoundingBox* src, const D3DXMATRIX* matrix );


#endif