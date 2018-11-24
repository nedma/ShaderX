// global functions
#ifndef __LIBRARY_FX
#define __LIBRARY_FX

#include "_shared.fx"

// --------------------------------------------------------------------------
//  Small common functions

static inline float3 gDiffuseDir(
	float3 normal,		// normal
	float3 lightDir,	// negated
	float3 matDiffuse )
{
	return max( 0, dot( normal, lightDir ) ) * matDiffuse;
}


// --------------------------------------------------------------------------
//  Compute atmospheric scattering (Hoffman et al., SigGraph2002)

// according to paper, HGg is (g, (1-g)^2, 1+g*g, 2*g)
// according to demo,  HGg is (g, 1-g*g, 1+g, 2*g)
static inline void gScattering(
	float distance,	// distance
	float3 viewDir,	// normalized view direction
	float3 sunDir,	// normalized sun direction
	float3 betaR,	// Rayleigh scattering coeffs
	float3 betaM,	// Mie scattering coeffs
	float3 betaSum, // betaR+betaM
	float3 betaR_,	// betaR * 3/(16*PI)
	float3 betaM_,	// betaM * 1/(4*PI)
	float3 sunColorDivBetaSum, // sun color / betaSum
	float4 HGg,		// g value in Henyey Greenstein's fn: (g, 1-g*g, 1+g, 2*g)
	out float3 extinct,
	out float3 inscatter )
{
	// cos(-angle(V,L))
	float cosTheta = dot( -viewDir, sunDir );

	// extinction
	extinct = exp( -betaSum * distance );

	// inscattering
	float3 phaseR = betaR_ * (1.0 + cosTheta*cosTheta);
	float mul = HGg.z + HGg.w*cosTheta;
	mul = rsqrt( mul );
	mul = mul*mul*mul * HGg.y;
	float3 phaseM = betaM_ * mul;
	inscatter = (phaseR + phaseM) * sunColorDivBetaSum * (1 - extinct);
}


// --------------------------------------------------------------------------
//  Normal/Z output

// Outputs normal in RGB (scaled into 0..1 range), sort-of-depth in A.
// Input: pos - Final (clip space) position
// Input: normal - World space normal
static inline float4 gNormalZ( float4 pos, float3 normal )
{
	float4 o;
	o.xyz = normal * 0.5 + 0.5;
	o.w = 100.0 / (pos.w + 100.0); // kind of depth value :)
	//o.xyz = o.w;
	return o;
}

// --------------------------------------------------------------------------
//  Fallback Normal/Z for non-MRT cards

struct VS_OUTPUT_NZ_NOMRT {
	float4 pos		: POSITION;
	float3 normal	: TEXCOORD0;
	float  z		: TEXCOORD1;
};

VS_OUTPUT_NZ_NOMRT vsNormalZNoMRT (
	float4	pos : POSITION,
	float3	normal : NORMAL )
{
    VS_OUTPUT_NZ_NOMRT o;
	o.pos = mul( pos, mWVP );
	float4 nz = gNormalZ( o.pos, mul( normal, (float3x3)mWorld ) );
	o.normal = nz.xyz;
	o.z = nz.w;
	return o;
}

float4 psNormalZNoMRT( float3 normalZ : TEXCOORD0, float z : TEXCOORD1 ) : COLOR
{
	return float4( normalZ, z );
}

shared vertexshader vshNormalZNoMRT = compile vs_1_1 vsNormalZNoMRT();
shared pixelshader pshNormalZNoMRT = compile ps_1_1 psNormalZNoMRT();


#endif
