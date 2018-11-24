//-----------------------------------------------------------------------------
// File: BasicHLSL.cpp
//
// Desc: This sample shows a simple example of the Microsoft Direct3D's High-Level 
//       Shader Language (HLSL) using the Effect interface. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include <dxerr9.h>
#include "resource.h"
#include <iostream>
#include <fstream>
#include <string>
//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 
 
using namespace std;     
//-------------------GLOBAL VARIABLE------------------------------
 

float PI = 3.14159265f; 

// Custom D3D vertex format used by the vertex buffer
struct MYVERTEX
{
    D3DXVECTOR3 p;       // vertex position
    D3DXVECTOR3 n;       // vertex normal
 //	D3DXVECTOR4 bn;      // vertex bi-normal

    D3DXVECTOR2 tex;      // vertex normal

    static const DWORD FVF;
};
const DWORD MYVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_TEX1;

// Screen quad vertex format
struct ScreenVertex
{
    D3DXVECTOR4 p; // position
    D3DXVECTOR2 t; // texture coordinate

    static const DWORD FVF;
};
const DWORD ScreenVertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Main class to run this application. Most functionality is inherited
//       from the CD3DApplication base class.
//-----------------------------------------------------------------------------
int formulaID=0;


class CMyD3DApplication : public CD3DApplication
{
private:
	float					gFact;
 	float					gSpeed;
	float					gAnim;
	float                   gSeedRange;
	int						maxIter;
	int						tex_size;
	int						currColoring;

	int						stex_size ;
	bool					gJulia;
    bool                    m_bShowHelp;          // If true, it renders the UI control text
    

	D3DFORMAT				gtFormat;
	D3DXVECTOR2				gShift;
	
	ID3DXFont*              m_pFont;              // Font for drawing text
   	CModelViewerCamera      m_Camera;             // A model viewing camera
    LPD3DXEFFECT            m_pEffect;            // D3DX effect interface


	LPDIRECT3DTEXTURE9		m_pRenderTargetTexture;
	LPDIRECT3DTEXTURE9		m_pRenderTargetTexture1;
	LPDIRECT3DTEXTURE9		m_pRenderTargetTexture2;

    D3DXHANDLE              m_hFact;                        // Handle to var in the effect 
    D3DXHANDLE              m_hSpeed;                       // Handle to var in the effect 
    D3DXHANDLE              m_hTime;                        // Handle to var in the effect 
    D3DXHANDLE              m_hIterFac;                     // Handle to var in the effect 
    D3DXHANDLE              m_hWorld;                       // Handle to var in the effect 
    D3DXHANDLE              m_hMeshTexture;                 // Handle to var in the effect 
    D3DXHANDLE              m_hWorldViewProjection;         // Handle to var in the effect 
    D3DXHANDLE              m_hTechniqueRenderScene;        // Handle to technique in the effect 
	D3DXHANDLE              m_hShift;						// Handle to technique in the effect 
	D3DXHANDLE              m_hJulia;						// Handle to technique in the effect 
	D3DXHANDLE              m_hSeedRange;					// Handle to technique in the effect 
	
protected:
	
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT FinalCleanup();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
                           D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );

  
	HRESULT RenderText();
	HRESULT RenderFunction();
	
	HRESULT compileShader();
	
	void DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV);
public:
    CMyD3DApplication();
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};



// Helper class for D3DXCreateEffect() to be able to load include files
class CIncludeManager : public ID3DXInclude
{
protected: 

public: 
    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    STDMETHOD(Close)(LPCVOID pData);
};


HRESULT CIncludeManager::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{	

  int length;
  char * buffer;

  ifstream is;
  if(*pName=='&'){
	  switch(formulaID){
		  case 1:
			pName="Function_2.fx";	
		  break;
		  case 2:
			pName="Function_3.fx";	
		  break;
		  case 3:
			pName="Function_4.fx";	
		  break;
		  case 4:
			pName="Function_5.fx";	
		  break;
		  case 5:
			pName="Function_6.fx";	
		  break;
		  case 6:
			pName="Function_7.fx";	
		  break;
		  case 7:
			pName="Function_8.fx";	
		  break;
		  case 8:
			pName="Function_9.fx";	
		  break;
		  default:
			pName="Function_1.fx";
	  }
  }
  is.open (pName, ios::binary );

 // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);

  // allocate memory:
  buffer = new char [length];
	
  // read data as a block:
  is.read (buffer,length);

  is.close();

  //cout.write (buffer,length);
  
	int i;
  for(i = 0 ; i<length;i++){
  
	  if((buffer[i]=='#')&&(buffer[i+1]=='F')){
		break;				
	  }
  }
	//       #FUNCTION
  string fn="m_c_sin   ";
  if(i!=length)
  for(int k=0;k<9;k++){
	buffer[i+k]=fn[k];
  }

  *ppData = buffer;
  *pBytes = length;

  return S_OK;
}


HRESULT CIncludeManager::Close(LPCVOID pData)
{
    BYTE* pData2 = (BYTE*)pData;
    SAFE_DELETE_ARRAY( pData2 );
   	
	return S_OK;

}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    InitCommonControls();

    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_strWindowTitle = _T("Complex Formula");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_dwCreationWidth  = 640;
    m_dwCreationHeight = 480;
    m_pFont = NULL;
    m_bShowHelp = TRUE;
     m_pEffect	= NULL;

    m_hWorld = NULL;
    m_hMeshTexture = NULL;
    m_hWorldViewProjection = NULL;
    m_hTechniqueRenderScene = NULL;

	// Maximum Number of Iterations to be used
	maxIter=25;
   
	//formulaID=0;
	gSeedRange=0;
	gFact=1;
	gSpeed=0; 
	gShift.x=0;
	gShift.y=0;
	gtFormat=D3DFMT_A16B16G16R16F;
	gAnim=0;
	tex_size=256;
	gJulia=false;
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;

    // Initialize the font
    HDC hDC = GetDC( NULL );
    int nHeight = -MulDiv( 9, GetDeviceCaps(hDC, LOGPIXELSY), 72 );
    ReleaseDC( NULL, hDC );
    if( FAILED( hr = D3DXCreateFont( m_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         TEXT("Arial"), &m_pFont ) ) )
        return DXTRACE_ERR( TEXT("D3DXCreateFont"), hr );

  
   if( FAILED( hr =  compileShader() ) )
        return DXTRACE_ERR( TEXT("CompileShader"), hr );
	

    // Setup the camera's projection parameters
    float fAspectRatio = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
    m_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 1000.0f );
    m_Camera.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
    m_Camera.SetViewParams( &vecEye, &vecAt );



    return S_OK;
}



HRESULT CMyD3DApplication::compileShader(){
	
    HRESULT hr;
	SAFE_RELEASE(m_pEffect);
	CIncludeManager Includer;
 	// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the shader debugger.  
    // Debugging vertex shaders requires either REF or software vertex processing, and debugging 
    // pixel shaders requires REF.  The D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug 
    // experience in the shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile against the next 
    // higher available software target, which ensures that the unoptimized shaders do not exceed 
    // the shader model limitations.  Setting these flags will cause slower rendering since the shaders 
    // will be unoptimized and forced into software.  See the DirectX documentation for more information 
    // about using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif
  
	char* filename="HLSL_Function.fx";

	// Read the D3DX effect file
    TCHAR str[MAX_PATH];
		hr = DXUtil_FindMediaFileCb( str, sizeof(str), TEXT("HLSL_Function.fx") );
	 
	if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DXUtil_FindMediaFileCb"), D3DAPPERR_MEDIANOTFOUND );
    hr = D3DXCreateEffectFromFile(m_pd3dDevice, str, NULL, &Includer, dwShaderFlags, NULL, &m_pEffect, NULL );
  //  hr = D3DXCreateEffectFromResource(m_pd3dDevice, NULL, MAKEINTRESOURCE(buffer),NULL,&Includer, dwShaderFlags, NULL, &m_pEffect, NULL );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    if( FAILED( hr ) )
        return DXTRACE_ERR( TEXT("D3DXCreateEffectFromFile"), hr );
    return S_OK;
	
}


 


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    HRESULT hr; 


		// Create a RENDER Target Texture
	    hr =m_pd3dDevice->CreateTexture( tex_size, tex_size, 0,
        D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRenderTargetTexture, NULL );
	
		if( FAILED(hr) )
			return DXTRACE_ERR( TEXT("D3DXCreateTexture"), hr );
	    
	
		// The next 2 textures are used as in-between texture to save results
		hr =m_pd3dDevice->CreateTexture( tex_size, tex_size, 0,
        D3DUSAGE_RENDERTARGET, gtFormat, D3DPOOL_DEFAULT, &m_pRenderTargetTexture1, NULL );
		if( FAILED(hr) )
			return DXTRACE_ERR( TEXT("D3DXCreateTexture"), hr );

	    hr =m_pd3dDevice->CreateTexture( tex_size, tex_size, 0,
        D3DUSAGE_RENDERTARGET, gtFormat, D3DPOOL_DEFAULT, &m_pRenderTargetTexture2, NULL );
		if( FAILED(hr) )
		    return DXTRACE_ERR( TEXT("D3DXCreateTexture"), hr );
		//

		if( m_pFont )
			m_pFont->OnResetDevice();
		if( m_pEffect )
			m_pEffect->OnResetDevice();

  
		// To read or write to D3DX effect variables we can use the string name 
		// instead of using handles, however it improves perf to use handles since then 
		// D3DX won't have to spend time doing string compares
		m_hTechniqueRenderScene         = m_pEffect->GetTechniqueByName( "RenderScene" );
		m_hShift				        = m_pEffect->GetParameterByName(NULL, "shift" );
		m_hTime                         = m_pEffect->GetParameterByName( NULL, "g_fTime" );
 
		m_hFact                          = m_pEffect->GetParameterByName( NULL, "g_fact" );
		m_hIterFac                          = m_pEffect->GetParameterByName( NULL, "iterFac" );
 		m_hSpeed                         = m_pEffect->GetParameterByName( NULL, "g_speed" );
 		m_hSeedRange                     = m_pEffect->GetParameterByName( NULL, "g_srange" );
 		m_hJulia                         = m_pEffect->GetParameterByName( NULL, "gJulia" );
 
		 
		m_hWorld                        = m_pEffect->GetParameterByName( NULL, "g_mWorld" );
		m_hWorldViewProjection          = m_pEffect->GetParameterByName( NULL, "g_mWorldViewProjection" );
		m_hMeshTexture                  = m_pEffect->GetParameterByName( NULL, "g_RenderTexture" );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // Update the camera's postion based on user input 
    m_Camera.FrameMove( m_fElapsedTime );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;
    UINT iPass, cPasses;
    

	// Render the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// Render the active Function 
		RenderFunction();

        // Clear the render target and the zbuffer 
		// m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00003F3F, 1.0f, 0);
		  

        // Update the effect's variables 
        m_pEffect->SetFloat( m_hTime, m_fTime ); 
        m_pEffect->SetFloat( m_hFact, gFact ); 
        m_pEffect->SetFloat( m_hSpeed, gSpeed ); 
        m_pEffect->SetTexture( m_hMeshTexture, m_pRenderTargetTexture );
	    		  
		// Set Technique to default technique
		m_pEffect->SetTechnique(m_pEffect->GetTechnique(0));
	 
		
		// Render all passes of technique ( 0 ) 

        hr = m_pEffect->Begin(&cPasses, 0);
        if( FAILED( hr ) )
            return DXTRACE_ERR( TEXT("Begin"), hr );

        for (iPass = 0; iPass < cPasses; iPass++)
        {
            hr = m_pEffect->Pass(iPass);
            if( FAILED( hr ) )
                return DXTRACE_ERR( TEXT("Pass"), hr );
 		
 				 DrawFullScreenQuad(0,0,1,1);
			 
	    }
    
		m_pEffect->End();

        hr = RenderText();
        if( FAILED( hr ) )
            return DXTRACE_ERR( TEXT("RenderText"), hr );

        m_pd3dDevice->EndScene();
    }
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: RenderFunction()
// Desc: Called once per frame, this function renders the given formula
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderFunction()
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;
   

	// Get the new render target surface
    PDIRECT3DSURFACE9 pRenderTargetSurf = NULL; // final texture surface
    PDIRECT3DSURFACE9 pRenderTargetSurf1 = NULL; // temporary memory 
    PDIRECT3DSURFACE9 pRenderTargetSurf2 = NULL;
    PDIRECT3DSURFACE9 pSaveTarget = NULL;
    
	// Obtain the surfaces 
	hr = m_pRenderTargetTexture->GetSurfaceLevel( 0, &pRenderTargetSurf );
    hr = m_pRenderTargetTexture1->GetSurfaceLevel( 0, &pRenderTargetSurf1 );
    hr = m_pRenderTargetTexture2->GetSurfaceLevel( 0, &pRenderTargetSurf2 );
    
	
    hr=m_pd3dDevice->GetRenderTarget(0, &pSaveTarget);
	  if( FAILED( hr ) )
            return DXTRACE_ERR( TEXT("GetRenderTarget"), hr );
      
        // Set Effect Variables
		m_pEffect->SetFloat( m_hFact, gFact ); 
        m_pEffect->SetFloat( m_hSpeed, gAnim); 
		
		gAnim+=gSpeed ;
        
		m_pEffect->SetBool( m_hJulia, gJulia); 
		m_pEffect->SetFloat(m_hIterFac,1.0f/(float)maxIter);
        m_pEffect->SetValue( m_hShift, &gShift,sizeof(gShift) ); 
        m_pEffect->SetValue( m_hSeedRange, &gSeedRange,sizeof(gSeedRange) ); 
     
		// Use technique 1 for rendering the FIRST iteration ( initialization ! ) 
		m_pEffect->SetTechnique(m_pEffect->GetTechnique(1));
	
        hr = m_pEffect->Begin(NULL, 0);

		// set render target to temporary buffer
		hr=m_pd3dDevice->SetRenderTarget( 0,pRenderTargetSurf1  );
		
        if( FAILED( hr ) )
            return DXTRACE_ERR( TEXT("Begin"), hr );

        hr = m_pEffect->Pass(0);
        if( FAILED( hr ) )
           return DXTRACE_ERR( TEXT("Pass"), hr );
 				 
		// Draw a full screen quad to initalize the temporary memory
		DrawFullScreenQuad(0,0,1,1);
			
		 
		// Now Follows the iterative part
		// It is called in a loop always the same pass(1) is used
		int i;
		
        hr = m_pEffect->Pass(1);
		for(i = 0 ;i<=maxIter;i++)
		{
			// Alternate the in-between textures
			// and render targets
	 		if(i%2 == 0)
			{
				m_pEffect->SetTexture( m_hMeshTexture, m_pRenderTargetTexture1);
				m_pd3dDevice->SetRenderTarget( 0,pRenderTargetSurf2  );
			}
			else
			{
				m_pEffect->SetTexture( m_hMeshTexture, m_pRenderTargetTexture2);
				m_pd3dDevice->SetRenderTarget( 0,pRenderTargetSurf1  );
			};

			// Always draw a fullscreenquad
 			DrawFullScreenQuad(0,0,1,1);
		
		}

		////////// The Final Step
	
		// Set the right source texture
 		if(i%2 == 0){
			m_pEffect->SetTexture( m_hMeshTexture, m_pRenderTargetTexture1);
		}else{
			m_pEffect->SetTexture( m_hMeshTexture, m_pRenderTargetTexture2);
		};

		// Set render target to the final texture
		m_pd3dDevice->SetRenderTarget( 0,pRenderTargetSurf  );

		// and initialize 2nd ( final ) pass
		hr = m_pEffect->Pass(2);
            if( FAILED( hr ) )
                return DXTRACE_ERR( TEXT("Pass"), hr );

		// the last draw of the fullscreenquad returns a colorized image
		DrawFullScreenQuad(0,0,1,1);
		

        m_pEffect->End();


	// Restore original Rendertarget
	m_pd3dDevice->SetRenderTarget( 0,pSaveTarget );

	// and free up the used memory
	SAFE_RELEASE(pRenderTargetSurf);
	SAFE_RELEASE(pRenderTargetSurf1);
	SAFE_RELEASE(pRenderTargetSurf2);
	SAFE_RELEASE(pSaveTarget); 

	// everything went ok
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void CMyD3DApplication::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV)
{
    D3DSURFACE_DESC dtdsdRT;
    PDIRECT3DSURFACE9 pSurfRT;

    // Acquire render target width and height
    m_pd3dDevice->GetRenderTarget(0, &pSurfRT);
    pSurfRT->GetDesc(&dtdsdRT);
    pSurfRT->Release();

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
    FLOAT fWidth5 = (FLOAT)dtdsdRT.Width - 0.5f;
    FLOAT fHeight5 = (FLOAT)dtdsdRT.Height - 0.5f;

    // Draw the quad
    ScreenVertex svQuad[4];

    svQuad[0].p = D3DXVECTOR4(-0.5f, -0.5f, 0.5f, 1.0f);
    svQuad[0].t = D3DXVECTOR2(fLeftU, fTopV);

    svQuad[1].p = D3DXVECTOR4(fWidth5, -0.5f, 0.5f, 1.0f);
    svQuad[1].t = D3DXVECTOR2(fRightU, fTopV);

    svQuad[2].p = D3DXVECTOR4(-0.5f, fHeight5, 0.5f, 1.0f);
    svQuad[2].t = D3DXVECTOR2(fLeftU, fBottomV);

    svQuad[3].p = D3DXVECTOR4(fWidth5, fHeight5, 0.5f, 1.0f);
    svQuad[3].t = D3DXVECTOR2(fRightU, fBottomV);

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pd3dDevice->SetFVF(ScreenVertex::FVF);
    m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex));
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}
//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Draw the help & statistics for running sample
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    HRESULT hr;
    RECT rc;

    if( NULL == m_pFont )
        return S_OK;

    // Output statistics
    int iLineY = 0;
    SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
    hr = m_pFont->DrawText( NULL, m_strFrameStats, -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ));
    if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DrawText"), hr );
    SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
    hr = m_pFont->DrawText( NULL, m_strDeviceStats, -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DrawText"), hr );

    TCHAR sz[100];

    // Draw help
    if( m_bShowHelp )
    {
        iLineY = m_d3dsdBackBuffer.Height-15*11;
        SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
        hr = m_pFont->DrawText( NULL, _T("Controls:"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );

        SetRect( &rc, 20, iLineY, 0, 0 ); 
        hr = m_pFont->DrawText( NULL, _T("T Texture Size       J : Julia/Mandel      F: Floating Point 16/32 Bit\n")
                                      _T("NumPad 7/4/1 : IncSpeed/Speed=0/DecSpeed  \n")
                                      _T("NumPad 8/5/2 : IncSeedRange/SeedRange=0/DecSeedRange\n")
                                      _T("NumPad 9/6/3 : IncIteration/Iteration=25/DecIteration\n")                                      
                                      _T("Cursor keys: Shift View\n")                                      
                                      _T("+/-		 : Zoom  View\n"),                                      
                                      -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );

	    if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );	
		iLineY += 15*6;
        SetRect( &rc, 20, iLineY, 0, 0 );
        hr = m_pFont->DrawText( NULL, _T("Change Device: F2\n")
                                      _T("View readme: F9\n")
                                      _T("Quit: ESC"),
                                      -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
    }
    else
    {
        SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
        hr = m_pFont->DrawText( NULL, _T("Press F1 to help"), 
                                -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    if( m_pFont )
        m_pFont->OnLostDevice();
    if( m_pEffect )
        m_pEffect->OnLostDevice();
 

	SAFE_RELEASE(m_pRenderTargetTexture);
	SAFE_RELEASE(m_pRenderTargetTexture1);
	SAFE_RELEASE(m_pRenderTargetTexture2);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    SAFE_RELEASE(m_pEffect);
    SAFE_RELEASE(m_pFont); 
	SAFE_RELEASE(m_pRenderTargetTexture);
	SAFE_RELEASE(m_pRenderTargetTexture1);
	SAFE_RELEASE(m_pRenderTargetTexture2);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
    #ifdef DEBUG_VS
        if( pCaps->DeviceType != D3DDEVTYPE_REF && 
            (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0 )
            return E_FAIL;
    #endif
    #ifdef DEBUG_PS
        if( pCaps->DeviceType != D3DDEVTYPE_REF )
            return E_FAIL;
    #endif

    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL;

    // No fallback, so need ps2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
        return E_FAIL;

    // If device doesn't support 1.1 vertex shaders in HW, switch to SWVP.
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        if( (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) == 0 )
        {
            return E_FAIL;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   // m_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    switch( uMsg )
    {
        case WM_COMMAND:
        {
            switch( LOWORD(wParam) )
            {
                case IDM_TOGGLEHELP:
                    m_bShowHelp = !m_bShowHelp;
                break;
                case IDM_INC:
                   gFact*=1.1f; 
                break;
			    case IDM_DEC:
                    if(gFact>0.001)gFact*=(float)0.9;
                break;
				case IDM_TOGGLE_JULIA:
					gJulia=!gJulia;
				break;
			 
				case IDM_PREVOBJECT:	
					formulaID=abs(formulaID-1)%10;
					Cleanup3DEnvironment();
					Initialize3DEnvironment();
				break;	
				case IDM_NEXTOBJECT:
				 	formulaID=abs(formulaID+1)%10;				
					Cleanup3DEnvironment();
					Initialize3DEnvironment();
				break;	
				case IDM_TOGGLE_FLOATFORMAT:
					if(gtFormat==D3DFMT_A16B16G16R16F){
						gtFormat=D3DFMT_A32B32G32R32F;
					}else{
						gtFormat=D3DFMT_A16B16G16R16F;
					}
			        Cleanup3DEnvironment();
					Initialize3DEnvironment();
				break;
			 	case IDM_INC_SPEED:
							gSpeed+=(float) 0.001f;
					break;
				case IDM_DEC_SPEED:
					if(gSpeed>0){
							gSpeed-=(float) 0.001f;
					}
					break;
				case IDM_RESET_SPEED:
						gSpeed=0.0f;
					break;	 	
				case IDM_INC_SEED_RANGE:
							gSeedRange+=(float) 0.01f;
					break;
				case IDM_DEC_SEED_RANGE:
				 
							gSeedRange-=(float) 0.01f;
				 
					break;
				case IDM_RESET_SEED_RANGE:
						gSeedRange=0.0f;
					break;	
				case IDM_INC_ITER:
							maxIter+=1;
					break;
				case IDM_DEC_ITER:
				 
					if(maxIter>1){maxIter-=1;}				 
					break;
				case IDM_RESET_ITER:
						maxIter=20;
					break;
					case IDM_INC_SHIFTX: 
							gShift.y+=0.1*gFact; 
					break;
					case IDM_INC_SHIFTY:
							gShift.x-=0.1*gFact; 
					break;
					case IDM_DEC_SHIFTX:
							gShift.y-=0.1*gFact; 
					break;
					case IDM_DEC_SHIFTY:
							gShift.x+=0.1*gFact; 
					break;	
					case IDM_TEX_SIZE_CHANGE:
							tex_size*=2;
							if(tex_size>512)tex_size=16; 
							Cleanup3DEnvironment();
							Initialize3DEnvironment();
		
					break;			
            }
        }
    }

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}


