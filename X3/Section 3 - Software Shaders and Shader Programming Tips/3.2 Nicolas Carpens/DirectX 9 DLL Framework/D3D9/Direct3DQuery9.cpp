#include "Direct3DQuery9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DQuery9::Direct3DQuery9()
	{
	}

	Direct3DQuery9::~Direct3DQuery9()
	{
	}

	long Direct3DQuery9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3DQuery9::AddRef()
	{
		return Unknown::AddRef();
	}
	
	unsigned long Direct3DQuery9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DQuery9::GetData(void *data, unsigned long size, unsigned long flags)
	{
		if(!data)
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

	unsigned long Direct3DQuery9::GetDataSize()
	{
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

	long Direct3DQuery9::Issue(unsigned long flags)
	{
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