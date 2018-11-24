#ifndef D3D9_Direct3DSurface9_hpp
#define D3D9_Direct3DSurface9_hpp

#include "Direct3DResource9.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DSurface9 : public IDirect3DSurface9, protected Direct3DResource9
	{
	public:
		Direct3DSurface9(IDirect3DDevice9 *device, void *buffer, int width, int height, D3DFORMAT format, D3DPOOL pool = D3DPOOL_DEFAULT, D3DMULTISAMPLE_TYPE multisample = D3DMULTISAMPLE_NONE, bool lockable = FALSE, unsigned long usage = 0);

		~Direct3DSurface9();

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

		// IDirect3DSurface9 methods
		long __stdcall GetDC(HDC *deviceContext);
		long __stdcall ReleaseDC(HDC deviceContext);
		long __stdcall LockRect(D3DLOCKED_RECT *lockedRect, const RECT *rect, unsigned long flags);
		long __stdcall UnlockRect();
		long __stdcall GetContainer(const IID &iid, void **container);
		long __stdcall GetDesc(D3DSURFACE_DESC *desc);

	private:
	};
}

#endif // D3D9_Direct3DSurface9_hpp
