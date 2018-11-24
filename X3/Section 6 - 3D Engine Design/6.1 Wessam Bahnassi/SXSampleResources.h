/////////////////////////////////////////////////////////
// File : SXSampleResources.h
// Description : Illustration of how to implement common
//				 Direct3D resources using the proposed
//				 system.
//				 (SXFont) class declaration.
//				 (SXTeapot) class declaration.
//				 (SXGeometryBuffer) class declaration.
//				 (SXTexture2D) class declaration.
//				 (SXShader) class declaration.
// Author : Wessam Bahnassi, In|Framez
//
/////////////////////////////////////////////////////////

///////////////// SXFont Class Declaration /////////////////
class SXFont : public SXResource
{
public:
	SXFont(const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L);
	~SXFont();

	CD3DFont& GetD3DFont() { return m_d3dFont; };
protected:
	// Overrides
	HRESULT OnDeviceLoss(void);
	HRESULT OnDeviceRelease(void);
	HRESULT OnDeviceRestore(void);
	CD3DFont m_d3dFont;
};

class SXTeapot : public SXResource
{
public:
	SXTeapot();
	~SXTeapot();

	ID3DXMesh* GetD3DXMesh(void) const { return m_pMesh; };

protected:
	// Overrides
	HRESULT OnDeviceRelease(void);
	HRESULT OnDeviceRestore(void);
	ID3DXMesh *m_pMesh;
};


///////////////// SXGeometryBuffer Class Declaration /////////////////
class SXGeometryBuffer : public SXResource
{
public:
	SXGeometryBuffer();
	~SXGeometryBuffer();

	// Initialization
	HRESULT CreateVertexBuffer(DWORD dwVertexSize,DWORD dwVerticesCount,const void *pVertices = NULL);
	HRESULT CreateIndexBuffer(DWORD dwIndicesCount,const WORD* pIndices = NULL);

	// Accessors
	PDIRECT3DVERTEXBUFFER9 GetVertexBuffer(void) const { return m_pVB; };
	PDIRECT3DINDEXBUFFER9 GetIndexBuffer(void) const { return m_pIB; };
	DWORD GetVerticesCount(void) const;
	DWORD GetIndicesCount(void) const;
	DWORD GetVertexSize(void) const { return m_dwVertexSize; };
	void* GetVertices(void) const { return m_pVertices; };
	WORD* GetIndices(void) const { return m_pIndices; };

protected:
	// Overrides
	HRESULT OnDeviceLoss(void);
	HRESULT OnDeviceRelease(void);
	HRESULT OnDeviceRestore(void);

	DWORD m_dwIndicesCount;
	DWORD m_dwVerticesCount;
	DWORD m_dwVertexSize;
	void *m_pVertices;
	WORD *m_pIndices;

	PDIRECT3DVERTEXBUFFER9 m_pVB;
	PDIRECT3DINDEXBUFFER9 m_pIB;
};


///////////////// SXTexture2D Class Declaration /////////////////
class SXTexture2D : public SXResource
{
public:
	SXTexture2D();
	~SXTexture2D();

	// Initialization
	HRESULT SetTextureFile(PCTSTR pszFileName);

	// Accessors
	PDIRECT3DTEXTURE9 GetTexture(void) const { return m_pTexture; };
	PCTSTR GetSourceFileName(void) const { return m_tszFileName; };

protected:
	// Overrides
	//HRESULT OnDeviceLoss(void);
	HRESULT OnDeviceRelease(void);
	HRESULT OnDeviceRestore(void);

	TCHAR m_tszFileName[MAX_PATH];
	PDIRECT3DTEXTURE9 m_pTexture;
};


///////////////// SXShader Class Declaration /////////////////
class SXShader : public SXResource
{
public:
	SXShader();
	~SXShader();

	// Initialization
	HRESULT SetShaderFile(PCTSTR pszFileName);

	// Accessors
	PDIRECT3DVERTEXSHADER9 GetVertexShader(void) const { return m_pVertexShader; };
	PDIRECT3DPIXELSHADER9 GetPixelShader(void) const { return m_pPixelShader; };
	const DWORD* GetFunction(void) const { return m_pShaderFunction; };

protected:
	// Overrides
	//HRESULT OnDeviceLoss(void);
	HRESULT OnDeviceRelease(void);
	HRESULT OnDeviceRestore(void);

	DWORD *m_pShaderFunction;
	PDIRECT3DVERTEXSHADER9 m_pVertexShader;
	PDIRECT3DPIXELSHADER9 m_pPixelShader;
};

///////////////// End of File : SXSampleResources.h /////////////////