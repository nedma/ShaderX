#include <fstream>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include <assert.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "resource.h"
#include "dpGPU.h"
#include "gpu_dp.h"
#include "pixelshaderutil.h"
#include "neighbourhoodFuncs.h"
#include "debugFuncs.h"




// -----------------------------------------------------------------------------------------
HRESULT	DPGPU::CreateNInit_DataTexture()
{
	HRESULT hr;
	D3DXIMAGE_INFO	imageInfoOut;

	hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, m_InitOnceParams.m_pDataFileName, D3DX_DEFAULT, D3DX_DEFAULT,
		1, 0, m_DataTextureFormat, D3DPOOL_MANAGED, D3DX_FILTER_POINT, D3DX_FILTER_NONE,
		0, &imageInfoOut, NULL,	&m_pDataTexture );
	if( FAILED(hr) )
	{
		char msg[128];
		sprintf(msg,"Could not find data file \"%s\"", m_InitOnceParams.m_pDataFileName);
		MessageBox(NULL, (LPCSTR)msg, "gpu_dp.exe", MB_OK);
		return E_ABORT;
	}
	m_DataWidth = imageInfoOut.Width;
	m_DataHeight = imageInfoOut.Height;
	m_DataSize  = m_DataWidth* m_DataHeight;

	// write data into texture
	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = m_pDataTexture->LockRect( 0, &lrect, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect", hr );

	float *psData = (float*)(lrect.pBits);
	float *pdat=0;
	for ( int i=0; i< m_DataSize; i++ )
	{
		if ( D3DFMT_G32R32F == m_DataTextureFormat )
			pdat = &psData[2*i];
		else if ( D3DFMT_R32F == m_DataTextureFormat )
			pdat = &psData[i];

		if ( (*pdat)*255 > 1.0f )
			*pdat *= 100;
			*pdat = (*pdat)*255.0f; // scale to value between 0-255

		//		 scale down to avoid hitting upper limits...
	//		*pdat/= float(1<<12);
	}


	m_pDataTexture->UnlockRect(0);

	return S_OK;
}

HRESULT DPGPU::ApplyBoundaryMaskToDataTexture()
{
	HRESULT hr;
	D3DLOCKED_RECT	lrect;
	D3DLOCKED_RECT	lrect2;
	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = m_pBoundaryMapTex->LockRect( 0, &lrect, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pBoundaryMapTex", hr );
	if( FAILED( hr = m_pDataTexture->LockRect( 0, &lrect2, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pDataTexture", hr );
	// expect boundary map to be 24bit unsigned D3DFMT_R8G8B8. be wary of alignment probs.

	// source data
	unsigned char* pSrcData = (unsigned char*)(lrect.pBits);
	static const int  tstride= 4;
	float *psData = (float*)(lrect2.pBits);
	float *pdat;
	for ( int i=0; i< m_DataSize; i++ )
	{
		int		currTexel = i*tstride; //xrgb format

		if ( D3DFMT_G32R32F == m_DataTextureFormat )
			pdat = &psData[2*i];
		else if ( D3DFMT_R32F == m_DataTextureFormat )
			pdat = &psData[i];

		if ( pSrcData[currTexel] == 0 )
			*pdat = 0.0f;
	}

	m_pDataTexture->UnlockRect(0);
	m_pBoundaryMapTex->UnlockRect(0);

	return S_OK;
}

HRESULT	DPGPU::CreateNInit_NeighbourhoodData()
{
	HRESULT hr;
	// initialise neighbour mask. and residue probabilities.

	int i;
	for ( i=0; i<2; i++ )
	{
		if ( FAILED( m_pd3dDevice->CreateTexture( m_DataWidth, m_DataHeight,
			1, 0, m_NeighbourMaskFormat,  D3DPOOL_MANAGED, &m_pNeighbourMaskTex[i], NULL ) ) )
		{
			MessageBox(NULL, "Could not generate m_pNeighbourMaskTex[i] texture", "gpu_dp.exe", MB_OK);
			return E_FAIL;
		}
		if ( FAILED( m_pd3dDevice->CreateTexture( m_DataWidth, m_DataHeight,
			1, 0, m_ResidueProbsFormat,  D3DPOOL_MANAGED, &m_pResidueProbsTex[i], NULL ) ) )
		{
			MessageBox(NULL, "Could not generate m_pResidueProbsTex[i] texture", "gpu_dp.exe", MB_OK);
			return E_FAIL;
		}
	}

	D3DXIMAGE_INFO	imageInfoOut;
	// load map with boundaries. convert to a neighbourhood mask map.
	hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, m_InitOnceParams.m_pBoundaryMapFileName, m_DataWidth , m_DataHeight,
		1, 0, m_BoundaryMapFormat , D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR,
		0, &imageInfoOut, NULL,	&m_pBoundaryMapTex);
	if( FAILED(hr) )
	{
		DXTRACE_ERR( "couldn't  D3DXCreateTextureFromFileEx boundary map file ", hr );
		return E_ABORT;
	}

	D3DLOCKED_RECT	lrect;
	D3DLOCKED_RECT	lrectdsta;
	D3DLOCKED_RECT	lrectdstb;
	D3DLOCKED_RECT	lrectdstc;
	D3DLOCKED_RECT	lrectdstd;

	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = m_pBoundaryMapTex->LockRect( 0, &lrect, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pBoundaryMapTex", hr );
	if( FAILED( hr = m_pNeighbourMaskTex[0]->LockRect( 0, &lrectdsta, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pNeighbourMaskTex[0]", hr );
	if( FAILED( hr = m_pNeighbourMaskTex[1]->LockRect( 0, &lrectdstb, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pNeighbourMaskTex[1]", hr );
	if( FAILED( hr = m_pResidueProbsTex[0]->LockRect( 0, &lrectdstc, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pResidueProbsTex[0]", hr );
	if( FAILED( hr = m_pResidueProbsTex[1]->LockRect( 0, &lrectdstd, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pResidueProbsTex[1]", hr );


	// source data
	unsigned char* pSrcData = (unsigned char*)(lrect.pBits);


	float *paopm = m_AOPM.GetAOPMDataPtr();
	unsigned char	neighbours[8];
	D3DCOLOR* pDesta = (D3DCOLOR*)(lrectdsta.pBits);
	D3DCOLOR* pDestb = (D3DCOLOR*)(lrectdstb.pBits);

	assert( sizeof(int) == sizeof(float) );
	void		*pDestc= (int*)(lrectdstc.pBits);
	void		*pDestd= (int*)(lrectdstd.pBits);

	for ( i=0; i< m_DataSize; i++ )
	{
		unsigned char texVal;
		texVal = _GetNeighboursRFromRGB( i, neighbours, m_DataWidth , m_DataHeight, pSrcData, m_BoundaryMapFormat );
		pDesta[i] = D3DCOLOR_RGBA( neighbours[0], neighbours[1], neighbours[2],neighbours[3] );
		pDestb[i] = D3DCOLOR_RGBA( neighbours[4], neighbours[5], neighbours[6],neighbours[7] );

		_CalcResidues( i, neighbours, NUM_ACTIONS, NUM_OUTCOMES, (D3DXVECTOR4*)paopm, pDestc, pDestd, m_ResidueProbsFormat );
	}

	m_pResidueProbsTex[0]->UnlockRect(0);
	m_pResidueProbsTex[1]->UnlockRect(0);
	m_pNeighbourMaskTex[0]->UnlockRect(0);
	m_pNeighbourMaskTex[1]->UnlockRect(0);
	m_pBoundaryMapTex->UnlockRect(0);

	_ValidateBoundaryMap();

	return S_OK;
}

HRESULT DPGPU::_ValidateBoundaryMap()
{
	HRESULT hr;
	// check that map has a border of at least 1 pixel surrounding it's edge.
	D3DLOCKED_RECT	lrect;

	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = m_pBoundaryMapTex->LockRect( 0, &lrect, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pBoundaryMapTex", hr );
	


	if ( m_BoundaryMapFormat != D3DFMT_X8R8G8B8 )
	{
		MessageBox(NULL, "_ValidateBoundaryMap, incorrect texture format", "gpu_dp.exe", MB_OK);
	}

	
	bool bSetBoundary= false;
	int stride =4;
	int i;
		// top:
	unsigned char* pSrcData = (unsigned char*)(lrect.pBits);
	for ( i=0; i<m_DataWidth; i++ )
	{
		if ( (*pSrcData) != 0 )
		{
			*pSrcData= 0;
			bSetBoundary= true;
		}
		pSrcData+= stride;
	}
		// sides.
	pSrcData=  (unsigned char*)(lrect.pBits);
	for ( i=0; i<m_DataHeight; i++)
	{
		if ( *pSrcData!= 0 )	// left
		{	*pSrcData= 0;
			bSetBoundary= true;
		}
		pSrcData+= stride*(m_DataWidth-1);
		if ( *pSrcData!= 0 )	// right
		{	*pSrcData= 0;
			bSetBoundary= true;
		}
		pSrcData+= stride;
	}
	
		// bottom
	pSrcData = (unsigned char*)(lrect.pBits);
	pSrcData+= stride*m_DataWidth*(m_DataHeight-1);
	for ( i=0; i<m_DataWidth; i++ )
	{
		if ( (*pSrcData) != 0 )
		{
			*pSrcData= 0;
			bSetBoundary= true;
		}
		pSrcData+= stride;
	}

	if ( bSetBoundary )
	{
		// warn
		MessageBox(NULL, "_ValidateBoundaryMap: Boundary map borders must be surrounded by cells of zero R value. Fixed. Press to continue", "gpu_dp.exe", MB_OK);
	}


	m_pBoundaryMapTex->UnlockRect(0);

	return S_OK;
}

HRESULT DPGPU::ValidateDataMap()
{
	// check that at least one cell (that doesnt lie on a boundary) has zero value (a sink)
	// if not then set the very first cell not on a boundary cell to be zero.

	bool	bFoundAZeroCell= false;
	int row, col;
	for ( row=0; row<m_DataHeight; row++ )
	{
		for ( col=0; col<m_DataWidth; col++ )
		{
			if ( IsBoundary( row, col ) )
				continue;
			if ( GetOrigValue(row,col) == 0.0f )
			{
				bFoundAZeroCell = true;
			}
		}
	}

	if ( bFoundAZeroCell )
		return S_OK;
	else
	{
		for ( row=0; row<m_DataHeight; row++ )
		{
			for ( col=0; col<m_DataWidth; col++ )
			{
				if ( IsBoundary( row, col ) )
					continue;
				if ( !FAILED( ModifyMap3x3( row, col, -FLT_MAX/2.f ) ) )
				{
					MessageBox(NULL, "ValidateDataMap: No Sink (zero value cell) found. Added one at first opportunity. Press to continue", "gpu_dp.exe", MB_OK);
					return S_OK;
				}
			}
		}
	}

	MessageBox(NULL, "ValidateDataMap: missing zero cells. couldnt correct. exitting ", "gpu_dp.exe", MB_OK);
	return E_ABORT;
}

// -----------------------------------------------------------------------------------


const float DPGPU::m_OrigValueScaleFactor= 1.0f/(1<<12); //scale down to fit into dynamic range of float
// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
DPGPU::DPGPU()
{
	int i;
	for ( i=0; i<NUMPIXELSHADERS; i++ )
	{
		m_pPixelShader[i]		= NULL;
	}

	m_pd3dDevice				= NULL;
	m_pDataTexture				= NULL;
	m_DataTextureFormat			= D3DFMT_R32F;

	m_pRenderTargTex[0]			= NULL;
	m_pRenderTargTex[1]			= NULL;
	m_pRenderTargSurf[0]		= NULL;
	m_pRenderTargSurf[1]		= NULL;
	m_pDataRetrieveTexture[0]	= NULL;
	m_pDataRetrieveTexture[1]	= NULL;
	m_pDataRetrieveSurface[0]	= NULL;
	m_pDataRetrieveSurface[1]	= NULL;
	m_pActionTex				= NULL;
	m_pActionSurf				= NULL;
	m_ActionTexFormat			= m_DataTextureFormat; //D3DFMT_A4R4G4B4;


	m_pVB						= NULL;

	m_pBoundaryMapTex			= NULL;
	m_BoundaryMapFormat			= D3DFMT_X8R8G8B8;
	m_pNeighbourMaskTex[0]		= NULL;
	m_pNeighbourMaskTex[1]		= NULL;
	m_NeighbourMaskFormat		= D3DFMT_A8R8G8B8;
//	m_NeighbourMaskFormat		= D3DFMT_A1R5G5B5;
	m_pResidueProbsTex[0]		= NULL;
	m_pResidueProbsTex[1]		= NULL;
	m_ResidueProbsFormat		= D3DFMT_A16B16G16R16; // could be A8b8g8r8 if probs were multiples of 1/256
//	m_ResidueProbsFormat		= D3DFMT_A32B32G32R32F; // could be A8b8g8r8 if probs were multiples of 1/256


	m_InitOnceParams.m_pBoundaryMapFileName= 0;
	m_InitOnceParams.m_pDataFileName= 0;
	m_InitOnceParams.m_pImageFileName = 0;

	m_pOrigImage				= NULL;
	m_pCurrImageTex[0]			= NULL;
	m_pCurrImageTex[1]			= NULL;
	m_pCurrImageSurf[0]			= NULL;
	m_pCurrImageSurf[1]			= NULL;
	m_ImageWidth				= 0;
	m_ImageHeight				= 0;
	m_ImageFormat				= D3DFMT_A8R8G8B8;
	m_ImageBuffId				= 0;

	m_bFirstPass				= 1;
}



int DPGPU::IterateVals(int numIterations, bool bRestartFromInitialData)
{
	static int dumpOnce =1;

	m_IVPSConsts.SetPSConsts(m_pd3dDevice);

	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	m_pd3dDevice->SetPixelShader( m_pPixelShader[ITERATE_VALS_PS] );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );

	if ( bRestartFromInitialData )
	{
		m_bFirstPass = 1;
		numIterations++;
	}

	// first pass uses data texture. future passes uses results.
	if ( m_bFirstPass )
	{
		m_pd3dDevice->SetRenderTarget( 0, m_pRenderTargSurf[0] );
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	0x00000000, 1.0f, 0L );

		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{
			m_pd3dDevice->SetTexture( 0, m_pDataTexture );
			m_pd3dDevice->SetTexture( 1, m_pNeighbourMaskTex[0] );
			m_pd3dDevice->SetTexture( 2, m_pNeighbourMaskTex[1] );
			m_pd3dDevice->SetTexture( 3, m_pResidueProbsTex[0] );
			m_pd3dDevice->SetTexture( 4, m_pResidueProbsTex[1] );
			m_pd3dDevice->SetTexture( 5, m_pDataTexture );
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
			m_pd3dDevice->EndScene();
		}
	}
	//if ( dumpOnce>0 )
	//{
	//	int BufferNum= 0;
	//	m_pd3dDevice->GetRenderTargetData( m_pRenderTargSurf[BufferNum], m_pDataRetrieveSurface[BufferNum] );
	//	(*g_poutfile) << "\n Iteration : " << 2;
	//	DumpValsToFile( *g_poutfile, m_pDataRetrieveSurface[BufferNum], m_DataWidth,
	//		m_DataHeight/*format defaults to r32f*/ );
	//}


	m_pd3dDevice->SetTexture( 1, m_pNeighbourMaskTex[0] ); //reset
	int i;
	for ( i=0; i< numIterations/2; i++ )	// round down to nearest num of pairs.
	{
		m_pd3dDevice->SetRenderTarget( 0, m_pRenderTargSurf[1] );
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{
			m_pd3dDevice->SetTexture( 0, m_pRenderTargTex[0] );
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
			m_pd3dDevice->EndScene();
		}
		

		m_pd3dDevice->SetRenderTarget( 0, m_pRenderTargSurf[0] );
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{
			m_pd3dDevice->SetTexture( 0, m_pRenderTargTex[1] );
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
			m_pd3dDevice->EndScene();
		}

		//if ( dumpOnce>0 )
		//{
		//	int BufferNum= 0;
		//	m_pd3dDevice->GetRenderTargetData( m_pRenderTargSurf[BufferNum], m_pDataRetrieveSurface[BufferNum] );
		//	(*g_poutfile) << "\n Iteration : " << ((i+1)*2) +1;
		//	DumpValsToFile( *g_poutfile, m_pDataRetrieveSurface[BufferNum], m_DataWidth,
		//										m_DataHeight/*format defaults to r32f*/ );
		//}

	}

		// copy results back into ram.
	int BufferNum= 0;
	m_pd3dDevice->GetRenderTargetData( m_pRenderTargSurf[BufferNum], m_pDataRetrieveSurface[BufferNum] );

	CreateActionMap();

	//if ( dumpOnce>0 )
	//{
	//	int BufferNum= 1;
	//	m_pd3dDevice->GetRenderTargetData( m_pRenderTargSurf[BufferNum], m_pDataRetrieveSurface[BufferNum] );
	//	(*g_poutfile) << "\n Action Map : ";
	//	DumpValsToFile( *g_poutfile, m_pDataRetrieveSurface[BufferNum], m_DataWidth,
	//		m_DataHeight/*format defaults to r32f*/ );

	//}


	if (dumpOnce>=0)
	{dumpOnce--;}

	if (m_bFirstPass)
	{--m_bFirstPass;}

	m_ImageBuffId^=1;
	return 0;
}




int DPGPU::CreateActionMap()
{
	m_CAMPSConsts.SetPSConsts(m_pd3dDevice);
	m_pd3dDevice->SetPixelShader( m_pPixelShader[CREATE_ACTION_MAP_PS] );
	m_pd3dDevice->SetRenderTarget( 0, m_pActionSurf );

	if (m_bFirstPass)
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		m_pd3dDevice->SetTexture( 0, m_pRenderTargTex[0] );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	}
	m_pd3dDevice->EndScene();


		// copy back into ram. No further calls to GetRenderTargetData to obtain actions are necessary for the frame.
	int BufferNum =1;
	m_pd3dDevice->GetRenderTargetData( m_pActionSurf, m_pDataRetrieveSurface[BufferNum] );

	// access to ensure data copied back to main ram for timing purposes.
	GetAction( m_DataHeight-1, m_DataWidth-1 );
	return 0;

}

int	DPGPU::GetAction( int row, int col )
{
	// return action code for cell at row,col.
	HRESULT	hr;
	// grab the texture to have a look.
	int BufferNum= 1;

	// read data from surface
	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = row+1;
	lockregion.left	  = col;
	lockregion.right  = col+1;
	lockregion.top    = row;

	if( FAILED( hr = m_pDataRetrieveSurface[BufferNum]->LockRect( &lrect, &lockregion, D3DLOCK_READONLY ) ) )
	{DXTRACE_ERR( "LockRect", hr );}

	int retval;
//	unsigned short us;
	float *psData;
	switch ( m_ActionTexFormat )
	{
	case D3DFMT_A4R4G4B4:
		exit(0);
		break;
	case D3DFMT_R32F:
	case D3DFMT_G32R32F:
		psData = (float*)(lrect.pBits);
		retval = (int)(*psData);
		break;


	}
	m_pDataRetrieveSurface[BufferNum]->UnlockRect();

	return retval;

}

float DPGPU::GetCurrValue( int row, int col )
{
	// return action code for cell at row,col.

	if ( (row<0)||(row>=m_DataHeight)||(col<0)||(col>=m_DataWidth) )
		return -1;
	HRESULT	hr;

	int BufferNum= 0; // copy back into ram is done once a frame

	// read data from surface
	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = row+1;
	lockregion.left	  = col;
	lockregion.right  = col+1;
	lockregion.top    = row;

	if( FAILED( hr = m_pDataRetrieveSurface[BufferNum]->LockRect( &lrect, &lockregion, D3DLOCK_READONLY ) ) )
	{DXTRACE_ERR( "LockRect", hr );}

	int stride;
	if ( D3DFMT_G32R32F == m_DataTextureFormat )
		stride = 2;
	else if ( D3DFMT_R32F == m_DataTextureFormat )
		stride =1;

	float *psData = (float*)(lrect.pBits);

	m_pDataRetrieveSurface[BufferNum]->UnlockRect();

	return (*psData);

}

HRESULT DPGPU::OneTimeSceneInit(DPParams* p)
{
	DPGPU_Params* pd = (DPGPU_Params*)p;
	m_InitOnceParams = *pd;
	return S_OK;
}


HRESULT DPGPU::InitDeviceObjects(DPParams* p)
{
	HRESULT hr;

	m_pd3dDevice = ((DPGPU_Params*)p)->m_pd3dDevice;


	// load data texture. datawidth and dataheight set by incoming texture.
	if ( FAILED(hr= CreateNInit_DataTexture() ) )
		return DXTRACE_ERR( "couldnt CreateNInit_DataTexture", hr );


	
	m_IVPSConsts.Init(m_DataWidth,m_DataHeight);
	m_AOPM.Init( (float*)&m_IVPSConsts.m_PSConst[0] , NUM_ACTIONS, NUM_OUTCOMES  );
	m_CAMPSConsts.Init(m_DataWidth,m_DataHeight);
	m_WIPSConsts.Init(m_DataWidth,m_DataHeight);



	// Create the vertex buffer
	if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 6*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &m_pVB, NULL ) ) )
		return DXTRACE_ERR( "CreateVertexBuffer", hr );

	// Fill the vertex buffer with 2 triangles (to make quad)
	CUSTOMVERTEX* pVertices;

	if( FAILED( hr = m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
		return DXTRACE_ERR( "Lock", hr );

	float		tscalex = 1.0f;//16.0f; // scale poly up to show texture better.
	float		tscaley = 1.0f;//16.0f; // scale poly up to show texture better.
	float		width = m_DataWidth * tscalex;
	float		height = m_DataHeight * tscaley;


	pVertices[0].position = D3DXVECTOR4( 0.0f,  0.0f, 0.0f, 1.0f );
	pVertices[0].texcoords0= pVertices[0].texcoords1 = D3DXVECTOR2( 0.0f,  0.0f );
	pVertices[1].position = D3DXVECTOR4(  (float)width, 0, 0.0f, 1.0f );
	pVertices[1].texcoords0 = pVertices[1].texcoords1= D3DXVECTOR2( 1.0f,  0.0f );
	pVertices[2].position = D3DXVECTOR4(  0.0f, (float)height, 0.0f, 1.0f );
	pVertices[2].texcoords0 = pVertices[2].texcoords1= D3DXVECTOR2( 0.0f,  1.0f );

	pVertices[3].position = D3DXVECTOR4(  (float)width, 0, 0.0f, 1.0f );
	pVertices[3].texcoords0 = pVertices[3].texcoords1 = D3DXVECTOR2( 1.0f,  0.0f );
	pVertices[4].position = D3DXVECTOR4(  0.0f, (float)height, 0.0f, 1.0f );
	pVertices[4].texcoords0 = pVertices[4].texcoords1 = D3DXVECTOR2( 0.0f,  1.0f );
	pVertices[5].position = D3DXVECTOR4(  (float)width, (float)height, 0.0f, 1.0f );
	pVertices[5].texcoords0 = pVertices[5].texcoords1 = D3DXVECTOR2( 1.0f,  1.0f );


	m_pVB->Unlock();


	if ( FAILED(hr = CreateNInit_NeighbourhoodData()) )
		return DXTRACE_ERR( "couldnt CreateNInit_NeighbourhoodData", hr );

	if ( FAILED(hr = ApplyBoundaryMaskToDataTexture() ) )
		return DXTRACE_ERR( "couldnt ApplyBoundaryMaskToDataTexture", hr );

	if ( FAILED(hr =  ValidateDataMap() ) )
		return DXTRACE_ERR( "couldnt ValidateDataMap", hr );

	// initialise 2 surfaces in system mem to retrieve results
	// nb need to create as textures since R32F format not supported in createoffscreenplain surface.
	int i;
	for ( i= 0; i<2 ; i++ )
	{
		if ( FAILED( hr = m_pd3dDevice->CreateTexture( m_DataWidth, m_DataHeight, 1,
			D3DUSAGE_DYNAMIC, m_DataTextureFormat,
			D3DPOOL_SYSTEMMEM, &m_pDataRetrieveTexture[i], NULL ) ) )
			return DXTRACE_ERR( "couldnt create m_pDataRetrieveTexture", hr );

		if ( FAILED( hr = m_pDataRetrieveTexture[i]->GetSurfaceLevel(0, &m_pDataRetrieveSurface[i]) ) )
			return DXTRACE_ERR( "couldnt create m_pDataRetrieveSurface", hr );
	}


	hr = LoadShaderFile(m_pd3dDevice, "dp_iterate20_v10.psh", &m_pPixelShader[ITERATE_VALS_PS]);
	if ( FAILED(hr) )
		return DXTRACE_ERR( "cannot load pixel shader", hr );

	hr = LoadShaderFile(m_pd3dDevice, "ActionMapShader.psh", &m_pPixelShader[CREATE_ACTION_MAP_PS]);
	if ( FAILED(hr) )
		return DXTRACE_ERR( "cannot load pixel shader", hr );

	return S_OK;
}

HRESULT DPGPU::RestoreDeviceObjects(DPParams* p)
{
	HRESULT retres= S_OK;
	HRESULT hr;

	int i;
	for ( i= 0; i<2 ; i++ )
	{
		if ( !FAILED( hr = m_pd3dDevice->CreateTexture( m_DataWidth, m_DataHeight, 1,
			D3DUSAGE_RENDERTARGET, m_DataTextureFormat,
			D3DPOOL_DEFAULT, &m_pRenderTargTex[i], NULL ) ) )
		{
			if ( FAILED( hr = m_pRenderTargTex[i]->GetSurfaceLevel(0, &m_pRenderTargSurf[i]) ) )
			{retres = S_FALSE;}
		}
		else
            {retres = S_FALSE;}

	}
	if ( !FAILED( hr = m_pd3dDevice->CreateTexture( m_DataWidth, m_DataHeight, 1,
		D3DUSAGE_RENDERTARGET, m_ActionTexFormat,
		D3DPOOL_DEFAULT, &m_pActionTex , NULL ) ) )
	{
		if ( FAILED( hr = m_pActionTex->GetSurfaceLevel(0, &m_pActionSurf ) ) )
		{retres = S_FALSE;}
	}
	else
	{retres = S_FALSE;}



	hr = m_pd3dDevice->CreateTexture( m_DataWidth, m_DataHeight, 1,
		0, m_DataTextureFormat,	D3DPOOL_DEFAULT, &m_pDisplayTex, NULL );
	if( FAILED(hr) )
		return hr;

	return retres;
}

HRESULT DPGPU::InvalidateDeviceObjects()
{
	for ( int i=0; i<2; i++ )
	{
		SAFE_RELEASE( m_pRenderTargTex[i] );
		SAFE_RELEASE( m_pRenderTargSurf[i] );
		SAFE_RELEASE( m_pCurrImageTex[i] );
		SAFE_RELEASE( m_pCurrImageSurf[i] );
	}
	SAFE_RELEASE( m_pActionTex );
	SAFE_RELEASE( m_pActionSurf );
	SAFE_RELEASE( m_pDisplayTex );
	return S_OK;
}

HRESULT DPGPU::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_pVB );
	Release_DataTexture();
	Release_NeighbourhoodData();
	SAFE_RELEASE( m_pDataRetrieveTexture[0] );
	SAFE_RELEASE( m_pDataRetrieveTexture[1] );
	SAFE_RELEASE( m_pDataRetrieveSurface[0] );
	SAFE_RELEASE( m_pDataRetrieveSurface[1] );

	int i;
	for ( i=0; i<NUMPIXELSHADERS; i++ )
	{	SAFE_RELEASE( m_pPixelShader[i] );		}

	SAFE_RELEASE( m_pOrigImage );

	return S_OK;
}

HRESULT DPGPU::FinalCleanup()
{
	return S_OK;

}

//-----------------------------------------------------------------------------
HRESULT DPGPU::Release_DataTexture()
{
	SAFE_RELEASE( m_pDataTexture );
	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT DPGPU::Release_NeighbourhoodData()
{
	SAFE_RELEASE( m_pBoundaryMapTex );
	SAFE_RELEASE( m_pNeighbourMaskTex[0] );
	SAFE_RELEASE( m_pNeighbourMaskTex[1] );
	SAFE_RELEASE( m_pResidueProbsTex[0] );
	SAFE_RELEASE( m_pResidueProbsTex[1] );
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT DPGPU::FillFArrayWithBoundaryMapData( unsigned char** retArray, int dataSizeIn )
{
	assert( m_DataSize == dataSizeIn );

	HRESULT hr;
	D3DLOCKED_RECT	lrect;
//	D3DLOCKED_RECT	lrect2;
	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = m_pBoundaryMapTex->LockRect( 0, &lrect, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pBoundaryMapTex", hr );
	// expect boundary map to be 24bit unsigned D3DFMT_R8G8B8. be wary of alignment probs.

	unsigned char* allocDat = new unsigned char[dataSizeIn];
	
	// source data
	unsigned char* pSrcData = (unsigned char*)(lrect.pBits);
	static const int  tstride= 4;
	for ( int i=0; i< m_DataSize; i++ )
	{
		int		currTexel = i*tstride; //xrgb format
		allocDat[i] = pSrcData[currTexel];
	}
	*retArray = allocDat;

	m_pBoundaryMapTex->UnlockRect(0);

	return S_OK;
}

bool DPGPU::IsBoundary(int row, int col)
{
	// return action code for cell at row,col.
	HRESULT	hr;
	bool retval= false;
	// grab the texture to have a look.

	// read data from surface
	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = row+1;
	lockregion.left	  = col;
	lockregion.right  = col+1;
	lockregion.top    = row;

	if( FAILED( hr = m_pBoundaryMapTex->LockRect( 0, &lrect, &lockregion, 0 ) ) )
	{
		DXTRACE_ERR( "LockRect m_pBoundaryMapTex", hr );
		return false;
	}
	// expect boundary map to be 24bit unsigned D3DFMT_R8G8B8. be wary of alignment probs.

	// source data
	unsigned char* pSrcData = (unsigned char*)(lrect.pBits);
	if (*pSrcData==0)
		 retval = true;

	m_pBoundaryMapTex->UnlockRect(0);
	return retval;

}

float DPGPU::GetOrigValue( int row, int col )
{
	HRESULT hr;
	if ( (row<0)||(row>=m_DataHeight)||(col<0)||(col>=m_DataWidth) )
		return -1;

	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = row+1;
	lockregion.left	  = col;
	lockregion.right  = col+1;
	lockregion.top    = row;

	if( FAILED( hr = m_pDataTexture->LockRect( 0, &lrect, &lockregion, 0 ) ) )
	{
		DXTRACE_ERR( "LockRect", hr );
		return -1;
	}
	float *psData = (float*)(lrect.pBits);
	m_pDataTexture->UnlockRect(0);
	return *psData;
}

HRESULT DPGPU::ModifyMap3x3( int row, int col, float modVal )
{

	if ( (row-1<0)||(row+1>=m_DataHeight)||(col-1<0)||(col+1>=m_DataWidth) )
		return -1;

	HRESULT hr;
	D3DLOCKED_RECT	lrect;
	D3DLOCKED_RECT	lrect2;
	RECT		lockregion;
	lockregion.bottom = row+2;
	lockregion.left	  = col-1;
	lockregion.right  = col+2;
	lockregion.top    = row-1;


	if( FAILED( hr = m_pDataTexture->LockRect( 0, &lrect, &lockregion, 0 ) ) )
	{
		DXTRACE_ERR( "LockRect", hr );
		return E_FAIL;
	}
	if( FAILED( hr = m_pBoundaryMapTex->LockRect( 0, &lrect2, &lockregion, 0 ) ) )
	{
		DXTRACE_ERR( "LockRect", hr );
		m_pDataTexture->UnlockRect(0);
		return E_FAIL;
	}

	// around row,col. add the modval to the elems (using scaled gaussian filter weights)
	float cornerVal = modVal/4.0f;
	float edgeVal = modVal/2.0f;
	int		valStride;
	int		boundaryMapStride = 4;

	if ( m_DataTextureFormat == D3DFMT_R32F )
		valStride = 1;
	else if ( m_DataTextureFormat == D3DFMT_G32R32F )
		valStride = 2;
	else { assert(0); }

	float*	pval = (float*)(lrect.pBits);
	unsigned char*	pbound = (unsigned char*)(lrect2.pBits);
	
//	int r,c;
	// top row
	_ModifyVal( *pbound, pval, cornerVal );
	pbound += boundaryMapStride;
	pval += valStride;

	_ModifyVal( *pbound, pval, edgeVal );
	pbound += boundaryMapStride;
	pval += valStride;

	_ModifyVal( *pbound, pval, cornerVal );
	pbound += boundaryMapStride;
	pval += valStride;

		// mid row
	pbound += boundaryMapStride*(m_DataWidth-3);
	pval += valStride*(m_DataWidth-3);
	_ModifyVal( *pbound, pval, edgeVal );
	pbound += boundaryMapStride;
	pval += valStride;

	_ModifyVal( *pbound, pval, modVal );
	pbound += boundaryMapStride;
	pval += valStride;

	_ModifyVal( *pbound, pval, edgeVal );
	pbound += boundaryMapStride;
	pval += valStride;


	// bottom row
	pbound += boundaryMapStride*(m_DataWidth-3);
	pval += valStride*(m_DataWidth-3);
	_ModifyVal( *pbound, pval, cornerVal );
	pbound += boundaryMapStride;
	pval += valStride;

	_ModifyVal( *pbound, pval, edgeVal );
	pbound += boundaryMapStride;
	pval += valStride;

	_ModifyVal( *pbound, pval, cornerVal );
	pbound += boundaryMapStride;
	pval += valStride;


	m_pBoundaryMapTex->UnlockRect(0);
	m_pDataTexture->UnlockRect(0);

	return S_OK;
}

inline void DPGPU::_ModifyVal( const unsigned char boundVal, float *pVal, float modVal )
{
	if ( boundVal != 0 )
	{
		if ( *pVal > 0.0f )	// cannot modify goal cells (=0 in value) 
		{
			*pVal += modVal;
			if (*pVal <0.0f)
				*pVal = 0.0f;
		}
	}
}


HRESULT DPGPU::IterateValsPSConsts::Init( int dataWidth, int dataHeight )
{
static const float root2 = 1.414213562373f;
static const float psFLT_EPSILON=     FLT_EPSILON;// 1.192093e-6f;
static const float psFLT_MAX=			FLT_MAX;// 3.4028e37f;

	if ( ( dataWidth <= 0 ) || ( dataHeight <= 0 ) )
	{
		MessageBox(NULL, "IterateValsPSConsts::init(), ( dataWidth <= 0 ) || ( dataHeight <= 0 ) ", "gpu_dp.exe", MB_OK);
		return E_FAIL;
	}



	float f[4*7] = { root2, 1.0f, root2, 1.0f, 
					1.0f, root2, 1.0f, root2,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f/dataWidth, 0, 1.0f/dataHeight, 0,
					psFLT_MAX, psFLT_MAX, psFLT_MAX, psFLT_MAX,
					psFLT_EPSILON, psFLT_EPSILON, psFLT_EPSILON, psFLT_EPSILON,
					0,0,0,0,
	};

	memcpy( &m_PSConst[16*4], f, 4*7*sizeof(float) );

	//def c16, root2, 1.0f, root2, 1.0f
	//def c17, 1.0f, root2, 1.0f, root2
	//def c18, 1.0f, 1.0f, 1.0f, 1.0f
	//D3DXVECTOR4		wh= D3DXVECTOR4( 1.0f/texWidth, 0, 1.0f/texHeight, 0 );
	//// pd3dDevice->SetPixelShaderConstantF(  19, (float*)wh, 1 );
	//def c20, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
	//def c21, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON
	//def c22, 0,0,0,0
	
	return S_OK;
}

HRESULT DPGPU::IterateValsPSConsts::SetPSConsts( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return pd3dDevice->SetPixelShaderConstantF(0,m_PSConst, 23);
}

HRESULT DPGPU::CreateActionMapPSConsts::Init( int dataWidth, int dataHeight )
{
	// constants common to IterateValsPSConsts are not init'd. as action shader
	// is set to directly follow iterateval shader.

	float f[2*4] = { 0,1.f,2.f,3.f,4.f,5.f,6.f,7.f, };
	memcpy( &m_PSConst[23], f, 2*4*sizeof(float) );
//		def c23, 0,1,2,3		// actionid = id * 20 ; simply for display purposes
//		def c24, 4,5,6,7

	return S_OK;
}

HRESULT DPGPU::CreateActionMapPSConsts::SetPSConsts( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return pd3dDevice->SetPixelShaderConstantF(23,&m_PSConst[23],2);
}

HRESULT DPGPU::WarpImagePSConsts::Init( int dataWidth, int dataHeight )
{
	HRESULT hr = S_OK;
	float width,height;
	float constants0To7[4*8];

	if( dataWidth <= 0 || dataHeight <= 0)
		return E_FAIL;

	width = 1.f/(float)dataWidth;
	height = 1.f/(float)dataHeight;
	memset(constants0To7,0,sizeof(constants0To7));

	// TOP ROW //
	constants0To7[0] = -width;         //c10.x// coord for up left
	constants0To7[1] = -height;        //c10.y// coord for up left
	constants0To7[4] = 0.0;            //c11.x// coord for up
	constants0To7[5] = -height;        //c11.y// coord for up
	constants0To7[8] = width;          //c12.x// coord for up right 
	constants0To7[9] = -height;        //c12.y// coord for up right 

	// CENTER ROW //
	constants0To7[12] = -width;        //c13.x// coord for left 
	constants0To7[13] = 0.0;           //c13.y// coord for left 
	constants0To7[16] = width;          //c14.x// coord for right 
	constants0To7[17] = 0.0;            //c14.y// coord for right 


	// BOTTOM ROW //
	constants0To7[20] = -width;         //c15.x// coord for down left
	constants0To7[21] = height;         //c15.y// coord for down left
	constants0To7[24] = 0.0;            //c16.y// coord for down 
	constants0To7[25] = height;         //c16.x// coord for down 
	constants0To7[28] = width;           //c17.x// coord for down right 
	constants0To7[29] = height;          //c17.y// coord for down right 

	memcpy(m_PSConst,constants0To7, 4*8*sizeof(float) );
	return hr;


}
HRESULT DPGPU::WarpImagePSConsts::SetPSConsts( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}

