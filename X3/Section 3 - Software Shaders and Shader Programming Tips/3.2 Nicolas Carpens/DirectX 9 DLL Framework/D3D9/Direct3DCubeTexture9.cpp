#include "Direct3DCubeTexture9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DCubeTexture9::Direct3DCubeTexture9(IDirect3DDevice9 *device, unsigned int edgeLength, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool) : Direct3DBaseTexture9(device, D3DRTYPE_CUBETEXTURE, levels, usage)
	{
	}

	Direct3DCubeTexture9::~Direct3DCubeTexture9()
	{
	}

	long Direct3DCubeTexture9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;
	}

	unsigned long Direct3DCubeTexture9::AddRef()
	{
		return Direct3DBaseTexture9::AddRef();
	}

	unsigned long Direct3DCubeTexture9::Release()
	{
		return Direct3DBaseTexture9::Release();
	}

	long Direct3DCubeTexture9::FreePrivateData(const GUID &guid)
	{
		return Direct3DBaseTexture9::FreePrivateData(guid);
	}

	long Direct3DCubeTexture9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		return Direct3DBaseTexture9::GetPrivateData(guid, data, size);
	}

	void Direct3DCubeTexture9::PreLoad()
	{
		Direct3DBaseTexture9::PreLoad();
	}

	long Direct3DCubeTexture9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		return Direct3DBaseTexture9::SetPrivateData(guid, data, size, flags);
	}

	long Direct3DCubeTexture9::GetDevice(IDirect3DDevice9 **device)
	{
		return Direct3DBaseTexture9::GetDevice(device);
	}

	unsigned long Direct3DCubeTexture9::SetPriority(unsigned long newPriority)
	{
		return Direct3DBaseTexture9::SetPriority(newPriority);
	}

	unsigned long Direct3DCubeTexture9::GetPriority()
	{
		return Direct3DBaseTexture9::GetPriority();
	}

	D3DRESOURCETYPE Direct3DCubeTexture9::GetType()
	{
		return Direct3DBaseTexture9::GetType();
	}

	void Direct3DCubeTexture9::GenerateMipSubLevels()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
		}
	}

	D3DTEXTUREFILTERTYPE Direct3DCubeTexture9::GetAutoGenFilterType()
	{
		return Direct3DBaseTexture9::GetAutoGenFilterType();
	}

	unsigned long Direct3DCubeTexture9::GetLevelCount()
	{
		return Direct3DBaseTexture9::GetLevelCount();
	}

	unsigned long Direct3DCubeTexture9::GetLOD()
	{
		return Direct3DBaseTexture9::GetLOD();
	}

	long Direct3DCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType)
	{
		return Direct3DBaseTexture9::SetAutoGenFilterType(filterType);
	}

	unsigned long Direct3DCubeTexture9::SetLOD(unsigned long newLOD)
	{
		return Direct3DBaseTexture9::SetLOD(newLOD);
	}

	long Direct3DCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES face, const RECT *dirtyRect)
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

	long Direct3DCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES face, unsigned int level , IDirect3DSurface9 **cubeMapSurface)
	{
		if(!cubeMapSurface)
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

	long Direct3DCubeTexture9::GetLevelDesc(unsigned int level, D3DSURFACE_DESC *description)
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

	long Direct3DCubeTexture9::LockRect(D3DCUBEMAP_FACES face, unsigned int level, D3DLOCKED_RECT *lockedRect, const RECT *rect, unsigned long flags)
	{
		if(!lockedRect)
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

	long Direct3DCubeTexture9::UnlockRect(D3DCUBEMAP_FACES face, unsigned int level)
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
