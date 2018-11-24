//--------------------------------------------------------------------------------------
// 
// File: Topology2.fx
// Calculates a face normal in a pixel shader using a topology map
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float2	g_vertexMapAddrConsts;
texture g_positionVertexMap;
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


texture g_faceTopoMap;
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

struct sTriangleFace
{
	float3 vertexIndex;
};

float3 CalcFaceNormal( sTriangleFace face )
{
   float3 p0 = tex2D( g_positionSampler, GPUConvert1DAddressTo2D(face.vertexIndex.x, g_vertexMapAddrConsts) ).xyz;
   float3 p1 = tex2D( g_positionSampler, GPUConvert1DAddressTo2D(face.vertexIndex.y, g_vertexMapAddrConsts) ).xyz;
   float3 p2 = tex2D( g_positionSampler, GPUConvert1DAddressTo2D(face.vertexIndex.z, g_vertexMapAddrConsts) ).xyz;
   
   // calc face normal
   float3 v0 = p1 - p0;
   float3 v1 = p2 - p0;
   float3 n = cross( v0, v1 );
   return( normalize(n) ); 

}


float4 FaceNormalPS( in float2 inUV : TEXCOORD0) : COLOR0
{
	sTriangleFace data;
	float4 tmp = tex2D( g_topoMapSampler, inUV ).xyzw;

	data.vertexIndex = tmp.xyz;
	
	return float4( CalcFaceNormal( data ),1 );
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {          
	PixelShader = compile ps_3_0 FaceNormalPS();
    }
}
