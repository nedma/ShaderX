//--------------------------------------------------------------------------------------
// File: Topology1.fx
//
// Basic implementation of topology maps, replicated tradional indexed triangles
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float	 g_fTime;					// App's time in seconds
float4x4 g_mWorld;					// World matrix for object
float4x4 g_mWorldViewProjection;	// World * View * Projection matrix
float g_recipTexWidth;				// size of texture for 1D to 2D address conversion
float g_recipTexHeight;				// size of texture for 1D to 2D address conversion

texture g_positionVertexMap;
texture g_normalVertexMap;
texture g_uvVertexMap;

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

sampler2D g_uvSampler =
sampler_state
{
    Texture = <g_uvVertexMap>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;	// required for fast 1D to 2D conversion
    AddressV = Clamp;
};


float2 GPUConvert1DAddressTo2D( float oneD )
{
	oneD = oneD * g_recipTexWidth;
	
	return( float2(oneD, oneD * g_recipTexHeight) );
}

// converts a 1D to a vertex addressable coordinate (1D to 2D with w = 0)
float4 OneDToVertexTexAddr( float oneD )
{
	return float4(GPUConvert1DAddressTo2D(oneD), 0, 0 );
}

struct sVertex
{
	float3 position;
	float3 normal;
	float2 uv;
};

sVertex getVertexFromVertexMaps( float3 primData, float vertIndicator )
{
	sVertex data;
	float addr1D = 0;
	
	if( vertIndicator < 0.5f ) // vertIndicator == 0
	{
		addr1D = primData.x;
	} else if( vertIndicator < 1.5f ) // vertIndicator == 1
	{
		addr1D = primData.y;
	} else if( vertIndicator < 2.5f ) // vertIndicator == 2
	{
		addr1D = primData.z;
	}
	
	data.position = tex2Dlod( g_positionSampler, OneDToVertexTexAddr(addr1D) ).xyz;
	data.normal = tex2Dlod( g_normalSampler, OneDToVertexTexAddr(addr1D) ).xyz;
	data.uv = tex2Dlod( g_uvSampler, OneDToVertexTexAddr(addr1D) ).xy ;

	return data;
}

void IndexedTriTopology( 		in float3 inPrimData 		: POSITION1,
								in float inVertIndiactor  	: POSITION0,
								out float4 outPosition		: POSITION,
								out float3 outColour		: COLOR0,
								out float2 outUV			: TEXCOORD0 )
{
	sVertex vertex =  getVertexFromVertexMaps( inPrimData, inVertIndiactor );
	
	outPosition = mul( float4(vertex.position,1), g_mWorldViewProjection);
	outColour = (vertex.normal/2)+0.5;
	outUV = vertex.uv;

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
	VertexShader = compile vs_3_0  IndexedTriTopology();
	PixelShader = compile ps_3_0 DummyPS();
    }
}
