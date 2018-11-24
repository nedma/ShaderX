
// Terrain rendering.
// Fallback for non-MRT cards. Color only part.
//
// Versions: vs1.1, ps2.0

#include "_shared.fx"
#include "_library.fx"

texture		tBase0;
texture		tBase1;
texture		tBase2;
texture		tNoise0;
texture		tShadowMap;

// --------------------------------------------------------------------------
//  samplers 

sampler smpBase0 = sampler_state {
    Texture   = (tBase0);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};
sampler smpBase1 = sampler_state {
    Texture   = (tBase1);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};
sampler smpBase2 = sampler_state {
    Texture   = (tBase2);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};
sampler smpNoise0 = sampler_state {
    Texture   = (tNoise0);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};
sampler smpShadowMap = sampler_state {
    Texture   = (tShadowMap);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float2 uvTex[3]	: TEXCOORD0; // detail0, detail1, noise
	float2 uvShadow[4] : TEXCOORD3; // RGAA shadow map taps
	float4 color[2]	: COLOR0;
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float4	normal4 : NORMAL )
{
    VS_OUTPUT o;

	float3 normal = normal4.xyz*2-1;
	//pos.xyz += normal*5;

	// final pos
	o.pos = mul( pos, mViewProj );

	// detail/noise texture coords
	float2 uv0 = pos.xz / 32.0f;
	float2 uv1 = pos.xz / float2(512.0f,-512.0f) - 0.5f;
	o.uvTex[0] = uv0 * 7;
	o.uvTex[1] = uv0 * 6;
	o.uvTex[2] = uv1;

	// shadow map coords for 4xRGAA
	o.uvShadow[0] = mul( pos, mShadowProj0 );
	o.uvShadow[1] = mul( pos, mShadowProj1 );
	o.uvShadow[2] = mul( pos, mShadowProj2 );
	o.uvShadow[3] = mul( pos, mShadowProj3 );

	// diffuse directional light
	o.color[0].xyz = gDiffuseDir( normal, vLightDir, 0.8 ) + 0.2;
	o.color[0].w = 1;

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
	o.color[0].xyz *= extinct * fScatterExtMult;
	o.color[1] = float4( inscatter * fScatterInscatMult, 1 );

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float2 uvTex[3] : TEXCOORD0; // detail0, detail1, noise
	float2 uvShadow[4] : TEXCOORD3; // shadow map taps
	half4 color[2] : COLOR0;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	// detail textures
	half4 c0 = tex2D( smpBase0, i.uvTex[0] ); // grass1
	half4 c1 = tex2D( smpBase1, i.uvTex[1] ); // grass2
	half4 c2 = tex2D( smpBase2, i.uvTex[1] ); // road
	// noise
	half4 noiz = tex2D( smpNoise0, i.uvTex[2] );
	// get a final "grass" blend - lerp 0 and 1 with noise r
	half4 f01 = lerp(c0,c1,noiz.r);
	// blend final "grass" with "road" - lerp 0/1 and 2 with noise g
	half4 f = lerp(c2,f01,noiz.g);

	// 4xRGAA shadow map
	half4 shadowTaps;
	shadowTaps.x = tex2D( smpShadowMap, i.uvShadow[0] ).a;
	shadowTaps.y = tex2D( smpShadowMap, i.uvShadow[1] ).a;
	shadowTaps.z = tex2D( smpShadowMap, i.uvShadow[2] ).a;
	shadowTaps.w = tex2D( smpShadowMap, i.uvShadow[3] ).a;
	half shadow = dot( shadowTaps, 0.25 );

	// color
	return (f*shadow) * i.color[0] + i.color[1];
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
