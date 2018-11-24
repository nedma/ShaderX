//-----------------------------------------------------------------------------
// File: GlobalObjects.h
//
// Desc: Header file for objects that are shared all over the place
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: struct CULLINFO
// Desc: Stores information that will be used when culling objects.  It needs
//       to be recomputed whenever the view matrix or projection matrix changes.
//-----------------------------------------------------------------------------
struct CULLINFO
{
    D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
    D3DXPLANE planeFrustum[6];    // planes of the view frustum
};
//-----------------------------------------------------------------------------
// Name: enum CULLSTATE
// Desc: Represents the result of the culling calculation on an object.
//-----------------------------------------------------------------------------
enum CULLSTATE
{
    CS_UNKNOWN,      // cull state not yet computed
    CS_INSIDE,       // object bounding box is at inside the frustum
    CS_OUTSIDE,      // object bounding box is outside the frustum
	CS_CROSSING,	 // is possible in and out
};

enum CULLPLANES
{
	CP_NEAR		= 0,
	CP_FAR		= 1,
	CP_LEFT		= 2,
	CP_RIGHT	= 3,
	CP_TOP		= 4,
	CP_BOTTOM	= 5,
};

class GlobalObjects
{
public:
	ID3DXEffectPool*	m_EffectPool;		// Pool that all mesh shader should use
	ID3DXEffectPool*	m_LightEffectPool;	// Pool that all lights should use

	IDirect3D9*			m_pD3D;				// the D3D object
	IDirect3DDevice9*	m_pD3DDevice;		// the D3D device object
	class GBuffer*		m_Framebuffer;		// the frame object

	CULLINFO			m_CullInfo;			// the frustum culling info

	GlobalObjects() : 
		m_EffectPool(0),
		m_LightEffectPool(0),
		m_pD3D(0),
		m_pD3DDevice(0),
		m_Framebuffer(0)
	{}


};

extern GlobalObjects g_Objs;