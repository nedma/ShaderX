#ifndef D3D9_Direct3DStateBlock9_hpp
#define D3D9_Direct3DStateBlock9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DStateBlock9 : public IDirect3DStateBlock9, protected Unknown
	{
	public:
		Direct3DStateBlock9();

		~Direct3DStateBlock9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DStateBlock9 methods
		long __stdcall Apply();
		long __stdcall Capture();
	};
}

#endif   // D3D9_Direct3DStateBlock9_hpp