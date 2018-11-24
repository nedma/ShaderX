
// Cloud rendering.
// Fallback for non-MRT cards. Color only part.
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
	float4	pos : POSITION,
	float3	normal : NORMAL )
{
    VS_OUTPUT o;
	o.pos = mul( pos, mWVP );
	float3 n = mul( normal, (float3x3)mWorld );
	
	// slightly over-edge directional light
	float cosang = ( dot( n, vLightDir ) + 1.0 ) * 0.5;
	o.color.xyz = cosang * vScatterSunColor * 0.5 + 0.5;
	o.color.w = 1;

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

float4 psMain( float4 color : COLOR0 ) : COLOR
{
	return color;
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
