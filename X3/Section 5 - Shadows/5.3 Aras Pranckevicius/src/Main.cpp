#include "stdafx.h"
#include "common/resource.h"
#include "Mesh.h"
#include "VisibilityCalc.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "utils/fastvector.h"
#include "Effect.h"

//---------------------------------------------------------------------------

#define PROJECTILE_LIFETIME 2.0f

/**
 *  Simple "projectile" - a moving light source with limited lifetime.
 */
struct SProjectile {
public:
	SProjectile( const SMatrix4x4& camera ) {
		position = camera.getOrigin();
		velocity = camera.getAxisZ() * 6.0f;
		time2live = PROJECTILE_LIFETIME;
	}
	/** @return true if still alive. */
	bool update( float dt ) {
		color.set( 0.8f, 0.5f, 0.1f, 1.0f );
		if( time2live < PROJECTILE_LIFETIME/2 ) {
			color *= time2live / (PROJECTILE_LIFETIME/2);
		}
		position += velocity * dt;
		time2live -= dt;
		return (time2live > 0.0f);
	}
public:
	SVector3	position;
	SVector3	velocity;
	SVector4	color;
	float		time2live;
};



//---------------------------------------------------------------------------

/**
 *  Main demo app.
 */
class CMyD3DApplication : public CD3DApplication {
private:
	/// How many cubemaps for SH evaluation we need. We don't need them for first 9 coeffs.
	enum { SHCUBEMAPS = (CMesh::SH_COEFFS-9+3)/4 };
	/// SH evaluation cubemap size
	enum { SHCUBEMAP_SIZE = 64 };

	/// Shadow cubemap size
	enum { SHADOWCUBEMAP_SIZE = 512 };

	/// Normal light count (additional lights are projectiles).
	enum { LIGHTS = 3 };

	/// SH mode: 25, 9, 4
	enum { SH_25 = 0, SH_9, SH_4, SHMODECOUNT };
	
private:
	ID3DXFont*	mFont;
	bool		mShowHelp;
	bool		mShowWireframe;
	
	bool		mUseLocalSH; // true = localsh, false = normal shadow cubemap
	int			mSHMode;

	CFirstPersonCamera	mCamera;
	SMatrix4x4	mCameraMatrix;
	
	CMesh*			mMesh;
	ID3DXMesh*		mMeshBulb;
	ID3DXEffect*	mEffect;
	ID3DXEffect*	mEffectBulb;
	CVisibilityCalculator	mVisibilityCalc;
	
	ID3DXEffect*	mEffectShadowDepth;
	ID3DXEffect*	mEffectShadowMap;

	int 	mActiveLight;
	
	IDirect3DCubeTexture9*	mTexSHCubes[SHCUBEMAPS];

	IDirect3DCubeTexture9*	mTexShadowCube;
	IDirect3DSurface9*		mSurfShadowZ;
	
	SVector3	mLightPos[LIGHTS];
	SVector3	mLightCenterPos[LIGHTS];
	SVector4	mLightCol[LIGHTS];
	bool		mLightAnim[LIGHTS];

	typedef fastvector<SProjectile>	TProjectileVector;
	TProjectileVector	mProjectiles;
	
	ID3DXMesh*	mMeshSphere;	// tesselated sphere
	ID3DXMesh*	mMeshSHFunc;	// sh function at some vertex
	int 		mSHFuncVertex;
	SVector3	mSHFuncPos;
	bool		mShowSHFunc;
	ID3DXEffect*	mEffectFunc;
	
protected:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	void RenderText();
	
private:
	HRESULT loadEffect( const char* name, ID3DXEffect** fx );
	void	showVisibilityAtCursor();
	void	setupSHMeshForVertex( int i );
	void	renderShadowmap( const SVector3& pos, const SVector4& col );
	int		getLightCount() const { return LIGHTS + mProjectiles.size(); }
	void	getLightParams( int l, SVector3& pos, SVector4& col ) const;
public:
	CMyD3DApplication();
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	CMyD3DApplication d3dApp;
	if( FAILED( d3dApp.Create( hInst ) ) ) {
		return 0;
	}
	
	return d3dApp.Run();
}


CMyD3DApplication::CMyD3DApplication()
{
	m_strWindowTitle = _T("Local SH");
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	m_dwCreationWidth  = 640;
	m_dwCreationHeight = 480;
	mFont = NULL;

	mEffect = NULL; mEffectBulb = NULL;
	mEffectShadowDepth = NULL; mEffectShadowMap = NULL;
	mEffectFunc = NULL;

	mMesh = NULL; mMeshBulb = NULL; mMeshSphere = NULL; mMeshSHFunc = NULL;
	mShowHelp = false;
	mShowWireframe = false;
	mUseLocalSH = true;
	mSHMode = SH_25;
	for( int cm = 0; cm < SHCUBEMAPS; ++cm ) {
		mTexSHCubes[cm] = 0;
	}
	mTexShadowCube = 0;
	mSurfShadowZ = 0;
	mActiveLight = 0;
}



HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	//mLightPos[0].set( -6,3.5f, 2);
	//mLightPos[1].set(-1,1.5f,-2);
	//mLightPos[2].set( -7,3.5f,-2);
	mLightPos[0].set( 0,3.5f, 4);
	mLightPos[1].set(-1,1.5f,-2);
	mLightPos[2].set( 7,2.5f,-4);
	for( int i = 0; i < LIGHTS; ++i ) {
		mLightCenterPos[i] = mLightPos[i];
		mLightAnim[i] = false;
	}
	mLightCol[0].set( 1.2f, 0.7f, 0.7f, 1.0f );
	mLightCol[1].set( 0.7f, 1.2f, 0.7f, 1.0f );
	mLightCol[2].set( 0.7f, 0.7f, 1.2f, 1.0f );

	D3DXVECTOR3 vecEye( -7.0f, 4.3f, 4.5f );
	D3DXVECTOR3 vecAt ( -0.5f, 3.0f, 0.0f );
	mCamera.SetViewParams( &vecEye, &vecAt );
	mCamera.SetScalers( 0.01f, 2.0f );
	
	return S_OK;
}

static void WINAPI gSHCubeFiller( D3DXVECTOR4 *out, const D3DXVECTOR3 *uvw, const D3DXVECTOR3 *texelSize, void* data )
{
	float coeffs[CMesh::SH_COEFFS+4];
	int offset = *(int*)data;
	D3DXVECTOR3 dir;
	D3DXVec3Normalize( &dir, uvw );
	D3DXSHEvalDirection( coeffs, CMesh::SH_ORDER, &dir );
	const float window = 0.75f;
	*out = D3DXVECTOR4(&coeffs[offset]) * window;
}

HRESULT CMyD3DApplication::loadEffect( const char* name, ID3DXEffect** fx )
{
	HRESULT hr = D3DXCreateEffectFromFile( m_pd3dDevice, name, NULL, NULL, 0, NULL, fx, NULL );
	if( FAILED( hr ) )
		return hr;
	D3DXHANDLE fxTech;
	hr = (*fx)->FindNextValidTechnique( 0, &fxTech );
	if( FAILED(hr) )
		return hr;
	(*fx)->SetTechnique( fxTech );
	return S_OK;
}

HRESULT CMyD3DApplication::InitDeviceObjects()
{
	HRESULT hr;
	
	// SH cubemaps
	int shOffset = 9; // first 9 analytically
	for( int cm = 0; cm < SHCUBEMAPS; ++cm ) {
		//D3DXCreateCubeTexture( m_pd3dDevice, SHCUBEMAP_SIZE, 0, 0, D3DFMT_Q16W16V16U16, D3DPOOL_MANAGED, &mTexSHCubes[cm] );
		D3DXCreateCubeTexture( m_pd3dDevice, SHCUBEMAP_SIZE, 0, 0, D3DFMT_Q8W8V8U8, D3DPOOL_MANAGED, &mTexSHCubes[cm] );
		//D3DXCreateCubeTexture( m_pd3dDevice, SHCUBEMAP_SIZE, 0, 0, D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED, &mTexSHCubes[cm] );
		D3DXFillCubeTexture( mTexSHCubes[cm], gSHCubeFiller, &shOffset );
		shOffset += 4; // coeffs in one cubemap
	}

	// Initialize the font
	HDC hDC = GetDC( NULL );
	int nHeight = -MulDiv( 9, GetDeviceCaps(hDC, LOGPIXELSY), 72 );
	ReleaseDC( NULL, hDC );
	if( FAILED( hr = D3DXCreateFont( m_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("Arial"), &mFont ) ) )
		return hr;
	
	// Meshes
	mMesh = new CMesh();
	if( FAILED( hr = mMesh->create( m_pd3dDevice, "data/IndoorsPlain.x", "data/Indoors50k.x" ) ) )
		return hr;
	if( FAILED( hr = D3DXCreateSphere( m_pd3dDevice, 0.2f, 8, 6, &mMeshBulb, NULL ) ) )
		return hr;
	if( FAILED( hr = D3DXLoadMeshFromX( "data/SphereFull.x", D3DXMESH_SYSTEMMEM, m_pd3dDevice, NULL, NULL, NULL, NULL, &mMeshSphere ) ) )
		return hr;
	if( FAILED( hr = mMeshSphere->CloneMeshFVF( D3DXMESH_MANAGED, mMeshSphere->GetFVF(), m_pd3dDevice, &mMeshSHFunc ) ) )
		return hr;
	
	// Calc occlusion
	mVisibilityCalc.calculate( *mMesh );

	//
	mSHFuncVertex = 100;
	setupSHMeshForVertex( mSHFuncVertex );
	
	// Effects
	hr = loadEffect( "data/localsh.fx", &mEffect );
	if( FAILED( hr ) )
		return hr;
	hr = loadEffect( "data/bulb.fx", &mEffectBulb );
	if( FAILED( hr ) )
		return hr;
	hr = loadEffect( "data/shadowdepth.fx", &mEffectShadowDepth );
	if( FAILED( hr ) )
		return hr;
	hr = loadEffect( "data/shadowmap.fx", &mEffectShadowMap );
	if( FAILED( hr ) )
		return hr;
	hr = loadEffect( "data/func.fx", &mEffectFunc );
	if( FAILED( hr ) )
		return hr;
	
	// Setup the camera
	float fAspectRatio = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
	mCamera.SetProjParams( D3DX_PI/3, fAspectRatio, 0.3f, 50.0f );
	
	return S_OK;
}


HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	if( mFont )
		mFont->OnResetDevice();
	
	mCamera.SetResetCursorAfterMove( !m_bWindowed );
	
	if( mEffect ) mEffect->OnResetDevice();
	if( mEffectBulb ) mEffectBulb->OnResetDevice();
	if( mEffectShadowDepth ) mEffectShadowDepth->OnResetDevice();
	if( mEffectShadowMap ) mEffectShadowMap->OnResetDevice();
	if( mEffectFunc ) mEffectFunc->OnResetDevice();

	HRESULT hr = D3DXCreateCubeTexture( m_pd3dDevice, SHADOWCUBEMAP_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mTexShadowCube );
	hr = m_pd3dDevice->CreateDepthStencilSurface( SHADOWCUBEMAP_SIZE, SHADOWCUBEMAP_SIZE, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &mSurfShadowZ, NULL );
	
	return S_OK;
}


HRESULT CMyD3DApplication::FrameMove()
{
	// Update the camera's postion based on user input 
	mCamera.FrameMove( m_fElapsedTime );

	const D3DXMATRIX* camview = mCamera.GetViewMatrix();
	D3DXMatrixInverse( &mCameraMatrix, NULL, camview );
	
	if( GetAsyncKeyState('I') ) mLightCenterPos[mActiveLight].x += m_fElapsedTime * 2.0f;
	if( GetAsyncKeyState('K') ) mLightCenterPos[mActiveLight].x -= m_fElapsedTime * 2.0f;
	if( GetAsyncKeyState('J') ) mLightCenterPos[mActiveLight].z -= m_fElapsedTime * 2.0f;
	if( GetAsyncKeyState('L') ) mLightCenterPos[mActiveLight].z += m_fElapsedTime * 2.0f;

	for( int i = 0; i < LIGHTS; ++i ) {
		if( mLightAnim[i] ) {
			float t1 = fmodf( m_fTime * 1.2f+i, 2.0f * D3DX_PI );
			float t2 = fmodf( m_fTime * 0.7f+i, 2.0f * D3DX_PI );
			float t3 = fmodf( m_fTime * 1.7f+i, 2.0f * D3DX_PI );
			mLightPos[i].x = mLightCenterPos[i].x + cosf( t1 ) * 1.0f;
			mLightPos[i].y = mLightCenterPos[i].y + cosf( t2 ) * 0.3f;
			mLightPos[i].z = mLightCenterPos[i].z + cosf( t3 ) * 1.0f;
		} else
			mLightPos[i] = mLightCenterPos[i];
	}

	TProjectileVector::iterator it;
	for( it = mProjectiles.begin(); it != mProjectiles.end(); /* */ ) {
		bool alive = it->update( m_fElapsedTime );
		if( !alive )
			it = mProjectiles.erase(it);
		else
			++it;
	}

	return S_OK;
}


void CMyD3DApplication::renderShadowmap( const SVector3& pos, const SVector4& col )
{
	D3DXMATRIXA16 mProj;
	D3DXMatrixPerspectiveFovLH( &mProj, D3DX_PI * 0.5f, 1.0f, 0.05f, 50.0f );

	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mViewProjection;

	HRESULT hr;

	UINT passes;
	hr = mEffectShadowDepth->Begin( &passes, 0 );
	fxBeginPass( mEffectShadowDepth, 0 );

	IDirect3DSurface9 *mainRT, *mainZ;
	m_pd3dDevice->GetRenderTarget( 0, &mainRT );
	m_pd3dDevice->GetDepthStencilSurface( &mainZ );

	m_pd3dDevice->SetDepthStencilSurface( mSurfShadowZ );
	for( UINT i = 0; i < 6; ++i ) {
		IDirect3DSurface9* s;
		mTexShadowCube->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &s );
		m_pd3dDevice->SetRenderTarget( 0, s );

		m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFffffff, 1.0f, 0L );
		m_pd3dDevice->BeginScene();

		mView = D3DUtil_GetCubeMapViewMatrix( i, pos );
		mViewProjection = mView * mProj;

		mEffectShadowDepth->SetMatrix( "mViewProj", &mViewProjection );
		mEffectShadowDepth->SetValue( "vLightPos", &pos, sizeof(pos) );

		fxCommit( mEffectShadowDepth );
		mMesh->getMeshPlain().DrawSubset( 0 );
		m_pd3dDevice->EndScene();
		s->Release();
	}
	m_pd3dDevice->SetRenderTarget( 0, mainRT );
	m_pd3dDevice->SetDepthStencilSurface( mainZ );
	mainRT->Release();
	mainZ->Release();

	fxEndPass( mEffectShadowDepth );
	hr = mEffectShadowDepth->End();
}

void CMyD3DApplication::getLightParams( int light, SVector3& pos, SVector4& col ) const
{
	if( light < LIGHTS ) {
		pos = mLightPos[light];
		col = mLightCol[light];
	} else {
		light -= LIGHTS;
		pos = mProjectiles[light].position;
		col = mProjectiles[light].color;
	}
}

HRESULT CMyD3DApplication::Render()
{
	D3DXMATRIXA16 mProj;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mViewProjection;
	
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) ) {

		//
		// common params on effects

		mViewProjection = (*mCamera.GetViewMatrix()) * (*mCamera.GetProjMatrix());
		mEffect->SetMatrix( "mViewProj", &mViewProjection );
		mEffectBulb->SetMatrix( "mViewProj", &mViewProjection );
		mEffectShadowMap->SetMatrix( "mViewProj", &mViewProjection );
		mEffectFunc->SetMatrix( "mViewProj", &mViewProjection );
		for( int cm = 0; cm < SHCUBEMAPS; ++cm ) {
			char buf[10];
			buf[0] = 't'; buf[1] = 'S'; buf[2] = 'H'; buf[3] = '0'+cm; buf[4] = 0;
			mEffect->SetTexture( buf, mTexSHCubes[cm] );
		}
		
		UINT passes;

		ID3DXEffect* mainfx = mUseLocalSH ? mEffect : mEffectShadowMap;
		
		//
		// main geometry

		int nlights = getLightCount();
		SVector3 lightPos;
		SVector4 lightCol;
		
		mainfx->Begin( &passes, 0 );
		fxBeginPass( mainfx, 0 ); // z fill pass
		mMesh->getMeshSH().DrawSubset( 0 );
		fxEndPass( mainfx );

		if( mUseLocalSH ) {
			fxBeginPass( mainfx, mSHMode + 1 ); // ligth pass
			for( int l = 0; l < nlights; ++l ) {
				getLightParams( l, lightPos, lightCol );
				mainfx->SetValue( "vLightPos", &lightPos, sizeof(lightPos) );
				mainfx->SetValue( "vLightColor", &lightCol, sizeof(lightCol) );
				fxCommit( mainfx );
				mMesh->getMeshSH().DrawSubset( 0 );
			}
		} else {
			fxBeginPass( mainfx, 1 ); // ligth pass
			for( int l = 0; l < nlights; ++l ) {
				getLightParams( l, lightPos, lightCol );
				mainfx->SetTexture( "tShadow", 0 );
				fxCommit( mainfx );
				fxEndPass( mainfx );
				mainfx->End();
				m_pd3dDevice->EndScene();

				renderShadowmap( lightPos, lightCol );

				m_pd3dDevice->BeginScene();
				mainfx->Begin( &passes, 0 );
				fxBeginPass( mainfx, 1 );
				mainfx->SetTexture( "tShadow", mTexShadowCube );
				mainfx->SetValue( "vLightPos", &lightPos, sizeof(lightPos) );
				mainfx->SetValue( "vLightColor", &lightCol, sizeof(lightCol) );
				fxCommit( mainfx );
				mMesh->getMeshSH().DrawSubset( 0 );
			}
		}
		fxEndPass( mainfx );
		if( mShowWireframe ) { // wireframe pass
			fxBeginPass( mainfx, (mUseLocalSH) ? 4 : 2 );
			mMesh->getMeshSH().DrawSubset( 0 );
			fxEndPass( mainfx );
		}
		mainfx->End();
		
		//
		// light bulbs
		
		mEffectBulb->Begin( &passes, 0 );
		fxBeginPass( mEffectBulb, 0 );
		for( int l = 0; l < nlights; ++l ) {
			getLightParams( l, lightPos, lightCol );
			mEffectBulb->SetValue( "vLightPos", &lightPos, sizeof(lightPos) );
			SVector4 c = lightCol;
			c *= 1.2f;
			mEffectBulb->SetValue( "vLightColor", &c, sizeof(c) );
			fxCommit( mEffectBulb );
			mMeshBulb->DrawSubset( 0 );
		}
		// sh vertex
		/*
		if( mUseLocalSH ) {
			mEffectBulb->SetValue( "vLightPos", &mSHFuncPos, sizeof(mSHFuncPos) );
			SVector4 c;
			c.set(1,0,0,1);
			mEffectBulb->SetValue( "vLightColor", &c, sizeof(c) );
			mMeshBulb->DrawSubset( 0 );
		}
		*/
		fxEndPass( mEffectBulb );
		mEffectBulb->End();

		//
		// approximated functions
		
		/*
		if( mUseLocalSH ) {
			mEffectFunc->Begin( &passes, 0 );
			mEffectFunc->Pass(0);
			mMeshSHFunc->DrawSubset( 0 );
			mEffectFunc->End();
		}
		*/
		
		RenderText();
		m_pd3dDevice->EndScene();
	}
	
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Draw the help & statistics for running sample
//-----------------------------------------------------------------------------
void CMyD3DApplication::RenderText()
{
	RECT rc;
	
	if( NULL == mFont )
		return;

	char buf[100];
	
	// Output statistics
	int iLineY = 0;
	SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
	mFont->DrawText( NULL, m_strFrameStats, -1, &rc, DT_NOCLIP, 0xC0FFFFFF );
	SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
	mFont->DrawText( NULL, m_strDeviceStats, -1, &rc, DT_NOCLIP, 0xC0FFFFFF );
	
	SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 45;
	const char* shModes[] = {
		"LocalSH 25 coeffs",
		"LocalSH 9 coeffs",
		"LocalSH 4 coeffs"
	};
	sprintf( buf, "Shadows: %s\nActive light: %i",
		mUseLocalSH ? (shModes[mSHMode]) : "ShadowMap",
		mActiveLight );
	mFont->DrawText( NULL, buf, -1, &rc, DT_NOCLIP, 0xFFFFFF00 );
	
	// Draw help
	if( mShowHelp ) {
		iLineY = m_d3dsdBackBuffer.Height-15*6;
		SetRect( &rc, 20, iLineY, 0, 0 );
		mFont->DrawText( NULL,
			"Rotate: LMouse; Move: A/W/S/D or arrows; Up/down: Q/E or PgUp/PgDn\n"
			"Fire: Space\n"
			"Toggle wireframe: F3\n"
			"Toggle LocalSH vs ShadowMap: F4\n"
			"Toggle 25 vs 9 coeff SH: F5\n"
			"Change active light: P; Move light: J/I/K/L; Animate light: M\n",
			-1, &rc, DT_NOCLIP, 0xFFffff00 );
		
	} else {
		SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
		mFont->DrawText( NULL, _T("Press F1 for help"), 
			-1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	}
}

HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	if( mFont )
		mFont->OnLostDevice();
	if( mEffect ) mEffect->OnLostDevice();
	if( mEffectBulb ) mEffectBulb->OnLostDevice();
	if( mEffectShadowDepth ) mEffectShadowDepth->OnLostDevice();
	if( mEffectShadowMap ) mEffectShadowMap->OnLostDevice();
	if( mEffectFunc ) mEffectFunc->OnLostDevice();

	DXSAFE_RELEASE( mSurfShadowZ );
	DXSAFE_RELEASE( mTexShadowCube );

	return S_OK;
}


HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	DXSAFE_RELEASE( mFont );
	DXSAFE_DELETE( mMesh );
	DXSAFE_RELEASE( mMeshBulb );
	DXSAFE_RELEASE( mMeshSphere );
	DXSAFE_RELEASE( mMeshSHFunc );
	DXSAFE_RELEASE( mEffect );
	DXSAFE_RELEASE( mEffectBulb );
	DXSAFE_RELEASE( mEffectShadowDepth );
	DXSAFE_RELEASE( mEffectShadowMap );
	DXSAFE_RELEASE( mEffectFunc );
	for( int cm = 0; cm < SHCUBEMAPS; ++cm ) {
		DXSAFE_RELEASE( mTexSHCubes[cm] );
	}
	return S_OK;
}

HRESULT CMyD3DApplication::FinalCleanup()
{
	return S_OK;
}

HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	// No fallback, so need ps2.0
	if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
	return E_FAIL;
	
	// If device doesn't support 1.1 vertex shaders in HW, switch to SWVP.
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) ) {
		if( (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) == 0 )
			return E_FAIL;
	}
	
	return S_OK;
}

LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	mCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
	
	switch( uMsg ) {
	case WM_COMMAND: {
		switch( LOWORD(wParam) ) {
		case IDM_TOGGLEHELP:
			mShowHelp = !mShowHelp;
			break;
		case IDM_TOGGLEWIRE:
			mShowWireframe = !mShowWireframe;
			break;
		case IDM_OPTIONS_NEXTLIGHT:
			mActiveLight++;
			if( mActiveLight >= LIGHTS )
				mActiveLight = 0;
			break;
		case IDM_OPTIONS_SHNEXT:
			mSHFuncVertex += 10;
			if( mSHFuncVertex >= mMesh->getMeshSH().GetNumVertices() )
				mSHFuncVertex = 0;
			setupSHMeshForVertex( mSHFuncVertex );
			break;
		case IDM_OPTIONS_LIGHTANIM:
			mLightAnim[mActiveLight] = !mLightAnim[mActiveLight];
			break;
		case IDM_OPTIONS_LSHSHADOWMAP:
			mUseLocalSH = !mUseLocalSH;
			break;
		case IDM_OPTIONS_259:
			mSHMode++;
			if( mSHMode >= SHMODECOUNT )
				mSHMode = 0;
			break;
		case IDM_OPTIONS_FIRE:
			if( mProjectiles.size() < 5 ) {
				SProjectile proj( mCameraMatrix );
				mProjectiles.push_back( proj );
			}
			break;
		}
	}
	}
	return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CMyD3DApplication::showVisibilityAtCursor()
{
	const D3DXMATRIX& mproj = *mCamera.GetProjMatrix();
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( m_hWnd, &ptCursor );
	
	// Compute the vector of the pick ray in screen space
	SVector3 v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / m_d3dsdBackBuffer.Width	) - 1 ) / mproj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / m_d3dsdBackBuffer.Height ) - 1 ) / mproj._22;
	v.z =  1.0f;
	
	// Get the inverse of the view matrix
	SMatrix4x4 m = *mCamera.GetViewMatrix();
	m.invert();
	
	// Transform the screen space pick ray into 3D space
	SVector3 pickDir;
	SVector3 pickOrigin = m.getOrigin();
	m.getOrigin().set(0,0,0);
	D3DXVec3TransformCoord( &pickDir, &pickDir, &m );

	// intersect with scene
	BOOL hit;
	DWORD hitFace;
	float hitU, hitV, hitDist;
	D3DXIntersect( &mMesh->getMeshSH(), &pickOrigin, &pickDir, &hit, &hitFace, &hitU, &hitV, &hitDist, NULL, NULL );
}

void CMyD3DApplication::setupSHMeshForVertex( int idx )
{
	CMesh::SSHVertex* vbsh = 0;
	CMesh::SVertex* vbsphere = 0;
	CMesh::SVertex* vbdest = 0;
	int n = mMeshSphere->GetNumVertices();
	mMesh->getMeshSH().LockVertexBuffer( D3DLOCK_READONLY, (void**)&vbsh );
	mMeshSphere->LockVertexBuffer( D3DLOCK_READONLY, (void**)&vbsphere );
	mMeshSHFunc->LockVertexBuffer( 0, (void**)&vbdest );
	vbsh += idx;
	mSHFuncPos = vbsh->p;
	float shc[CMesh::SH_COEFFS];
	for( int i = 0; i < n; ++i ) {
		SVector3 dir = vbsphere->p;
		dir.normalize();
		D3DXSHEvalDirection( shc, CMesh::SH_ORDER, &dir );
		float dt = D3DXSHDot( CMesh::SH_ORDER, shc, vbsh->sh );
		vbdest->p = mSHFuncPos + dir * dt;
		++vbsphere;
		++vbdest;
	}
	mMeshSHFunc->UnlockVertexBuffer();
	mMeshSphere->UnlockVertexBuffer();
	mMesh->getMeshSH().UnlockVertexBuffer();
	D3DXComputeNormals( mMeshSHFunc, NULL );
}

