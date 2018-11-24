#ifndef D3D9_Direct3DVertexShader9_hpp
#define D3D9_Direct3DVertexShader9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DVertexShader9 : public IDirect3DVertexShader9, protected Unknown
	{
	public:
		Direct3DVertexShader9();

		~Direct3DVertexShader9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DVertexShader9 methods
		long __stdcall GetFunction(void *data, unsigned int *size);
	};
}

#endif   // D3D9_Direct3DVertexShader9_hpp