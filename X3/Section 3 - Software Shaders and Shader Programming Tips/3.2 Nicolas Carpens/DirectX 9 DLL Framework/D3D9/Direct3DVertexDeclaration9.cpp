#include "Direct3DVertexDeclaration9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DVertexDeclaration9::Direct3DVertexDeclaration9()
	{
	}

	Direct3DVertexDeclaration9::~Direct3DVertexDeclaration9()
	{
	}

	long Direct3DVertexDeclaration9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3DVertexDeclaration9::AddRef()
	{
		return Unknown::AddRef();
	}
	
	unsigned long Direct3DVertexDeclaration9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DVertexDeclaration9::GetDeclaration(D3DVERTEXELEMENT9 *declaration, unsigned int *numElements)
	{
		if(!declaration || !numElements)
		{
			return D3DERR_INVALIDCALL;
		}

		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}
}