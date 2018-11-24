// pixel functions and variables that are used in multiple light shaders
// included from the actual fx file

// 1 over the width and height of the framebuffer (for correct texture sampling)
shared float fRcpFrameWidth;
shared float fRcpFrameHeight;

// texture and sample for each buffer making up the far framebuffer
shared texture Buffer0Texture;
sampler Buffer0 = sampler_state 
{
	Texture = (Buffer0Texture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

shared texture Buffer1Texture;
sampler Buffer1 = sampler_state 
{
	Texture = (Buffer1Texture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

shared texture Buffer2Texture;
sampler Buffer2 = sampler_state 
{
	Texture = (Buffer2Texture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

shared texture Buffer3Texture;
sampler Buffer3 = sampler_state 
{
	Texture = (Buffer3Texture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

// some define to get better names for the buffers
#define PositionBufferTexture	Buffer0Texture
#define NormalBufferTexture		Buffer1Texture
#define Cs0BufferTexture		Buffer2Texture
#define Cs1BufferTexture		Buffer3Texture

#define PositionBuffer		Buffer0
#define NormalBuffer		Buffer1
#define Cs0Buffer			Buffer2
#define Cs1Buffer			Buffer3

// for viewing shadow maps
shared texture GlobalShadowMapTexture;
sampler GlobalShadowMap = sampler_state 
{
	Texture = (GlobalShadowMapTexture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

// the material table table
shared texture MaterialMapTexture;
sampler MaterialMap = sampler_state
{
	Texture = (MaterialMapTexture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

// the dot product functor table
shared texture DotProductFunctionMapTexture;
sampler DotProductFunctionMap = sampler_state
{
	Texture = (DotProductFunctionMapTexture);
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

// the attenuation functor table
shared texture AttenuationMapTexture;
sampler AttenuationMap = sampler_state
{
	Texture = (AttenuationMapTexture);
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

shared half3 f3WorldUpInViewSpace	= float3(0,1,0);			// the world up vector in view space
shared float4x4 matProjectionInvScaled;		// inverse projection matrix scaled by ZFar (from HCLIP to view space)
shared half4	f4ShadowJitter0;			// should equal <0.1 / SHADOW_MAP_SIZE, 0.1/SHADOW_MAP_SIZE, 0, 0 >
shared half4	f4ShadowJitter1;			// should equal <0.1 / SHADOW_MAP_SIZE, 0.9/SHADOW_MAP_SIZE, 0, 0 >
shared half4	f4ShadowJitter2;			// should equal <0.9 / SHADOW_MAP_SIZE, 0.1/SHADOW_MAP_SIZE, 0, 0 >
shared half4	f4ShadowJitter3;			// should equal <0.9 / SHADOW_MAP_SIZE, 0.9/SHADOW_MAP_SIZE, 0, 0 >

// we store position in the fat framebuffer as post-perspective space 
// the light shader (usually) converts back into view space where lighting takes place
// the range of the position is 0-1 to maximise integer usage
float4 UnPackPositionFromFatFramebuffer( float3 inp )
{
	float4 o;
	o.xyzw = mul( float4(inp,1), matProjectionInvScaled );
	o.xyz = o.xyz / o.w;
	return o;
}

float3 UnPackNormalFromFatFramebuffer( float3 inp )
{
	return (inp - 0.5) * 2.0;
}

void LookupMaterialData( half index, out half4 a)
{
	a = tex2D( MaterialMap, index );
}

half ShapeDotProduct( half dot, half function )
{
	half2 uv;
	
	// bias dot for lookup
	uv.x = (dot * 0.5) + 0.5;
	uv.y = function;

	return tex2D( DotProductFunctionMap, uv );
}

half LookupAttenuation( half D, half rcpMaxD, half function )
{
	return tex2D( AttenuationMap, half2(D * rcpMaxD, function) );
}

/* vectors ala Renderman
N = Shading Normal
I = Incident vector (vector from eye to surface point)
L = Light vector
Cs0 = Surface colour 0 with Fa in W
Cs1 = Surface colour 1 with Fb in W
LCol = Colour of the light
----------------------
outCol = output colour 0
*/
void Illuminate( half3 N, half3 I, half3 L, half4  Cs0, half4 Cs1, half3 LCol, half MatIndx,
					out half3 outCol )
{
	half4 data;
	LookupMaterialData( MatIndx, data );

	// calculate the H vector
	half3 H = normalize(L + I);

	half NdotL = dot(N, L);
	half NdotH = dot(N, H);

	half2 KaFaNdotL = ShapeDotProduct( NdotL, Cs0.w );
	half2 KbFbNdotH = ShapeDotProduct( NdotH, Cs1.w );

	outCol = (KaFaNdotL.x * Cs0.xyz * LCol.xyz) + (KbFbNdotH.x * lerp(Cs1.xyz, Cs1.xyz * LCol.xyz, data.y) * KaFaNdotL.y);
}