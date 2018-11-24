//
// Direct3D helper functions (for ShaderX³)
//
//	written 2004 by Ronny Burkersroda
//

// header includes
#include					"Direct3D functions.h"						// Direct3D functions
#include					<d3dx9.h>									// Direct3D 8 extensions


// Direct3D namespace
namespace					Direct3D
{


	// ************************
	// *** global functions ***
	// ************************

	GRAPHICCHIPTYPE GetAdapterChip( DWORD nAdapter, PUINT64 pnDriverVersion )
	{
		D3DADAPTER_IDENTIFIER9	adid;
		PDIRECT3D9				pd3d;

		// create Direct3D object
		if( !( pd3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return GRAPHICCHIP_FAILURE;

		// get adapter identifier
		if( FAILED( pd3d->GetAdapterIdentifier( nAdapter, 0, &adid ) ) )
		{
			// release Direct3D object
			pd3d->Release();
			return GRAPHICCHIP_FAILURE;
		}

		// release Direct3D object
		pd3d->Release();

		// store driver version
		if( pnDriverVersion )
			*pnDriverVersion = adid.DriverVersion.QuadPart;

		// process by vendor ID
		switch( adid.VendorId )
		{
		// NVidia ID
		case 0x10DE:

			// check for TNT
			if( adid.DeviceId == 0x020 )
				return GRAPHICCHIP_NV1;

			// check for TNT 2
			if( ( adid.DeviceId >= 0x028 ) && ( adid.DeviceId <= 0x0FF ) )
				return GRAPHICCHIP_NV5;

			// check for GeForce
			if( ( adid.DeviceId >= 0x100 ) && ( adid.DeviceId <= 0x10F ) )
				return GRAPHICCHIP_NV10;

			// check for GeForce 2 and 4 MX, GO
			if( ( adid.DeviceId >= 0x110 ) && ( adid.DeviceId <= 0x1FF ) )
				return GRAPHICCHIP_NV15;

			// check for GeForce 3
			if( ( adid.DeviceId >= 0x200 ) && ( adid.DeviceId <= 0x24F ) )
				return GRAPHICCHIP_NV20;

			// check for GeForce 4
			if( ( adid.DeviceId >= 0x250 ) && ( adid.DeviceId <= 0x2FF ) )
				return GRAPHICCHIP_NV25;

			// check for GeForce FX 5200, 5600 and 5800
			if( ( adid.DeviceId >= 0x300 ) && ( adid.DeviceId <= 0x32F ) )
				return GRAPHICCHIP_NV30;

			// check for GeForce FX 5900
			if( ( adid.DeviceId >= 0x330 ) && ( adid.DeviceId <= 0x34F ) )
				return GRAPHICCHIP_NV35;

			return GRAPHICCHIP_NVIDIA;

		// ATI ID
		case 0x1002:

			return GRAPHICCHIP_ATI;
		}

		return GRAPHICCHIP_UNKNOWN;
	}

	HRESULT InitializeDevice( PDIRECT3DDEVICE9* ppd3dd, D3DPRESENT_PARAMETERS* pprspSetting, DWORD nAdapter, D3DDEVTYPE devtType, BOOL bMultiThreaded )
	{
		D3DCAPS9				caps;
		PDIRECT3D9				pd3d;
		HRESULT					hResult;
		DWORD					nDeviceFlags;

		// check parameters
		if( !pprspSetting || !ppd3dd )
			return D3DERR_INVALIDCALL;

		// create Direct3D object
		if( !( pd3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return D3DERR_DRIVERINTERNALERROR;

		// get device capabilities
		if( FAILED( hResult = pd3d->GetDeviceCaps( nAdapter, devtType, &caps ) ) )
			goto FunctionEnd;

		// set vertex processor
		if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) && ( caps.MaxVertexBlendMatrices >= 4 ) )
			nDeviceFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			nDeviceFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		// set multi threading
		if( bMultiThreaded )
			nDeviceFlags |= D3DCREATE_MULTITHREADED;

		// create Direct3D device
		hResult = pd3d->CreateDevice( nAdapter, devtType, pprspSetting->hDeviceWindow, nDeviceFlags, pprspSetting, ppd3dd );

	FunctionEnd:
		// release Direct3D object
		pd3d->Release();

		return hResult;
	}

	HRESULT BuildPresentParameters( D3DPRESENT_PARAMETERS* pprspSetting, HWND hwndWindow, DWORD nAdapter, D3DDEVTYPE devtType, DWORD nWidth, DWORD nHeight, DWORD nRefreshRate, BOOL bWindowed, D3DFORMAT fmtDisplay, D3DFORMAT fmtDepthStencil, DWORD nBackBufferQuantity, DWORD nFlipInterval, D3DMULTISAMPLE_TYPE mstMultiSample, DWORD nMultiSampleQuality, D3DSWAPEFFECT swefSwapEffect, DWORD nFlagsValue )
	{
		D3DPRESENT_PARAMETERS	prsp;
		D3DDISPLAYMODE			dspm;
		RECT					rect;
		PDIRECT3D9				pd3d;
		HRESULT					hResult;
		DWORD					nCounter;

		// check parameters
		if( !hwndWindow || !pprspSetting )
			return D3DERR_INVALIDCALL;

		// create Direct3D object
		if( !( pd3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return D3DERR_DRIVERINTERNALERROR;

		// get current display mode
		if( FAILED( pd3d->GetAdapterDisplayMode( nAdapter, &dspm ) ) )
		{
			hResult = D3DERR_DRIVERINTERNALERROR;
			goto FunctionEnd;
		}

		// set display format
		if( fmtDisplay == D3DFMT_UNKNOWN )
			fmtDisplay = dspm.Format;

		// check device type
		if( FAILED( hResult = pd3d->CheckDeviceType( nAdapter, devtType, fmtDisplay, fmtDisplay, bWindowed ) ) )
		{
			// check for alternative format
			if( fmtDisplay == D3DFMT_R8G8B8 )
				fmtDisplay = D3DFMT_X8R8G8B8;

			// check device type again
			if( FAILED( hResult = pd3d->CheckDeviceType( nAdapter, devtType, fmtDisplay, fmtDisplay, bWindowed ) ) )
				goto FunctionEnd;
		}

		// check for undefined multi sample use
		if( mstMultiSample == D3DMULTISAMPLE_FORCE_DWORD )
		{
			// test non-maskable sampling type
			if( SUCCEEDED( pd3d->CheckDeviceMultiSampleType( nAdapter, devtType, fmtDisplay, bWindowed, D3DMULTISAMPLE_NONMASKABLE, &nMultiSampleQuality ) ) )
				mstMultiSample = D3DMULTISAMPLE_NONMASKABLE;

			if( mstMultiSample == D3DMULTISAMPLE_FORCE_DWORD )
			{
				nCounter = 16;

				// test for multisampling
				while( ( mstMultiSample == D3DMULTISAMPLE_FORCE_DWORD ) && ( nCounter >= 2 ) )
				{
					// check if multisample type can be used
					if( SUCCEEDED( pd3d->CheckDeviceMultiSampleType( nAdapter, devtType, fmtDisplay, bWindowed, (D3DMULTISAMPLE_TYPE) nCounter, &nMultiSampleQuality ) ) )
						mstMultiSample = (D3DMULTISAMPLE_TYPE) nCounter;

					--nCounter;
				}
			}

			// no multisample can be used?
			if( mstMultiSample == D3DMULTISAMPLE_FORCE_DWORD )
				mstMultiSample = D3DMULTISAMPLE_NONE;
			else
				nMultiSampleQuality >>= 1;
		}
		// test multi-sampling type
		else if( FAILED( pd3d->CheckDeviceMultiSampleType( nAdapter, devtType, fmtDisplay, bWindowed, mstMultiSample, &prsp.MultiSampleQuality ) ) )
		{
			// change multi sample types
			if( mstMultiSample == D3DMULTISAMPLE_NONMASKABLE )
			{
				mstMultiSample		= (D3DMULTISAMPLE_TYPE)( nMultiSampleQuality + D3DMULTISAMPLE_NONE );
				nMultiSampleQuality	= 0;
			}
			else
			{
				nMultiSampleQuality	= mstMultiSample;
				mstMultiSample		= D3DMULTISAMPLE_NONMASKABLE;
			}

			// check for multi sample type
			if( FAILED( pd3d->CheckDeviceMultiSampleType( nAdapter, devtType, fmtDisplay, bWindowed, mstMultiSample, &nMultiSampleQuality ) ) )
			{
				mstMultiSample		= D3DMULTISAMPLE_NONE;
				nMultiSampleQuality	= 0;
			}
		}

		// check for depth stencil buffer
		if( fmtDepthStencil != D3DFMT_FORCE_DWORD )
		{
			// check defined depth stencil format and unset it if it is not supported
			if( ( fmtDepthStencil != D3DFMT_UNKNOWN ) && FAILED( pd3d->CheckDepthStencilMatch( nAdapter, devtType, fmtDisplay, fmtDisplay, fmtDepthStencil ) ) )
				fmtDepthStencil = D3DFMT_UNKNOWN;

			// check for undefined depth stencil format
			if( fmtDepthStencil == D3DFMT_UNKNOWN )
			{
				nCounter = 70;

				// test for depth stencil format
				while( ( fmtDepthStencil == D3DFMT_UNKNOWN ) && ( nCounter < 256 ) )
				{
					// check if format can be used as depth buffer
					if( SUCCEEDED( pd3d->CheckDeviceFormat( nAdapter, devtType, fmtDisplay, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, (D3DFORMAT) nCounter ) ) && SUCCEEDED( pd3d->CheckDepthStencilMatch( nAdapter, devtType, fmtDisplay, fmtDisplay, (D3DFORMAT) nCounter ) ) && ( ( mstMultiSample == D3DMULTISAMPLE_NONE ) || ( ( nCounter != D3DFMT_D16_LOCKABLE ) && ( nCounter != D3DFMT_D32F_LOCKABLE ) ) ) )
						fmtDepthStencil = (D3DFORMAT) nCounter;

					++nCounter;
				}

				// no depth stencil format found?
				if( fmtDepthStencil == D3DFMT_UNKNOWN )
				{
					hResult = D3DERR_INVALIDCALL;
					goto FunctionEnd;
				}
			}
		}

		// get window size
		if( !nWidth || !nHeight )
			GetClientRect( hwndWindow, &rect );

		// get back buffer width
		if( !nWidth ) if( bWindowed )
			nWidth	= rect.right;
		else
			nWidth	= dspm.Width;

		// get back buffer height
		if( !nHeight ) if( bWindowed )
			nHeight	= rect.bottom;
		else
			nHeight	= dspm.Height;

		if( !bWindowed )
		{
			// check if fullscreen format exists
			for( nCounter = pd3d->GetAdapterModeCount( nAdapter, fmtDisplay ); nCounter--; ) if( SUCCEEDED( pd3d->EnumAdapterModes( nAdapter, fmtDisplay, nCounter, &dspm ) ) && ( dspm.Width == nWidth ) && ( dspm.Height == nHeight ) )
				break;

			if( nCounter == 0xFFFFFFFF )
			{
				hResult = E_INVALIDARG;
				goto FunctionEnd;
			}
		}

		// set back buffer format and count
		prsp.BackBufferWidth			= nWidth;
		prsp.BackBufferHeight			= nHeight;
		prsp.BackBufferFormat			= fmtDisplay;
		prsp.BackBufferCount			= nBackBufferQuantity;

		// set window properties
		prsp.hDeviceWindow				= hwndWindow;
		prsp.Windowed					= bWindowed;

		// set auto depth stencil buffer
		prsp.AutoDepthStencilFormat		= fmtDepthStencil;
		prsp.EnableAutoDepthStencil		= ( fmtDepthStencil != D3DFMT_FORCE_DWORD );

		// set full screen properties
		prsp.FullScreen_RefreshRateInHz	= nRefreshRate;
		prsp.PresentationInterval		= nFlipInterval;

		// set swap effect, multi sample type and flags
		prsp.SwapEffect					= swefSwapEffect;
		prsp.MultiSampleType			= mstMultiSample;
		prsp.MultiSampleQuality			= nMultiSampleQuality;
		prsp.Flags						= nFlagsValue;

		// copy present parameters
		*pprspSetting = prsp;

	FunctionEnd:
		// release Direct3D object
		pd3d->Release();

		return hResult;
	}

	HRESULT UpdatePresentParameters( D3DPRESENT_PARAMETERS* pprspSetting, DWORD nWidth, DWORD nHeight )
	{
		RECT rect;

		// get window size
		if( !nWidth || !nHeight )
			GetClientRect( pprspSetting->hDeviceWindow, &rect );

		// set back buffer width
		if( nWidth )
			pprspSetting->BackBufferWidth	= nWidth;
		else if( pprspSetting->Windowed )
			pprspSetting->BackBufferWidth	= rect.right;

		// set back buffer height
		if( nHeight )
			pprspSetting->BackBufferHeight	= nHeight;
		else if( pprspSetting->Windowed )
			pprspSetting->BackBufferHeight	= rect.bottom;

		return S_OK;
	}


}
