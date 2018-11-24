// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"

#include "D3DEnumeration.h"

using namespace dingus;


//---------------------------------------------------------------------------

/**
 *  Returns number of color channel bits for D3DFORMAT.
 */
static int gGetColorChannelBits( D3DFORMAT fmt ) {
	switch( fmt ) {
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
		return 10;
	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return 8;
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		return 5;
	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
		return 4;
	case D3DFMT_R3G3B2:
	case D3DFMT_A8R3G3B2:
		return 2;
	default:
		return 0;
	}
};


//---------------------------------------------------------------------------

/**
 *  Returns number of alpha channel bits for D3DFORMAT.
 */
static int gGetAlphaChannelBits( D3DFORMAT fmt ) {
	switch( fmt ) {
	case D3DFMT_R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R3G3B2:
	case D3DFMT_X4R4G4B4:
		return 0;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A8R3G3B2:
		return 8;
	case D3DFMT_A1R5G5B5:
		return 1;
	case D3DFMT_A4R4G4B4:
		return 4;
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
		return 2;
	default:
		return 0;
	}
};


//---------------------------------------------------------------------------

/**
 *  Returns number of depth bits for D3DFORMAT.
 */
static int gGetDepthBits( D3DFORMAT fmt ) {
	switch( fmt ) {
	case D3DFMT_D16:
		return 16;
	case D3DFMT_D15S1:
		return 15;
	case D3DFMT_D24X8:
	case D3DFMT_D24S8:
	case D3DFMT_D24X4S4:
		return 24;
	case D3DFMT_D32:
		return 32;
	default:
		return 0;
	}
};


//---------------------------------------------------------------------------

/**
 *  Returns number of stencil bits for D3DFORMAT.
 */
static int gGetStencilBits( D3DFORMAT fmt ) {
	switch( fmt ) {
	case D3DFMT_D16:
	case D3DFMT_D24X8:
	case D3DFMT_D32:
		return 0;
	case D3DFMT_D15S1:
		return 1;
	case D3DFMT_D24S8:
		return 8;
	case D3DFMT_D24X4S4:
		return 4;
	default:
		return 0;
	}
};


//---------------------------------------------------------------------------
//  SD3DAdapterInfo
//---------------------------------------------------------------------------

SD3DAdapterInfo::~SD3DAdapterInfo()
{
	stl_utils::wipe( deviceInfos );
}

//---------------------------------------------------------------------------
// SD3DDeviceInfo
//---------------------------------------------------------------------------

SD3DDeviceInfo::~SD3DDeviceInfo()
{
	stl_utils::wipe( deviceCombos );
}



//---------------------------------------------------------------------------
// CD3DEnumeration
//---------------------------------------------------------------------------


CD3DEnumeration::CD3DEnumeration()
:	mMinFullscreenWidth(640),
	mMinFullscreenHeight(480),
	mMinColorChannelBits(5),
	mMinAlphaChannelBits(0),
	mMinDepthBits(15),
	mMinStencilBits(0),
	mUsesDepthBuffer(false),
	mUsesMixedVP(false),
	mRequiresWindowed(false),
	mRequiresFullscreen(false)
{
}


CD3DEnumeration::~CD3DEnumeration()
{
	stl_utils::wipe( mAdapterInfos );
}


//-----------------------------------------------------------------------------

// Used to sort display modes.
/*
bool operator< ( const D3DDISPLAYMODE& dm1, const D3DDISPLAYMODE& dm2 )
{
	if (dm1.Width < dm2.Width)
		return true;
	if (dm1.Height < dm2.Height)
		return true;
	if (dm1.Format < dm2.Format)
		return true;
	if (dm1.RefreshRate < dm2.RefreshRate)
		return true;
	return false;
};
*/


HRESULT CD3DEnumeration::enumerate()
{
	HRESULT hr;
	
	if( !mDirect3D )
		return E_FAIL;
	
	stl_utils::wipe( mAdapterInfos );
	
	mAllowedFormats.clear();
	mAllowedFormats.push_back( D3DFMT_X8R8G8B8 );
	mAllowedFormats.push_back( D3DFMT_X1R5G5B5 );
	mAllowedFormats.push_back( D3DFMT_R5G6B5 );
	mAllowedFormats.push_back( D3DFMT_A2R10G10B10 );

	//
	// get all adapters

	int adapterCount = mDirect3D->GetAdapterCount();
	for( int adOrd = 0; adOrd < adapterCount; ++adOrd ) {
		SD3DAdapterInfo *adapterInfo = new SD3DAdapterInfo();
		if( adapterInfo == NULL ) return E_OUTOFMEMORY;

		adapterInfo->adapterOrdinal = adOrd;
		mDirect3D->GetAdapterIdentifier( adOrd, 0, &adapterInfo->adapterID );
		
		//
		// get list of all display modes on this adapter.  
		// also build a temporary list of all display adapter formats.

		TDwordVector adapterFormatList; // D3DFORMAT
		
		for( int ifmt = 0; ifmt < mAllowedFormats.size(); ++ifmt ) {
			D3DFORMAT format = (D3DFORMAT)mAllowedFormats[ifmt];
			int modeCount = mDirect3D->GetAdapterModeCount( adOrd, format );
			for( int mode = 0; mode < modeCount; ++mode ) {
				D3DDISPLAYMODE dm;
				mDirect3D->EnumAdapterModes( adOrd, format, mode, &dm );
				if( dm.Width < mMinFullscreenWidth || 
					dm.Height < mMinFullscreenHeight ||
					gGetColorChannelBits(dm.Format) < mMinColorChannelBits )
				{
					continue;
				}
				adapterInfo->displayModes.push_back( dm );
				if( std::find(adapterFormatList.begin(),adapterFormatList.end(),dm.Format) == adapterFormatList.end() )
					adapterFormatList.push_back( dm.Format );
			}
		}

		// sort displaymode list
		// TBD: no sort, causes some problems on some weird computers
		//std::sort( adapterInfo->displayModes.begin(), adapterInfo->displayModes.end() );
		
		// get info for each device on this adapter
		if( FAILED( hr = enumerateDevices( *adapterInfo, adapterFormatList ) ) ) {
			delete adapterInfo;
			return hr;
		}

		// if at least one device on this adapter is available and compatible
		// with the app, add adapterInfo to the list
		if( adapterInfo->deviceInfos.empty() )
			delete adapterInfo;
		else
			mAdapterInfos.push_back( adapterInfo );
	}

	return S_OK;
}




HRESULT CD3DEnumeration::enumerateDevices( SD3DAdapterInfo& adapterInfo, const TDwordVector& adapterFormats )
{
	const D3DDEVTYPE devTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_SW, D3DDEVTYPE_REF };
	const int devTypeCount = sizeof(devTypes) / sizeof(devTypes[0]);
	
	HRESULT hr;
	
	for( int idt = 0; idt < devTypeCount; ++idt ) {
		SD3DDeviceInfo* deviceInfo = new SD3DDeviceInfo();
		if( deviceInfo == NULL ) return E_OUTOFMEMORY;

		deviceInfo->adapterOrdinal = adapterInfo.adapterOrdinal;
		deviceInfo->deviceType = devTypes[idt];
		if( FAILED( mDirect3D->GetDeviceCaps( adapterInfo.adapterOrdinal, deviceInfo->deviceType, &deviceInfo->caps ) ) ) {
			delete deviceInfo;
			continue;
		}
		
		// get info for each device combo on this device
		if( FAILED( hr = enumerateDeviceCombos( *deviceInfo, adapterFormats ) ) ) {
			delete deviceInfo;
			return hr;
		}
		
		// if at least one combo for is found, add the deviceInfo to the list
		if( deviceInfo->deviceCombos.empty() ) {
			delete deviceInfo;
			continue;
		}
		adapterInfo.deviceInfos.push_back( deviceInfo );
	}
	return S_OK;
}




HRESULT CD3DEnumeration::enumerateDeviceCombos( SD3DDeviceInfo& deviceInfo, const TDwordVector& adapterFormats )
{
	const D3DFORMAT bbufferFormats[] = {
		D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10,
		D3DFMT_R5G6B5, D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5
	};
	const int bbufferFormatCount = sizeof(bbufferFormats) / sizeof(bbufferFormats[0]);
	
	bool isWindowedArray[] = { false, true };
	
	//
	// see which adapter formats are supported by this device
	for( int iaf = 0; iaf < adapterFormats.size(); ++iaf ) {
		D3DFORMAT format = (D3DFORMAT)adapterFormats[iaf];
		for( int ibbf = 0; ibbf < bbufferFormatCount; ibbf++ ) {
			D3DFORMAT bbufferFormat = bbufferFormats[ibbf];
			if( gGetAlphaChannelBits(bbufferFormat) < mMinAlphaChannelBits )
				continue;
			for( int iiw = 0; iiw < 2; ++iiw ) {
				bool isWindowed = isWindowedArray[iiw];
				if( !isWindowed && mRequiresWindowed )
					continue;
				if( isWindowed && mRequiresFullscreen )
					continue;
				if( FAILED( mDirect3D->CheckDeviceType(
					deviceInfo.adapterOrdinal, deviceInfo.deviceType, 
					format, bbufferFormat, isWindowed ) ) )
				{
					continue;
				}

				// Here, we have an adapter/device/adapterformat/backbufferformat/iswindowed
				// combo that is supported by the system. We still need to
				// confirm that it's compatible with the app, and find one or
				// more suitable depth/stencil buffer format, multisample type,
				// vertex processing type, and present interval.
				SD3DDeviceCombo* devCombo = new SD3DDeviceCombo();
				if( devCombo == NULL ) return E_OUTOFMEMORY;

				devCombo->adapterOrdinal = deviceInfo.adapterOrdinal;
				devCombo->deviceType = deviceInfo.deviceType;
				devCombo->adapterFormat = format;
				devCombo->backBufferFormat = bbufferFormat;
				devCombo->isWindowed = isWindowed;
				if( mUsesDepthBuffer ) {
					buildDepthStencilFormats( *devCombo );
					if( devCombo->depthStencilFormats.empty() ) {
						delete devCombo;
						continue;
					}
				}
				buildMultiSampleTypes( *devCombo );
				if( devCombo->multiSampleTypes.empty() ) {
					delete devCombo;
					continue;
				}
				buildConflicts( *devCombo );
				buildVertexProcessings( deviceInfo, *devCombo );
				if( devCombo->vertexProcessings.empty() ) {
					delete devCombo;
					continue;
				}
				buildPresentIntervals( deviceInfo, *devCombo );
				
				deviceInfo.deviceCombos.push_back( devCombo );
			}
		}
	}
	
	return S_OK;
}



void CD3DEnumeration::buildDepthStencilFormats( SD3DDeviceCombo& devCombo )
{
	const D3DFORMAT dsFormats[] = {
		D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D32,
	};
	const int dsFormatCount = sizeof(dsFormats) / sizeof(dsFormats[0]);
	
	for( int idsf = 0; idsf < dsFormatCount; ++idsf ) {
		D3DFORMAT format = dsFormats[idsf];
		if( gGetDepthBits(format) < mMinDepthBits )
			continue;
		if( gGetStencilBits(format) < mMinStencilBits )
			continue;
		if( SUCCEEDED( mDirect3D->CheckDeviceFormat(
			devCombo.adapterOrdinal, devCombo.deviceType, devCombo.adapterFormat, 
			D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, format ) ) )
		{
			if( SUCCEEDED( mDirect3D->CheckDepthStencilMatch(
				devCombo.adapterOrdinal, devCombo.deviceType, devCombo.adapterFormat, 
				devCombo.backBufferFormat, format ) ) )
			{
				devCombo.depthStencilFormats.push_back( format );
			}
		}
	}
}


void CD3DEnumeration::buildMultiSampleTypes( SD3DDeviceCombo& devCombo )
{
	const D3DMULTISAMPLE_TYPE msTypes[] = { 
		D3DMULTISAMPLE_NONE,
		D3DMULTISAMPLE_NONMASKABLE,
		D3DMULTISAMPLE_2_SAMPLES,
		D3DMULTISAMPLE_3_SAMPLES,
		D3DMULTISAMPLE_4_SAMPLES,
		D3DMULTISAMPLE_5_SAMPLES,
		D3DMULTISAMPLE_6_SAMPLES,
		D3DMULTISAMPLE_7_SAMPLES,
		D3DMULTISAMPLE_8_SAMPLES,
		D3DMULTISAMPLE_9_SAMPLES,
		D3DMULTISAMPLE_10_SAMPLES,
		D3DMULTISAMPLE_11_SAMPLES,
		D3DMULTISAMPLE_12_SAMPLES,
		D3DMULTISAMPLE_13_SAMPLES,
		D3DMULTISAMPLE_14_SAMPLES,
		D3DMULTISAMPLE_15_SAMPLES,
		D3DMULTISAMPLE_16_SAMPLES,
	};
	const int msTypeCount = sizeof(msTypes) / sizeof(msTypes[0]);
	
	for( int imst = 0; imst < msTypeCount; ++imst ) {
		D3DMULTISAMPLE_TYPE	msType = msTypes[imst];
		DWORD msQuality;
		if( SUCCEEDED( mDirect3D->CheckDeviceMultiSampleType(
			devCombo.adapterOrdinal, devCombo.deviceType, 
			devCombo.backBufferFormat, devCombo.isWindowed,
			msType, &msQuality ) ) )
		{
			devCombo.multiSampleTypes.push_back( msType );
			devCombo.multiSampleQualities.push_back( msQuality );
		}
	}
}


void CD3DEnumeration::buildConflicts( SD3DDeviceCombo& devCombo )
{
	
	for( int ids = 0; ids < devCombo.depthStencilFormats.size(); ++ids ) {
		D3DFORMAT format = (D3DFORMAT)devCombo.depthStencilFormats[ids];
		for( int ims = 0; ims < devCombo.multiSampleTypes.size(); ++ims ) {
			D3DMULTISAMPLE_TYPE msType = (D3DMULTISAMPLE_TYPE)devCombo.multiSampleTypes[ims];
			if( FAILED( mDirect3D->CheckDeviceMultiSampleType(
				devCombo.adapterOrdinal, devCombo.deviceType,
				format, devCombo.isWindowed, msType, NULL ) ) )
			{
				SD3DDeviceCombo::SDSMSConflict conflict;
				conflict.format = format;
				conflict.type = msType;
				devCombo.conflicts.push_back( conflict );
			}
		}
	}
};



void CD3DEnumeration::buildVertexProcessings( const SD3DDeviceInfo& deviceInfo, SD3DDeviceCombo& devCombo )
{
	if( deviceInfo.caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) {
		if( deviceInfo.caps.DevCaps & D3DDEVCAPS_PUREDEVICE ) {
			if( !mConfirmDeviceCallback ||
				mConfirmDeviceCallback( deviceInfo.caps, PURE_HARDWARE_VP, devCombo.backBufferFormat ) )
			{
				devCombo.vertexProcessings.push_back( PURE_HARDWARE_VP );
			}
		}
		if( !mConfirmDeviceCallback ||
			mConfirmDeviceCallback( deviceInfo.caps, HARDWARE_VP, devCombo.backBufferFormat ) )
		{
			devCombo.vertexProcessings.push_back( HARDWARE_VP );
		}
		if( mUsesMixedVP && (!mConfirmDeviceCallback ||
			mConfirmDeviceCallback( deviceInfo.caps, MIXED_VP, devCombo.backBufferFormat ) ) )
		{
			devCombo.vertexProcessings.push_back( MIXED_VP );
		}
	}

	if( !mConfirmDeviceCallback ||
		mConfirmDeviceCallback( deviceInfo.caps, SOFTWARE_VP, devCombo.backBufferFormat ) )
	{
		devCombo.vertexProcessings.push_back( SOFTWARE_VP );
	}
}



void CD3DEnumeration::buildPresentIntervals( const SD3DDeviceInfo& deviceInfo, SD3DDeviceCombo& devCombo )
{
	const int pis[] = { 
		D3DPRESENT_INTERVAL_IMMEDIATE,
		D3DPRESENT_INTERVAL_DEFAULT,
		D3DPRESENT_INTERVAL_ONE,
		D3DPRESENT_INTERVAL_TWO,
		D3DPRESENT_INTERVAL_THREE,
		D3DPRESENT_INTERVAL_FOUR,
	};
	const int piCount = sizeof(pis) / sizeof(pis[0]);
	
	for( int ipi = 0; ipi < piCount; ++ipi ) {
		int pi = pis[ipi];
		if( devCombo.isWindowed ) {
			if( pi == D3DPRESENT_INTERVAL_TWO ||
				pi == D3DPRESENT_INTERVAL_THREE ||
				pi == D3DPRESENT_INTERVAL_FOUR )
			{
				// These intervals are not supported in windowed mode.
				continue;
			}
		}
		// D3DPRESENT_INTERVAL_DEFAULT is zero, so you
		// can't do a caps check for it -- it is always available.
		if( pi == D3DPRESENT_INTERVAL_DEFAULT || (deviceInfo.caps.PresentationIntervals & pi) )
			devCombo.presentIntervals.push_back( pi );
	}
}
