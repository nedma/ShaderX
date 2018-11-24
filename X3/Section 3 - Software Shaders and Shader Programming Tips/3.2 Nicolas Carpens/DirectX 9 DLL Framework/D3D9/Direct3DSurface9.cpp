#include "Direct3DSurface9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DSurface9::Direct3DSurface9(IDirect3DDevice9 *device, void *buffer, int width, int height, D3DFORMAT format, D3DPOOL pool, D3DMULTISAMPLE_TYPE multisample, bool lockable, unsigned long usage) : Direct3DResource9(device, D3DRTYPE_SURFACE)
	{
	}

	Direct3DSurface9::~Direct3DSurface9()
	{
	}

	long Direct3DSurface9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;
	}

	unsigned long Direct3DSurface9::AddRef()
	{
		return Direct3DResource9::AddRef();
	}

	unsigned long Direct3DSurface9::Release()
	{
		return Direct3DResource9::Release();
	}

	long Direct3DSurface9::FreePrivateData(const GUID &guid)
	{
		return Direct3DResource9::FreePrivateData(guid);
	}

	long Direct3DSurface9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		return Direct3DResource9::GetPrivateData(guid, data, size);
	}

	void Direct3DSurface9::PreLoad()
	{
		Direct3DResource9::PreLoad();
	}

	long Direct3DSurface9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		return Direct3DResource9::SetPrivateData(guid, data, size, flags);
	}

	long Direct3DSurface9::GetDevice(IDirect3DDevice9 **device)
	{
		return Direct3DResource9::GetDevice(device);
	}

	unsigned long Direct3DSurface9::SetPriority(unsigned long newPriority)
	{
		return Direct3DResource9::SetPriority(newPriority);
	}

	unsigned long Direct3DSurface9::GetPriority()
	{
		return Direct3DResource9::GetPriority();
	}

	D3DRESOURCETYPE Direct3DSurface9::GetType()
	{
		return Direct3DResource9::GetType();
	}

	long Direct3DSurface9::GetDC(HDC *deviceContext)
	{
		if(!deviceContext)
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

	long Direct3DSurface9::ReleaseDC(HDC deviceContext)
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

	long Direct3DSurface9::LockRect(D3DLOCKED_RECT *lockedRect, const RECT *rect, unsigned long flags)
	{	
		if(!lockedRect)
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

	long Direct3DSurface9::UnlockRect()
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

	long Direct3DSurface9::GetContainer(const IID &iid, void **container)
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

	long Direct3DSurface9::GetDesc(D3DSURFACE_DESC *desc)
	{
		if(!desc)
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
}
