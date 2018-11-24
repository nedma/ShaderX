#ifndef D3D9_Direct3DVolume9_hpp
#define D3D9_Direct3DVolume9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

#include <map>

namespace D3D9
{
	class Direct3DVolume9 : public IDirect3DVolume9, protected Unknown
	{
	public:
		Direct3DVolume9(IDirect3DDevice9 *device);

		~Direct3DVolume9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DVolume9 methods
		long __stdcall FreePrivateData(const GUID &guid);
		long __stdcall GetContainer(const IID &iid, void **container);
		long __stdcall GetDesc(D3DVOLUME_DESC *description);
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		long __stdcall LockBox(D3DLOCKED_BOX *lockedVolume, const D3DBOX *box, unsigned long flags);
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall UnlockBox();

	private:
	};
}

#endif // D3D9_Direct3DVolume9_hpp
