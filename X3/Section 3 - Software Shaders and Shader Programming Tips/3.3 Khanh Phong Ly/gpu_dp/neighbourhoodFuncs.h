#ifndef __NEIGHBOURHOODFUNCS_H__
#define __NEIGHBOURHOODFUNCS_H__

#include <d3dx9math.h>

// funcs to initialise neighbourhood mask and residue maps for a given boundary map.
typedef struct
{
	unsigned short	m[4];
}A16B16G16R16_Elem;






//void Init_ActionOutcomeProbabilityMap(D3DXVECTOR4 *pv, int numActions, int numOutcomes);

// returns current texel value.
unsigned char _GetNeighboursRFromRGB( const int texelIndex, unsigned char neighbours[],
		int width , int height, const unsigned char* const pSrcData, D3DFORMAT format );

void _CalcResidues( const int texelIndex, const unsigned char neighbours[], int numActions, int numOutcomes,
				   const D3DXVECTOR4 aAOPM[], void* pDestc, void* pDestd, D3DFORMAT format );






#endif //__NEIGHBOURHOODFUNCS_H__