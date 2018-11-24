#ifndef D3D9_Direct3DResource9_hpp
#define D3D9_Direct3DResource9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

#include <map>

namespace D3D9
{
	class Direct3DResource9 : public IDirect3DResource9, protected Unknown
	{
	public:
		Direct3DResource9(IDirect3DDevice9 *device, D3DRESOURCETYPE type);

		~Direct3DResource9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DResource9 methods
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		long __stdcall FreePrivateData(const GUID &guid);
		unsigned long __stdcall SetPriority(unsigned long newPriority);
		unsigned long __stdcall GetPriority();
		void __stdcall PreLoad();
		D3DRESOURCETYPE __stdcall GetType();

	private:
	};
}

#endif   // D3D9_Direct3DResource9_hpp