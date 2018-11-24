#if !defined(VERTEX_MAP_CREATOR_HEADER)
#define VERTEX_MAP_CREATOR_HEADER

class VertexMapCreator
{
	const static int SIZE_OF_MAP = 256;
public:
	enum VMC_CONVERTTYPES
	{
		VC_POSITION = 0,
		VC_NORMAL,
		VC_UV,	
	};

	enum MAP_FREQ
	{
		MF_PER_VERTEX = 0,
		MF_PER_FACE = 1,
	};

	static HRESULT CreateRTMap(	IDirect3DDevice9* pd3dDevice, 
								ID3DXMesh* pMesh, 
								MAP_FREQ eFreq, 
								IDirect3DTexture9*& RTMap,
								float* vertSize );

	// extracts data from an X mesh into a vertex texture
	static HRESULT ConvertXToVertexMap( IDirect3DDevice9* pd3dDevice, 
											ID3DXMesh* pMesh, 
											VMC_CONVERTTYPES type,
											IDirect3DTexture9*& vertexMap,
											float* vertSize ); 

	// extracts data from an X mesh index buffer in a vertex stream
	static HRESULT ConvertXIndexToVertexStream(	IDirect3DDevice9* pd3dDevice, 
												ID3DXMesh* pMesh, 
												IDirect3DVertexBuffer9*& vertexStream );

	static HRESULT ConvertXIndexToFaceTopologyMap(	IDirect3DDevice9* pd3dDevice, 
													ID3DXMesh* pMesh, 
													IDirect3DTexture9*& topoMap,
													 float* vertSize );

	// create a vertex stream counting from 0 to count.
	static HRESULT CreateCounterVertexStream(	IDirect3DDevice9* pd3dDevice, 
												IDirect3DVertexBuffer9*& vertexStream,
												unsigned int count );

	static HRESULT Create123VertexStream(	IDirect3DDevice9* pd3dDevice, 
											IDirect3DVertexBuffer9*& vertexStream );

	// fix for dx9.0c vertex frequency weirdness
	static HRESULT Create123IndexStream(	IDirect3DDevice9* pd3dDevice, 
											IDirect3DIndexBuffer9*& indexStream );

};



#endif