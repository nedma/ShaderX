#ifndef D3D9_Direct3DDevice9_hpp
#define D3D9_Direct3DDevice9_hpp

#include "Unknown.hpp"

#include "Direct3D9.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9 : public IDirect3DDevice9, protected Unknown
	{
	public:
		Direct3DDevice9(IDirect3D9 *d3d9, HWND windowHandle, int width, int height, const D3DPRESENT_PARAMETERS &presentation);

		~Direct3DDevice9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DDevice9 methods
		long __stdcall BeginScene();
		long __stdcall BeginStateBlock();
		long __stdcall Clear(unsigned long count, const D3DRECT *rects, unsigned long flags, unsigned long Color, float Z, unsigned long Stencil);
		long __stdcall ColorFill(IDirect3DSurface9 *surface, const RECT *rect, D3DCOLOR color);
		long __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *presentationParameters, IDirect3DSwapChain9 **pSwapChain);
		long __stdcall CreateCubeTexture(unsigned int edgeLength, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DCubeTexture9 **cubeTexture, void **sharedHandle);
		long __stdcall CreateDepthStencilSurface(unsigned int width, unsigned int height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multiSample, unsigned long multiSampleQuality, int discard, IDirect3DSurface9 **surface, void **sharedHandle);
		long __stdcall CreateIndexBuffer(unsigned int length, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DIndexBuffer9 **indexBuffer, void **sharedHandle);
		long __stdcall CreateOffscreenPlainSurface(unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool, IDirect3DSurface9 **surface, void **sharedHandle);
		long __stdcall CreatePixelShader(const unsigned long *function, IDirect3DPixelShader9 **shader);
		long __stdcall CreateQuery(D3DQUERYTYPE type, IDirect3DQuery9 **query);
		long __stdcall CreateRenderTarget(unsigned int width, unsigned int height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multiSample, unsigned long multiSampleQuality, int lockable, IDirect3DSurface9 **surface, void **sharedHandle);
		long __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE type, IDirect3DStateBlock9 **stateBlock);
		long __stdcall CreateTexture(unsigned int width, unsigned int height, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **texture, void **sharedHandle);
		long __stdcall CreateVertexBuffer(unsigned int length, unsigned long usage, unsigned long FVF, D3DPOOL, IDirect3DVertexBuffer9 **ppVertexBuffer, void **sharedHandle);
		long __stdcall CreateVertexDeclaration(const D3DVERTEXELEMENT9 *vertexElements, IDirect3DVertexDeclaration9 **declaration);
		long __stdcall CreateVertexShader(const unsigned long *function, IDirect3DVertexShader9 **shader);
		long __stdcall CreateVolumeTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DVolumeTexture9 **volumeTexture, void **sharedHandle);
		long __stdcall DeletePatch(unsigned int handle);
		long __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE type, int baseVertexIndex, unsigned int minIndex, unsigned int numVertices, unsigned int startIndex, unsigned int primitiveCount);
		long __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE type, unsigned int minVertexIndex, unsigned int numVertexIndices, unsigned int primitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *vertexStreamZeroData, unsigned int vertexStreamZeroStride);
		long __stdcall DrawPrimitive(D3DPRIMITIVETYPE primitiveType, unsigned int startVertex, unsigned int primitiveCount);
		long __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE primitiveType, unsigned int primitiveCount, const void *vertexStreamZeroData, unsigned int vertexStreamZeroStride);
		long __stdcall DrawRectPatch(unsigned int handle, const float *numSegments, const D3DRECTPATCH_INFO *rectPatchInfo);
		long __stdcall DrawTriPatch(unsigned int handle, const float *numSegments, const D3DTRIPATCH_INFO *triPatchInfo);
		long __stdcall EndScene();
		long __stdcall EndStateBlock(IDirect3DStateBlock9 **stateBlock);
		long __stdcall EvictManagedResources();
		unsigned int __stdcall GetAvailableTextureMem();
		long __stdcall GetBackBuffer(unsigned int swapChain, unsigned int index, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **backBuffer);
		long __stdcall GetClipPlane(unsigned long index, float *plane);
		long __stdcall GetClipStatus(D3DCLIPSTATUS9 *clipStatus);
		long __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *parameters);
		long __stdcall GetCurrentTexturePalette(unsigned int *paletteNumber);
		long __stdcall GetDepthStencilSurface(IDirect3DSurface9 **depthStencilSurface);
		long __stdcall GetDeviceCaps(D3DCAPS9 *caps);
		long __stdcall GetDirect3D(IDirect3D9 **D3D);
		long __stdcall GetDisplayMode(unsigned int swapChain ,D3DDISPLAYMODE *mode);
		long __stdcall GetFrontBufferData(unsigned int swapChain, IDirect3DSurface9 *destSurface);
		long __stdcall GetFVF(unsigned long *FVF);
		void __stdcall GetGammaRamp(unsigned int swapChain, D3DGAMMARAMP *ramp);
		long __stdcall GetIndices(IDirect3DIndexBuffer9 **indexData);
		long __stdcall GetLight(unsigned long index, D3DLIGHT9 *light);
		long __stdcall GetLightEnable(unsigned long index , int *enable);
		long __stdcall GetMaterial(D3DMATERIAL9 *material);
		float __stdcall GetNPatchMode();
		unsigned int __stdcall GetNumberOfSwapChains();
		long __stdcall GetPaletteEntries(unsigned int paletteNumber, PALETTEENTRY *entries);
		long __stdcall GetPixelShader(IDirect3DPixelShader9 **shader);
		long __stdcall GetPixelShaderConstantB(unsigned int startRegister, int *constantData, unsigned int count);
		long __stdcall GetPixelShaderConstantF(unsigned int startRegister, float *constantData, unsigned int count);
		long __stdcall GetPixelShaderConstantI(unsigned int startRegister, int *constantData, unsigned int count);
		long __stdcall GetRasterStatus(unsigned int swapChain, D3DRASTER_STATUS *rasterStatus);
		long __stdcall GetRenderState(D3DRENDERSTATETYPE State, unsigned long *value);
		long __stdcall GetRenderTarget(unsigned long index, IDirect3DSurface9 **renderTarget);
		long __stdcall GetRenderTargetData(IDirect3DSurface9 *renderTarget, IDirect3DSurface9 *destSurface);
		long __stdcall GetSamplerState(unsigned long sampler, D3DSAMPLERSTATETYPE type, unsigned long *value);
		long __stdcall GetScissorRect(RECT *rect);
		int __stdcall GetSoftwareVertexProcessing();
		long __stdcall GetStreamSource(unsigned int streamNumber, IDirect3DVertexBuffer9 **streamData, unsigned int *offset, unsigned int *stride);
		long __stdcall GetStreamSourceFreq(unsigned int streamNumber, unsigned int *divider);
		long __stdcall GetSwapChain(unsigned int index, IDirect3DSwapChain9 **swapChain);
		long __stdcall GetTexture(unsigned long stage, IDirect3DBaseTexture9 **texture);
		long __stdcall GetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long *value);
		long __stdcall GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix);
		long __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9 **declaration);
		long __stdcall GetVertexShader(IDirect3DVertexShader9 **shader);
		long __stdcall GetVertexShaderConstantB(unsigned int startRegister, int *constantData, unsigned int count);
		long __stdcall GetVertexShaderConstantF(unsigned int startRegister, float *constantData, unsigned int count);
		long __stdcall GetVertexShaderConstantI(unsigned int startRegister, int *constantData, unsigned int count);
		long __stdcall GetVertexShaderDeclaration(unsigned long handle, void *data, unsigned long *size);
		long __stdcall GetVertexShaderFunction(unsigned long handle, void *data, unsigned long *size);
		long __stdcall GetViewport(D3DVIEWPORT9 *viewport);
		long __stdcall LightEnable(unsigned long index, int enable);
		long __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix);
		long __stdcall Present(const RECT *sourceRect, const RECT *destRect, HWND destWindowOverride, const RGNDATA *dirtyRegion);
		long __stdcall ProcessVertices(unsigned int srcStartIndex, unsigned int destIndex, unsigned int vertexCount, IDirect3DVertexBuffer9 *destBuffer, IDirect3DVertexDeclaration9 *vertexDeclaration, unsigned long flags);
		long __stdcall Reset(D3DPRESENT_PARAMETERS *presentationParameters);
		long __stdcall SetClipPlane(unsigned long index, const float *plane);
		long __stdcall SetClipStatus(const D3DCLIPSTATUS9 *clipStatus);
		long __stdcall SetCurrentTexturePalette(unsigned int paletteNumber);
		void __stdcall SetCursorPosition(int x, int y, unsigned long flags);
		long __stdcall SetCursorProperties(unsigned int x, unsigned int y, IDirect3DSurface9 *cursorBitmap);
		long __stdcall SetDepthStencilSurface(IDirect3DSurface9 *newDepthStencil);
		long __stdcall SetDialogBoxMode(int enableDialogs);
		long __stdcall SetFVF(unsigned long FVF);
		void __stdcall SetGammaRamp(unsigned int swapChain, unsigned long flags, const D3DGAMMARAMP *ramp);
		long __stdcall SetIndices(IDirect3DIndexBuffer9 *indexData);
		long __stdcall SetLight(unsigned long index, const D3DLIGHT9 *p);
		long __stdcall SetMaterial(const D3DMATERIAL9 *material);
		long __stdcall SetNPatchMode(float segments);
		long __stdcall SetPaletteEntries(unsigned int paletteNumber, const PALETTEENTRY *entries);
		long __stdcall SetPixelShader(IDirect3DPixelShader9 *shader);
		long __stdcall SetPixelShaderConstantB(unsigned int startRegister, const int *constantData, unsigned int count);
		long __stdcall SetPixelShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		long __stdcall SetPixelShaderConstantI(unsigned int startRegister, const int *constantData, unsigned int count);
		long __stdcall SetRenderState(D3DRENDERSTATETYPE State, unsigned long pValue);
		long __stdcall SetRenderTarget(unsigned long index, IDirect3DSurface9 *renderTarget);
		long __stdcall SetSamplerState(unsigned long sampler, D3DSAMPLERSTATETYPE type, unsigned long value);
		long __stdcall SetScissorRect(const RECT *rect);
		long __stdcall SetSoftwareVertexProcessing(int software);
		long __stdcall SetStreamSource(unsigned int stream, IDirect3DVertexBuffer9 *data, unsigned int offset, unsigned int stride);
		long __stdcall SetStreamSourceFreq(unsigned int streamNumber, unsigned int divider);
		long __stdcall SetTexture(unsigned long Stage, IDirect3DBaseTexture9 *ppTexture);
		long __stdcall SetTextureStageState(unsigned long Stage, D3DTEXTURESTAGESTATETYPE Type, unsigned long pValue);
		long __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *matrix);
		long __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9 *declaration);
		long __stdcall SetVertexShader(IDirect3DVertexShader9 *shader);
		long __stdcall SetVertexShaderConstantB(unsigned int startRegister, const int *constantData, unsigned int count);
		long __stdcall SetVertexShaderConstantF(unsigned int startRegister, const float *constantData, unsigned int count);
		long __stdcall SetVertexShaderConstantI(unsigned int startRegister, const int *constantData, unsigned int count);
		long __stdcall SetViewport(const D3DVIEWPORT9 *viewport);
		int __stdcall ShowCursor(int show);
		long __stdcall StretchRect(IDirect3DSurface9 *sourceSurface, const RECT *sourceRect, IDirect3DSurface9 *destSurface, const RECT *destRect, D3DTEXTUREFILTERTYPE filter);
		long __stdcall TestCooperativeLevel();
		long __stdcall UpdateSurface(IDirect3DSurface9 *sourceSurface, const RECT *sourceRect, IDirect3DSurface9 *destinationSurface, const POINT *destPoint);
		long __stdcall UpdateTexture(IDirect3DBaseTexture9 *sourceTexture, IDirect3DBaseTexture9 *destTexture);
		long __stdcall ValidateDevice(unsigned long *numPasses);

	private:
		unsigned long renderState[D3DRS_BLENDOPALPHA + 1];
		unsigned long textureStageState[8][D3DTSS_CONSTANT + 1];
		bool init;
	};
}

#endif // D3D9_Direct3DDevice9_hpp
