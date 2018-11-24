//
// Direct3D Sample base class
//
//	written 2004 by Ronny Burkersroda
//

#ifndef						_SAMPLE_BASE_CLASS_H__INCLUDED_
#define						_SAMPLE_BASE_CLASS_H__INCLUDED_


#pragma once


// header includes
#include					<d3d9.h>									// Direct3D
#include					<crtdbg.h>									// memory debugging


// class definitions
class						CD3DSampleBase;								// Direct3D sample class base


// pointer definitions
typedef						CD3DSampleBase								// Direct3D sample base class
								*PD3DSAMPLEBASE,
								*LPD3DSAMPLEBASE;


//
// base class for the Direct3D sample applications
//	is to derive sample classes from.
//
class						CD3DSampleBase								// Direct3D base class
{
#ifdef						CreateWindowA
#undef						CreateWindowA
#endif

protected:
	typedef enum				_DEVICE3DFLAGS
	{
		DEVICE3D_NONE				= 0x00000000,
		DEVICE3D_RESET				= 0x00000001,
		DEVICE3D_DISABLERENDERING	= 0x00000002,

		DEVICE3D_FORCE_DWORD		= 0xFFFFFFFF
	}
	DEVICE3DFLAGS;

public:
	// method declarations
	CD3DSampleBase(															// standard constructor
		);
	virtual ~CD3DSampleBase(												// standard destructor
		);

	virtual INT					Main(										// main function of the object
		HINSTANCE					hmodInstance,								// handle of the application instance
		PSTR						pstrCommandLine,							// pointer to command line string
		INT							nWindowShowValue							// flags specifying how the main window is show
		);

protected:
	ATOM						RegisterWindowClass(						// register a window class
		UINT						nStyle,										// class style
		WNDPROC						pwprcWindowProcedure,						// window message procedure
		HICON						hicoIcon,									// handle of the window icon
		HCURSOR						hcurCursor,									// handle of the window cursor
		HBRUSH						hbrshBackground,							// handle of the window background brush
		LPCTSTR						pstrMenuName,								// pointer to a menu name
		LPCTSTR						pstrClassName								// pointer to a class name
		);
	HWND						CreateWindow(								// create a window
		LPCTSTR						pstrClassName,								// pointer to the windows class name
		LPCTSTR						pstrWindowName,								// pointer to the window name
		DWORD						nStyle,										// window style flags
		DWORD						nExtendedStyle,								// extended window style flags
		DWORD						nLeftPosition,								// left position
		DWORD						nTopPosition,								// top position
		DWORD						nWidth,										// window width
		DWORD						nHeight,									// window height
		HWND						hwndParent,									// handle of the parent window
		HMENU						hmenuMenu,									// handle of a menu
		PVOID						pParameter									// initializing parameter
		);
	static LRESULT WINAPI		WindowMessageProcedure(						// window message procedure
		HWND						hwndWindow,									// handle of the window
		UINT						nMessage,									// code of the message
		WPARAM						wprm,										// first message parameter
		LPARAM						lprm										// second message parameter
		);

	BOOL						TestDevice(									// test a device, if it can be rendered
		PDIRECT3DDEVICE9			pd3dd,										// pointer to Direct3D device interface
		D3DPRESENT_PARAMETERS*		pprsp										// pointer to present parameters for device reset
		);
	BOOL						InitializeTimer(							// initiate the timer values
		);
	FLOAT						CalculateElapsedTime(						// calculate and store elapsed time
		);

protected:
	// attribute declarations
#ifdef						_DEBUG

	_CrtMemState				m_mems;										// memory status to dump unfreed block

#endif

	HWND						m_hwndMain;									// handle of the main window
	HINSTANCE					m_hmodApplication;							// handle of the application instance

	LARGE_INTEGER				m_nTimerFrequency;							// timer frequency
	volatile DWORD				m_nDevice3DFlagsValue;						// flags for the 3D device
	FLOAT						m_fElapsedTime;								// time since last update
};	


#endif
