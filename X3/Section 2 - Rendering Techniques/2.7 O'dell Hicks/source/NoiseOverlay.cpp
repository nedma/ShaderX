//separated from the main source file for clariy
//draws a noisy overlay over the viewport

#include <NoiseOverlay.h>
#include <DXUtil.h>

NoiseOverlay::NoiseOverlay(LPDIRECT3DDEVICE9 pD3DDev, int screenWidth, int screenHeight)
{
	m_pVertexBuffer = NULL;	
	m_pIndexBuffer = NULL;		
	m_pTexture = NULL;	
	
	SCREEN_VERTEX* vertices;
	WORD* indices;

	//vertex, index buffer - 4 vertices, 6 indices
	pD3DDev->CreateVertexBuffer( 4 * sizeof(SCREEN_VERTEX),
                                      D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
                                      &m_pVertexBuffer, NULL );
    
    pD3DDev->CreateIndexBuffer( 6 * sizeof(WORD),
                                      D3DUSAGE_WRITEONLY,
                                      D3DFMT_INDEX16, D3DPOOL_MANAGED,
                                      &m_pIndexBuffer, NULL );
	
	m_pVertexBuffer->Lock( 0, 0, (void**)&vertices, 0 );

	DWORD color = D3DCOLOR_RGBA(255, 255, 255, 128);
	//top left	
	vertices[0].xyz_rhw = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	vertices[0].diffuse = color;
	vertices[0].textCoord = D3DXVECTOR2(0.0f, 0.0f);

	//top right
	vertices[1].xyz_rhw = D3DXVECTOR4(screenWidth, 0.0f, 0.0f, 1.0f);
	vertices[1].diffuse = color;
	vertices[1].textCoord = D3DXVECTOR2(1.0f, 0.0f);

	//bottom right
	vertices[2].xyz_rhw = D3DXVECTOR4(screenWidth, screenHeight, 0.0f, 1.0f);
	vertices[2].diffuse = color;
	vertices[2].textCoord = D3DXVECTOR2(1.0f, 1.0f);

	//bottom left
	vertices[3].xyz_rhw = D3DXVECTOR4(0.0f, screenHeight, 0.0f, 1.0f);
	vertices[3].diffuse = color;
	vertices[3].textCoord = D3DXVECTOR2(0.0f, 1.0f);

	m_pVertexBuffer->Unlock();


	m_pIndexBuffer->Lock( 0, 0, (void**)&indices, 0 );

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	m_pIndexBuffer->Unlock();


	//texture
	D3DXCreateTextureFromFile( pD3DDev, "noise.bmp", &m_pTexture );

	m_fNoiseUpdateTimer = 0.0f;
}

NoiseOverlay::~NoiseOverlay()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pTexture);
}

void NoiseOverlay::Render(LPDIRECT3DDEVICE9 pD3DDev, float timeDelta)
{	
	pD3DDev->SetVertexShader(NULL );		
	pD3DDev->SetPixelShader(NULL );		
	pD3DDev->SetFVF(SCREEN_FVF);

	pD3DDev->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(SCREEN_VERTEX) );
	pD3DDev->SetIndices(m_pIndexBuffer);					
				
	pD3DDev->SetTexture(0, m_pTexture);			
	
	//alpha blending on
	pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pD3DDev->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA  );
	pD3DDev->SetRenderState( D3DRS_DESTBLEND,   D3DBLEND_INVSRCALPHA  );

	//randomize noise coords no more than every 50ms

	m_fNoiseUpdateTimer += timeDelta;

	if(m_fNoiseUpdateTimer > 0.020f)
	{	
		m_fNoiseUpdateTimer = 0.0f;

		SCREEN_VERTEX* vertices;
		m_pVertexBuffer->Lock( 0, 0, (void**)&vertices, 0 );

		float x = (float)(rand() % 127 ) / 255.0f; //0 through .5
		float y = (float)(rand() % 127 ) / 255.0f; //0 through .5

		
		//top left
		vertices[0].textCoord = D3DXVECTOR2(x, y);

		//top right	
		vertices[1].textCoord = D3DXVECTOR2(x + 0.5f, y);

		//bottom right	
		vertices[2].textCoord = D3DXVECTOR2(x + 0.5f, y + 0.5f);

		//bottom left	
		vertices[3].textCoord = D3DXVECTOR2(x, y + 0.5f);

		m_pVertexBuffer->Unlock();	
	}
	
	pD3DDev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST ,0,0,4,
		0, 2);	

	//alpha blending off
	pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
}



