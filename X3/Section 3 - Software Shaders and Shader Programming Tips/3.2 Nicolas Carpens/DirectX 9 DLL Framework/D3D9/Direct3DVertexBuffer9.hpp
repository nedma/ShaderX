#ifndef D3D9_Direct3DVertexBuffer9_hpp
#define D3D9_Direct3DVertexBuffer9_hpp

#include "Direct3DResource9.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DVertexBuffer9 : public IDirect3DVertexBuffer9, protected Direct3DResource9
	{
	public:
		Direct3DVertexBuffer9(IDirect3DDevice9 *device, unsigned int length, unsigned long usage, long FVF, D3DPOOL pool);

		~Direct3DVertexBuffer9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DResource9 methods
		long __stdcall FreePrivateData(const GUID &guid);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		void __stdcall PreLoad();
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		unsigned long __stdcall SetPriority(unsigned long newPriority);
		unsigned long __stdcall GetPriority();
		D3DRESOURCETYPE __stdcall GetType();

		// IDirect3DVertexBuffer9 methods
		long __stdcall Lock(unsigned int offsetToLock, unsigned int size, void **data, unsigned long flags);
		long __stdcall Unlock();
		long __stdcall GetDesc(D3DVERTEXBUFFER_DESC *description);

	private:
	};
}

#endif // D3D9_Direct3DVertexBuffer9_hpp
