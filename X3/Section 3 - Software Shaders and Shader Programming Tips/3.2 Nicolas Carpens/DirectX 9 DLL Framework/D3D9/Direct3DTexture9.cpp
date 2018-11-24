#include "Direct3DTexture9.hpp"

#include "Direct3DSurface9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DTexture9::Direct3DTexture9(IDirect3DDevice9 *device, unsigned int width, unsigned int height, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool) : Direct3DBaseTexture9(device, D3DRTYPE_TEXTURE, levels, usage)
	{
	}

	Direct3DTexture9::~Direct3DTexture9()
	{
	}

	long Direct3DTexture9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;
	}

	unsigned long Direct3DTexture9::AddRef()
	{
		return Direct3DBaseTexture9::AddRef();
	}

	unsigned long Direct3DTexture9::Release()
	{
		return Direct3DBaseTexture9::Release();
	}

	long Direct3DTexture9::FreePrivateData(const GUID &guid)
	{
		return Direct3DBaseTexture9::FreePrivateData(guid);
	}

	long Direct3DTexture9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		return Direct3DBaseTexture9::GetPrivateData(guid, data, size);
	}

	void Direct3DTexture9::PreLoad()
	{
		Direct3DBaseTexture9::PreLoad();
	}

	long Direct3DTexture9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		return Direct3DBaseTexture9::SetPrivateData(guid, data, size, flags);
	}

	long Direct3DTexture9::GetDevice(IDirect3DDevice9 **device)
	{
		return Direct3DBaseTexture9::GetDevice(device);
	}

	unsigned long Direct3DTexture9::SetPriority(unsigned long newPriority)
	{
		return Direct3DBaseTexture9::SetPriority(newPriority);
	}

	unsigned long Direct3DTexture9::GetPriority()
	{
		return Direct3DBaseTexture9::GetPriority();
	}

	D3DRESOURCETYPE Direct3DTexture9::GetType()
	{
		return Direct3DBaseTexture9::GetType();
	}

	void Direct3DTexture9::GenerateMipSubLevels()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return;
		}
	}

	D3DTEXTUREFILTERTYPE Direct3DTexture9::GetAutoGenFilterType()
	{
		return Direct3DBaseTexture9::GetAutoGenFilterType();
	}

	unsigned long Direct3DTexture9::GetLevelCount()
	{
		return Direct3DBaseTexture9::GetLevelCount();
	}

	unsigned long Direct3DTexture9::GetLOD()
	{
		return Direct3DBaseTexture9::GetLOD();
	}

	long Direct3DTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType)
	{
		return Direct3DBaseTexture9::SetAutoGenFilterType(filterType);
	}

	unsigned long Direct3DTexture9::SetLOD(unsigned long newLOD)
	{
		return Direct3DBaseTexture9::SetLOD(newLOD);
	}

	long Direct3DTexture9::GetLevelDesc(unsigned int level, D3DSURFACE_DESC *description)
	{	
		if(level >= GetLevelCount())
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

	long Direct3DTexture9::LockRect(unsigned int level, D3DLOCKED_RECT *lockedRect, const RECT *rect, unsigned long flags)
	{
		if(level >= GetLevelCount())
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

	long Direct3DTexture9::GetSurfaceLevel(unsigned int level, IDirect3DSurface9 **surfaceLevel)
	{
		if(level >= GetLevelCount())
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

	long Direct3DTexture9::UnlockRect(unsigned int level)
	{
		if(level >= GetLevelCount())
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

	long Direct3DTexture9::AddDirtyRect(const RECT *dirtyRect)
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
