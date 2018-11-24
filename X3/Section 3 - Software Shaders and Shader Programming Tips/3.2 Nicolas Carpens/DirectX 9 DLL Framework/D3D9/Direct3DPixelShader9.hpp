#ifndef D3D9_Direct3DPixelShader9_hpp
#define D3D9_Direct3DPixelShader9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DPixelShader9 : public IDirect3DPixelShader9, protected Unknown
	{
	public:
		Direct3DPixelShader9();

		~Direct3DPixelShader9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DPixelShader9 methods
		long __stdcall GetFunction(void *data, unsigned int *size);
	};
}

#endif   // D3D9_Direct3DPixelShader9_hpp