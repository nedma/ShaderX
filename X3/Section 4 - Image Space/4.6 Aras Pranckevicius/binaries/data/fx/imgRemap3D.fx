
// Generic image space color remapper.
//
// Remaps colors via dependant read into volume texture.
// Does point sampling on volume texture for this demo purposes :)

#include "_shared.fx"

texture		tBase;
texture		tRemap;

// --------------------------------------------------------------------------
//  samplers 

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpRemap = sampler_state {
    Texture   = (tRemap);
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
};


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float2 uv	: TEXCOORD0;
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float2  uv : TEXCOORD0 )
{
    VS_OUTPUT o;
	// fullscreen quad
	o.pos = pos * float4(-2,2,1,1) - vQuadOffset;
	o.uv = uv;
	return o;
}


// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float2 uv : TEXCOORD0;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	float4 color = tex2D( smpBase, i.uv );
	float4 remap = tex3D( smpRemap, color.xyz );
	return remap;
}

// --------------------------------------------------------------------------
//  effect

technique tec0
{
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		//Fvf = Xyz | Normal | Tex1;
		PixelShader = compile ps_1_4 psMain();

		ZEnable = False;
		ZWriteEnable = False;

		//CullMode = None;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
	}
}
