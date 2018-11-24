#ifndef D3D9_Unknown_hpp
#define D3D9_Unknown_hpp

#include <d3d9.h>

namespace D3D9
{
	class Unknown : IUnknown
	{
	public:
		Unknown();

		virtual ~Unknown() = 0;

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object) = 0;
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

	private:
		unsigned int referenceCount;
	};
}

#endif   // D3D9_Unknown_hpp
