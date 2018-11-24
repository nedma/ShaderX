
// ******** plus: HSV->RGB and back here

// HSV colorspace image space distorter.
//
// Here:
//	- sample image at normal position, convert to HSV
//	- sample the same image at two distorted positions (EMBM-like, via two offset textures).
//	  The offset texture coords are controlled from code a bit, so that it doesn't
//	  look totally ugly in motion :)
//	- Use a blend of distorted samples if hue or saturation or value differences
//	  between them exceed some thresholds. This way we get distortion along
//	  "noticeable" color boundaries only.
//  - In any case, keep the original hue channel undistorted.
//
// Versions: vs1.1, ps2.0

#include "_shared.fx"

texture		tBase;
texture		tBleedB;
texture		tBleedC;
texture		tRGB2HSV;
texture		tHSV2RGB;

float		fOffsetX;
float		fOffsetY;

// --------------------------------------------------------------------------
//  samplers 

sampler2D smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};
sampler2D smpBleedB = sampler_state {
    Texture   = (tBleedB);
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};
sampler2D smpBleedC = sampler_state {
    Texture   = (tBleedC);
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};
sampler3D smpRGB2HSV = sampler_state {
    Texture   = (tRGB2HSV);
    MipFilter = Point; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
};
sampler3D smpHSV2RGB = sampler_state {
    Texture   = (tHSV2RGB);
    MipFilter = Point; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
};


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos		: POSITION;
	float2 uv[2]	: TEXCOORD0; // center, bleed
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float2  uv : TEXCOORD0 )
{
    VS_OUTPUT o;
	o.pos = pos * float4(-2,2,1,1) - vQuadOffset;
	o.uv[0] = uv;
	o.uv[1] = uv + float2( fOffsetX, fOffsetY );
	return o;
}


// --------------------------------------------------------------------------
//  pixel shader

struct PS_INPUT {
	float2 uv[2] : TEXCOORD0;
};

float4 psMain( PS_INPUT i ) : COLOR
{
	// sample RGB pixel, convert into HSV
	half3 base = tex2D( smpBase, i.uv[0] ).rgb;
	base = tex3D( smpRGB2HSV, base ).rgb;

	// get two displaced sample locations
	half2 bleedB = tex2D( smpBleedB, i.uv[1] ).rg*2-1;
	half2 bleedC = tex2D( smpBleedC, i.uv[1] ).rg*2-1;
	float2 uvB = i.uv[0] + bleedB * (8.0/512);
	float2 uvC = i.uv[0] + bleedC * (-7.0/512);

	// sample base at displaced locations, convert to HSV
	half3 baseB = tex2D( smpBase, uvB ).rgb;
	baseB = tex3D( smpRGB2HSV, baseB );
	half3 baseC = tex2D( smpBase, uvC ).rgb;
	baseC = tex3D( smpRGB2HSV, baseC );
	half3 bleed = baseB*0.5 + baseC*0.5;

	// get difference in displaced locations' HSV values
	// final color is base if differences in all HSV values are smaller than
	// tresholds; else average of displaced values
	half3 diff = abs(baseB - baseC) - half3( 1.0/8.0, 1.0/3.0, 1.0/3.0 );
	half3 final = all( diff < float3(0,0,0) ) ? base : bleed;

	final.r = base.r;

	return tex3D( smpHSV2RGB, final );
}

// --------------------------------------------------------------------------
//  effect

technique tec0
{
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		//Fvf = Xyz | Normal | Tex1;
		PixelShader = compile ps_2_0 psMain();

		ZEnable = False;
		ZWriteEnable = False;

		//CullMode = None;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
	}
}
