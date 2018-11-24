#if 1
//#include <Windows.h>
#include "dropper.h"
#include "dxutil.h"
#include "dpGPU.h"
#include "worldtoscreen.h"
#include <assert.h>

// Helper function to stuff a FLOAT into a DWORD argument
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


struct POINTVERTEX
{
	D3DXVECTOR3 v;
	D3DCOLOR    color;

	static const DWORD FVF;
};
const DWORD POINTVERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;




float CParticleSystem::m_fPSysTime = 0.0f;
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem( DWORD dwFlush, DWORD dwDiscard )
{

	m_dwBase         = dwDiscard;
	m_dwFlush        = dwFlush;
	m_dwDiscard      = dwDiscard;

	m_dwParticles    = 0;
	m_dwParticlesLim = 2048;

	m_pParticles     = NULL;
	m_pParticlesFree = NULL;
	m_pVB            = NULL;
	m_fPSysTime		 = 0;

	m_pWST				= NULL;
	m_pDPGPU			= NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
	InvalidateDeviceObjects();

	while( m_pParticles )
	{
		PARTICLE* pSpark = m_pParticles;
		m_pParticles = pSpark->m_pNext;
		delete pSpark;
	}

	while( m_pParticlesFree )
	{
		PARTICLE *pSpark = m_pParticlesFree;
		m_pParticlesFree = pSpark->m_pNext;
		delete pSpark;
	}
}

//-----------------------------------------------------------------------------

HRESULT CParticleSystem::InstanceNewParticle( int row, int column, const D3DXCOLOR &clrEmitColor )
{
	// given screen coords instance a new particle.
	PARTICLE *pParticle;

	// check if trying to instance on a boundary cell.
	if ( m_pDPGPU->IsBoundary(row,column) )
		return E_FAIL;

	if( (m_dwParticles+1<m_dwParticlesLim) )
	{
		if( m_pParticlesFree )
		{
			pParticle = m_pParticlesFree;
			m_pParticlesFree = pParticle->m_pNext;
		}
		else
		{
			if( NULL == ( pParticle = new PARTICLE ) )
				return E_OUTOFMEMORY;
		}

		pParticle->m_pNext = m_pParticles;
		m_pParticles = pParticle;
		m_dwParticles++;

		// Emit new particle
		pParticle->m_vPos.x = (float)column;
		pParticle->m_vPos.y = (float)row;

		pParticle->m_ActionID = m_pDPGPU->GetAction(row,column);

		pParticle->m_CountsPerMove= 800;
		pParticle->m_Counter = pParticle->m_CountsPerMove;


		pParticle->m_clrDiffuse = clrEmitColor;
        pParticle->m_fTime0     = m_fPSysTime;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------

bool CParticleSystem::GoalReached(PARTICLE* pParticle)
{
	// check if particle has reached goal.
	if ( m_pDPGPU->GetCurrValue(int(pParticle->m_vPos.y), int(pParticle->m_vPos.x)) == 0.0f )
	{
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------

HRESULT CParticleSystem::InitDeviceObjects( WorldScreenTransform* pWST, DPGPU* pDPGPU, HWND hwnd )
{
	m_pWST = pWST;
	m_pDPGPU =  pDPGPU;
	m_hwnd = hwnd;
	return S_OK;
}
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	// Create a vertex buffer for the particle system.  The size of this buffer
	// does not relate to the number of particles that exist.  Rather, the
	// buffer is used as a communication channel with the device.. we fill in 
	// a bit, and tell the device to draw.  While the device is drawing, we
	// fill in the next bit using NOOVERWRITE.  We continue doing this until 
	// we run out of vertex buffer space, and are forced to DISCARD the buffer
	// and start over at the beginning.

	if(FAILED(hr = pd3dDevice->CreateVertexBuffer( m_dwDiscard * 
		sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
		POINTVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
		return E_FAIL;
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pVB );

	return S_OK;
}


//-----------------------------------------------------------------------------
HRESULT CParticleSystem::DeleteDeviceObjects()
{
	m_pWST = NULL;
	m_pDPGPU = NULL;
	return S_OK;
}




//-----------------------------------------------------------------------------
void GetDeltaFromAction( int actionID, int& dRowOut, int& dColOut )
{
	// 0 1 2
	// 3   4
	// 5 6 7

	dRowOut=dColOut=0;

	switch (actionID)
	{
	case 0:
		dRowOut = -1;
		dColOut = -1;
		break;
	case 1:
		dRowOut = -1;
		dColOut = 0;
		break;
	case 2:
		dRowOut = -1;
		dColOut = 1;
		break;
	case 3:
		dRowOut = 0;
		dColOut = -1;
		break;
	case 4:
		dRowOut = 0;
		dColOut = +1;
		break;
	case 5:
		dRowOut = +1;
		dColOut = -1;
		break;
	case 6:
		dRowOut = +1;
		dColOut = 0;
		break;
	case 7:
		dRowOut = +1;
		dColOut = +1;
		break;
	default:
		assert(0);
	}

	
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::Update( FLOAT fSecsPerFrame )
{
	PARTICLE *pParticle, **ppParticle;
	m_fPSysTime += 0.01f;

	ppParticle = &m_pParticles;

	while( *ppParticle )
	{
		pParticle = *ppParticle;

		if (pParticle->m_Counter>0)
		{
			pParticle->m_Counter-= 0.5f*fSecsPerFrame ;
			continue;
		}

		// Calculate new position. remembering that there is a chance of 
		// ending up somewhere other than the intended action.
		
		int outcomeID = m_pDPGPU->m_AOPM.GetOutcomeActionID(pParticle->m_ActionID);
		int drow, dcol;
		int row, col;
		GetDeltaFromAction( outcomeID , drow, dcol );
		col = int(pParticle->m_vPos.x) + dcol;
		row = int(pParticle->m_vPos.y) + drow;
		// boundary check:
		if (! m_pDPGPU->IsBoundary(row, col) )
		{
			pParticle->m_vPos.x = float(col);
			pParticle->m_vPos.y = float(row);
			pParticle->m_ActionID = m_pDPGPU->GetAction(row,col );
		}

		pParticle->m_Counter = pParticle->m_CountsPerMove;

		// Kill particles on reaching goal or living too long
		if (GoalReached(pParticle) || (m_fPSysTime-pParticle->m_fTime0 >50.0f) )
		{
			*ppParticle = pParticle->m_pNext;
			pParticle->m_pNext = m_pParticlesFree;
			m_pParticlesFree = pParticle;
			m_dwParticles--;
		}
		else
		{
			ppParticle = &pParticle->m_pNext;
		}
	}

	pParticle = m_pParticles;

#ifdef _SETCURSOR_
	if ( pParticle )
	{
		POINT p;
		m_pWST->WorldToScreen( pParticle->m_vPos.y, pParticle->m_vPos.x, (int&)(p.x), (int&)p.y );
		ClientToScreen(m_hwnd, &p);
		SetCursorPos( p.x, p.y );
	}
#endif
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the particle system using either pointsprites (if supported)
//       or using 4 vertices per particle
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifndef _SETCURSOR_



	HRESULT hr;

	// Set up the vertex buffer to be rendered
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(POINTVERTEX) );
	pd3dDevice->SetFVF( POINTVERTEX::FVF );

	if ( FAILED(pd3dDevice->SetPixelShader(NULL) ) )
	{
		assert(0);
	}
	// Set the render states for using point sprites
	pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(m_pWST->Get_ScrDeltaX()) );

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );


	PARTICLE*    pParticle = m_pParticles;
	POINTVERTEX* pVertices;
	DWORD        dwNumParticlesToRender = 0;

	// Lock the vertex buffer.  We fill the vertex buffer in small
	// chunks, using D3DLOCK_NOOVERWRITE.  When we are done filling
	// each chunk, we call DrawPrim, and lock the next chunk.  When
	// we run out of space in the vertex buffer, we start over at
	// the beginning, using D3DLOCK_DISCARD.

	m_dwBase += m_dwFlush;

	if(m_dwBase >= m_dwDiscard)
		m_dwBase = 0;

	if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
	{
		return hr;
	}

	// Render each particle
	while( pParticle )
	{
		int scrx, scry;
		m_pWST->WorldToScreen( int(pParticle->m_vPos.y), int(pParticle->m_vPos.x), scrx, scry );
		D3DXVECTOR3 vPos( (float)scrx, (float)scry, 0 );
		UINT        dwSteps;
		DWORD dwDiffuse = (pParticle->m_clrDiffuse);

		dwSteps = 1;
		// Render each particle a bunch of times to get a blurring effect
		for( DWORD i = 0; i < dwSteps; i++ )
		{
			pVertices->v     = vPos;
			pVertices->color = dwDiffuse;
			pVertices++;

			++dwNumParticlesToRender;
			if( dwNumParticlesToRender == m_dwFlush )
			{
				// Done filling this chunk of the vertex buffer.  Lets unlock and
				// draw this portion so we can begin filling the next chunk.

				m_pVB->Unlock();

				if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender)))
					return hr;

				// Lock the next chunk of the vertex buffer.  If we are at the 
				// end of the vertex buffer, DISCARD the vertex buffer and start
				// at the beginning.  Otherwise, specify NOOVERWRITE, so we can
				// continue filling the VB while the previous chunk is drawing.
				m_dwBase += m_dwFlush;

				if(m_dwBase >= m_dwDiscard)
					m_dwBase = 0;

				if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
					(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
				{
					return hr;
				}

				dwNumParticlesToRender = 0;
			}
		}
		pParticle = pParticle->m_pNext;
	}

	// Unlock the vertex buffer
	m_pVB->Unlock();

	// Render any remaining particles
	if( dwNumParticlesToRender )
	{
		if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender )))
			return hr;
	}

	// Reset render states
	pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

#endif

	return S_OK;
}
//-------------------------------------------

HRESULT CParticleSystem::KillAllParticles()
{
	PARTICLE *pParticle, **ppParticle;
	ppParticle = &m_pParticles;

	while( *ppParticle )
	{
		pParticle = *ppParticle;

		*ppParticle = pParticle->m_pNext;
		pParticle->m_pNext = m_pParticlesFree;
		m_pParticlesFree = pParticle;
		m_dwParticles--;
	}
	return S_OK;
}



#else
//-------------------------------------------

#include "dropper.h"
#include "dxutil.h"
#include "dpGPU.h"
#include "worldtoscreen.h"
#include <assert.h>

// Helper function to stuff a FLOAT into a DWORD argument

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


struct POINTVERTEX
{
	D3DXVECTOR3 v;
	D3DCOLOR    color;

	static const DWORD FVF;
};
const DWORD POINTVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;




float CParticleSystem::m_fPSysTime = 0.0f;
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem( DWORD dwFlush, DWORD dwDiscard )
{

	m_dwBase         = dwDiscard;
	m_dwFlush        = dwFlush;
	m_dwDiscard      = dwDiscard;

	m_dwParticles    = 0;
	m_dwParticlesLim = 2048;

	m_pParticles     = NULL;
	m_pParticlesFree = NULL;
	m_pVB            = NULL;
	m_fPSysTime		 = 0;

	m_pWST				= NULL;
	m_pDPGPU			= NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
}

//-----------------------------------------------------------------------------

HRESULT CParticleSystem::InstanceNewParticle( int row, int column, const D3DXCOLOR &clrEmitColor )
{
	return S_OK;
}

//-----------------------------------------------------------------------------

bool CParticleSystem::GoalReached(PARTICLE* pParticle)
{
	return false;
}
//-----------------------------------------------------------------------------

HRESULT CParticleSystem::InitDeviceObjects( WorldScreenTransform* pWST, DPGPU* pDPGPU )
{
	return S_OK;
}
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::InvalidateDeviceObjects()
{
	return S_OK;
}


//-----------------------------------------------------------------------------
HRESULT CParticleSystem::DeleteDeviceObjects()
{
	return S_OK;
}




//-----------------------------------------------------------------------------
void GetDeltaFromAction( int actionID, int& dRowOut, int& dColOut )
{
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::Update( FLOAT fSecsPerFrame )
{
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the particle system using either pointsprites (if supported)
//       or using 4 vertices per particle
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	return S_OK;
}


HRESULT CParticleSystem::KillAllParticles()
{
	return S_OK;
}
#endif