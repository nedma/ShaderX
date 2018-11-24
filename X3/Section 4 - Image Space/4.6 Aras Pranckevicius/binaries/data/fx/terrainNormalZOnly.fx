
// Terrain rendering.
// Fallback for non-MRT cards. Normal/Z only part.
//
// Versions: vs1.1, ps2.0

#include "_shared.fx"
#include "_library.fx"

texture		tShadowMap;


// --------------------------------------------------------------------------
//  samplers 

sampler smpShadowMap = sampler_state {
    Texture   = (tShadowMap);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float2 uvShadow[4] : TEXCOORD0; // RGAA shadow map taps
	float4 normalZ : TEXCOORD4; // normal (RGB), depth (A)
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float3	normal : NORMAL )
{
    VS_OUTPUT o;

	normal = normal*2-1;

	// final pos
	o.pos = mul( pos, mViewProj );

	// shadow map coords for 4xRGAA
	o.uvShadow[0] = mul( pos, mShadowProj0 );
	o.uvShadow[1] = mul( pos, mShadowProj1 );
	o.uvShadow[2] = mul( pos, mShadowProj2 );
	o.uvShadow[3] = mul( pos, mShadowProj3 );

	// normal/Z
	o.normalZ = gNormalZ( o.pos, normal );

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float2 uvShadow[4] : TEXCOORD0; // shadow map taps
	half4 normalZ : TEXCOORD4;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	// 4xRGAA shadow map
	half4 shadowTaps;
	shadowTaps.x = tex2D( smpShadowMap, i.uvShadow[0] ).a;
	shadowTaps.y = tex2D( smpShadowMap, i.uvShadow[1] ).a;
	shadowTaps.z = tex2D( smpShadowMap, i.uvShadow[2] ).a;
	shadowTaps.w = tex2D( smpShadowMap, i.uvShadow[3] ).a;
	half shadow = dot( shadowTaps, 0.25 );

	// normal/Z
	half3 norm = i.normalZ.xyz;
	half z = i.normalZ.w;
	if( shadow < 0.75 ) // change depth in shadow
		z = 1.0;

	return half4( norm, z );
}


// --------------------------------------------------------------------------
//  effect

technique tecPS20 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_2_0 psMain();
	}
	pass PLast {
	}
}
