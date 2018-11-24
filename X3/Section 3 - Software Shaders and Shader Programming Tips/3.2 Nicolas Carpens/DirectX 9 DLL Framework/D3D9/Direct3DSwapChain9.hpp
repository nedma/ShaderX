#ifndef D3D9_Direct3DSwapChain9_hpp
#define D3D9_Direct3DSwapChain9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DSwapChain9 : public IDirect3DSwapChain9, protected Unknown
	{
	public:
		Direct3DSwapChain9();

		~Direct3DSwapChain9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DSwapChain9 methods
		long __stdcall GetBackBuffer(unsigned int index, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **backBuffer);
		long __stdcall Present(const RECT *sourceRect, const RECT *destRect, HWND destWindowOverride, const RGNDATA *dirtyRegion);
	};
}

#endif // D3D9_Direct3DSwapChain9_hpp
