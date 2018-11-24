// an effect that shows/combines each buffer in the fat framebuffer

#include "SharedLightPixel.fx"



float4 DisplayBuffer0( float2 uv : TEXCOORD0 ): COLOR0
{
	return tex2D( Buffer0, uv );
}

float4 DisplayBuffer1( float2 uv : TEXCOORD0 ): COLOR0
{
	return tex2D( Buffer1, uv ) + float4(1,0,0,0);
}

float4 DisplayBuffer2( float2 uv : TEXCOORD0 ): COLOR0
{
	return tex2D( Buffer2, uv );
}

float4 DisplayBuffer3( float2 uv : TEXCOORD0 ): COLOR0
{
	return tex2D( Buffer3, uv );
}

float4 DisplayUnpackedPosition( float2 uv : TEXCOORD0 ): COLOR0
{
	return abs(UnPackPositionFromFatFramebuffer( tex2D( Buffer0, uv ) ) / 5);
}

float4 ShowShadowMap( float2 uv : TEXCOORD0 ): COLOR
{
	return tex2D( GlobalShadowMap, uv	) * 3.f;
}

float4 CombineAverage( float2 uv : TEXCOORD0 ): COLOR0
{
	return (tex2D( Buffer0, uv ) + tex2D(Buffer2,uv))/2 + (tex2D( Buffer1, uv ) + tex2D( Buffer3, uv))/2;
}


// techniques that compile and use the approiate pixel shader
technique T0
{
	pass P0
	{
		PixelShader = compile ps_2_0 DisplayBuffer0();
	}
}
technique T1
{
	pass P0
	{
		PixelShader = compile ps_2_0 DisplayBuffer1();
	}
}
technique T2
{
	pass P0
	{
		PixelShader = compile ps_2_0 DisplayBuffer2();
	}
}
technique T3
{
	pass P0
	{
		PixelShader = compile ps_2_0 DisplayBuffer3();
	}
}
technique T4
{
	pass P0
	{
		PixelShader = compile ps_2_0 DisplayUnpackedPosition();
	}
}
technique T5
{
	pass P0
	{
		PixelShader = compile ps_2_0 ShowShadowMap();
	}
}
technique T6
{
	pass P0
	{
		PixelShader = compile ps_2_0 CombineAverage();
	}
}