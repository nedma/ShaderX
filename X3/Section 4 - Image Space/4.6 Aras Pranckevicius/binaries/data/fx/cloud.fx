
// Cloud rendering.
//
// Outputs color into RT0, normal/Z into RT1.
// Clouds aren't affected by atmospheric scattering here, and are over-edge
// diffuse shaded.
//
// Versions: vs1.1, ps2.0

#include "_shared.fx"
#include "_library.fx"


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float4 normalZ	: TEXCOORD0;
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

	// normal/Z
	o.normalZ = gNormalZ( o.pos, n );

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float4 normalZ : TEXCOORD0;
	float4 color : COLOR0;
};

struct PS_OUTPUT {
	half4 color : COLOR0;
	half4 normalZ : COLOR1;
};

PS_OUTPUT psMain( PS_INPUT i ) : COLOR
{
	PS_OUTPUT o;
	// color
	o.color = i.color;

	// normal/Z
	o.normalZ = float4( i.normalZ.xyz, i.normalZ.w );

	return o;
}


// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_2_0 psMain();
	}
	pass PLast {
	}
}
technique tecDummy {
	pass P0 {
	}
}
