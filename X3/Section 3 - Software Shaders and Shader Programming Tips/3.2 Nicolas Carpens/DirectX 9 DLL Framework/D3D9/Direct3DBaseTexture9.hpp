#ifndef D3D9_Direct3DBaseTexture9_hpp
#define D3D9_Direct3DBaseTexture9_hpp

#include "Direct3DResource9.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DBaseTexture9 : public IDirect3DBaseTexture9, protected Direct3DResource9
	{
	public:
		Direct3DBaseTexture9(IDirect3DDevice9 *device, D3DRESOURCETYPE type, unsigned long levels, unsigned long usage);

		~Direct3DBaseTexture9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DResource9 methods
		long __stdcall FreePrivateData(const GUID &guid);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		void __stdcall PreLoad();
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		unsigned long __stdcall SetPriority(unsigned long newPriority);
		unsigned long __stdcall GetPriority();
		D3DRESOURCETYPE __stdcall GetType();

		// IDirect3DBaseTexture9 methods
		void __stdcall GenerateMipSubLevels() = 0;
		D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType() = 0;
		unsigned long __stdcall GetLevelCount();
		unsigned long __stdcall GetLOD();
		long __stdcall SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType);
		long __stdcall SetLOD(long newLOD);

	private:
	};
}

#endif // D3D9_Direct3DBaseTexture9_hpp
