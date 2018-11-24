#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "baseclass.h"

class CFrustum : public CBaseClass {
public:

	// Call this every time the camera moves to update the frustum
	void CalculateFrustum(const D3DXMATRIX &mview, const D3DXMATRIX &projection);

	// This takes a 3D point and returns TRUE if it's inside of the frustum
	bool PointInFrustum(const D3DXVECTOR3 &point);

	// This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
	bool SphereInFrustum(const D3DXVECTOR3 &center, const float radius);

	// This takes the center and half the length of the cube.
	bool CubeInFrustum( const D3DXVECTOR3 &center, const float size );

private:

	// This holds the A B C and D values for each side of our frustum.
	float m_Frustum[6][4];

public:
    CFrustum(char *ObjName,CEngine *OwnerEngine);
    virtual ~CFrustum(void);

    MAKE_CLSCREATE(CFrustum);
    MAKE_CLSNAME("CFrustum");
    MAKE_DEFAULTGARBAGE()
};
