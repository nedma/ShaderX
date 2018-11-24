#include "Direct3DSwapChain9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3DSwapChain9::Direct3DSwapChain9()
	{
	}

	Direct3DSwapChain9::~Direct3DSwapChain9()
	{
	}

	long Direct3DSwapChain9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO
	}

	unsigned long Direct3DSwapChain9::AddRef()
	{
		return Unknown::AddRef();
	}

	unsigned long Direct3DSwapChain9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DSwapChain9::GetBackBuffer(unsigned int index, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **backBuffer)
	{
		if(!backBuffer)
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

	long Direct3DSwapChain9::Present(const RECT *sourceRect, const RECT *destRect, HWND destWindowOverride, const RGNDATA *dirtyRegion)
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