//
// Direct3D Color Grader library 1.1 for ShaderX³
//
// ©2004 Ronny Burkersroda <RBurkersroda@lightrock.biz>
//
//
// This software is property of the Ronny Burkersroda and may not be copied,
// redistributed or published in any way without permission!
// Abuse will immediately result in legal steps without preliminary warnings.
//
// You are allowed to use this software in your own products, if they are only
// published as compiled programs. Please inform me about the product and add
// me to the credits!
//

#include					"D3D Color Grader.h"						// Direct3D color grader class
#include					<d3dx9.h>									// Direct3D 9 extensions

#ifdef						_DEBUG_OUTPUT

#include					<stdio.h>									// standard in- and output

#endif

namespace					ScreenVertexShaderForPS_1_1
{
	#include					"screen for ps_1_1.vsh.h"					// screen vertex shader code
}

namespace					ScreenVertexShaderForPS_1_4
{
	#include					"screen for ps_1_4.vsh.h"					// screen vertex shader code
}

namespace					VolumeTextureColorGradingPixelShader
{
	#include					"volume texture color grading.1.1.psh.h"	// volume texture color grading 1.1 pixel shader code
	#include					"volume texture color grading.1.4.psh.h"	// volume texture color grading 1.4 pixel shader code
}


// ***********************
// *** constant values ***
// ***********************

// strings

#ifdef						_DEBUG_OUTPUT

const PCSTR					c_pstrD3DColorGraderDeviceError				// error for wrong device
								= "D3DColorGrader ERROR: Given argument is not a pointer to a valid Direct3D device interface.\n";

#endif


// *****************************
// *** class implementations ***
// *****************************

//
// standard constructor
//	sets standard values and registers the new object.
//
CD3DColorGrader::CD3DColorGrader()
	: m_nReferenceQuantity( 1 )
	, m_pd3dd( NULL )
	, m_pvltxGradient( NULL )
	, m_pvtxbScreen( NULL )
	, m_pvshdScreen( NULL )
	, m_ppshdColorGrading( NULL )
	, m_nPixelShaderVersion( 0 )
	, m_nRedResolution( 2 )
	, m_nGreenResolution( 2 )
	, m_nBlueResolution( 2 )
	, m_cgftFormat( D3DCOLGRFMT_24BIT )
{
}

//
// standard destructor
//	unregisters the deleted object.
//
CD3DColorGrader::~CD3DColorGrader()
{
	// release gradient texture, vertex buffer, declaration and shaders
	if( m_pvltxGradient )
		m_pvltxGradient->Release();
	if( m_pvtxbScreen )
		m_pvtxbScreen->Release();
	if( m_ppshdColorGrading )
		m_ppshdColorGrading->Release();
	if( m_pvshdScreen )
		m_pvshdScreen->Release();

	// release Direct3D device
	if( m_pd3dd )
		m_pd3dd->Release();
}

//
// create static method
//	creates an new object of this class and returns it interface.
//
PD3DCOLORGRADER9 CD3DColorGrader::Create( PDIRECT3DDEVICE9 pd3dd )
{
	// create new object
	if( CD3DColorGrader* pcolg = new CD3DColorGrader() )
	{
		// set Direct3D device to object and add a reference
		( pcolg->m_pd3dd = pd3dd )->AddRef();

		return pcolg;
	}
	
	return NULL;
}

//
// release method
//	decreases references count and deletes the object if count has reached zero.
//
DWORD CD3DColorGrader::Release()
{
	// decrease references count and return it if it is larger than zero
	if( InterlockedDecrement( (LONG volatile*) &m_nReferenceQuantity ) )
		return m_nReferenceQuantity;

	// delete this object
	delete this;

	// return zero as count
	return 0;
}

//
// add reference method
//	increases references count and returns it.
//
DWORD CD3DColorGrader::AddRef()
{
	return InterlockedIncrement( (LONG volatile*) &m_nReferenceQuantity );
}

//
// get reference method
//	returns current references count.
//
DWORD CD3DColorGrader::GetRef() const
{
	return InterlockedCompareExchange( (LONG volatile*) &m_nReferenceQuantity, 0, 0 );
}

//
// set gradient format method
//	changes the resolution and format of the gradient.
//
HRESULT CD3DColorGrader::SetGradientFormat( DWORD nRed, DWORD nGreen, DWORD nBlue, D3DCOLORGRADIENTFORMATTYPE cgftFormat )
{
	// correct red resolution
	if( nRed > 256 )
		nRed = 256;
	else if( !nRed )
		nRed = m_nRedResolution;

	// correct green resolution
	if( nGreen > 256 )
		nGreen = 256;
	else if( !nGreen )
		nGreen = m_nGreenResolution;

	// correct blue resolution
	if( nBlue > 256 )
		nBlue = 256;
	else if( !nBlue )
		nBlue = m_nBlueResolution;

	// correct format
	if( ( cgftFormat > D3DCOLGRFMT_COMPRESSEDARGB ) || ( cgftFormat == D3DCOLGRFMT_UNKNOWN ) )
		cgftFormat = m_cgftFormat;

	// check resolution values for change and rebuild texture
	if( ( nRed != m_nRedResolution ) || ( nGreen != m_nGreenResolution ) || ( nBlue != m_nBlueResolution ) || ( cgftFormat != m_cgftFormat ) )
		return CreateGradientTexture( nRed, nGreen, nBlue, cgftFormat );

	return S_FALSE;
}

//
// get gradient format method
//	copies the resolution and format of the gradient.
//
HRESULT CD3DColorGrader::GetGradientFormat( DWORD* pnRed, DWORD* pnGreen, DWORD* pnBlue, D3DCOLORGRADIENTFORMATTYPE* pcgftFormat ) const
{
	// copy resolution values
	if( pnRed )
		*pnRed			= m_nRedResolution;
	if( pnGreen )
		*pnGreen		= m_nGreenResolution;
	if( pnBlue )
		*pnBlue			= m_nBlueResolution;
	if( pcgftFormat )
		*pcgftFormat	= m_cgftFormat;

	return S_OK;
}

//
// fill gradient method
//	changes content of the gradient texture.
//
HRESULT CD3DColorGrader::FillGradient( PD3DGRADIENTFUNCTION pgrfcCallback, WPARAM wprmParameter )
{
	HRESULT hResult;

	// check for existing texture or create a new one and fill texture with the gradient calculated by the callback function
	if( m_pvltxGradient || SUCCEEDED( hResult = CreateGradientTexture( m_nRedResolution, m_nGreenResolution, m_nBlueResolution, m_cgftFormat ) ) )
		hResult = D3DXFillVolumeTexture( m_pvltxGradient, (LPD3DXFILL3D) pgrfcCallback, (PVOID) wprmParameter );

	return hResult;
}

//
// reset gradient method
//	releases gradient texture and restores all start up settings.
//
HRESULT CD3DColorGrader::ResetGradient()
{
	// check for existing texture and release it
	if( m_pvltxGradient )
	{
		m_pvltxGradient->Release();
		m_pvltxGradient = NULL;
	}

	// check for existing vertex buffer and release it
	if( m_pvtxbScreen )
	{
		m_pvtxbScreen->Release();
		m_pvtxbScreen = NULL;
	}

	// check for vertex and pixel shader and release them
	if( m_pvshdScreen )
	{
		m_pvshdScreen->Release();
		m_pvshdScreen = NULL;
	}
	if( m_ppshdColorGrading )
	{
		m_ppshdColorGrading->Release();
		m_ppshdColorGrading = NULL;
		
		m_nPixelShaderVersion = 0;
	}

	// reset resolution and format
	m_nRedResolution	= 2;
	m_nGreenResolution	= 2;
	m_nBlueResolution	= 2;
	m_cgftFormat		= D3DCOLGRFMT_24BIT;

	return S_OK;
}

//
// save 3D gradient method
//	stores the gradient to a DDS file.
//
HRESULT CD3DColorGrader::SaveGradientTo3DFile( PCTSTR pstrFileName )
{
	// check parameter
	if( !pstrFileName )
		return E_INVALIDARG;

	// check for gradient texture
	if( !m_pvltxGradient )
		return E_UNEXPECTED;

	// store 3D texture to DDS file
	return D3DXSaveTextureToFile( pstrFileName, D3DXIFF_DDS, m_pvltxGradient, NULL );
}

//
// save 2D gradient method
//	stores the gradient to a BMP file.
//
HRESULT CD3DColorGrader::SaveGradientTo2DFile( PCTSTR pstrFileName )
{
	D3DLOCKED_BOX			lbox;
	D3DLOCKED_RECT			lrct;
	PDIRECT3DTEXTURE9		ptxr;
	PDIRECT3DVOLUMETEXTURE9 pvltx;
	PDIRECT3DVOLUME9		pvolSource;
	PDIRECT3DVOLUME9		pvolDestination;
	HRESULT					hResult;
	DWORD					nRed;
	DWORD					nGreen;
	DWORD					nBlue;
	DWORD					nWidth;
	DWORD					nColumn;
	DWORD					nLine;

	// check parameter
	if( !pstrFileName )
		return E_INVALIDARG;

	// check for gradient texture
	if( !m_pvltxGradient )
		return E_UNEXPECTED;

	// create 24 bit volume texture as scratch
	if( SUCCEEDED( hResult = D3DXCreateVolumeTexture( m_pd3dd, m_nRedResolution, m_nGreenResolution, m_nBlueResolution, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &pvltx ) ) )
	{
		// calculate length of a line and resulting height of a column
		nWidth	= (DWORD) ceil( sqrt( (DOUBLE)( nLine = m_nRedResolution * m_nGreenResolution * m_nBlueResolution ) ) );
		nLine	= (DWORD) ceil( (DOUBLE) nLine / (DOUBLE) nWidth );

		// create 2D texture
		if( SUCCEEDED( hResult = D3DXCreateTexture( m_pd3dd, nWidth, nLine, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &ptxr ) ) )
		{
			// get source volume
			if( SUCCEEDED( hResult = m_pvltxGradient->GetVolumeLevel( 0, &pvolSource ) ) )
			{
				// get destination volume
				if( SUCCEEDED( hResult = pvltx->GetVolumeLevel( 0, &pvolDestination ) ) )
				{
					// load new volume from old one
					hResult = D3DXLoadVolumeFromVolume( pvolDestination, NULL, NULL, pvolSource, NULL, NULL, D3DX_FILTER_POINT, 0 );

					// release destination volume
					pvolDestination->Release();
				}

				// release source volume
				pvolSource->Release();
			}

			// lock volume box
			if( SUCCEEDED( hResult ) && SUCCEEDED( hResult = pvltx->LockBox( 0, &lbox, NULL, 0 ) ) )
			{
				// lock surface rectangle
				if( SUCCEEDED( hResult = ptxr->LockRect( 0, &lrct, NULL, 0 ) ) )
				{
					// copy all pixels from 3D volume to 2D surface
					nColumn	= 0;
					nLine	= 0;
					for( nRed = 0; nRed < m_nRedResolution; ++nRed ) for( nGreen = 0; nGreen < m_nGreenResolution; ++nGreen ) for( nBlue = 0; nBlue < m_nBlueResolution; ++nBlue )
					{
						// copy current pixel
						memcpy( &( (PBYTE) lrct.pBits )[ nColumn * 3 + nLine * lrct.Pitch ], &( (PBYTE) lbox.pBits )[ nRed * 3 + nGreen * lbox.RowPitch + nBlue * lbox.SlicePitch ], 3 );

						// increase column number and check for end of line
						if( ++nColumn >= nWidth )
						{
							// increase line and reset column number
							++nLine;
							nColumn = 0;
						}
					}

					// unlock surface rectangle
					ptxr->UnlockRect( 0 );
				}

				// unlock volume box
				pvltx->UnlockBox( 0 );
			}

			// store 2D texture to BMP file
			if( SUCCEEDED( hResult ) )
				hResult = D3DXSaveTextureToFile( pstrFileName, D3DXIFF_BMP, ptxr, NULL );

			// release 2D texture
			ptxr->Release();
		}

		// release volume texture
		pvltx->Release();
	}

	return hResult;
}

//
// save gradient to memory method
//	stores the gradient to a buffer.
//
HRESULT CD3DColorGrader::SaveGradientToMemory( PVOID pBuffer, PDWORD pnSize )
{
	D3DLOCKED_BOX			lbox;
	PDIRECT3DVOLUMETEXTURE9 pvltx;
	PDIRECT3DVOLUME9		pvolSource;
	PDIRECT3DVOLUME9		pvolDestination;
	HRESULT					hResult;
	DWORD					nRed;
	DWORD					nGreen;
	DWORD					nBlue;

	// check parameters
	if( !pBuffer && !pnSize )
		return E_INVALIDARG;

	// check for gradient texture
	if( !m_pvltxGradient )
		return E_UNEXPECTED;

	// check for buffer
	if( pBuffer )
	{
		// check size
		if( !pnSize || ( *pnSize >= m_nRedResolution * m_nGreenResolution * m_nBlueResolution * 3 + 20 ) )
		{
			// copy ID to buffer
			strcpy( (PSTR) pBuffer, "ColGrad" );
			*(PBYTE*) &pBuffer += 8;

			// copy dimension to buffer
			( (PDWORD) pBuffer )[ 0 ] = m_nRedResolution;
			( (PDWORD) pBuffer )[ 1 ] = m_nGreenResolution;
			( (PDWORD) pBuffer )[ 2 ] = m_nBlueResolution;
			*(PDWORD*) &pBuffer += 3;

			// create 24 bit volume texture as scratch
			if( SUCCEEDED( hResult = D3DXCreateVolumeTexture( m_pd3dd, m_nRedResolution, m_nGreenResolution, m_nBlueResolution, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &pvltx ) ) )
			{
				// get source volume
				if( SUCCEEDED( hResult = m_pvltxGradient->GetVolumeLevel( 0, &pvolSource ) ) )
				{
					// get destination volume
					if( SUCCEEDED( hResult = pvltx->GetVolumeLevel( 0, &pvolDestination ) ) )
					{
						// load new volume from old one
						hResult = D3DXLoadVolumeFromVolume( pvolDestination, NULL, NULL, pvolSource, NULL, NULL, D3DX_FILTER_POINT, 0 );

						// release destination volume
						pvolDestination->Release();
					}

					// release source volume
					pvolSource->Release();
				}

				// lock volume box
				if( SUCCEEDED( hResult ) && SUCCEEDED( hResult = pvltx->LockBox( 0, &lbox, NULL, 0 ) ) )
				{
					// copy all pixels from 3D volume to buffer
					for( nRed = 0; nRed < m_nRedResolution; ++nRed ) for( nGreen = 0; nGreen < m_nGreenResolution; ++nGreen ) for( nBlue = 0; nBlue < m_nBlueResolution; ++nBlue )
					{
						memcpy( pBuffer, &( (PBYTE) lbox.pBits )[ nRed * 3 + nGreen * lbox.RowPitch + nBlue * lbox.SlicePitch ], 3 );
						*(PBYTE*) &pBuffer += 3;
					}

					// unlock volume box
					pvltx->UnlockBox( 0 );
				}

				// release volume texture
				pvltx->Release();
			}
		}
	}
	else
		hResult	= S_OK;
	
	// calculate size of the gradient
	if( pnSize )
		*pnSize	= m_nRedResolution * m_nGreenResolution * m_nBlueResolution * 3 + 20;

	return hResult;
}

//
// load gradient from file method
//	loads the gradient from a file.
//
HRESULT CD3DColorGrader::LoadGradientFromFile( PCTSTR pstrFileName, DWORD nRed, DWORD nGreen, DWORD nBlue )
{
	D3DLOCKED_BOX			lbox;
	D3DLOCKED_RECT			lrct;
	D3DXIMAGE_INFO			imgi;
	PDIRECT3DTEXTURE9		ptxr;
	PDIRECT3DVOLUMETEXTURE9 pvltx;
	PDIRECT3DVOLUME9		pvolSource;
	PDIRECT3DVOLUME9		pvolDestination;
	HRESULT					hResult;
	DWORD					nWidth;
	DWORD					nColumn;
	DWORD					nLine;

	// check parameter
	if( !pstrFileName )
		return E_INVALIDARG;

	// check for gradient texture
	if( !m_pvltxGradient )
		return E_UNEXPECTED;

	// get image information
	if( FAILED( hResult = D3DXGetImageInfoFromFile( pstrFileName, &imgi ) ) )
		return hResult;

	// check for non-volume texture
	if( imgi.ResourceType != D3DRTYPE_VOLUMETEXTURE )
	{
		// check if dimension is given
		if( !nRed || !nGreen || !nBlue )
			return E_INVALIDARG;

		// load 2D texture from file
		if( SUCCEEDED( hResult = D3DXCreateTextureFromFileEx( m_pd3dd, pstrFileName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0, &imgi, NULL, &ptxr ) ) )
		{
			// set length of a line and calculate resulting height of a column
			nWidth	= imgi.Width;
			nLine	= (DWORD) ceil( (DOUBLE)( nRed * nGreen * nBlue ) / (DOUBLE) nWidth );

			// check if volume with given dimension fits into the 2D image
			if( imgi.Width * imgi.Height >= nWidth * nLine )
			{
				// create 24 bit volume texture as scratch
				if( SUCCEEDED( hResult = D3DXCreateVolumeTexture( m_pd3dd, nRed, nGreen, nBlue, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &pvltx ) ) )
				{
					// lock volume box
					if( SUCCEEDED( hResult = pvltx->LockBox( 0, &lbox, NULL, 0 ) ) )
					{
						// lock surface rectangle
						if( SUCCEEDED( hResult = ptxr->LockRect( 0, &lrct, NULL, 0 ) ) )
						{
							// copy all pixels from 2D surface to 3D volume
							nColumn	= 0;
							nLine	= 0;
							for( imgi.Width = 0; imgi.Width < nRed; ++imgi.Width ) for( imgi.Height = 0; imgi.Height < nGreen; ++imgi.Height ) for( imgi.Depth = 0; imgi.Depth < nBlue; ++imgi.Depth )
							{
								// copy current pixel
								memcpy( &( (PBYTE) lbox.pBits )[ imgi.Width * 3 + imgi.Height * lbox.RowPitch + imgi.Depth * lbox.SlicePitch ], &( (PBYTE) lrct.pBits )[ nColumn * 3 + nLine * lrct.Pitch ], 3 );

								// increase column number and check for end of line
								if( ++nColumn >= nWidth )
								{
									// increase line and reset column number
									++nLine;
									nColumn = 0;
								}
							}

							// unlock surface rectangle
							ptxr->UnlockRect( 0 );
						}

						// unlock volume box
						pvltx->UnlockBox( 0 );
					}
				}
			}
			else
				hResult = E_INVALIDARG;

			// release 2D texture
			ptxr->Release();
		}
	}
	else
		// load 3D texture from file
		hResult = D3DXCreateVolumeTextureFromFileEx( m_pd3dd, pstrFileName, nRed, nGreen, nBlue, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0, &imgi, NULL, &pvltx );

	// check current result
	if( SUCCEEDED( hResult ) )
	{
		// get source volume
		if( SUCCEEDED( hResult = pvltx->GetVolumeLevel( 0, &pvolSource ) ) )
		{
			// get destination volume
			if( SUCCEEDED( hResult = m_pvltxGradient->GetVolumeLevel( 0, &pvolDestination ) ) )
			{
				// load new volume from old one
				hResult = D3DXLoadVolumeFromVolume( pvolDestination, NULL, NULL, pvolSource, NULL, NULL, D3DX_FILTER_TRIANGLE, 0 );

				// release destination volume
				pvolDestination->Release();
			}

			// release source volume
			pvolSource->Release();
		}

		// release volume texture
		pvltx->Release();
	}

	return hResult;
}

//
// load gradient from memory method
//	loads the gradient from a buffer.
//
HRESULT CD3DColorGrader::LoadGradientFromMemory( const VOID* pData, DWORD nSize, DWORD nRed, DWORD nGreen, DWORD nBlue )
{
	D3DLOCKED_BOX			lbox;
	D3DLOCKED_RECT			lrct;
	D3DXIMAGE_INFO			imgi;
	PDIRECT3DTEXTURE9		ptxr;
	PDIRECT3DVOLUMETEXTURE9 pvltx;
	PDIRECT3DVOLUME9		pvolSource;
	PDIRECT3DVOLUME9		pvolDestination;
	HRESULT					hResult;
	DWORD					nWidth;
	DWORD					nColumn;
	DWORD					nLine;

	// check parameters
	if( !pData || !nSize )
		return E_INVALIDARG;

	// check for gradient texture
	if( !m_pvltxGradient )
		return E_UNEXPECTED;

	// get image information
	if( SUCCEEDED( hResult = D3DXGetImageInfoFromFileInMemory( pData, nSize, &imgi ) ) )
	{
		// check for non-volume texture
		if( imgi.ResourceType != D3DRTYPE_VOLUMETEXTURE )
		{
			// check if dimension is given
			if( !nRed || !nGreen || !nBlue )
				return E_INVALIDARG;

			// load 2D texture from file in memory
			if( SUCCEEDED( hResult = D3DXCreateTextureFromFileInMemoryEx( m_pd3dd, pData, nSize, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0, &imgi, NULL, &ptxr ) ) )
			{
				// set length of a line and calculate resulting height of a column
				nWidth	= imgi.Width;
				nLine	= (DWORD) ceil( (DOUBLE)( nRed * nGreen * nBlue ) / (DOUBLE) nWidth );

				// check if volume with given dimension fits into the 2D image
				if( imgi.Width * imgi.Height >= nWidth * nLine )
				{
					// create 24 bit volume texture as scratch
					if( SUCCEEDED( hResult = D3DXCreateVolumeTexture( m_pd3dd, nRed, nGreen, nBlue, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &pvltx ) ) )
					{
						// lock volume box
						if( SUCCEEDED( hResult = pvltx->LockBox( 0, &lbox, NULL, 0 ) ) )
						{
							// lock surface rectangle
							if( SUCCEEDED( hResult = ptxr->LockRect( 0, &lrct, NULL, 0 ) ) )
							{
								// copy all pixels from 2D surface to 3D volume
								nColumn	= 0;
								nLine	= 0;
								for( imgi.Width = 0; imgi.Width < nRed; ++imgi.Width ) for( imgi.Height = 0; imgi.Height < nGreen; ++imgi.Height ) for( imgi.Depth = 0; imgi.Depth < nBlue; ++imgi.Depth )
								{
									// copy current pixel
									memcpy( &( (PBYTE) lbox.pBits )[ imgi.Width * 3 + imgi.Height * lbox.RowPitch + imgi.Depth * lbox.SlicePitch ], &( (PBYTE) lrct.pBits )[ nColumn * 3 + nLine * lrct.Pitch ], 3 );

									// increase column number and check for end of line
									if( ++nColumn >= nWidth )
									{
										// increase line and reset column number
										++nLine;
										nColumn = 0;
									}
								}

								// unlock surface rectangle
								ptxr->UnlockRect( 0 );
							}

							// unlock volume box
							pvltx->UnlockBox( 0 );
						}
					}
				}
				else
					hResult = E_INVALIDARG;

				// release 2D texture
				ptxr->Release();
			}
		}
		else
			// load 3D texture from file in memory
			hResult = D3DXCreateVolumeTextureFromFileInMemoryEx( m_pd3dd, pData, nSize, nRed, nGreen, nBlue, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0, &imgi, NULL, &pvltx );
	}
	// check minimum size and ID of own format
	else if( ( nSize >= 20 ) && !strcmp( (PCSTR) pData, "ColGrad" ) )
	{
		*(const BYTE**) &pData += 8;

		// get dimension from data
		nWidth	= ( (const DWORD*) pData )[ 0 ];
		nLine	= ( (const DWORD*) pData )[ 1 ];
		nColumn	= ( (const DWORD*) pData )[ 2 ];
		*(const DWORD**) &pData += 3;

		// check buffer size
		if( nSize >= nWidth * nLine * nColumn * 3 + 20 )
		{
			// create 24 bit volume texture as scratch
			if( SUCCEEDED( hResult = D3DXCreateVolumeTexture( m_pd3dd, nWidth, nLine, nColumn, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &pvltx ) ) )
			{
				// lock volume box
				if( SUCCEEDED( hResult = pvltx->LockBox( 0, &lbox, NULL, 0 ) ) )
				{
					// copy all pixels from data to 3D volume
					for( imgi.Width = 0; imgi.Width < nWidth; ++imgi.Width ) for( imgi.Height = 0; imgi.Height < nLine; ++imgi.Height ) for( imgi.Depth = 0; imgi.Depth < nColumn; ++imgi.Depth )
					{
						memcpy( &( (PBYTE) lbox.pBits )[ imgi.Width * 3 + imgi.Height * lbox.RowPitch + imgi.Depth * lbox.SlicePitch ], pData, 3 );
						*(PBYTE*) &pData += 3;
					}

					// unlock volume box
					pvltx->UnlockBox( 0 );
				}
			}
		}
		else
			hResult = E_OUTOFMEMORY;
	}

	// check current result
	if( SUCCEEDED( hResult ) )
	{
		// get source volume
		if( SUCCEEDED( hResult = pvltx->GetVolumeLevel( 0, &pvolSource ) ) )
		{
			// get destination volume
			if( SUCCEEDED( hResult = m_pvltxGradient->GetVolumeLevel( 0, &pvolDestination ) ) )
			{
				// load new volume from old one
				hResult = D3DXLoadVolumeFromVolume( pvolDestination, NULL, NULL, pvolSource, NULL, NULL, D3DX_FILTER_TRIANGLE, 0 );

				// release destination volume
				pvolDestination->Release();
			}

			// release source volume
			pvolSource->Release();
		}

		// release volume texture
		pvltx->Release();
	}

	return hResult;
}

//
// render screen method
//	draws the given texture with graded colors to current render target.
//
HRESULT CD3DColorGrader::RenderScreen( PDIRECT3DTEXTURE9 ptxrSource, const TEXTUREPLACEMENT* ptxplTextureCoordinates, D3DTEXTUREFILTERTYPE txftFilter )
{
	typedef struct		_VERTEX
	{
		D3DXVECTOR3			vct3Position;
		D3DXVECTOR3			vct3Normal;
		D3DXVECTOR2			vct2Texture;
	}
	VERTEX;

	FLOAT				fConstants[ 16 ];
	DWORD				nSamplingStates[ 2 ][ 6 ];
	DWORD				nRenderStates[ 2 ];
	D3DXMATRIX			mtrxProjection;
	D3DSURFACE_DESC		sfds;
	PVOID				pData;
	HRESULT				hResult;

	// get render target
	if( SUCCEEDED( hResult = m_pd3dd->GetRenderTarget( 0, (PDIRECT3DSURFACE9*) &pData ) ) )
	{
		// get description of the render target
		hResult = ( (PDIRECT3DSURFACE9) pData )->GetDesc( &sfds );

		// release render target
		( (PDIRECT3DSURFACE9) pData )->Release();
	}

	// check for existing vertex buffer
	if( !m_pvtxbScreen && SUCCEEDED( hResult ) )
	{
		// create vertex buffer for screen rendering
		if( SUCCEEDED( hResult = m_pd3dd->CreateVertexBuffer( sizeof( VERTEX ) << 2, D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, &m_pvtxbScreen, NULL ) ) )
		{
			// lock vertex buffer
			if( SUCCEEDED( hResult = m_pvtxbScreen->Lock( 0, 0, &pData, 0 ) ) )
			{
				// set vertices
				( (VERTEX*) pData )[ 0 ].vct3Position	= D3DXVECTOR3( -1.0f, 1.0f, 0.0f );
				( (VERTEX*) pData )[ 0 ].vct3Normal		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				( (VERTEX*) pData )[ 0 ].vct2Texture	= D3DXVECTOR2( 0.0f, 0.0f );
				( (VERTEX*) pData )[ 1 ].vct3Position	= D3DXVECTOR3( 1.0f, 1.0f, 0.0f );
				( (VERTEX*) pData )[ 1 ].vct3Normal		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				( (VERTEX*) pData )[ 1 ].vct2Texture	= D3DXVECTOR2( 1.0f, 0.0f );
				( (VERTEX*) pData )[ 2 ].vct3Position	= D3DXVECTOR3( -1.0f, -1.0f, 0.0f );
				( (VERTEX*) pData )[ 2 ].vct3Normal		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				( (VERTEX*) pData )[ 2 ].vct2Texture	= D3DXVECTOR2( 0.0f, 1.0f );
				( (VERTEX*) pData )[ 3 ].vct3Position	= D3DXVECTOR3( 1.0f, -1.0f, 0.0f );
				( (VERTEX*) pData )[ 3 ].vct3Normal		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				( (VERTEX*) pData )[ 3 ].vct2Texture	= D3DXVECTOR2( 1.0f, 1.0f );

				// unlock vertex buffer
				m_pvtxbScreen->Unlock();
			}
		}
	}

	// check for existing pixel shader and create it if needed
	if( !m_ppshdColorGrading && SUCCEEDED( hResult ) )
	{
		// try to create pixel shader version 1.4
		if( FAILED( hResult = m_pd3dd->CreatePixelShader( VolumeTextureColorGradingPixelShader::g_ps14_main, &m_ppshdColorGrading ) ) )
		{
			// create pixel shader version 1.1
			if( SUCCEEDED( hResult = m_pd3dd->CreatePixelShader( VolumeTextureColorGradingPixelShader::g_ps11_main, &m_ppshdColorGrading ) ) )
				m_nPixelShaderVersion = 0x00011000;
		}
		else
			m_nPixelShaderVersion = 0x00014000;
	}

	// check for existing vertex shader and create it if needed
	if( !m_pvshdScreen && SUCCEEDED( hResult ) ) switch( m_nPixelShaderVersion )
	{
	case 0x00014000:

		hResult = m_pd3dd->CreateVertexShader( ScreenVertexShaderForPS_1_4::g_vs11_main, &m_pvshdScreen );

		break;

	default:

		hResult = m_pd3dd->CreateVertexShader( ScreenVertexShaderForPS_1_1::g_vs11_main, &m_pvshdScreen );
	}

	if( SUCCEEDED( hResult ) )
	{
		// build view matrix
		D3DXMatrixLookAtLH( &mtrxProjection, &D3DXVECTOR3( 0.0f, 0.0f, -1.1f ), &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

		// build projection matrix and multiply it to view matrix
		D3DXMatrixMultiply( &mtrxProjection, &mtrxProjection, D3DXMatrixOrthoLH( (D3DXMATRIX*) fConstants, 2.0f, 2.0f, 0.1f, 2.1f ) );

		// get original vertex shader constants
		if( SUCCEEDED( hResult = m_pd3dd->GetVertexShaderConstantF( 0, fConstants, 4 ) ) )
		{
			// set projection matrix
			if( SUCCEEDED( hResult = m_pd3dd->SetVertexShaderConstantF( 0, (const FLOAT*) &mtrxProjection, 4 ) ) )
			{
				// set vertex buffer
				m_pd3dd->SetStreamSource( 0, m_pvtxbScreen, 0, sizeof( VERTEX ) );

				// unset vertex declaration and set flexible vertex format
				m_pd3dd->SetVertexDeclaration( NULL );
				m_pd3dd->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

				// set vertex and pixel shader
				m_pd3dd->SetVertexShader( m_pvshdScreen );
				m_pd3dd->SetPixelShader( m_ppshdColorGrading );

				// get original render states
				m_pd3dd->GetRenderState( D3DRS_MULTISAMPLEANTIALIAS, &nRenderStates[ 0 ] );

				// set render states
				m_pd3dd->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );

				// get original sampler states
				m_pd3dd->GetSamplerState( 0, D3DSAMP_ADDRESSU, &nSamplingStates[ 0 ][ 0 ] );
				m_pd3dd->GetSamplerState( 0, D3DSAMP_ADDRESSV, &nSamplingStates[ 0 ][ 1 ] );
				m_pd3dd->GetSamplerState( 0, D3DSAMP_ADDRESSW, &nSamplingStates[ 0 ][ 2 ] );
				m_pd3dd->GetSamplerState( 0, D3DSAMP_MAGFILTER, &nSamplingStates[ 0 ][ 4 ] );
				m_pd3dd->GetSamplerState( 0, D3DSAMP_MINFILTER, &nSamplingStates[ 0 ][ 3 ] );
				m_pd3dd->GetSamplerState( 0, D3DSAMP_MIPFILTER, &nSamplingStates[ 0 ][ 5 ] );

				if( m_nPixelShaderVersion == 0x00014000 )
				{
					m_pd3dd->GetSamplerState( 1, D3DSAMP_ADDRESSU, &nSamplingStates[ 1 ][ 0 ] );
					m_pd3dd->GetSamplerState( 1, D3DSAMP_ADDRESSV, &nSamplingStates[ 1 ][ 1 ] );
					m_pd3dd->GetSamplerState( 1, D3DSAMP_ADDRESSW, &nSamplingStates[ 1 ][ 2 ] );
					m_pd3dd->GetSamplerState( 1, D3DSAMP_MAGFILTER, &nSamplingStates[ 1 ][ 4 ] );
					m_pd3dd->GetSamplerState( 1, D3DSAMP_MINFILTER, &nSamplingStates[ 1 ][ 3 ] );
					m_pd3dd->GetSamplerState( 1, D3DSAMP_MIPFILTER, &nSamplingStates[ 1 ][ 5 ] );
				}
				else
				{
					m_pd3dd->GetSamplerState( 3, D3DSAMP_ADDRESSU, &nSamplingStates[ 1 ][ 0 ] );
					m_pd3dd->GetSamplerState( 3, D3DSAMP_ADDRESSV, &nSamplingStates[ 1 ][ 1 ] );
					m_pd3dd->GetSamplerState( 3, D3DSAMP_ADDRESSW, &nSamplingStates[ 1 ][ 2 ] );
					m_pd3dd->GetSamplerState( 3, D3DSAMP_MAGFILTER, &nSamplingStates[ 1 ][ 4 ] );
					m_pd3dd->GetSamplerState( 3, D3DSAMP_MINFILTER, &nSamplingStates[ 1 ][ 3 ] );
					m_pd3dd->GetSamplerState( 3, D3DSAMP_MIPFILTER, &nSamplingStates[ 1 ][ 5 ] );
				}

				// set sampler states
				if( m_nPixelShaderVersion == 0x00014000 )
				{
					m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_MAGFILTER, txftFilter );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_MINFILTER, txftFilter );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
				}
				else
				{
					m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
					m_pd3dd->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_MAGFILTER, txftFilter );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_MINFILTER, txftFilter );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
				}

				// set textures
				if( m_nPixelShaderVersion == 0x00014000 )
				{
					m_pd3dd->SetTexture( 0, m_pvltxGradient );
					m_pd3dd->SetTexture( 1, ptxrSource );
				}
				else
				{
					m_pd3dd->SetTexture( 0, ptxrSource );
					m_pd3dd->SetTexture( 1, NULL );
					m_pd3dd->SetTexture( 2, NULL );
					m_pd3dd->SetTexture( 3, m_pvltxGradient );
				}

				// draw rectangle to render target
				m_pd3dd->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

				// restore original sampler states
				m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSU, nSamplingStates[ 0 ][ 0 ] );
				m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSV, nSamplingStates[ 0 ][ 1 ] );
				m_pd3dd->SetSamplerState( 0, D3DSAMP_ADDRESSW, nSamplingStates[ 0 ][ 2 ] );
				m_pd3dd->SetSamplerState( 0, D3DSAMP_MAGFILTER, nSamplingStates[ 0 ][ 4 ] );
				m_pd3dd->SetSamplerState( 0, D3DSAMP_MINFILTER, nSamplingStates[ 0 ][ 3 ] );
				m_pd3dd->SetSamplerState( 0, D3DSAMP_MIPFILTER, nSamplingStates[ 0 ][ 5 ] );

				if( m_nPixelShaderVersion == 0x00014000 )
				{
					m_pd3dd->SetSamplerState( 1, D3DSAMP_ADDRESSU, nSamplingStates[ 1 ][ 0 ] );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_ADDRESSV, nSamplingStates[ 1 ][ 1 ] );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_ADDRESSW, nSamplingStates[ 1 ][ 2 ] );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_MAGFILTER, nSamplingStates[ 1 ][ 4 ] );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_MINFILTER, nSamplingStates[ 1 ][ 3 ] );
					m_pd3dd->SetSamplerState( 1, D3DSAMP_MIPFILTER, nSamplingStates[ 1 ][ 5 ] );
				}
				else
				{
					m_pd3dd->SetSamplerState( 3, D3DSAMP_ADDRESSU, nSamplingStates[ 1 ][ 0 ] );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_ADDRESSV, nSamplingStates[ 1 ][ 1 ] );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_ADDRESSW, nSamplingStates[ 1 ][ 2 ] );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_MAGFILTER, nSamplingStates[ 1 ][ 4 ] );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_MINFILTER, nSamplingStates[ 1 ][ 3 ] );
					m_pd3dd->SetSamplerState( 3, D3DSAMP_MIPFILTER, nSamplingStates[ 1 ][ 5 ] );
				}

				// restore original render states
				m_pd3dd->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, nRenderStates[ 0 ] );
			}

			// restore original vertex shader constants
			m_pd3dd->SetVertexShaderConstantF( 0, fConstants, 4 );
		}
	}

	return hResult;
}

//
// create gradient texture method
//	creates a new volume texture of the given resolution and copies volume from old texture, if such exists.
//
HRESULT CD3DColorGrader::CreateGradientTexture( DWORD nRed, DWORD nGreen, DWORD nBlue, D3DCOLORGRADIENTFORMATTYPE cgftFormat )
{
	PDIRECT3DVOLUMETEXTURE9	pvltx;
	D3DVOLUME_DESC			vlds;
	HRESULT					hResult;

	// set Direct3D format for texture
	switch( cgftFormat )
	{
	case D3DCOLGRFMT_8BIT:

		vlds.Format = D3DFMT_R3G3B2;

		break;

	case D3DCOLGRFMT_16BIT:

		vlds.Format = D3DFMT_R5G6B5;

		break;

	case D3DCOLGRFMT_24BIT:

		vlds.Format = D3DFMT_R8G8B8;

		break;

	case D3DCOLGRFMT_32BIT:

		vlds.Format = D3DFMT_A8R8G8B8;

		break;

	case D3DCOLGRFMT_COMPRESSEDRGB:

		vlds.Format = D3DFMT_DXT1;

		break;

	case D3DCOLGRFMT_COMPRESSEDARGB:

		vlds.Format = D3DFMT_DXT3;

		break;

	default:

		return D3DERR_INVALIDCALL;
	}

	// create new volume texture
	if( SUCCEEDED( hResult = D3DXCreateVolumeTexture( m_pd3dd, nRed, nGreen, nBlue, 1, D3DUSAGE_DYNAMIC, vlds.Format, D3DPOOL_DEFAULT, &pvltx ) ) )
	{
		// get description of the texture
		pvltx->GetLevelDesc( 0, &vlds );

		// set resolutions
		m_nRedResolution	= vlds.Width;
		m_nGreenResolution	= vlds.Height;
		m_nBlueResolution	= vlds.Depth;

		// set format
		switch( vlds.Format )
		{
		case D3DFMT_R3G3B2:

			m_cgftFormat = D3DCOLGRFMT_8BIT;

			break;

		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:

			m_cgftFormat = D3DCOLGRFMT_16BIT;

			break;

		case D3DFMT_R8G8B8:

			m_cgftFormat = D3DCOLGRFMT_24BIT;

			break;

		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8B8G8R8:

			m_cgftFormat = D3DCOLGRFMT_32BIT;

			break;

		case D3DFMT_DXT1:

			m_cgftFormat = D3DCOLGRFMT_COMPRESSEDRGB;

			break;

		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_DXT5:

			m_cgftFormat = D3DCOLGRFMT_COMPRESSEDARGB;

			break;

		default:

			m_cgftFormat = D3DCOLGRFMT_UNKNOWN;

			break;
		}

		// check for old texture
		if( m_pvltxGradient )
		{
			PDIRECT3DVOLUME9	pvolSource;
			PDIRECT3DVOLUME9	pvolDestination;

			// get source volume
			if( SUCCEEDED( m_pvltxGradient->GetVolumeLevel( 0, &pvolSource ) ) )
			{
				// get destination volume
				if( SUCCEEDED( pvltx->GetVolumeLevel( 0, &pvolDestination ) ) )
				{
					// load new volume from old one
					hResult = D3DXLoadVolumeFromVolume( pvolDestination, NULL, NULL, pvolSource, NULL, NULL, D3DX_FILTER_TRIANGLE, 0 );

					// release destination volume
					pvolDestination->Release();
				}

				// release source volume
				pvolSource->Release();
			}

			// release old texture
			m_pvltxGradient->Release();
		}

		// set new texture
		m_pvltxGradient = pvltx;
	}

	return hResult;
}


// ********************************
// *** function implementations ***
// ********************************

//
// Direct3D color grader create function
//	checks Direct3D device and creates a color grader object.
//
PD3DCOLORGRADER9 D3DColorGraderCreate9( PDIRECT3DDEVICE9 pd3ddDevice )
{
	PD3DCOLORGRADER9 pcolg;

	// get correct Direct3D device interface
	if( pd3ddDevice && ( pd3ddDevice->QueryInterface( IID_IDirect3DDevice9, (PVOID*) &pd3ddDevice ) == S_OK ) )
	{
		// create color grader object and store interface to return at the end of the function
		pcolg = CD3DColorGrader::Create( pd3ddDevice );

		// release Direct3D device
		pd3ddDevice->Release();
	}
	else
	{

#ifdef						_DEBUG_OUTPUT

		// output error
		OutputDebugString( c_pstrD3DColorGraderDeviceError );

#endif

		// unset interface pointer
		pcolg = NULL;
	}

	return pcolg;
}

