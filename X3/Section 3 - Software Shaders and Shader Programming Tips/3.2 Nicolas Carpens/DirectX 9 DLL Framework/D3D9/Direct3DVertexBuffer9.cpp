#include "Direct3DVertexBuffer9.hpp"

#include "Direct3DDevice9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DVertexBuffer9::Direct3DVertexBuffer9(IDirect3DDevice9 *device, unsigned int length, unsigned long usage, long FVF, D3DPOOL pool) : Direct3DResource9(device, D3DRTYPE_VERTEXBUFFER)
	{
	}

	Direct3DVertexBuffer9::~Direct3DVertexBuffer9()
	{
	}

	long Direct3DVertexBuffer9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO
	}

	unsigned long Direct3DVertexBuffer9::AddRef()
	{
		return Direct3DResource9::AddRef();
	}

	unsigned long Direct3DVertexBuffer9::Release()
	{
		return Direct3DResource9::Release();
	}

	long Direct3DVertexBuffer9::FreePrivateData(const GUID &guid)
	{
		return Direct3DResource9::FreePrivateData(guid);
	}

	long Direct3DVertexBuffer9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		return Direct3DResource9::GetPrivateData(guid, data, size);
	}

	void Direct3DVertexBuffer9::PreLoad()
	{
		Direct3DResource9::PreLoad();
	}

	long Direct3DVertexBuffer9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		return Direct3DResource9::SetPrivateData(guid, data, size, flags);
	}

	long Direct3DVertexBuffer9::GetDevice(IDirect3DDevice9 **device)
	{
		return Direct3DResource9::GetDevice(device);
	}

	unsigned long Direct3DVertexBuffer9::SetPriority(unsigned long newPriority)
	{
		return Direct3DResource9::SetPriority(newPriority);
	}

	unsigned long Direct3DVertexBuffer9::GetPriority()
	{
		return Direct3DResource9::GetPriority();
	}

	D3DRESOURCETYPE Direct3DVertexBuffer9::GetType()
	{
		return Direct3DResource9::GetType();
	}

	long Direct3DVertexBuffer9::Lock(unsigned int offsetToLock, unsigned int size, void **data, unsigned long flags)
	{
		if(!data)
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

	long Direct3DVertexBuffer9::Unlock()
	{
		return D3D_OK;
	}

	long Direct3DVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC *description)
	{
		if(!description)
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
