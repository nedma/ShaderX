// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef __D3D_DEVICE_H
#define __D3D_DEVICE_H

#include "Proxies.h"
#include "RenderStats.h"
#include "EffectStateManager.h"


namespace dingus {

/**
 *  D3D device singleton.
 *
 *  Singleton of D3D device and some convenience stuff: caps, main backbuffer
 *  and z/stencil, etc.
 */
class CD3DDevice : public boost::noncopyable {
public:
	enum { VS_FFP = 0, VS_1_1, VS_2_0, VS_3_0 };
	enum { VP_PURE_HW = 0, VP_HW, VP_MIXED, VP_SW };
	enum { MRT_COUNT = 4, VSTREAM_COUNT = 16 };
public:
	static CD3DDevice& getInstance() { return mSingleInstance; }

public:
	
	// --------------------------------

	/**
	 *  Gets device.
	 *  Use unchecked only in normal cases (device can be NULL during device
	 *  recreation etc.).
	 */
	IDirect3DDevice9& getDevice() const { return *mDevice; }

	bool isDevice() const { return mDevice != 0; }

	/// Sets device. Can be NULL during device recreation, etc.
	void setDevice( IDirect3DDevice9* dx );

	/// Activate device initially or after device reset.
	void activateDevice();
	/// Passivate device after device loss (before reset).
	void passivateDevice();

	const CRenderStats& getStats() const { return mStats; }
	CRenderStats& getStats() { return mStats; }

	const CEffectStateManager& getStateManager() const { return mStateManager; }
	CEffectStateManager& getStateManager() { return mStateManager; }
	
	void	resetCachedState();

	
	// --------------------------------
	// device capabilities

	const D3DCAPS9& getCaps() const { return mCaps; }
	
	/// @return One of VS_ constants.
	int getVShaderVersion() const { return mVShaderVersion; }
	/// @return One of VP_ constants.
	int getVertexProcessing() const { return mVertexProcessing; }
	/// Number of MRTs supported
	int getMRTCount() const { return mCaps.NumSimultaneousRTs; }

	
	// --------------------------------
	// scene begin/end

	void	sceneBegin();
	void	sceneEnd();
	
	// --------------------------------
	// buffers

	void	setIndexBuffer( CD3DIndexBuffer* ib );
	void	setVertexBuffer( int stream, CD3DVertexBuffer* vb, unsigned int offset, unsigned int stride );
	void	setDeclaration( CD3DVertexDecl& decl );
	void	setDeclarationFVF( DWORD fvf );

	// --------------------------------
	// render targets

	/**
	 *  Sets current rendertarget.
	 *  @param rt Rendertarget surface (can be NULL).
	 *  @param index Rendertarget index (for multiple render targets).
	 */
	void	setRenderTarget( CD3DSurface* rt, int index = 0 );
	/**
	 *  Sets default rendertarget (backbuffer on index 0).
	 */
	void	setDefaultRenderTarget();
	/**
	 *  Sets current z/stencil surface.
	 *  @param zs Z/Stencil surface (can be NULL).
	 */
	void	setZStencil( CD3DSurface* zs );
	/**
	 *  Sets default z/stencil (main z/stencil).
	 */
	void	setDefaultZStencil();

	/**
	 *  Clears current rendertarget/zstencil.
	 */
	void	clearTargets(
		bool clearRT, bool clearZ, bool clearStencil,
		D3DCOLOR color, float z = 1.0f, DWORD stencil = 0L );

	
	// --------------------------------
	// main backbuffer/zstencil

	IDirect3DSurface9* getBackBuffer() const { return mBackBuffer; }
	IDirect3DSurface9* getMainZStencil() const { return mMainZStencil; }
	const D3DSURFACE_DESC& getBackBufferDesc() const { return mBackBufferDesc; }
	const D3DSURFACE_DESC& getMainZStencilDesc() const { return mMainZStencilDesc; }
	int		getBackBufferWidth() const { return mBackBufferDesc.Width; }
	int		getBackBufferHeight() const { return mBackBufferDesc.Height; }
	/// @return Width/height ratio
	float	getBackBufferAspect() const { return mBackBufferAspect; }

private:
	CD3DDevice();
	static CD3DDevice	mSingleInstance;

	void	internalSetRenderTarget( IDirect3DSurface9* rt, int index );
	void	internalSetZStencil( IDirect3DSurface9* zs );

private:
	CEffectStateManager	mStateManager;
	D3DCAPS9			mCaps;
	CRenderStats		mStats;
	D3DSURFACE_DESC		mBackBufferDesc;
	D3DSURFACE_DESC		mMainZStencilDesc;

	IDirect3DDevice9*	mDevice;
	IDirect3DSurface9*	mBackBuffer;
	IDirect3DSurface9*	mMainZStencil;
	
	int					mVShaderVersion;
	int					mVertexProcessing;

	float				mBackBufferAspect;

	// active things, for redundant filtering
	IDirect3DSurface9*	mActiveRT[MRT_COUNT];
	IDirect3DSurface9*	mActiveZS;
	IDirect3DVertexBuffer9*	mActiveVB[VSTREAM_COUNT];
	unsigned int		mActiveVBOffset[VSTREAM_COUNT];
	unsigned int		mActiveVBStride[VSTREAM_COUNT];
	CD3DIndexBuffer*	mActiveIB;
	CD3DVertexDecl*		mActiveDeclaration;
	DWORD				mActiveFVF;
};

}; // namespace


#endif
