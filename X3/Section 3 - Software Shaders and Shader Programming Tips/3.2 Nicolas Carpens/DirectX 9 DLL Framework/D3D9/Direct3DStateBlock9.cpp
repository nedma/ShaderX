#include "Direct3DStateBlock9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DStateBlock9::Direct3DStateBlock9()
	{
	}

	Direct3DStateBlock9::~Direct3DStateBlock9()
	{
	}

	long Direct3DStateBlock9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3DStateBlock9::AddRef()
	{
		return Unknown::AddRef();
	}
	
	unsigned long Direct3DStateBlock9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DStateBlock9::Apply()
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

	long Direct3DStateBlock9::Capture()
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