//-----------------------------------------------------------------------------
// File: DefMesh.cpp
//
// Desc: Mesh handler class, simplifies the sample that render lots of 
//       different meshes
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include "GBuffer.h"
#include "GlobalObjects.h"
#include "GlobalMatrices.h"
#include "DefMesh.h"

//-----------------------------------------------------------------------------
// Name: DefMesh CreateBuiltIn
// Desc: Creates the mesh from D3DX Shape lists, using the specified FX file
//       for rendering
//-----------------------------------------------------------------------------

HRESULT DefMesh::CreateBuiltIn( BUILTIN_TYPE type, const TCHAR* FxFile )
{
	HRESULT hr;

	switch( type )
	{
	case BT_CUBE:
		// Create a box mesh using D3DX
		if( FAILED( hr = D3DXCreateBox( g_Objs.m_pD3DDevice, 1.0f, 1.0f, 1.0f, &m_Mesh, NULL ) ) )
			return DXTRACE_ERR( L"D3DXCreateBox", hr );
		break;
	case BT_CYLINDER:
		// Create a cylinder mesh using D3DX
		if( FAILED( hr = D3DXCreateCylinder( g_Objs.m_pD3DDevice, 0.5f, 0.5f, 1.0f, 10, 10, &m_Mesh, NULL ) ) )
			return DXTRACE_ERR( L"D3DXCreateCylinder", hr );
		break;
	case BT_CONE:
		// Create a cone mesh using D3DX
		if( FAILED( hr = D3DXCreateCylinder( g_Objs.m_pD3DDevice, 0.5f, 0.0f, 1.0f, 20, 10, &m_Mesh, NULL ) ) )
			return DXTRACE_ERR( L"D3DXCreateCylinder", hr );
		break;
	case BT_SPHERE:
		// Create a sphere mesh using D3DX
		if( FAILED( hr = D3DXCreateSphere( g_Objs.m_pD3DDevice, 0.5f, 20, 10, &m_Mesh, NULL ) ) )
			return DXTRACE_ERR( L"D3DXCreateTorus", hr );
		break;
	case BT_TEAPOT:
		// Create a teapot mesh using D3DX
		if( FAILED( hr = D3DXCreateTeapot( g_Objs.m_pD3DDevice, &m_Mesh, NULL ) ) )
			return DXTRACE_ERR( L"D3DXCreateTeapot", hr );
		break;
	case BT_TORUS:
		// Create a torus mesh using D3DX
		if( FAILED( hr = D3DXCreateTorus( g_Objs.m_pD3DDevice, 0.25f, 0.5f, 20, 20, &m_Mesh, NULL ) ) )
			return DXTRACE_ERR( L"D3DXCreateTorus", hr );
		break;
	}

	// Create an effect (outputs the mesh into the fat buffer)
	if( FAILED( hr = D3DXCreateEffectFromFile(	g_Objs.m_pD3DDevice, 
												FxFile,
												0, // MACROs
												0, // Include
												0, // flags
												g_Objs.m_EffectPool, // Effect Pool
												&m_Shader,
												0 ) ) )
        return DXTRACE_ERR( L"D3DXCreateEffectFromFile", hr );

	D3DXMatrixIdentity( &m_WorldMatrix );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DefMesh ctor
//-----------------------------------------------------------------------------
DefMesh::~DefMesh()
{
	SAFE_RELEASE( m_Shader );
	SAFE_RELEASE( m_Mesh );
}

//-----------------------------------------------------------------------------
// Name: DefMesh Update
//-----------------------------------------------------------------------------
void DefMesh::Update()
{
}

//-----------------------------------------------------------------------------
// Name: DefMesh reset
// Desc: Called when the device has changed in some way
//-----------------------------------------------------------------------------
void DefMesh::Reset()
{
	if(m_Shader)
		m_Shader->OnLostDevice();
}

//-----------------------------------------------------------------------------
// Name: BaseLight Render
// Desc: Render the mesh handle MRT or single render target
//-----------------------------------------------------------------------------
void DefMesh::Render( unsigned int renderPass )
{
	unsigned int passCount;
   
	const D3DXMATRIXA16& matView = g_Matrices.m_View;
	const D3DXMATRIXA16& matViewProjection = g_Matrices.m_ViewProj;

	D3DXMATRIXA16 matWorldView, matWorldViewProjection;

	D3DXMatrixMultiply( &matWorldView, &m_WorldMatrix, &matView );
	D3DXMatrixMultiply( &matWorldViewProjection, &m_WorldMatrix, &matViewProjection );

	D3DXMATRIXA16 matTransInvWorldView;
	D3DXMatrixInverse( &matTransInvWorldView, 0, &matWorldView );
	D3DXMatrixTranspose( &matTransInvWorldView, &matTransInvWorldView);

	m_Shader->SetMatrix( "matWorld", &m_WorldMatrix );
	m_Shader->SetMatrix( "matWorldView", &matWorldView );
	m_Shader->SetMatrix( "matWorldViewProjection", &matWorldViewProjection );
	m_Shader->SetMatrix( "matTranInvWorldView", &matTransInvWorldView);

	if( g_Objs.m_Framebuffer->GetType() == MT_MRT )
		m_Shader->SetTechnique( "T0" );
	else
	{
		if( renderPass == 0 )
			m_Shader->SetTechnique( "T2" );
		else if( renderPass == 1 )
			m_Shader->SetTechnique( "T3" );
		else if( renderPass == 2 )
			m_Shader->SetTechnique( "T4" );
		else if( renderPass == 3 )
			m_Shader->SetTechnique( "T5" );
	}

    // Render the mesh
	m_Shader->Begin( &passCount, D3DXFX_DONOTSAVESTATE );
	for(unsigned int i=0;i < passCount;i++)
	{
		m_Shader->BeginPass(i);
		m_Mesh->DrawSubset(0);
		m_Shader->EndPass();
	}
	m_Shader->End();
}

//-----------------------------------------------------------------------------
// Name: BaseLight RenderShadow
// Desc: Render the mesh handle into a shadow map
//-----------------------------------------------------------------------------
void DefMesh::RenderShadow()
{
	unsigned int passCount;
   
	const D3DXMATRIXA16& matView = g_Matrices.m_ShadowView;
	const D3DXMATRIXA16& matViewProjection = g_Matrices.m_ShadowViewProj;
	D3DXMATRIXA16 matWorldView, matWorldViewProjection;

	D3DXMatrixMultiply( &matWorldView, &m_WorldMatrix, &matView );
	D3DXMatrixMultiply( &matWorldViewProjection, &m_WorldMatrix, &matViewProjection );

	m_Shader->SetMatrix( "matWorldView", &matWorldView );
	m_Shader->SetMatrix( "matWorldViewProjection", &matWorldViewProjection );

	m_Shader->SetTechnique( "T1" );

    // Render the mesh
	m_Shader->Begin( &passCount, D3DXFX_DONOTSAVESTATE );
	for(unsigned int i=0;i < passCount;i++)
	{
		m_Shader->BeginPass(i);
		m_Mesh->DrawSubset(0);
		m_Shader->EndPass();
	}
	m_Shader->End();
}
