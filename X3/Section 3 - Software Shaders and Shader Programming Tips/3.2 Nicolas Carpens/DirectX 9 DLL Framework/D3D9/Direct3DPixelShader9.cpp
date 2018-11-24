#include "Direct3DPixelShader9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DPixelShader9::Direct3DPixelShader9()
	{
	}

	Direct3DPixelShader9::~Direct3DPixelShader9()
	{
	}

	long Direct3DPixelShader9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3DPixelShader9::AddRef()
	{
		return Unknown::AddRef();
	}
	
	unsigned long Direct3DPixelShader9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DPixelShader9::GetFunction(void *data, unsigned int *size)
	{
		if(!size)
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