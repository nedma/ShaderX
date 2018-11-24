#include "stdafx.h"
#include "System.h"

#include <dingus/resource/DeviceResource.h>

#include <dingus/renderer/EffectParamsNotifier.h>

#include <dingus/console/W32StdConsoleRenderingContext.h>
#include <dingus/console/WDebugConsoleRenderingContext.h>

#include <dingus/dxutils/D3DFont.h>
#include <dingus/console/D3DConsoleRenderingContext.h>


CSystem::CSystem()
:	mStdConsoleCtx(0),
	mAppInited( false )
{
	mCreationWidth			= 640;
	mCreationHeight 		= 480;
	mWindowTitle			= "";

	const bool devMode = false;
	mStartFullscreen		= !devMode;
	mVSyncFullscreen		= false;
	mSelectDeviceAtStartup	= !devMode;
	mShowFrameDeviceStats = false;

	mShowCursorWhenFullscreen	= false;
	mDebugTimer					= false;
	mEnumeration.mUsesDepthBuffer		= true;
	mEnumeration.mMinColorChannelBits	= 8;
	mEnumeration.mMinAlphaChannelBits	= 8;
	mEnumeration.mMinDepthBits			= 16;
	mEnumeration.mMinStencilBits		= 0;
	mEnumeration.mUsesMixedVP			= true;
	mDataPath = "data/";

	// init console
	//if( devMode ) {
		mStdConsoleCtx = new CW32StdConsoleRenderingContext();
		CConsole::getInstance().setDefaultRenderingContext( *mStdConsoleCtx );
	//}

	mFont = new CD3DFont( "Arial", 10, dingus::CD3DFont::BOLD );
	mD3DConsoleCtx = new CD3DTextBoxConsoleRenderingContext( *mFont, 2, 2, 0xC0ffff20, 0x80000000 );
	CConsole::getChannel( "system" ).setRenderingContext( *mD3DConsoleCtx );
};


CSystem::~CSystem()
{
	delete mFont;
}


/**
 *  Initialization. Paired with shutdown().
 *  The window has been created and the IDirect3D9 interface has been
 *  created, but the device has not been created yet.
 */
HRESULT CSystem::initialize()
{
	//
	// resources

	mDeviceManager = new dingus::CDeviceResourceManager();

	CTextureBundle::getInstance().addDirectory( mDataPath + "tex/" );
	CMeshBundle::getInstance().addDirectory( mDataPath + "mesh/" );
	CEffectBundle::getInstance().addDirectory( mDataPath + "fx/" );

	//
	// device dependant resources

	mDeviceManager->addListener( CSharedTextureBundle::getInstance() );
	mDeviceManager->addListener( CSharedSurfaceBundle::getInstance() );
	mDeviceManager->addListener( CSharedVolumeBundle::getInstance() );
	mDeviceManager->addListener( CSharedMeshBundle::getInstance() );
	mDeviceManager->addListener( CTextureBundle::getInstance() );
	mDeviceManager->addListener( CMeshBundle::getInstance() );
	mDeviceManager->addListener( CEffectBundle::getInstance() );
	mDeviceManager->addListener( CVertexDeclBundle::getInstance() );

	return S_OK;
}


/**
 *  Called during device initialization, this code checks the display device
 *  for some minimum set of capabilities.
 */
HRESULT CSystem::checkDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT format )
{
	return appCheckDevice( caps, behavior, format ) ? S_OK : E_FAIL;
}



/**
 *  createDeviceObjects(). Paired with deleteDeviceObjects().
 *  The device has been created.  Resources that are not lost on
 *  Reset() can be created here -- resources in D3DPOOL_MANAGED, D3DPOOL_SCRATCH,
 *  or D3DPOOL_SYSTEMMEM.
 */
HRESULT CSystem::createDeviceObjects()
{
	assert( mD3DDevice );
	dingus::CD3DDevice& dev = dingus::CD3DDevice::getInstance();
	dev.setDevice( mD3DDevice );


	mFont->createDeviceObjects();
	mDeviceManager->createResource();
	
	//
	// renderer

	if( !G_RENDERCTX )
		G_RENDERCTX = new CRenderContext( *RGET_FX("_global") );

	dingus::CEffectParamsNotifier::getInstance().notify();

	return S_OK;
}


/**
 *  activateDeviceObjects(). Paired with passivateDeviceObjects().
 *  The device exists, but may have just been Reset().
 */
HRESULT CSystem::activateDeviceObjects()
{
	mFont->activateDeviceObjects();
	dingus::CD3DDevice::getInstance().activateDevice();
	mDeviceManager->activateResource();

	if( !mAppInited ) {
		appInitialize();
		mAppInited = true;
	}
	
	return S_OK;
}


/**
 *  Called once per frame.
 */
HRESULT CSystem::performOneTime()
{
	dingus::CD3DDevice& device = dingus::CD3DDevice::getInstance();

	dingus::CSystemClock& c = dingus::CSystemClock::getInstance();
	c.setTimes( mTime, mElapsedTime, c.getPerformCount()+1 );

	//
	// pipeline

	device.getStats().reset();
	appPerform();
	if( mShowFrameDeviceStats ) {
		dingus::CConsoleChannel& cc = dingus::CConsole::getChannel( "system" );
		cc.write( mFrameStats );
		cc.write( mDeviceStats );

		const dingus::CRenderStats& stats = device.getStats();
		char buf[300];
		sprintf( buf, "draws %i, fx %i, prims %i (%.1fM/s), verts %i",
			stats.getDrawCalls(),
			stats.getEffectChanges(),
			stats.getPrimsRendered(),
			stats.getPrimsRendered() * mFPS * 1.0e-6f,
			stats.getVerticesRendered() );
		cc.write( buf );

		const dingus::CRenderStats::SStateStats& ssc = stats.changes;
		const dingus::CRenderStats::SStateStats& ssf = stats.filtered;
		sprintf( buf, "vb:%i/%i ib:%i/%i dcl:%i/%i rt:%i/%i zs:%i/%i vs:%i/%i ps:%i/%i tr:%i lit:%i/%i",
			ssc.vbuffer, ssf.vbuffer, ssc.ibuffer, ssf.ibuffer, ssc.declarations, ssf.declarations,
			ssc.renderTarget, ssf.renderTarget, ssc.zStencil, ssf.zStencil,
			ssc.vsh, ssf.vsh, ssc.psh, ssf.psh,
			ssc.transforms, ssc.lighting, ssf.lighting );
		cc.write( buf );

		sprintf( buf, "t:%i/%i rs:%i/%i tss:%i/%i smp:%i/%i const:%i",
			ssc.textures, ssf.textures, ssc.renderStates, ssf.renderStates,
			ssc.textureStages, ssf.textureStages, ssc.samplers, ssf.samplers,
			ssc.vshConst+ssf.pshConst
			);
		cc.write( buf );

		dingus::CD3DDevice::getInstance().getDevice().BeginScene();
		mD3DConsoleCtx->flush();
		dingus::CD3DDevice::getInstance().getDevice().EndScene();
	}

	return S_OK;
}


/**
 *  Overrrides the main WndProc, so the sample can do custom message handling
 *  (e.g. processing mouse, keyboard, or menu commands).
 */
LRESULT CSystem::msgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static bool reloadKeyPressed = false;
	switch( msg ) {
	case WM_KEYDOWN:
		if( wParam == VK_F5 )
			reloadKeyPressed = true;
		break;
	case WM_KEYUP:
		if( wParam == VK_F5 ) {
			if( reloadKeyPressed ) {
				//mReloadableManager->reload();
				dingus::CEffectParamsNotifier::getInstance().notify();
			}
			reloadKeyPressed = false;
		}
	}
	return CD3DApplication::msgProc( hWnd, msg, wParam, lParam );
}


/**
 *  Invalidates device objects.  Paired with activateDeviceObjects().
 */
HRESULT CSystem::passivateDeviceObjects()
{
	mFont->passivateDeviceObjects();
	mDeviceManager->passivateResource();
	dingus::CD3DDevice::getInstance().passivateDevice();
	return S_OK;
}

/**
 *  Paired with createDeviceObjects().
 *  Called when the app is exiting, or the device is being changed,
 *  this function deletes any device dependent objects.
 */
HRESULT CSystem::deleteDeviceObjects()
{
	mFont->deleteDeviceObjects();
	mDeviceManager->deleteResource();
	dingus::CD3DDevice::getInstance().setDevice( NULL );
	return S_OK;
}


/**
 *  Paired with initialize().
 *  Called before the app exits, this function gives the app the chance
 *  to cleanup after itself.
 */
HRESULT CSystem::shutdown()
{
	appShutdown();

	mDeviceManager->clear();
	delete mDeviceManager;

	safeDelete( G_RENDERCTX );

	CTextureBundle::finalize();
	CMeshBundle::finalize();
	CEffectBundle::finalize();
	CVertexDeclBundle::finalize();
	CSharedTextureBundle::finalize();
	CSharedVolumeBundle::finalize();
	CSharedSurfaceBundle::finalize();
	CSharedMeshBundle::finalize();

	dingus::CSystemClock::finalize();
	if( mStdConsoleCtx )
		delete mStdConsoleCtx;
	dingus::CConsole::finalize();

	return S_OK;
}
