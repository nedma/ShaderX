#include "stdafx.h"
#include "Demo.h"

#include <dingus/math/Plane.h>
#include "Terrain.h"
#include "SkyMesh.h"
#include "CloudMesh.h"
#include "../anim/CameraAnim.h"
#include "Projector.h"
#include "Resources.h"
#include "ColorOps.h"
#include <dingus/console/Console.h>
#include "../resource.h"
#include <dingus/dxutils/D3DFont.h>


CDemo::CDemo()
:	mTerrain(0), mSky(0)
{
	mBenchMode = false;
	//mStartFullscreen = true;
	mUseMRT = false;
	mWindowTitle = "Shaderey - Beyond3D/ATI DX9 competition entry by Aras 'NeARAZ' Pranckevicius";
}

CDemo::~CDemo()
{
}

bool CDemo::appCheckDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT format ) const
{
	// if doesn't support VS1.1, fail with HW VP
	if( (caps.VertexShaderVersion < D3DVS_VERSION(1,1)) && (behavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) )
		return false;
	// fail if no PS2.0
	if( caps.PixelShaderVersion < D3DPS_VERSION(2,0) )
		return false;
	// fail if REF
	if( caps.DeviceType != D3DDEVTYPE_HAL )
		return false;
	return true;
}

void CDemo::customSettingsInit( HWND dlg )
{
	CheckDlgButton( dlg, IDC_CHK_BENCHMARK, BST_UNCHECKED );
	CheckDlgButton( dlg, IDC_CHK_MRT, BST_UNCHECKED );
}

void CDemo::customSettingsOK( HWND dlg )
{
	mBenchMode = IsDlgButtonChecked(dlg,IDC_CHK_BENCHMARK) ? true : false;
	mUseMRT = IsDlgButtonChecked(dlg,IDC_CHK_MRT) ? true : false;
	if( mBenchMode )
		mVSyncFullscreen = false;
}

CConsoleChannel& CON_INFO = CConsole::getChannel("info");

void CDemo::appInitialize()
{
	mWireframe = false;
	
	mCamYawPitchRoll.set( D3DX_PI/2, 0.15f, 0.0f );
	mCamPosition.set(-50.0f,35.0f,-120.0f);
	//mCamPosition.set(-54.0865f,31.3442f,-100.094f);
	//mCamYawPitchRoll.set(2.24454f,-0.562182f,0.0f);
	//mCamPosition.set(74.4553f,27.936f,-8.52147f);
	//mCamYawPitchRoll.set(0.6699f,-0.409292f,0.0f);
	D3DXMatrixRotationYawPitchRoll( &mMatCamera, mCamYawPitchRoll.x, mCamYawPitchRoll.y, mCamYawPitchRoll.z );
	mMatCamera.getOrigin() = mCamPosition;

	mCameraAnim = new CCameraAnim( mMatCamera );
	mCameraAnim->load( "data/camera.txt" );

	mSun.setParams( D3DX_PI*0.15f, 0.0f, 100.0f );

	// global params
	CON_INFO.write( "creating global params..." );
	CRenderCamera& camera = G_RENDERCTX->getCamera();
	CEffectParams& globalep = G_RENDERCTX->getGlobalParams();
	globalep.addMatrix4x4Ref( "mShadowProj0", mMatProjShadow[0] );
	globalep.addMatrix4x4Ref( "mShadowProj1", mMatProjShadow[1] );
	globalep.addMatrix4x4Ref( "mShadowProj2", mMatProjShadow[2] );
	globalep.addMatrix4x4Ref( "mShadowProj3", mMatProjShadow[3] );
	globalep.addMatrix4x4Ref( "mTexProj", mMatTexProj );
	globalep.addVector3Ref( "vLightDir", mLightDirNeg );
	globalep.addVector4Ref( "vQuadOffset", mQuadOffset );
	globalep.addVector3Ref( "vScatterBetaR", mAtmosphere.getMulBetaRay() );
	globalep.addVector3Ref( "vScatterBetaM", mAtmosphere.getMulBetaMie() );
	globalep.addVector3Ref( "vScatterBetaSum", mAtmosphere.getMulBetaRayMieSum() );
	globalep.addVector3Ref( "vScatterBetaR_", mAtmosphere.getMulBetaRay_() );
	globalep.addVector3Ref( "vScatterBetaM_", mAtmosphere.getMulBetaMie_() );
	globalep.addVector4Ref( "vScatterHGg", mAtmosphere.getHGg() );
	globalep.addVector3Ref( "vScatterSunColor", mSun.getColorMulIntensity() );
	globalep.addVector3Ref( "vScatterSunColorDivBetaSum", mSunColorDivBetaSum );
	globalep.addFloatRef( "fScatterExtMult", &mAtmosphere.getExtinctionMult() );
	globalep.addFloatRef( "fScatterInscatMult", &mAtmosphere.getInscatteringMult() );
	globalep.addFloatRef( "fTime", &mFloatTime );

	// shadow maps
	initShadowMaps();

	// water maps
	initWaterMaps();

	// other RTs
	initOtherSurfaces();

	// quantize maps
	initQuantizeMaps();

	// terrain
	CON_INFO.write( "creating terrain..." );
	mTerrain = new CTerrain( RID_HEIGHTMAP, 60.0f );
	mTerrain->getParams(BASE).setEffect( *RGET_FX("terrain") );
	mTerrain->getParams(BASE).addTexture( "tBase0", *RGET_TEX("grassA") );
	mTerrain->getParams(BASE).addTexture( "tBase1", *RGET_TEX("grassB") );
	mTerrain->getParams(BASE).addTexture( "tBase2", *RGET_TEX("dirtA") );
	mTerrain->getParams(BASE).addTexture( "tShadowMap", *RGET_S_TEX(RID_SHADOWMAP) );
	mTerrain->getParams(BASE).addTexture( "tNoise0", *RGET_TEX("noise0") );
	mTerrain->getParams(COLOR).setEffect( *RGET_FX("terrainColorOnly") );
	mTerrain->getParams(COLOR).addTexture( "tBase0", *RGET_TEX("grassA") );
	mTerrain->getParams(COLOR).addTexture( "tBase1", *RGET_TEX("grassB") );
	mTerrain->getParams(COLOR).addTexture( "tBase2", *RGET_TEX("dirtA") );
	mTerrain->getParams(COLOR).addTexture( "tShadowMap", *RGET_S_TEX(RID_SHADOWMAP) );
	mTerrain->getParams(COLOR).addTexture( "tNoise0", *RGET_TEX("noise0") );
	mTerrain->getParams(NORMALZ).setEffect( *RGET_FX("terrainNormalZOnly") );
	mTerrain->getParams(NORMALZ).addTexture( "tShadowMap", *RGET_S_TEX(RID_SHADOWMAP) );

	// sky
	CON_INFO.write( "creating sky..." );
	mSky = new CSkyMesh();

	// processing screen quads
	CON_INFO.write( "creating postprocessing fxs..." );
	mQuadEdges = new CQuadMesh( "imgEdgeDetect" );
	mQuadEdges->getParams().addTexture( "tBase", *RGET_S_TEX(RID_NORMALZ) );
	mQuadEdges->getParams().addTexture( "tHatch", *RGET_TEX("hatch") );

	mQuadBleed = new CQuadMesh( "imgBleed" );
	mQuadBleed->getParams().addTexture( "tBase", *RGET_S_TEX(RID_FIXEDRT512) );
	mQuadBleed->getParams().addTexture( "tBleedB", *RGET_TEX("bleedB") );
	mQuadBleed->getParams().addTexture( "tBleedC", *RGET_TEX("bleedC") );
	mQuadBleed->getParams().addVolumeTexture( "tRGB2HSV", *RGET_S_VOLTEX(RID_RGB2HSV) );
	mQuadBleed->getParams().addVolumeTexture( "tHSV2RGB", *RGET_S_VOLTEX(RID_HSV2RGB) );
	mQuadBleed->getParams().addFloatRef( "fOffsetX", &mBleedOffsetX );
	mQuadBleed->getParams().addFloatRef( "fOffsetY", &mBleedOffsetY );

	mQuadFinal = new CQuadMesh( "imgComposite" );
	mQuadFinal->getParams().addTexture( "tBase", *RGET_S_TEX(RID_BASERT) );
	mQuadFinal->getParams().addTexture( "tBleed", *RGET_S_TEX(RID_BLEEDACC) );
	mQuadFinal->getParams().addTexture( "tEdges", *RGET_S_TEX(RID_EDGES) );

	// static level
	CON_INFO.write( "creating objects..." );
	initLevel();

	mRenderedFrames = 0;
	mBenchOver = false;
	mAnimate = true;
	CON_INFO.write( "initialized" );
}

void CDemo::initOtherSurfaces()
{
	CON_INFO.write( "creating other RTs..." );
	CSharedTextureBundle& stb = CSharedTextureBundle::getInstance();
	CSharedSurfaceBundle& ssb = CSharedSurfaceBundle::getInstance();

	ITextureCreator* creatorBase = new CScreenBasedTextureCreator(1.0f, 1.0f, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT );
	ITextureCreator* creator512 = new CFixedTextureCreator(512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT );
	stb.registerTexture( RID_BASERT, creatorBase );
	stb.registerTexture( RID_FIXEDRT512, creator512 );
	stb.registerTexture( RID_QUANTIZED, creator512 );
	stb.registerTexture( RID_BLEEDACC, creator512 );
	stb.registerTexture( RID_NORMALZ, creatorBase );
	stb.registerTexture( RID_EDGES, creatorBase );

	ssb.registerSurface( RID_BASERT, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_BASERT) ) );
	ssb.registerSurface( RID_FIXEDRT512, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_FIXEDRT512) ) );
	ssb.registerSurface( RID_QUANTIZED, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_QUANTIZED) ) );
	ssb.registerSurface( RID_BLEEDACC, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_BLEEDACC) ) );
	ssb.registerSurface( RID_NORMALZ, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_NORMALZ) ) );
	ssb.registerSurface( RID_EDGES, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_EDGES) ) );
	ssb.registerSurface( RID_LAKEREFL, new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_LAKEREFL) ) );
}

void CDemo::initShadowMaps()
{
	CON_INFO.write( "creating shadowmap RTs..." );
	CSharedTextureBundle& stb = CSharedTextureBundle::getInstance();
	CSharedSurfaceBundle& ssb = CSharedSurfaceBundle::getInstance();

	// register shadow map RT
	stb.registerTexture( RID_SHADOWMAP,
		new CFixedTextureCreator( SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A4R4G4B4, D3DPOOL_DEFAULT ) );
	ssb.registerSurface( RID_SHADOWMAP,
		new CTextureLevelSurfaceCreator( *RGET_S_TEX(RID_SHADOWMAP), 0 ) );
}

void CDemo::initWaterMaps()
{
	CON_INFO.write( "creating reflection RTs..." );
	CSharedTextureBundle& stb = CSharedTextureBundle::getInstance();
	CSharedSurfaceBundle& ssb = CSharedSurfaceBundle::getInstance();
	stb.registerTexture( RID_LAKEREFL, new CFixedTextureCreator(LAKE_REFL_SIZE, LAKE_REFL_SIZE, 1, D3DUSAGE_RENDERTARGET /*| D3DUSAGE_AUTOGENMIPMAP*/, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT ) );
	ssb.registerSurface( RID_LAKEREFLZ, new CFixedSurfaceCreator( LAKE_REFL_SIZE, LAKE_REFL_SIZE, true, D3DFMT_D16 ) );
}

class CVolumeFuncCreator : public CFixedVolumeCreator {
public:
	CVolumeFuncCreator( int size, LPD3DXFILL3D func )
		: CFixedVolumeCreator(size,size,size, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ), mFunc(func) { }

	virtual IDirect3DVolumeTexture9* createTexture() {
		IDirect3DVolumeTexture9* tex = CFixedVolumeCreator::createTexture();
		D3DXFillVolumeTexture( tex, mFunc, 0 );
		return tex;
	}

private:
	LPD3DXFILL3D	mFunc;
};

void CDemo::initQuantizeMaps()
{
	CON_INFO.write( "creating volume textures..." );
	CSharedVolumeBundle& svb = CSharedVolumeBundle::getInstance();

	const int VOL_SIZE = 32;
	// create volume RGB->HSV and HSV->RGB textures
	svb.registerVolume( RID_RGB2HSV, new CVolumeFuncCreator( VOL_SIZE, gVolumeFillerRGB2HSV ) );
	svb.registerVolume( RID_HSV2RGB, new CVolumeFuncCreator( VOL_SIZE, gVolumeFillerHSV2RGB ) );
}

void CDemo::initLevel()
{
	int i;

	//
	// read scene file and create meshes

	FILE* f = fopen( "data/scene.txt", "rt" );
	assert( f );
	int count;
	fscanf( f, "count = %i\n", &count );
	for( i = 0; i < count; ++i ) {
		char name[100];
		SVector3 pos; SQuaternion rot;
		fscanf( f, "name=%s pos=%f %f %f rot=%f %f %f %f\n", name, &pos.x, &pos.y, &pos.z, &rot.x, &rot.y, &rot.z, &rot.w );
		std::string sname = name;
		CSimpleMesh* mesh = 0;
		if( sname=="Pusis1" || sname=="Pusis2" ) {
			mesh = new CSimpleMesh( sname );
			const char* tex0[] = { "medisA", "medisB" };
			const char* tex1[] = { "lapaiA", "lapaiC", "lapaiC" };
			mesh->addMeshPart( 0, tex0[rand()%2] );
			mesh->addMeshPart( 1, tex1[rand()%3] );
		} else if( sname=="Medis1" ) {
			mesh = new CSimpleMesh( sname );
			const char* tex0[] = { "medisA", "medisB" };
			const char* tex1[] = { "lapaiA", "lapaiB" };
			mesh->addMeshPart( 0, tex0[rand()%2] );
			mesh->addMeshPart( 1, tex1[rand()%2] );
		} else if( sname=="Pusis3" ) {
			mesh = new CSimpleMesh( sname );
			const char* tex1[] = { "medisA", "medisB" };
			mesh->addMeshPart( 1, tex1[rand()%2] );
			mesh->addMeshPart( 0, "lapaiC" );
		} else if( sname=="House1" ) {
			mesh = new CSimpleMesh( sname );
			mesh->addMeshPart( 0, "medisB" );
			mesh->addMeshPart( 1, "siaudai" );
		} else if( sname=="Lake1" || sname=="Lake2" || sname=="Lake3" || sname=="Lake4" || sname=="Lake5" ) {
			CWaterMesh* wmesh = new CWaterMesh( sname );
			wmesh->mMatrix = SMatrix4x4( pos, rot );
			mWaterMeshes.push_back( wmesh );
		}
		if( mesh ) {
			mesh->mMatrix = SMatrix4x4( pos, rot );
			mSimpleMeshes.push_back( mesh );
		}
	}
	fclose( f );

	//
	// create some clouds

	const int CLOUD_COUNT = 20;
	mCloudMeshes.reserve( CLOUD_COUNT );
	const char* cnames[2] = { "CloudA", "CloudB" };
	for( i = 0; i < CLOUD_COUNT; ++i ) {
		CResourceId meshID = cnames[rand()%2];
		CCloudMesh*	cloud = new CCloudMesh( meshID );
		cloud->mMatrix.identify();
		cloud->mMatrix.getOrigin().set(
			(rand()%600)-300,
			130 + (rand()&25),
			(rand()%600)-300 );
		mCloudMeshes.push_back( cloud );
	}
}

void CDemo::appShutdown()
{
	safeDelete( mTerrain );
	safeDelete( mSky );
}

void CDemo::processInput()
{
	//
	// hacky keyboard

	const float dt = mElapsedTime;
	const float d_move = 12.0f * dt;
	const float d_rise = 6.0f * dt;
	const float d_turn = D3DX_PI*0.4f * dt;

	float oldY = mCamPosition.y;
	if( GetAsyncKeyState(VK_UP) ) {
		mCamPosition += mMatCamera.getAxisZ() * d_move;
		mCamPosition.y = oldY;
	}
	if( GetAsyncKeyState(VK_DOWN) ) {
		mCamPosition -= mMatCamera.getAxisZ() * d_move;
		mCamPosition.y = oldY;
	}
	if( GetAsyncKeyState('A') )
		mCamPosition.y += d_rise;
	if( GetAsyncKeyState('Z') )
		mCamPosition.y -= d_rise;
	if( GetAsyncKeyState(VK_LEFT) )
		mCamYawPitchRoll.x -= d_turn;
	if( GetAsyncKeyState(VK_RIGHT) )
		mCamYawPitchRoll.x += d_turn;
	if( GetAsyncKeyState('S') )
		mCamYawPitchRoll.y -= d_turn*0.5;
	if( GetAsyncKeyState('X') )
		mCamYawPitchRoll.y += d_turn*0.5;

	float sunTheta = mSun.getTheta();
	if( GetAsyncKeyState(VK_NEXT) )
		sunTheta += 0.5f*dt;
	if( GetAsyncKeyState(VK_PRIOR) )
		sunTheta -= 0.5f*dt;
	if( sunTheta < -D3DX_PI*0.48f )
		sunTheta = -D3DX_PI*0.48f;
	if( sunTheta > D3DX_PI*0.48f )
		sunTheta = D3DX_PI*0.48f;
	mSun.setParams( sunTheta, mSun.getPhi(), mSun.getIntensity() );

	static bool justPressedW = false;
	if( GetAsyncKeyState('W') ) {
		if( !justPressedW )
			mWireframe = !mWireframe;
		justPressedW = true;
	} else
		justPressedW = false;

	static bool justPressedQ = false;
	if( GetAsyncKeyState('Q') ) {
		if( !justPressedQ )
			mAnimate = !mAnimate;
		justPressedQ = true;
	} else
		justPressedQ = false;

	D3DXMatrixRotationYawPitchRoll( &mMatCamera, mCamYawPitchRoll.x, mCamYawPitchRoll.y, mCamYawPitchRoll.z );
	mMatCamera.getOrigin() = mCamPosition;

	// dump camera params
	static bool justPressedP = false;
	if( GetAsyncKeyState('P') ) {
		if( !justPressedP ) {
			CON_INFO << "camera pos " << mCamPosition.x << "  " << mCamPosition.y << "  " << mCamPosition.z << endl;
			CON_INFO << "camera rot " << mCamYawPitchRoll.x << "  " << mCamYawPitchRoll.y << "  " << mCamYawPitchRoll.z << endl;
		}
		justPressedP = true;
	} else
		justPressedP = false;
}

void CDemo::appPerform()
{
	static int initSkipFrames = 0;
	const int SKIP_FRAMES = 20;
	bool skipElapsed = true;
	static double startTime = 0.0;
	if( initSkipFrames < SKIP_FRAMES ) {
		if( mBenchMode || initSkipFrames==0 ) {
			startTime = mTime;
			mRenderedFrames = 0;
		}
		++initSkipFrames;
		skipElapsed = false;
	}
	double timeFromStart = mTime - startTime;
	double duration = mBenchMode ? 20.0 : 160.0;
	double relTime = timeFromStart / duration;

	if( relTime > 1.0f )
		mBenchOver = true;

	mFloatTime = mTime;

	// input
	if( !mBenchMode )
		processInput();

	// animate
	float camFov = D3DX_PI * 0.25f;
	if( mBenchMode || mAnimate ) {
		float sunTheta;
		mCameraAnim->animate( relTime, camFov, sunTheta );
		mSun.setParams( sunTheta, mSun.getPhi(), mSun.getIntensity() );
	}
	mSunColorDivBetaSum = mSun.getColorMulIntensity();
	mSunColorDivBetaSum.x *= mAtmosphere.getMulInvBetaRayMieSum().x;
	mSunColorDivBetaSum.y *= mAtmosphere.getMulInvBetaRayMieSum().y;
	mSunColorDivBetaSum.z *= mAtmosphere.getMulInvBetaRayMieSum().z;
	
	// quad offset
	CD3DDevice& dx = CD3DDevice::getInstance();
	float pixX = 1.0f / dx.getBackBufferWidth();
	float pixY = 1.0f / dx.getBackBufferHeight();
	mQuadOffset.set( pixX * 0.5f, -pixY * 0.5f, 0.0f, 0.0f );

	dx.getDevice().SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	// light dir from sun
	mLightDirNeg = mSun.getDirection();

	// move clouds
	/*
	for( int c = 0; c < mCloudMeshes.size(); ++c ) {
		SMatrix4x4& m = mCloudMeshes[c]->mMatrix;
		SVector3& o = m.getOrigin();
		SVector3 v;
		v.x = -10.0f + (170 - o.y) * 0.20f;
		v.y = 0.0f;
		v.z = -2.0f + (170 - o.y) * 0.02f;
		v *= mElapsedTime;
		o += v;
		if( o.x > 400 ) o.x = -400;
		else if( o.x < -400 ) o.x = 400;
		if( o.z > 400 ) o.z = -400;
		else if( o.z < -400 ) o.z = 400;
	}
	*/

	// shadow map
	renderShadowMap();

	// camera
	CRenderCamera& camera = G_RENDERCTX->getCamera();
	SMatrix4x4 proj;
	float aspect = dx.getBackBufferAspect();
	D3DXMatrixPerspectiveFovLH( &proj, camFov, aspect, 0.1f, 1200.0f );
	camera.setCameraMatrix( mMatCamera );
	camera.setProjectionMatrix( proj );

	// lake reflection
	renderLakeRefl();

	// camera
	camera.setCameraMatrix( mMatCamera );
	CProjector::computeTextureProjection( mMatCamera, mMatTexProj );

	SVector3 lookDir = camera.getCameraMatrix().getAxisZ();
	lookDir.y = 0.0f;
	lookDir.normalize();
	float phi = atan2f( lookDir.z, lookDir.x );
	mBleedOffsetX = -phi/(camFov*aspect);
	const float bleedVDist = 50.0f;
	SVector3 target = camera.getCameraMatrix().getOrigin() + camera.getCameraMatrix().getAxisZ() * bleedVDist;
	mBleedOffsetY = -target.y / (bleedVDist * tanf(camFov));
	
	// base
	renderBase();

	dx.setZStencil( NULL );

	// build small RT
	computeSmallRT();

	//// RGB->HSV
	//fullscreenPass( RID_QUANTIZED, *mQuadRGB2HSV );
	// compute bleeds
	fullscreenPass( RID_BLEEDACC, *mQuadBleed );
	//// back to RGB
	//fullscreenPass( RID_FIXEDRT512, *mQuadHSV2RGB );

	// detect edges
	fullscreenPass( RID_EDGES, *mQuadEdges );


	// final composite
	dx.sceneBegin();
	dx.setDefaultRenderTarget();
	G_RENDERCTX->directBegin();
	G_RENDERCTX->directRender( *mQuadFinal );
	G_RENDERCTX->directEnd();

	// some text
	dx.resetCachedState();

	CD3DFont& font = getFont();
	char buf[300];
	if( mBenchMode ) {
		D3DCOLOR textColor = 0xFFC00000;
		sprintf( buf, "BENCHMARK MODE. %ix%i", mBackBuffer.Width, mBackBuffer.Height );
		font.drawText( 5, 5+16*0, textColor, buf );
		if( !skipElapsed ) {
			font.drawText( 5, 5+16*1, textColor, "WARMING UP..." );
		} else if( mBenchOver ) {
			sprintf( buf, "BENCHMARK OVER. SCORE %i", mRenderedFrames );
			font.drawText( 5, 5+16*1, textColor, buf );
		} else {
			sprintf( buf, "%i%% completed", (int)(relTime*100.0) );
			font.drawText( 5, 5+16*1, textColor, buf );
		}
	} else {
		const CRenderStats& stats = dx.getStats();
		D3DCOLOR textColor = 0xA0101010;
		font.drawText( 5, 5+16*0, textColor, mFrameStats );
		font.drawText( 5, 5+16*1, textColor, mDeviceStats );
		char buf[200];
		sprintf( buf,
			"%.1f Mprims/s;  %i DI/DIPs;  %i RTchanges;  %.1f Kverts;  %.1f Kprims",
			(stats.getPrimsRendered() * mFPS) / 1000000.0f,
			stats.getDrawCalls(),
			stats.changes.renderTarget,
			stats.getVerticesRendered() / 1000.0f,
			stats.getPrimsRendered() / 1000.0f );
		font.drawText( 5, 5+16*2, textColor, buf );
	}
	dx.sceneEnd();

	if( !mBenchOver )
		++mRenderedFrames;
}

void CDemo::justRenderBaseNoLakes( eRenderMode mode, int terrainLod )
{
	CD3DDevice& dx = CD3DDevice::getInstance();

	mTerrain->render( terrainLod, mode );
	if( mode != NORMALZ )
		mSky->render();

	// clouds
	assert( !mCloudMeshes.empty() );
	for( int i = 0; i < mCloudMeshes.size(); ++i )
		mCloudMeshes[i]->render( mode );

	// static meshes
	renderStaticMeshes( mode );
}


void CDemo::renderBase()
{
	CD3DDevice& dx = CD3DDevice::getInstance();
	bool haveMRT = (dx.getMRTCount() >= 2);
	if( !mUseMRT )
		haveMRT = false;

	// clear color for normal/z to match negative light direction
	D3DXCOLOR clearColor;
	clearColor.r = (mLightDirNeg.x + 1.0f) * 0.5f;
	clearColor.g = (mLightDirNeg.y + 1.0f) * 0.5f;
	clearColor.b = (mLightDirNeg.z + 1.0f) * 0.5f;
	clearColor.a = 1.0f;

	dx.getDevice().SetRenderState( D3DRS_FILLMODE, mWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );

	dx.setDefaultZStencil();
	dx.setRenderTarget( RGET_S_SURF(RID_BASERT), 0 );

	if( haveMRT ) {
		// set normal/Z RT1
		dx.setRenderTarget( RGET_S_SURF(RID_NORMALZ), 1 );
		dx.clearTargets( true, true, false, clearColor, 1.0f, 0L );
		dx.sceneBegin();
		justRenderBaseNoLakes( BASE, 0 );
		renderWater( BASE );
		G_RENDERCTX->perform();
		dx.setRenderTarget( NULL, 1 );
		dx.sceneEnd();
	} else {
		// fallback to separate passes
		// color pass
		dx.clearTargets( false, true, false, clearColor, 1.0f, 0L );
		dx.sceneBegin();
		justRenderBaseNoLakes( COLOR, 0 );
		renderWater( COLOR );
		G_RENDERCTX->perform();
		dx.sceneEnd();
		// normal/Z pass
		dx.setRenderTarget( RGET_S_SURF(RID_NORMALZ), 0 );
		//dx.setRenderTarget( RGET_S_SURF(RID_BASERT), 0 );
		dx.clearTargets( true, true, false, clearColor/*0xFF000000*/, 1.0f, 0L );
		dx.sceneBegin();
		justRenderBaseNoLakes( NORMALZ, 0 );
		renderWater( NORMALZ );
		G_RENDERCTX->perform();
		dx.sceneEnd();
	}

	dx.getDevice().SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

void CDemo::renderWater( eRenderMode mode )
{
	//CD3DDevice::getInstance().getDevice().SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

	assert( !mWaterMeshes.empty() );
	int n = mWaterMeshes.size();
	for( int i = 0; i < n; ++i )
		mWaterMeshes[i]->render( mode );
}

void CDemo::renderStaticMeshes( eRenderMode mode )
{
	//CD3DDevice::getInstance().getDevice().SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
	assert( !mSimpleMeshes.empty() );
	for( int i = 0; i < mSimpleMeshes.size(); ++i )
		mSimpleMeshes[i]->render( mode );
}

void CDemo::renderShadowMap()
{
	CRenderCamera& camera = G_RENDERCTX->getCamera();
	CD3DDevice& dx = CD3DDevice::getInstance();

	//
	// setup shadow map transform params

	const float SHADOW_CENTER_FWD = 100.0f;
	const float SHADOW_CENTER_Y = 40.0f;
	const float SHADOW_EYE_DIST = 260.0f;
	const float SHADOW_REGION = 300.0f;
	SVector3 shadowCenter = mMatCamera.getOrigin() + mMatCamera.getAxisZ() * SHADOW_CENTER_FWD;
	shadowCenter.y = SHADOW_CENTER_Y;
	SVector3 shadowEye = shadowCenter + mLightDirNeg * SHADOW_EYE_DIST;

	// camera matrix
	SMatrix4x4 matCam;
	matCam.identify();
	matCam.getOrigin() = shadowEye;
	matCam.getAxisZ() = -mLightDirNeg;
	matCam.getAxisY().set(0,1,0);
	matCam.getAxisX() = matCam.getAxisY().cross( matCam.getAxisZ() ).getNormalized();
	matCam.getAxisY() = matCam.getAxisZ().cross( matCam.getAxisX() ).getNormalized();
	
	// now, set camera origin into "quantized" grid on the camera plane, so as we move,
	// the texels of the shadowmap don't fiddle

	// project point (0,0,0) into camera space. we could do this with view matrix,
	// but that would require inversion, and one more inversion later (when we'll adjust the
	// camera matrix)
	SVector3 line = -shadowEye;
	float projX = matCam.getAxisX().dot( line );
	float projY = matCam.getAxisY().dot( line );
	float projZ = matCam.getAxisZ().dot( line );
	const float TEXEL_QUANTIZE = SHADOW_REGION / (SHADOW_MAP_SIZE-2); // one pixel border
	const float modX = fmodf( projX, TEXEL_QUANTIZE );
	const float modY = fmodf( projY, TEXEL_QUANTIZE );
	const float modZ = fmodf( projZ, TEXEL_QUANTIZE );
	projX -= modX;
	projY -= modY;
	projZ -= modZ;
	SVector3 adjOrigin = matCam.getOrigin();
	adjOrigin += matCam.getAxisX() * projX;
	adjOrigin += matCam.getAxisY() * projY;
	adjOrigin += matCam.getAxisZ() * projZ;
	matCam.getOrigin() -= adjOrigin;

	camera.setCameraMatrix( matCam );
	SMatrix4x4 matProj;
	D3DXMatrixOrthoLH( &matProj, SHADOW_REGION, SHADOW_REGION, 0.1f, SHADOW_EYE_DIST*2.0f );
	camera.setProjectionMatrix( matProj );

	// texture adjustment matrix
	// jitter 4 samples
	float fC1 = -0.1f / SHADOW_MAP_SIZE;
	float fC2 = 0.9f / SHADOW_MAP_SIZE;
	SMatrix4x4 matTexAdj;
	matTexAdj.identify();
	matTexAdj._11 = 0.5f;
	matTexAdj._22 = -0.5f;
	matTexAdj._33 = 0.0f;
	matTexAdj._43 = 1.0f;

	matTexAdj._41 = 0.5f+fC1; matTexAdj._42 = 0.5f+fC1;
	mMatProjShadow[0] = camera.getViewProjMatrix() * matTexAdj;
	matTexAdj._41 = 0.5f+fC1; matTexAdj._42 = 0.5f+fC2;
	mMatProjShadow[1] = camera.getViewProjMatrix() * matTexAdj;
	matTexAdj._41 = 0.5f+fC2; matTexAdj._42 = 0.5f+fC1;
	mMatProjShadow[2] = camera.getViewProjMatrix() * matTexAdj;
	matTexAdj._41 = 0.5f+fC2; matTexAdj._42 = 0.5f+fC2;
	mMatProjShadow[3] = camera.getViewProjMatrix() * matTexAdj;

	//
	// render into shadow RT

	dx.setRenderTarget( RGET_S_SURF(RID_SHADOWMAP), 0 );
	dx.setZStencil( NULL );
	dx.clearTargets( true, false, false, 0xFFffffff, 1.0f, 0L );
	dx.sceneBegin();

	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 1;
	vp.Height = vp.Width = SHADOW_MAP_SIZE-2;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	dx.getDevice().SetViewport( &vp );

	renderStaticMeshes( SHADOW );
	G_RENDERCTX->perform();
	
	dx.sceneEnd();
}

void CDemo::renderLakeRefl()
{
	CRenderCamera& camera = G_RENDERCTX->getCamera();
	CD3DDevice& dx = CD3DDevice::getInstance();

	//
	// find nearest lake to the viewer

	CWaterMesh* lake = 0;
	float		minDistSq = 1.0e10f;
	int			nlakes = mWaterMeshes.size();
	for( int i = 0; i < nlakes; ++i ) {
		CWaterMesh* l = mWaterMeshes[i];
		if( l->mAABB.frustumCull( l->mMatrix, camera.getViewProjMatrix() ) )
			continue;
		SVector3 v = l->mMatrix.getOrigin() - camera.getCameraMatrix().getOrigin();
		float distSq = v.lengthSq();
		if( distSq < minDistSq ) {
			minDistSq = distSq;
			lake = l;
		}
	}
	if( !lake )
		return;

	//
	// find lake's XZ plane, reflect camera, clip plane etc.

	const SMatrix4x4& lakeMat = lake->mMatrix;

	SPlane lakePlane( lakeMat.getOrigin() + SVector3(0,0.5f,0), lakeMat.getAxisY() );

	SMatrix4x4 reflectMat;
	D3DXMatrixReflect( &reflectMat, &lakePlane );
	SMatrix4x4 matCam;
	matCam = camera.getCameraMatrix() * reflectMat;
	camera.setCameraMatrix( matCam );

	SMatrix4x4 matFoo = camera.getViewProjMatrix();
	matFoo.invert();
	matFoo.transpose();
	SPlane clipPlane;
	D3DXPlaneTransform( &clipPlane, &lakePlane, &matFoo );

	dx.getDevice().SetClipPlane( 0, clipPlane );

	//
	// reverse culling, enable clip plane

	dx.getDevice().SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	dx.getDevice().SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );

	//
	// reflection RT

	dx.setRenderTarget( RGET_S_SURF(RID_LAKEREFL), 0 );
	dx.setZStencil( RGET_S_SURF(RID_LAKEREFLZ) );
	dx.clearTargets( false, true, false, 0xFFffffff, 1.0f, 0L );
	dx.sceneBegin();

	justRenderBaseNoLakes( COLOR, 3 );
	G_RENDERCTX->perform();

	dx.sceneEnd();
	
	//
	// restore culling, disable clip planes

	dx.getDevice().SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	dx.getDevice().SetRenderState( D3DRS_CLIPPLANEENABLE, 0 );
}

void CDemo::computeSmallRT()
{
	CD3DDevice& dx = CD3DDevice::getInstance();

	IDirect3DSurface9* rtBig = RGET_S_SURF(RID_BASERT)->getObject();
	IDirect3DSurface9* rtSmall = RGET_S_SURF(RID_FIXEDRT512)->getObject();
	HRESULT hr = dx.getDevice().StretchRect( rtBig, 0, rtSmall, 0, D3DTEXF_NONE );
}

void CDemo::fullscreenPass( const char* renderTarget, CQuadMesh& quad )
{
	CD3DDevice& dx = CD3DDevice::getInstance();
	dx.setRenderTarget( RGET_S_SURF(renderTarget), 0 );
	dx.sceneBegin();
	G_RENDERCTX->directBegin();
	G_RENDERCTX->directRender( quad );
	G_RENDERCTX->directEnd();
	dx.sceneEnd();
}
