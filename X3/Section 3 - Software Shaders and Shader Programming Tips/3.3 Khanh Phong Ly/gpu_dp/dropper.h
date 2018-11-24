#ifndef  __DROPPER_H_
#define __DROPPER_H_

#include <d3dx9.h>
#include <tchar.h>


class WorldScreenTransform;
class DPGPU;
//-----------------------------------------------------------------------------
// Global data for the particles
//-----------------------------------------------------------------------------
struct PARTICLE
{
	D3DXVECTOR3 m_vPos;       // Current position in (column, row, 0) format
	int			m_ActionID;		// current action
    FLOAT       m_fTime0;     // Time of creation
	float		m_Counter;
	float		m_CountsPerMove;
	D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
	PARTICLE*   m_pNext;      // Next particle in list
};





//-----------------------------------------------------------------------------
// Name: Class that controls how instances move using action map. draw point sprites
// Desc:
//-----------------------------------------------------------------------------
class CParticleSystem
{
protected:

	DWORD     m_dwBase;
	DWORD     m_dwFlush;
	DWORD     m_dwDiscard;

	DWORD     m_dwParticles;
	DWORD     m_dwParticlesLim;
	PARTICLE* m_pParticles;
	PARTICLE* m_pParticlesFree;

	WorldScreenTransform*	m_pWST;
	DPGPU*					m_pDPGPU;	// essentially the particle controller.
	HWND					m_hwnd;
	// Geometry
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

	static float m_fPSysTime;// = 0.0f;

	bool	GoalReached(PARTICLE* pParticle);
public:
	CParticleSystem( DWORD dwFlush, DWORD dwDiscard );
	~CParticleSystem();

	HRESULT InitDeviceObjects( WorldScreenTransform* pWST, DPGPU* pDPGPU, HWND hwnd	);
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	HRESULT Update( FLOAT fSecsPerFrame );
	HRESULT	InstanceNewParticle( int row, int column, const D3DXCOLOR &clrEmitColor );
	HRESULT KillAllParticles();

	HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );

};





#endif// __DROPPER_H_
