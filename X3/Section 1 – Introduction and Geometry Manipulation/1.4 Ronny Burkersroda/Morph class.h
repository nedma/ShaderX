//
// ShaderX³: Morphing Between Different Objects
//
//	written 2004 by Ronny Burkersroda
//

#ifndef						_MORPHER_SAMPLE_CLASS_H__INCLUDED_
#define						_MORPHER_SAMPLE_CLASS_H__INCLUDED_


#pragma once


// header includes
#include					"base class.h"								// sample base class


//
// class of the "Two In One" sample
//	shows morphing between two objects.
//
class						CD3DMorpherSample								// Direct3D "Two In One" sample class
	: public					CD3DSampleBase								// sample base class
{
protected:
	// structure declarations
	typedef struct				_MATERIAL									// material colors and base texture
	{
		D3DMATERIAL9				matrColors;									// color values
		PDIRECT3DTEXTURE9			ptxtrTexture;								// pointer to interface of the base texture
	}
	MATERIAL;

public:
	// method declarations
	CD3DMorpherSample(														// standard constructor
		);
	virtual ~CD3DMorpherSample(												// standard destructor
		);

	INT							Main(										// main function of the object
		HINSTANCE					hmodInstance,								// handle of the application instance
		PSTR						pstrCommandLine,							// pointer to command line string
		INT							nWindowShowValue							// flags specifying how the main window is show
		);

protected:
	static LRESULT WINAPI		WindowMessageProcedure(						// window message procedure
		HWND						hwndWindow,									// handle of the window
		UINT						nMessage,									// code of the message
		WPARAM						wprm,										// first message parameter
		LPARAM						lprm										// second message parameter
		);

protected:
	// attribute declarations
	PDIRECT3DDEVICE9			m_pd3dd;									// Direct3D device

	DWORD						m_nBlending;								// active blending type
	FLOAT						m_fBlendingTime;							// morph position
};


#endif