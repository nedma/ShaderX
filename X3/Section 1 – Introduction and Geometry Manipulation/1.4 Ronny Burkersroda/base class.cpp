//
// Direct3D Sample base class
//
//	written 2004 by Ronny Burkersroda
//

// header includes
#include					"base class.h"								// Direct3D base class
#include					<d3dx9.h>									// Direct3D extensions


// *****************************
// *** class implementations ***
// *****************************


//
// standard constructor
//	sets standard values.
//
CD3DSampleBase::CD3DSampleBase()
	: m_hwndMain( NULL )
	, m_hmodApplication( NULL )
	, m_nDevice3DFlagsValue( DEVICE3D_NONE )
	, m_fElapsedTime( 0.0f )
{
#ifdef						_DEBUG

	// save memory dump state
	_CrtMemCheckpoint( &m_mems );

#endif

	m_nTimerFrequency.QuadPart = 0;
}

//
// standard destructor
//	cleans up the object.
//
CD3DSampleBase::~CD3DSampleBase()
{
#ifdef						_DEBUG

	OutputDebugString( "Direct3D sample class destruction: " );

	// start memory dump
	_CrtMemDumpAllObjectsSince( &m_mems );

#endif
}

//
// main method
//	stores instance handle.
//
INT CD3DSampleBase::Main( HINSTANCE hmodInstance, PSTR pstrCommandLine, INT nWindowShowValue )
{
	// set application instance handle
	m_hmodApplication = hmodInstance;

	return 0;
}

//
// create window method
//	creates an new window basing on a registered window class.
//
ATOM CD3DSampleBase::RegisterWindowClass( UINT nStyle, WNDPROC pwprcWindowProcedure, HICON hicoIcon, HCURSOR hcurCursor, HBRUSH hbrshBackground, LPCTSTR pstrMenuName, LPCTSTR pstrClassName )
{
	WNDCLASSEX wcx = { sizeof( WNDCLASSEX ), nStyle, pwprcWindowProcedure, 0, 0, m_hmodApplication, hicoIcon, hcurCursor, hbrshBackground, pstrMenuName, pstrClassName, NULL };

	// call Platform SDK class register function
	return RegisterClassEx( &wcx );
}

//
// create window method
//	creates an new window basing on a registered window class.
//
HWND CD3DSampleBase::CreateWindow( PCTSTR pstrClassName, PCTSTR pstrWindowName, DWORD nStyle, DWORD nExtendedStyle, DWORD nLeftPosition, DWORD nTopPosition, DWORD nWidth, DWORD nHeight, HWND hwndParent, HMENU hmenuMenu, LPVOID pParameter )
{
	// call Platform SDK function
	return CreateWindowEx( nExtendedStyle, pstrClassName, pstrWindowName, nStyle, nLeftPosition, nTopPosition, nWidth, nHeight, hwndParent, hmenuMenu, m_hmodApplication, pParameter );
}

//
// window message procedure
//	is called by Windows to handle window messages.
//
LRESULT WINAPI CD3DSampleBase::WindowMessageProcedure( HWND hwndWindow, UINT nMessage, WPARAM wprm, LPARAM lprm )
{
	static CD3DSampleBase* pbase = NULL;

	// process messages
	switch( nMessage )
	{
	case WM_CREATE:
		// set object
		pbase = (CD3DSampleBase*)( (LPCREATESTRUCT) lprm )->lpCreateParams;
		break;

	// window maximize and restore
	case WM_SYSCOMMAND:

		// disable drawing while minimized window
		if( wprm == SC_MINIMIZE )
			pbase->m_nDevice3DFlagsValue |= DEVICE3D_DISABLERENDERING;

		// break if other system command
		if( ( wprm != SC_MAXIMIZE ) && ( wprm != SC_RESTORE ) && ( wprm != 61490 ) && ( wprm != 61730 ) )
			break;

		// call standard window procedure
		DefWindowProc( hwndWindow, nMessage, wprm, lprm );

		// set flag to reset device
		pbase->m_nDevice3DFlagsValue |= DEVICE3D_RESET;

	// window sizing
	case WM_SIZE:
	case WM_EXITSIZEMOVE:

		// set flag to reset device
		if( wprm != SIZE_MINIMIZED )
			pbase->m_nDevice3DFlagsValue |= DEVICE3D_RESET;
		else
			pbase->m_nDevice3DFlagsValue |= DEVICE3D_DISABLERENDERING;

		return 0;

	// window activation
	case WM_ACTIVATE:

		// set flag to reset device
		if( LOWORD( wprm ) == WA_INACTIVE )
			pbase->m_nDevice3DFlagsValue |= DEVICE3D_DISABLERENDERING;
		else
			pbase->m_nDevice3DFlagsValue |= DEVICE3D_RESET;

		return 0;

	// window closing
	case WM_CLOSE:

		// quit the program
		PostQuitMessage( 0 );

		return 0;
	}

	// call standard window procedure
	return DefWindowProc( hwndWindow, nMessage, wprm, lprm );
}

//
// test device method
//	checks if a Direct3D device is drawable.
//
BOOL CD3DSampleBase::TestDevice( PDIRECT3DDEVICE9 pd3dd, D3DPRESENT_PARAMETERS* pprsp )
{
	BOOL bResult;

	// check if a device reset is needed
	if( m_nDevice3DFlagsValue & DEVICE3D_RESET )
	{
		// test if device has to be reseted
		switch( pd3dd->TestCooperativeLevel() )
		{
		case D3DERR_DEVICELOST:
		case D3DERR_DEVICENOTRESET:

			bResult = FALSE;

			break;

		default:

			// enable rendering
			m_nDevice3DFlagsValue &= ( DEVICE3D_FORCE_DWORD ^ ( DEVICE3D_DISABLERENDERING | DEVICE3D_RESET ) );

			bResult = TRUE;
		}
	}
	// test if device is lost
	else switch( pd3dd->TestCooperativeLevel() )
	{
	case D3DERR_DEVICELOST:
	case D3DERR_DEVICENOTRESET:

		bResult = FALSE;

		break;

	default:

		// check if rendering is disabled
		if( m_nDevice3DFlagsValue & DEVICE3D_DISABLERENDERING )
			bResult = FALSE;
		else
			bResult = TRUE;
	}

	return bResult;
}

//
// initialize timer method
//	starts the high performance timer.
//
BOOL CD3DSampleBase::InitializeTimer()
{
	// get timer frequency
	return QueryPerformanceFrequency( &m_nTimerFrequency );
}

//
// calculate elapsed time method
//	computes the elapsed time since last call.
//
FLOAT CD3DSampleBase::CalculateElapsedTime()
{
	static LARGE_INTEGER	nPreviousTime	= { 0, 0 };
	LARGE_INTEGER			nCurrentTime;

	// get current time
	if( !QueryPerformanceCounter( &nCurrentTime ) )
		return m_fElapsedTime;

	// set first time
	if( !nPreviousTime.QuadPart )
		nPreviousTime = nCurrentTime;

	// calculate elapsed time
	m_fElapsedTime = (FLOAT)( (DOUBLE)( nCurrentTime.QuadPart - nPreviousTime.QuadPart ) / (DOUBLE) m_nTimerFrequency.QuadPart );

	// set current as previous time
	nPreviousTime = nCurrentTime;

	return m_fElapsedTime;
}
