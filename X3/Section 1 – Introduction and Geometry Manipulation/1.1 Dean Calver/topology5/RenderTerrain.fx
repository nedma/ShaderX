//--------------------------------------------------------------------------------------
// File: RenderTerrain.fx
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float4x4 g_mWorldViewProjection;	// World * View * Projection matrix

float2	g_vertexMapAddrConsts;
texture g_positionVertexMap;
texture g_normalVertexMap;

sampler2D g_positionSampler =
sampler_state
{
    Texture = <g_positionVertexMap>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;	// required for fast 1D to 2D conversion
    AddressV = Clamp;
};

sampler2D g_normalSampler =
sampler_state
{
    Texture = <g_normalVertexMap>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;	// required for fast 1D to 2D conversion
    AddressV = Clamp;
};

// texConsts.x = 1.f/TexWidth
// texConsts.y = (1.f/TexWidth)*(1.f/TexHeight);
float2 GPUConvert1DAddressTo2D( float oneD, float2 texConsts )
{
	return( float2(oneD, oneD) *  texConsts );
}

// converts a 1D to a vertex addressable coordinate (1D to 2D with w = 0)
float4 OneDToVertexTexAddr( float oneD, float2 texConsts  )
{
	return float4(GPUConvert1DAddressTo2D(oneD, texConsts), 0, 0 );
}

struct sVertex
{
	float3 position;
	float3 normal;
};

sVertex getVertexFromVertexMaps( float addr1D )
{
	sVertex data;
		
	data.position = tex2Dlod( g_positionSampler, OneDToVertexTexAddr(addr1D, g_vertexMapAddrConsts) ).xyz;
	data.normal = tex2Dlod( g_normalSampler, OneDToVertexTexAddr(addr1D, g_vertexMapAddrConsts) ).xyz;

	return data;
}

void RenderTerrain( 	in float inVertIndex  	: POSITION0,
						out float4 outPosition		: POSITION,
						out float3 outColour		: COLOR0 )
{
	sVertex vertex =  getVertexFromVertexMaps( inVertIndex );
		
	outPosition = mul( float4(vertex.position,1), g_mWorldViewProjection);
//	outColour = (vertex.normal/2)+0.5;
	outColour = dot( vertex.normal, float3(-0.707,-0.707,0) );
}

float4 DummyPS( in float3 inColour : COLOR0 ) : COLOR0
{
	return float4( inColour,1);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {          
		CULLMODE = NONE;
		VertexShader = compile vs_3_0  RenderTerrain();
		PixelShader = compile ps_3_0 DummyPS();
    }
}
