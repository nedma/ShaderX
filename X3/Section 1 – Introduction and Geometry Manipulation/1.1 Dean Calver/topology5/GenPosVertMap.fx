//--------------------------------------------------------------------------------------
// File: GenPosVertMap.fx
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float3 g_heightfieldScale;
texture g_heightFieldMap;

sampler2D g_heightFieldSampler = 
sampler_state
{
	Texture = <g_heightFieldMap>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;	
};

float4 GenPosVertMap( in float2 inUV : TEXCOORD0,
					  in float2 inPos : VPOS ) : COLOR0
{
	// this is a simple height field expansion there are MUCH more general methods
	float height = tex2D( g_heightFieldSampler, inUV ).r; // use red channel at 0-1 height
	
	float3 pos = float3(inPos.x, -height, inPos.y );

	// a scale of (1.f/HEIGHTFIELD_WIDTH, 1, 1.f/HEIGHTFIELD_HEIGHT) will produce a 1x1x1 cube
	pos = pos * g_heightfieldScale;
	
	return float4( pos, 1 );
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {          
		PixelShader = compile ps_3_0 GenPosVertMap();
    }
}