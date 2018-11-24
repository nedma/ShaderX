#include "stdafx.h"

#include "../demo/Demo.h"


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	try {
		CDemo* system = new CDemo();
		if( SUCCEEDED( system->create( hInst, false ) ) )
			system->run();
		delete system;
	} catch( std::exception& e ) {
		std::string willExit = "\n\nThe application will now exit.";
		MessageBox( 0, (e.what()+willExit).c_str(), "Fatal error", MB_OK );
		return 0;
	}
	return 0;
}
