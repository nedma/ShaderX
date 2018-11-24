// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef D3D_ENUM_H__
#define D3D_ENUM_H__

namespace dingus {


//---------------------------------------------------------------------------

/** Vertex processing types. */
enum eVertexProcessing {
    SOFTWARE_VP,
    MIXED_VP,
    HARDWARE_VP,
    PURE_HARDWARE_VP
};


//---------------------------------------------------------------------------

struct SD3DDeviceCombo;
struct SD3DDeviceInfo;
struct SD3DAdapterInfo;

typedef std::vector<DWORD>							TDwordVector;
typedef std::vector<SD3DDeviceCombo*>				TDeviceComboVector;
typedef std::vector<D3DDISPLAYMODE>					TDisplayModeVector;
typedef std::vector<SD3DDeviceInfo*>				TDeviceInfoVector;
typedef std::vector<SD3DAdapterInfo*>				TAdapterInfoVector;


//---------------------------------------------------------------------------

/**
 *  A combo of adapter format, back buffer format, and windowed/fulscreen that
 *  is compatible with a particular D3D device and the application.
 */
struct SD3DDeviceCombo : public boost::noncopyable {
public:
	/// A depth/stencil buffer format that is incompatible with a multisample type.
	struct SDSMSConflict {
		D3DFORMAT			format;
		D3DMULTISAMPLE_TYPE type;
	};
	typedef std::vector<SDSMSConflict> TConflictVector;
public:
    int			adapterOrdinal;
    D3DDEVTYPE	deviceType;
    D3DFORMAT	adapterFormat;
    D3DFORMAT	backBufferFormat;
    bool		isWindowed;

	TDwordVector	depthStencilFormats;
	TDwordVector	multiSampleTypes;
	TDwordVector	multiSampleQualities;
	TConflictVector	conflicts;
	TDwordVector	vertexProcessings;
	TDwordVector	presentIntervals;
};


//---------------------------------------------------------------------------

/**
 *  Info on D3D device. Includes list of D3DDeviceCombos that work with it.
 */
struct SD3DDeviceInfo : public boost::noncopyable {
public:
	SD3DDeviceInfo() : adapterOrdinal(-1), deviceType(D3DDEVTYPE_HAL) { }
    ~SD3DDeviceInfo();

public:
    D3DCAPS9			caps;
	TDeviceComboVector	deviceCombos;
    int					adapterOrdinal;
    D3DDEVTYPE			deviceType;
};


//---------------------------------------------------------------------------

/**
 *  Display adapter info.
 */
struct SD3DAdapterInfo : public boost::noncopyable {
public:
    ~SD3DAdapterInfo();

public:
    int						adapterOrdinal;
    D3DADAPTER_IDENTIFIER9	adapterID;
	TDisplayModeVector		displayModes;
	TDeviceInfoVector		deviceInfos;
};


//---------------------------------------------------------------------------

/**
 *  Device confirmation callback.
 */
typedef bool (*TConfirmDeviceCallback) (
	const D3DCAPS9& caps, 
    eVertexProcessing vp,
	D3DFORMAT backBufferFormat
);


//---------------------------------------------------------------------------

/**
 *  Enumerates available D3D adapters, devices, modes, etc.
 */
class CD3DEnumeration {
public:
    CD3DEnumeration();
    ~CD3DEnumeration();

    void	setDirect3D( IDirect3D9& d3d ) { mDirect3D = &d3d; }
    
	HRESULT enumerate();

public:
	TAdapterInfoVector	mAdapterInfos;

    // The following variables can be used to limit what modes, formats, 
    // etc. are enumerated.  Set them to the values you want before calling
    // enumerate().
	TConfirmDeviceCallback	mConfirmDeviceCallback;
	int		mMinFullscreenWidth;
    int		mMinFullscreenHeight;
    int		mMinColorChannelBits; // min color bits per channel in adapter format
    int		mMinAlphaChannelBits; // min alpha bits per pixel in back buffer format
    int		mMinDepthBits;
    int		mMinStencilBits;
    bool	mUsesDepthBuffer;
    bool	mUsesMixedVP; // whether app can take advantage of mixed vp mode
    bool	mRequiresWindowed;
    bool	mRequiresFullscreen;
	TDwordVector	mAllowedFormats; // D3DFORMATs

private:
    HRESULT enumerateDevices( SD3DAdapterInfo& adapterInfo, const TDwordVector& adapterFormats );
    HRESULT enumerateDeviceCombos( SD3DDeviceInfo& deviceInfo, const TDwordVector& adapterFormats );
    void	buildDepthStencilFormats( SD3DDeviceCombo& deviceCombo );
    void	buildMultiSampleTypes( SD3DDeviceCombo& deviceCombo );
    void	buildConflicts( SD3DDeviceCombo& deviceCombo );
    void	buildVertexProcessings( const SD3DDeviceInfo& deviceInfo, SD3DDeviceCombo& deviceCombo );
    void	buildPresentIntervals( const SD3DDeviceInfo& deviceInfo, SD3DDeviceCombo& deviceCombo );

private:
    IDirect3D9*		mDirect3D;
};


}; // namespace dingus


#endif
