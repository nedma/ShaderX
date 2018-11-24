#include "dpInterface.h"
#include "neighbourhoodFuncs.h"
#include <assert.h>




// ---------------------------------------------------------------------------------
void _PackProbsIntoVec4s( float	prb[], D3DXVECTOR4 *pv )
{
	pv->x = prb[0];	pv->y = prb[1];	pv->z = prb[2];	pv->w = prb[3];
	++pv;
	pv->x = prb[4];	pv->y = prb[5];	pv->z = prb[6];	pv->w = prb[7];
}

// ---------------------------------------------------------------------------------



// -----------------------------------------------------------------------------------
// function : _GetNeighboursRFromRGB
// Get the neigbour's R component for texel from an rgb texture of size width*height.
// R holds BOUNDVAL if it is a boundary pixel( dont confuse with pixels along edges of texture)
// texel order is 0 for top left counting up as we count along +ve x. 
// texture passed in as char* . use +=3 to move to next texel.
// writes to neighbours.
//
// neighbour indices...
// 0  1  2
// 3     4
// 5  6  7
//
unsigned char _GetNeighboursRFromRGB( const int texelIndex, unsigned char neighbours[], int width , int height, const unsigned char* const pSrcData, D3DFORMAT format )
{
//	static const char BOUNDVAL = 0; // used in texture to represent a boundary
	char BOUNDVAL = DPBase::BOUNDVAL;

	int  tstride;
	if ( format == D3DFMT_X8R8G8B8 )
	{	tstride =4;	}
	else
	{
		MessageBox(NULL, "Invalid boundary texture format specified", "gpu_dp.exe", MB_OK);
		exit(0);
	}

	int		currTexel = texelIndex*tstride; //rgb format
	int		tex0 = (currTexel - tstride) - (width*tstride);
    int		tex1 = (currTexel) - (width*tstride);
	int		tex2 = (currTexel + tstride) - (width*tstride);
	int		tex3 = (currTexel - tstride);
	int		tex4 = (currTexel + tstride);
	int		tex5 = (currTexel - tstride) + (width*tstride);
    int		tex6 = (currTexel) + (width*tstride);
	int		tex7 = (currTexel + tstride) + (width*tstride);


		// we get data from the lowest byte in the stride. This is normally the B channel of XRGB format
	if ( 0==texelIndex )	//topleft texel
	{
		neighbours[0]= neighbours[1]= neighbours[2]= neighbours[3]= neighbours[5]= BOUNDVAL;
		neighbours[4] = pSrcData[tex4];
		neighbours[6] = pSrcData[tex6];
		neighbours[7] = pSrcData[tex7];
	}
	else if ( texelIndex == width-1 ) // top right texel
	{
		neighbours[0]= neighbours[1]= neighbours[2]= neighbours[4]= neighbours[7]= BOUNDVAL;
		neighbours[3] = pSrcData[tex3];
		neighbours[5] = pSrcData[tex5];
		neighbours[6] = pSrcData[tex6];
	}
	else if ( texelIndex== 1+ (width*height)- width ) // bottom left
	{
		neighbours[0]= neighbours[3]= neighbours[5]= neighbours[6]= neighbours[7]= BOUNDVAL;
		neighbours[1] = pSrcData[tex1];
		neighbours[2] = pSrcData[tex2];
		neighbours[4] = pSrcData[tex4];
	}
	else if ( texelIndex== (width*height) -1 ) // bottom right
	{
		neighbours[2]= neighbours[4]= neighbours[5]= neighbours[6]= neighbours[7]= BOUNDVAL;
		neighbours[0] = pSrcData[tex0];
		neighbours[1] = pSrcData[tex1];
		neighbours[3] = pSrcData[tex3];
	}
	else if ( texelIndex< width )	// top row
	{
		neighbours[0]= neighbours[1]= neighbours[2]= BOUNDVAL;
		neighbours[3] = pSrcData[tex3];
		neighbours[4] = pSrcData[tex4];
		neighbours[5] = pSrcData[tex5];
		neighbours[6] = pSrcData[tex6];
		neighbours[7] = pSrcData[tex7];
	}
	else if ( texelIndex> 1+ (width*height)- width ) // bottom row
	{
		neighbours[5]= neighbours[6]= neighbours[7]= BOUNDVAL;
		neighbours[0] = pSrcData[tex0];
		neighbours[1] = pSrcData[tex1];
		neighbours[2] = pSrcData[tex2];
		neighbours[3] = pSrcData[tex3];
		neighbours[4] = pSrcData[tex4];
	}
	else if ( texelIndex%width == 0 ) // left column
	{
		neighbours[0]= neighbours[3]= neighbours[5]= BOUNDVAL;
		neighbours[1] = pSrcData[tex1];
		neighbours[2] = pSrcData[tex2];
		neighbours[4] = pSrcData[tex4];
		neighbours[6] = pSrcData[tex6];
		neighbours[7] = pSrcData[tex7];
	}
	else if ( (texelIndex+1)%width == 0 ) // right column
	{
		neighbours[2]= neighbours[4]= neighbours[7]= BOUNDVAL;
		neighbours[0] = pSrcData[tex0];
		neighbours[1] = pSrcData[tex1];
		neighbours[3] = pSrcData[tex3];
		neighbours[5] = pSrcData[tex5];
		neighbours[6] = pSrcData[tex6];
	}
	else			// interior texel.
	{
		neighbours[0] = pSrcData[tex0];
		neighbours[1] = pSrcData[tex1];
		neighbours[2] = pSrcData[tex2];
		neighbours[3] = pSrcData[tex3];
		neighbours[4] = pSrcData[tex4];
		neighbours[5] = pSrcData[tex5];
		neighbours[6] = pSrcData[tex6];
		neighbours[7] = pSrcData[tex7];
	}
	
	// check and set valid values. Vals either have to be 0 or 1.
	// done so that invalid pixel vals are corrected.
	for ( int i=0; i<8; i++ )
	{
		if ( neighbours[i] != 0 )
		{
			neighbours[i]= 255;
		}
	}


	return pSrcData[currTexel];
}
// ----------------------------------------------------------------------------
void _CalcResidueProbForAction( const unsigned char neighbours[], int numOutcomes, const float* const actionOutcomeProbs, float* retProb )
{
	int i;
	float accum=0;
	for (i=0; i<numOutcomes; i++)
	{
		accum += ((float)neighbours[i] * actionOutcomeProbs[i])/255.0f; // assume that neighbour vals are either 0 or 256(=1.0)
	}
	*retProb = 1.0f - accum;
}

// ----------------------------------------------------------------------------
// for a given texel, pack the residues into 2 4vecs.
// ----------------------------------------------------------------------------
//void _CalcResidues( const int texelIndex, const unsigned char neighbours[], int numActions, int numOutcomes,
//				   const D3DXVECTOR4 aAOPM[], A16B16G16R16_Elem* pDestc, A16B16G16R16_Elem* pDestd )
void _CalcResidues( const int texelIndex, const unsigned char neighbours[], int numActions, int numOutcomes,
				   const D3DXVECTOR4 aAOPM[], void* pDestc, void* pDestd, D3DFORMAT format )
{
	assert(numActions==8);

	int j;
	float *pRetProb= new float[numActions];
	
	for ( j=0; j<numActions; j++ )
	{
		int indexIntoAOPM = (j*numOutcomes*sizeof(float))/ sizeof(D3DXVECTOR4)  ;
		_CalcResidueProbForAction( neighbours, numOutcomes, (float*)&aAOPM[indexIntoAOPM], &pRetProb[j] );
	}


	if ( format == D3DFMT_A16B16G16R16 )
	{
	// pack into A16B16G16R16_Elem format.
		A16B16G16R16_Elem* _pDestc = (A16B16G16R16_Elem*)pDestc;
		A16B16G16R16_Elem* _pDestd = (A16B16G16R16_Elem*)pDestd;
		for ( j=0; j<4; j++ )
		{
			_pDestc[texelIndex].m[j] = (unsigned short)( pRetProb[j]*65535 );
		}
		for ( j=0; j<4; j++ )
		{
			_pDestd[texelIndex].m[j] = (unsigned short)( pRetProb[j+4]*65535 );
		}
	}
	else if ( format == D3DFMT_A32B32G32R32F )
	{
		D3DXVECTOR4* _pDestc = (D3DXVECTOR4*)pDestc;
		D3DXVECTOR4* _pDestd = (D3DXVECTOR4*)pDestd;
		_pDestc[texelIndex] = D3DXVECTOR4( &pRetProb[0] );
		_pDestd[texelIndex] = D3DXVECTOR4( &pRetProb[4] );

//		_pDestc[texelIndex].m[j] = (unsigned short)( pRetProb[j]*65535 );
//		_pDestd[texelIndex].m[j] = (unsigned short)( pRetProb[j+4]*65535 );
	}

	delete pRetProb;
}

// ----------------------------------------------------------------------------
