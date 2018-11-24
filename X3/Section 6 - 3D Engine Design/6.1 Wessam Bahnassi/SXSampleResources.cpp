/////////////////////////////////////////////////////////
// File : SXSampleResources.cpp
// Description : Illustration of how to implement common
//				 Direct3D resources using the proposed
//				 system.
//				 (SXFont) class implementation.
//				 (SXTeapot) class implementation.
//				 (SXGeometryBuffer) class implementation.
//				 (SXTexture2D) class implementation.
//				 (SXShader) class implementation.
// Author : Wessam Bahnassi, In|Framez
//
/////////////////////////////////////////////////////////

///////////////// #includes /////////////////
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "SXSampleResources.h"
#include "ResMgmt.h"


///////////////// #defines /////////////////
#define IS_VERTEXSHADER_FUNCTION(pFunc) (((*pFunc) >> 16) == 0xFFFE)
#define IS_PIXELSHADER_FUNCTION(pFunc) (((*pFunc) >> 16) == 0xFFFF)


///////////////// SXFont Class Implementation /////////////////
SXFont::SXFont(const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags) :
	m_d3dFont(strFontName,dwHeight,dwFlags)
{
}

SXFont::~SXFont()
{
}

HRESULT SXFont::OnDeviceLoss(void)
{
	return m_d3dFont.InvalidateDeviceObjects();
}

HRESULT SXFont::OnDeviceRelease(void)
{
	m_d3dFont.InvalidateDeviceObjects();
	return m_d3dFont.DeleteDeviceObjects();
}

HRESULT SXFont::OnDeviceRestore(void)
{
	if (m_d3dFont.GetD3DDevice())
		return m_d3dFont.RestoreDeviceObjects();
	
	HRESULT hRetval = m_d3dFont.InitDeviceObjects(m_pOwnerProbe->GetD3DDevice());
	if (FAILED(hRetval))
		return hRetval;
	return m_d3dFont.RestoreDeviceObjects();
}


///////////////// SXTeapot Class Implementation /////////////////
SXTeapot::SXTeapot() : m_pMesh(NULL)
{
}

SXTeapot::~SXTeapot()
{
}

HRESULT SXTeapot::OnDeviceRelease(void)
{
	SXSAFE_RELEASE(m_pMesh);
	return S_OK;
}

HRESULT SXTeapot::OnDeviceRestore(void)
{
	if (m_pMesh)
		return S_OK;

	return D3DXCreateTeapot(m_pOwnerProbe->GetD3DDevice(),&m_pMesh,NULL);
}


///////////////// SXGeometryBuffer Class Implementation /////////////////
SXGeometryBuffer::SXGeometryBuffer() :
	m_dwIndicesCount(0),
	m_dwVerticesCount(0),
	m_dwVertexSize(0),
	m_pVertices(NULL),
	m_pIndices(NULL),
	m_pVB(NULL),
	m_pIB(NULL)
{
}

SXGeometryBuffer::~SXGeometryBuffer()
{
	SXSAFE_RELEASE(m_pVB);
	SXSAFE_RELEASE(m_pIB);

	SXSAFE_DELETE_ARRAY(m_pVertices);
	SXSAFE_DELETE_ARRAY(m_pIndices);
}

HRESULT SXGeometryBuffer::CreateVertexBuffer(DWORD dwVertexSize,DWORD dwVerticesCount,const void *pVertices)
{
	if (!dwVertexSize)
	{
		SXTRACE("SXGeometryBuffer ERROR: Invalid vertex size");
		return E_INVALIDARG;
	}
	if (!dwVerticesCount)
	{
		SXTRACE("SXGeometryBuffer ERROR: Invalid vertices count");
		return E_INVALIDARG;
	}

	void *pNewVB = new BYTE[dwVertexSize*dwVerticesCount];
	if (!pNewVB)
		return E_OUTOFMEMORY;

	// Get rid of the old buffer
	SXSAFE_RELEASE(m_pVB);
	SXSAFE_DELETE_ARRAY(m_pVertices);
	m_pVertices = pNewVB;

	m_dwVertexSize = dwVertexSize;
	m_dwVerticesCount = dwVerticesCount;

	if (pVertices)
		memcpy(m_pVertices,pVertices,dwVertexSize*dwVerticesCount);

	return S_OK;
}

HRESULT SXGeometryBuffer::CreateIndexBuffer(DWORD dwIndicesCount,const WORD* pIndices)
{
	if (!dwIndicesCount)
	{
		SXTRACE("SXGeometryBuffer ERROR: Invalid indices count");
		return E_INVALIDARG;
	}

	WORD *pNewIB = new WORD[dwIndicesCount];
	if (!pNewIB)
		return E_OUTOFMEMORY;

	// Get rid of the old buffer
	SXSAFE_RELEASE(m_pIB);
	SXSAFE_DELETE_ARRAY(m_pIndices);
	m_pIndices = pNewIB;

	m_dwIndicesCount = dwIndicesCount;

	if (pIndices)
		memcpy(m_pIndices,pIndices,sizeof(WORD)*dwIndicesCount);

	return S_OK;
}

HRESULT SXGeometryBuffer::OnDeviceLoss(void)
{
	SXSAFE_RELEASE(m_pVB);
	SXSAFE_RELEASE(m_pIB);
	return S_OK;
}

HRESULT SXGeometryBuffer::OnDeviceRelease(void)
{
	return OnDeviceLoss();
}

HRESULT SXGeometryBuffer::OnDeviceRestore(void)
{
	PDIRECT3DDEVICE9 pDevice = m_pOwnerProbe->GetD3DDevice();
	HRESULT hRetval = S_OK;

	// Get this device's VP
	DWORD dwUsageVP = pDevice->GetSoftwareVertexProcessing()?D3DUSAGE_SOFTWAREPROCESSING:0;

	// Create vertex buffer?
	if (m_pVertices && !m_pVB)
	{
		hRetval = pDevice->CreateVertexBuffer(
			m_dwVerticesCount*m_dwVertexSize,	// Length
			dwUsageVP|D3DUSAGE_WRITEONLY,	// Usage
			0,	// FVF
			D3DPOOL_DEFAULT,	// Pool
			&m_pVB,NULL);
		if (FAILED(hRetval))
			return hRetval;

		// Lock'n'load
		void *pData = NULL;
		hRetval = m_pVB->Lock(0,m_dwVertexSize*m_dwVerticesCount,&pData,0);
		if (FAILED(hRetval))
		{
			SXSAFE_RELEASE(m_pVB);
			return hRetval;
		}
		memcpy(pData,m_pVertices,m_dwVertexSize*m_dwVerticesCount);	// Single yummy memcpy
		m_pVB->Unlock();
	}

	if (!m_pIndices || m_pIB)
		return S_OK;	// Non-indexed, or already valid

	hRetval = pDevice->CreateIndexBuffer(
		m_dwIndicesCount*sizeof(WORD),	// Length
		dwUsageVP|D3DUSAGE_WRITEONLY,	// Usage
		D3DFMT_INDEX16,	// Format
		D3DPOOL_DEFAULT,	// Pool
		&m_pIB,NULL);
	if (FAILED(hRetval))
		return hRetval;

	// Lock'n'load
	void *pData = NULL;
	hRetval = m_pIB->Lock(0,sizeof(WORD)*m_dwIndicesCount,&pData,0);
	if (FAILED(hRetval))
	{
		SXSAFE_RELEASE(m_pIB);
		return hRetval;
	}
	memcpy(pData,m_pIndices,sizeof(WORD)*m_dwIndicesCount);	// Single yummy memcpy
	m_pIB->Unlock();

	return hRetval;
}


///////////////// SXTexture2D Class Implementation /////////////////
SXTexture2D::SXTexture2D() : m_pTexture(NULL)
{
	memset(m_tszFileName,0,sizeof(m_tszFileName));
}

SXTexture2D::~SXTexture2D()
{
	SXSAFE_RELEASE(m_pTexture);
}

HRESULT SXTexture2D::SetTextureFile(PCTSTR pszFileName)
{
	if (!pszFileName || !pszFileName[0])
	{
		SXTRACE("SXTexture2D ERROR: Invalid file name");
		return E_INVALIDARG;
	}

	// Get rid of the old texture
	SXSAFE_RELEASE(m_pTexture);
	_tcscpy(m_tszFileName,pszFileName);
	return S_OK;
}

HRESULT SXTexture2D::OnDeviceRelease(void)
{
	SXSAFE_RELEASE(m_pTexture);
	return S_OK;
}

HRESULT SXTexture2D::OnDeviceRestore(void)
{
	if (m_pTexture || !m_tszFileName[0])
		return S_OK;	// Not released, or invalid file name

	PDIRECT3DDEVICE9 pDevice = m_pOwnerProbe->GetD3DDevice();
	return D3DXCreateTextureFromFile(pDevice,m_tszFileName,&m_pTexture);
}


///////////////// SXShader Class Implementation /////////////////
SXShader::SXShader() :
	m_pShaderFunction(NULL),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL)
{
}

SXShader::~SXShader()
{
	SXSAFE_RELEASE(m_pVertexShader);
	SXSAFE_RELEASE(m_pPixelShader);
	SXSAFE_DELETE_ARRAY(m_pShaderFunction);
}

HRESULT SXShader::SetShaderFile(PCTSTR pszFileName)
{
	if (!pszFileName || !pszFileName[0])
	{
		SXTRACE("SXShader ERROR: Invalid file name");
		return E_INVALIDARG;
	}

	LPD3DXBUFFER pShaderBuffer = NULL;
	HRESULT hRetval = D3DXAssembleShaderFromFile(pszFileName,NULL,NULL,0,&pShaderBuffer,NULL);
	if (FAILED(hRetval))
		return hRetval;

	DWORD *pShaderFunction = (DWORD*)(new BYTE[pShaderBuffer->GetBufferSize()]);
	if (!pShaderFunction)
	{
		SXSAFE_RELEASE(pShaderBuffer);
		return E_OUTOFMEMORY;
	}

	SXSAFE_DELETE_ARRAY(m_pShaderFunction);
	m_pShaderFunction = pShaderFunction;

	memcpy(m_pShaderFunction,pShaderBuffer->GetBufferPointer(),pShaderBuffer->GetBufferSize());

	SXSAFE_RELEASE(m_pVertexShader);
	SXSAFE_RELEASE(m_pPixelShader);
	return S_OK;
}

HRESULT SXShader::OnDeviceRelease(void)
{
	SXSAFE_RELEASE(m_pVertexShader);
	SXSAFE_RELEASE(m_pPixelShader);
	return S_OK;
}

HRESULT SXShader::OnDeviceRestore(void)
{
	if (m_pVertexShader || m_pPixelShader || !m_pShaderFunction)
		return S_OK;	// Not released, or not created yet

	PDIRECT3DDEVICE9 pDevice = m_pOwnerProbe->GetD3DDevice();

	// Create the shader... Determine its type
	HRESULT hRetval = E_INVALIDARG;
	if (IS_VERTEXSHADER_FUNCTION(m_pShaderFunction))
		hRetval = pDevice->CreateVertexShader(m_pShaderFunction,&m_pVertexShader);
	else if (IS_PIXELSHADER_FUNCTION(m_pShaderFunction))
		hRetval = pDevice->CreatePixelShader(m_pShaderFunction,&m_pPixelShader);
	if (FAILED(hRetval))
		return hRetval;

	return S_OK;
}

///////////////// End of File : SXSampleResources.cpp /////////////////