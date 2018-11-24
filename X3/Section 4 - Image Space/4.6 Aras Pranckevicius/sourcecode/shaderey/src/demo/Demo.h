#ifndef __MY_DEMO_H
#define __MY_DEMO_H

#include "../system/System.h"
#include "SimpleMesh.h"
#include "WaterMesh.h"
#include "QuadMesh.h"
#include "Scattering.h"

class CSkyMesh;
class CCloudMesh;
class CTerrain;
class CCameraAnim;


class CDemo : public CSystem {
public:
	CDemo();
	virtual ~CDemo();

protected:
	virtual void customSettingsInit( HWND dlg );
	virtual void customSettingsOK( HWND dlg );

	virtual bool appCheckDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat ) const;
	virtual void appInitialize();
	virtual void appPerform();
	virtual void appShutdown();

private:
	typedef std::vector<CSimpleMesh*>	TSimpleMeshVector;
	typedef std::vector<CWaterMesh*>	TWaterMeshVector;
	typedef std::vector<CCloudMesh*>	TCloudMeshVector;

	void	initLevel();
	void	initWaterMaps();
	void	initOtherSurfaces();
	void	initShadowMaps();
	void	initQuantizeMaps();
	void	renderShadowMap();
	void	renderLakeRefl();
	void	renderBase();
	void	processInput();
	void	computeSmallRT();

	// render everything, but no lakes - used in main and in lake reflections
	void	justRenderBaseNoLakes( eRenderMode mode, int terrainLod );

	void	fullscreenPass( const char* renderTarget, CQuadMesh& quad );

	void	renderWater( eRenderMode mode );
	void	renderStaticMeshes( eRenderMode mode );

private:
	/// Are we in benchmarking mode or not?
	bool		mBenchMode;
	int			mRenderedFrames;
	bool		mBenchOver;
	bool		mUseMRT; // flag to use MRT if available. It's strange, but sometimes non-MRT version is faster

	bool		mAnimate;

	CTerrain*			mTerrain;
	CSkyMesh*			mSky;
	TSimpleMeshVector	mSimpleMeshes;
	TWaterMeshVector	mWaterMeshes;
	TCloudMeshVector	mCloudMeshes;
	SVector3			mLightDirNeg;

	CCameraAnim*	mCameraAnim;
	float			mCameraFov;

	SMatrix4x4	mMatProjShadow[4]; // for antialiased shadows
	SMatrix4x4	mMatTexProj; // for camera-projected textures
	SVector3	mCamYawPitchRoll;
	SVector3	mCamPosition;
	SMatrix4x4	mMatCamera;
	bool		mWireframe;

	SVector4	mQuadOffset;
	float		mBleedOffsetX;
	float		mBleedOffsetY;

	CQuadMesh*	mQuadBleed;
	CQuadMesh*	mQuadFinal;
	//CQuadMesh*	mQuadRGB2HSV;
	//CQuadMesh*	mQuadHSV2RGB;
	CQuadMesh*	mQuadEdges;

	CSun		mSun;
	CAtmosphere	mAtmosphere;
	SVector3	mSunColorDivBetaSum;
	float		mFloatTime;
};



#endif
