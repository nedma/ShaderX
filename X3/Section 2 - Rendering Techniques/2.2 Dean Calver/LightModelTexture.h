//-----------------------------------------------------------------------------
// File: LightModelTexture.h
//
// Desc: Header file the textures used to create the light model class 
//-----------------------------------------------------------------------------
#pragma once

// round up sigh
#define BILERP_PROTECTION	2

// space for 512 different materials
#define NUM_MATERIALS		512
// dot ranges from -1 to 1 with linear interpolation between samples
#define DOTP_WIDTH			2048
// space for 512 different dot product functions (need bilerp protection in y)
#define NUM_DOTP_FUNCTIONS	512


// attenuation texture function width ranges from 0-1 with linear interpolation between sample
#define ATTENUATION_WIDTH			512
// space for 512 different attuentation functions
#define NUM_ATTENUATION_FUNCTIONS	512

/*
The dotproduct functions take a single dotproduct (ranging from -1 to 1) 

In practise we use 2D textures so we can store a series of functions in a 
single texture and then pick the function via the extra coordinate.
*/
enum DOTP_FUNCTIONS
{
	DPF_ZERO		=	0,
	DPF_QUARTER,
	DPF_HALF,
	DPF_THREEQUARTER,
	DPF_ONE,

	DPF_FRONT_POW1,
	DPF_FRONT_POW2,
	DPF_FRONT_POW3,
	DPF_FRONT_POW4,

	DPF_FRONT_POW8,
	DPF_FRONT_POW12,
	DPF_FRONT_POW16,
	DPF_FRONT_POW20,
	DPF_FRONT_POW24,
	DPF_FRONT_POW28,
	DPF_FRONT_POW32,
	DPF_FRONT_POW36,
	DPF_FRONT_POW40,
	DPF_FRONT_POW44,
	DPF_FRONT_POW48,
	DPF_FRONT_POW52,
	DPF_FRONT_POW56,
	DPF_FRONT_POW60,
	DPF_FRONT_POW64,

	DPF_FRONT_POW128,
	DPF_FRONT_POW256,
	DPF_FRONT_POW512,
	DPF_FRONT_POW640,
	DPF_FRONT_POW768,
	DPF_FRONT_POW896,
	DPF_FRONT_POW1024,

	// enough of the blinn/phong stuff lets get creative

	// this is a slight power function around 0 (90 degrees out of normal) nice back lit effect with N.E
	DPF_FRESNEL_POW4,
	// similar to above but shifted to around +0.3 and clamped so negative results
	DPF_SHIFTED_FRESNEL_POW8,

};

/* Some attenuation shapes
*/
enum ATTENUATION_FUNCTION
{
	AF_ZERO		=	0,
	AF_QUARTER,
	AF_HALF,
	AF_THREEQUARTER,
	AF_ONE,

	AF_LINEAR,
	AF_SQUARED,
	AF_SHAPE1,
	AF_SHAPE2,
};

/* Material parameters that are NOT evaluated per-pixel (lookup'ed per pixel)
*/
struct Material
{
	float KAmb;
	float KEmm;
	float KSpecCol;	// a blend factor between specular taking surface colour only (metals) or light colour

	Material( float inKAmb, float inKEmm, float inKSpecCol )
	{
		KAmb = inKAmb;
		KEmm = inKEmm;
		KSpecCol = inKSpecCol;
	}
				
};

// default materials
#define MATERIAL_NIL							0
#define MATERIAL_FULL_AMBIENT					1
#define MATERIAL_FULL_EMMISIVE					2
#define MATERIAL_STD_SPECULAR					3
#define MATERIAL_METAL_SPECULAR					4

// D3DX Fill Texture callbacks
extern VOID WINAPI FillDotProductTexture( 	D3DXVECTOR4* pOut, 
									CONST D3DXVECTOR2* pTexCoord,
									CONST D3DXVECTOR2* pTexelSize,
									LPVOID pData );

extern VOID WINAPI FillMaterialTexture(	D3DXVECTOR4* pOut, 
									CONST D3DXVECTOR2* pTexCoord,
									CONST D3DXVECTOR2* pTexelSize,
									LPVOID pData );
extern VOID WINAPI FillAttenuationTexture( 	D3DXVECTOR4* pOut, 
										CONST D3DXVECTOR2* pTexCoord,
										CONST D3DXVECTOR2* pTexelSize,
										LPVOID pData );

// add a new texture to the material list (should be all set before FillMaterialTexture is called)
extern unsigned int RegisterMaterial( const Material& mat );
// clear the material list
extern void EmptyMaterialList();
//get the texture coordinate of the specified material
extern float MaterialIndexToV( unsigned int material );
//get the texture coordinate of the attenuation function
extern float AttenuationIndexToV( enum ATTENUATION_FUNCTION func );
//get the texture coordinate of the dot product function
extern float GetDotProductFunctionU( enum DOTP_FUNCTIONS func );
