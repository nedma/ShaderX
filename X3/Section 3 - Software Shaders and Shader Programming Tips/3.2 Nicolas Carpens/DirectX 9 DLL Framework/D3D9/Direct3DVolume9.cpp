#include "Direct3DVolume9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DVolume9::Direct3DVolume9(IDirect3DDevice9 *device)
	{
	}

	Direct3DVolume9::~Direct3DVolume9()
	{
	}

	long __stdcall Direct3DVolume9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long __stdcall Direct3DVolume9::AddRef()
	{
		return Unknown::AddRef();
	}

	unsigned long __stdcall Direct3DVolume9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DVolume9::FreePrivateData(const GUID &guid)
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

	long Direct3DVolume9::GetContainer(const IID &iid, void **container)
	{
		if(!container)
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

	long Direct3DVolume9::GetDesc(D3DVOLUME_DESC *description)
	{
		if(!description)
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

	long Direct3DVolume9::GetDevice(IDirect3DDevice9 **device)
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

	long Direct3DVolume9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
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

	long Direct3DVolume9::LockBox(D3DLOCKED_BOX *lockedVolume, const D3DBOX *box, unsigned long flags)
	{
		if(!lockedVolume)   // NOTE: box can be null
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

	long Direct3DVolume9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
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

	long Direct3DVolume9::UnlockBox()
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

