//-----------------------------------------------------------------------------
// File: LightHemishere.cpp
//
// Desc: Implementation of hemisphere light code 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include "GBuffer.h"
#include "GlobalObjects.h"
#include "GlobalMatrices.h"

#include "BaseLight.h"
#include "LightHemisphere.h"

//-----------------------------------------------------------------------------
// Name: LightHemisphere Create
// Desc: Loads the FX for processing hemisphere lights
//-----------------------------------------------------------------------------
HRESULT LightHemisphere::Create()
{
	HRESULT hr;
	// a pixel shader that does directional lighting on the fat framebuffer
	if( FAILED( hr = D3DXCreateEffectFromFile(	g_Objs.m_pD3DDevice, 
												TEXT("LightHemisphere.fx"),
												0, // MACROs
												0, // Include
												0, // flags
												g_Objs.m_LightEffectPool, // Effect Pool
												&m_Shader,
												0 ) ) )
		return DXTRACE_ERR( L"D3DXCreateEffectFromFile LightHemisphere.fx", hr );

	m_LightColour = D3DXVECTOR4(0.01f, 0.2f, 0.01f, 0);
	m_SkyColour = D3DXVECTOR4(0.1f, 0.1f, 0.2f, 0);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LightHemisphere Update
// Desc: Updates hemisphere lights parameters
//-----------------------------------------------------------------------------
void LightHemisphere::Update()
{
	// call our super update
	BaseLight::Update();

	D3DXVECTOR4 vec;
	const D3DXVECTOR3 yAxis(0,1,0);
	// extract the world up direction (up the y)
	D3DXVec3TransformNormal( (D3DXVECTOR3*)&vec, &yAxis, &m_WorldViewMatrix );
	D3DXVec3Normalize( (D3DXVECTOR3*)&vec, (D3DXVECTOR3*)&vec );
	m_Shader->SetVector( "f3WorldUpInViewSpace", &vec );

	D3DXVECTOR4 ActualSkyColour = m_SkyColour * m_LightIntensity;

	// ground colour = actual colour, sky colour = SkyColour * Intensity
	m_Shader->SetVector( "f3GroundColour", &m_ActualColour );
	m_Shader->SetVector( "f3SkyColour", &ActualSkyColour );
}


//-----------------------------------------------------------------------------
// Name: LightHemisphere render
// Desc: hemisphere lights are full screen lights
//-----------------------------------------------------------------------------
void LightHemisphere::Render()
{
	unsigned int numPasses;
	// directional lights don't use the mesh object they render to the entire screen
	m_Shader->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	for(unsigned int i=0;i < numPasses;i++)
	{
		m_Shader->BeginPass(i);
		ApplyFullScreenPixelShader( g_Objs.m_Framebuffer->GetWidth(), g_Objs.m_Framebuffer->GetHeight() );
		m_Shader->EndPass();
	}

	m_Shader->End();
}
