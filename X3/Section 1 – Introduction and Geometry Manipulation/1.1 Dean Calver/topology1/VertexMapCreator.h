#if !defined(VERTEX_MAP_CREATOR_HEADER)
#define VERTEX_MAP_CREATOR_HEADER

class VertexMapCreator
{
public:
	enum VMC_CONVERTTYPES
	{
		VC_POSITION,
		VC_NORMAL,
		VC_UV,	
	};

	const static int SIZE_OF_MAP = 256;
	static int HeightOfMap;

	// extracts data from an X mesh into a vertex texture
	static HRESULT ConvertXToVertexMap( IDirect3DDevice9* pd3dDevice, 
											ID3DXMesh* pMesh, 
											VMC_CONVERTTYPES type,
											IDirect3DTexture9*& vertexMap ); 
	// extracts data from an X mesh index buffer in a vertex stream
	static HRESULT ConvertXIndexToVertexStream(	IDirect3DDevice9* pd3dDevice, 
												ID3DXMesh* pMesh, 
												IDirect3DVertexBuffer9*& vertexStream );

	static HRESULT VertexMapCreator::Create123VertexStream(	IDirect3DDevice9* pd3dDevice, 
														IDirect3DVertexBuffer9*& vertexStream );

	// fix for dx9.0c vertex frequency weirdness
	static HRESULT VertexMapCreator::Create123IndexStream(	IDirect3DDevice9* pd3dDevice, 
															IDirect3DIndexBuffer9*& indexStream );

};



#endif