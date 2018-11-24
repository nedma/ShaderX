
// Water rendering.
// Fallback for non-MRT cards. Color only part.
//
// Versions: vs1.1, ps2.0

#include "_shared.fx"

texture		tRefl;
texture		tWaves;


// --------------------------------------------------------------------------
//  samplers 

sampler smpRefl = sampler_state {
    Texture   = (tRefl);
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpWaves = sampler_state {
    Texture   = (tWaves);
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};

// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float4 uv		: TEXCOORD0;
	float2 uvWaves[2] : TEXCOORD1;
};

VS_OUTPUT vsMain (
	float4	pos : POSITION )
{
    VS_OUTPUT o;
	o.pos = mul( pos, mWVP );
	float4 camPos = mul( pos, mWorldView );
	camPos = mul( camPos, mTexProj );
	o.uv = camPos;

	float2 uvWaves = pos.xz * 0.02f;
	o.uvWaves[0] = uvWaves + float2(  fTime * 0.05f,  fTime * 0.01f );
	o.uvWaves[1] = uvWaves + float2( -fTime * 0.01f, -fTime * 0.04f );

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float4 uv		: TEXCOORD0;
	float2 uvWaves[2] : TEXCOORD1;
};

half4 psMain( PS_INPUT i ) : COLOR
{
	float2 uv = i.uv.xy / i.uv.w;
	half2 t0 = tex2D( smpWaves, i.uvWaves[0] ).xy;
	half2 t1 = tex2D( smpWaves, i.uvWaves[1] ).xy;
	half2 t = (t0 + t1) * 0.01;
	return tex2D( smpRefl, uv+t );
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
