#include "pixelshaderutil.h"

#include <dxerr9.h>
#include <stdio.h>
#include <assert.h>


/**************************************************************************\
* Msg
* Debug tool. This code just pops up a message box if not in fullscreen mode.
\**************************************************************************/
void Msg(TCHAR *szFormat, ...)
{
    TCHAR szBuffer[512];

        va_list pArgs;
        va_start(pArgs, szFormat);
        _vstprintf(szBuffer, szFormat, pArgs);
        va_end(pArgs);
        OutputDebugString(szBuffer);

        MessageBox(NULL, szBuffer, TEXT("gpu_dp.exe"),MB_OK | MB_ICONERROR);

}

/**************************************************************************\
* DisplayShaderERR
* Puts up a message box that shows the err message give at shader assemble
* time.
\**************************************************************************/
void DisplayShaderERR(LPD3DXBUFFER ErrorMessages,HRESULT hr)
{
	if ( !ErrorMessages )
		return;
    
    char *c = (char *)ErrorMessages->GetBufferPointer();
    Msg(TEXT(c),hr);
}


/**************************************************************************\
* LoadShaderFile
* Loads pixel shaders
\**************************************************************************/
HRESULT LoadShaderFile(LPDIRECT3DDEVICE9 pd3dDevice, char *filename, IDirect3DPixelShader9 ** shader)
{
    TCHAR        strShaderPath[512];
    LPD3DXBUFFER pCode;
    HRESULT hr;
    LPD3DXBUFFER ErrorMessages;

    ////////////////////////////
    // Construct shader name  //
    // /////////////////////////
    strncpy(strShaderPath, _T(filename),512 );

    int n = strlen(filename);

    if( stricmp( &filename[n-4], "hlsl") == 0)
    {

        /////////////////////////////////////////////
        // Compile HLSL pixel shader from the file //
        /////////////////////////////////////////////
        if( FAILED( hr = D3DXCompileShaderFromFile( filename, 0,0,"main", "ps_2_0",0,&pCode, & ErrorMessages,NULL ) ) )
        {
            ////////////////////////
            // Inform user of err //
            ////////////////////////
            Msg(TEXT("Could not open an hlsl pixel shader file.\n"),hr);
//            DXTRACE_ERR( "couldnt assemble pixel shader from hlsl", hr );
    
            return hr;
        }
    }
    else
    {
        /////////////////////////////////////////////
        // Assemble the pixel shader from the file //
        /////////////////////////////////////////////
#if defined(_DEB_PS)
        if( FAILED( hr = D3DXAssembleShaderFromFile( strShaderPath, 0,0,D3DXSHADER_DEBUG, &pCode, &ErrorMessages ) ) )
#else
        if( FAILED( hr = D3DXAssembleShaderFromFile( strShaderPath, 0,0,0, &pCode, &ErrorMessages ) ) )
#endif
        {
            ////////////////////////
            // Inform user of err //
            ////////////////////////
            DisplayShaderERR(ErrorMessages,hr);
			DXTRACE_ERR( "couldnt assemble pixel shader", hr );
            return hr;
        }
		else
		{
			DisplayShaderERR(ErrorMessages,hr);
		}
    }

    /////////////////////////////
    // Create the pixel shader //
    /////////////////////////////
    hr = pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(),shader);
    pCode->Release();

    return hr;

}



HRESULT SetPixelOffsetConstants(LPDIRECT3DDEVICE9 pd3dDevice, int imageWidth, int imageHeight)
{
	HRESULT hr = S_OK;
	float width, height; //of each pixel
	float constants0To7[4*8];


	if( imageWidth == 0 || imageHeight == 0)
	{
		return S_FALSE;
	}
	width = 1.f/(float)imageWidth;
	height = 1.f/(float)imageHeight;
	memset(constants0To7,0,sizeof(constants0To7));

	/////////////
	// TOP ROW //
	/////////////
	constants0To7[0] = -width;         //c10.x// coord for up left
	constants0To7[1] = -height;        //c10.y// coord for up left

	constants0To7[4] = 0.0;            //c11.x// coord for up
	constants0To7[5] = -height;        //c11.y// coord for up

	constants0To7[8] = width;          //c12.x// coord for up right 
	constants0To7[9] = -height;        //c12.y// coord for up right 

	////////////////
	// CENTER ROW //
	////////////////
	constants0To7[12] = -width;        //c13.x// coord for left 
	constants0To7[13] = 0.0;           //c13.y// coord for left 

	constants0To7[16] = width;          //c14.x// coord for right 
	constants0To7[17] = 0.0;            //c14.y// coord for right 


	////////////////
	// BOTTOM ROW //
	////////////////
	constants0To7[20] = -width;         //c15.x// coord for down left
	constants0To7[21] = height;         //c15.y// coord for down left

	constants0To7[24] = 0.0;            //c16.y// coord for down 
	constants0To7[25] = height;         //c16.x// coord for down 

	constants0To7[28] = width;           //c17.x// coord for down right 
	constants0To7[29] = height;          //c17.y// coord for down right 


	///////////////////////////////
	// Load offsets into c10-c17 //
	///////////////////////////////
	hr = pd3dDevice->SetPixelShaderConstantF(  0, constants0To7,  8 );

	return hr;
}


