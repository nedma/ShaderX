//======================================================================================//
// filename: ShaderX3_StatueDemo.cpp                                                    //
//                                                                                      //
// author:   Jason L. Mitchell                                                          //
//           ATI Research, Inc.                                                         //
//           3D Application Research Group                                              //
//           email: JasonM@ati.com                                                      //
//                                                                                      //
// Description: Sample application which illustrates a variety of advanced visual       //
//              effects including:                                                      //
//                                                                                      //
//              o Volumetric light shafts with projective noise                         //
//              o Ambient occlusion                                                     //
//              o Use of a cube map for directional ambient term                        //
//              o Depth-based shadow mapping & novel filtering for variable penumbra    //
//              o Procedural marble texture                                             //
//                                                                                      //
// Sample application for the following chapters in ShaderX3:                           //
//                                                                                      //
//              o Poisson Shadow Blur                                                   //
//              o Light Shaft Rendering                                                 //
//                                                                                      //
//======================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                  //
//======================================================================================//


#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdio.h>
#include <d3dx9.h>

#include <model_t.h>
#include <d3dmodel_t.h>
#include "DXUtil.h"
#include "D3DUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "resource.h"
#include "CSpotlight.h"
#include "CBackdrop.h"
#include "COverlayQuad.h"
#include "CVolVizShells.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define WINDOW_CREATION_WIDTH                1024
#define WINDOW_CREATION_HEIGHT                768

#define SHADOW_MAP_SIZE                       512

// Object animation states
#define OBJECT_STATE_FROZEN                     0
#define OBJECT_STATE_INVISIBLE                  1
#define NUM_OBJECT_STATES                       2

// Shadow map overlay modes
#define SHOW_SHADOW_MAP_MODE_NONE               0
#define SHOW_SHADOW_MAP_MODE_DEPTH              1
#define SHOW_SHADOW_MAP_MODE_BLACK_AND_WHITE    2
#define SHOW_SHADOW_MAP_MODE_FILTERED           3
#define NUM_SHOW_SHADOW_MAP_MODES               4

// Parameter codes
#define SPOTLIGHT_PARAM_WIDTH                   0
#define SPOTLIGHT_PARAM_HEIGHT                  1
#define NUM_SPOTLIGHT_PARAMETERS                2

// Step sizes for modification via the UI (naturally somewhat tuned to the size of this scene)
#define SPOTLIGHT_STEP_SIZE_WIDTH               0.01f

// Initial spotlight parameters
#define SPOTLIGHT_INITIAL_WIDTH                 0.25f
#define SPOTLIGHT_INITIAL_HEIGHT                0.25f
#define SPOTLIGHT_INITIAL_NEAR_PLANE            10.0f
#define SPOTLIGHT_INITIAL_FAR_PLANE            400.0f

// Arcball parameters
#define ARCBALL_RADIUS                          0.85f
#define ARCBALL_TRANSLATION_RADIUS              100.0f

#define NUM_COOKIES               6

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

char g_strCookieFiles[NUM_COOKIES][64] = { _T("textures\\RadialFade.tga"),
                                           _T("textures\\HarshCircle.tga"),
                                           _T("textures\\star.dds"),
                                           _T("textures\\TreeCanopy.tga"),
                                           _T("textures\\Holes.tga"),
                                           _T("textures\\MoreHoles.tga")};

// Some global matrices accessed by other modules
D3DXMATRIX          g_matProj;
D3DXMATRIX          g_matWorldViewProj;
D3DXMATRIX          g_matWorldView;
D3DXMATRIX          g_matITWorldView;


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication
// Desc: 
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
public:
    CMyD3DApplication();

    virtual HRESULT OneTimeSceneInit(void);
    virtual HRESULT InitDeviceObjects(void);
    virtual HRESULT RestoreDeviceObjects(void);
    virtual HRESULT InvalidateDeviceObjects(void);
    virtual HRESULT DeleteDeviceObjects(void);
    virtual HRESULT FrameMove(void);
    virtual HRESULT Render(void);
    virtual HRESULT FinalCleanup(void);

    virtual LRESULT MsgProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
   VOID    ColorPick(D3DCOLORVALUE *rgbColor);
   HRESULT InitSplineTextures(void);
   HRESULT InitAmbientOcclusionTexture(void);
   HRESULT InitNoiseTexture(void);
   HRESULT InitAmbientCube(void);

   // Scene Objects
   CSpotlight          m_Spotlight;
   CBackdrop           m_Backdrop;
   COverlayQuad        m_ShadowMapOverlayQuad;
   COverlayQuad        m_CurrentCookieOverlayQuad;
   COverlayQuad        m_FinalFogOverlayQuad;
   CVolVizShells       m_VolVizShells;
   model_t             m_3DSObject;

   D3DXVECTOR3         m_vSceneLightPos;           // Global position for lighting the light widget(s) only
   D3DXVECTOR3         m_vVmodel;                  // Viewer in model space
   D3DXVECTOR3         m_vLmodel;                  // Light in model space

   DWORD               m_dwCurrentParameter;       // For controlling parameter modification UI

   // Light parameters
   FLOAT               m_fLightFOV;
   D3DXVECTOR3         m_vLightPos;
   FLOAT               m_fWidth;
   FLOAT               m_fHeight;
   FLOAT               m_fNearPlane;
   FLOAT               m_fFarPlane;

   D3DXPLANE           m_WorldSpaceLightFrustumPlanes[6]; // Read these from each projective light
   D3DXPLANE           m_ClipSpaceLightFrustumPlanes[6];  // Transform into here and pass these in to vol viz shells

   // Scene state
   BOOL                m_bFreezeVolVizShells; // These are useful for debugging/visualization but are coded out right now
   BOOL                m_bUseAmbientOcclusion;
   BOOL                m_bUseAmbientCube;
   BOOL                m_bSpotlightVolume;
   BOOL                m_bShowClippingFrustum;
   BOOL                m_bShowLight;
   INT                 m_ShowShadowMapMode;
   BOOL                m_bShowScrollingNoise;
   BOOL                m_bShowLightShafts;
   BOOL                m_bAnimateLights;
   BOOL                m_bShowCurrentCookie;
   BOOL                m_bShadowMapDirty;
   BOOL                m_bShadowMapping;
   BOOL                m_bUsingCookie;
   BOOL                m_bDisplayHelp;
   BOOL                m_bFirstUnpausedFrame;
   BOOL                m_bFirstPausedFrame;

   FLOAT               m_fFrozenLightTime;
   FLOAT               m_fUnfrozenLightTime;
   FLOAT               m_fFrozenObjectTime;
   FLOAT               m_fLightTime;

   INT                 m_CurrentCookie;
   INT                 m_ObjectState;

   FLOAT               m_fSamplingDelta;     // Spacing of sampling planes

   // Effect for 3DS object
   LPD3DXEFFECT        m_pEffect;
   BOOL                m_bEffectSucceeded;

   // UI text overlay
   CD3DFont*           m_pFont;
   CD3DFont*           m_pFontSmall;

   // UI controls
   CD3DArcBall         m_ArcBall;                  // ArcBall used for mouse input

   // Transforms
   D3DXMATRIX          m_matView;
   D3DXMATRIX          m_matWorld;
   D3DXMATRIX          m_matArcBallRot;
   D3DXMATRIX          m_matArcBallTrans;

   D3DXMATRIX          m_mat3DSObjectWorld;
   D3DXMATRIX          m_mat3DSObjectWorldView;
   D3DXMATRIX          m_mat3DSObjectITWorldView;
   D3DXMATRIX          m_mat3DSObjectWorldViewProj;

   D3DXMATRIX          m_matITWorldViewProjection; // For transforming clip planes

   D3DXMATRIX          m_matLight3DSObjectWorldView;
   D3DXMATRIX          m_matLight3DSObjectITWorldView;
   D3DXMATRIX          m_matLight3DSObjectWorldViewProj;
   D3DXMATRIX          m_matLight3DSObjectWorldViewProjBias;
   D3DXMATRIX          m_matLight3DSObjectWorldViewProjScroll1;
   D3DXMATRIX          m_matLight3DSObjectWorldViewProjScroll2;

   D3DXMATRIX          m_matViewWorldLight;
   D3DXMATRIX          m_matWorldLight;                 // World to light transform
   D3DXMATRIX          m_matITWorldLight;               // Inverse Transpose of World to Light transform


   D3DXMATRIX          m_matWorldLightProj;
   D3DXMATRIX          m_matWorldLightProjBias;         // For projective cookie texture on backdrop
   D3DXMATRIX          m_matWorldLightProjScroll1;      // For projective scrolling noise texture on backdrop
   D3DXMATRIX          m_matWorldLightProjScroll2;      // For projective scrolling noise texture on backdrop

   D3DXMATRIX          m_matViewWorldLightProj;
   D3DXMATRIX          m_matViewWorldLightProjBias;     // For projective cookie texture on volviz shells
   D3DXMATRIX          m_matViewWorldLightProjScroll1;  // For projective scrolling noise texture on volviz shells
   D3DXMATRIX          m_matViewWorldLightProjScroll2;  // For projective scrolling noise texture on volviz shells

   D3DXMATRIX          m_matLightProj;
   D3DXMATRIX          m_matLightProjBias;              // For projective cookie texture on backdrop
   D3DXMATRIX          m_matLightProjScroll1;           // For projective scrolling noise texture on backdrop
   D3DXMATRIX          m_matLightProjScroll2;           // For projective scrolling noise texture on backdrop


   // Viewports   
   D3DVIEWPORT9        m_WholeWindowViewport;
   D3DVIEWPORT9        m_ShadowMapViewport;

   // Shadow map textures and related surfaces
   LPDIRECT3DTEXTURE9  m_pShadowMap;                           // D3DFMT_L16 texture which actually gets allocated
   LPDIRECT3DTEXTURE9  m_pBlackAndWhiteShadowMap;              // D3DFMT_L8
   LPDIRECT3DTEXTURE9  m_pFilteredBlackAndWhiteShadowMap;      // D3DFMT_L8
   
	LPDIRECT3DSURFACE9  m_pShadowMapZ;                          // Depth-Stencil surface for shadow map
	LPDIRECT3DSURFACE9  m_pShadowMapSurf;                       // Pointer to level 0 of m_pShadowMap for SRT()

   LPDIRECT3DSURFACE9  m_pBlackAndWhiteShadowMapSurf;          // Pointer to level 0 of m_pBlackAndWhiteShadowMap for SRT()
	LPDIRECT3DSURFACE9  m_pFilteredBlackAndWhiteShadowMapSurf;  // Pointer to level 0 of m_pFilteredBlackAndWhiteShadowMap for SRT()

   // Fog buffer for accumulating and filtering fog
   LPDIRECT3DTEXTURE9  m_pFogBuffer;                           // RGBA - 1/4 of the planes per channel
   LPDIRECT3DTEXTURE9  m_pFilteredFogBuffer;                   // D3DFMT_L8 - temp for resolving luminance from RGBA fog buffer
	LPDIRECT3DSURFACE9  m_pFogBufferSurf;                       // Pointer to level 0 of m_pFogBuffer for SRT()
      
   // Other textures
   LPDIRECT3DTEXTURE9  m_pCookies[NUM_COOKIES];                // RGBA cookie textures
   LPDIRECT3DTEXTURE9  m_pScrollingNoise;                      // Tilable RGBA noise texture
                                                               
   // Textures for shaded object                               
   LPDIRECT3DTEXTURE9  m_pMarbleColorSplineTexture;            // 1D marble spline texture
   LPDIRECT3DTEXTURE9  m_pAmbientOcclusion;                    // 2D texture generated offline which encodes occlusion due to local geometry
   LPDIRECT3DVOLUMETEXTURE9 m_pVolumeNoise;                    // Tilable grayscale 3D noise texture
   LPDIRECT3DCUBETEXTURE9 m_pAmbientCube;                      // Diffuse cube map for low-frequency color bleed from environment

};


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    InitCommonControls();
    if (FAILED (d3dApp.Create (hInst)))
        return 0;

    return d3dApp.Run();
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication
// Desc:
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication(void)
{
   m_strWindowTitle    = _T("ShaderX3 - Statue Demo 2004 - (c) ATI Research, Inc.");

   m_dwCreationWidth   = WINDOW_CREATION_WIDTH;
   m_dwCreationHeight  = WINDOW_CREATION_HEIGHT;

   m_bIgnoreSizeChange = TRUE;

   m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
   m_d3dEnumeration.AppMinDepthBits    = 24;

   m_pFont         = new CD3DFont (_T("Comic Sans MS"), 12, 0);
   m_pFontSmall    = new CD3DFont (_T("Comic Sans MS"), 10, 0);

   m_bEffectSucceeded   = TRUE;

   // 3DS Object and its Effect
   Load3ds("models\\Hebe.3ds", &m_3DSObject);
   m_pEffect = NULL;

   // Default scene state
   m_bFreezeVolVizShells   = FALSE; 
   m_bUseAmbientOcclusion  = TRUE;
   m_bUseAmbientCube       = TRUE;
   m_bShowClippingFrustum  = FALSE;
   m_bSpotlightVolume      = FALSE;
   m_bShowLight            = TRUE;
   m_ShowShadowMapMode     = FALSE;
   m_bAnimateLights        = TRUE;
   m_bShowScrollingNoise   = TRUE;
   m_bShowLightShafts      = TRUE;
   m_bShadowMapping        = TRUE;
   m_bShowCurrentCookie    = FALSE;
   m_bUsingCookie          = FALSE;
   m_bShadowMapDirty       = FALSE;
   m_bDisplayHelp          = TRUE;
   m_bFirstUnpausedFrame   = FALSE;
   m_bFirstPausedFrame     = FALSE;

   m_ObjectState = OBJECT_STATE_FROZEN;

   if (m_bAnimateLights == FALSE)
   {
      m_fFrozenLightTime = 0.0f;
   }
   else
   {
      m_fFrozenLightTime = m_fTime;
   }

   m_fUnfrozenLightTime = 0.0f;
   m_fFrozenObjectTime = 0.0f;

   //
   // Null out textures and surfaces
   //
   m_pShadowMap                           = NULL;
	m_pShadowMapZ                          = NULL;   
	m_pShadowMapSurf                       = NULL;

   m_pBlackAndWhiteShadowMapSurf          = NULL;
   m_pFilteredBlackAndWhiteShadowMapSurf  = NULL;

   m_pFogBuffer                           = NULL;
   m_pFilteredFogBuffer                   = NULL;
   m_pFogBufferSurf                       = NULL;

   m_pBlackAndWhiteShadowMap              = NULL;
   m_pFilteredBlackAndWhiteShadowMap      = NULL;
   m_pVolumeNoise                         = NULL;
   m_pAmbientCube                         = NULL;
   m_pMarbleColorSplineTexture            = NULL;
   m_pAmbientOcclusion                    = NULL;
   m_pScrollingNoise                      = NULL;

   for (int i=0; i < NUM_COOKIES; i++)
   {
      m_pCookies[i] = NULL;
   }

   m_CurrentCookie      = 0;

   m_vSceneLightPos     = D3DXVECTOR3(500.0f, 500.0f, -300.0f);

   m_fLightFOV          = D3DX_PI/4.0f;
   m_fWidth             = SPOTLIGHT_INITIAL_WIDTH;
   m_fHeight            = SPOTLIGHT_INITIAL_HEIGHT;
   m_fNearPlane         = SPOTLIGHT_INITIAL_NEAR_PLANE;
   m_fFarPlane          = SPOTLIGHT_INITIAL_FAR_PLANE;

   m_dwCurrentParameter = SPOTLIGHT_PARAM_WIDTH;

   // Sampling planes spaced this distance apart
   m_fSamplingDelta = 0.01f;
}



//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit(void)
{
   HRESULT hr;

   // Set light position used to light the little frustum geometry
   m_Spotlight.SetSceneLightPos(m_vSceneLightPos);

   if (FAILED (hr = m_Spotlight.Initialize()))
   {
      return hr;
   }

   // Initialize the objects
   m_Backdrop.Initialize();
   m_ShadowMapOverlayQuad.Initialize(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0.0f, 0.0f);
   m_CurrentCookieOverlayQuad.Initialize(128.0f, 128.0f, 0.0f, 1024.0f - 128.0f);
   
   m_VolVizShells.Initialize();

   //
   // Set up viewports
   //

   // Whole window (used for clearing)
   m_WholeWindowViewport.X      = 0;
   m_WholeWindowViewport.Y      = 0;
   m_WholeWindowViewport.Width  = m_dwCreationWidth;
   m_WholeWindowViewport.Height = m_dwCreationHeight;
   m_WholeWindowViewport.MinZ   = 0.0f;
   m_WholeWindowViewport.MaxZ   = 1.0f;

   // Shadow map (view from Light1)
   m_ShadowMapViewport.X      = 0;
   m_ShadowMapViewport.Y      = 0;
   m_ShadowMapViewport.Width  = SHADOW_MAP_SIZE;
   m_ShadowMapViewport.Height = SHADOW_MAP_SIZE;
   m_ShadowMapViewport.MinZ   = 0.0f;
   m_ShadowMapViewport.MaxZ   = 1.0f;

   return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitAmbientCube(void)
// Desc: Open ambient cube map and filter to create mip-maps
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitAmbientCube(void)
{
   SAFE_RELEASE (m_pAmbientCube);

   if (FAILED (D3DXCreateCubeTextureFromFileEx (m_pd3dDevice, _T("textures\\NewDiffuseCube_DXT1.dds"), D3DX_DEFAULT,
                                                0, 0, D3DFMT_DXT1, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
                                                0xFF000000, NULL, NULL, &m_pAmbientCube)))
   {
       return E_FAIL;
   }


   return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitNoiseTexture(void)
// Desc: Creates mip-mapped volume noise texture and loads noise from pre-authored
//       volumetric noise map which is authored to tile correctly.
//       The map is not assumed to contain mip-maps, so the app uses D3DX to fill them in 
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitNoiseTexture(void)
{
   SAFE_RELEASE (m_pVolumeNoise);

   if (FAILED (D3DXCreateVolumeTextureFromFileEx (m_pd3dDevice, _T("textures\\NoiseVolume.dds"), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
                                                  1, 0, D3DFMT_L8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
                                                  0xFF000000, NULL, NULL, &m_pVolumeNoise)))
   {
      return E_FAIL;
   }
  
   if (FAILED (D3DXFilterVolumeTexture (m_pVolumeNoise, NULL, 0, D3DX_DEFAULT)))
   {
      return E_FAIL;
   }

   return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitAmbientOcclusionTexture(void)
// Desc: Creates Ambient Occlusion texture
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitAmbientOcclusionTexture(void)
{
   SAFE_RELEASE (m_pAmbientOcclusion);

   if (FAILED (D3DXCreateTextureFromFileEx (m_pd3dDevice, _T("textures\\HebeWorldNmAmbiOccl1024.dds"), D3DX_DEFAULT, D3DX_DEFAULT,
                                            0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
                                            0xFF000000, NULL, NULL, &m_pAmbientOcclusion)))
   {
      return E_FAIL;
   }

   return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitSplineTextures(void)
// Desc: Creates 1D color spline texture
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitSplineTextures(void)
{
   HRESULT hr;

   SAFE_RELEASE (m_pMarbleColorSplineTexture);

   if (FAILED (hr = D3DXCreateTextureFromFileEx (m_pd3dDevice, _T("textures\\MarbleSpline.tga"), D3DX_DEFAULT, D3DX_DEFAULT,
                                                1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
                                                0xFF000000, NULL, NULL, &m_pMarbleColorSplineTexture)))
   {
       return hr;
   }

   return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects(void)
{
   HRESULT hr;

   // Initialize the font's internal textures
   m_pFont->InitDeviceObjects( m_pd3dDevice );
   m_pFontSmall->InitDeviceObjects( m_pd3dDevice );

   // OnCreateDevice
   if (FAILED (hr = m_Spotlight.OnCreateDevice(m_pd3dDevice)))
   {
      return hr;
   }

   // Init the teapot
   d3dInitializeModelNoMat(&m_3DSObject, m_pd3dDevice);

   // Initialize backdrop
   m_Backdrop.OnCreateDevice(m_pd3dDevice);

   // Initialize overlay quad used to display shadow map
   m_ShadowMapOverlayQuad.OnCreateDevice(m_pd3dDevice);

   // Initialize overlay quad used to display current cookie
   m_CurrentCookieOverlayQuad.OnCreateDevice(m_pd3dDevice);
  
   m_FinalFogOverlayQuad.Initialize((float)m_d3dsdBackBuffer.Height, (float)m_d3dsdBackBuffer.Width, 0.0f, 0.0f);
   m_FinalFogOverlayQuad.OnCreateDevice(m_pd3dDevice);

   // Initialize volume visualization shells
   m_VolVizShells.OnCreateDevice(m_pd3dDevice);


   //
   // Create textures
   //

   for (int i=0; i < NUM_COOKIES; i++)
   {
      SAFE_RELEASE (m_pCookies[i]);

      hr = D3DXCreateTextureFromFileEx(m_pd3dDevice, g_strCookieFiles[i], D3DX_DEFAULT, D3DX_DEFAULT, 
      0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
      D3DX_DEFAULT, 0, NULL, NULL, &m_pCookies[i]);
   }


   SAFE_RELEASE (m_pScrollingNoise);

   hr = D3DXCreateTextureFromFileEx(m_pd3dDevice, "textures\\cloud.tga", D3DX_DEFAULT, D3DX_DEFAULT, 
   0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
   D3DX_DEFAULT, 0, NULL, NULL, &m_pScrollingNoise);


   SAFE_RELEASE (m_pFogBuffer);

   if (FAILED (hr = D3DXCreateTexture (m_pd3dDevice, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT , &m_pFogBuffer)))
   {
       return hr;
   }

	if (FAILED (m_pFogBuffer->GetSurfaceLevel(0, &m_pFogBufferSurf)))
   {
		return E_FAIL;
   }


   SAFE_RELEASE (m_pShadowMap);

   if (FAILED (hr = D3DXCreateTexture (m_pd3dDevice, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_L16, D3DPOOL_DEFAULT , &m_pShadowMap)))
   {
       return hr;
   }

	if (FAILED (m_pShadowMap->GetSurfaceLevel(0, &m_pShadowMapSurf)))
   {
		return E_FAIL;
   }

	if (FAILED (m_pd3dDevice->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowMapZ, NULL)))
   {
		return E_FAIL;
   }


   //
   // The following "shadow maps" are for the walls and floor to give them a soft edge
   //
   // See "Poisson Shadow Blur" chapter in ShaderX3, Charles River Media 2004
   //
   // The texture m_pBlackAndWhiteShadowMap is an 8 bit texture which is not mip-mapped
   // A black object is drawn into this map
   //
   // The black and white image in m_pBlackAndWhiteShadowMap is filtered into m_pFilteredBlackAndWhiteShadowMap
   // This operation uses a growable Poisson filter to simulate a blurry penumbras farther from the object
   // The map is then auto-gen mip-mapped
   //

   SAFE_RELEASE (m_pBlackAndWhiteShadowMap);

   if (FAILED (hr = D3DXCreateTexture (m_pd3dDevice, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_L8, D3DPOOL_DEFAULT , &m_pBlackAndWhiteShadowMap)))
   {
       return hr;
   }

	if (FAILED (m_pBlackAndWhiteShadowMap->GetSurfaceLevel (0, &m_pBlackAndWhiteShadowMapSurf)))
   {
		return E_FAIL;
   }

   SAFE_RELEASE (m_pFilteredBlackAndWhiteShadowMap);

   if (FAILED (hr = D3DXCreateTexture (m_pd3dDevice, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, D3DUSAGE_RENDERTARGET|D3DUSAGE_AUTOGENMIPMAP, D3DFMT_L8, D3DPOOL_DEFAULT , &m_pFilteredBlackAndWhiteShadowMap)))
   {
       return hr;
   }

   // Set the filter type since this texture will be automipgen'd
   m_pFilteredBlackAndWhiteShadowMap->SetAutoGenFilterType(D3DTEXF_LINEAR);

	if (FAILED (m_pFilteredBlackAndWhiteShadowMap->GetSurfaceLevel (0, &m_pFilteredBlackAndWhiteShadowMapSurf)))
   {
		return E_FAIL;
   }

   //
   // Textures for procedural shading
   //

   // Create and fill the spline textures
   if (FAILED (InitSplineTextures()))
   {
      return E_FAIL;
   }

   // Create and load the volume noise texture
   if (FAILED (InitNoiseTexture()))
   {
      return E_FAIL;
   }

   //
   // Maps for ambient radiosity and ambient occlusion
   //

   // Create and load the ambient cube map
   if (FAILED (InitAmbientCube()))
   {
      return E_FAIL;
   }

   // Create the ambient occlusion map
   if (FAILED (InitAmbientOcclusionTexture()))
   {
      return E_FAIL;
   }

   return S_OK;
}



    
//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects(void)
{
   HRESULT hr;

   // Restore the font
   m_pFont->RestoreDeviceObjects();
   m_pFontSmall->RestoreDeviceObjects();


   //
   // Set up the spotlight object...
   //

   m_Spotlight.SetView( D3DXVECTOR3( 0.0f, 210.0f, 0.0f),   // vEyePt
                        D3DXVECTOR3( 0.0f, 50.0f, 0.0f),    // vLookAtPt
                        D3DXVECTOR3( 0.0f, 1.0f, 0.0f),     // vUpVec
                        m_fWidth ,
                        m_fHeight ,
                        m_fNearPlane ,           // fZNear
                        m_fFarPlane );           // fZFar

   // Initialize spotlight parameters
   m_Spotlight.SetWidth(m_fWidth);
   m_Spotlight.SetHeight(m_fHeight);
   
   if (FAILED (hr = m_Spotlight.OnResetDevice()))
   {
      return hr;
   }

   // Set light color and cookie
   m_Spotlight.SetLightColor(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
   m_Spotlight.SetCookie(m_pCookies[m_CurrentCookie]);

   // Set the ArcBall parameters
   m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, ARCBALL_RADIUS);
   m_ArcBall.SetTranslationRadius (ARCBALL_TRANSLATION_RADIUS);


   LPD3DXBUFFER pBufferErrors = NULL;
   hr = D3DXCreateEffectFromFile (m_pd3dDevice, TEXT("effects\\Spotlight_Object.fx"), NULL, NULL, 0, NULL, &m_pEffect, &pBufferErrors );
   if (FAILED (hr))
   {
      return hr;
   }

   m_Backdrop.OnResetDevice();

   m_ShadowMapOverlayQuad.OnResetDevice();

   m_CurrentCookieOverlayQuad.OnResetDevice();

   m_FinalFogOverlayQuad.OnResetDevice();

   m_VolVizShells.OnResetDevice();

   m_pEffect->OnResetDevice();

   return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects(void)
{
   HRESULT hr;

   m_pFont->InvalidateDeviceObjects();
   m_pFontSmall->InvalidateDeviceObjects();

   // OnLostDevice
   if (FAILED (hr = m_Spotlight.OnLostDevice())) 
   {
      return hr;
   }

   m_pEffect->OnLostDevice();

   m_Backdrop.OnLostDevice();

   m_ShadowMapOverlayQuad.OnLostDevice();

   m_CurrentCookieOverlayQuad.OnLostDevice();

   m_FinalFogOverlayQuad.OnLostDevice();

   m_VolVizShells.OnLostDevice();
  
   return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects(void)
{
   HRESULT hr;

   m_pFont->DeleteDeviceObjects();
   m_pFontSmall->DeleteDeviceObjects();

   // OnDestroyDevice
   if (FAILED (hr = m_Spotlight.OnDestroyDevice())) 
   {
      return hr;
   }

   // Free the 3DS Object
   d3dFreeModel (&m_3DSObject);
   
   m_Backdrop.OnDestroyDevice();

   m_ShadowMapOverlayQuad.OnDestroyDevice();

   m_CurrentCookieOverlayQuad.OnDestroyDevice();

   m_FinalFogOverlayQuad.OnDestroyDevice();

   m_VolVizShells.OnDestroyDevice();

   SAFE_RELEASE(m_pEffect);


   //
   // Release Textures and surfaces
   //
   SAFE_RELEASE (m_pShadowMap);
   SAFE_RELEASE (m_pShadowMapZ);
   SAFE_RELEASE (m_pShadowMapSurf);
   SAFE_RELEASE (m_pBlackAndWhiteShadowMapSurf);
   SAFE_RELEASE (m_pFilteredBlackAndWhiteShadowMapSurf);

   SAFE_RELEASE (m_pFogBuffer);
   SAFE_RELEASE (m_pFilteredFogBuffer);
   SAFE_RELEASE (m_pFogBufferSurf);

   SAFE_RELEASE (m_pBlackAndWhiteShadowMap);
   SAFE_RELEASE (m_pFilteredBlackAndWhiteShadowMap);

   SAFE_RELEASE (m_pVolumeNoise);
   SAFE_RELEASE (m_pAmbientCube);   
   SAFE_RELEASE (m_pAmbientOcclusion);
   
   SAFE_RELEASE (m_pMarbleColorSplineTexture);

   SAFE_RELEASE (m_pScrollingNoise);

   for (int i=0; i < NUM_COOKIES; i++)
   {
      SAFE_RELEASE (m_pCookies[i]);
   }

   return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove(void)
{
   // Local scratch matrices for positioning 3DS Object
   D3DXMATRIX matInvView, matStartView, matArcBall, mat3DSObjectScale, mat3DSObjectSpinX, mat3DSObjectSpinY, mat3DSObjectSpinZ, mat3DSObjectTranslation;
   FLOAT fObjectTime;

   // Static View Matrix
   D3DXVECTOR3 vEyePt    = D3DXVECTOR3 (0.0f, 400.0f, -400.0f);
   D3DXVECTOR3 vLookatPt = D3DXVECTOR3 (0.0f, 0.0f,  0.0f);
   D3DXVECTOR3 vUpVec    = D3DXVECTOR3 (0.0f, 1.0f,  0.0f);
   D3DXMatrixLookAtLH (&matStartView, &vEyePt, &vLookatPt, &vUpVec);


   if(m_bFirstUnpausedFrame == TRUE)
   {
      m_fUnfrozenLightTime = m_fTime;
      m_bFirstUnpausedFrame = FALSE;
   }

   if(m_bFirstPausedFrame == TRUE)
   {
      m_fFrozenLightTime = m_fFrozenLightTime + (m_fTime - m_fUnfrozenLightTime);
      m_bFirstPausedFrame = FALSE;
   }



   // Set time depending on animation
   if (m_bAnimateLights != FALSE)
   {
      m_fLightTime = m_fFrozenLightTime + (m_fTime - m_fUnfrozenLightTime);
   }
   else
   {
      m_fLightTime = m_fFrozenLightTime;
   }
   
   // Set time depending on animation
   if (m_ObjectState != OBJECT_STATE_FROZEN)
   {
      fObjectTime = m_fTime;
   }
   else
   {
      fObjectTime = m_fFrozenObjectTime;
   }


   // Make light orbit the scene
   m_vLightPos = D3DXVECTOR3(140*sinf(m_fLightTime/2.5f),  260.0f, 140*cosf(m_fLightTime/2.5f));

   m_Spotlight.SetView( m_vLightPos,                        // vEyePt
                           D3DXVECTOR3(0.0f, 100.0f, 0.0f), // vLookAtPt
                           D3DXVECTOR3(0.0f, 1.0f, 0.0f),   // vUpVec
                           m_fWidth ,             // Width
                           m_fHeight,            // Height
                           m_fNearPlane,          // Near
                           m_fFarPlane);         // Far

   // Static Projection Matrix for main viewport
   FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
   D3DXMatrixPerspectiveFovLH (&g_matProj, D3DX_PI/4.0f, fAspect, 5.0f, 2000.0f);

   // World matrix determined by ArcBall
   D3DXMatrixIdentity (&m_matWorld);

   memcpy(&m_matArcBallRot, m_ArcBall.GetRotationMatrix(), sizeof(D3DXMATRIX));
   memcpy(&m_matArcBallTrans, m_ArcBall.GetTranslationMatrix(), sizeof(D3DXMATRIX));

   D3DXMatrixMultiply (&matArcBall, &m_matArcBallRot, &m_matArcBallTrans);
   D3DXMatrixMultiply (&m_matView, &matArcBall, &matStartView);

   // Position statue in center of floor and invert
   D3DXMatrixScaling (&mat3DSObjectScale, 40.0f, 40.0f, -40.0f);  // Negate z to match original model
   D3DXMatrixRotationX (&mat3DSObjectSpinX, D3DX_PI/2.0f);
   D3DXMatrixRotationY (&mat3DSObjectSpinY, D3DX_PI/2.0f);
   D3DXMatrixIdentity (&m_mat3DSObjectWorld);
   D3DXMatrixMultiply (&m_mat3DSObjectWorld, &m_mat3DSObjectWorld, &mat3DSObjectScale);
   D3DXMatrixMultiply (&m_mat3DSObjectWorld, &m_mat3DSObjectWorld, &mat3DSObjectSpinX);
//   D3DXMatrixMultiply (&m_mat3DSObjectWorld, &m_mat3DSObjectWorld, &mat3DSObjectSpinY);

   // Compute model-space viewer and light for lighting which uses a model-space normal map
   D3DXMATRIX matInverse3DSObjectWorld;
//   D3DXMatrixInverse(&matInverse3DSObjectWorld, NULL, &m_mat3DSObjectWorld);     // Transformation from world to model
   D3DXMatrixScaling (&matInverse3DSObjectWorld, 1.0f/40.0f, 1.0f/40.0f, -1.0f/40.0f);

   D3DXMatrixInverse (&matInvView, NULL, &m_matView);                            // Transformation from view space to world space

   D3DXVECTOR3 vWSViewer, vOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
   D3DXVec3TransformCoord (&vWSViewer, &vOrigin, &matInvView);                        // World space viewer
   D3DXVec3TransformCoord (&m_vVmodel, &vWSViewer, &matInverse3DSObjectWorld);        // Model space viewer
   D3DXVec3TransformCoord (&m_vLmodel, &m_vLightPos, &matInverse3DSObjectWorld);      // Model space light

   // Global WorldView, ITWorldView, and WorldViewProjection matrices for scene
   D3DXMatrixMultiply (&g_matWorldView, &m_matWorld, &m_matView);
   D3DXMatrixMultiply (&g_matWorldViewProj, &g_matWorldView, &g_matProj);

   D3DXMatrixInverse (&g_matITWorldView, NULL, &g_matWorldView);
   D3DXMatrixTranspose (&g_matITWorldView, &g_matITWorldView);

   // Global WorldView ITWorldView and WorldViewProjection matrices for 3DS Object
   D3DXMatrixMultiply (&m_mat3DSObjectWorldView, &m_mat3DSObjectWorld, &m_matView);
   D3DXMatrixMultiply (&m_mat3DSObjectWorldViewProj, &m_mat3DSObjectWorldView, &g_matProj);

   D3DXMatrixInverse (&m_mat3DSObjectITWorldView, NULL, &m_mat3DSObjectWorldView);
   D3DXMatrixTranspose (&m_mat3DSObjectITWorldView, &m_mat3DSObjectITWorldView);

   // Compose scale and bias matrix for projective texture
   D3DXMATRIX matHalfScale, matHalfBias, matBias, matScroll1, matScaleScroll1, matScroll2, matScaleScroll2;
   D3DXMatrixScaling (&matHalfScale, 0.5f, -0.5f, 0.5f);  // Funny hack to fix up projection...not 100% sure why this works
   D3DXMatrixScaling (&matScaleScroll1, 0.6f, -0.6f, 0.1f);
   D3DXMatrixScaling (&matScaleScroll2, 0.6f, -0.6f, 0.1f);
   D3DXMatrixTranslation (&matHalfBias, 0.5f, 0.5f, 0.5f);
   D3DXMatrixTranslation (&matScroll1, fmodf(0.073f*m_fTime, 1.0f), -fmodf(0.082f*m_fTime, 1.0f), 0.5f);
   D3DXMatrixTranslation (&matScroll2, fmodf(0.063f*m_fTime, 1.0f), fmodf(0.058f*m_fTime, 1.0f), 0.5f);
   D3DXMatrixMultiply (&matBias, &matHalfScale, &matHalfBias);
   D3DXMatrixMultiply (&matScroll1, &matScaleScroll1, &matScroll1);
   D3DXMatrixMultiply (&matScroll2, &matScaleScroll2, &matScroll2);

   // Light WorldView, ITWorldView, and WorldViewProjection matrices for scene
   D3DXMatrixMultiply (&m_matWorldLight, &m_matWorld, m_Spotlight.GetViewMatrix());
   D3DXMatrixMultiply (&m_matWorldLightProj, &m_matWorldLight, m_Spotlight.GetProjectionMatrix());

   D3DXMatrixInverse (&m_matITWorldLight, NULL, &m_matWorldLight);
   D3DXMatrixTranspose (&m_matITWorldLight, &m_matITWorldLight);

   // Light WorldView ITWorldView and WorldViewProjection matrices for 3DS Object
   D3DXMatrixMultiply (&m_matLight3DSObjectWorldView, &m_mat3DSObjectWorld, m_Spotlight.GetViewMatrix());
   D3DXMatrixMultiply (&m_matLight3DSObjectWorldViewProj, &m_matLight3DSObjectWorldView, m_Spotlight.GetProjectionMatrix());

   D3DXMatrixInverse (&m_matLight3DSObjectITWorldView, NULL, &m_matLight3DSObjectWorldView);   
   D3DXMatrixTranspose (&m_matLight3DSObjectITWorldView, &m_matLight3DSObjectITWorldView);

   // Texture matrices for volviz slices
   D3DXMatrixMultiply (&m_matWorldLightProjBias, &m_matWorldLightProj, &matBias);
   D3DXMatrixMultiply (&m_matWorldLightProjScroll1, &m_matWorldLightProj, &matScroll2);
   D3DXMatrixMultiply (&m_matWorldLightProjScroll1, &m_matWorldLightProj, &matScroll2);
   D3DXMatrixMultiply (&m_matViewWorldLightProjBias, &matInvView, &m_matWorldLightProjBias);
   D3DXMatrixMultiply (&m_matViewWorldLightProjScroll1, &matInvView, &m_matWorldLightProjScroll1);
   D3DXMatrixMultiply (&m_matViewWorldLightProjScroll2, &matInvView, &m_matWorldLightProjScroll2);
   D3DXMatrixMultiply (&m_matViewWorldLightProj, &matInvView, &m_matWorldLightProj);

   // Texture matrices for backdrop
   D3DXMatrixMultiply (&m_matLightProj, m_Spotlight.GetViewMatrix(), m_Spotlight.GetProjectionMatrix());
   D3DXMatrixMultiply (&m_matLightProjBias, &m_matLightProj, &matBias);
   D3DXMatrixMultiply (&m_matLightProjScroll1, &m_matLightProj, &matScroll1);
   D3DXMatrixMultiply (&m_matLightProjScroll2, &m_matLightProj, &matScroll2);

   // View->Light space matrix for scattering equations
   D3DXMatrixMultiply (&m_matViewWorldLight, &matInvView, m_Spotlight.GetViewMatrix());

   // Texture matrices for 3DS Object
   D3DXMatrixMultiply (&m_matLight3DSObjectWorldViewProjBias, &m_matLight3DSObjectWorldViewProj, &matBias);
   D3DXMatrixMultiply (&m_matLight3DSObjectWorldViewProjScroll1, &m_matLight3DSObjectWorldViewProj, &matScroll1);
   D3DXMatrixMultiply (&m_matLight3DSObjectWorldViewProjScroll2, &m_matLight3DSObjectWorldViewProj, &matScroll2);

   // Matrix for transforming clip planes from worldspace to clip space
   D3DXMatrixInverse (&m_matITWorldViewProjection, NULL, &g_matWorldViewProj);
   D3DXMatrixTranspose (&m_matITWorldViewProjection, &m_matITWorldViewProjection);

   return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render(void)
{
   HRESULT hr;
   UINT iPass, cPasses;
   LPDIRECT3DSURFACE9 pMainBackBuffer, pMainZBuffer;

   // ============================================================================================================
   //
   //   Draw to the shadow map if light or object is moving or FOV just changed (and we're shadow mapping at all)
   //
   // ============================================================================================================

   if (((m_bAnimateLights != FALSE) || (m_ObjectState != OBJECT_STATE_INVISIBLE) || (m_bShadowMapDirty != FALSE)) && (m_bShadowMapping != FALSE))
   {
      m_bShadowMapDirty = FALSE;

      // Save old render target
      m_pd3dDevice->GetRenderTarget(0, &pMainBackBuffer);
      m_pd3dDevice->GetDepthStencilSurface(&pMainZBuffer);

      m_pd3dDevice->SetRenderTarget(0, m_pShadowMapSurf);
      m_pd3dDevice->SetDepthStencilSurface(m_pShadowMapZ);

      if (FAILED(hr = m_pd3dDevice->BeginScene()))
      {
         return hr;
      }

      // Set the Viewport to the whole shadow map
      m_pd3dDevice->SetViewport(&m_ShadowMapViewport);

      // Clear the depth/stencil surface
      m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

      // Clear the high-precision scalar buffer to the maximum value
      m_ShadowMapOverlayQuad.SetClearColor(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
      m_ShadowMapOverlayQuad.Draw(0);


      //
      // First render the object
      //

      if (m_ObjectState != OBJECT_STATE_INVISIBLE)
      {
         m_pEffect->SetTechnique(m_pEffect->GetTechniqueByName("depth_technique"));

         m_pEffect->SetFloat ("fDepthScaleFactor", m_fFarPlane);

         m_pEffect->SetMatrixTranspose ("matWorldViewProj", &m_matLight3DSObjectWorldViewProj);
         m_pEffect->SetMatrixTranspose ("matLightWorldViewProj", &m_matLight3DSObjectWorldViewProj); // (same as above matrix, for this pass, but different later)
         m_pEffect->SetMatrixTranspose ("matWorldView", &m_matLight3DSObjectWorldView);
         m_pEffect->SetMatrixTranspose ("matITWorldView", &m_matLight3DSObjectITWorldView);

         m_pEffect->Begin(&cPasses, 0);
         for (iPass = 0; iPass < cPasses; iPass++)
         {
            m_pEffect->Pass(iPass);
            d3dRenderModelNoMat (&m_3DSObject,  m_pd3dDevice);
         }
         m_pEffect->End();
      }

      if (FAILED (hr = m_pd3dDevice->EndScene()))
      {
         return hr;
      }

      // These got AddRef'd above
      pMainBackBuffer->Release();
      pMainZBuffer->Release();


      //
      // Render to m_pBlackAndWhiteShadowMap
      //

      // Set the render target
      m_pd3dDevice->SetRenderTarget(0, m_pBlackAndWhiteShadowMapSurf);

      // Set the Viewport to the whole black and whiteshadow map
//      m_pd3dDevice->SetViewport(&m_ShadowMapViewport);

      // Clear the surface to white
      m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xFFFFFFFF, 1.0f, 0);

      if (FAILED (hr = m_pd3dDevice->BeginScene()))
      {
         return hr;
      }

      //
      // Render the object
      //
      if (m_ObjectState != OBJECT_STATE_INVISIBLE)
      {
         m_pEffect->SetTechnique(m_pEffect->GetTechniqueByName("black_technique"));

         m_pEffect->SetMatrixTranspose ("matWorldViewProj", &m_matLight3DSObjectWorldViewProj);
         m_pEffect->SetMatrixTranspose ("matLightWorldViewProj", &m_matLight3DSObjectWorldViewProj); // (same as above matrix, for this pass, but different later)
         m_pEffect->SetMatrixTranspose ("matWorldView", &m_matLight3DSObjectWorldView);
         m_pEffect->SetMatrixTranspose ("matITWorldView", &m_matLight3DSObjectITWorldView);

         m_pEffect->Begin(&cPasses, 0);
         for (iPass = 0; iPass < cPasses; iPass++)
         {
            m_pEffect->Pass(iPass);
            d3dRenderModelNoMat (&m_3DSObject,  m_pd3dDevice);
         }
         m_pEffect->End();
      }

      if (FAILED (hr = m_pd3dDevice->EndScene()))
      {
         return hr;
      }


      // Render to Filtered Black And White Shadow Map

      m_pd3dDevice->SetRenderTarget(0, m_pFilteredBlackAndWhiteShadowMapSurf);

      if (FAILED (hr = m_pd3dDevice->BeginScene()))
      {
         return hr;
      }

      // Set the Viewport to the whole Filtered Black And White Shadow Map
      m_pd3dDevice->SetViewport(&m_ShadowMapViewport);

      // Clear the high-precision scalar buffer to the maximum value
      m_ShadowMapOverlayQuad.SetOverlayTexture(m_pBlackAndWhiteShadowMap);
      m_ShadowMapOverlayQuad.Draw(OVERLAY_QUAD_SHADOW_BLUR);

      m_pFilteredBlackAndWhiteShadowMap->GenerateMipSubLevels();

      if (FAILED (hr = m_pd3dDevice->EndScene()))
      {
         return hr;
      }

      m_pd3dDevice->SetRenderTarget(0, pMainBackBuffer);
      m_pd3dDevice->SetDepthStencilSurface(pMainZBuffer);

   }

   // ======================================================================================================
   //
   //   Draw to the back buffer
   //
   // ======================================================================================================

   if (FAILED (hr = m_pd3dDevice->BeginScene()))
   {
      return hr;
   }

   // Set the Viewport to the whole window
   m_pd3dDevice->SetViewport(&m_WholeWindowViewport);

   // Clear the whole window
   m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x09090909, 1.0f, 0);

   //
   // First render the object
   //

   // Select the proper technique
   if (m_bShowScrollingNoise != FALSE)
   {
      if (m_bShadowMapping != FALSE)
      {
         m_pEffect->SetTechnique(m_pEffect->GetTechniqueByName("noise_shadow_technique"));
      }
      else
      {
         m_pEffect->SetTechnique(m_pEffect->GetTechniqueByName("noise_noshadow_technique"));
      }
   }
   else
   {
      if (m_bShadowMapping != FALSE)
      {
         m_pEffect->SetTechnique(m_pEffect->GetTechniqueByName("nonoise_shadow_technique"));
      }
      else
      {
         m_pEffect->SetTechnique(m_pEffect->GetTechniqueByName("nonoise_noshadow_technique"));
      }
   }


   // Set up the necessary transforms
   m_pEffect->SetMatrixTranspose ("matWorldViewProj", &m_mat3DSObjectWorldViewProj);
   m_pEffect->SetMatrixTranspose ("matLightWorldViewProj", &m_matLight3DSObjectWorldViewProj); // for computing light-space projective depth
   m_pEffect->SetMatrixTranspose ("matWorldLight", &m_matLight3DSObjectWorldView); // for transforming from object space to light-space 
   m_pEffect->SetMatrixTranspose ("matWorldView", &m_mat3DSObjectWorldView);
   m_pEffect->SetMatrixTranspose ("matITWorldView", &m_mat3DSObjectITWorldView);
   m_pEffect->SetMatrixTranspose ("matWorldLightProjBias",  &m_matLight3DSObjectWorldViewProjBias);
   m_pEffect->SetMatrixTranspose ("matWorldLightProjScroll1",  &m_matLight3DSObjectWorldViewProjScroll1);
   m_pEffect->SetMatrixTranspose ("matWorldLightProjScroll2",  &m_matLight3DSObjectWorldViewProjScroll2);

   // Set up the necessary textures
   m_pEffect->SetTexture ("tCookie",           m_Spotlight.GetCookie());
   m_pEffect->SetTexture ("tScrollingNoise",   m_pScrollingNoise);
   m_pEffect->SetTexture ("tShadowMap",        m_pShadowMap);
   m_pEffect->SetTexture ("tMarbleSpline",     m_pMarbleColorSplineTexture);
   m_pEffect->SetTexture ("tVolumeNoise",      m_pVolumeNoise);
   m_pEffect->SetTexture ("tAmbientCube",      m_pAmbientCube);
   m_pEffect->SetTexture ("tAmbientOcclusion", m_pAmbientOcclusion);
   m_pEffect->SetFloat   ("fDepthScaleFactor", m_fFarPlane);

   // Set up the model space quantities for lighting
   D3DXVECTOR4 v4 = D3DXVECTOR4(m_vVmodel.x, m_vVmodel.y, m_vVmodel.z, 1.0f);
   m_pEffect->SetVector("g_Vmodel",  &v4);
   v4 = D3DXVECTOR4(m_vLmodel.x, m_vLmodel.y, m_vLmodel.z, 1.0f);
   m_pEffect->SetVector("g_Lmodel",  &v4);
 
  
   if (m_ObjectState != OBJECT_STATE_INVISIBLE)
   {
      m_pEffect->Begin(&cPasses, 0);
      for (iPass = 0; iPass < cPasses; iPass++)
      {
         m_pEffect->Pass(iPass);
         d3dRenderModelNoMat (&m_3DSObject,  m_pd3dDevice);
      }
      m_pEffect->End();
   }

   // Draw the backdrop with texture projected on it
   m_Backdrop.SetWorldLightProjBiasMatrix(&m_matLightProjBias);
   m_Backdrop.SetWorldLightProjScrollMatrices(&m_matLightProjScroll1, &m_matLightProjScroll2);
   m_Backdrop.SetWorldLightProjMatrix(&m_matWorldLightProj);
   m_Backdrop.SetWorldViewProjMatrix(&g_matWorldViewProj);
   m_Backdrop.SetWorldLightMatrix(&m_matWorldLight);
   m_Backdrop.SetWorldViewMatrix(&g_matWorldView);
   m_Backdrop.SetITWorldViewMatrix(&g_matITWorldView);

   m_Backdrop.SetTextureHandles(m_Spotlight.GetCookie(), m_pScrollingNoise, m_pFilteredBlackAndWhiteShadowMap);

   m_Backdrop.SetLightParameters(m_Spotlight.GetLightColor(), m_Spotlight.GetWidth(), m_Spotlight.GetHeight());

   m_Backdrop.SetSceneLightPos(m_vSceneLightPos);

   //
   // Draw the backdrop
   //

   DWORD dwBackdropDrawFlags = 0;

   if (m_bShowScrollingNoise != FALSE)
   {
      dwBackdropDrawFlags |= BACKDROP_SCROLLING_NOISE;
   }

   if ((m_bShadowMapping != FALSE) && (m_ObjectState != OBJECT_STATE_INVISIBLE))
   {
      dwBackdropDrawFlags |= BACKDROP_SHADOW_MAPPING;
   }

   if (m_bUsingCookie != FALSE)
   {
      dwBackdropDrawFlags |= BACKDROP_COOKIE;
   }

   m_Backdrop.Draw(dwBackdropDrawFlags);


   //
   // Draw the light
   //

   DWORD dwLightDrawFlags = 0;

   if (m_bSpotlightVolume != FALSE)
   {
      dwLightDrawFlags |= SPOTLIGHT_SHOW_FRUSTUM;
   }

   if (m_bShowLight != FALSE)
   {
      dwLightDrawFlags |= SPOTLIGHT_SHOW_LIGHT;
   }

   if (m_bShowClippingFrustum != FALSE)
   {
      dwLightDrawFlags |= SPOTLIGHT_SHOW_CLIPPING_FRUSTUM;
   }

   m_Spotlight.Draw(dwLightDrawFlags);

   if (FAILED (hr = m_pd3dDevice->EndScene()))
   {
      return hr;
   }



   // ======================================================================================================
   //
   //   Draw to the fog buffer
   //
   // ======================================================================================================


   // View-space bounding box of light frustum
   D3DXVECTOR4 vMinBounds, vMaxBounds;

   if (m_bShowLightShafts != FALSE)
   {
      if (m_bFreezeVolVizShells)
      {
         D3DXPlaneTransformArray (m_ClipSpaceLightFrustumPlanes, sizeof(D3DXPLANE), m_WorldSpaceLightFrustumPlanes, sizeof(D3DXPLANE), &m_matITWorldViewProjection, 6);

         m_VolVizShells.SetClipPlanes(m_ClipSpaceLightFrustumPlanes);

         m_VolVizShells.DrawFrozenShells();
      }
      else
      {
         // Save old render target
         m_pd3dDevice->GetRenderTarget(0, &pMainBackBuffer);

         m_pd3dDevice->SetRenderTarget(0, m_pFogBufferSurf);

         // Clear the depth/stencil surface
         m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);

         if (FAILED (hr = m_pd3dDevice->BeginScene()))
         {
            return hr;
         }

         // Get the view-space bounds on the frustum for this light
         m_Spotlight.GetViewSpaceBounds(&vMinBounds, &vMaxBounds);

         // Set volviz shells to span view-aligned parallelepiped which bounds the light frustum, spacing planes a fixed distance apart
         m_VolVizShells.SetVolVizBounds(vMinBounds, vMaxBounds, m_fSamplingDelta);

         // Get the world-space clipping planes for this light
         m_Spotlight.GetWorldSpaceFrustumPlanes(m_WorldSpaceLightFrustumPlanes);

         // Transform the clip planes to clip-space for the viewer
         D3DXPlaneTransformArray (m_ClipSpaceLightFrustumPlanes, sizeof(D3DXPLANE), m_WorldSpaceLightFrustumPlanes, sizeof(D3DXPLANE), &m_matITWorldViewProjection, 6);

         // Pass the clip-space planes to the shells
         m_VolVizShells.SetClipPlanes(m_ClipSpaceLightFrustumPlanes);

         // Pass in the View->World->Light->Project->Bias matrix in for projection onto the volviz shells
         m_VolVizShells.SetViewWorldLightProjBiasMatrix(&m_matViewWorldLightProjBias);

         // Pass in the View->World->Light->Project->Scroll matrix in for scrolling projection onto the volviz shells
         m_VolVizShells.SetViewWorldLightProjScrollMatrices(&m_matViewWorldLightProjScroll1, &m_matViewWorldLightProjScroll2);

         // Pass in the View->World->Light->Project matrix to compute projective depth of volviz shells
         m_VolVizShells.SetViewWorldLightProjMatrix(&m_matViewWorldLightProj);

         // Pass in the View->World->Light matrix in for scattering math
         m_VolVizShells.SetViewWorldLightMatrix(&m_matViewWorldLight);

         // Set the correct cookie texture, scrolling noise and shadow map
         m_VolVizShells.SetTextures(m_Spotlight.GetCookie(), m_pScrollingNoise, m_pShadowMap);

         // Set the correct cookie texture, scrolling noise and shadow map
         m_VolVizShells.SetLightParameters(m_Spotlight.GetLightColor(), m_Spotlight.GetWidth(), m_Spotlight.GetHeight(), m_fFarPlane);

         // Draw the volviz shells which make up the light shafts
         DWORD dwShellsDrawFlags = VOLVIZ_SHELLS_CLIP; // always clip
         
         if (m_bUsingCookie != FALSE)
         {
            dwShellsDrawFlags |= VOLVIZ_COOKIE;
         }

         if (m_bShowScrollingNoise != FALSE)
         {
            dwShellsDrawFlags |= VOLVIZ_SCROLLING_NOISE;
         }

         if (m_bShadowMapping != FALSE)
         {
            dwShellsDrawFlags |= VOLVIZ_SHADOW_MAPPING;
         }

         m_VolVizShells.Draw(dwShellsDrawFlags);

         if (FAILED (hr = m_pd3dDevice->EndScene()))
         {
            return hr;
         }

         // These got AddRef'd above
         pMainBackBuffer->Release();

         // Set render target to back buffer
         m_pd3dDevice->SetRenderTarget(0, pMainBackBuffer);
      }
   }


   // ======================================================================================================
   //
   //   Draw overlay quads to the back buffer
   //
   // ======================================================================================================

   if (FAILED (hr = m_pd3dDevice->BeginScene()))
   {
      return hr;
   }

   if ((m_bShowLightShafts == TRUE) && (m_bFreezeVolVizShells == FALSE))
   {
      // Composite quad onto back buffer
      m_FinalFogOverlayQuad.SetOverlayTexture(m_pFogBuffer);
      m_FinalFogOverlayQuad.Draw(OVERLAY_QUAD_COMPOSITE_FOG_BUFFER);
   }

   // Optionally show the shadow map
   if ((m_ShowShadowMapMode != SHOW_SHADOW_MAP_MODE_NONE) && (m_bShadowMapping != FALSE))
   {
      // Select the proper texture
      if (m_ShowShadowMapMode == SHOW_SHADOW_MAP_MODE_FILTERED)
      {
         m_ShadowMapOverlayQuad.SetOverlayTexture(m_pFilteredBlackAndWhiteShadowMap);
      }
      else if (m_ShowShadowMapMode == SHOW_SHADOW_MAP_MODE_BLACK_AND_WHITE)
      {
         m_ShadowMapOverlayQuad.SetOverlayTexture(m_pBlackAndWhiteShadowMap);
      }
      else // must be SHOW_SHADOW_MAP_MODE_DEPTH
      {
         m_ShadowMapOverlayQuad.SetOverlayTexture(m_pShadowMap);
      }

      // Draw the overlay quad
      m_ShadowMapOverlayQuad.Draw(OVERLAY_QUAD_SHOW_TEXTURE);
   }

   // Optionally show the current cookie as well
   if (m_bShowCurrentCookie != FALSE)
   {
      m_CurrentCookieOverlayQuad.SetOverlayTexture(m_Spotlight.GetCookie());
      m_CurrentCookieOverlayQuad.Draw(OVERLAY_QUAD_SHOW_TEXTURE);
   }

   if ((m_bDisplayHelp != FALSE) && ((m_ShowShadowMapMode == SHOW_SHADOW_MAP_MODE_NONE) || (m_bShadowMapping == FALSE)))
   {
      //
      // Output statistics and UI
      //
      char strBuff[128];
      float lineNum = 0;
      float lineSpacing = 13.0f;
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), m_strFrameStats);
      lineNum+=lineSpacing;
//      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), m_strDeviceStats);
//      lineNum+=lineSpacing;

      lineNum-=8;
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,128,128,128), "_________________");
      lineNum+= (lineSpacing+2);

      if (m_bAnimateLights != FALSE)
      {
         sprintf (strBuff, "A - Stop light animation");
      }
      else
      {
         sprintf (strBuff, "A - Start light animation");
      }
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      if (m_bShowClippingFrustum != FALSE)
      {
         sprintf (strBuff, "F - Hide Light Frustum");
      }
      else
      {
         sprintf (strBuff, "F - Show Light Frustum");
      }
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      sprintf (strBuff, "D - Cycle to next shadow map mode");
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      if (m_ObjectState == OBJECT_STATE_FROZEN)
      {
         sprintf (strBuff, "O - Hide Object");
      }
      else
      {
         sprintf (strBuff, "O - Show Object");
      }
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      if (m_bShadowMapping != FALSE)
      {
         sprintf (strBuff, "S - Turn off shadows");
      }
      else
      {
         sprintf (strBuff, "S - Turn on shadows");
      }
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      if (m_bShowScrollingNoise != FALSE)
      {
         sprintf (strBuff, "N - Turn off noise");
      }
      else
      {
         sprintf (strBuff, "N - Turn on noise");
      }
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      if (m_bShowLightShafts != FALSE)
      {
         sprintf (strBuff, "L - Turn off atmospheric shader");
      }
      else
      {
         sprintf (strBuff, "L - Turn on atmospheric shader");
      }
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;

      lineNum-=8;
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,128,128,128), "_________________");
      lineNum+= (lineSpacing+2);

      sprintf (strBuff, "Left Drag to rotate scene");
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;
      sprintf (strBuff, "Right Drag to translate scene in plane of screen");
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing;
      sprintf (strBuff, "Middle Drag to translate scene perpendicular to screen");
      m_pFontSmall->DrawText (2, lineNum, D3DCOLOR_ARGB (255,255,255,255), strBuff);
      lineNum+=lineSpacing+10;
   }

   if (FAILED (hr = m_pd3dDevice->EndScene()))
   {
      return hr;
   }

   return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup(void)
{
   SAFE_DELETE (m_pFont);
   SAFE_DELETE (m_pFontSmall);

   FreeModel (&m_3DSObject);

   return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ColorPick(D3DCOLORVALUE *rgbColor)
// Desc:
//-----------------------------------------------------------------------------
void CMyD3DApplication::ColorPick(D3DCOLORVALUE *rgbColor)
{
   CHOOSECOLOR cc;
   COLORREF    crCustColors[16];
   COLORREF    rgb;
   BYTE        red, green, blue;

   red   = (BYTE) ((rgbColor->r) * 255.0f);
   green = (BYTE) ((rgbColor->g) * 255.0f);
   blue  = (BYTE) ((rgbColor->b) * 255.0f);

   rgb = RGB((BYTE)((red>>16)&0xff), (BYTE)((green>>8)&0xff), (BYTE)(blue&0xff));

   cc.lStructSize		= sizeof(CHOOSECOLOR);
   cc.hwndOwner		= m_hWnd;
   cc.hInstance		= m_hWnd;
   cc.rgbResult		= rgb;
   cc.lpCustColors	= crCustColors;
   cc.Flags			   = CC_RGBINIT | CC_FULLOPEN;
   cc.lCustData		= 0l;
   cc.lpfnHook			= NULL;
   cc.lpTemplateName	= NULL;

   if (ChooseColor(&cc))
   {
      rgbColor->r = (FLOAT) (GetRValue(cc.rgbResult) / 255.0f);
      rgbColor->g = (FLOAT) (GetGValue(cc.rgbResult) / 255.0f);
      rgbColor->b = (FLOAT) (GetBValue(cc.rgbResult) / 255.0f);
   }

}


//-----------------------------------------------------------------------------
// Name: MsgProc
// Desc:
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LPD3DXBUFFER pBufferErrors = NULL;
   LPD3DXEFFECT pDummyEffect  = NULL;

   m_ArcBall.HandleMessages( hWnd, uMsg, wParam, lParam );

   // Trap context menu
   if (WM_CONTEXTMENU == uMsg )
   {
      return 0;
   }

    // Handle menu commands
    if (WM_COMMAND == uMsg )
    {
        switch( LOWORD(wParam) )
        {
            case IDM_TOGGLE_DISPLAY_HELP:
               m_bDisplayHelp = !m_bDisplayHelp;
               break;
          
            case IDM_CHOOSE_LIGHT_COLOR:
               D3DCOLORVALUE rgbColor;
               ColorPick(&rgbColor);

               m_Spotlight.SetLightColor(D3DXVECTOR4(rgbColor.r, rgbColor.g, rgbColor.b, 1.0f));

               break;


            case IDM_REFRESH_EFFECTS:

                HRESULT hr;

                hr = D3DXCreateEffectFromFile(m_pd3dDevice, TEXT("effects\\Spotlight_Object.fx"), NULL, NULL, 0, NULL, &pDummyEffect, &pBufferErrors );

                if (FAILED (hr))
                {
                   m_bEffectSucceeded = FALSE;
                   OutputDebugString ( (char *) pBufferErrors->GetBufferPointer());
                }
                else
                {
                   m_bEffectSucceeded = TRUE;
                   SAFE_RELEASE(m_pEffect);

                   m_pEffect = pDummyEffect;
                }

                if (FAILED (hr = m_Spotlight.RefreshEffect()))
                {
                   return hr;
                }

                // Tell the other objects to refresh their effects
                m_Backdrop.RefreshEffect();
                m_ShadowMapOverlayQuad.RefreshEffect();
                m_CurrentCookieOverlayQuad.RefreshEffect();
                m_FinalFogOverlayQuad.RefreshEffect();
                m_VolVizShells.RefreshEffect();

                break;

             case IDM_TOGGLE_SHOW_CLIPPING_FRUSTUM:
                 m_bShowClippingFrustum = !m_bShowClippingFrustum;
                 break;

             case IDM_NEXT_SHOW_SHADOW_MAP_MODE:
                 m_ShowShadowMapMode = (m_ShowShadowMapMode + 1) % NUM_SHOW_SHADOW_MAP_MODES;
                 break;

             case IDM_TOGGLE_AMBIENT_CUBE:               // B
                m_bUseAmbientCube = !m_bUseAmbientCube;
                break;


             case IDM_TOGGLE_AMBIENT_OCCLUSION:          // M
                m_bUseAmbientOcclusion = !m_bUseAmbientOcclusion;
                break;

             case IDM_TOGGLE_LIGHT_SHAFTS:               // L
                 m_bShowLightShafts = !m_bShowLightShafts;
                 break;

             case IDM_TOGGLE_SHOW_TILING_NOISE:          // N
                 m_bShowScrollingNoise = !m_bShowScrollingNoise;
                 break;

             case IDM_TOGGLE_SHADOW_MAPPING:
                 m_bShadowMapping = !m_bShadowMapping;

                 m_bShadowMapDirty = TRUE;

                 break;

             case IDM_CYCLE_OBJECT_ANIMATION:
                 m_ObjectState = (m_ObjectState + 1) % NUM_OBJECT_STATES;

                 m_bShadowMapDirty = TRUE;

                 break;

             case IDM_TOGGLE_LIGHT_ANIMATION:
                 
                 m_bAnimateLights = !m_bAnimateLights;

                 if (m_bAnimateLights == FALSE)
                 {
                    m_bFirstPausedFrame = TRUE;
                 }
                 else
                 {
                    m_bFirstUnpausedFrame = TRUE;
                 }

                 break;

             case IDM_NEXT_COOKIE:
                 
                 m_CurrentCookie = (m_CurrentCookie + 1) % NUM_COOKIES;

                 m_Spotlight.SetCookie(m_pCookies[m_CurrentCookie]);

                 break;

             case IDM_PREV_COOKIE:
                 
                 m_CurrentCookie = (m_CurrentCookie + NUM_COOKIES - 1) % NUM_COOKIES;

                 m_Spotlight.SetCookie(m_pCookies[m_CurrentCookie]);

                 break;
/*

No parameter modification in this app...but left code in here in case it's needed...

            case IDM_INCREASE_CURRENT_PARAMETER:

               m_bShadowMapDirty = TRUE;

               switch (m_dwCurrentParameter)
               {
                  case SPOTLIGHT_PARAM_WIDTH:
                     m_fWidth += SPOTLIGHT_STEP_SIZE_WIDTH;
                     m_Spotlight.SetWidth(m_fWidth);
                     break;

                  case SPOTLIGHT_PARAM_HEIGHT:
                     m_fHeight += SPOTLIGHT_STEP_SIZE_WIDTH;
                     m_Spotlight.SetHeight(m_fHeight);
                     break;
               }

               break;
            case IDM_DECREASE_CURRENT_PARAMETER:

               // Mark shadow map dirty so that it will get updated
               m_bShadowMapDirty = TRUE;

               // Modify the appropriate spotlight parameter
               switch (m_dwCurrentParameter)
               {
                  case SPOTLIGHT_PARAM_WIDTH:

                     if (m_fWidth > 0.02f)
                     {                     
                        m_fWidth -= SPOTLIGHT_STEP_SIZE_WIDTH;
                        m_Spotlight.SetWidth(m_fWidth);
                     }
                     break;

                  case SPOTLIGHT_PARAM_HEIGHT:

                     if (m_fHeight > 0.02f)
                     {                     
                        m_fHeight -= SPOTLIGHT_STEP_SIZE_WIDTH;
                        m_Spotlight.SetHeight(m_fHeight);
                     }
                     break;
               }

               break;
            case IDM_NEXT_PARAMETER:
               m_dwCurrentParameter = (m_dwCurrentParameter + 1) % NUM_SPOTLIGHT_PARAMETERS;
               break;

            case IDM_PREVIOUS_PARAMETER:
               m_dwCurrentParameter = (m_dwCurrentParameter - 1 + NUM_SPOTLIGHT_PARAMETERS) % NUM_SPOTLIGHT_PARAMETERS;
               break;
*/

        }
    }

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}


