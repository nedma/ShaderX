// a light shader that performs basic directional lighting
// no materials yet but has a fixed specular exponent
#include "SharedLightPixel.fx"

// we have 512 materials so out integer material number are actually x / 512 in uv coordinates
#define MATERIAL_NUM(x) ((x) / 512.f) + 1e-7f

// sky and ground colour of the hemisphere light
half3 f3LightColour		= float3(0.5,0.5,0.5);					// colour of the light
half3 f3LightDirection		= float3(-0.707,0.707,0);				// direction of the light

float4x4 matShadowWarp;				// matrix that that a position from view space to shadow space
texture ShadowMapTexture;
sampler ShadowMap = sampler_state 
{
	Texture = (ShadowMapTexture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

// output from the pixel shader is back buffer
struct PS_OUT
{
	half4 Colour : COLOR0;
};

PS_OUT LightDirectional( float2 uv : TEXCOORD0 )
{
	PS_OUT outp;

	// lookup framebuffer at the pixel
	float4 packPos = tex2D( PositionBuffer, uv );
	float4 packNormal = tex2D( NormalBuffer, uv );
	float4 packCs0 = tex2D( Cs0Buffer, uv );
	float4 packCs1 = tex2D( Cs1Buffer, uv );
	
	// unpack parameters from framebuffer	
	half MatIndx;
	// unpack position and normal into view space
	float3 posInView = UnPackPositionFromFatFramebuffer( packPos.xyz );
	half3 N = UnPackNormalFromFatFramebuffer( packNormal.xyz );
	MatIndx = packNormal.w;

	half shadow;	// multiple colour by shadow (0 when in shadow)
	float4 dist;
	float4 posInShadow = mul( float4(posInView,1), matShadowWarp );
	float distInShadow = posInShadow.z / posInShadow.w;
	
	

	// get the 4 jittered distances as a vector
	dist.x = tex2Dproj( ShadowMap, posInShadow + f4ShadowJitter0 ); // do projection in texture load
	dist.y = tex2Dproj( ShadowMap, posInShadow + f4ShadowJitter1 ); // do projection in texture load
	dist.z = tex2Dproj( ShadowMap, posInShadow + f4ShadowJitter2 ); // do projection in texture load
	dist.w = tex2Dproj( ShadowMap, posInShadow + f4ShadowJitter3 ); // do projection in texture load
	
	// < is a vector op under HLSL preduces a boolean result vector
	dist.xyzw = (distInShadow < dist.xyzw);

	// dist.xyzw each component contains 1 not in shadow, acculamate and scale shadow value
	shadow = dot(dist,dist) * 0.25f;
	
	// dynamic pixel shader branch means we can't use tex2Dproj, so we do them and just not use them
	// we select which which material recieves shadow
	if( MatIndx < MATERIAL_NUM(0) )
	{
		// normal shadow
	} else if( MatIndx < MATERIAL_NUM(1) )
	{
		// material 1 is the floor material, we do a weird things based wether we are in out out of shadow
		packCs0 = lerp( float4(1,0,0,0.1f), float4(1,1,1,0.1f), shadow );
		packCs1 = lerp( float4(1,0,0,0.1f), float4(1,1,1,0), shadow );
		// now remove the shadow
		shadow = 1;
	} else if( MatIndx < MATERIAL_NUM(4) )
	{
		// material 0 - 4 recieve shadow
	} else if( MatIndx < MATERIAL_NUM(5) )
	{
		// sharpen the shadow edge
		shadow = shadow * shadow;
	} else
	{
		// all others use shadow normally
	}

	
	// camera vector
	half3 I = normalize( -posInView.xyz );

	// recieve colour from the illumate function
	half3 Col;
	
	Illuminate( N, I, f3LightDirection, packCs0, packCs1, f3LightColour, MatIndx, Col );
	
	half3 finalColour =  Col * shadow;

	outp.Colour = half4( finalColour, 1 );
	return outp;	
}

// techniques that compile and use the approiate pixel shader
technique T0
{
	pass P0
	{
		PixelShader = compile ps_2_0 LightDirectional();
		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaRef = 0;
		SrcBlend = One;
		DestBlend = One;
		ZEnable = false;
	}
}