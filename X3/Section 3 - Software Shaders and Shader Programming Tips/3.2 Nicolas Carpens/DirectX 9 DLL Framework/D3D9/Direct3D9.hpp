#ifndef D3D9_Direct3D9_hpp
#define D3D9_Direct3D9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3D9 : public IDirect3D9, protected Unknown
	{
	public:
		Direct3D9();

		~Direct3D9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3D9 methods
		long __stdcall CheckDepthStencilMatch(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, D3DFORMAT renderTargetFormat, D3DFORMAT depthStencilFormat);
		long __stdcall CheckDeviceFormat(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT adapaterFormat, unsigned long usage, D3DRESOURCETYPE resourceType, D3DFORMAT checkFormat);
		long __stdcall CheckDeviceFormatConversion(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT sourceFormat, D3DFORMAT targetFormat);
		long __stdcall CheckDeviceMultiSampleType(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT surfaceFormat, int windowed, D3DMULTISAMPLE_TYPE multiSampleType, unsigned long *qualityLevels);
		long __stdcall CheckDeviceType(unsigned int adapter, D3DDEVTYPE checkType, D3DFORMAT displayFormat, D3DFORMAT backBufferFormat, int windowed);
		long __stdcall CreateDevice(unsigned int adapter, D3DDEVTYPE deviceType, HWND focusWindow, unsigned long behaviorFlags, D3DPRESENT_PARAMETERS *presentationParameters, IDirect3DDevice9 **returnedDeviceInterface);
		long __stdcall EnumAdapterModes(unsigned int adapter, D3DFORMAT format, unsigned int index, D3DDISPLAYMODE *mode);
		unsigned int __stdcall GetAdapterCount();
		long __stdcall GetAdapterDisplayMode(unsigned int adapter, D3DDISPLAYMODE *mode);
		long __stdcall GetAdapterIdentifier(unsigned int adapter, unsigned long flags, D3DADAPTER_IDENTIFIER9 *identifier);
		unsigned int __stdcall GetAdapterModeCount(unsigned int adapter, D3DFORMAT format);
		HMONITOR __stdcall GetAdapterMonitor(unsigned int adapter);
		long __stdcall GetDeviceCaps(unsigned int adapter, D3DDEVTYPE deviceType, D3DCAPS9 *caps);
		long __stdcall RegisterSoftwareDevice(void *initializeFunction);

	private:
	};
}

#endif   // D3D9_Direct3D9_hpp
