//--------------------------------------------------------------------------------------
// File: VertexMapCreator.cpp
//
// Turns simple X meshes into vertex map and topology map mesh
//
#include "dxstdafx.h"
#include "VertexMapCreator.h"

extern bool g_softwareVS30;
int VertexMapCreator::HeightOfMap = 0;

HRESULT VertexMapCreator::ConvertXToVertexMap( IDirect3DDevice9* pd3dDevice, 
											  ID3DXMesh* pMesh, 
											  VertexMapCreator::VMC_CONVERTTYPES type,
											  IDirect3DTexture9*& vertexMap )
{
	HRESULT hr;
	unsigned int numVertices = pMesh->GetNumVertices();

	unsigned int mapHeight = (numVertices / SIZE_OF_MAP) + 1;

	HeightOfMap = mapHeight;

	V_RETURN( pd3dDevice->CreateTexture( SIZE_OF_MAP, mapHeight, 1, 
								0, 
								D3DFMT_A32B32G32R32F, 
								g_softwareVS30 ? D3DPOOL_SCRATCH : D3DPOOL_MANAGED, 
								&vertexMap, 0 ) );

	D3DLOCKED_RECT lockRect = {0};

	V_RETURN( vertexMap->LockRect( 0, &lockRect, 0, 0 ) );

	void* pVertexData;
	V_RETURN( pMesh->LockVertexBuffer( 0, &pVertexData ) );

	unsigned int uiOffset = 0; // in floats
	unsigned int uiSize = 0; // in bytes

	switch( type )
	{
	case VC_POSITION:
		uiOffset = 0;
		uiSize = sizeof(float) * 3;
		break;
	case VC_NORMAL:
		uiOffset = 3;
		uiSize = sizeof(float) * 3;
		break;
	case VC_UV:
		uiOffset = 6;
		uiSize = sizeof(float) * 2;
		break;
	}

	const float* srcData = (float*) pVertexData;
	float* dstData = (float*) lockRect.pBits;
	float* dstTmp = dstData;

	int uiVerticesleft = numVertices;

	for( unsigned int y = 0; y < mapHeight; ++y )
	{	

		dstData = dstTmp + y * (lockRect.Pitch / sizeof(float));
		unsigned int mapWidth = min( uiVerticesleft, SIZE_OF_MAP );

		for( unsigned int x = 0; x < mapWidth; ++x )
		{
			memcpy( dstData, srcData + uiOffset, uiSize );
			dstData += 4;
			srcData += pMesh->GetNumBytesPerVertex() / sizeof(float);
			--uiVerticesleft;
		}
	}

	V_RETURN( pMesh->UnlockVertexBuffer() );

	V_RETURN( vertexMap->UnlockRect( 0 ) );

	return S_OK;
}

// extracts data from an X mesh index buffer in a vertex stream
HRESULT VertexMapCreator::ConvertXIndexToVertexStream(	IDirect3DDevice9* pd3dDevice, 
												ID3DXMesh* pMesh, 
												IDirect3DVertexBuffer9*& vertexStream )
{
	HRESULT hr;

	const unsigned int numFaces = pMesh->GetNumFaces();

	V_RETURN( pd3dDevice->CreateVertexBuffer( numFaces * sizeof(WORD) * 4,
									(g_softwareVS30 ? D3DUSAGE_SOFTWAREPROCESSING : 0) | D3DUSAGE_WRITEONLY,
									0,
									D3DPOOL_MANAGED,
									&vertexStream,
									0 ) );
	void* dstVData;
	V_RETURN( vertexStream->Lock( 0, 0, &dstVData, 0 ) );

	void* srcVData;
	V_RETURN( pMesh->LockIndexBuffer( 0, &srcVData ) );

	WORD* dstData = (WORD*) dstVData;
	WORD* srcData = (WORD*) srcVData;
	for( unsigned int i = 0;i < numFaces;++i )
	{
		dstData[0] = srcData[0];
		dstData[1] = srcData[1];
		dstData[2] = srcData[2];
		dstData[3] = 0; // PADD word not used

		srcData += 3;
		dstData += 4;
	}

	V_RETURN( pMesh->UnlockIndexBuffer() );
	V_RETURN( vertexStream->Unlock() );

	return S_OK;
}

HRESULT VertexMapCreator::Create123VertexStream(	IDirect3DDevice9* pd3dDevice, 
													IDirect3DVertexBuffer9*& vertexStream )
{
	HRESULT hr;

	V_RETURN( pd3dDevice->CreateVertexBuffer( 3  * sizeof(float),
									(g_softwareVS30 ? D3DUSAGE_SOFTWAREPROCESSING : 0) | D3DUSAGE_WRITEONLY,
									0,
									D3DPOOL_MANAGED,
									&vertexStream,
									0 ) );
	void* dstVData;
	V_RETURN( vertexStream->Lock( 0, 0, &dstVData, 0 ) );

	float* dstData = (float*) dstVData;
	dstData[0] = 0.f;
	dstData[1] = 1.f;
	dstData[2] = 2.f;


	V_RETURN( vertexStream->Unlock() );

	return S_OK;
}

HRESULT VertexMapCreator::Create123IndexStream(	IDirect3DDevice9* pd3dDevice, 
												IDirect3DIndexBuffer9*& indexStream )
{
	HRESULT hr;

	V_RETURN( pd3dDevice->CreateIndexBuffer( 3  * sizeof(WORD),
									(g_softwareVS30 ? D3DUSAGE_SOFTWAREPROCESSING : 0) | D3DUSAGE_WRITEONLY,
									D3DFMT_INDEX16,
									D3DPOOL_MANAGED,
									&indexStream,
									0 ) );
	void* dstVData;
	V_RETURN( indexStream->Lock( 0, 0, &dstVData, 0 ) );

	WORD* dstData = (WORD*) dstVData;
	dstData[0] = 0;
	dstData[1] = 1;
	dstData[2] = 2;


	V_RETURN( indexStream->Unlock() );

	return S_OK;
}