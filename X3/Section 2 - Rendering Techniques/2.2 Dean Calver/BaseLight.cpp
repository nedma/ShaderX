//-----------------------------------------------------------------------------
// File: BaseLight.cpp
//
// Desc: Implementation of Base lights code shared by sub-classes
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include "GBuffer.h"
#include "GlobalObjects.h"
#include "GlobalMatrices.h"

#include "BaseLight.h"
#include "LightDirectional.h"

//-----------------------------------------------------------------------------
// Name: BaseLight ctor
//-----------------------------------------------------------------------------
BaseLight::BaseLight() : 
	m_Shader(0), 
	m_Mesh(0),
	m_LightColour(1,1,1,0),
	m_LightIntensity(1)
{
	D3DXMatrixIdentity( &m_WorldMatrix );
};

//-----------------------------------------------------------------------------
// Name: BaseLight dtor
//-----------------------------------------------------------------------------
BaseLight::~BaseLight()
{
	SAFE_RELEASE( m_Shader );
	SAFE_RELEASE( m_Mesh );
}

//-----------------------------------------------------------------------------
// Name: BaseLight reset
// Desc: Called when the device has changed in some way
//-----------------------------------------------------------------------------
void BaseLight::Reset()
{
	if(m_Shader)
		m_Shader->OnLostDevice();
}

//-----------------------------------------------------------------------------
// Name: BaseLight Update
// Desc: Make sure matrices etc are upto date
//-----------------------------------------------------------------------------
void BaseLight::Update()
{
	// calculate our local matrix in view space
	const D3DXMATRIXA16& matView = g_Matrices.m_View;
	D3DXMatrixMultiply( &m_WorldViewMatrix, &m_WorldMatrix, &matView );

	// calculate actual colour
	m_ActualColour = m_LightColour * m_LightIntensity;
}

//-----------------------------------------------------------------------------
// Name: BaseLight ApplyFullScreenPixelShader
// Desc: Render a full screen quad with whatever settings are currently set
//-----------------------------------------------------------------------------
void BaseLight::ApplyFullScreenPixelShader( const unsigned int width, const unsigned int height )
{
	// render pixel shader quad to combine the render target
	struct
	{
		float x,y,z,w;
		float u0,v0;
	} quad[4] = 
	{ 
		{ (float)0,		(float)0,			0.1f,1,	0,0, }, 
		{ (float)width, (float)0,			0.1f,1,	1,0, }, 
		{ (float)0,		(float)height,		0.1f,1,	0,1, }, 
		{ (float)width, (float)height,		0.1f,1,	1,1, } 
	};

	g_Objs.m_pD3DDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	g_Objs.m_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])  );
}
