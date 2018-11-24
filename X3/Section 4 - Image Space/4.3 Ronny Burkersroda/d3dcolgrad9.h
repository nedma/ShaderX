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

#ifndef						_D3DCOLGRAD9_H__INCLUDED_
#define						_D3DCOLGRAD9_H__INCLUDED_


#pragma						once


// global definitions
#ifdef						WIN32_LEAN_AND_MEAN
#undef						WIN32_LEAN_AND_MEAN
#endif


// header includes
#include					<windows.h>									// windows types
#include					<d3d9.h>									// Direct3D 9


// enumeration declarations
typedef enum				_D3DCOLORGRADIENTFORMATTYPE					// format types of color gradient
{
	D3DCOLGRFMT_UNKNOWN			= 0x00000000,								// unknown format
	D3DCOLGRFMT_8BIT			= 0x00000001,								// 8 bit format
	D3DCOLGRFMT_16BIT			= 0x00000002,								// 16 bit format
	D3DCOLGRFMT_24BIT			= 0x00000003,								// 24 bit format
	D3DCOLGRFMT_32BIT			= 0x00000004,								// 32 bit format
	D3DCOLGRFMT_COMPRESSEDRGB	= 0x00000005,								// compressed format
	D3DCOLGRFMT_COMPRESSEDARGB	= 0x00000006,								// compressed format including alpha channel
}
D3DCOLORGRADIENTFORMATTYPE;


// structure declarations
typedef struct				_TEXTUREPLACEMENT							// texture placement coordinates
{
    FLOAT						fLeft;										// left u coordinate
    FLOAT						fTop;										// top v coordinate
    FLOAT						fRight;										// right u coordinate
    FLOAT						fBottom;									// bottom v coordinate
}
TEXTUREPLACEMENT;

typedef struct				_RGBCOLORVALUE								// RGB color value
{
    FLOAT						r;											// red component
    FLOAT						g;											// green component
    FLOAT						b;											// blue component
}
RGBCOLORVALUE;


// interface definitions
interface					ID3DColorGrader9;							// Direct3D color grader


// pointer definitions
typedef struct				ID3DColorGrader9							// interface of Direct3D color grader
								*PD3DCOLORGRADER9,
								*LPD3DCOLORGRADER9;


// function type definitions
typedef						VOID ( CALLBACK *PD3DGRADIENTFUNCTION )(	// callback function to build color gradient
								D3DCOLORVALUE*,								// pointer to store graded color value to
								const RGBCOLORVALUE*,						// pointer to key color value of original red, green and blue
								const RGBCOLORVALUE*,						// pointer to color value with the ranges of the key
								WPARAM										// application defined parameter
								);
typedef						PD3DGRADIENTFUNCTION
								LPD3DGRADIENTFUNCTION;


// global functions
extern "C" PD3DCOLORGRADER9	D3DColorGraderCreate9(						// create a color grader object and returns the pointer to its interface
	PDIRECT3DDEVICE9			pd3ddDevice									// pointer to Direct3D device interface
	);


//
// virtual declaration of the interface for the Direct3D color grader
//
interface					ID3DColorGrader9							// Direct3D color grader
{
	virtual DWORD				Release(									// release a reference of the object
		) = 0;
	virtual DWORD				AddRef(										// add a reference to the object
		) = 0;
	virtual DWORD				GetRef(										// return current reference count of the object
		) const = 0;

	virtual HRESULT				SetGradientFormat(							// set resolution and format of the gradient
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue,										// resolution for blue component
		D3DCOLORGRADIENTFORMATTYPE	cgftFormat									// gradient format
		) = 0;
	virtual HRESULT				GetGradientFormat(							// get resolution and format of the gradient
		DWORD*						pnRed,										// pointer to store resolution for red component to
		DWORD*						pnGreen,									// pointer to store resolution for green component to
		DWORD*						pnBlue,										// pointer to store resolution for blue component to
		D3DCOLORGRADIENTFORMATTYPE*	pcgftFormat									// pointer to store gradient format to
		) const = 0;
	virtual HRESULT				FillGradient(								// fill gradient with values calculated by a callback function
		PD3DGRADIENTFUNCTION		pgrfcCallback,								// pointer to callback function
		WPARAM						wprmParameter								// application defined parameter for callback function
		) = 0;
	virtual HRESULT				ResetGradient(								// reset gradient
		) = 0;
	virtual HRESULT				SaveGradientTo3DFile(						// save gradient as DDS file
		PCTSTR						pstrFileName								// pointer to file name
		) = 0;
	virtual HRESULT				SaveGradientTo2DFile(						// save gradient as BMP file
		PCTSTR						pstrFileName								// pointer to file name
		) = 0;
	virtual HRESULT				SaveGradientToMemory(						// save gradient to memory buffer
		PVOID						pBuffer,									// pointer to store gradient to
		PDWORD						pnSize										// pointer to size of the buffer
		) = 0;
	virtual HRESULT				LoadGradientFromFile(						// load gradient from file
		PCTSTR						pstrFileName,								// pointer to file name
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue										// resolution for blue component
		) = 0;
	virtual HRESULT				LoadGradientFromMemory(						// load gradient from memory
		const VOID*					pData,										// pointer to data
		DWORD						nSize,										// data size
		DWORD						nRed,										// resolution for red component
		DWORD						nGreen,										// resolution for green component
		DWORD						nBlue										// resolution for blue component
		) = 0;

	virtual HRESULT				RenderScreen(								// render a texture with graded colors to current render target
		PDIRECT3DTEXTURE9			ptxrSource,									// pointer to interface of the source texture
		const TEXTUREPLACEMENT*		ptxplTextureCoordinates,					// pointer to source texture coordinates
		D3DTEXTUREFILTERTYPE		txftFilter									// filter for interpolation between gradient keys
		) = 0;
};


#endif