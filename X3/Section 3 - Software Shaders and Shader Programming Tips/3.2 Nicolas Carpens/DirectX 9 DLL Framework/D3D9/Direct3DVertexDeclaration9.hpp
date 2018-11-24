#ifndef D3D9_Direct3DVertexDeclaration9_hpp
#define D3D9_Direct3DVertexDeclaration9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DVertexDeclaration9 : public IDirect3DVertexDeclaration9, protected Unknown
	{
	public:
		Direct3DVertexDeclaration9();

		~Direct3DVertexDeclaration9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DVertexDeclaration9 methods
		long __stdcall GetDeclaration(D3DVERTEXELEMENT9 *declaration, unsigned int *numElements);
	};
}

#endif   // D3D9_Direct3DVertexDeclaration9_hpp