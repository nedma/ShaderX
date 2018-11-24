
float4x4	mViewProj;

float3		vLightPos;
float4		vLightColor;

// --------------------------------------------------------------------------
//  vertex shader

struct VS_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 pos	: POSITION;
};

VS_OUTPUT vsMain( VS_INPUT i )
{
    VS_OUTPUT o;

	float4 pos = i.pos;
	pos.xyz += vLightPos;
	o.pos = mul( pos, mViewProj );

	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

float4 psMain() : COLOR
{
	return vLightColor;
}

// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass PMain {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_1_1 psMain();
		ZWriteEnable = True;
		ZFunc = Less;
		AlphaBlendEnable = False;
	}
}
