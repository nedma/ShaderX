
// Textured mesh rendering (these are tree parts and house parts).
// Fallback for non-MRT cards. Color only part.
//
// Versions: vs1.1, ps1.1

#include "_shared.fx"
#include "_library.fx"

texture		tBase;

// --------------------------------------------------------------------------
//  samplers 

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};

// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float2 uv		: TEXCOORD0;
	float4 color[2]	: COLOR0;
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float3	normal : NORMAL,
	float2  uv : TEXCOORD0 )
{
    VS_OUTPUT o;
	o.pos = mul( pos, mWVP );
	float3 n = mul( normal, (float3x3)mWorld );
	o.uv = uv;
	
	// diffuse directional light
	o.color[0].xyz = gDiffuseDir( n, vLightDir, 0.7 ) + 0.3;
	o.color[0].w = 1;

	// scattering
	float3 extinct, inscatter;
	float3 viewDir = normalize( mul( pos, mWorld ) - vEye );
	float distance = mul( pos, mWorldView ).z;
	gScattering(
		distance, viewDir, vLightDir,
		vScatterBetaR, vScatterBetaM, vScatterBetaSum, vScatterBetaR_, vScatterBetaM_,
		vScatterSunColorDivBetaSum, vScatterHGg,
		extinct, inscatter
	);
	o.color[0].xyz *= extinct * fScatterExtMult;
	o.color[1] = float4( inscatter * fScatterInscatMult, 1 );

	return o;
}


// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float2 uv : TEXCOORD0;
	float4 color[2] : COLOR0;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	return tex2D( smpBase, i.uv ) * i.color[0] + i.color[1];
}


// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_1_1 psMain();
	}
	pass PLast {
	}
}
