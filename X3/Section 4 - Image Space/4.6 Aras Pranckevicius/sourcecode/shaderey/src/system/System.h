
#ifndef __KERNEL_SYSTEM_H
#define __KERNEL_SYSTEM_H

#include <dingus/dxutils/D3DApplication.h>

namespace dingus {
	class CDeviceResourceManager;
	class IConsoleRenderingContext;
	class CD3DTextBoxConsoleRenderingContext;
	class CD3DFont;
};


class CSystem : public dingus::CD3DApplication {
public:
	CSystem();
	virtual ~CSystem();

	LRESULT msgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

protected:
	virtual HRESULT initialize();
	virtual HRESULT createDeviceObjects();
	virtual HRESULT activateDeviceObjects();
	virtual HRESULT passivateDeviceObjects();
	virtual HRESULT deleteDeviceObjects();
	virtual HRESULT performOneTime();
	virtual HRESULT shutdown();
	virtual HRESULT checkDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat );

protected:
	virtual bool appCheckDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat ) const = 0;
	virtual void appInitialize() = 0;
	virtual void appPerform() = 0;
	virtual void appShutdown() = 0;

	CD3DFont& getFont() { return *mFont; }

private:
	std::string 						mDataPath;
	CDeviceResourceManager*		mDeviceManager;
	IConsoleRenderingContext*	mStdConsoleCtx;
	CD3DTextBoxConsoleRenderingContext* mD3DConsoleCtx;
	CD3DFont*					mFont;
	bool	mAppInited;
	bool	mShowFrameDeviceStats;
};


#endif
