//
// Direct3D Color Grader library 1.1 for ShaderX³
//
// ©2004 Ronny Burkersroda <RBurkersroda@lightrock.biz>
//
//
// This software is property of the Ronny Burkersroda and may not be copied,
// redistributed or published in any way without permission!
// Abuse will immediately result in legal steps without preliminary warnings.
//
// You are allowed to use this software in your own products, if they are only
// published as compiled programs. Please inform me about the product and add
// me to the credits!
//

#ifndef						_D3D_COLOR_GRADER_H__INCLUDED_
#define						_D3D_COLOR_GRADER_H__INCLUDED_


#pragma						once


// header includes
#include					"d3dcolgrad9.h"								// Direct3D 9 color grader


// class definition
class						CD3DColorGrader;							// Direct3D color grader


//
// declaration of the Direct3D color grader class
//
class						CD3DColorGrader								// Direct3D color grader
	: public virtual			ID3DColorGrader9							// interface of Direct3D color grader
{
public:
	// method declarations
	DWORD						Release(									// release a reference of the object
		);
	DWORD						AddRef(										// add a reference to the object
		);
	DWORD						GetRef(										// return current reference count of the object
		) const;

	HRESULT						SetGradientFormat(							// set resolution and format of the gradient
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue,										// resolution for blue component
		D3DCOLORGRADIENTFORMATTYPE	cgftFormat									// gradient format
		);
	HRESULT						GetGradientFormat(							// get resolution and format of the gradient
		DWORD*						pnRed,										// pointer to store resolution for red component to
		DWORD*						pnGreen,									// pointer to store resolution for green component to
		DWORD*						pnBlue,										// pointer to store resolution for blue component to
		D3DCOLORGRADIENTFORMATTYPE*	pcgftFormat									// pointer to store gradient format to
		) const;
	HRESULT						FillGradient(								// fill gradient with values calculated by a callback function
		PD3DGRADIENTFUNCTION		pgrfcCallback,								// pointer to callback function
		WPARAM						wprmParameter								// application defined parameter for callback function
		);
	HRESULT						ResetGradient(								// reset gradient
		);
	HRESULT						SaveGradientTo3DFile(						// save gradient as DDS file
		PCTSTR						pstrFileName								// pointer to file name
		);
	HRESULT						SaveGradientTo2DFile(						// save gradient as BMP file
		PCTSTR						pstrFileName								// pointer to file name
		);
	HRESULT						SaveGradientToMemory(						// save gradient to memory buffer
		PVOID						pBuffer,									// pointer to store gradient to
		PDWORD						pnSize										// pointer to size of the buffer
		);
	HRESULT						LoadGradientFromFile(						// load gradient from file
		PCTSTR						pstrFileName,								// pointer to file name
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue										// resolution for blue component
		);
	HRESULT						LoadGradientFromMemory(						// load gradient from memory
		const VOID*					pData,										// pointer to data
		DWORD						nSize,										// data size
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue										// resolution for blue component
		);

	HRESULT						RenderScreen(								// render a texture with graded colors to current render target
		PDIRECT3DTEXTURE9			ptxrSource,									// pointer to interface of the source texture
		const TEXTUREPLACEMENT*		ptxplTextureCoordinates,					// pointer to source texture coordinates
		D3DTEXTUREFILTERTYPE		txftFilter									// filter for interpolation between gradient keys
		);

	static PD3DCOLORGRADER9		Create(										// create an object of this class and return interface to it
		PDIRECT3DDEVICE9			pd3dd										// pointer to Direct3D device interface
		);

protected:
	CD3DColorGrader(														// standard constructor
		);
	virtual ~CD3DColorGrader(												// standard destructor
		);

	HRESULT						CreateGradientTexture(						// create a gradient texture of the given resolution
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue,										// resolution for blue component
		D3DCOLORGRADIENTFORMATTYPE	cgftFormat									// gradient format
		);

public:
	// attribute declarations
	PDIRECT3DDEVICE9			m_pd3dd;									// pointer to Direct3D device interface
	PDIRECT3DVOLUMETEXTURE9		m_pvltxGradient;							// pointer to volume texture with gradient
	PDIRECT3DVERTEXBUFFER9		m_pvtxbScreen;								// pointer to screen vertex buffer interface
	PDIRECT3DVERTEXSHADER9		m_pvshdScreen;								// pointer to screen vertex shader interface
	PDIRECT3DPIXELSHADER9		m_ppshdColorGrading;						// pointer to color grading pixel shader interface

	DWORD						m_nPixelShaderVersion;						// version of the pixel shader used
	DWORD						m_nRedResolution;							// resolution for red component
	DWORD						m_nGreenResolution;							// resolution for green component
	DWORD						m_nBlueResolution;							// resolution for blue component
	D3DCOLORGRADIENTFORMATTYPE	m_cgftFormat;								// gradient format

private:
	DWORD						m_nReferenceQuantity;						// count of the references on the object
};


#endif
