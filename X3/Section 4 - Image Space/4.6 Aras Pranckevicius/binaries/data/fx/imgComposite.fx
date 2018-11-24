
// Final image space compositor.
//
// Lays edge-detected/hatched image over a distorted-lower-res image.
//
// Versions: vs1.1, ps1.1

#include "_shared.fx"

texture		tBase;
texture		tBleed;
texture		tEdges;


// --------------------------------------------------------------------------
//  samplers 

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpBleed = sampler_state {
    Texture   = (tBleed);
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpEdges = sampler_state {
    Texture   = (tEdges);
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};

// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float2 uv[2]	: TEXCOORD0; // base, bleed
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float2  uv : TEXCOORD0 )
{
    VS_OUTPUT o;
	o.pos = pos * float4(-2,2,1,1) - vQuadOffset;
	o.uv[0] = uv;
	o.uv[1] = uv;
	return o;
}


// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float2 uv[2] : TEXCOORD0;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	half4 cedges = 1.0 - tex2D( smpEdges, i.uv[0] ) * 0.8;
	half4 cbleed = tex2D( smpBleed, i.uv[1] );
	return cbleed * cedges;
}

// --------------------------------------------------------------------------
//  effect

technique tec0
{
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		//Fvf = Xyz | Normal | Tex1;
		PixelShader = compile ps_1_1 psMain();

		ZEnable = False;
		ZWriteEnable = False;

		//CullMode = None;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
	}
}
