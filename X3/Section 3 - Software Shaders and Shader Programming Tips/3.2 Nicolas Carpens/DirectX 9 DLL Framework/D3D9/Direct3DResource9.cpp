#include "Direct3DResource9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DResource9::Direct3DResource9(IDirect3DDevice9 *device, D3DRESOURCETYPE type)
	{
	}

	Direct3DResource9::~Direct3DResource9()
	{
	}

	unsigned long Direct3DResource9::AddRef()
	{
		return Unknown::AddRef();
	}

	unsigned long Direct3DResource9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DResource9::GetDevice(IDirect3DDevice9 **device)
	{
		if(!device)
		{
			return D3DERR_INVALIDCALL;
		}

		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3DResource9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3DResource9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3DResource9::FreePrivateData(const GUID &guid)
	{
		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	unsigned long Direct3DResource9::SetPriority(unsigned long newPriority)
	{
		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return 0;
		}

		return 0;
	}

	unsigned long Direct3DResource9::GetPriority()
	{
		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return 0;
		}

		return 0;
	}

	void Direct3DResource9::PreLoad()
	{
		try
		{
			throw INTERNAL_ERROR;    // TODO: Unimplemented
		}
		catch(...)
		{
			return;
		}

		return;
	}

	D3DRESOURCETYPE Direct3DResource9::GetType()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DRTYPE_SURFACE;
		}

		return D3DRTYPE_SURFACE;
	}
}