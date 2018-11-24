//--------------------------------------------------------------------------------------
// File: terrain.cpp
//
// simple little terrain class as a sample to Topology article in ShaderX3
//
#include "dxstdafx.h"
#include "VertexMapCreator.h"
#include "terrain.h"

static const unsigned int TERRAIN_WIDTH = 128; 
static const unsigned int TERRAIN_HEIGHT = 128;

#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

// evil but this is only a demo...
extern void ApplyFullScreenPixelShader( IDirect3DDevice9* pd3dDevice,	const unsigned int width, 
																		const unsigned int height,
																		const bool bTexelOffset );

Terrain::Terrain( IDirect3DDevice9* pd3dDevice ) :
	m_pd3dDevice( pd3dDevice ),
	m_pGenNormalVertMap(0),
	m_pGenFaceNormTopoMap(0),
	m_pRenderTerrain(0),
	m_pGenPosVertMap(0),
	m_vvTopoMap(0),
	m_faceNormalMap(0),
	m_faceTopoMap(0),
	m_normVertexMap(0),
	m_posVertexMap(0),
	m_indexVSDecl(0),
	m_renderVertexBuffer(0),
	m_renderIndexBuffer(0),
	m_pTexture(0)
{
	HRESULT hr;

	m_numTriangles = (TERRAIN_WIDTH-1) * (TERRAIN_HEIGHT-1) * 2;
	m_numVertices = TERRAIN_WIDTH * TERRAIN_HEIGHT;

	V( VertexMapCreator::CreateCounterVertexStream( m_pd3dDevice,
													m_renderVertexBuffer, m_numVertices ) );

	V( VertexMapCreator::CreateHeightFieldIndexStream( m_pd3dDevice,
														TERRAIN_WIDTH,
														TERRAIN_HEIGHT,
														m_renderIndexBuffer ) );

	D3DVERTEXELEMENT9 indexDecl[] = 
	{
		{ 0, 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END(),
	};

	V( m_pd3dDevice->CreateVertexDeclaration( indexDecl, &m_indexVSDecl ) );

	// create topology maps
	// 1st up we need the vertex to triangle vertex topology map
	V( VertexMapCreator::CreateHeightFieldFaceTopologyMap(	m_pd3dDevice,
										TERRAIN_WIDTH,
										TERRAIN_HEIGHT,
										m_faceTopoMap,
										m_faceMapSizes ) );

	// 2nd up we need the vertex valency (vertex to surround faces)
	V( VertexMapCreator::CreateHeightFieldVertexValencyTopologyMap(	m_pd3dDevice,
										TERRAIN_WIDTH,
										TERRAIN_HEIGHT,
										m_vvTopoMap,
										m_faceMapSizes ) );


	DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif
    
	WCHAR str[MAX_PATH];

	V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"GenPosVertMap.fx" ) );
    V( D3DXCreateEffectFromFile( m_pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &m_pGenPosVertMap, NULL ) );

	V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"RenderTerrain.fx" ) );
    V( D3DXCreateEffectFromFile( m_pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &m_pRenderTerrain, NULL ) );

	V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"GenFaceNormalTopoMap.fx" ) );
    V( D3DXCreateEffectFromFile( m_pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &m_pGenFaceNormTopoMap, NULL ) );

	V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"GenNormalVertMap.fx" ) );
    V( D3DXCreateEffectFromFile( m_pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &m_pGenNormalVertMap, NULL ) );


}

Terrain::~Terrain()
{
	ReleaseResources();
}

void Terrain::SetupRenderTargets()
{
	HRESULT hr;

	WCHAR str[MAX_PATH];

	if( m_pTexture == 0 )
	{
		V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"terrain.bmp" ) );

		// load the terrain height field
		V( D3DXCreateTextureFromFileEx( m_pd3dDevice,
										L"terrain.bmp",
										TERRAIN_WIDTH,
										TERRAIN_HEIGHT,
										1,
										D3DUSAGE_RENDERTARGET,
										D3DFMT_A8R8G8B8,
										D3DPOOL_DEFAULT,
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										0,
										NULL,
										0,
										&m_pTexture ) );
	}

	if( m_posVertexMap == 0 )
	{
		// create dyanamic buffers for vertex maps
		V( VertexMapCreator::CreateRTMap(	m_pd3dDevice,  
											TERRAIN_WIDTH,
											TERRAIN_HEIGHT,
											m_posVertexMap,
											m_vertMapSizes ) );
	}

	if( m_normVertexMap == 0 )
	{
		
		V( VertexMapCreator::CreateRTMap(	m_pd3dDevice,  
											TERRAIN_WIDTH,
											TERRAIN_HEIGHT,
											m_normVertexMap,
											m_vertMapSizes ) );
	}

	if( m_faceNormalMap == 0 )
	{


		// we need a temp storage for face normals
		V( VertexMapCreator::CreateRTMap(	m_pd3dDevice,  
											TERRAIN_WIDTH,
											TERRAIN_HEIGHT,
											m_faceNormalMap,
											m_faceMapSizes ) );
	}

}


void Terrain::ReleaseResources()
{
	SAFE_RELEASE( m_pGenNormalVertMap );
	SAFE_RELEASE( m_pGenFaceNormTopoMap );
	SAFE_RELEASE( m_pRenderTerrain );
	SAFE_RELEASE( m_pGenPosVertMap );
	SAFE_RELEASE( m_vvTopoMap );
	SAFE_RELEASE( m_faceNormalMap );
	SAFE_RELEASE( m_faceTopoMap );
	SAFE_RELEASE( m_normVertexMap );
	SAFE_RELEASE( m_posVertexMap );
	SAFE_RELEASE( m_indexVSDecl );
	SAFE_RELEASE( m_renderVertexBuffer );
	SAFE_RELEASE( m_renderIndexBuffer );
	SAFE_RELEASE( m_pTexture );
}

void Terrain::Render( const D3DXMATRIXA16& inMatrix )
{
	HRESULT hr;

	V( m_pRenderTerrain->SetTexture( "g_positionVertexMap", m_posVertexMap ) );
	V( m_pRenderTerrain->SetTexture( "g_normalVertexMap", m_normVertexMap ) );
	float vertSize[] = { (1.f / m_vertMapSizes[0]), (1.f / m_vertMapSizes[0]) * (1.f / m_vertMapSizes[1] ) };
	V( m_pRenderTerrain->SetFloatArray( "g_vertexMapAddrConsts", vertSize, 2 ) );

	D3DXMATRIXA16 sclMatrix;
	D3DXMatrixScaling( &sclMatrix, -1,1,1 );
	D3DXMATRIXA16 matrix = sclMatrix * inMatrix;
	V( m_pRenderTerrain->SetMatrix( "g_mWorldViewProjection", &matrix ) );

	unsigned int iPasses;
	m_pRenderTerrain->Begin( &iPasses, 0 );

	for( unsigned int i = 0;i < iPasses;i++)
	{
		m_pRenderTerrain->BeginPass( i );
		V( m_pd3dDevice->SetVertexDeclaration( m_indexVSDecl ) );

		// this streams is a indexed stream repeated n times
		V( m_pd3dDevice->SetStreamSource( 0, m_renderVertexBuffer, 0, sizeof(float) ) );
		V( m_pd3dDevice->SetStreamSourceFreq( 0, 1 ) ); 
		V( m_pd3dDevice->SetIndices( m_renderIndexBuffer ) );
		V( m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_numVertices, 0, m_numTriangles ) );

		m_pRenderTerrain->EndPass();
	}

	m_pRenderTerrain->End();
}

void Terrain::RegenerateVertexData()
{
	// calculate the vertex position from the height first
	GenerateVertexPositions();
	GenerateFaceNormals();
	GenerateVertexNormals();
}

void Terrain::GenerateVertexPositions()
{
	HRESULT hr;
	V( m_pGenPosVertMap->SetTexture( "g_heightFieldMap", m_pTexture ) );
	float hfScale[] = { (1.f / TERRAIN_WIDTH), 1.f / 10.f, (1.f / TERRAIN_HEIGHT) };
	V( m_pGenPosVertMap->SetFloatArray( "g_heightfieldScale", hfScale, 3 ) );

	IDirect3DSurface9* surface;
	V( m_posVertexMap->GetSurfaceLevel(0, &surface ) );
	m_pd3dDevice->SetRenderTarget(0, surface );
	surface->Release();

	unsigned int iPasses;
	m_pGenPosVertMap->Begin( &iPasses, 0 );
	for( unsigned int i = 0;i < iPasses;i++)
	{
		m_pGenPosVertMap->BeginPass( i );	
		ApplyFullScreenPixelShader( m_pd3dDevice, (int)m_vertMapSizes[0], (int)m_vertMapSizes[1], true );
		m_pGenPosVertMap->EndPass();
	}

	m_pGenPosVertMap->End();

}

void Terrain::GenerateFaceNormals()
{	
	HRESULT hr;
	V( m_pGenFaceNormTopoMap->SetTexture( "g_positionVertexMap", m_posVertexMap ) );
	float vertSize[] = { (1.f / m_vertMapSizes[0]), (1.f / m_vertMapSizes[0]) * (1.f / m_vertMapSizes[1] ) };
	V( m_pGenFaceNormTopoMap->SetFloatArray( "g_vertexMapAddrConsts", vertSize, 2 ) );

	V( m_pGenFaceNormTopoMap->SetTexture( "g_faceTopoMap", m_faceTopoMap ) );

	IDirect3DSurface9* surface;
	V( m_faceNormalMap->GetSurfaceLevel(0, &surface ) );
	m_pd3dDevice->SetRenderTarget(0, surface );
	surface->Release();

	unsigned int iPasses;
	m_pGenFaceNormTopoMap->Begin( &iPasses, 0 );
	for( unsigned int i = 0;i < iPasses;i++)
	{
		m_pGenFaceNormTopoMap->BeginPass( i );	
		ApplyFullScreenPixelShader( m_pd3dDevice, (int)m_faceMapSizes[0], (int)m_faceMapSizes[1],true );
		m_pGenFaceNormTopoMap->EndPass();
	}

	m_pGenFaceNormTopoMap->End();

}

void Terrain::GenerateVertexNormals()
{	
	HRESULT hr;
	V( m_pGenNormalVertMap->SetTexture( "g_vvTopoMap", m_vvTopoMap ) );
	V( m_pGenNormalVertMap->SetTexture( "g_faceNormalMap", m_faceNormalMap ) );
	float vertSize[] = { (1.f / m_faceMapSizes[0]/2), (1.f / m_faceMapSizes[0]/2) * (1.f / m_faceMapSizes[1] ) };
	V( m_pGenNormalVertMap->SetFloatArray( "g_faceMapAddrConsts", vertSize, 2 ) );

	V( m_pGenNormalVertMap->SetFloat( "g_OneTexel", (1.f / (m_faceMapSizes[0]*2)) ) );

	IDirect3DSurface9* surface;
	V( m_normVertexMap->GetSurfaceLevel(0, &surface ) );
	m_pd3dDevice->SetRenderTarget(0, surface );
	surface->Release();

	unsigned int iPasses;
	m_pGenNormalVertMap->Begin( &iPasses, 0 );
	for( unsigned int i = 0;i < iPasses;i++)
	{
		m_pGenNormalVertMap->BeginPass( i );	
		ApplyFullScreenPixelShader( m_pd3dDevice, (int)m_vertMapSizes[0], (int)m_vertMapSizes[1],false );
		m_pGenNormalVertMap->EndPass();
	}

	m_pGenNormalVertMap->End();

}

void Terrain::Reset()
{
	m_pGenPosVertMap->OnResetDevice();
	m_pRenderTerrain->OnResetDevice();
	m_pGenFaceNormTopoMap->OnResetDevice();
	m_pGenNormalVertMap->OnResetDevice();

	SetupRenderTargets();
}

void Terrain::LostDevice()
{

	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_posVertexMap );
	SAFE_RELEASE( m_normVertexMap );
	SAFE_RELEASE( m_faceNormalMap );

	m_pGenPosVertMap->OnLostDevice();
	m_pRenderTerrain->OnLostDevice();
	m_pGenFaceNormTopoMap->OnLostDevice();
	m_pGenNormalVertMap->OnLostDevice();
}
