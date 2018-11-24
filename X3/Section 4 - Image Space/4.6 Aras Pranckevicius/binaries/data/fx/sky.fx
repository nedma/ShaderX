
// Skydome rendering.
//
// Outputs color into RT0, nothing into RT1.
// We don't want to edge-detect or hatch the skydome, hence nothing into RT1,
// RT1 gets cleared in code to max Z and normal that coincides with light.
// Sky is affected by atmospheric in-scattering only.
//
// Versions: vs1.1, ps1.1

#include "_shared.fx"
#include "_library.fx"

// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float4 color	: COLOR0;
};

VS_OUTPUT vsMain (
	float4	pos : POSITION )
{
    VS_OUTPUT o;
	o.pos = mul( pos, mViewProj );

	// scattering
	float3 extinct, inscatter;
	float3 viewDir = normalize( pos - vEye );
	float distance = mul( pos, mView ).z;
	gScattering(
		distance, viewDir, vLightDir,
		vScatterBetaR, vScatterBetaM, vScatterBetaSum, vScatterBetaR_, vScatterBetaM_,
		vScatterSunColorDivBetaSum, vScatterHGg,
		extinct, inscatter
	);
	o.color = float4( inscatter * fScatterInscatMult, 1 );
	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float4 color : COLOR0;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	return i.color;
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
