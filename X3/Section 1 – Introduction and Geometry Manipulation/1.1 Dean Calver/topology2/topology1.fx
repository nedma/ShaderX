//--------------------------------------------------------------------------------------
// File: Topology2.fx
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float	 g_fTime;					// App's time in seconds
float4x4 g_mWorld;					// World matrix for object
float4x4 g_mWorldViewProjection;	// World * View * Projection matrix
float g_recipTexWidth;				// size of texture for 1D to 2D address conversion
float g_recipTexHeight;				// size of texture for 1D to 2D address conversion

float2	g_vertexMapAddrConsts;
texture g_positionVertexMap;
texture g_normalVertexMap;
texture g_uvVertexMap;
texture g_faceTopoMap;

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

float2	g_topoMapAddrConsts;
sampler2D g_topoMapSampler = 
sampler_state
{
	Texture = <g_faceTopoMap>;
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
	
	data.position = tex2Dlod( g_positionSampler, OneDToVertexTexAddr(addr1D, g_vertexMapAddrConsts) ).xyz;
// In this example we don't get the vertex normal as we replace it with face normal
//	data.normal = tex2Dlod( g_normalSampler, OneDToVertexTexAddr(addr1D, g_vertexMapAddrConsts) ).xyz;
	data.normal = 0;
	data.uv = tex2Dlod( g_uvSampler, OneDToVertexTexAddr(addr1D, g_vertexMapAddrConsts) ).xy ;

	return data;
}

struct sTriangleFace
{
	float3 vertexIndex; 
};

sTriangleFace getFaceFromTopoMap( float addr1D )
{
	sTriangleFace data;
	data.vertexIndex = tex2Dlod( g_topoMapSampler, OneDToVertexTexAddr(addr1D, g_topoMapAddrConsts) ).xyz;
	
	return data;
}

float3 CalcFaceNormal( sTriangleFace face )
{
   float3 p0 = tex2Dlod( g_positionSampler, OneDToVertexTexAddr(face.vertexIndex.x, g_vertexMapAddrConsts) ).xyz;
   float3 p1 = tex2Dlod( g_positionSampler, OneDToVertexTexAddr(face.vertexIndex.y, g_vertexMapAddrConsts) ).xyz;
   float3 p2 = tex2Dlod( g_positionSampler, OneDToVertexTexAddr(face.vertexIndex.z, g_vertexMapAddrConsts) ).xyz;
   
   // calc face normal
   float3 v0 = p1 - p0;
   float3 v1 = p2 - p0;
   float3 n = cross( v0, v1 );
   return( normalize(n) ); 

}

void IndexedTriTopology( 		in float1 inCounter 		: POSITION1,
								in float inVertIndiactor  	: POSITION0,
								out float4 outPosition		: POSITION,
								out float3 outColour		: COLOR0,
								out float2 outUV			: TEXCOORD0 )
{
	sTriangleFace face = getFaceFromTopoMap( inCounter );
	sVertex vertex =  getVertexFromVertexMaps( face.vertexIndex, inVertIndiactor );
	
	outPosition = mul( float4(vertex.position,1), g_mWorldViewProjection);
	// override vertex normal with face normal
	vertex.normal = CalcFaceNormal( face );
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
