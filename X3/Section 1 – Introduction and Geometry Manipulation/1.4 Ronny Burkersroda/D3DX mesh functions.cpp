//
// Direct3D Extension Mesh functions (for ShaderX³)
//
//	written 2004 by Ronny Burkersroda
//

// header includes
#include					"D3DX mesh functions.h"						// D3DX mesh functions


// *****************************
// *** structure declaration ***
// *****************************

struct						D3DXVERTEXELEMENT							// vertex element extended by some methods
	: D3DVERTEXELEMENT9
{
	D3DXVERTEXELEMENT(													// default constructor
		BOOL						bEnd						= TRUE		// end element flag
		)
	{
		// set constructing values
		if( bEnd )
		{
			Stream		= 0xFF;
			Offset		= 0;
			Type		= D3DDECLTYPE_UNUSED;
			Method		= 0;
			Usage		= 0;
			UsageIndex	= 0;
		}
		else
			ZeroMemory( this, sizeof( this ) );
	}

	D3DXVERTEXELEMENT(													// constructor with value definition
		BYTE						nStream,								// stream number
		BYTE						nOffset,								// offset position
		BYTE						nType,									// data type
		BYTE						nMethod,								// tessellator method
		BYTE						nUsage,									// data usage
		BYTE						nUsageIndex								// index of data usage
		)
	{
		// set constructing values
		Stream		= nStream;
		Offset		= nOffset;
		Type		= nType;
		Method		= nMethod;
		Usage		= nUsage;
		UsageIndex	= nUsageIndex;
	}

	D3DXVERTEXELEMENT(													// constructor with value definition
		BYTE						pData[ 6 ]								// pointer to values
		)
	{
		// set constructing values
		Stream		= pData[ 0 ];
		Offset		= pData[ 1 ];
		Type		= pData[ 2 ];
		Method		= pData[ 3 ];
		Usage		= pData[ 4 ];
		UsageIndex	= pData[ 5 ];
	}

	static BOOL					CheckEnd(								// check if given element is defining the end
		const D3DVERTEXELEMENT9*	pvtxeElement							// pointer to the element to check
		)
	{
		// return, if element is the end
		return ( pvtxeElement->Stream == 0xFF ) && !pvtxeElement->Offset && ( pvtxeElement->Type == D3DDECLTYPE_UNUSED ) && !pvtxeElement->Method && !pvtxeElement->Usage && !pvtxeElement->UsageIndex;
	}
};


// ************************
// *** global functions ***
// ************************

HRESULT D3DXMeshFunctions::BuildMorphingMesh( ID3DXMesh* pmeshSource, ID3DXMesh* pmeshTarget, const D3DVECTOR* pvctCenter, ID3DXMesh** ppmeshMorphing )
{
	D3DVERTEXELEMENT9	pvtxeDeclaration[ MAX_FVF_DECL_SIZE ];
	D3DXVECTOR3			vct3Direction;
	D3DXVECTOR3			vct3Center;
	PDIRECT3DDEVICE9	pd3dd;
	ID3DXMesh*			pmshSource							= NULL;
	ID3DXMesh*			pmeshDestination;
	PBYTE				pVertex;
	HRESULT				hResult;
	DWORD				nType;
	DWORD				nCounter;
	DWORD				nPosition;
	DWORD				nSourcePositionOffset;
	DWORD				nTargetPositionOffset;
	DWORD				nVertexSize							= 0;
	FLOAT				fRadius;
	FLOAT				fDistance;

	// check parameters
	if( !pmeshSource || !pmeshTarget || !ppmeshMorphing )
		return E_INVALIDARG;

	// get declarations of the source mesh and melt them
	if( ( hResult = pmeshSource->GetDeclaration( pvtxeDeclaration ) ) == S_OK )
	{
		// get highest index for position usage
		nType		= 0;
		nPosition	= 0xFFFFFFFF;
		for( nCounter = 0; !D3DXVERTEXELEMENT::CheckEnd( &pvtxeDeclaration[ nCounter ] ); ++nCounter ) if( ( pvtxeDeclaration[ nCounter ].Usage == D3DDECLUSAGE_POSITION ) && ( pvtxeDeclaration[ nCounter ].UsageIndex >= nType ) )
			nType = pvtxeDeclaration[ nPosition = nCounter ].UsageIndex + 1;

		// check if one position can be added
		if( ( nType < 15 ) && ( ++nCounter < MAX_FVF_DECL_SIZE ) )
		{
			// set last position if no texture coordinate set is found
			if( nPosition == 0xFFFFFFFF )
				nPosition = nCounter;
			else
				++nPosition;

			// enhance declaration for morphing data
			memmove( &pvtxeDeclaration[ nPosition + 1 ], &pvtxeDeclaration[ nPosition ], sizeof( D3DVERTEXELEMENT9 ) * ( nCounter - nPosition ) );
			pvtxeDeclaration[ nPosition ] = D3DXVERTEXELEMENT( 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, (BYTE) nType );

			// compute vertex size and data position in vertex
			for( nCounter = 0; !D3DXVERTEXELEMENT::CheckEnd( &pvtxeDeclaration[ nCounter ] ); ++nCounter ) if( pvtxeDeclaration[ nCounter ].Type != D3DDECLTYPE_UNUSED )
			{
				// copy the position of the position vector
				if( ( pvtxeDeclaration[ nCounter ].Usage == D3DDECLUSAGE_POSITION ) && ( pvtxeDeclaration[ nCounter ].UsageIndex == 0 ) )
					nSourcePositionOffset = nVertexSize;

				// copy data position
				if( nCounter == nPosition )
					nTargetPositionOffset = nVertexSize;

				// set correct offset
				pvtxeDeclaration[ nCounter ].Offset = (WORD) nVertexSize;

				// enlarge vertex size
				switch( pvtxeDeclaration[ nCounter ].Type )
				{
				case D3DDECLTYPE_FLOAT1:
				case D3DDECLTYPE_D3DCOLOR:
				case D3DDECLTYPE_UBYTE4:
				case D3DDECLTYPE_UBYTE4N:
				case D3DDECLTYPE_SHORT2:
				case D3DDECLTYPE_SHORT2N:
				case D3DDECLTYPE_USHORT2N:
				case D3DDECLTYPE_UDEC3:
				case D3DDECLTYPE_DEC3N:
				case D3DDECLTYPE_FLOAT16_2:

					nVertexSize += 4;

					break;

				case D3DDECLTYPE_FLOAT2:
				case D3DDECLTYPE_SHORT4:
				case D3DDECLTYPE_SHORT4N:
				case D3DDECLTYPE_USHORT4N:
				case D3DDECLTYPE_FLOAT16_4:

					nVertexSize += 8;

					break;

				case D3DDECLTYPE_FLOAT3:

					nVertexSize += 12;

					break;

				case D3DDECLTYPE_FLOAT4:

					nVertexSize += 16;

					break;
				}
			}
		}
		else
			return E_INVALIDARG;

		// get Direct3D device
		if( SUCCEEDED( hResult = pmeshSource->GetDevice( &pd3dd ) ) )
		{
			// clone source mesh with built declaration
			if( SUCCEEDED( hResult = pmeshSource->CloneMesh( pmeshSource->GetOptions(), pvtxeDeclaration, pd3dd, &pmeshDestination ) ) )
			{
				// lock vertex buffer
				if( SUCCEEDED( hResult = pmeshDestination->LockVertexBuffer( 0, (PVOID*) &pVertex ) ) )
				{
					// compute values of the bounding sphere
					D3DXComputeBoundingSphere( (D3DXVECTOR3*) pVertex, pmeshDestination->GetNumVertices(), pmeshDestination->GetNumBytesPerVertex(), &vct3Center, &fRadius );

					// check for application-defined projection center
					if( pvctCenter )
					{
						// increase radius
						fRadius += D3DXVec3Length( &D3DXVECTOR3( vct3Center - *(D3DXVECTOR3*) pvctCenter ) );

						// set new center
						vct3Center = *(D3DXVECTOR3*) pvctCenter;
					}

					// double radius
					fRadius *= 2.0f;

					// compute projected vectors
					for( nCounter = pmeshDestination->GetNumVertices(); nCounter--; )
					{
						// calculate direction from center to vertex position
						D3DXVec3Normalize( &vct3Direction, &D3DXVECTOR3( vct3Center - *(D3DXVECTOR3*)( pVertex + nSourcePositionOffset ) ) );

						// calculate intersection with destination mesh from outstanding point in direction to the center
						D3DXIntersect( pmeshTarget, &D3DXVECTOR3( vct3Center - vct3Direction * fRadius ), &vct3Direction, (BOOL*) &nType, NULL, NULL, NULL, &fDistance, NULL, NULL );

						// calculate projected vector
						if( nType )
							*(D3DXVECTOR3*)( pVertex + nTargetPositionOffset ) = vct3Center + vct3Direction * ( fDistance - fRadius ) - *(D3DXVECTOR3*)( pVertex + nSourcePositionOffset );
						else
							*(D3DXVECTOR3*)( pVertex + nTargetPositionOffset ) = vct3Center - *(D3DXVECTOR3*)( pVertex + nSourcePositionOffset );

						// goto next vertex
						pVertex += nVertexSize;
					}

					// unlock vertex buffer
					pmeshDestination->UnlockVertexBuffer();
				}

				// check for success, copy destination mesh pointer and add a reference
				if( SUCCEEDED( hResult ) )
					( *ppmeshMorphing = pmeshDestination )->AddRef();

				// release destination mesh
				pmeshDestination->Release();
			}

			// release Direct3D device
			pd3dd->Release();
		}
	}

	return hResult;
}
