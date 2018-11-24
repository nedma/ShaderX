#include <assert.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"

#include "dpSoft.h"
#include "dpGPU.h"
#include "dpSoftHelper.h"
#include <dxerr9.h>


// --------------------------------------------------------------------------

DPSoft::DPSoft(): m_pValueMap(0), m_pNeighbourhoodMask(0), m_pActionMap(0),
m_pBoundaryMap(0)
{
	m_CurrValBuffID			= 0;
	m_NextValBuffID			= 1;
	m_pCurrValueMap[0]		= NULL;
	m_pCurrValueMap[1]		= NULL;
}
// --------------------------------------------------------------------------


HRESULT DPSoft::InitValueMap(DPParams* p)
{
	HRESULT hr;
	DPGPU* pd= ((DPSoft_Params*)p)->m_pDPGPU;

	if ( (pd->m_pDataTexture == NULL) )
		return E_FAIL;

	m_pCurrValueMap[0] = new float[m_DataSize];
	m_pCurrValueMap[1] = new float[m_DataSize];
	m_pValueMap = new float[m_DataSize];


	// get data from textures
	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = pd->m_pDataTexture->LockRect( 0, &lrect, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect", hr );

	if ( FAILED( hr = pd->FillFArrayWithBoundaryMapData( &m_pBoundaryMap, m_DataSize ) ) )
		return DXTRACE_ERR( "FillFArrayWithBoundaryMapData", hr );

	float *psData = (float*)(lrect.pBits);
	float *pdat=0;
	for ( int i=0; i< m_DataSize; i++ )
	{
		if ( D3DFMT_G32R32F == pd->m_DataTextureFormat )
			pdat = &psData[2*i];
		else if ( D3DFMT_R32F == pd->m_DataTextureFormat )
			pdat = &psData[i];
		m_pValueMap[i] = m_pCurrValueMap[0][i] =  m_pCurrValueMap[1][i]= *pdat;
	}

	pd->m_pDataTexture->UnlockRect(0);

	return S_OK;
}
// --------------------------------------------------------------------------

inline unsigned char GetUCharFromD3DCOLOR(int iChar, D3DCOLOR col)
{
	// note that d3color has 8bit components. Anything with val>255 will be 
	// truncated (bitwise!). 
	// packed as argb ie 3,0,1,2 where 0 is the first component of the mask.

	int shift;
	if ( 3==iChar )
		shift= 3*8;
	else
		shift = 8*(2-iChar);
	return (unsigned char)( (col>>shift)&0xf );
}
// --------------------------------------------------------------------------

HRESULT DPSoft::InitNeighbourhoodMask(DPParams* p)
{
	HRESULT hr;
	m_pNeighbourhoodMask  = new NeighbourMask[m_DataSize];

	DPGPU* pd= ((DPSoft_Params*)p)->m_pDPGPU;

	if ( (pd->m_pNeighbourMaskTex[0]==NULL) || (pd->m_pNeighbourMaskTex[0]==NULL) )
		return E_FAIL;

	D3DLOCKED_RECT	lrectdsta;
	D3DLOCKED_RECT	lrectdstb;

	RECT		lockregion;
	lockregion.bottom = m_DataHeight;
	lockregion.left	  = 0;
	lockregion.right  = m_DataWidth;
	lockregion.top    = 0;

	if( FAILED( hr = pd->m_pNeighbourMaskTex[0]->LockRect( 0, &lrectdsta, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pNeighbourMaskTex[0]", hr );
	if( FAILED( hr = pd->m_pNeighbourMaskTex[1]->LockRect( 0, &lrectdstb, &lockregion, 0 ) ) )
		return DXTRACE_ERR( "LockRect m_pNeighbourMaskTex[1]", hr );

	// dest data
	D3DCOLOR* pDesta = (D3DCOLOR*)(lrectdsta.pBits);
	D3DCOLOR* pDestb = (D3DCOLOR*)(lrectdstb.pBits);

//	unsigned char	neighbours[8];
	int i;
	for ( i=0; i< m_DataSize; i++ )
	{
		// unpack vals from d3dcolor:
		
		m_pNeighbourhoodMask[i].m_mask[0]= GetUCharFromD3DCOLOR(0,pDesta[i]);
		m_pNeighbourhoodMask[i].m_mask[1]= GetUCharFromD3DCOLOR(1,pDesta[i]);
		m_pNeighbourhoodMask[i].m_mask[2]= GetUCharFromD3DCOLOR(2,pDesta[i]);
		m_pNeighbourhoodMask[i].m_mask[3]= GetUCharFromD3DCOLOR(3,pDesta[i]);
		m_pNeighbourhoodMask[i].m_mask[4]= GetUCharFromD3DCOLOR(0,pDestb[i]);
		m_pNeighbourhoodMask[i].m_mask[5]= GetUCharFromD3DCOLOR(1,pDestb[i]);
		m_pNeighbourhoodMask[i].m_mask[6]= GetUCharFromD3DCOLOR(2,pDestb[i]);
		m_pNeighbourhoodMask[i].m_mask[7]= GetUCharFromD3DCOLOR(3,pDestb[i]);
	}

	pd->m_pNeighbourMaskTex[0]->UnlockRect(0);
	pd->m_pNeighbourMaskTex[1]->UnlockRect(0);

	return S_OK;

}
// --------------------------------------------------------------------------

HRESULT DPSoft::OneTimeSceneInit(DPParams* p)
{
	// from the given DPGPU, initialise the ActionOutcomeProbability map.
//	DPGPU* pd= ((DPSoft_Params*)p)->m_pDPGPU;

	m_AOPM.Init( m_AOPMData, NUM_ACTIONS, NUM_OUTCOMES ); // this is duplicated data if a dpgpu instance exists. better than dangling ptrs.

	return S_OK;
}
// --------------------------------------------------------------------------

HRESULT DPSoft::InitDeviceObjects(DPParams* p)
{
//	HRESULT hr;
	HRESULT retres= S_OK;
	DPGPU* pd= ((DPSoft_Params*)p)->m_pDPGPU;

	m_DataWidth = pd->GetDataWidth();
	m_DataHeight = pd->GetDataHeight();
	m_DataSize  = m_DataWidth* m_DataHeight;


	if (FAILED(InitValueMap(p)))
		retres = S_FALSE;

	if (FAILED(InitNeighbourhoodMask(p)))
		retres = S_FALSE;

	m_pActionMap= new 	unsigned char[m_DataSize];

	return retres;
}
// --------------------------------------------------------------------------

HRESULT DPSoft::RestoreDeviceObjects(DPParams* p)
{
	// do nothing
	return S_OK;
}
// --------------------------------------------------------------------------

HRESULT DPSoft::InvalidateDeviceObjects()
{
	// do nothing
	return S_OK;
}
// --------------------------------------------------------------------------

HRESULT DPSoft::DeleteDeviceObjects()
{
	SAFE_DELETE_ARRAY( m_pCurrValueMap[0] );
	SAFE_DELETE_ARRAY( m_pCurrValueMap[1] );
	SAFE_DELETE_ARRAY( m_pActionMap );
	SAFE_DELETE_ARRAY( m_pValueMap );
	SAFE_DELETE_ARRAY( m_pNeighbourhoodMask );
	SAFE_DELETE_ARRAY( m_pBoundaryMap );
	return S_OK;
}
// --------------------------------------------------------------------------

HRESULT DPSoft::FinalCleanup()
{
	assert(m_pValueMap==0);
	assert(m_pNeighbourhoodMask==0);
	assert(m_pActionMap==0);
	assert(m_pBoundaryMap==0);

	return S_OK;
}
// --------------------------------------------------------------------------

inline float DPSoft::GetOrigValue( int row, int col )
{
	assert(row<m_DataHeight);
	assert(col<m_DataWidth);
	assert(row>=0);
	assert(col>=0);
	return m_pValueMap[row*m_DataWidth+col];
}
// --------------------------------------------------------------------------

float DPSoft::GetCurrValue( int row, int col )
{
	assert(row<m_DataHeight);
	assert(col<m_DataWidth);
	assert(row>=0);
	assert(col>=0);
	return m_pCurrValueMap[m_CurrValBuffID][row*m_DataWidth+col];
}
// --------------------------------------------------------------------------

inline void DPSoft::SetNextCurrValue( int row, int col, float val )
{
	assert(row<m_DataHeight);
	assert(col<m_DataWidth);
	assert(row>=0);
	assert(col>=0);
	m_pCurrValueMap[m_NextValBuffID][row*m_DataWidth+col]= val;
}
// --------------------------------------------------------------------------

inline void DPSoft::SetAction( int row, int col, unsigned char val )
{
	m_pActionMap[row*m_DataWidth+col] = val;
}
// --------------------------------------------------------------------------

inline DPSoft::NeighbourMask* DPSoft::GetNeighbourMask( int row, int col )
{
	assert(row<m_DataHeight);
	assert(col<m_DataWidth);
	assert(row>=0);
	assert(col>=0);

	return &m_pNeighbourhoodMask[row*m_DataWidth+col];
}
// --------------------------------------------------------------------------

// calc the cost of all the actions at cell at (row,col)
// out: actioncosts

bool DPSoft::IsBoundary(int row, int col)
{
	if ( (row<0) || (row>=m_DataHeight) || (col<0) || (col>=m_DataWidth) )
		return true;

	return (m_pBoundaryMap[row*m_DataWidth+col] <=0);
}
// --------------------------------------------------------------------------

inline float DPSoft::MinActionCost( float actionCost[] )
{
	// return min of costs.
	float minf= actionCost[0];
	int i;
	for (i=1; i<NUM_ACTIONS; i++)
	{
		minf = min( actionCost[i], minf );
	}
	return minf;
}

// --------------------------------------------------------------------------

void DPSoft::ComputeActionCosts( int row, int col, float actionCost[] )
{
	float neighbourVals[NUM_ACTIONS];

	GetNeighbourVals( row, col, neighbourVals );

	if ( IsBoundary(row-1,col-1) )
		actionCost[0]= FLT_MAX;
	else
		actionCost[0] = ComputeCostOfAction( row, col, neighbourVals, 0 );

	if ( IsBoundary(row-1,col) )
		actionCost[1]= FLT_MAX;
	else
		actionCost[1] = ComputeCostOfAction( row, col, neighbourVals, 1 );

	if ( IsBoundary(row-1,col+1) )
		actionCost[2]= FLT_MAX;
	else
		actionCost[2] = ComputeCostOfAction( row, col, neighbourVals, 2 );

	if ( IsBoundary(row,col-1) )
		actionCost[3]= FLT_MAX;
	else
		actionCost[3] = ComputeCostOfAction( row, col, neighbourVals, 3 );

	if ( IsBoundary(row,col+1) )
		actionCost[4]= FLT_MAX;
	else
		actionCost[4] = ComputeCostOfAction( row, col, neighbourVals, 4 );

	if ( IsBoundary(row+1,col-1) )
		actionCost[5]= FLT_MAX;
	else
		actionCost[5] = ComputeCostOfAction( row, col, neighbourVals, 5 );

	if ( IsBoundary(row+1,col) )
		actionCost[6]= FLT_MAX;
	else
		actionCost[6] = ComputeCostOfAction( row, col, neighbourVals, 6 );

	if ( IsBoundary(row+1,col+1) )
		actionCost[7]= FLT_MAX;
	else
		actionCost[7] = ComputeCostOfAction( row, col, neighbourVals, 7 );

}
// --------------------------------------------------------------------------

void DPSoft::GetNeighbourVals( int row, int col, float outArray[] )
{
	// diagonal neighbours are scaled by a geometrical factor.

	if (! IsBoundary(row-1,col-1) )
		outArray[0] = GetCurrValue( row-1, col-1 )* sqrt(2.0f);

	if ( !IsBoundary(row-1,col) )
		outArray[1] = GetCurrValue( row-1, col );

	if ( !IsBoundary(row-1,col+1) )
		outArray[2] = GetCurrValue( row-1, col+1 )* sqrt(2.0f);

	if ( !IsBoundary(row,col-1) )
		outArray[3] = GetCurrValue( row, col-1 );

	if ( !IsBoundary(row,col+1) )
		outArray[4] = GetCurrValue( row, col+1 );

	if ( !IsBoundary(row+1,col-1) )
		outArray[5] = GetCurrValue( row+1, col-1 )* sqrt(2.0f);

	if ( !IsBoundary(row+1,col) )
		outArray[6] = GetCurrValue( row+1, col );

	if ( !IsBoundary(row+1,col+1) )
		outArray[7] = GetCurrValue( row+1, col+1 )* sqrt(2.0f);
}
// --------------------------------------------------------------------------

float DPSoft::ComputeCostOfAction( int row, int col, float cachedNeighbourVals[], int actionID )
{
	// assumes that cell is not a boundary cell.
	int i;
	float accum=0;
	float* pActionProbs = m_AOPM.GetAOPMDataPtr();// &m_AOPM[actionID][0];
	pActionProbs += NUM_OUTCOMES*actionID;
	float accumProbs= 0;
	NeighbourMask* pnm = GetNeighbourMask(row,col);
	for ( i=0; i< NUM_ACTIONS; i++ )
	{
		if ( pnm->m_mask[i]!=0 )
		{
			accum += cachedNeighbourVals[i]*pActionProbs[i];
			accumProbs+= pActionProbs[i];
		}
	}

	accum += (1.0f- accumProbs)*GetCurrValue(row,col);
	return accum;
}
// --------------------------------------------------------------------------

int DPSoft::IterateVals(int numIterations, bool bRestartFromInitialData)
{
	assert( m_pValueMap );
	assert( m_pNeighbourhoodMask );
	assert( m_pActionMap );


	if ( bRestartFromInitialData )
		numIterations++;	// to make it consistent with gpu version

	float actionCost[NUM_ACTIONS];
	int ni;
	for ( ni=0; ni<numIterations; ni++ )
	{
		for (int row = 0; row < m_DataHeight; row++)
		{
			for (int col = 0; col < m_DataWidth; col++)
			{
				if ( IsBoundary(row,col) )
					continue;

				ComputeActionCosts( row, col, actionCost );

				float minCost = MinActionCost( actionCost );
				
				//	Update value function
				SetNextCurrValue( row, col, minCost + GetOrigValue(row,col) );
			}
		}
		m_NextValBuffID = m_CurrValBuffID;
		m_CurrValBuffID = m_CurrValBuffID^1;
	}

	return 0;
}
// --------------------------------------------------------------------------
inline unsigned char DPSoft::GetMinCostActionID( float actionCost[] )
{
	// return the ID of the action with the smallest action cost.
	// return min of costs.
	float minf= actionCost[0];
	int id=0;
	int i;
	for (i=1; i<NUM_ACTIONS; i++)
	{
		if ( actionCost[i] < minf )
		{
			minf = actionCost[i];
			id = i;
		}
	}
	return (unsigned char)id;

}



int DPSoft::CreateActionMap()
{
	float actionCost[NUM_ACTIONS];

	for (int row = 0; row < m_DataWidth; row++)
	{
		for (int col = 0; col < m_DataHeight; col++)
		{
			if ( IsBoundary(row,col) )
				continue;

			int i;
			for ( i=0; i< NUM_ACTIONS; i++)
			{actionCost[i]= FLT_MAX;}
			
			GetNeighbourVals( row, col, actionCost );

//			float minCost = MinActionCost( actionCost );
			unsigned char actionID = GetMinCostActionID( actionCost );
			SetAction( row, col, actionID );
			
		}
	}
	m_NextValBuffID = m_CurrValBuffID;
	m_CurrValBuffID = m_CurrValBuffID^1;

	return 0;
}
// --------------------------------------------------------------------------

int	DPSoft::GetAction( int row, int col )
{
	return (int)(m_pActionMap[row*m_DataWidth+col]);
}
// --------------------------------------------------------------------------
