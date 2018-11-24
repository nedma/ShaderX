#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include <assert.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"

#include <fstream>
#include "debugFuncs.h"



std::fstream* g_poutfile= NULL;
/*
void DumpValsToFile(std::fstream& pfstream, LPDIRECT3DSURFACE9 psurf, int width, int height, D3DFORMAT format )
{
	return;

	// only support r32f format for now.
	if ( format != D3DFMT_R32F )
		return;
	HRESULT		hr;
	D3DLOCKED_RECT	lrect;
	RECT		lockregion;
	lockregion.bottom = height;
	lockregion.left	  = 0;
	lockregion.right  = width;
	lockregion.top    = 0;

//	if( FAILED( hr = psurf->LockRect( &lrect, &lockregion, D3DLOCK_NO_DIRTY_UPDATE ) ) )
	if( FAILED( hr = psurf->LockRect( &lrect, &lockregion, D3DLOCK_READONLY ) ) )
		{DXTRACE_ERR( "LockRect", hr );}

	float *psData = (float*)(lrect.pBits);
	int i,j, k=0;
	for ( i=0; i < height; i++)
	{
		char pbuf[128];

		pfstream << "\n";
		for ( j=0; j< width; j++ )
		{
//			if ( (j!=0)&&(j!=width-1) )
			{
				sprintf( pbuf, "%5.2f\t", psData[k] );
				pfstream << pbuf;
			}
		k++;
		}
	}
	psurf->UnlockRect();
	pfstream.flush();

}
*/
