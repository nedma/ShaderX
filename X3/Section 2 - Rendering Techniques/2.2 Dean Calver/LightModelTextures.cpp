#include "dxstdafx.h"

#include <vector>

#include "LightModelTexture.h"

inline float bias( float x )
{
	return (x * 0.5f) + 0.5f;
}

inline float unbias( float x )
{
	return (x * 2.f) - 1.f;
}

float GetDotProductFunctionU( enum DOTP_FUNCTIONS func )
{
	float u = (float(func) * float(BILERP_PROTECTION)) + float(BILERP_PROTECTION)/2;
	const float spix = 1.f / (float(NUM_DOTP_FUNCTIONS)*BILERP_PROTECTION);
	u *= spix;
	return u;
}

float MaterialIndexToV( unsigned int material )
{
	return float(material) / float(NUM_MATERIALS);
}

float AttenuationIndexToV( enum ATTENUATION_FUNCTION func )
{
	float u = (float(func) * float(BILERP_PROTECTION)) + float(BILERP_PROTECTION)/2;
	const float spix = 1.f / (float(NUM_ATTENUATION_FUNCTIONS)*BILERP_PROTECTION);
	u *= spix;
	return u;
}


std::vector< Material > MaterialList;

unsigned int RegisterMaterial( const Material& mat )
{
	MaterialList.push_back( mat );
	return MaterialList.size() - 1;
}

void EmptyMaterialList()
{
	MaterialList.clear();

	// add the default materials 
	MaterialList.push_back( Material( 0, 0, 1 ) );
	// ambient only
	MaterialList.push_back( Material( 1, 0, 1 ) );
	// emmisive only
	MaterialList.push_back( Material( 0, 1, 1 ) );
	// specular (no ambient or emmisive)
	MaterialList.push_back( Material( 0, 0, 1 ) );
	// metal specular (no ambient slight emmsive)
	MaterialList.push_back( Material( 0, 0.1f, 0 ) );
}

/* The material texture takes the single index stored in the fat framebuffer to the
parameters used to select the various components of the light model
*/
VOID WINAPI FillMaterialTexture(	D3DXVECTOR4* pOut, 
									CONST D3DXVECTOR2* pTexCoord,
									CONST D3DXVECTOR2* pTexelSize,
									LPVOID pData )
{
	UNREFERENCED_PARAMETER( pData );
	UNREFERENCED_PARAMETER( pTexelSize );

	// pTexCoord->x = u = material item (1 of the 4 items)
	// pTexCoord->y = u = material number
	const unsigned int index = unsigned int(pTexCoord->x * NUM_MATERIALS);
	const unsigned int matNum = index;

	if( matNum < MaterialList.size() )
	{
		pOut->x = 0; // unused
		pOut->y = MaterialList[matNum].KSpecCol;
		pOut->z = MaterialList[matNum].KAmb;
		pOut->w = MaterialList[matNum].KEmm;		
	} else
	{
		*pOut = D3DXVECTOR4(0,0,0,0);
	}
}

VOID WINAPI FillDotProductTexture( 		D3DXVECTOR4* pOut, 
										CONST D3DXVECTOR2* pTexCoord,
										CONST D3DXVECTOR2* pTexelSize,
										LPVOID pData )
{
	UNREFERENCED_PARAMETER( pData );
	UNREFERENCED_PARAMETER( pTexelSize );
	// pTexCoord->x need unbiasing to -1 to 1

	// pTexCoord->x = u = N dot L
	// pTexCoord->y = v = material select
	// pOut->x = shaped dot product result
	const unsigned int index = unsigned int(pTexCoord->y *float(NUM_DOTP_FUNCTIONS) * float(BILERP_PROTECTION));
	const unsigned int matNum = index / BILERP_PROTECTION;
	float dot = unbias(pTexCoord->x);

	// each dot product function take 2 lines in v
	// if both lines are the same the function is stepped, else you can use bilerp to give
	// you a peiceiwise linear function between functions.
	// dot product funcion 0 is texel.y 1, function 1 = texel.y = 3
	// sampling should take place in the middle of lines

	if( matNum <= DPF_ONE )
	{
		switch( matNum )
		{
		case DPF_ZERO:
			pOut->x = 0.0f;
			pOut->y = 1.0f;
			break;
		case DPF_QUARTER:
			pOut->x = 0.25f;
			pOut->y = 1.0f;
			break;
		case DPF_HALF:
			pOut->x = 0.5f;
			pOut->y = 1.0f;
			break;
		case DPF_THREEQUARTER:
			pOut->x = 0.75f;
			pOut->y = 1.0f;
			break;
		case DPF_ONE:
			pOut->x = 1.f;
			pOut->y = 1.0f;
			break;
		}
	} 
	// 4 dot product functions are stepped
	// Nil				- return 0 
	// front linear	pow1	- standard lambert diffuse (back culled)
	// front linear pow2	- if( dot < 0) return 0 else return	dot^2
	// front linear pow3	- if( dot < 0) return 0 else return	dot^3
	// front linear pow4	- if( dot < 0) return 0 else return	dot^4
	else if( matNum <= DPF_FRONT_POW4 )
	{
		// backface light
		dot = max( dot, 0.f );
		pOut->y = (dot <= 0) ? 0.f : 1.0f;

		float power = float(matNum - DPF_FRONT_POW1);
		power += 1;
		pOut->x = powf( dot, power );

	}
	// next dot product functions are stepped
	// each one is front facing only and has the dot raised
	// power from 8 to 64 every 4 powers
	else if( matNum <= DPF_FRONT_POW64 )
	{
		// backface light
		dot = max( dot, 0.f );
		pOut->y = (dot <= 0) ? 0.f : 1.0f;

		float power = float(matNum - DPF_FRONT_POW8);
		power = (power * 4) + 8;
		pOut->x = powf( dot, power );
	}
	// next dot product functions are stepped
	// each one is front facing only and has the dot raised
	// power from 128 to 1024 every 218 powers
	else if( matNum <= DPF_FRONT_POW1024 )
	{
		// backface light
		dot = max( dot, 0.f );
		pOut->y = (dot <= 0) ? 0.f : 1.0f;

		float power = float(matNum - DPF_FRONT_POW128);
		power = (power * 128) + 128;
		pOut->x = powf( dot, power );
	} 	else if( matNum <= DPF_FRESNEL_POW4 )
	{
		float power = float(matNum - DPF_FRESNEL_POW4);
		power = (power * 4) + 4;

		// make the dot go 0 to 1 to 0 rather than -1 to 0 to 1
		if( dot < 0 )
			dot= fabsf(dot+1);
		else
			dot= fabsf(dot-1);

		pOut->x = powf( dot, power );
		pOut->y = 1.0f;

	} 	else if( matNum <= DPF_SHIFTED_FRESNEL_POW8 )
	{
		float power = float(matNum - DPF_SHIFTED_FRESNEL_POW8);
		power = (power * 4) + 8;

		dot = dot - 0.4f;
		if(dot < -1)
			dot = -1;

		// make the dot go 0 to 1 to 0 rather than -1 to 0 to 1
		if( dot < 0 )
			dot= fabsf(dot+1);
		else
			dot= fabsf(dot-1);

		pOut->x = powf( dot, power );
		pOut->y = 1.0f;
	}
}

VOID WINAPI FillAttenuationTexture( 	D3DXVECTOR4* pOut, 
										CONST D3DXVECTOR2* pTexCoord,
										CONST D3DXVECTOR2* pTexelSize,
										LPVOID pData )
{
	UNREFERENCED_PARAMETER( pData );
	UNREFERENCED_PARAMETER( pTexelSize );

	const unsigned int index = unsigned int(pTexCoord->y *float(NUM_ATTENUATION_FUNCTIONS) * float(BILERP_PROTECTION));
	const unsigned int matNum = index / BILERP_PROTECTION;

	if( matNum <= AF_ONE )
	{
		switch( matNum )
		{
		case AF_ZERO:
			pOut->x = 0.0f;
			break;
		case AF_QUARTER:
			pOut->x = 0.25f;
			break;
		case AF_HALF:
			pOut->x = 0.5f;
			break;
		case AF_THREEQUARTER:
			pOut->x = 0.75f;
			break;
		case AF_ONE:
			pOut->x = 1.f;
			break;
		}
	}  else if ( matNum <= AF_LINEAR )
	{
		// just flip things over so at distance 0 attenuation = 1 
		float atten = 1.f - pTexCoord->x;
		pOut->x = atten;
	} else if ( matNum <= AF_SQUARED )
	{
		// square the flipped input
		float atten = 1.f - pTexCoord->x;
		atten = atten * atten;
		pOut->x = atten;
	} else if ( matNum <= AF_SHAPE1 )
	{
		// a constant start (for 0.5f)then linear to 0 at in->x = 1
		float atten = 1.f - pTexCoord->x;
		atten = (atten * 1.5f);
		if( atten > 1 )
			atten = 1;
		pOut->x = atten;
	} else if ( matNum <= AF_SHAPE2 )
	{
		// a constant start (for 0.3f) then squared to 0 at in->x = 1
		float atten = 1.f - pTexCoord->x;
		atten = (atten * 1.3f);
		if( atten > 1 )
			atten = 1;
		atten /= 1.3f;
		atten = atten * atten;
		pOut->x = atten;
	}

	if( pTexCoord->x > 0.999f )
	{
		pOut->x = 0;
	}

}