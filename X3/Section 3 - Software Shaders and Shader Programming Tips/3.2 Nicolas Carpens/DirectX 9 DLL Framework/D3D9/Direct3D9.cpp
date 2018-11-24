#include "Direct3D9.hpp"

#include "Direct3DDevice9.hpp"

#include "Error.hpp"

namespace D3D9
{
	Direct3D9::Direct3D9()
	{
	}

	Direct3D9::~Direct3D9()
	{
	}

	long Direct3D9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3D9::AddRef()
	{
		return Unknown::AddRef();
	}

	unsigned long Direct3D9::Release()
	{
		return Unknown::Release();
	}

	long Direct3D9::CheckDepthStencilMatch(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, D3DFORMAT renderTargetFormat, D3DFORMAT depthStencilFormat)
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

	long Direct3D9::CheckDeviceFormat(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT AdapterFormat, unsigned long usage, D3DRESOURCETYPE resourceType, D3DFORMAT checkFormat)
	{
		try
		{
			switch(resourceType)
			{
			case D3DRTYPE_SURFACE:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DRTYPE_VOLUME:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DRTYPE_CUBETEXTURE:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DRTYPE_VOLUMETEXTURE:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DRTYPE_TEXTURE:
				switch(checkFormat)
				{
				case D3DFMT_R8G8B8:
					throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DFMT_X8R8G8B8:
					throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DFMT_A8R8G8B8:
					throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				default:
					throw INTERNAL_ERROR;
				}
			case D3DRTYPE_VERTEXBUFFER:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DRTYPE_INDEXBUFFER:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			default:
				throw INTERNAL_ERROR;
			}
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3D9::CheckDeviceFormatConversion(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT sourceFormat, D3DFORMAT targetFormat)
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

	long Direct3D9::CheckDeviceMultiSampleType(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT surfaceFormat, int windowed, D3DMULTISAMPLE_TYPE multiSampleType, unsigned long *qualityLevels)
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

	long Direct3D9::CheckDeviceType(unsigned int adapter, D3DDEVTYPE checkType, D3DFORMAT displayFormat, D3DFORMAT backBufferFormat, int windowed)
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

	long Direct3D9::CreateDevice(unsigned int adapter, D3DDEVTYPE deviceType, HWND focusWindow, unsigned long behaviorFlags, D3DPRESENT_PARAMETERS *presentationParameters, IDirect3DDevice9 **returnedDeviceInterface)
	{
		if(!focusWindow || !presentationParameters)
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

	long Direct3D9::EnumAdapterModes(unsigned int adapter, D3DFORMAT format, unsigned int index, D3DDISPLAYMODE *mode)
	{
		if(!mode)
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

	unsigned int Direct3D9::GetAdapterCount()
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

	long Direct3D9::GetAdapterDisplayMode(unsigned int adapter, D3DDISPLAYMODE *mode)
	{
		if(!mode)
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

	long Direct3D9::GetAdapterIdentifier(unsigned int adapter, unsigned  long flags, D3DADAPTER_IDENTIFIER9 *identifier)
	{
		if(!identifier)
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

	unsigned int Direct3D9::GetAdapterModeCount(unsigned int adapter, D3DFORMAT format)
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

	HMONITOR Direct3D9::GetAdapterMonitor(unsigned int adapter)
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

	long Direct3D9::GetDeviceCaps(unsigned int adapter, D3DDEVTYPE deviceType, D3DCAPS9 *capabilities)
	{
		if(!capabilities)
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

	long Direct3D9::RegisterSoftwareDevice(void *initializeFunction)
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