//--------------------------------------------------------------------------------------
// File: VertexMapCreator.cpp
//
// Turns simple X meshes into vertex map and topology map mesh
//
#include "dxstdafx.h"
#include "VertexMapCreator.h"

extern bool g_softwareVS30;

HRESULT VertexMapCreator::CreateRTMap(	IDirect3DDevice9* pd3dDevice, 
										ID3DXMesh* pMesh, 
										MAP_FREQ eFreq, 
										IDirect3DTexture9*& RTMap,
										float* vertSize )
{
	HRESULT hr;
	unsigned int numElements;

	if( eFreq == MF_PER_VERTEX )
	{
		numElements = pMesh->GetNumVertices();
	} else if( eFreq == MF_PER_FACE )
	{
		numElements = pMesh->GetNumFaces();
	}
	const unsigned int mapHeight = (numElements / SIZE_OF_MAP) + 1;

	vertSize[0] = (float)SIZE_OF_MAP;
	vertSize[1] = (float)mapHeight;

	V_RETURN( pd3dDevice->CreateTexture( SIZE_OF_MAP, mapHeight, 1, 
								D3DUSAGE_RENDERTARGET, 
								D3DFMT_A32B32G32R32F, 
								g_softwareVS30 ? D3DPOOL_SCRATCH : D3DPOOL_DEFAULT, 
								&RTMap, 0 ) );

	// no data to copy as these are render targets
	return S_OK;
}

HRESULT VertexMapCreator::ConvertXToVertexMap( IDirect3DDevice9* pd3dDevice, 
											  ID3DXMesh* pMesh, 
											  VertexMapCreator::VMC_CONVERTTYPES type,
											  IDirect3DTexture9*& vertexMap,
											  float* vertSize )
{
	HRESULT hr;

	const unsigned int numVertices = pMesh->GetNumVertices();
	const unsigned int mapHeight = (numVertices / SIZE_OF_MAP) + 1;

	vertSize[0] = (float)SIZE_OF_MAP;
	vertSize[1] = (float)mapHeight;
	
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

// extracts data from an X mesh index buffer in a face topology map
HRESULT VertexMapCreator::ConvertXIndexToFaceTopologyMap(	IDirect3DDevice9* pd3dDevice, 
												ID3DXMesh* pMesh, 
												IDirect3DTexture9*& topoMap,
												 float* vertSize )
{
	HRESULT hr;

	const unsigned int numFaces = pMesh->GetNumFaces();
	const unsigned int mapHeight = (numFaces / SIZE_OF_MAP) + 1;

	vertSize[0] = (float)SIZE_OF_MAP;
	vertSize[1] = (float)mapHeight;

	V_RETURN( pd3dDevice->CreateTexture( SIZE_OF_MAP, mapHeight, 1, 
								0, 
								D3DFMT_A32B32G32R32F, 
								g_softwareVS30 ? D3DPOOL_SCRATCH : D3DPOOL_MANAGED, 
								&topoMap, 0 ) );

	D3DLOCKED_RECT lockRect = {0};

	V_RETURN( topoMap->LockRect( 0, &lockRect, 0, 0 ) );

	memset( lockRect.pBits, 0x0, sizeof(float)*4*4*4 );

	void* srcVData;
	V_RETURN( pMesh->LockIndexBuffer( 0, &srcVData ) );

	unsigned int uiFacesLeft = numFaces;
	float* dstTmp = (float*) lockRect.pBits;
	WORD* srcData = (WORD*) srcVData;

	const int FACES_IN_GROUP = 30;
	unsigned int group = 0;
	unsigned int groupCount = FACES_IN_GROUP;

	for( unsigned int y = 0; y < mapHeight; ++y )
	{	
		float* dstData = dstTmp + y * (lockRect.Pitch / sizeof(float));
		unsigned int mapWidth = min( uiFacesLeft, SIZE_OF_MAP );

		for( unsigned int x = 0; x < mapWidth; ++x )
		{
			dstData[0] = srcData[0];
			dstData[1] = srcData[1];
			dstData[2] = srcData[2];

			// pack in a group id, for this demo I change group every FACES_IN_GROUP polygons
			dstData[3] = (float)group;

			srcData += 3;
			dstData += 4;
			--uiFacesLeft;

			// simple logic to change group every n faces
			--groupCount;
			if( groupCount == 0 )
			{
				groupCount = FACES_IN_GROUP;
				++group;
				// max group to not take up to much constant space
				if( group >= 20 )
				{
					group = 0;
				}
			}
		}
	}

	V_RETURN( pMesh->UnlockIndexBuffer() );
	V_RETURN( topoMap->UnlockRect( 0 ) );

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

// this is needed as vertex frequency on Dx9.0c requires an indexed stream, this just produces the NULL
// index stream
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
HRESULT VertexMapCreator::CreateCounterVertexStream(	IDirect3DDevice9* pd3dDevice, 
														IDirect3DVertexBuffer9*& vertexStream,
														unsigned int count )
{
	HRESULT hr;

	// TODO should use smallest type < count (i.e. BYTE or SHORT if count < 256 or 65536 respectively)
	V_RETURN( pd3dDevice->CreateVertexBuffer( count  * sizeof(float),
									(g_softwareVS30 ? D3DUSAGE_SOFTWAREPROCESSING : 0) | D3DUSAGE_WRITEONLY,
									0,
									D3DPOOL_MANAGED,
									&vertexStream,
									0 ) );
	void* dstVData;
	V_RETURN( vertexStream->Lock( 0, 0, &dstVData, 0 ) );

	float* dstData = (float*) dstVData;
	for(unsigned int i=0; i < count;i++)
	{
		dstData[i] = (float)i;
	}

	V_RETURN( vertexStream->Unlock() );
	return S_OK;
}

