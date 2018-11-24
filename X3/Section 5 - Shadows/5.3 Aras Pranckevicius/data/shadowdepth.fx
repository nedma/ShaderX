
float4x4	mViewProj;
float3		vLightPos;

// --------------------------------------------------------------------------
//  vertex shader

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float3 light : TEXCOORD0; // to light
};

VS_OUTPUT vsMain( float4 pos : POSITION )
{
	VS_OUTPUT o;
	o.pos = mul( pos, mViewProj );
	o.light = vLightPos - pos;
	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

float4 psMain( VS_OUTPUT i ) : COLOR
{
	return dot( i.light, i.light );
}

// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass PMain {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_2_0 psMain(); // floating point - hence ps 2.0
		CullMode = CCW;
		ZWriteEnable = True;
		ZEnable = True;
		AlphaBlendEnable = False;
		FillMode = Solid;
	}
}
