#ifndef __DPGPU_H__
#define __DPGPU_H__

#include "dpinterface.h"
#include <d3dx9.h>
#include <tchar.h>


// Custom D3D vertex format used by the vertex buffer
struct CUSTOMVERTEX
{
	D3DXVECTOR4 position;       // vertex position
	D3DXVECTOR2 texcoords0;		
	D3DXVECTOR2 texcoords1;

};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1))


struct DPGPU_Params : public DPParams
{
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	char*					m_pDataFileName;
	char*					m_pBoundaryMapFileName;
	char*					m_pImageFileName;
};




class DPSoft;


class DPGPU : public DPBase
{
	friend DPSoft;
protected:

    LPDIRECT3DDEVICE9		m_pd3dDevice;        // copy of ref to the The D3D rendering device

    LPDIRECT3DVERTEXBUFFER9 m_pVB;                  // for data iteration
	D3DFORMAT				m_DataTextureFormat;


	LPDIRECT3DTEXTURE9      m_pNeighbourMaskTex[2];
	D3DFORMAT				m_NeighbourMaskFormat;
	LPDIRECT3DTEXTURE9      m_pResidueProbsTex[2];
	D3DFORMAT				m_ResidueProbsFormat;

	int						m_bFirstPass;

	static const int		m_MaxNumPSConst4Vecs= 32;
	static const float		m_OrigValueScaleFactor; //scale down to fit into dynamic range of float

	struct IterateValsPSConsts
	{
		float				m_PSConst[4*m_MaxNumPSConst4Vecs];
		HRESULT Init( int dataWidth, int dataHeight );
		HRESULT SetPSConsts( LPDIRECT3DDEVICE9 pd3dDevice );
	};

	struct CreateActionMapPSConsts
	{
		float				m_PSConst[4*m_MaxNumPSConst4Vecs];
		HRESULT Init( int dataWidth, int dataHeight );
		HRESULT SetPSConsts( LPDIRECT3DDEVICE9 pd3dDevice );
	};

	struct WarpImagePSConsts
	{
		float				m_PSConst[4*m_MaxNumPSConst4Vecs];
		HRESULT Init( int dataWidth, int dataHeight );
		HRESULT SetPSConsts( LPDIRECT3DDEVICE9 pd3dDevice );
	};


	enum
	{
		ITERATE_VALS_PS,
		CREATE_ACTION_MAP_PS,
		NUMPIXELSHADERS,
	};

	IterateValsPSConsts			m_IVPSConsts;
	CreateActionMapPSConsts		m_CAMPSConsts;
	WarpImagePSConsts			m_WIPSConsts;

	LPDIRECT3DPIXELSHADER9	m_pPixelShader[NUMPIXELSHADERS];

	HRESULT	CreateNInit_DataTexture();
	HRESULT ApplyBoundaryMaskToDataTexture();
	HRESULT	CreateNInit_NeighbourhoodData();
	HRESULT	_ValidateBoundaryMap();
	HRESULT ValidateDataMap();

	HRESULT	Release_DataTexture();
	HRESULT Release_NeighbourhoodData();

	inline void _ModifyVal( const unsigned char boundVal, float *pVal, float modVal );
public:
	DPGPU();
	virtual ~DPGPU(){}

	virtual int IterateVals(int numIterations, bool bRestartFromInitialData);
	virtual int CreateActionMap();

	virtual HRESULT OneTimeSceneInit(DPParams* p);
	virtual HRESULT InitDeviceObjects(DPParams* p);
	virtual HRESULT RestoreDeviceObjects(DPParams* p);
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT FinalCleanup();


	DPGPU_Params		m_InitOnceParams;
	
	// access funtions
	virtual int	GetAction( int row, int col );
	virtual float GetCurrValue( int row, int col );
	virtual bool IsBoundary(int row, int col);
	float		GetOrigValue( int row, int col );

//	void Init_ActionOutcomeProbabilityMap(D3DXVECTOR4 *pv, int numActions, int numOutcomes);
//	int	FillActionOutcomeProbabilityMap( float* pOutArray ); // make a copy for clients.
//	float	*GetActionOutcomeProbabilityMap(){return (float*)(&m_IVPSConsts.m_PSConst[0]) ; }




	HRESULT ModifyMap3x3( int row, int col, float modVal );



	D3DFORMAT	GetDataTextureFormat(){return m_DataTextureFormat;}

		// allocs and sets retArray on return.
	HRESULT FillFArrayWithBoundaryMapData( unsigned char** retArray, int dataSizeIn );

	LPDIRECT3DTEXTURE9      m_pBoundaryMapTex;
	D3DFORMAT				m_BoundaryMapFormat;
	LPDIRECT3DTEXTURE9      m_pDataTexture;
	LPDIRECT3DTEXTURE9		m_pRenderTargTex[2];
	LPDIRECT3DSURFACE9		m_pRenderTargSurf[2];
	LPDIRECT3DTEXTURE9		m_pDataRetrieveTexture[2];
	LPDIRECT3DSURFACE9		m_pDataRetrieveSurface[2];	// retrieve from graphics card to here.
	LPDIRECT3DTEXTURE9		m_pDisplayTex;
	LPDIRECT3DTEXTURE9		m_pActionTex;
	LPDIRECT3DSURFACE9		m_pActionSurf;
	D3DFORMAT				m_ActionTexFormat;

	LPDIRECT3DTEXTURE9      m_pOrigImage;
	D3DFORMAT				m_ImageFormat;
	LPDIRECT3DTEXTURE9		m_pCurrImageTex[2];
	LPDIRECT3DSURFACE9		m_pCurrImageSurf[2];
	int						m_ImageWidth;
	int						m_ImageHeight;
	int						m_ImageBuffId;

	

};




#endif // __DPGPU_H__