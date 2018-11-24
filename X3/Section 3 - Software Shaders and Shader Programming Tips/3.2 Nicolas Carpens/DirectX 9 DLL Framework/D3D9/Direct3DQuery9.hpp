#ifndef D3D9_Direct3DQuery9_hpp
#define D3D9_Direct3DQuery9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DQuery9 : public IDirect3DQuery9, protected Unknown
	{
	public:
		Direct3DQuery9();

		~Direct3DQuery9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DQuery9 methods
		long __stdcall GetData(void *data, unsigned long size, unsigned long flags);
		unsigned long __stdcall GetDataSize();
		long __stdcall Issue(unsigned long flags);
	};
}

#endif   // D3D9_Direct3DQuery9_hpp