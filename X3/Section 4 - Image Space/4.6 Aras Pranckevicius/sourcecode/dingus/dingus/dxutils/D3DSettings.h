// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef D3D_SETTINGS_H__
#define D3D_SETTINGS_H__

#include "D3DEnumeration.h"

namespace dingus {

class CD3DApplication;


//---------------------------------------------------------------------------

/**
 *  Current D3D settings: adapter, device, mode, formats, etc.
 */
class CD3DSettings {
public:
	enum eMode { WINDOWED = 0, FULLSCREEN, MODE_COUNT };

	struct SSettings {
		const SD3DAdapterInfo*	adapterInfo;
		const SD3DDeviceInfo*	deviceInfo;
		const SD3DDeviceCombo*	deviceCombo;

		D3DDISPLAYMODE		displayMode;
		D3DFORMAT			depthStencilFormat;
		D3DMULTISAMPLE_TYPE multisampleType;
		DWORD				multisampleQuality;
		eVertexProcessing	vertexProcessing;
		UINT				presentInterval;
	};
public:
	eMode			mMode;

	SSettings		mSettings[MODE_COUNT];

	int		mWindowedWidth;
	int		mWindowedHeight;

public:
    const SD3DAdapterInfo& getAdapterInfo() const { return *mSettings[mMode].adapterInfo; }
    const SD3DDeviceInfo& getDeviceInfo() const { return *mSettings[mMode].deviceInfo; }
    const SD3DDeviceCombo& getDeviceCombo() const { return *mSettings[mMode].deviceCombo; }

    int			getAdapterOrdinal() const { return getDeviceCombo().adapterOrdinal; }
    D3DDEVTYPE	getDevType() const { return getDeviceCombo().deviceType; }
    D3DFORMAT	getBackBufferFormat() { return getDeviceCombo().backBufferFormat; }

    const D3DDISPLAYMODE&	getDisplayMode() const { return mSettings[mMode].displayMode; }
    D3DDISPLAYMODE&	getDisplayMode() { return mSettings[mMode].displayMode; }
    void			setDisplayMode( D3DDISPLAYMODE dm ) { mSettings[mMode].displayMode = dm; }

    D3DFORMAT		getDepthStencilFormat() const { return mSettings[mMode].depthStencilFormat; }
    void			setDepthStencilFormat( D3DFORMAT fmt ) { mSettings[mMode].depthStencilFormat = fmt; }

    D3DMULTISAMPLE_TYPE	getMultiSampleType() const { return mSettings[mMode].multisampleType; }
    void				setMultiSampleType( D3DMULTISAMPLE_TYPE ms ) { mSettings[mMode].multisampleType = ms; }

    DWORD	getMultiSampleQuality() const { return mSettings[mMode].multisampleQuality; }
    void	setMultiSampleQuality( DWORD q ) { mSettings[mMode].multisampleQuality = q; }

    eVertexProcessing	getVertexProcessing() const { return mSettings[mMode].vertexProcessing; }
    void				setVertexProcessing( eVertexProcessing vp ) { mSettings[mMode].vertexProcessing = vp; }

    int		getPresentInterval() const { return mSettings[mMode].presentInterval; }
    void	setPresentInterval( int pi ) { mSettings[mMode].presentInterval = pi; }
};


//---------------------------------------------------------------------------

/**
 *  Dialog box to allow change the D3D settings.
 */
class CD3DSettingsDialog {
public:
    CD3DSettingsDialog( const CD3DEnumeration& enumeration, const CD3DSettings& settings );
    INT_PTR		showDialog( HINSTANCE instance, HWND hwndParent, CD3DApplication& application );
    INT_PTR		dialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
    void		getFinalSettings( CD3DSettings& settings ) { settings = mSettings; }

private:
    HWND				mDlg;
    const CD3DEnumeration*	mEnumeration;
    CD3DSettings		mSettings;

private:
    // ComboBox helper functions
    void	comboBoxAdd( int id, const void* pData, const TCHAR* pstrDesc );
    void	comboBoxSelect( int id, const void* pData );
    const void*	comboBoxSelected( int id );
    bool	comboBoxSomethingSelected( int id );
    int		comboBoxCount( int id );
    void	comboBoxSelectIndex( int id, int index );
    void	comboBoxClear( int id );
    bool	comboBoxContainsText( int id, const TCHAR* pstrText );

    void	adapterChanged();
    void	deviceChanged();
    void	windowedFullscreenChanged();
    void	adapterFormatChanged();
    void	resolutionChanged();
    void	refreshRateChanged();
    void	backBufferFormatChanged();
    void	depthStencilBufferChanged();
    void	multiSampleTypeChanged();
    void	multiSampleQualityChanged();
    void	vertexProcessingChanged();
    void	presentIntervalChanged();
};


}; // namespace dingus


#endif



