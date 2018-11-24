// A light shader that performs hemisphere lighting, hemisphere lighting is better than ambient
// and slightly more expensive so why bother supporting ambient?

#include "SharedLightPixel.fx"

// sky and ground colour of the hemisphere light
half3 f3SkyColour			= half3(0,0,0.5);					// sky colour up the screen
half3 f3GroundColour		= half3(0,0.5,0);					// ground colour down the screen

// output from the pixel shader is back buffer
struct PS_OUT
{
	half4 Colour : COLOR0;
};

PS_OUT LightHemisphere( float2 uv : TEXCOORD0 )
{
	PS_OUT outp;

	// lookup framebuffer at the pixel
	float4 packNormal = tex2D( NormalBuffer, uv );
	float4 packCs0 = tex2D( Cs0Buffer, uv );
	float4 packCs1 = tex2D( Cs1Buffer, uv );
	
	// unpack parameters from framebuffer	
	half3 Cs0, Cs1;
	half MatIndx;
	// unpack position and normal into view space
	half3 N = UnPackNormalFromFatFramebuffer( packNormal.xyz );
	Cs0 = packCs0;
	Cs1 = packCs1;
	MatIndx = packNormal.w;

	// dot the normal with the up vector for hemisphere
	half hemi_dot = dot( N, f3WorldUpInViewSpace );

	// could use real solution? (requires an if) so I'm using Chas Boyd's version
	half alpha = 0.5 + 0.5 * hemi_dot;

	// lookup materials for ambient and emmisive coefficent
	half4 data;
	LookupMaterialData( MatIndx, data );
	
	half3 hemi_colour = lerp( f3GroundColour, f3SkyColour, alpha);
	
	// add hemi (ambient) * the ambient coefficent + 
	// add in Cs1 * the emmisive coefficent
	half3 final_colour = (hemi_colour * data.z) +  (Cs1 * data.w);
	
	outp.Colour = half4( final_colour, 1 );

	return outp;
}

// techniques that compile and use the approiate pixel shader
technique T0
{
	pass P0
	{
		PixelShader = compile ps_2_0 LightHemisphere();
		ZEnable = false;
	}
}