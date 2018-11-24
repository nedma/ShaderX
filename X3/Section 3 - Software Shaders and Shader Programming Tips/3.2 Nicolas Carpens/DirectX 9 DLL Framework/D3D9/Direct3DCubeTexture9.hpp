#ifndef D3D9_Direct3DCubeTexture9_hpp
#define D3D9_Direct3DCubeTexture9_hpp

#include "Direct3DBaseTexture9.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DCubeTexture9 : public IDirect3DCubeTexture9, protected Direct3DBaseTexture9
	{
	public:
		Direct3DCubeTexture9(IDirect3DDevice9 *device, unsigned int edgeLength, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool);

		~Direct3DCubeTexture9();

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

		// IDirect3DBaseTexture methods
		void __stdcall GenerateMipSubLevels();
		D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType();
		unsigned long __stdcall GetLevelCount();
		unsigned long __stdcall GetLOD();
		long __stdcall SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType);
		unsigned long __stdcall SetLOD(unsigned long newLOD);

		// IDirect3DCubeTexture9 methods
		long __stdcall AddDirtyRect(D3DCUBEMAP_FACES face, const RECT *dirtyRect);
		long __stdcall GetCubeMapSurface(D3DCUBEMAP_FACES face, unsigned int level , IDirect3DSurface9 **cubeMapSurface);
		long __stdcall GetLevelDesc(unsigned int level, D3DSURFACE_DESC *description);
		long __stdcall LockRect(D3DCUBEMAP_FACES face, unsigned int level, D3DLOCKED_RECT *lockedRect, const RECT *rect, unsigned long flags);
		long __stdcall UnlockRect(D3DCUBEMAP_FACES face, unsigned int level);

	private:
	};
}

#endif // D3D9_Direct3D9_hpp
