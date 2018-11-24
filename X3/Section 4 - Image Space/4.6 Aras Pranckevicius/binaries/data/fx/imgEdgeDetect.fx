
// Image space edge-detector and (some form of) hatcher.
//
// Here:
//	- detect edges from normal or Z differences.
//	- add hatch-like texture in shaded regions (based on dot normal with light).
//
// Versions: vs1.1, ps2.0+ps1.4

// NOTE that it seems to fit into one pass ps_2_0 shader, but either there's a bug
// in current HLSL compiler, or a bug in my videocard's driver (didn't have
// time to check HLSL output...). Hence we do two passed here - one for edges-from-Z,
// another for edges-from-normals and hatching.

#include "_shared.fx"

texture		tBase;
texture		tHatch;


// --------------------------------------------------------------------------
//  samplers 

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpHatch = sampler_state {
    Texture   = (tHatch);
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    AddressU = Wrap; AddressV = Wrap;
};


// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float2 uv[3]	: TEXCOORD0; // base, taps[2]
};

VS_OUTPUT vsMain (
	float4	pos : POSITION,
	float2  uv : TEXCOORD0 )
{
    VS_OUTPUT o;
	o.pos = pos * float4(-2,2,1,1) - vQuadOffset;
	o.uv[0] = uv;
	o.uv[1] = uv + float2(  vQuadOffset.x*2, 0 );
	o.uv[2] = uv + float2( 0,  vQuadOffset.y*2 );
	return o;
}


// --------------------------------------------------------------------------
//  pixel shader

half4 psMain( VS_OUTPUT i ) : COLOR
{
	// sample center and 2 neighbours
	half4 cbase = tex2D( smpBase, i.uv[0] );
	half4 cb1 = tex2D( smpBase, i.uv[1] );
	half4 cb3 = tex2D( smpBase, i.uv[2] );
	
	// normals into -1..1 range
	half3 nbase = cbase.xyz*2-1;
	half3 nb1 = cb1.xyz*2-1;
	half3 nb3 = cb3.xyz*2-1;

	// edges from normals
	half2 ndiff;
	ndiff.x = dot( nbase, nb1 );
	ndiff.y = dot( nbase, nb3 );
	ndiff -= 0.6;
	ndiff = ndiff > half2(0,0) ? half2(0,0) : half2(1,1);
	half ndiff1 = ndiff.x + ndiff.y;

	// sample hatch
	half4 chatch = tex2D( smpHatch, i.uv[0] );
	// dot normal with light
	half dotNL = dot( nbase, vLightDir );
	// hatch blend factor
	half factor = saturate( (1.0 - 0.9 - dotNL) * 2 );
	chatch *= factor;

	// edges from Z
	float2 zdiff;
	zdiff.x = cbase.a - cb1.a;
	zdiff.y = cbase.a - cb3.a;
	zdiff = abs( zdiff ) - 0.02;
	zdiff = zdiff > half2(0,0) ? half2(1,1) : half2(0,0);

	return chatch + ndiff1 + dot(zdiff,half2(1,1));
}

/*
float4 psMain2( VS_OUTPUT i ) : COLOR
{
	// sample center and 2 neighbours
	float4 cbase = tex2D( smpBase, i.uv[0] );
	float4 cb1 = tex2D( smpBase, i.uv[1] );
	float4 cb3 = tex2D( smpBase, i.uv[2] );
	
	// edges from Z
	float2 zdiff;
	zdiff.x = cbase.a - cb1.a;
	zdiff.y = cbase.a - cb3.a;
	zdiff = abs( zdiff ) - 0.02;
	zdiff = zdiff > float2(0,0) ? float2(1,1) : float2(0,0);

	return dot( zdiff, float2(1,1) );
}
*/

// --------------------------------------------------------------------------
//  effect

technique tec0
{
	// edges from normals + hatch
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_2_0 psMain();

		ZEnable = False;
		ZWriteEnable = False;
	}
	// edges from Z
	/*
	pass P1 {
		PixelShader = compile ps_1_4 psMain2();
		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = One;
	}
	*/
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		AlphaBlendEnable = False;
	}
}
