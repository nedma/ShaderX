#include "Direct3DVolumeTexture9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DVolumeTexture9::Direct3DVolumeTexture9(IDirect3DDevice9 *device, unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool) : Direct3DBaseTexture9(device, D3DRTYPE_VOLUMETEXTURE, levels, usage)
	{
	}

	Direct3DVolumeTexture9::~Direct3DVolumeTexture9()
	{
	}

	long Direct3DVolumeTexture9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3DVolumeTexture9::AddRef()
	{
		return Direct3DBaseTexture9::AddRef();
	}

	unsigned long Direct3DVolumeTexture9::Release()
	{
		return Direct3DBaseTexture9::Release();
	}

	long Direct3DVolumeTexture9::FreePrivateData(const GUID &guid)
	{
		return Direct3DBaseTexture9::FreePrivateData(guid);
	}

	long Direct3DVolumeTexture9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		return Direct3DBaseTexture9::GetPrivateData(guid, data, size);
	}

	void Direct3DVolumeTexture9::PreLoad()
	{
		Direct3DBaseTexture9::PreLoad();
	}

	long Direct3DVolumeTexture9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		return Direct3DBaseTexture9::SetPrivateData(guid, data, size, flags);
	}

	long Direct3DVolumeTexture9::GetDevice(IDirect3DDevice9 **device)
	{
		return Direct3DBaseTexture9::GetDevice(device);
	}

	unsigned long Direct3DVolumeTexture9::SetPriority(unsigned long newPriority)
	{
		return Direct3DBaseTexture9::SetPriority(newPriority);
	}

	unsigned long Direct3DVolumeTexture9::GetPriority()
	{
		return Direct3DBaseTexture9::GetPriority();
	}

	D3DRESOURCETYPE Direct3DVolumeTexture9::GetType()
	{
		return Direct3DBaseTexture9::GetType();
	}

	void Direct3DVolumeTexture9::GenerateMipSubLevels()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
		}
	}

	D3DTEXTUREFILTERTYPE Direct3DVolumeTexture9::GetAutoGenFilterType()
	{
		return Direct3DBaseTexture9::GetAutoGenFilterType();
	}

	unsigned long Direct3DVolumeTexture9::GetLevelCount()
	{
		return Direct3DBaseTexture9::GetLevelCount();
	}

	unsigned long Direct3DVolumeTexture9::GetLOD()
	{
		return Direct3DBaseTexture9::GetLOD();
	}

	long Direct3DVolumeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType)
	{
		return Direct3DBaseTexture9::SetAutoGenFilterType(filterType);
	}

	unsigned long Direct3DVolumeTexture9::SetLOD(unsigned long newLOD)
	{
		return Direct3DBaseTexture9::SetLOD(newLOD);
	}

	long Direct3DVolumeTexture9::GetVolumeLevel(unsigned int level, IDirect3DVolume9 **volume)
	{
		if(!volume)
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

	long Direct3DVolumeTexture9::LockBox(unsigned int level, D3DLOCKED_BOX *lockedVolume, const D3DBOX *box, unsigned long flags)
	{
		if(!lockedVolume)
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

	long Direct3DVolumeTexture9::UnlockBox(unsigned int level)
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

	long Direct3DVolumeTexture9::AddDirtyBox(const D3DBOX *dirtyBox)
	{
		if(!dirtyBox)
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

	long Direct3DVolumeTexture9::GetLevelDesc(unsigned int level, D3DVOLUME_DESC *description)
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
}