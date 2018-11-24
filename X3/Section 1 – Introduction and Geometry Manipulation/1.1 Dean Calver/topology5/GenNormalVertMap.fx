//--------------------------------------------------------------------------------------
// File: GenPosNormalMap.fx
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float g_OneTexel;
float2	g_faceMapAddrConsts;

texture g_vvTopoMap;
sampler2D g_vvMapSampler = 
sampler_state
{
	Texture = <g_vvTopoMap>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;	// required for fast 1D to 2D conversion
    AddressV = Clamp;	
};

texture g_faceNormalMap;
sampler2D g_faceNormalSampler = 
sampler_state
{
	Texture = <g_faceNormalMap>;
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

float3 CalcVertexNormal( float2 inUV )
{
    float4 vv0 = tex2D( g_vvMapSampler, inUV ).xyzw;
    float4 vv1 = tex2D( g_vvMapSampler, inUV + float2(g_OneTexel,0) ).xyzw;
    float3 vertexNorm;
	
	// 6 lookups 
	vertexNorm = tex2D(  g_faceNormalSampler, GPUConvert1DAddressTo2D( vv0.x, g_faceMapAddrConsts ) );
	vertexNorm += tex2D(  g_faceNormalSampler, GPUConvert1DAddressTo2D( vv0.y, g_faceMapAddrConsts ) );
	vertexNorm += tex2D(  g_faceNormalSampler, GPUConvert1DAddressTo2D( vv0.z, g_faceMapAddrConsts ) );
	vertexNorm += tex2D(  g_faceNormalSampler, GPUConvert1DAddressTo2D( vv0.w, g_faceMapAddrConsts ) );
	vertexNorm += tex2D(  g_faceNormalSampler, GPUConvert1DAddressTo2D( vv1.x, g_faceMapAddrConsts ) );
	vertexNorm += tex2D(  g_faceNormalSampler, GPUConvert1DAddressTo2D( vv1.y, g_faceMapAddrConsts ) );

    return normalize( vertexNorm );
}

float4 GenFaceNormalTopoMap( in float2 inUV : TEXCOORD0 ) : COLOR0
{	
	return float4( CalcVertexNormal( inUV ),1 );
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {          
		PixelShader = compile ps_3_0 GenFaceNormalTopoMap();
    }
}