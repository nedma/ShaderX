//-----------------------------------------------------------------------------
// Path:  SDK\DEMOS\Direct3D9\src\PracticalCubeShadowMaps
// File:  BoundingUtil.h
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


#ifndef __BOUNDING_UTIL_H_included_
#define __BOUNDING_UTIL_H_included_

//////////////////////////////////////////////////////////////////////////
//    Utility functions called by functions in Bounding.cpp
//
//////////////////////////////////////////////////////////////////////////

const float EPSILON = 0.00000135f;

//  these are just names for bit masked fields that are used throughout the various functions, to make the code
//  a bit more readable. 
//  this list is a 3-bit field which defines 1 of 8 points in an axis-aligned bounding box
enum {
	MIN_X = 1,
	MAX_X = 0,
	MIN_Y = 2,
	MAX_Y = 0,
	MIN_Z = 4,
	MAX_Z = 0,
	INVALID = -1 
};
//  and this list is names for each of the 8 points bounding a view frustum (intersection of 3 planes, r=right, l=left, etc.)
enum
{
    r_b_f = 0,
    r_b_n = 1,
    r_t_f = 2,
    r_t_n = 3,
    l_b_f = 4,
    l_b_n = 5,
    l_t_f = 6,
    l_t_n = 7
};


//////////////////////////////////////////////////////////////////////////
//  isSpecial
//     returns if a floating point number is an IEEE special (NaN/Inf)
inline bool isSpecial( const float& f )
{
    DWORD bits = *(const DWORD*)(&f);
    return ((bits&0x7f800000)==0x7f800000);
}

//////////////////////////////////////////////////////////////////////////
//  RayPlaneIntersect
//    returns the parametric intersection distance (t) from a point to a plane, along a specified direction
//    returns Inf if the ray is parallel to the plane
static float RayPlaneIntersect( const D3DXPLANE* _p, const D3DXVECTOR3* org, const D3DXVECTOR3* dir )
{
    float distance = D3DXPlaneDotCoord(_p, org);
    float len      = D3DXPlaneDotNormal(_p, dir);

    float t = -distance / len;
    return t;
}

///////////////////////////////////////////////////////////////////////////
//  PlaneIntersection
//    computes the point where three planes intersect
//    returns whether or not the point exists.
static inline BOOL PlaneIntersection( D3DXVECTOR3* intersectPt, const D3DXPLANE* p0, const D3DXPLANE* p1, const D3DXPLANE* p2 )
{
    D3DXVECTOR3 n0( p0->a, p0->b, p0->c );
    D3DXVECTOR3 n1( p1->a, p1->b, p1->c );
    D3DXVECTOR3 n2( p2->a, p2->b, p2->c );

    D3DXVECTOR3 n1_n2, n2_n0, n0_n1;  
    
    D3DXVec3Cross( &n1_n2, &n1, &n2 );
    D3DXVec3Cross( &n2_n0, &n2, &n0 );
    D3DXVec3Cross( &n0_n1, &n0, &n1 );

    float denominator = 1.f / D3DXVec3Dot( &n0, &n1_n2 );

    n1_n2 = n1_n2 * p0->d;
    n2_n0 = n2_n0 * p1->d;
    n0_n1 = n0_n1 * p2->d;

    *intersectPt = -(n1_n2 + n2_n0 + n0_n1) * denominator;

    if ( isSpecial(intersectPt->x) || isSpecial(intersectPt->y) || isSpecial(intersectPt->z) )
        return FALSE;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//  misc. utility functions
//  these are declared in Bounding.h, so they are exported to the rest of the program
//  placed here, rather than Bounding.cpp, to keep the files shorter
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  GetSceneBoundingBox
//    computes the union of all bounding boxes in the scene, and computes the bounding
//    box for all individual models in the scene
void GetSceneBoundingBox( BoundingBox* box, std::vector<BoundingBox>* modelBoxen, const NVBScene* scene )
{
    BoundingBox tmp;
    box->minPt = D3DXVECTOR3( 3.3e33f,  3.3e33f,  3.3e33f );
    box->maxPt = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f );

    for (unsigned int i=0; i<scene->m_NumMeshes; i++)
    {
        GetModelBoundingBox(&tmp, &scene->m_Meshes[i]);
        modelBoxen->push_back(tmp);
        box->Merge(&tmp.minPt);
        box->Merge(&tmp.maxPt);
    }
}

///////////////////////////////////////////////////////////////////////////////
//  GetModelBoundingBox
//    computes the bounding box for a specific mesh inside a NVBScene
void GetModelBoundingBox( BoundingBox* box, const NVBScene::Mesh* mesh )
{
    BoundingBox tmp;
    tmp.minPt = D3DXVECTOR3( 3.3e33f,  3.3e33f,  3.3e33f);
    tmp.maxPt = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f);

    for (unsigned int i=0; i<mesh->m_NumVertices; i++)
        tmp.Merge(&mesh->m_Vertices[i].m_Position);

    XFormBoundingBox( box, &tmp, &mesh->m_Transform );
}

/////////////////////////////////////////////////////////////////////////////////
//  XFormBoundingBox
//  Transform an axis-aligned bounding box by the specified matrix, and compute a new axis-aligned bounding box
void XFormBoundingBox( BoundingBox* result, const BoundingBox* src, const D3DXMATRIX* matrix )
{
    D3DXVECTOR3  pts[8];
    pts[0] = D3DXVECTOR3( src->minPt.x, src->minPt.y, src->minPt.z );
    pts[1] = D3DXVECTOR3( src->minPt.x, src->minPt.y, src->maxPt.z );
    pts[2] = D3DXVECTOR3( src->minPt.x, src->maxPt.y, src->minPt.z );
    pts[3] = D3DXVECTOR3( src->minPt.x, src->maxPt.y, src->maxPt.z );
    pts[4] = D3DXVECTOR3( src->maxPt.x, src->minPt.y, src->minPt.z );
    pts[5] = D3DXVECTOR3( src->maxPt.x, src->minPt.y, src->maxPt.z );
    pts[6] = D3DXVECTOR3( src->maxPt.x, src->maxPt.y, src->minPt.z );
    pts[7] = D3DXVECTOR3( src->maxPt.x, src->maxPt.y, src->maxPt.z );

    result->minPt = D3DXVECTOR3(3.3e33f, 3.3e33f, 3.3e33f);
    result->maxPt = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f);

    for (int i=0; i<8; i++)
    {
        D3DXVECTOR3 tmp;
        D3DXVec3TransformCoord(&tmp, &pts[i], matrix);
        result->minPt.x = min(result->minPt.x, tmp.x);
        result->minPt.y = min(result->minPt.y, tmp.y);
        result->minPt.z = min(result->minPt.z, tmp.z);
        result->maxPt.x = max(result->maxPt.x, tmp.x);
        result->maxPt.y = max(result->maxPt.y, tmp.y);
        result->maxPt.z = max(result->maxPt.z, tmp.z);
    }
}

#endif