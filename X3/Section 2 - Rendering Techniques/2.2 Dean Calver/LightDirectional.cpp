//-----------------------------------------------------------------------------
// File: LightDirectional.cpp
//
// Desc: Implementation of directional light code 
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include "GBuffer.h"
#include "GlobalObjects.h"
#include "GlobalMatrices.h"

#include "BaseLight.h"
#include "LightDirectional.h"

//-----------------------------------------------------------------------------
// Name: LightDirectional Create
// Desc: Loads the FX for processing directional lights
//-----------------------------------------------------------------------------
HRESULT LightDirectional::Create()
{
	HRESULT hr;
	// a pixel shader that does directional lighting on the fat framebuffer
	if( FAILED( hr = D3DXCreateEffectFromFile(	g_Objs.m_pD3DDevice, 
												TEXT("LightDirectional.fx"),
												0, // MACROs
												0, // Include
												0, // flags
												g_Objs.m_LightEffectPool, // Effect Pool
												&m_Shader,
												0 ) ) )
		return DXTRACE_ERR( L"D3DXCreateEffectFromFile LightDirectional.fx", hr );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LightDirectional Update
// Desc: Updates directional lights parameters
//-----------------------------------------------------------------------------
void LightDirectional::Update()
{
	// call our super update
	BaseLight::Update();

	D3DXVECTOR4 vec;
	const D3DXVECTOR3 zAxis(0,0,1);
	// extract the view direction (faces down the z)
	D3DXVec3TransformNormal( (D3DXVECTOR3*)&vec, &zAxis, &m_WorldViewMatrix );
	D3DXVec3Normalize( (D3DXVECTOR3*)&vec, (D3DXVECTOR3*)&vec );
	m_Shader->SetVector( "f3LightDirection", &vec );

	// update colour
	m_Shader->SetVector( "f3LightColour", &m_ActualColour );
}

//-----------------------------------------------------------------------------
// Name: LightDirectional Render
// Desc: Directional lights are full screen lights
//-----------------------------------------------------------------------------
void LightDirectional::Render()
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
