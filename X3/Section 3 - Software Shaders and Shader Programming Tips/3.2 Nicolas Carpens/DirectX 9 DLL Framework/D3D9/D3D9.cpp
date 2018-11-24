#include "Direct3D9.hpp"

extern "C"
{
	IDirect3D9 *__stdcall Direct3DCreate9(unsigned int version)
	{
		if(version != D3D_SDK_VERSION)
		{
			return 0;
		}

		try
		{
			return new D3D9::Direct3D9();
		}
		catch(...)
		{
			return 0;
		}
	}
}