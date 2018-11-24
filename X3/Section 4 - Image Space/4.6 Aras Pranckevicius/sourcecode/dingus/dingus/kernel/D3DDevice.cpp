// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"

#include "D3DDevice.h"
#include "../utils/Errors.h"

using namespace dingus;

CD3DDevice CD3DDevice::mSingleInstance;

// define for testing only
//#define DISABLE_FILTERING


// --------------------------------------------------------------------------

CD3DDevice::CD3DDevice()
:	mDevice(NULL),
	mBackBuffer(NULL),
	mMainZStencil(NULL),
	mVShaderVersion(VS_FFP)
{
	resetCachedState();
}

void CD3DDevice::resetCachedState()
{
	mStateManager.reset();

	int i;
	for( i = 0; i < MRT_COUNT; ++i )
		mActiveRT[i] = (IDirect3DSurface9*)-1;
	mActiveZS = (IDirect3DSurface9*)-1;
	for( i = 0; i < VSTREAM_COUNT; ++i ) {
		mActiveVB[i] = (IDirect3DVertexBuffer9*)-1;
		mActiveVBOffset[i] = -1;
		mActiveVBStride[i] = -1;
	}
	mActiveIB = (CD3DIndexBuffer*)-1;
	mActiveDeclaration = (CD3DVertexDecl*)-1;
	mActiveFVF = -1;
}

void CD3DDevice::activateDevice()
{
	assert( !mMainZStencil );
	assert( !mBackBuffer );

	resetCachedState();

	// backbuffer
	mDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &mBackBuffer );
	assert( mBackBuffer );
	mBackBuffer->GetDesc( &mBackBufferDesc );
	mBackBufferAspect = (float)getBackBufferWidth() / (float)getBackBufferHeight();
	mActiveRT[0] = mBackBuffer;

	// Z/stencil
	mDevice->GetDepthStencilSurface( &mMainZStencil );
	assert( mMainZStencil );
	mMainZStencil->GetDesc( &mMainZStencilDesc );
	mActiveZS = mMainZStencil;

	mStats.reset();
}

void CD3DDevice::passivateDevice()
{
	int i;

	// set default rendertargets
	setDefaultRenderTarget();
	for( i = 1; i < getMRTCount(); ++i )
		setRenderTarget( NULL, i );
	setDefaultZStencil();

	// set null streams
	setIndexBuffer( NULL );
	for( i = 0; i < VSTREAM_COUNT; ++i )
		setVertexBuffer( 0, NULL, 0, 0 );
	setDeclarationFVF( D3DFVF_XYZ );

	// release backbuffer/zstencil
	if( mBackBuffer ) {
		mBackBuffer->Release();
		mBackBuffer = NULL;
	}
	if( mMainZStencil ) {
		mMainZStencil->Release();
		mMainZStencil = NULL;
	}
}

void CD3DDevice::setDevice( IDirect3DDevice9* dx )
{
	mDevice = dx;
	if( !mDevice ) {
		memset( &mCaps, 0, sizeof(mCaps) );
		mVShaderVersion = VS_FFP;
		assert( !mBackBuffer );
		assert( !mMainZStencil );
		return;
	}
	assert( mDevice );
	mDevice->GetDeviceCaps( &mCaps );

	// VS version
	DWORD vs = mCaps.VertexShaderVersion;
	if( vs >= D3DVS_VERSION(3,0) )
		mVShaderVersion = VS_3_0;
	else if( vs >= D3DVS_VERSION(2,0) )
		mVShaderVersion = VS_2_0;
	else if( vs >= D3DVS_VERSION(1,1) )
		mVShaderVersion = VS_1_1;
	else
		mVShaderVersion = VS_FFP;

	// vertex processing
	D3DDEVICE_CREATION_PARAMETERS createParams;
	mDevice->GetCreationParameters( &createParams );
	DWORD flags = createParams.BehaviorFlags;
	if( flags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		mVertexProcessing = (flags&D3DCREATE_PUREDEVICE) ? VP_PURE_HW : VP_HW;
	else if( flags & D3DCREATE_MIXED_VERTEXPROCESSING )
		mVertexProcessing = VP_MIXED;
	else
		mVertexProcessing = VP_SW;
	
	assert( !mBackBuffer );
	assert( !mMainZStencil );
}


// --------------------------------------------------------------------------

void CD3DDevice::setRenderTarget( CD3DSurface* rt, int index )
{
	internalSetRenderTarget( rt ? rt->getObject() : NULL, index );
}

void CD3DDevice::setDefaultRenderTarget()
{
	internalSetRenderTarget( mBackBuffer, 0 );
}

void CD3DDevice::internalSetRenderTarget( IDirect3DSurface9* rt, int index )
{
	assert( mDevice );

	// #MRT check
	if( index >= getMRTCount() ) {
		// debug complaint
		ASSERT_FAIL_MSG( "rendertarget index larger than number of MRTs supported!" );
		return;
	}

	// redundant set check
#ifndef DISABLE_FILTERING
	if( mActiveRT[index] == rt ) {
		++mStats.filtered.renderTarget;
		return;
	}
#endif

	// set RT
	HRESULT hr = mDevice->SetRenderTarget( index, rt );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set render target" );

	mActiveRT[index] = rt;
	++mStats.changes.renderTarget;
}

void CD3DDevice::setZStencil( CD3DSurface* zs )
{
	internalSetZStencil( zs ? zs->getObject() : NULL );
}

void CD3DDevice::setDefaultZStencil()
{
	internalSetZStencil( mMainZStencil );
}

void CD3DDevice::internalSetZStencil( IDirect3DSurface9* zs )
{
	assert( mDevice );

	// redundant set check
#ifndef DISABLE_FILTERING
	if( mActiveZS == zs ) {
		++mStats.filtered.zStencil;
		return;
	}
#endif

	// set RT
	HRESULT hr = mDevice->SetDepthStencilSurface( zs );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set z/stencil" );

	mActiveZS = zs;
	++mStats.changes.zStencil;
}

void CD3DDevice::clearTargets( bool clearRT, bool clearZ, bool clearStencil, D3DCOLOR color, float z, DWORD stencil )
{
	assert( mDevice );

	DWORD clrFlags = 0;
	if( clearRT ) clrFlags |= D3DCLEAR_TARGET;
	if( clearZ ) clrFlags |= D3DCLEAR_ZBUFFER;
	if( clearStencil ) clrFlags |= D3DCLEAR_STENCIL;

	HRESULT hr = mDevice->Clear( 0, NULL, clrFlags, color, z, stencil );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to clear render target" );
}


// --------------------------------------------------------------------------

void CD3DDevice::sceneBegin()
{
	assert( mDevice );
	HRESULT hr = mDevice->BeginScene();
	if( FAILED(hr) )
		THROW_DXERROR( hr, "failed to begin scene" );
}

void CD3DDevice::sceneEnd()
{
	assert( mDevice );
	HRESULT hr = mDevice->EndScene();
	if( FAILED(hr) )
		THROW_DXERROR( hr, "failed to end scene" );
}

// --------------------------------------------------------------------------

void CD3DDevice::setIndexBuffer( CD3DIndexBuffer* ib )
{
	assert( mDevice );
	
	// redundant set check
#ifndef DISABLE_FILTERING
	if( mActiveIB == ib ) {
		++mStats.filtered.ibuffer;
		return;
	}
#endif

	// set
	HRESULT hr = mDevice->SetIndices( ib ? ib->getObject() : NULL );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set indices" );

	mActiveIB = ib;
	++mStats.changes.ibuffer;
}

void CD3DDevice::setVertexBuffer( int stream,  CD3DVertexBuffer* vb, unsigned int offset, unsigned int stride )
{
	assert( mDevice );
	assert( stream >= 0 && stream < VSTREAM_COUNT );
	
	// redundant set check
	IDirect3DVertexBuffer9* vb9 = vb ? vb->getObject() : NULL;
#ifndef DISABLE_FILTERING
	if( mActiveVB[stream]==vb9 && mActiveVBOffset[stream]==offset && mActiveVBStride[stream]==stride ) {
		++mStats.filtered.vbuffer;
		return;
	}
#endif

	// set
	HRESULT hr = mDevice->SetStreamSource( stream, vb9, offset, stride );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set vertex buffer" );

	mActiveVB[stream] = vb9;
	mActiveVBOffset[stream] = offset;
	mActiveVBStride[stream] = stride;
	++mStats.changes.vbuffer;
}

void CD3DDevice::setDeclaration( CD3DVertexDecl& decl )
{
	assert( mDevice );
	assert( &decl );
	
	// redundant set check
#ifndef DISABLE_FILTERING
	if( mActiveDeclaration == &decl ) {
		++mStats.filtered.declarations;
		return;
	}
#endif

	// set
	HRESULT hr = mDevice->SetVertexDeclaration( &decl ? decl.getObject() : NULL );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set declaration" );

	mActiveDeclaration = &decl;
	mActiveFVF = 0; // FVF and declaration are coupled!
	++mStats.changes.declarations;
}

void CD3DDevice::setDeclarationFVF( DWORD fvf )
{
	assert( mDevice );
	assert( fvf );
	
	// redundant set check
#ifndef DISABLE_FILTERING
	if( mActiveFVF == fvf ) {
		++mStats.filtered.declarations;
		return;
	}
#endif

	// set
	HRESULT hr = mDevice->SetFVF( fvf );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set declaration fvf" );

	mActiveFVF = fvf; 
	mActiveDeclaration = NULL; // FVF and declaration are coupled!
	++mStats.changes.declarations;
}
