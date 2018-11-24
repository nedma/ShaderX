#include "Direct3DDevice9.hpp"

#include "Direct3D9.hpp"
#include "Direct3DSurface9.hpp"
#include "Direct3DIndexBuffer9.hpp"
#include "Direct3DVertexBuffer9.hpp"
#include "Direct3DTexture9.hpp"
#include "Direct3DVolumeTexture9.hpp"
#include "Direct3DCubeTexture9.hpp"

#include "Error.hpp"

#include <d3dx9.h>

namespace D3D9
{
	inline unsigned long FtoDW(float f)
	{
		return (unsigned long&)f;
	}

	inline float DWtoF(unsigned long dw)
	{
		return (float&)dw;
	}

	Direct3DDevice9::Direct3DDevice9(IDirect3D9 *d3d9, HWND windowHandle, int width, int height, const D3DPRESENT_PARAMETERS &presentation)
	{
		throw INTERNAL_ERROR;   // TODO: Unimplemented

		init = true;

		SetRenderState(D3DRS_ZENABLE, presentation.EnableAutoDepthStencil == TRUE ? D3DZB_TRUE : D3DZB_FALSE);
		SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		SetRenderState(D3DRS_LASTPIXEL, TRUE);
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		SetRenderState(D3DRS_ALPHAREF, 0);
		SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		SetRenderState(D3DRS_DITHERENABLE, FALSE);
		SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		SetRenderState(D3DRS_FOGENABLE, FALSE);
		SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	//	SetRenderState(D3DRS_ZVISIBLE, 0);
		SetRenderState(D3DRS_FOGCOLOR, 0);
		SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetRenderState(D3DRS_FOGSTART, FtoDW(0.0f));
		SetRenderState(D3DRS_FOGEND, FtoDW(1.0f));
		SetRenderState(D3DRS_FOGDENSITY, FtoDW(1.0f));
		SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
		SetRenderState(D3DRS_STENCILENABLE, FALSE);
		SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		SetRenderState(D3DRS_STENCILREF, 0);
		SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		SetRenderState(D3DRS_WRAP0, 0);
		SetRenderState(D3DRS_WRAP1, 0);
		SetRenderState(D3DRS_WRAP2, 0);
		SetRenderState(D3DRS_WRAP3, 0);
		SetRenderState(D3DRS_WRAP4, 0);
		SetRenderState(D3DRS_WRAP5, 0);
		SetRenderState(D3DRS_WRAP6, 0);
		SetRenderState(D3DRS_WRAP7, 0);
		SetRenderState(D3DRS_CLIPPING, TRUE);
		SetRenderState(D3DRS_LIGHTING, TRUE);
		SetRenderState(D3DRS_AMBIENT, 0);
		SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
		SetRenderState(D3DRS_COLORVERTEX, TRUE);
		SetRenderState(D3DRS_LOCALVIEWER, TRUE);
		SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
		SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		SetRenderState(D3DRS_POINTSIZE, FtoDW(1.0f));
		SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));
		SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
		SetRenderState(D3DRS_POINTSCALE_A, FtoDW(1.0f));
		SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
		SetRenderState(D3DRS_POINTSCALE_C, FtoDW(0.0f));
		SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		SetRenderState(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
		SetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
		SetRenderState(D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);
		SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(64.0f));
		SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
		SetRenderState(D3DRS_TWEENFACTOR, FtoDW(0.0f));
		SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		for(int i = 0; i < 8; i++)
		{
			SetTextureStageState(i, D3DTSS_COLOROP, i == 0 ? D3DTOP_MODULATE : D3DTOP_DISABLE);
			SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_ALPHAOP, i == 0 ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE);
			SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			SetTextureStageState(i, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_BUMPENVMAT00, FtoDW(0.0f));
			SetTextureStageState(i, D3DTSS_BUMPENVMAT01, FtoDW(0.0f));
			SetTextureStageState(i, D3DTSS_BUMPENVMAT10, FtoDW(0.0f));
			SetTextureStageState(i, D3DTSS_BUMPENVMAT11, FtoDW(0.0f));
			SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
			SetTextureStageState(i, D3DTSS_BUMPENVLSCALE, FtoDW(0.0f));
			SetTextureStageState(i, D3DTSS_BUMPENVLOFFSET, FtoDW(0.0f));
			SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			SetTextureStageState(i, D3DTSS_COLORARG0, D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_RESULTARG, D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_CONSTANT, 0x00000000);
		}

		D3DMATERIAL9 material;

		material.Diffuse.r = 1.0f;
		material.Diffuse.g = 1.0f;
		material.Diffuse.b = 1.0f;
		material.Diffuse.a = 0.0f;
		material.Ambient.r = 0.0f;
		material.Ambient.g = 0.0f;
		material.Ambient.b = 0.0f;
		material.Ambient.a = 0.0f;
		material.Emissive.r = 0.0f;
		material.Emissive.g = 0.0f;
		material.Emissive.b = 0.0f;
		material.Emissive.a = 0.0f;
		material.Specular.r = 0.0f;
		material.Specular.g = 0.0f;
		material.Specular.b = 0.0f;
		material.Specular.a = 0.0f;
		material.Power = 0.0f;

		SetMaterial(&material);

		D3DLIGHT9 light;

		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r = 1.0f;
		light.Diffuse.g = 1.0f;
		light.Diffuse.b = 1.0f;
		light.Diffuse.a = 0.0f;
		light.Specular.r = 0.0f;
		light.Specular.g = 0.0f;
		light.Specular.b = 0.0f;
		light.Specular.a = 0.0f;
		light.Ambient.r = 0.0f;
		light.Ambient.g = 0.0f;
		light.Ambient.b = 0.0f;
		light.Ambient.a = 0.0f;
		light.Position.x = 0.0f;
		light.Position.y = 0.0f;
		light.Position.z = 0.0f;
		light.Direction.x = 0.0f;
		light.Direction.y = 0.0f;
		light.Direction.z = 1.0f;
		light.Range = 0.0f;
		light.Falloff = 0.0f; 
		light.Attenuation0 = 0.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Theta = 0.0f;
		light.Phi = 0.0f;

		for(int i = 0; i < 8; i++)
		{
			SetLight(i, &light);
		}

		init = false;
	}

	Direct3DDevice9::~Direct3DDevice9()
	{
	}

	long Direct3DDevice9::QueryInterface(const IID &iid, void **object)
	{
		return E_NOINTERFACE;   // TODO: Unimplemented
	}

	unsigned long Direct3DDevice9::AddRef()
	{
		return Unknown::AddRef();
	}
	
	unsigned long Direct3DDevice9::Release()
	{
		return Unknown::Release();
	}

	long Direct3DDevice9::BeginScene()
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

	long Direct3DDevice9::BeginStateBlock()
	{
		return D3D_OK;
	}

	long Direct3DDevice9::Clear(unsigned long count, const D3DRECT *rects, unsigned long flags, unsigned long Color, float Z, unsigned long Stencil)
	{
		if(rects == 0 && count != 0)
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

	long Direct3DDevice9::ColorFill(IDirect3DSurface9 *surface, const RECT *rect, D3DCOLOR color)
	{
		if(!surface)
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

	long Direct3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *presentationParameters, IDirect3DSwapChain9 **swapChain)
	{
		return D3D_OK;
	}

	long Direct3DDevice9::CreateCubeTexture(unsigned int edgeLength, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DCubeTexture9 **cubeTexture, void **sharedHandle)
	{
		if(!cubeTexture)
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

	long Direct3DDevice9::CreateDepthStencilSurface(unsigned int width, unsigned int height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multiSample, unsigned long multiSampleQuality, int discard, IDirect3DSurface9 **surface, void **sharedHandle)
	{
		if(!surface)
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

	long Direct3DDevice9::CreateIndexBuffer(unsigned int length, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DIndexBuffer9 **indexBuffer, void **sharedHandle)
	{
		if(!indexBuffer)
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

	long Direct3DDevice9::CreateOffscreenPlainSurface(unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool, IDirect3DSurface9 **surface, void **sharedHandle)
	{
		if(!surface)
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

	long Direct3DDevice9::CreatePixelShader(const unsigned long *function, IDirect3DPixelShader9 **shader)
	{
		if(!function || !shader)
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

	long Direct3DDevice9::CreateQuery(D3DQUERYTYPE type, IDirect3DQuery9 **query)
	{
		if(!query)
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

	long Direct3DDevice9::CreateRenderTarget(unsigned int width, unsigned int height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multiSample, unsigned long multiSampleQuality, int lockable, IDirect3DSurface9 **surface, void **sharedHandle)
	{
		if(!surface)
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

	long Direct3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE type, IDirect3DStateBlock9 **stateBlock)
	{
		if(!stateBlock)
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

	long Direct3DDevice9::CreateTexture(unsigned int width, unsigned int height, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **texture, void **sharedHandle)
	{
		if(!texture)
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

	long Direct3DDevice9::CreateVertexBuffer(unsigned int length, unsigned long usage, unsigned long FVF, D3DPOOL pool, IDirect3DVertexBuffer9 **vertexBuffer, void **sharedHandle)
	{
		if(FVF & D3DFVF_RESERVED0 || (FVF & D3DFVF_XYZRHW && FVF & D3DFVF_XYZ) || (FVF & D3DFVF_XYZRHW && FVF & D3DFVF_NORMAL))
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

	long Direct3DDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *vertexElements, IDirect3DVertexDeclaration9 **declaration)
	{
		if(!vertexElements || !declaration)
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

	long Direct3DDevice9::CreateVertexShader(const unsigned long *function, IDirect3DVertexShader9 **shader)
	{
		if(!function || !shader)
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

	long Direct3DDevice9::CreateVolumeTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DVolumeTexture9 **volumeTexture, void **sharedHandle)
	{
		if(!volumeTexture)
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

	long Direct3DDevice9::DeletePatch(unsigned int handle)
	{
		try
		{
			throw INTERNAL_ERROR;
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, int baseVertexIndex, unsigned int minIndex, unsigned int numVertices, unsigned int startIndex, unsigned int primitiveCount)
	{	
		try
		{
			switch(type)
			{
			case D3DPT_POINTLIST:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_LINELIST:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_LINESTRIP:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_TRIANGLELIST:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_TRIANGLESTRIP:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_TRIANGLEFAN:
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

	long Direct3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE type, unsigned int minVertexIndex, unsigned int numVertexIndices, unsigned int primitiveCount, const void *indexData, D3DFORMAT indexDataFormat, const void *vertexStreamZeroData, unsigned int vertexStreamZeroStride)
	{
		try
		{
			throw INTERNAL_ERROR;
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE primitiveType, unsigned int startVertex, unsigned int primitiveCount)
	{
		try
		{
			switch(primitiveType)
			{
			case D3DPT_POINTLIST:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_LINELIST:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_LINESTRIP:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_TRIANGLELIST:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_TRIANGLESTRIP:
				throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DPT_TRIANGLEFAN:
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

	long Direct3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE primitiveType, unsigned int primitiveCount, const void *vertexStreamZeroData, unsigned int vertexStreamZeroStride)
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

	long Direct3DDevice9::DrawRectPatch(unsigned int handle, const float *numSegs, const D3DRECTPATCH_INFO *rectPatchInfo)
	{
		if(!numSegs || !rectPatchInfo)
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

	long Direct3DDevice9::DrawTriPatch(unsigned int handle, const float *numSegments, const D3DTRIPATCH_INFO *triPatchInfo)
	{
		if(!numSegments || !triPatchInfo)
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

	long Direct3DDevice9::EndScene()
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

	long Direct3DDevice9::EndStateBlock(IDirect3DStateBlock9 **stateBlock)
	{
		if(!stateBlock)
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

	long Direct3DDevice9::EvictManagedResources()
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

	unsigned int Direct3DDevice9::GetAvailableTextureMem()
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

	long Direct3DDevice9::GetBackBuffer(unsigned int swapChain, unsigned int index, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **backBuffer)
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

	long Direct3DDevice9::GetClipPlane(unsigned long index, float *plane)
	{
		if(!plane)
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

	long Direct3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *clipStatus)
	{
		if(!clipStatus)
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

	long Direct3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *parameters)
	{
		if(!parameters)
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

	long Direct3DDevice9::GetCurrentTexturePalette(unsigned int *paletteNumber)
	{
		if(!paletteNumber)
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

	long Direct3DDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
	{
		if(!ppZStencilSurface)
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

	long Direct3DDevice9::GetDeviceCaps(D3DCAPS9 *caps)
	{
		if(!caps)
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

	long Direct3DDevice9::GetDirect3D(IDirect3D9 **d3d9)
	{
		if(!d3d9)
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

	long Direct3DDevice9::GetDisplayMode(unsigned int swapChain, D3DDISPLAYMODE *mode)
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

	long Direct3DDevice9::GetFrontBufferData(unsigned int swapChain, IDirect3DSurface9 *destSurface)
	{
		if(!destSurface)
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

	long Direct3DDevice9::GetFVF(unsigned long *FVF)
	{
		if(!FVF)
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

	void Direct3DDevice9::GetGammaRamp(unsigned int swapChain, D3DGAMMARAMP *ramp)
	{
		if(!ramp)
		{
			return;
		}

		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
		}
	}

	long Direct3DDevice9::GetIndices(IDirect3DIndexBuffer9 **indexData)
	{
		if(!indexData)
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

	long Direct3DDevice9::GetLight(unsigned long index, D3DLIGHT9 *light)
	{
		if(!light)
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

	long Direct3DDevice9::GetLightEnable(unsigned long index , int *enable)
	{
		if(!enable)
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

	long Direct3DDevice9::GetMaterial(D3DMATERIAL9 *material)
	{
		if(!material)
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

	float Direct3DDevice9::GetNPatchMode()
	{
		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
			return 0.0f;
		}
		
		return 0.0f;
	}

	unsigned int Direct3DDevice9::GetNumberOfSwapChains()
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

	long Direct3DDevice9::GetPaletteEntries(unsigned int paletteNumber, PALETTEENTRY *entries)
	{
		if(!entries)
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

	long Direct3DDevice9::GetPixelShader(IDirect3DPixelShader9 **shader)
	{
		if(!shader)
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

	long Direct3DDevice9::GetPixelShaderConstantB(unsigned int startRegister, int *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::GetPixelShaderConstantF(unsigned int startRegister, float *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::GetPixelShaderConstantI(unsigned int startRegister, int *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::GetRasterStatus(unsigned int swapChain, D3DRASTER_STATUS *rasterStatus)
	{
		if(!rasterStatus)
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

	long Direct3DDevice9::GetRenderState(D3DRENDERSTATETYPE state, unsigned long *value)
	{
		if(!value)
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

	long Direct3DDevice9::GetRenderTarget(unsigned long index, IDirect3DSurface9 **renderTarget)
	{
		if(!renderTarget)
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

	long Direct3DDevice9::GetRenderTargetData(IDirect3DSurface9 *renderTarget, IDirect3DSurface9 *destSurface)
	{
		if(!renderTarget || !destSurface)
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

	long Direct3DDevice9::GetSamplerState(unsigned long sampler, D3DSAMPLERSTATETYPE type, unsigned long *value)
	{
		if(!value)
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

	long Direct3DDevice9::GetScissorRect(RECT *rect)
	{
		if(!rect)
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

	int Direct3DDevice9::GetSoftwareVertexProcessing()
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

	long Direct3DDevice9::GetStreamSource(unsigned int streamNumber, IDirect3DVertexBuffer9 **streamData, unsigned int *offset, unsigned int *stride)
	{
		if(!streamData || !offset || !stride)
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

	long Direct3DDevice9::GetStreamSourceFreq(unsigned int streamNumber, unsigned int *divider)
	{
		if(!divider)
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

	long Direct3DDevice9::GetSwapChain(unsigned int index, IDirect3DSwapChain9 **swapChain)
	{
		if(!swapChain)
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

	long Direct3DDevice9::GetTexture(unsigned long stage, IDirect3DBaseTexture9 **ppTexture)
	{
		if(!ppTexture)
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

	long Direct3DDevice9::GetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long *value)
	{
		if(!value)
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

	long Direct3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
	{
		if(!matrix)
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

	long Direct3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **declaration)
	{
		if(!declaration)
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

	long Direct3DDevice9::GetVertexShader(IDirect3DVertexShader9 **shader)
	{
		if(!shader)
		{
			return D3DERR_INVALIDCALL;
		}

		try
		{
			throw INTERNAL_ERROR;
		}
		catch(...)
		{
			return D3DERR_INVALIDCALL;
		}

		return D3D_OK;
	}

	long Direct3DDevice9::GetVertexShaderConstantB(unsigned int startRegister, int *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::GetVertexShaderConstantF(unsigned int startRegister, float *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::GetVertexShaderConstantI(unsigned int startRegister, int *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::GetVertexShaderDeclaration(unsigned long handle, void *data, unsigned long *size)
	{
		if(!data || !size)
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

	long Direct3DDevice9::GetVertexShaderFunction(unsigned long handle, void *data, unsigned long *size)
	{
		if(!data || !size)
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

	long Direct3DDevice9::GetViewport(D3DVIEWPORT9 *viewport)
	{
		if(!viewport)
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

	long Direct3DDevice9::LightEnable(unsigned long index, int enable)
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

	long Direct3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix)
	{
		if(!matrix)
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

	long Direct3DDevice9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *dirtyRegion)
	{
		// NOTE: pSourceRect and pDestRect can be zero, dirtyRegion has to be zero

		if(dirtyRegion)
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

	long Direct3DDevice9::ProcessVertices(unsigned int srcStartIndex, unsigned int destIndex, unsigned int vertexCount, IDirect3DVertexBuffer9 *destBuffer, IDirect3DVertexDeclaration9 *vertexDeclaration, unsigned long flags)
	{
		if(!destBuffer || !vertexDeclaration)
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

	long Direct3DDevice9::Reset(D3DPRESENT_PARAMETERS *presentationParameters)
	{
		if(!presentationParameters)
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

	long Direct3DDevice9::SetClipPlane(unsigned long index, const float *plane)
	{
		if(!plane)
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

	long Direct3DDevice9::SetClipStatus(const D3DCLIPSTATUS9 *clipStatus)
	{
		if(!clipStatus)
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

	long Direct3DDevice9::SetCurrentTexturePalette(unsigned int paletteNumber)
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

	void Direct3DDevice9::SetCursorPosition(int x, int y, unsigned long flags)
	{
		::SetCursorPos(x, y);
	}

	long Direct3DDevice9::SetCursorProperties(unsigned int x, unsigned int y, IDirect3DSurface9 *cursorBitmap)
	{
		if(!cursorBitmap)
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

	long Direct3DDevice9::SetDepthStencilSurface(IDirect3DSurface9 *newDepthStencil)
	{
		if(!newDepthStencil)
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

	long Direct3DDevice9::SetDialogBoxMode(int enableDialogs)
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

	long Direct3DDevice9::SetFVF(unsigned long FVF)
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

	void Direct3DDevice9::SetGammaRamp(unsigned int swapChain, unsigned long flags, const D3DGAMMARAMP *ramp)
	{
		if(!ramp)
		{
			return;
		}

		try
		{
			throw INTERNAL_ERROR;   // TODO: Unimplemented
		}
		catch(...)
		{
		}
	}

	long Direct3DDevice9::SetLight(unsigned long index, const D3DLIGHT9 *d3dlite)
	{
		if(!d3dlite)
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


	long Direct3DDevice9::SetMaterial(const D3DMATERIAL9 *material)
	{
		if(!material)
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

	long Direct3DDevice9::SetNPatchMode(float segments)
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

	long Direct3DDevice9::SetPaletteEntries(unsigned int paletteNumber, const PALETTEENTRY *entries)
	{
		if(!entries)
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

	long Direct3DDevice9::SetPixelShader(IDirect3DPixelShader9 *shader)
	{
		if(!shader)
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

	long Direct3DDevice9::SetPixelShaderConstantB(unsigned int startRegister, const int *constantData, unsigned int count)
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

	long Direct3DDevice9::SetPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count)
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

	long Direct3DDevice9::SetPixelShaderConstantI(unsigned int startRegister, const int *constantData, unsigned int count)
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

	long Direct3DDevice9::SetRenderState(D3DRENDERSTATETYPE state, unsigned long value)
	{
		try
		{
			if(state < 0 || state > D3DRS_BLENDOPALPHA) throw INTERNAL_ERROR;

			if(!init && renderState[state] == value)
			{
				return D3D_OK;
			}

			renderState[state] = value;

			switch(state)
			{		
				case D3DRS_ZENABLE:
					switch(value)
					{
					case D3DZB_TRUE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DZB_USEW:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DZB_FALSE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_FILLMODE:
					switch(value)
					{
					case D3DFILL_POINT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DFILL_WIREFRAME:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DFILL_SOLID:
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_SHADEMODE:
					switch(value)
					{
					case D3DSHADE_FLAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSHADE_GOURAUD:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSHADE_PHONG:
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_ZWRITEENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_ALPHATESTENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_LASTPIXEL:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_SRCBLEND:
					switch(value)
					{
					case D3DBLEND_ZERO:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_ONE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_SRCCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVSRCCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_SRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVSRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_DESTALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVDESTALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_DESTCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVDESTCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_SRCALPHASAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_BOTHSRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_BOTHINVSRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_DESTBLEND:
					switch(value)
					{
					case D3DBLEND_ZERO:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_ONE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_SRCCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVSRCCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_SRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVSRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_DESTALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVDESTALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_DESTCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_INVDESTCOLOR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_SRCALPHASAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_BOTHSRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DBLEND_BOTHINVSRCALPHA:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_CULLMODE:	
					switch(value)
					{
					case D3DCULL_NONE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCULL_CCW:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCULL_CW:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_ZFUNC:
					switch(value)
					{
					case D3DCMP_NEVER:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_LESS:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_EQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_LESSEQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_GREATER:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_NOTEQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_GREATEREQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_ALWAYS:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_ALPHAREF:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_ALPHAFUNC:
					switch(value)
					{
					case D3DCMP_NEVER:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_LESS:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_EQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_LESSEQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_GREATER:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_NOTEQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_GREATEREQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_ALWAYS:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_DITHERENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_ALPHABLENDENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGCOLOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGTABLEMODE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGSTART:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGEND:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGDENSITY:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_RANGEFOGENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_SPECULARENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_STENCILENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_STENCILFAIL:
					switch(value)
					{
					case D3DSTENCILOP_KEEP:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_ZERO:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_REPLACE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INCRSAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_DECRSAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INVERT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INCR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;;
					case D3DSTENCILOP_DECR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_STENCILZFAIL:
					switch(value)
					{
					case D3DSTENCILOP_KEEP:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_ZERO:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_REPLACE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INCRSAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_DECRSAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INVERT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INCR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_DECR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_STENCILPASS:
					switch(value)
					{
					case D3DSTENCILOP_KEEP:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_ZERO:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_REPLACE:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INCRSAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_DECRSAT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INVERT:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_INCR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DSTENCILOP_DECR:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_STENCILFUNC:
					switch(value)
					{
					case D3DCMP_NEVER:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_LESS:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_EQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_LESSEQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_GREATER:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_NOTEQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_GREATEREQUAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DCMP_ALWAYS:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_STENCILREF:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_STENCILMASK:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_STENCILWRITEMASK:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_TEXTUREFACTOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP0:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP1:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP2:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP3:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP4:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP5:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP6:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_WRAP7:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_CLIPPING:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_LIGHTING:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_AMBIENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_FOGVERTEXMODE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_COLORVERTEX:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_LOCALVIEWER:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_NORMALIZENORMALS:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_DIFFUSEMATERIALSOURCE:
					switch(value)
					{
					case D3DMCS_MATERIAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR1:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR2:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_SPECULARMATERIALSOURCE:
					switch(value)
					{
					case D3DMCS_MATERIAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR1:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR2:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_AMBIENTMATERIALSOURCE:
					switch(value)
					{
					case D3DMCS_MATERIAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR1:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR2:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_EMISSIVEMATERIALSOURCE:
					switch(value)
					{
					case D3DMCS_MATERIAL:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR1:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					case D3DMCS_COLOR2:
						if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
						break;
					default:
						throw INTERNAL_ERROR;
					}
					break;
				case D3DRS_VERTEXBLEND:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_CLIPPLANEENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSIZE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSIZE_MIN:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSPRITEENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSCALEENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSCALE_A:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSCALE_B:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSCALE_C:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_MULTISAMPLEANTIALIAS:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_MULTISAMPLEMASK:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_PATCHEDGESTYLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_DEBUGMONITORTOKEN:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_POINTSIZE_MAX:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_INDEXEDVERTEXBLENDENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_COLORWRITEENABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_TWEENFACTOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DRS_BLENDOP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
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

	long Direct3DDevice9::SetRenderTarget(unsigned long index, IDirect3DSurface9 *renderTarget)
	{
		// NOTE: renderTarget can be zero

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

	long Direct3DDevice9::SetSamplerState(unsigned long sampler, D3DSAMPLERSTATETYPE type, unsigned long value)
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

	long Direct3DDevice9::SetScissorRect(const RECT *rect)
	{
		if(!rect)
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

	long Direct3DDevice9::SetSoftwareVertexProcessing(int software)
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

	long Direct3DDevice9::SetStreamSource(unsigned int stream, IDirect3DVertexBuffer9 *data, unsigned int offset, unsigned int stride)
	{
		// NOTE: streamData can be zero

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

	long Direct3DDevice9::SetStreamSourceFreq(unsigned int streamNumber, unsigned int divider)
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
	
	long Direct3DDevice9::SetTexture(unsigned long stage, IDirect3DBaseTexture9 *texture)
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

	long Direct3DDevice9::SetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long value)
	{
		try
		{
			if(type < 0 || type > D3DTSS_CONSTANT) throw INTERNAL_ERROR;

			if(!init && textureStageState[stage][type] == value)
			{
				return D3D_OK;
			}

			textureStageState[stage][type] = value;

			switch(type)
			{
			case D3DTSS_COLOROP:
				switch(value)
				{
				case D3DTOP_DISABLE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_SELECTARG1:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_SELECTARG2:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATE2X:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATE4X:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_ADD:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_ADDSIGNED:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_ADDSIGNED2X:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_SUBTRACT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_ADDSMOOTH:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BLENDDIFFUSEALPHA:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BLENDTEXTUREALPHA:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BLENDFACTORALPHA:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BLENDTEXTUREALPHAPM:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BLENDCURRENTALPHA:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_PREMODULATE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATEALPHA_ADDCOLOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATECOLOR_ADDALPHA:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BUMPENVMAP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_BUMPENVMAPLUMINANCE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_DOTPRODUCT3:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_MULTIPLYADD:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTOP_LERP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				default:
					throw INTERNAL_ERROR;
				}
				break;
			case D3DTSS_COLORARG1:
				switch(value)
				{
				case D3DTA_CURRENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_DIFFUSE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_SELECTMASK:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_SPECULAR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEMP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEXTURE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TFACTOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_ALPHAREPLICATE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_COMPLEMENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				default:
					throw INTERNAL_ERROR;
				}
				break;
			case D3DTSS_COLORARG2:
				switch(value)
				{
				case D3DTA_CURRENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_DIFFUSE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_SELECTMASK:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_SPECULAR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEMP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEXTURE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TFACTOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_ALPHAREPLICATE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_COMPLEMENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				default:
					throw INTERNAL_ERROR;
				}
				break;
			case D3DTSS_ALPHAOP:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_ALPHAARG1:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_ALPHAARG2:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_BUMPENVMAT00:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_BUMPENVMAT01:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_BUMPENVMAT10:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_BUMPENVMAT11:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_TEXCOORDINDEX:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_BUMPENVLSCALE:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_BUMPENVLOFFSET:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_TEXTURETRANSFORMFLAGS:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_COLORARG0:
				switch(value)
				{
				case D3DTA_CURRENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_DIFFUSE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_SELECTMASK:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_SPECULAR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEMP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEXTURE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TFACTOR:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_ALPHAREPLICATE:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_COMPLEMENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				default:
					throw INTERNAL_ERROR;
				}
				break;
			case D3DTSS_ALPHAARG0:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
				break;
			case D3DTSS_RESULTARG:
				switch(value)
				{
				case D3DTA_CURRENT:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				case D3DTA_TEMP:
					if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
					break;
				default:
					throw INTERNAL_ERROR;
				}
				break;
			case D3DTSS_CONSTANT:
				if(!init) throw INTERNAL_ERROR;   // TODO: Unimplemented
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

	long Direct3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix)
	{
		if(!matrix)
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

	long Direct3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *declaration)
	{
		if(!declaration)
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

	long Direct3DDevice9::SetVertexShader(IDirect3DVertexShader9 *shader)
	{
		if(!shader)
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

	long Direct3DDevice9::SetVertexShaderConstantB(unsigned int startRegister, const int *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::SetVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::SetVertexShaderConstantI(unsigned int startRegister, const int *constantData, unsigned int count)
	{
		if(!constantData)
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

	long Direct3DDevice9::SetViewport(const D3DVIEWPORT9 *viewport)
	{
		if(!viewport)
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

	int Direct3DDevice9::ShowCursor(int show)
	{
		return ::ShowCursor(show);
	}

	long Direct3DDevice9::StretchRect(IDirect3DSurface9 *sourceSurface, const RECT *sourceRect, IDirect3DSurface9 *destSurface, const RECT *destRect, D3DTEXTUREFILTERTYPE filter)
	{
		if(!sourceSurface || !destSurface)
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

	long Direct3DDevice9::TestCooperativeLevel()
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

	long Direct3DDevice9::UpdateSurface(IDirect3DSurface9 *sourceSurface, const RECT *sourceRect, IDirect3DSurface9 *destinationSurface, const POINT *destPoint)
	{
		if(!sourceSurface || !destinationSurface)
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

	long Direct3DDevice9::UpdateTexture(IDirect3DBaseTexture9 *sourceTexture, IDirect3DBaseTexture9 *destTexture)
	{
		if(!sourceTexture || !destTexture)
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

	long Direct3DDevice9::ValidateDevice(unsigned long *numPasses)
	{
		if(!numPasses)
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

	long Direct3DDevice9::SetIndices(IDirect3DIndexBuffer9* indexData)
	{
		if(!indexData)
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