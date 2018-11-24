#include "library.fx"

float4x4	mViewProj;

float3		vLightPos;
float4		vLightColor;

// Shadow cubemap
texture		tShadow;

sampler smpShadow = sampler_state {
	Texture = <tShadow>;
	MinFilter = Point; MagFilter = Point; MipFilter = None;
};

// --------------------------------------------------------------------------
//  vertex shader

struct VS_INPUT {
	float4 pos : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float4 light : TEXCOORD0; // to light, distance
	float4 color : COLOR0;
};

float4 vsZFill( float4 pos : POSITION ) : POSITION
{
	return mul( pos, mViewProj );
}

VS_OUTPUT vsMain( VS_INPUT i )
{
    VS_OUTPUT o;

	o.pos = mul( i.pos, mViewProj );

	o.light = tolight( i.pos, vLightPos );
	o.color = lighting( i.normal, o.light, vLightColor );

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

float4 psZFill() : COLOR
{
	return 0.0;
}

float4 psMain( VS_OUTPUT i ) : COLOR
{
	float dClose = texCUBE( smpShadow, -i.light.xyz ).r; // d^2 to closest from light
	float dHere = i.light.w*i.light.w; // d^2 to here
	
	float shadowFactor = (dHere*0.9) < dClose ? 1 : 0;

	return i.color * shadowFactor;
}


// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass PZFill {
		VertexShader = compile vs_1_1 vsZFill();
		PixelShader = compile ps_1_1 psZFill();
		ZWriteEnable = True;
		ZFunc = Less;
		AlphaBlendEnable = False;

		FillMode = Solid;

		ColorWriteEnable = 0;
	}
	pass PLight {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_2_0 psMain();

		ZFunc = LessEqual;

		ZWriteEnable = False;

		FillMode = Solid;

		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = One;

		ColorWriteEnable = Red | Green | Blue | Alpha;
	}
	pass PWire {
		VertexShader = compile vs_1_1 vsZFill();
		PixelShader = compile ps_1_1 psZFill();
		ZWriteEnable = False;
		ZFunc = LessEqual;
		AlphaBlendEnable = False;

		FillMode = Wireframe;
	}
}
