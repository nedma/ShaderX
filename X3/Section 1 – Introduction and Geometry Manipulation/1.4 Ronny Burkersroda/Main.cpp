//
// ShaderX³: Morphing Between Different Objects
//
//	written 2004 by Ronny Burkersroda
//

// header includes
#include					"Morph class.h"								// sample class


// ************************
// *** global functions ***
// ************************

INT WINAPI					WinMain(									// entry function of the executable
	HINSTANCE					hmodInstance,								// handle of the application instance
	HINSTANCE					hmodPreviousInstance,						// unused
	PSTR						pstrCommandLine,							// pointer to command line string
	INT							nWindowShowValue							// flags specifying how the main window is show
	);


// ********************************
// *** function implementations ***
// ********************************

INT WINAPI WinMain( HINSTANCE hmodInstance, HINSTANCE hmodPreviousInstance, PSTR pstrCommandLine, INT nWindowShowValue )
{
	CD3DMorpherSample mrph;

	// call main method of the sample object
	return mrph.Main( hmodInstance, pstrCommandLine, nWindowShowValue );
}
