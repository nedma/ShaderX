//-----------------------------------------------------------------------------
// File: DefMesh.h
//
// Desc: Header file for DefMesh (default mesh class)
//-----------------------------------------------------------------------------
#pragma once

class DefMesh
{
public:
	enum BUILTIN_TYPE
	{
		BT_CUBE,
		BT_CYLINDER,
		BT_CONE,
		BT_SPHERE,
		BT_TEAPOT,
		BT_TORUS
	};

	DefMesh() : m_Mesh(0), m_Shader(0), m_CastShadow(true) {};
	virtual ~DefMesh();

	HRESULT CreateBuiltIn( BUILTIN_TYPE type, const TCHAR* FxFile );

	virtual void Update();
	virtual void Reset();
	virtual void Render( unsigned int renderPass );
	virtual void RenderShadow();

	D3DXMATRIX					m_WorldMatrix;		// world position/rotation/scale
	ID3DXEffect*				m_Shader;			// shader that output this mesh into fat framebuffer
    ID3DXMesh*					m_Mesh;				// D3DX mesh to store object
	bool						m_CastShadow;		// does this mesh cast a shadow
};

