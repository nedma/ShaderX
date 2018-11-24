//
// ShaderX³: Morphing Between Different Objects
//
//	written 2004 by Ronny Burkersroda
//

// header includes
#include					"Morph class.h"								// "Two In One" sample class
#include					"Direct3D functions.h"						// Direct3D functions
#include					"D3DX mesh functions.h"						// D3DX mesh functions
#include					"resource.h"								// resource IDs
#include					<d3dx9.h>									// Direct3D 8 extensions
#include					<stdio.h>


//#define						_FULLSCREEN


// ***********************
// *** constant values ***
// ***********************

// strings
const PCSTR					c_pstrTH3DSMorphWindowClass						// pointer to a name for the class of the sample window
								= "TH3DS_Morph";
const PCSTR					c_pstrTH3DSMorphWindowName						// pointer to a name of the sample window
								= "ShaderX³: Morphing Between Different Objects";

// vectors and matrices
const D3DXMATRIX			c_mtrxIdentity									// identity matrix
								= D3DXMATRIX( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
const D3DXVECTOR3			c_vct3MorphingCenter							// morphing center
								= D3DXVECTOR3( 0.0f, 1.5f, 0.0f );


// *****************************
// *** class implementations ***
// *****************************

//
// standard constructor
//	sets standard values.
//
CD3DMorpherSample::CD3DMorpherSample()
	: m_pd3dd( NULL )
	, m_nBlending( 0 )
	, m_fBlendingTime( 0.0f )
{
}

//
// standard destructor
//	cleans up the object.
//
CD3DMorpherSample::~CD3DMorpherSample()
{
}

//
// sample main method
//	shows the working with frames.
//
INT CD3DMorpherSample::Main( HINSTANCE hmodInstance, PSTR pstrCommandLine, INT nWindowShowValue )
{
	D3DCAPS9				caps;
	D3DPRESENT_PARAMETERS	prsp;
	INT						nExitCode;

	// call base class function
	if( ( nExitCode = CD3DSampleBase::Main( hmodInstance, pstrCommandLine, nWindowShowValue ) ) >= 0 )
	{
		// register window class
		if( ATOM atomWindowClass = RegisterWindowClass( CS_CLASSDC, WindowMessageProcedure, LoadIcon( hmodInstance, MAKEINTRESOURCE( IDI_SAMPLE ) ), LoadCursor( NULL, IDC_ARROW ), NULL, NULL, c_pstrTH3DSMorphWindowClass ) )
		{
			// create window
			if( m_hwndMain = CreateWindow( c_pstrTH3DSMorphWindowClass, c_pstrTH3DSMorphWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 0, 640, 480, NULL, NULL, this ) )
			{

#ifdef						_FULLSCREEN

				// create Direct3D device
				if( SUCCEEDED( Direct3D::BuildPresentParameters( &prsp, m_hwndMain, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 800, 600, D3DPRESENT_RATE_DEFAULT, FALSE, D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, 2, D3DPRESENT_INTERVAL_IMMEDIATE ) ) && SUCCEEDED( Direct3D::InitializeDevice( &m_pd3dd, &prsp ) ) )

#else

				// create Direct3D device
				if( SUCCEEDED( Direct3D::BuildPresentParameters( &prsp, m_hwndMain ) ) && SUCCEEDED( Direct3D::InitializeDevice( &m_pd3dd, &prsp ) ) )

#endif
				{
					// create Direct3D object
					if( PDIRECT3D9 pd3d = Direct3DCreate9( D3D_SDK_VERSION ) )
					{
						// get original hardware capabilities without software simulation
						pd3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

						// release Direct3D
						pd3d->Release();
					}

					D3DXMATRIX					mtrxTransform;
					D3DXMATRIX					mtrxProjection;
					D3DXVECTOR4					vct4Temporary;
					D3DLIGHT9					lht								= { D3DLIGHT_DIRECTIONAL, { 1.0f, 0.9f, 0.8f, 1.0f }, { 0.75f, 0.75f, 0.75f, 1.0f }, { 0.4f, 0.5f, 0.6f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { -0.5f, -1.0f, 1.0f }, 1000.0f, 0.0f, 0.0f, 0.08f, 0.0f, 0.0f, 0.0f };
					MSG							msg;
					ID3DXBuffer*				pbufrMaterials[]				= { NULL, NULL };
					ID3DXMesh*					pmeshMeshes[]					= { NULL, NULL };
					ID3DXMesh*					pmeshTemporary;
					ID3DXBuffer*				pbufrTemporary;
					PDIRECT3DTEXTURE9			ptxtrTexture;
					PDIRECT3DVERTEXSHADER9		pvshVertexShader				= NULL;
					MATERIAL*					mtrlMaterials;
					DWORD						nMaterialsCounts[ 2 ];
					DWORD						nCounter;
					DWORD						nMesh;
					FLOAT						fTime							= 30.0f;
					FLOAT						fRotorSpeed						= 0.0f;
					FLOAT						fTimer							= 0.0f;
					FLOAT						fFrameQuantity					= 1.0f;
					FLOAT						fValue;
					FLOAT						fHeight							= 0.0f;

					// set current directory to medias
					SetCurrentDirectory( "media" );

					// load first mesh
					if( SUCCEEDED( D3DXLoadMeshFromX( "apple.x", D3DXMESH_MANAGED, m_pd3dd, NULL, &pbufrMaterials[ 0 ], NULL, &nMaterialsCounts[ 0 ], &pmeshMeshes[ 0 ] ) ) )
					{
						// get pointer to materials
						mtrlMaterials = (MATERIAL*) pbufrMaterials[ 0 ]->GetBufferPointer();

						// process materials
						for( nCounter = 0; nCounter < nMaterialsCounts[ 0 ]; ++nCounter )
						{
							// copy diffuse material colors to ambient ones
							mtrlMaterials[ nCounter ].matrColors.Ambient = mtrlMaterials[ nCounter ].matrColors.Diffuse;

							// load material texture
							if( SUCCEEDED( D3DXCreateTextureFromFile( m_pd3dd, ( (D3DXMATERIAL*) mtrlMaterials )[ nCounter ].pTextureFilename, &ptxtrTexture ) ) )
								mtrlMaterials[ nCounter ].ptxtrTexture = ptxtrTexture;
							else
								mtrlMaterials[ nCounter ].ptxtrTexture = NULL;
						}
					}

					// load second mesh
					if( SUCCEEDED( D3DXLoadMeshFromX( "orange.x", D3DXMESH_MANAGED, m_pd3dd, NULL, &pbufrMaterials[ 1 ], NULL, &nMaterialsCounts[ 1 ], &pmeshMeshes[ 1 ] ) ) )
					{
						// get pointer to materials
						mtrlMaterials = (MATERIAL*) pbufrMaterials[ 1 ]->GetBufferPointer();

						// process materials
						for( nCounter = 0; nCounter < nMaterialsCounts[ 1 ]; ++nCounter )
						{
							// copy diffuse material colors to ambient ones
							mtrlMaterials[ nCounter ].matrColors.Ambient = mtrlMaterials[ nCounter ].matrColors.Diffuse;

							// load material texture
							if( SUCCEEDED( D3DXCreateTextureFromFile( m_pd3dd, ( (D3DXMATERIAL*) mtrlMaterials )[ nCounter ].pTextureFilename, &ptxtrTexture ) ) )
								mtrlMaterials[ nCounter ].ptxtrTexture = ptxtrTexture;
							else
								mtrlMaterials[ nCounter ].ptxtrTexture = NULL;
						}
					}

					// build morphing mesh for the first one
					if( SUCCEEDED( D3DXMeshFunctions::BuildMorphingMesh( pmeshMeshes[ 0 ], pmeshMeshes[ 1 ], NULL, &pmeshTemporary ) ) )
					{
						// release original mesh
						pmeshMeshes[ 0 ]->Release();

						// set new one
						pmeshMeshes[ 0 ] = pmeshTemporary;
					}

					// build morphing mesh for the second one
					if( SUCCEEDED( D3DXMeshFunctions::BuildMorphingMesh( pmeshMeshes[ 1 ], pmeshMeshes[ 0 ], NULL, &pmeshTemporary ) ) )
					{
						// release original mesh
						pmeshMeshes[ 1 ]->Release();

						// set new one
						pmeshMeshes[ 1 ] = pmeshTemporary;
					}

					// compile vertex shader
					if( SUCCEEDED( D3DXAssembleShaderFromFile( "morphing with directional light.vsh", NULL, NULL, 0, &pbufrTemporary, NULL ) ) )
					{
						// create vertex shader
						m_pd3dd->CreateVertexShader( (PDWORD) pbufrTemporary->GetBufferPointer(), &pvshVertexShader );

						// release buffer
						pbufrTemporary->Release();
					}

					// initialize timer
					InitializeTimer();

					// show window
					UpdateWindow( m_hwndMain );
					ShowWindow( m_hwndMain, nWindowShowValue );

					// get and dispatch all messages but stop on quit
					do
					{
						PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE );

						TranslateMessage( &msg );
						DispatchMessage( &msg );

						// check reset flag
						if( m_nDevice3DFlagsValue & DEVICE3D_RESET )
						{
							// update window size
							Direct3D::UpdatePresentParameters( &prsp );

							// reset device
							if( SUCCEEDED( m_pd3dd->Reset( &prsp ) ) )
							{
								// build and set projection matrix
								D3DXMatrixPerspectiveFovLH( &mtrxProjection, D3DX_PI * 0.08f, (FLOAT) prsp.BackBufferWidth / (FLOAT) prsp.BackBufferHeight, 1.0f, 10000.0f );
								m_pd3dd->SetTransform( D3DTS_PROJECTION, &mtrxProjection );

								// set and enable light
								m_pd3dd->SetLight( 0, &lht );
								m_pd3dd->LightEnable( 0, TRUE );

								// set light direction to vertex shader
								m_pd3dd->SetVertexShaderConstantF( 12, (PFLOAT) &lht.Direction, 1 );

								// enable specular lighting
								m_pd3dd->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
								m_pd3dd->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

								// enable alpha blending
								m_pd3dd->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
								m_pd3dd->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
								m_pd3dd->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
								m_pd3dd->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
								m_pd3dd->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
								m_pd3dd->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

								// enable trilinear texture filtering
								m_pd3dd->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
								m_pd3dd->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
								m_pd3dd->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
								m_pd3dd->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
								m_pd3dd->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
								m_pd3dd->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

								// set identity world matrix to vertex shader
								m_pd3dd->SetVertexShaderConstantF( 0, (PFLOAT) &c_mtrxIdentity, 4 );
							}
						}

						// check if rendering is enabled
						if( TestDevice( m_pd3dd, &prsp ) )
						{
							// calculate elapsed time
							CalculateElapsedTime();
							fTime += m_fElapsedTime;

							// set maximum displacement value
							if( ( m_fBlendingTime < 0.1f ) || ( m_fBlendingTime > 0.9f ) )
								fValue = 0.0f;
							else if( m_fBlendingTime < 0.35f )
								fValue = ( m_fBlendingTime - 0.1f ) * 0.05f;
							else if( m_fBlendingTime > 0.65f )
								fValue = ( 0.9f - m_fBlendingTime ) * 0.05f;
							else
								fValue = 0.0125f;

							// calculate displacement table and set it to vertex shader
							for( nCounter = 64; nCounter--; )
							{
								vct4Temporary.x = vct4Temporary.y = vct4Temporary.z = vct4Temporary.w = sinf( fTime * 5.0f + (FLOAT) nCounter / 32.0f * D3DX_PI ) * fValue;
								m_pd3dd->SetVertexShaderConstantF( 16 + nCounter, (PFLOAT) &vct4Temporary, 1 );
							}

							// calculate blending time
							if( ( m_nBlending == 0 ) && ( ( m_fBlendingTime += m_fElapsedTime * 0.1f ) > 1.0f ) )
							{
								m_nBlending		= 1;
								m_fBlendingTime	= 1.0f;
							}
							else if( ( m_nBlending == 1 ) && ( ( m_fBlendingTime -= m_fElapsedTime * 1.2f ) < 0.0f ) )
							{
								m_nBlending		= 0;
								m_fBlendingTime	= 0.0f;
							}

							// calculate view position and set it to vertex shader
							*(D3DXVECTOR3*) &vct4Temporary = D3DXVECTOR3( sinf( fTime * 0.8f ) * 6.0f, cosf( fTime * 0.13f ) * 2.0f + 4.0f, cosf( fTime * 0.8f ) * 6.0f );
							m_pd3dd->SetVertexShaderConstantF( 13, (PFLOAT) &vct4Temporary, 1 );

							// build and set view matrix
							D3DXMatrixLookAtLH( &mtrxTransform, (D3DXVECTOR3*) &vct4Temporary, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
							m_pd3dd->SetTransform( D3DTS_VIEW, &mtrxTransform );

							// compute view projection matrix and set it to vertex shader
							m_pd3dd->SetVertexShaderConstantF( 4, (PFLOAT) D3DXMatrixTranspose( &mtrxTransform, D3DXMatrixMultiply( &mtrxTransform, &mtrxTransform, &mtrxProjection ) ), 4 );

							// activate vertex shader for morphing
							m_pd3dd->SetVertexShader( pvshVertexShader );

							// clear screen
							m_pd3dd->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF1F3F7F, 1.0f, 0 );
							m_pd3dd->BeginScene();

							// set mesh number by blending time
							if( m_fBlendingTime <= 0.5f )
								nMesh = 0;
							else
								nMesh = 1;

							// set blending factors for first mesh
							if( nMesh )
								m_pd3dd->SetVertexShaderConstantF( 15, (PFLOAT) &D3DXVECTOR4( 1.0f - m_fBlendingTime, 1.0f, 0.0f, 2.0f ), 1 );
							else
								m_pd3dd->SetVertexShaderConstantF( 15, (PFLOAT) &D3DXVECTOR4( m_fBlendingTime, 1.0f, 0.0f, 2.0f ), 1 );

							// get pointer to materials of the first mesh
							mtrlMaterials = (MATERIAL*) pbufrMaterials[ nMesh ]->GetBufferPointer();

							// draw first mesh
							for( nCounter = 0; nCounter < nMaterialsCounts[ nMesh ]; ++nCounter )
							{
								// set subset material
								m_pd3dd->SetMaterial( &mtrlMaterials[ nCounter ].matrColors );

								// modulate material colors by light and set them to vertex shader
								m_pd3dd->SetVertexShaderConstantF( 8, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Diffuse.r * lht.Diffuse.r, mtrlMaterials[ nCounter ].matrColors.Diffuse.g * lht.Diffuse.g, mtrlMaterials[ nCounter ].matrColors.Diffuse.b * lht.Diffuse.b, mtrlMaterials[ nCounter ].matrColors.Diffuse.a ), 1 );
								m_pd3dd->SetVertexShaderConstantF( 9, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Ambient.r * lht.Ambient.r, mtrlMaterials[ nCounter ].matrColors.Ambient.g * lht.Ambient.g, mtrlMaterials[ nCounter ].matrColors.Ambient.b * lht.Ambient.b, 1.0f ), 1 );
								m_pd3dd->SetVertexShaderConstantF( 10, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Specular.r * lht.Specular.r, mtrlMaterials[ nCounter ].matrColors.Specular.g * lht.Specular.g, mtrlMaterials[ nCounter ].matrColors.Specular.b * lht.Specular.b, mtrlMaterials[ nCounter ].matrColors.Power ), 1 );
								m_pd3dd->SetVertexShaderConstantF( 11, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Emissive.r, mtrlMaterials[ nCounter ].matrColors.Emissive.g, mtrlMaterials[ nCounter ].matrColors.Emissive.b, 1.0f ), 1 );

								// set subset texture
								m_pd3dd->SetTexture( 0, mtrlMaterials[ nCounter ].ptxtrTexture );

								// draw current subset
								pmeshMeshes[ nMesh ]->DrawSubset( nCounter );
							}

							// clear z-buffer
							m_pd3dd->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0xFF1F3F7F, 1.0f, 0 );

							// invert mesh number
							nMesh = 1 - nMesh;

							// set blending factors for first mesh
							if( nMesh )
								m_pd3dd->SetVertexShaderConstantF( 15, (PFLOAT) &D3DXVECTOR4( 1.0f - m_fBlendingTime, m_fBlendingTime, 0.0f, 2.0f ), 1 );
							else
								m_pd3dd->SetVertexShaderConstantF( 15, (PFLOAT) &D3DXVECTOR4( m_fBlendingTime, 1.0f - m_fBlendingTime, 0.0f, 2.0f ), 1 );

							// get pointer to materials of the second mesh
							mtrlMaterials = (MATERIAL*) pbufrMaterials[ nMesh ]->GetBufferPointer();

							// draw second mesh
							for( nCounter = 0; nCounter < nMaterialsCounts[ nMesh ]; ++nCounter )
							{
								// set subset material
								m_pd3dd->SetMaterial( &mtrlMaterials[ nCounter ].matrColors );

								// modulate material colors by light and set them to vertex shader
								m_pd3dd->SetVertexShaderConstantF( 8, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Diffuse.r * lht.Diffuse.r, mtrlMaterials[ nCounter ].matrColors.Diffuse.g * lht.Diffuse.g, mtrlMaterials[ nCounter ].matrColors.Diffuse.b * lht.Diffuse.b, mtrlMaterials[ nCounter ].matrColors.Diffuse.a ), 1 );
								m_pd3dd->SetVertexShaderConstantF( 9, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Ambient.r * lht.Ambient.r, mtrlMaterials[ nCounter ].matrColors.Ambient.g * lht.Ambient.g, mtrlMaterials[ nCounter ].matrColors.Ambient.b * lht.Ambient.b, 1.0f ), 1 );
								m_pd3dd->SetVertexShaderConstantF( 10, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Specular.r * lht.Specular.r, mtrlMaterials[ nCounter ].matrColors.Specular.g * lht.Specular.g, mtrlMaterials[ nCounter ].matrColors.Specular.b * lht.Specular.b, mtrlMaterials[ nCounter ].matrColors.Power ), 1 );
								m_pd3dd->SetVertexShaderConstantF( 11, (PFLOAT) &D3DXVECTOR4( mtrlMaterials[ nCounter ].matrColors.Emissive.r, mtrlMaterials[ nCounter ].matrColors.Emissive.g, mtrlMaterials[ nCounter ].matrColors.Emissive.b, 1.0f ), 1 );

								// set subset texture
								m_pd3dd->SetTexture( 0, mtrlMaterials[ nCounter ].ptxtrTexture );

								// draw current subset
								pmeshMeshes[ nMesh ]->DrawSubset( nCounter );
							}

							// show it
							m_pd3dd->EndScene();
							m_pd3dd->Present( NULL, NULL, NULL, NULL );
						}
					}
					while( msg.message != WM_QUIT );

					// release vertex shader
					if( pvshVertexShader )
						pvshVertexShader->Release();

					// get pointer to materials of the second mesh
					mtrlMaterials = (MATERIAL*) pbufrMaterials[ 1 ]->GetBufferPointer();

					// release textures
					for( nCounter = 0; nCounter < nMaterialsCounts[ 1 ]; ++nCounter ) if( mtrlMaterials[ nCounter ].ptxtrTexture )
						mtrlMaterials[ nCounter ].ptxtrTexture->Release();

					// get pointer to materials of the first mesh
					mtrlMaterials = (MATERIAL*) pbufrMaterials[ 0 ]->GetBufferPointer();

					// release textures
					for( nCounter = 0; nCounter < nMaterialsCounts[ 0 ]; ++nCounter ) if( mtrlMaterials[ nCounter ].ptxtrTexture )
						mtrlMaterials[ nCounter ].ptxtrTexture->Release();

					// release materials and meshes
					if( pbufrMaterials[ 1 ] )
						pbufrMaterials[ 1 ]->Release();
					if( pbufrMaterials[ 0 ] )
						pbufrMaterials[ 0 ]->Release();
					if( pmeshMeshes[ 1 ] )
						pmeshMeshes[ 1 ]->Release();
					if( pmeshMeshes[ 0 ] )
						pmeshMeshes[ 0 ]->Release();

					// set exit code
					nExitCode = (INT) msg.wParam;

					// release Direct3D device
					m_pd3dd->Release();
				}
				else
					nExitCode = -1;

				// destroy window
				DestroyWindow( m_hwndMain );
				m_hwndMain = NULL;
			}
			else
				nExitCode = -3;

			// unregister window class
			UnregisterClass( (LPCTSTR) atomWindowClass, m_hmodApplication );
		}
		else
			nExitCode = -5;
	}

	return nExitCode;
}

//
// window message procedure implementation
//	handles message for the window of the application.
//
LRESULT WINAPI CD3DMorpherSample::WindowMessageProcedure( HWND hwndWindow, UINT nMessage, WPARAM wprm, LPARAM lprm )
{
	static CD3DMorpherSample* pmrph = NULL;

	// process messages
	switch( nMessage )
	{
	case WM_CREATE:

		// set object
		pmrph = (CD3DMorpherSample*)( (LPCREATESTRUCT) lprm )->lpCreateParams;
		
		break;

	case WM_KEYDOWN:
		
		// check object
		if( pmrph && ( wprm == VK_SPACE ) && ( ( pmrph->m_fBlendingTime == 0.0f ) || ( pmrph->m_fBlendingTime == 1.0f ) ) )
		{
			if( pmrph->m_fBlendingTime )
				pmrph->m_nBlending = 2;
			else
				pmrph->m_nBlending = 1;
		}
		break;
	}

	// call base window procedure
	return CD3DSampleBase::WindowMessageProcedure( hwndWindow, nMessage, wprm, lprm );
}
