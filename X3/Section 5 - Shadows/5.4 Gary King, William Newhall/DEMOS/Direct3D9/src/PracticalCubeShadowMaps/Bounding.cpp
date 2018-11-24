//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  Bounding.cpp
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

#include "Bounding.h"
//  include utility functions
#include "BoundingUtil.h"
/////////////////////////////////////////////////////////////////////////////////

void BoundingBox::Merge(const D3DXVECTOR3* vec)
{
    minPt.x = min(minPt.x, vec->x);
    minPt.y = min(minPt.y, vec->y);
    minPt.z = min(minPt.z, vec->z);
    maxPt.x = max(maxPt.x, vec->x);
    maxPt.y = max(maxPt.y, vec->y);
    maxPt.z = max(maxPt.z, vec->z);
}

void BoundingBox::Clamp( const D3DXVECTOR3* _min, const D3DXVECTOR3* _max )
{
    minPt.x = max(_min->x, minPt.x);
    minPt.y = max(_min->y, minPt.y);
    minPt.z = max(_min->z, minPt.z);
    minPt.x = min(_max->x, minPt.x);
    minPt.y = min(_max->y, minPt.y);
    minPt.z = min(_max->z, minPt.z);


    maxPt.x = min(_max->x, maxPt.x);
    maxPt.y = min(_max->y, maxPt.y);
    maxPt.z = min(_max->z, maxPt.z);
    maxPt.x = max(_min->x, maxPt.x);
    maxPt.y = max(_min->y, maxPt.y);
    maxPt.z = max(_min->z, maxPt.z);
}

bool BoundingBox::TestPoint( const D3DXVECTOR3* _pt ) const
{
    return ( (_pt->x>=minPt.x) &&
             (_pt->y>=minPt.y) &&
             (_pt->z>=minPt.z) &&
             (_pt->x<=maxPt.x) &&
             (_pt->y<=maxPt.y) &&
             (_pt->z<=maxPt.z) );
}

//---------------------------------------------------------------------------
//  Frustum class

Frustum::Frustum() 
{
    for (int i=0; i<6; i++)
        camPlanes[i] = D3DXPLANE(0.f, 0.f, 0.f, 0.f);
}

void Frustum::BuildFrustumFromMatrix( const D3DXMATRIX* matrix )
{
    //  build a view frustum based on the current view & projection matrices...
    D3DXVECTOR4 column4( matrix->_14, matrix->_24, matrix->_34, matrix->_44 );
    D3DXVECTOR4 column1( matrix->_11, matrix->_21, matrix->_31, matrix->_41 );
    D3DXVECTOR4 column2( matrix->_12, matrix->_22, matrix->_32, matrix->_42 );
    D3DXVECTOR4 column3( matrix->_13, matrix->_23, matrix->_33, matrix->_43 );

    D3DXVECTOR4 planes[6];
    planes[0] = column4 - column1;  // left
    planes[1] = column4 + column1;  // right
    planes[2] = column4 - column2;  // bottom
    planes[3] = column4 + column2;  // top
    planes[4] = column4 - column3;  // near
    planes[5] = column4 + column3;  // far
    
    for (int p=0; p<6; p++)
    {
        camPlanes[p] = D3DXPLANE( planes[p].x, planes[p].y, planes[p].z, planes[p].w );
        D3DXPlaneNormalize( &camPlanes[p], &camPlanes[p] );
    }

    //  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
    for (int i=0; i<6; i++)
        nVertexLUT[i] = ((camPlanes[i].a<0.f)?1:0) | ((camPlanes[i].b<0.f)?2:0) | ((camPlanes[i].c<0.f)?4:0);

    for (int i=0; i<8; i++)  // compute extrema
    {
        const D3DXPLANE& p0 = (i&1)?camPlanes[4] : camPlanes[5];
        const D3DXPLANE& p1 = (i&2)?camPlanes[3] : camPlanes[2];
        const D3DXPLANE& p2 = (i&4)?camPlanes[0] : camPlanes[1];

        PlaneIntersection( &pntList[i], &p0, &p1, &p2 );
    }
}

//  Tests if an AABB is inside/intersecting the view frustum
int Frustum::TestBox( const BoundingBox* box ) const
{
    bool intersect = false;

    for (int i=0; i<6; i++)
    {
        int nV = nVertexLUT[i];
        // pVertex is diagonally opposed to nVertex
        D3DXVECTOR3 nVertex( (nV&1)?box->minPt.x:box->maxPt.x, (nV&2)?box->minPt.y:box->maxPt.y, (nV&4)?box->minPt.z:box->maxPt.z );
        D3DXVECTOR3 pVertex( (nV&1)?box->maxPt.x:box->minPt.x, (nV&2)?box->maxPt.y:box->minPt.y, (nV&4)?box->maxPt.z:box->minPt.z );

        if ( D3DXPlaneDotCoord(&camPlanes[i], &nVertex) < 0.f )
            return 0;
        if ( D3DXPlaneDotCoord(&camPlanes[i], &pVertex) < 0.f )
            intersect = true;
    }

    return (intersect)?2 : 1;
}

//  test if a sphere is inside the view frustum
BOOL Frustum::TestSphere( const D3DXVECTOR3* pnt, float radius ) const
{
    for (int i=0; i<6; i++)
    {
        if ( D3DXPlaneDotCoord(&camPlanes[i], pnt)+radius < 0.f )
            return FALSE;
    }
    return TRUE;
}

//  test if a line segment is fully or partially inside the view frustum
BOOL Frustum::TestEdge( const D3DXVECTOR3* a, const D3DXVECTOR3* b ) const
{
    const FLOAT fFrustumEdgeFudge = 0.01f;  //  because sometimes, 32-bit floating point just isn't enough
    unsigned char outCode[2] = { 0, 0 };

    for ( int i=0; i<6; i++ )
    {
        FLOAT aDist = D3DXPlaneDotCoord( &camPlanes[i], a );
        FLOAT bDist = D3DXPlaneDotCoord( &camPlanes[i], b );
        INT aSide = (aDist <= 0) ? 1 : 0;
        INT bSide = (bDist <= 0) ? 1 : 0;

        outCode[0] |= (aSide << i);  // outcode stores the plane sidedness for all 6 clip planes
        outCode[1] |= (bSide << i);  // if outCode[0]==0, vertex is inside frustum

        if ( aSide & bSide )   // trivial reject test (both points outside of one plane)
            return FALSE;

        if (aSide ^ bSide)      // complex intersection test
        {
            D3DXVECTOR3 rayDir = *b - *a;
            float t = RayPlaneIntersect( &camPlanes[i], a, &rayDir );
            if ((t>=0.f) && (t<=1.f))
            {
                D3DXVECTOR3 hitPt = *a + t*rayDir;
                if (TestSphere(&hitPt, fFrustumEdgeFudge))
                    return TRUE;  // intersects frustum
            }
        }
    }
    return (outCode[0]==0) || (outCode[1]==0);
}

BOOL Frustum::TestExtrudedBox( const ExtrudedBoundingBox *box ) const
{
	//  build an imaginary sphere around the light, representing the volume of
	//  max attenuation -- if this doesn't intersect the view frustum, then
	//  this caster can be trivially rejected
	if ( !TestSphere(&box->projectionPoint, box->length) )
		return FALSE;

	BOOL intersectA = (box->TestEdge( &pntList[l_b_f], &pntList[l_b_n] )) ||
					  (box->TestEdge( &pntList[l_b_n], &pntList[r_b_n] )) ||
					  (box->TestEdge( &pntList[r_b_n], &pntList[r_b_f] )) ||
					  (box->TestEdge( &pntList[r_b_f], &pntList[l_b_f] )) ||
					  (box->TestEdge( &pntList[r_b_f], &pntList[l_t_f] )) ||
					  (box->TestEdge( &pntList[r_b_n], &pntList[r_t_n] )) ||
					  (box->TestEdge( &pntList[l_b_f], &pntList[l_t_f] )) ||
					  (box->TestEdge( &pntList[l_b_n], &pntList[l_t_n] )) ||
					  (box->TestEdge( &pntList[l_t_n], &pntList[l_t_f] )) ||
					  (box->TestEdge( &pntList[l_t_f], &pntList[r_t_n] )) ||
					  (box->TestEdge( &pntList[r_t_f], &pntList[r_t_n] )) ||
					  (box->TestEdge( &pntList[r_t_n], &pntList[l_t_n] ));

	//  this isn't the most efficient code -- each extruded edge is tested twice
	//  I'm just too lazy to fix this.
	BOOL intersectB = FALSE;
	for ( int i=0; (i<box->nEdges) && !(intersectB||intersectA); i++ )
	{
		DWORD valA = box->silhouetteEdges[i][0];
		DWORD valB = box->silhouetteEdges[i][1];
		D3DXVECTOR3 ptA, ptB;
		ptA.x = (valA & MIN_X) ? box->srcBox.minPt.x : box->srcBox.maxPt.x;
		ptA.y = (valA & MIN_Y) ? box->srcBox.minPt.y : box->srcBox.maxPt.y;
		ptA.z = (valA & MIN_Z) ? box->srcBox.minPt.z : box->srcBox.maxPt.z;
		ptB.x = (valB & MIN_X) ? box->srcBox.minPt.x : box->srcBox.maxPt.x;
		ptB.y = (valB & MIN_Y) ? box->srcBox.minPt.y : box->srcBox.maxPt.y;
		ptB.z = (valB & MIN_Z) ? box->srcBox.minPt.z : box->srcBox.maxPt.z;
		D3DXVECTOR3 ray = ptA - box->projectionPoint;
		D3DXVec3Normalize( &ray, &ray );
		ray = box->projectionPoint + (ray*box->length);   // build an edge that extends projLength distance from the projection point
		intersectB |= TestEdge( &ptA, &ray );             // and test for an intersection
		ray = ptB - box->projectionPoint;
		D3DXVec3Normalize( &ray, &ray );
		ray = box->projectionPoint + (ray*box->length);
		intersectB |= TestEdge( &ptB, &ray );
	}

	return (intersectA || intersectB);
}

//  returns if two frusta intersect
BOOL Frustum::TestFrustum( const Frustum *other ) const
{
    //  TestEdge implicitly checks if vertices are inside frusta, so there is no need to
    //  duplicate the work here, too.
    BOOL intersectA = (TestEdge( &other->pntList[l_b_f], &other->pntList[l_b_n] )) ||
                      (TestEdge( &other->pntList[l_b_n], &other->pntList[r_b_n] )) ||
                      (TestEdge( &other->pntList[r_b_n], &other->pntList[r_b_f] )) ||
                      (TestEdge( &other->pntList[r_b_f], &other->pntList[l_b_f] )) ||
                      (TestEdge( &other->pntList[r_b_f], &other->pntList[r_t_f] )) ||
                      (TestEdge( &other->pntList[r_b_n], &other->pntList[r_t_n] )) ||
                      (TestEdge( &other->pntList[l_b_f], &other->pntList[l_t_f] )) ||
                      (TestEdge( &other->pntList[l_b_n], &other->pntList[l_t_n] )) ||
                      (TestEdge( &other->pntList[l_t_n], &other->pntList[l_t_f] )) ||
                      (TestEdge( &other->pntList[l_t_f], &other->pntList[r_t_f] )) ||
                      (TestEdge( &other->pntList[r_t_f], &other->pntList[r_t_n] )) ||
                      (TestEdge( &other->pntList[r_t_n], &other->pntList[l_t_n] ));

    BOOL intersectB = (other->TestEdge( &pntList[l_b_f], &pntList[l_b_n] )) ||
                      (other->TestEdge( &pntList[l_b_n], &pntList[r_b_n] )) ||
                      (other->TestEdge( &pntList[r_b_n], &pntList[r_b_f] )) ||
                      (other->TestEdge( &pntList[r_b_f], &pntList[l_b_f] )) ||
                      (other->TestEdge( &pntList[r_b_f], &pntList[l_t_f] )) ||
                      (other->TestEdge( &pntList[r_b_n], &pntList[r_t_n] )) ||
                      (other->TestEdge( &pntList[l_b_f], &pntList[l_t_f] )) ||
                      (other->TestEdge( &pntList[l_b_n], &pntList[l_t_n] )) ||
                      (other->TestEdge( &pntList[l_t_n], &pntList[l_t_f] )) ||
                      (other->TestEdge( &pntList[l_t_f], &pntList[r_t_n] )) ||
                      (other->TestEdge( &pntList[r_t_f], &pntList[r_t_n] )) ||
                      (other->TestEdge( &pntList[r_t_n], &pntList[l_t_n] ));

    return intersectA || intersectB;
}


///////////////////////////////////////////////////////////////////

ExtrudedBoundingBox::ExtrudedBoundingBox( const ExtrudedBoundingBox& other )
{
	srcBox.minPt = other.srcBox.minPt;
	srcBox.maxPt = other.srcBox.maxPt;
	length = other.length;
	nEdges = other.nEdges;
	projectionPoint = other.projectionPoint;

	for (int i=0; i<nEdges; i++)
	{
		extrudedPlanes[i] = other.extrudedPlanes[i];
		silhouetteEdges[i][0] = other.silhouetteEdges[i][0];
		silhouetteEdges[i][1] = other.silhouetteEdges[i][1];
	}
}

ExtrudedBoundingBox::ExtrudedBoundingBox( const BoundingBox *box, const D3DXVECTOR3 *pnt, float len )
{
	srcBox.minPt = box->minPt;
	srcBox.maxPt = box->maxPt;
	projectionPoint = *pnt;
	length = len;
	nEdges = 0;

    //  simple comparisons of the bounding box against the emanation point quickly determine which
    //  halfspaces the box exists in.  from this, look up into a precomputed lookup table that
    //  defines which edges of the bounding box are silhouettes.
	unsigned int halfspace = 0;

	if ( srcBox.minPt.x <= projectionPoint.x ) halfspace |= 0x1;
	if ( srcBox.maxPt.x >= projectionPoint.x ) halfspace |= 0x2;
	if ( srcBox.minPt.y <= projectionPoint.y ) halfspace |= 0x4;
	if ( srcBox.maxPt.y >= projectionPoint.y ) halfspace |= 0x8;
	if ( srcBox.minPt.z <= projectionPoint.z ) halfspace |= 0x10;
	if ( srcBox.maxPt.z >= projectionPoint.z ) halfspace |= 0x20;

#include "LookupTables.h"

	unsigned int remap = halfSpaceRemap[halfspace];

	int i=0;

	while ( (silhouettes[remap][i][0]!=INVALID) && (i<6))
	{
		D3DXVECTOR3 ptA, ptB;
		DWORD valA, valB;
		valA = silhouettes[remap][i][0];
		valB = silhouettes[remap][i][1];
		ptA.x = (valA & MIN_X) ? box->minPt.x : box->maxPt.x;
		ptA.y = (valA & MIN_Y) ? box->minPt.y : box->maxPt.y;
		ptA.z = (valA & MIN_Z) ? box->minPt.z : box->maxPt.z;
		ptB.x = (valB & MIN_X) ? box->minPt.x : box->maxPt.x;
		ptB.y = (valB & MIN_Y) ? box->minPt.y : box->maxPt.y;
		ptB.z = (valB & MIN_Z) ? box->minPt.z : box->maxPt.z;
		
		silhouetteEdges[i][0] = valA;
		silhouetteEdges[i][1] = valB;
		D3DXPlaneFromPoints( &extrudedPlanes[i++], pnt, &ptA, &ptB );
	}

	nEdges = i;
}

BOOL ExtrudedBoundingBox::TestPoint( const D3DXVECTOR3 *_pt ) const
{
	for ( int i=0; i<nEdges; i++ )
	{
		if ( D3DXPlaneDotCoord(&extrudedPlanes[i], _pt)<-EPSILON )
			return FALSE;
	}

	return TRUE;
}

BOOL ExtrudedBoundingBox::TestEdge( const D3DXVECTOR3 *a, const D3DXVECTOR3 *b ) const
{
    //  similar to frustum test -- iteratively walk each plane, compute intersection point of edge
    //  and plane.  test if each intersection point is inside (positive side) all silhouette planes --
    //  if so, then return TRUE
	D3DXVECTOR3 ray = *b - *a;

	for ( int i=0; i<nEdges; i++ )
	{
		float t = RayPlaneIntersect( &extrudedPlanes[i], a, &ray );
		if ( (t>0.f) && (t<1.f) && !isSpecial(t) )
		{
			D3DXVECTOR3 intersectPt = *a + (ray*t);
			if ( TestPoint(&intersectPt) )
				return TRUE;
		}
	}

	return FALSE;
}




