//-----------------------------------------------------------------------------
// File: HLSLDeform.cpp
//
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include <dxerr9.h>
#include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

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


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Main class to run this application. Most functionality is inherited
//       from the CD3DApplication base class.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
private:
	float					gFact;
	float					gTapeFact;
	float					gTwistFact;
	float					gSpherifyFact;
	float					gtCenter;	// Taper Center
	float					gtRange;	// Taper Range


	float					gSpeed;
	int						currObject; // The Object ID 0= Mesh, 1=Box, 2=Tube


    ID3DXFont*              m_pFont;              // Font for drawing text
    bool                    m_bShowHelp;          // If true, it renders the UI control text
    bool                    m_bShowWire;          // If true, it renders Wireframe
	int						m_bEffect;				
	UINT m_n;                          // Number of vertices in the wall mesh along X
    UINT m_m;                          // Number of vertices in the wall mesh along Z
    UINT m_nTriangles;                 // Number of triangles in the wall mesh
 
	CModelViewerCamera      m_Camera;             // A model viewing camera
    LPD3DXEFFECT            m_pEffect;            // D3DX effect interface

	LPD3DXMESH				m_pBoxMesh;            // Tessellated plane to serve as the walls and floor
 	LPD3DXMESH				m_pTubeMesh;            // Tessellated Tube Object
	LPD3DXMESH              m_pTeapot;              // Mesh object
 
    LPD3DXMESH              m_pMesh;              // Mesh object
    LPDIRECT3DTEXTURE9      m_pMeshTexture;       // Mesh texture

    D3DXHANDLE              m_hTapeFact;                        // Handle to var in the effect 
    D3DXHANDLE              m_hTwistFact;                        // Handle to var in the effect 
    D3DXHANDLE              m_hSpherifyFact;                        // Handle to var in the effect 
    D3DXHANDLE              m_hSpeed;                       // Handle to var in the effect 
	D3DXHANDLE              m_hTRange;                        // Handle to var in the effect 
    D3DXHANDLE              m_hTCenter;                       // Handle to var in the effect 

    D3DXHANDLE              m_hTime;                        // Handle to var in the effect 
    D3DXHANDLE              m_hWorld;                       // Handle to var in the effect 
    D3DXHANDLE              m_hMeshTexture;                 // Handle to var in the effect 
    D3DXHANDLE              m_hWorldViewProjection;         // Handle to var in the effect 
    D3DXHANDLE              m_hTechniqueRenderScene;        // Handle to technique in the effect 

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

    HRESULT LoadMesh( TCHAR* strFileName, LPD3DXMESH* ppMesh );
    HRESULT RenderText();
	HRESULT BuildTeapot( LPD3DXMESH* ppMesh );

    HRESULT BuildBoxMesh();
    HRESULT BuildTubeMesh();
public:
    CMyD3DApplication();
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
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
    m_strWindowTitle = _T("HLSL Deform");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_dwCreationWidth  = 640;
    m_dwCreationHeight = 480;
    m_pFont = NULL;
    m_bShowHelp = TRUE;
    m_bShowWire = FALSE;
    m_pEffect	= NULL;
	m_bEffect	= 0;

    m_hWorld = NULL;
    m_hMeshTexture = NULL;
    m_hWorldViewProjection = NULL;
    m_hTechniqueRenderScene = NULL;
 
    m_pMesh = NULL;
    m_pMeshTexture = NULL;
	
	m_pBoxMesh                 = NULL;
   
	// Set up mesh resolution.  Try changing m_n and m_m to see
    // how the lighting is affected.
    m_n = 16; 
    m_m = 16;
    m_nTriangles = (m_n-1)*(m_m-1)*2;
 
	gtCenter=0;		// Taper Center
	gtRange=0.5;	// Taper Range
	gFact=0;
	
	// The Following values save the current values for the different 
	// deformation functions
	gTapeFact=0;
	gTwistFact=0;
	gSpherifyFact=0;

	gSpeed=1;

	// currObject is the mesh to be rendered
	currObject=0;
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

    // Load the mesh
    if( FAILED( hr = LoadMesh( TEXT("tiger.x"), &m_pMesh ) ) )
        return DXTRACE_ERR( TEXT("LoadMesh"), hr );

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

    // Read the D3DX effect file
    TCHAR str[MAX_PATH];
	switch(m_bEffect){
		case 0:
		hr = DXUtil_FindMediaFileCb( str, sizeof(str), TEXT("HLSL_Taper.fx") );
		break;
		case 1:
		hr = DXUtil_FindMediaFileCb( str, sizeof(str), TEXT("HLSL_Twist.fx") );
		break;	
		case 2:
		hr = DXUtil_FindMediaFileCb( str, sizeof(str), TEXT("HLSL_Spherify.fx") );
		break;
		case 3:
		hr = DXUtil_FindMediaFileCb( str, sizeof(str), TEXT("HLSL_Taper_Twist_Spherify.fx") );
		break;
    }

	if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DXUtil_FindMediaFileCb"), D3DAPPERR_MEDIANOTFOUND );
    hr = D3DXCreateEffectFromFile(m_pd3dDevice, str, NULL, NULL, dwShaderFlags, NULL, &m_pEffect, NULL );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    if( FAILED( hr ) )
        return DXTRACE_ERR( TEXT("D3DXCreateEffectFromFile"), hr );

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







//-----------------------------------------------------------------------------
// Name: BuildWallMesh()
// Desc: Builds the wall mesh based on m_m and m_n.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::BuildBoxMesh()
{
    HRESULT hr = S_OK;
    MYVERTEX* v = NULL;
    LPD3DXMESH pBoxMeshTemp = NULL;
    DWORD* pdwAdjacency = NULL;

    SAFE_RELEASE( m_pBoxMesh );

    int numSides=6;
	float width=1.0f;
	float width2=width/2.0f;
	// Create square grids m_n*m_m for rendering a Box
    if( FAILED( hr = D3DXCreateMeshFVF( m_nTriangles*numSides, m_nTriangles * 3*numSides, 
        D3DXMESH_SYSTEMMEM, MYVERTEX::FVF, m_pd3dDevice, &pBoxMeshTemp ) ) )
    {
        goto End;
    }

    // Fill in the grid vertex data
    float dX;
    float dZ;
    UINT k;
    int z;
    if( FAILED( hr = pBoxMeshTemp->LockVertexBuffer(0, (LPVOID*)&v) ) )
        goto End;
    dX = 1.0f/(m_n-1);
    dZ = 1.0f/(m_m-1);
    k = 0;
	// First Face TOP
    for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {
        	v[k].p  = D3DXVECTOR3(width * x*dX-width2, width2, width * z*dZ -width2);
            v[k].n  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            k++;
           	v[k].p  = D3DXVECTOR3(width * x*dX-width2, width2, width * (z+1)*dZ-width2 );
            v[k].n  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            k++;
			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width2, width * (z+1)*dZ-width2 );
            v[k].n  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            k++;
		
       
        	
			v[k].p  = D3DXVECTOR3(width * x*dX-width2, width2, width * z*dZ-width2 );
            v[k].n  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            k++;
          
            v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2,width2, width * (z+1)*dZ-width2 );
            v[k].n  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            k++;
		    v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width2, width * z*dZ -width2);
            v[k].n  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            k++;
        }
    }
	// Second Face BOTTOM
   for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {
            v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, -width2, width * (z+1)*dZ-width2 );
            k++;
            v[k].p  = D3DXVECTOR3(width * x*dX-width2, -width2, width * (z+1)*dZ-width2 );
            k++;
            v[k].p  = D3DXVECTOR3(width * x*dX-width2, -width2, width * z*dZ -width2);
            k++;
        
			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, -width2, width * z*dZ -width2);
            k++;
            v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2,-width2, width * (z+1)*dZ-width2 );
            k++;
			v[k].p  = D3DXVECTOR3(width * x*dX-width2, -width2, width * z*dZ-width2 );
            v[k].n  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            k++;
        }
    }
 
	// Third Face Left
   for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {
          v[k].p  = D3DXVECTOR3(-width2,width * x*dX-width2,  width * z*dZ -width2);
          k++;
		  v[k].p  = D3DXVECTOR3(-width2,width * x*dX-width2, width * (z+1)*dZ-width2 );
          k++;
          v[k].p  = D3DXVECTOR3(-width2,width * (x+1)*dX-width2, width * (z+1)*dZ-width2 );
          k++;
          
		  v[k].p  = D3DXVECTOR3(-width2,width * x*dX-width2, width * z*dZ-width2 );
          k++;
       	
		  v[k].p  = D3DXVECTOR3(-width2,width * (x+1)*dX-width2, width * (z+1)*dZ-width2 );
          k++;
	 	  v[k].p  = D3DXVECTOR3(-width2,width * (x+1)*dX-width2, width * z*dZ -width2);
          k++;
		    
        }
    }
   // Fourth Face Right
   for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {  
		   v[k].p  = D3DXVECTOR3(width2,width * (x+1)*dX-width2, width * (z+1)*dZ-width2 );
           k++;
           v[k].p  = D3DXVECTOR3(width2,width * x*dX-width2, width * (z+1)*dZ-width2 );
           k++;
		   v[k].p  = D3DXVECTOR3(width2,width * x*dX-width2,  width * z*dZ -width2);
           k++;
		   v[k].p  = D3DXVECTOR3(width2,width * (x+1)*dX-width2, width * (z+1)*dZ-width2 );
           k++;
           v[k].p  = D3DXVECTOR3(width2,width * x*dX-width2, width * z*dZ-width2 );
           k++;
           v[k].p  = D3DXVECTOR3(width2,width * (x+1)*dX-width2, width * z*dZ -width2);
           k++;
		
        }
    }

	// Fifth Face Front
    for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {
            v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width * (z+1)*dZ-width2,width2 );
			k++;
            v[k].p  = D3DXVECTOR3(width * x*dX-width2, width * (z+1)*dZ-width2,width2 );
			k++;
            v[k].p  = D3DXVECTOR3(width * x*dX-width2,  width * z*dZ -width2,width2);
            k++;
      
			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width * z*dZ -width2,width2);
			k++;
			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width * (z+1)*dZ-width2,width2 );
			k++;
      		v[k].p  = D3DXVECTOR3(width * x*dX-width2, width * z*dZ-width2 ,width2);
			k++;
          }
    }
	// Sixth Face Back
	for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {
            v[k].p  = D3DXVECTOR3(width * x*dX-width2,  width * z*dZ -width2,-width2);
			k++;
			v[k].p  = D3DXVECTOR3(width * x*dX-width2, width * (z+1)*dZ-width2,-width2 );
			k++;       
			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width * (z+1)*dZ-width2,-width2 );
            k++;
       
           
         	v[k].p  = D3DXVECTOR3(width * x*dX-width2, width * z*dZ-width2 ,-width2);
			k++;  
 			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width * (z+1)*dZ-width2,-width2 );
			k++;	
			v[k].p  = D3DXVECTOR3(width * (x+1)*dX-width2, width * z*dZ -width2,-width2);
	        k++;
          }
    }

    pBoxMeshTemp->UnlockVertexBuffer();

    // Fill in index data
    WORD* pIndex;
    if( FAILED( hr = pBoxMeshTemp->LockIndexBuffer(0, (LPVOID*)&pIndex) ) )
        goto End;
    WORD iIndex;
    for( iIndex = 0; iIndex < m_nTriangles * 3*numSides; iIndex++ )
    {
        *(pIndex++) = iIndex;
    }
    pBoxMeshTemp->UnlockIndexBuffer();

   	/*
	// Eliminate redundant vertices
    pdwAdjacency = new DWORD[3 * m_nTriangles*numSides];
    if( pdwAdjacency == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto End;
    }

	if( FAILED( hr = D3DXWeldVertices( pBoxMeshTemp, D3DXWELDEPSILONS_WELDALL, NULL, NULL, pdwAdjacency, NULL, NULL ) ) )
    {
        goto End;
    }


    // Optimize the mesh
    if( FAILED( hr = pBoxMeshTemp->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
        pdwAdjacency, NULL, NULL, NULL ) ) )
    {
        goto End;
    }
	*/
	D3DXComputeNormals( pBoxMeshTemp, NULL );
	// Copy the mesh into fast write-only memory
    if( FAILED( hr = pBoxMeshTemp->CloneMeshFVF( D3DXMESH_WRITEONLY, MYVERTEX::FVF, m_pd3dDevice, &m_pBoxMesh ) ) )
    {
        goto End;
    }

End:
    SAFE_RELEASE( pBoxMeshTemp );
    SAFE_DELETE_ARRAY( pdwAdjacency );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: getCirclePos()
// Desc: gets the x/y/z coordinate of a vertex on a tube of radíus 
//-----------------------------------------------------------------------------

D3DXVECTOR3 getCirclePos(float x,float y,float height,float radius){
D3DXVECTOR3 temp;

	temp.x = (float)(sin(x*PI*2)-cos(x*PI*2)) * radius;
	temp.z = y*height-height/2;
	temp.y = (float)(cos(x*PI*2)+sin(x*PI*2)) * radius;

	return temp;

}
//-----------------------------------------------------------------------------
// Name: getCirclePos()
// Desc: gets the normal on a tube at given position 
//-----------------------------------------------------------------------------

D3DXVECTOR3 getCircleNormal(float x,float y){
D3DXVECTOR3 temp;

	temp.x = (float)(sin(x*PI*2)-cos(x*PI*2)) ;
	temp.z = 0;
	temp.y = (float)(cos(x*PI*2)+sin(x*PI*2)) ;

	return temp;

}
//-----------------------------------------------------------------------------
// Name: BuildTubeMesh()
// Desc: Builds a tube Mesh
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::BuildTubeMesh()
{
    HRESULT hr = S_OK;
    MYVERTEX* v = NULL;
    LPD3DXMESH pTubeMeshTemp = NULL;
    DWORD* pdwAdjacency = NULL;

   // SAFE_RELEASE( m_pTubeMesh );

    int numSides=1;
	float width=1.0f;
	float width2=width/2.0f;
	// Create a square grid m_n*m_m for rendering the wall
    if( FAILED( hr = D3DXCreateMeshFVF( m_nTriangles*numSides, m_nTriangles * 3*numSides, 
        D3DXMESH_SYSTEMMEM, MYVERTEX::FVF, m_pd3dDevice, &pTubeMeshTemp ) ) )
    {
        goto End;
    }

    // Fill in the grid vertex data
    float dX;
    float dZ;
    UINT k;
    int z;
    if( FAILED( hr = pTubeMeshTemp->LockVertexBuffer(0, (LPVOID*)&v) ) )
        goto End;
    dX = 1.0f/(m_n-1);
    dZ = 1.0f/(m_m-1);
    k = 0; 

	// Construct the vertex data for Tube
    for (z=0; z < (int)(m_m)-1; z++)
    {
        for (int x=0; x <(int) (m_n)-1; x++)
        {
            v[k].p  = getCirclePos(x*dX,z*dZ,width,(float)0.2);

            k++;

            v[k].p  = getCirclePos(x*dX,(z+1)*dZ,width,(float)0.2);

		    k++;
			
			v[k].p  = getCirclePos((x+1)*dX,(z+1)*dZ,width,(float)0.2);

            k++;
			
			v[k].p  = getCirclePos((x)*dX,z*dZ,width,(float)0.2);

            k++;
            
			v[k].p  = getCirclePos((x+1)*dX,(z+1)*dZ,width,(float)0.2);

            k++;
            
			v[k].p  = getCirclePos((x+1)*dX,(z)*dZ,width,(float)0.2);

            k++;
        }
    } 
    pTubeMeshTemp->UnlockVertexBuffer();

     // Fill in index data
    WORD* pIndex;
    if( FAILED( hr = pTubeMeshTemp->LockIndexBuffer(0, (LPVOID*)&pIndex) ) )
        goto End;
    WORD iIndex;
    for( iIndex = 0; iIndex < m_nTriangles * 3*numSides; iIndex++ )
    {
        *(pIndex++) = iIndex;
    }
    pTubeMeshTemp->UnlockIndexBuffer();

    // Eliminate redundant vertices
    pdwAdjacency = new DWORD[3 * m_nTriangles*numSides];
    if( pdwAdjacency == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto End;
    }
 
    if( FAILED( hr = D3DXWeldVertices( pTubeMeshTemp, D3DXWELDEPSILONS_WELDALL, NULL, NULL, pdwAdjacency, NULL, NULL ) ) )
    {
        goto End;
    }

    // Optimize the mesh
    if( FAILED( hr = pTubeMeshTemp->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
        pdwAdjacency, NULL, NULL, NULL ) ) )
    {
        goto End;
    }
    
	D3DXComputeNormals( pTubeMeshTemp, NULL );
  
	// Copy the mesh into fast write-only memory
    if( FAILED( hr = pTubeMeshTemp->CloneMeshFVF( D3DXMESH_WRITEONLY, MYVERTEX::FVF, m_pd3dDevice, &m_pTubeMesh ) ) )
    {
        goto End;
    }

End:
    SAFE_RELEASE( pTubeMeshTemp );
    SAFE_DELETE_ARRAY( pdwAdjacency );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: LoadMesh()
// Desc: Load the mesh and ensure the mesh has normals, and optimize
//       the mesh for this graphics card's vertex cache so the triangle list
//       inside in the mesh will cache miss less which improves perf. 
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::LoadMesh( TCHAR* strFileName, LPD3DXMESH* ppMesh )
{
    LPD3DXMESH pMesh = NULL;
    TCHAR str[MAX_PATH];
    HRESULT hr;

    if( ppMesh == NULL )
        return E_INVALIDARG;

    // Load the mesh with D3DX and get back a ID3DXMesh*.  For this
    // sample we'll ignore the X file's embedded materials since we know 
    // exactly the model we're loading.  See the mesh samples such as
    // "OptimizedMesh" for a more generic mesh loading example.
    hr = DXUtil_FindMediaFileCb( str, sizeof(str), strFileName );
    if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DXUtil_FindMediaFileCb"), D3DAPPERR_MEDIANOTFOUND );
    hr = D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, 
                           m_pd3dDevice, NULL, NULL, NULL, NULL, &pMesh);
    if( FAILED(hr) || (pMesh == NULL) )
        return DXTRACE_ERR( TEXT("D3DXLoadMeshFromX"), hr );

    DWORD *rgdwAdjacency = NULL;

    // Make sure there are normals which are required for lighting
    if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        LPD3DXMESH pTempMesh;
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
                                  pMesh->GetFVF() | D3DFVF_NORMAL, 
                                  m_pd3dDevice, &pTempMesh );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("CloneMeshFVF"), hr );

        D3DXComputeNormals( pTempMesh, NULL );

        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
    }

    // Optimize the mesh for this graphics card's vertex cache 
    // so when rendering the mesh's triangle list the vertices will 
    // cache hit more often so it won't have to re-execute the vertex shader 
    // on those vertices so it will improves perf.     
    rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency);
    pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL);
    delete []rgdwAdjacency;

    *ppMesh = pMesh;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: BuildTeapot()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::BuildTeapot( LPD3DXMESH* ppMesh )
{
    LPD3DXMESH pMesh = NULL;
	
    DWORD *rgdwAdjacency = NULL;
 
    HRESULT hr;

    if( ppMesh == NULL )
        return E_INVALIDARG;

  D3DXCreateTeapot(m_pd3dDevice,&pMesh,NULL);
    


    // Make sure there are normals which are required for lighting
    if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        LPD3DXMESH pTempMesh;
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
                                  pMesh->GetFVF() | D3DFVF_NORMAL, 
                                  m_pd3dDevice, &pTempMesh );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("CloneMeshFVF"), hr );

        D3DXComputeNormals( pTempMesh, NULL );

        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
    }

    // Optimize the mesh for this graphics card's vertex cache 
    // so when rendering the mesh's triangle list the vertices will 
    // cache hit more often so it won't have to re-execute the vertex shader 
    // on those vertices so it will improves perf.     
    rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency);
    pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL);
    delete []rgdwAdjacency;

    *ppMesh = pMesh;

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
    TCHAR str[MAX_PATH];

	hr=BuildBoxMesh();
	if(FAILED(hr)){  
		return DXTRACE_ERR( TEXT("BuildBox Error"), hr );

	}

	hr=BuildTubeMesh();
	if(FAILED(hr)){  
		return DXTRACE_ERR( TEXT("BuildTube Error"), hr );

	}

	hr=BuildTeapot(&m_pTeapot);
	if(FAILED(hr)){  
		return DXTRACE_ERR( TEXT("BuildTeapot Error"), hr );

	}


    if( m_pFont )
        m_pFont->OnResetDevice();
    if( m_pEffect )
        m_pEffect->OnResetDevice();

    // Create the mesh texture from a file
    hr = DXUtil_FindMediaFileCb( str, sizeof(str), TEXT("tiger.bmp") );
    if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DXUtil_FindMediaFileCb"), D3DAPPERR_MEDIANOTFOUND );
    hr =  D3DXCreateTextureFromFileEx( m_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
                                       D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
                                       D3DX_DEFAULT, D3DX_DEFAULT, 0, 
                                       NULL, NULL, &m_pMeshTexture );
    if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("D3DXCreateTextureFromFileEx"), hr );

    // Set effect variables as needed
    D3DXCOLOR colorMtrlDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
    D3DXCOLOR colorMtrlAmbient(0.35f, 0.35f, 0.35f, 0);
    m_pEffect->SetVector("g_MaterialAmbientColor", (D3DXVECTOR4*)&colorMtrlAmbient);
    m_pEffect->SetVector("g_MaterialDiffuseColor", (D3DXVECTOR4*)&colorMtrlDiffuse);

    // To read or write to D3DX effect variables we can use the string name 
    // instead of using handles, however it improves perf to use handles since then 
    // D3DX won't have to spend time doing string compares
    m_hTechniqueRenderScene         = m_pEffect->GetTechniqueByName( "RenderScene" );
    m_hTime                         = m_pEffect->GetParameterByName( NULL, "g_fTime" );
 
	m_hTwistFact                    = m_pEffect->GetParameterByName( NULL, "g_twist_fact" );
	m_hTapeFact                     = m_pEffect->GetParameterByName( NULL, "g_tape_fact" );
	m_hSpherifyFact                 = m_pEffect->GetParameterByName( NULL, "g_spherify_fact" );
 	m_hSpeed                        = m_pEffect->GetParameterByName( NULL, "g_speed" );
 
	m_hTRange                       = m_pEffect->GetParameterByName( NULL, "gtrange" );
 	m_hTCenter                      = m_pEffect->GetParameterByName( NULL, "gtcenter" );
 
	m_hWorld                        = m_pEffect->GetParameterByName( NULL, "g_mWorld" );
    m_hWorldViewProjection          = m_pEffect->GetParameterByName( NULL, "g_mWorldViewProjection" );
    m_hMeshTexture                  = m_pEffect->GetParameterByName( NULL, "g_MeshTexture" );

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
   
    // Clear the render target and the zbuffer 
    m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00003F3F, 1.0f, 0);


	// Render the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // Set world drawing technique

        // Get the projection & view matrix from the camera class
        mWorld= *m_Camera.GetWorldMatrix();       
        mProj = *m_Camera.GetProjMatrix();       
        mView = *m_Camera.GetViewMatrix();

        mWorldViewProjection = mWorld * mView * mProj;

        // Update the effect's variables 
        m_pEffect->SetMatrix( m_hWorldViewProjection, &mWorldViewProjection );
        m_pEffect->SetMatrix( m_hWorld, &mWorld );
        m_pEffect->SetFloat( m_hTime, m_fTime ); 
 
		m_pEffect->SetFloat( m_hTapeFact, gTapeFact ); 
		m_pEffect->SetFloat( m_hTwistFact, gTwistFact ); 
		m_pEffect->SetFloat( m_hSpherifyFact, gSpherifyFact ); 
        m_pEffect->SetFloat( m_hSpeed, gSpeed ); 
        m_pEffect->SetFloat( m_hTCenter, gtCenter ); 
        m_pEffect->SetFloat( m_hTRange, gtRange ); 
      
		  m_pEffect->SetTexture( m_hMeshTexture, m_pMeshTexture);

		if(m_bShowWire){
			m_pd3dDevice->SetRenderState(    D3DRS_FILLMODE ,D3DFILL_WIREFRAME );
			}else{
			m_pd3dDevice->SetRenderState(    D3DRS_FILLMODE ,D3DFILL_SOLID );
		}
		
		

        hr = m_pEffect->Begin(&cPasses, 0);
        if( FAILED( hr ) )
            return DXTRACE_ERR( TEXT("Begin"), hr );
        for (iPass = 0; iPass < cPasses; iPass++)
        {
            hr = m_pEffect->Pass(iPass);
            if( FAILED( hr ) )
                return DXTRACE_ERR( TEXT("Pass"), hr );
 
			switch(currObject){
			case 1:
						hr = m_pMesh->DrawSubset(0);
						break;
			case 2:
						hr = m_pTubeMesh->DrawSubset(0);
						break;			
			case 3:
						hr = m_pTeapot->DrawSubset(0);
						break;
			default: 
					hr =  m_pBoxMesh->DrawSubset(0);
		
					break;
            }
          if( FAILED( hr ) )
                return DXTRACE_ERR( TEXT("DrawSubset"), hr );
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

    if( FAILED(hr) )
        return DXTRACE_ERR( TEXT("DrawText"), hr );

    // Draw help
    if( m_bShowHelp )
    {
        iLineY = m_d3dsdBackBuffer.Height-15*13;
       
		SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("1 2 3 4 : Spherify/Tape/Twist/Twist+Tape "), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );

		SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("+ / - : Inc / Dec Effect Strength"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );

		SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("w : Switch Wireframe"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
	
		SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("PgUp/PgDown : Switch Object"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
       
	SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("KeyPad9/KeyPad3 : Move Tape Center"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
       
	SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("KeyPad9/KeyPad3 : Inc/Dec Tape Range"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
       
	SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("KeyPad7/KeyPad1 : Inc/Dec Twisting Factor"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
       
		
		SetRect( &rc, 2, iLineY, 0, 0 ); iLineY += 15;
       	hr = m_pFont->DrawText( NULL, _T("Controls:"), -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );
        SetRect( &rc, 20, iLineY, 0, 0 ); 
        hr = m_pFont->DrawText( NULL, _T("Rotate model: Left mouse button\n")
                                      _T("Rotate camera: Right mouse button\n")
                                      _T("Zoom camera: Mouse wheel scroll\n")
                                      _T("Hide help: F1\n"),                                      
                                      -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        if( FAILED(hr) )
            return DXTRACE_ERR( TEXT("DrawText"), hr );

        SetRect( &rc, 250, iLineY, 0, 0 );
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

    SAFE_RELEASE(m_pBoxMesh );
    SAFE_RELEASE(m_pTubeMesh );
    SAFE_RELEASE(m_pMeshTexture);
  	SAFE_RELEASE(m_pTeapot);

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
    SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pBoxMesh);
  	SAFE_RELEASE(m_pTubeMesh);
  	SAFE_RELEASE(m_pTeapot);
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

    // No fallback, so need ps1.1
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
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
    m_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

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
                   gFact+=0.01f;
				   if(gFact>1.0f)gFact=1.0f;
                break;
			    case IDM_DEC:
                    if(gFact>0.0)gFact-=0.01f;
                break;
				case IDM_DEC_TAPE:
                    if(gTapeFact>0.0)gTapeFact-=0.01f;
	  		    break;
				case IDM_RESET_TAPE:
                    gTapeFact=0.0f;
	  		    break;
				case IDM_INC_TAPE:
    				gTapeFact+=0.01f;
					if(gTapeFact>1.0f)gTapeFact=1.0f;
				break;		
				case IDM_DEC_TWIST:
                    if(gTwistFact>0.0)gTwistFact-=0.01f;
 			    break;
				case IDM_RESET_TWIST:
                    gTwistFact=0.0f;
 			    break;
				case IDM_INC_TWIST:
    				gTwistFact+=0.01f;
					if(gTwistFact>10.0f)gTwistFact=10.0f;
				break;
				case IDM_DEC_SPHERIFY:
                    if(gSpherifyFact>0.0)gSpherifyFact-=0.01f;
				break;
				case IDM_RESET_SPHERIFY:
                    gSpherifyFact=0.0f;
				break;
				case IDM_INC_SPHERIFY:
    				gSpherifyFact+=0.01f;
					if(gSpherifyFact>1.0f)gSpherifyFact=1.0f;
				break;
				case IDM_TOGGLE_WIRE:
                    m_bShowWire = !m_bShowWire;
                break;
				case IDM_PREVOBJECT:
					currObject=abs(currObject-1)%4;
				break;	
				case IDM_NEXTOBJECT:
					currObject=abs(currObject+1)%4;
				break;	
				case IDM_INCCENTER:
					gtCenter=gtCenter+0.01f;
				break;
				case IDM_DECCENTER:
					gtCenter=gtCenter-0.01f;
				break;	
				case IDM_INCRANGE:
					gtRange=gtRange+0.01f;
				break;
				case IDM_DECRANGE:
					if(gtRange>0.0f)gtRange=gtRange-0.01f;
				break;
				case IDM_INCTWISTSPEED:
							gSpeed+=1;
				break;
				case IDM_DECTWISTSPEED:
					if(gSpeed>0){
							gSpeed-=1;
					}
				break;	
				case IDM_TWIST:
					m_bEffect=1;
			
			        Cleanup3DEnvironment();
					Initialize3DEnvironment();
				break;
				case IDM_SPHERIFY:
					m_bEffect=2;
					Cleanup3DEnvironment();
					Initialize3DEnvironment();	
				break;
				case IDM_TAPE:
					m_bEffect=0;
					Cleanup3DEnvironment();
					Initialize3DEnvironment();
				break;
				case IDM_TAPE_TWIST:
					m_bEffect=3;
					Cleanup3DEnvironment();
					Initialize3DEnvironment();
				break;
			
            }
        }
    }

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}


