//
// Direct3D helper functions (for ShaderX³)
//
//	written 2004 by Ronny Burkersroda
//

#ifndef						_DIRECT3D_FUNCTIONS_H__INCLUDED_
#define						_DIRECT3D_FUNCTIONS_H__INCLUDED_


#pragma once


// header includes
#include					<d3d9.h>											// Direct3D 9


// Direct3D namespace
namespace					Direct3D
{


	// enumarations
	typedef enum				_GRAPHICCHIPTYPE									// types of graphic chips
	{
		GRAPHICCHIP_UNKNOWN			= 0x00000000,										// unknown chip
		GRAPHICCHIP_NV1				= 0x00000001,										// Riva TNT
		GRAPHICCHIP_NV5				= 0x00000002,										// Riva TNT 2
		GRAPHICCHIP_NV10			= 0x00000003,										// GeForce
		GRAPHICCHIP_NV15			= 0x00000004,										// GeForce 2
		GRAPHICCHIP_NV20			= 0x00000005,										// GeForce 3
		GRAPHICCHIP_NV25			= 0x00000006,										// GeForce 4
		GRAPHICCHIP_NV30			= 0x00000007,										// GeForce FX 5200, 5600, 5800
		GRAPHICCHIP_NV35			= 0x00000008,										// GeForce FX 5900
		GRAPHICCHIP_NVIDIA			= 0x000000FF,										// NVidia chip
		GRAPHICCHIP_ATI				= 0x0000FF00,										// ATI chip
		GRAPHICCHIP_FAILURE			= 0xFFFFFFFF										// failure determining the chip
	}
	GRAPHICCHIPTYPE;


	// global functions
	HRESULT						InitializeDevice(									// initialize Direct3D and its device
		PDIRECT3DDEVICE9*			ppd3ddDevice,										// pointer to store device pointer to
		D3DPRESENT_PARAMETERS*		pprspSetting,										// pointer to get present parameters from
		DWORD						nAdapter			= D3DADAPTER_DEFAULT,			// number of the graphic card
		D3DDEVTYPE					devtType			= D3DDEVTYPE_HAL,				// device type
		BOOL						bMultiThreaded		= FALSE							// enable multi threading
		);
    HRESULT						BuildPresentParameters(								// build present parameters from given arguments
		D3DPRESENT_PARAMETERS*		pprspSetting,										// pointer to store present parameters to
		HWND						hwndWindow,											// handle to the window
		DWORD						nAdapter			= D3DADAPTER_DEFAULT,			// number of the graphic card
		D3DDEVTYPE					devtType			= D3DDEVTYPE_HAL,				// device type
		DWORD						nWidth				= 0,							// window or screen width
		DWORD						nHeight				= 0,							// window or screen height
		DWORD						nRefreshRate		= D3DPRESENT_RATE_DEFAULT,		// full screen refresh rate
		BOOL						bWindowed			= TRUE,							// flag to display windowed
		D3DFORMAT					fmtDisplay			= D3DFMT_UNKNOWN,				// window or screen format
		D3DFORMAT					fmtDepthStencil		= D3DFMT_UNKNOWN,				// auto depth stencil format
		DWORD						nBackBufferQuantity	= 1,							// number of back buffers
		DWORD						nFlipInterval		= D3DPRESENT_INTERVAL_DEFAULT,	// presentation interval
		D3DMULTISAMPLE_TYPE			mstMultiSample		= D3DMULTISAMPLE_FORCE_DWORD,	// multi sample type
		DWORD						nMultiSampleQuality	= 0,							// multi sample quality
		D3DSWAPEFFECT				swefSwapEffect		= D3DSWAPEFFECT_DISCARD,		// swap effect for flipping
		DWORD						nFlagsValue			= 0								// flags
		);
    HRESULT						UpdatePresentParameters(							// update existing present parameters
		D3DPRESENT_PARAMETERS*		pprspSetting,										// pointer to existing present parameters
		DWORD						nWidth				= 0,							// window or screen width
		DWORD						nHeight				= 0								// window or screen height
		);


}


#endif