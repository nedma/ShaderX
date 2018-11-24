#if !defined(TERRAIN_HEADER)
#define TERRAIN_HEADER

class Terrain
{
public:
	Terrain( IDirect3DDevice9* pd3dDevice );
	~Terrain();

	void RegenerateVertexData();

	void Render( const D3DXMATRIXA16& matrix );

	void Reset(); // pass the resets through to the effects
	void LostDevice();
	IDirect3DTexture9*	GetHeightfield()
	{
		return m_pTexture;
	}
private:
	void SetupRenderTargets();
	void ReleaseResources();

	void GenerateVertexPositions();
	void GenerateFaceNormals();
	void GenerateVertexNormals();

	IDirect3DDevice9*	m_pd3dDevice;		//!< D3D device we are attached to
	IDirect3DTexture9*	m_pTexture;			//!< heightfield texture that is the source
	unsigned int		m_numTriangles;		//!< How many triangles make up this height field
	unsigned int		m_numVertices;		//!< How many vertices make up this height field
	IDirect3DIndexBuffer9*	m_renderIndexBuffer;	//!< index buffer for fast screen rendering
	IDirect3DVertexBuffer9* m_renderVertexBuffer;	//!< vertex buffer for fast screen rendering
	IDirect3DVertexDeclaration9* m_indexVSDecl;		//!< decleration for rendering

	// dynamic (GPU) generated data
	IDirect3DTexture9*	m_posVertexMap;
	IDirect3DTexture9*	m_normVertexMap;
	IDirect3DTexture9*	m_faceNormalMap;

	// topology maps
	IDirect3DTexture9*	m_faceTopoMap;	//!< vertex to face topology map
	IDirect3DTexture9*	m_vvTopoMap;	//!< vertex valency topology map

	// size of maps used from vertex or face shader
	float				m_vertMapSizes[2];
	float				m_faceMapSizes[2];

	// shader core
	ID3DXEffect*		m_pGenPosVertMap;	//!< D3DX effect to convert height into position
	ID3DXEffect*		m_pRenderTerrain;	//!< D3DX effect to render terrain
	ID3DXEffect*		m_pGenFaceNormTopoMap;	//!< D3DX effect to generate face normals
	ID3DXEffect*		m_pGenNormalVertMap;	//!< D3DX effect to generate vertex normals

};

#endif // end Terrain