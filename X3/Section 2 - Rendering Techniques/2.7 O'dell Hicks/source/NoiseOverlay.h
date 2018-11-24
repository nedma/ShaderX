#pragma once

#include <D3DX9.h>
#include <D3D9.h>

typedef struct 
{
	D3DXVECTOR4 xyz_rhw;	
	DWORD diffuse;	
	D3DXVECTOR2 textCoord;
}SCREEN_VERTEX;  

//FVF for screen vertices (for noise)
#define SCREEN_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )


class NoiseOverlay
{
public:
	NoiseOverlay(LPDIRECT3DDEVICE9 pD3DDev, int screenWidth, int screenHeight);
	~NoiseOverlay();

	void Render(LPDIRECT3DDEVICE9 pD3DDev, float timeDelta);

protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;	
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;		
	LPDIRECT3DTEXTURE9      m_pTexture;		
	
	float m_fNoiseUpdateTimer;
};