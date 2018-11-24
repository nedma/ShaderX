#include "Direct3DBaseTexture9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DBaseTexture9::Direct3DBaseTexture9(IDirect3DDevice9 *device, D3DRESOURCETYPE type, unsigned long levels, unsigned long usage) : Direct3DResource9(device, type)
	{
	}

	Direct3DBaseTexture9::~Direct3DBaseTexture9()
	{
	}
	
	long Direct3DBaseTexture9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;
	}

	unsigned long Direct3DBaseTexture9::AddRef()
	{
		return Direct3DResource9::AddRef();
	}

	unsigned long Direct3DBaseTexture9::Release()
	{
		return Direct3DResource9::Release();
	}

	long Direct3DBaseTexture9::FreePrivateData(const GUID &guid)
	{
		return Direct3DResource9::FreePrivateData(guid);
	}

	long Direct3DBaseTexture9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		return Direct3DResource9::GetPrivateData(guid, data, size);
	}

	void Direct3DBaseTexture9::PreLoad()
	{
		Direct3DResource9::PreLoad();
	}

	long Direct3DBaseTexture9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		return Direct3DResource9::SetPrivateData(guid, data, size, flags);
	}

	long Direct3DBaseTexture9::GetDevice(IDirect3DDevice9 **device)
	{
		return Direct3DResource9::GetDevice(device);
	}

	unsigned long Direct3DBaseTexture9::SetPriority(unsigned long newPriority)
	{
		return Direct3DResource9::SetPriority(newPriority);
	}

	unsigned long Direct3DBaseTexture9::GetPriority()
	{
		return Direct3DResource9::GetPriority();
	}

	D3DRESOURCETYPE Direct3DBaseTexture9::GetType()
	{
		return Direct3DResource9::GetType();
	}

	D3DTEXTUREFILTERTYPE Direct3DBaseTexture9::GetAutoGenFilterType()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return D3DTEXF_NONE;
		}

		return D3DTEXF_NONE;
	}

	unsigned long Direct3DBaseTexture9::GetLevelCount()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return 0;
		}

		return 0;
	}

	unsigned long Direct3DBaseTexture9::GetLOD()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return 0;
		}

		return 0;
	}

	long Direct3DBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType)
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

	long Direct3DBaseTexture9::SetLOD(long newLOD)
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