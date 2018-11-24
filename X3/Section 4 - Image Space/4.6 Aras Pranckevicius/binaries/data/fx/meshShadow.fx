
// Rendering into shadow map.
//
// Just const color.
//
// Versions: vs1.1, ps1.1

#include "_shared.fx"


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
};

VS_OUTPUT vsMain (
	float4	pos : POSITION )
{
    VS_OUTPUT o;
	o.pos = mul( pos, mWVP );
	return o;
}


// --------------------------------------------------------------------------
//  pixel shader

float4 psMain() : COLOR
{
	return float4( 0, 0, 0, 0.6 );
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
