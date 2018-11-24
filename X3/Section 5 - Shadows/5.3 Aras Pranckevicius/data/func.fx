
float4x4	mViewProj;
/*
static float4x4 mSphereMap = float4x4(
	0.5,0,  0, 0,
	0,  0.5,0, 0,
	0.5,0.5,1, 0,
	0,  0,  0, 1 );
*/

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
	o.pos = mul( i.pos, mViewProj );
	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

float4 psMain() : COLOR
{
	return float4( 1,1,1, 0.3 );
}

// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass PMain {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_1_1 psMain();
		ZWriteEnable = False;
		CullMode = None;
		ZFunc = Less;
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}
