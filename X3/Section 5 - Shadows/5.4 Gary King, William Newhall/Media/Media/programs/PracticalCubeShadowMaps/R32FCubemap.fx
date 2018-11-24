//-----------------------------------------------------------------------------
// Path:  SDK\MEDIA\programs\PracticalCubeShadowMaps
// File:  R32FCubemap.fx
// 
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//-----------------------------------------------------------------------------

float    cZBias;                        // constant Z-bias
float4   cLightPositionWorld;           // world-space light position
float4x4 cWorld;                        // object->world space matrix
float4x4 cWorldIT;                      // inverse transpose of object->world matrix
float4x4 cScreenViewProjection;         // world->clip space matrix
float4x4 cCubemapViewProjection;        // light->texture matrix
float4   cShadowResolution;             // [ cubeRes, 1/cubeRes, NA, NA ]
float4   cLightColor_Atten;             // [ Cr, Cg, Cb, quadratic attenuation ]

sampler ShadowMapSampler : register(s0);
sampler ColorMapSampler  : register(s1);

static const float3 cAmbient = float3(0.3, 0.3, 0.3);

//-----------------------------------------------------------------------------

struct VS_INPUT {
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 Texture  : TEXCOORD0;
    float3x3 TangentSpace  : TEXCOORD1;   
};

struct VS_OUTPUT {
    float4 Position  : POSITION;
    float3 Normal    : TEXCOORD0;
    float2 Texture   : TEXCOORD1;
    float3 LightVec[4]  : TEXCOORD2;
};

//----------------------------------------------------------------------------
//  RenderToShadowMap_*_[PS/VS]
//    Renders to the shadow map

VS_OUTPUT RenderToShadowMap_VS(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    float4 positionWorld = mul(IN.Position, cWorld);
    float4 positionLight = positionWorld - cLightPositionWorld;
    OUT.LightVec[0] = positionLight.xyz;
    OUT.Position = mul(positionLight, cCubemapViewProjection);
    return OUT;
}

float4 RenderToShadowMap_PS(VS_OUTPUT IN) : COLOR
{
    return length(IN.LightVec[0]) + cZBias;
}

//-----------------------------------------------------------------------------
//  RenderToScreen_*_[Un]filtered_[PS/VS]
//    Renders to the screen, using a point-sampled shadow map or a filtered shadow map

VS_OUTPUT RenderToScreen_VS(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
  
    //  compute lighting
    float4 positionWorld = mul(IN.Position, cWorld);
    float3 positionLight = positionWorld.xyz - cLightPositionWorld.xyz;
    float3 worldNormal   = normalize(mul(IN.Normal, (float3x3)cWorldIT));

    OUT.Normal = worldNormal;
    OUT.Texture = IN.Texture;
    OUT.LightVec[0] = positionLight;
    OUT.Position = mul(positionWorld, cScreenViewProjection);
    return OUT;
}


//////////////////////////////////////////////////////
//  LambertPointLight

float3 LambertPointLight( float3 N, float3 L )
{
    float3 LightColor = cLightColor_Atten.rgb;
    float  LightAtten = cLightColor_Atten.a;
    
    float cosAlpha = abs(dot( normalize(N), -normalize(L) ));
    float falloff  = LightAtten / dot(L,L);
    
    return LightColor * cosAlpha * falloff; 
}

float UnfilteredShadowComparison(samplerCUBE shadowMap, float3 texcoord, float compareDepth)
{
	return compareDepth < texCUBE(shadowMap, texcoord).r;
}

float FilteredShadowComparison(samplerCUBE shadowMap, float3 texcoord, float compareDepth)
{
    float4 shadowDepth[2];
    
    // compute a bunch of texture coordinates per-pixel
    //  a simple unweighted jittered lookup could be computed in the vertex shader
    //  and interpolated per-pixel, but it wouldn't look as good.
    float ma = max(abs(texcoord.x), max(abs(texcoord.y), abs(texcoord.z)));
    
    float3 projCoord = (texcoord / ma) * cShadowResolution.x;
    
    float3 weights = frac(projCoord);
    
    projCoord = floor(projCoord);// + float3(0.5, 0.5, 0.5);
    
    float3 shadowCoord[8] = {
        projCoord + float3( 0, 0, 0),  //0
        projCoord + float3( 0, 0, 1),  //1
        projCoord + float3( 0, 1, 0),  //2
        projCoord + float3( 0, 1, 1),  //3
        projCoord + float3( 1, 0, 0),  //4
        projCoord + float3( 1, 0, 1),  //5
        projCoord + float3( 1, 1, 0),  //6
        projCoord + float3( 1, 1, 1) };//7
        
    //  vectorize the lerps
    shadowDepth[0].x = texCUBE(shadowMap, shadowCoord[0] ).r;
    shadowDepth[0].y = texCUBE(shadowMap, shadowCoord[2] ).r;
    shadowDepth[0].z = texCUBE(shadowMap, shadowCoord[4] ).r;
    shadowDepth[0].w = texCUBE(shadowMap, shadowCoord[6] ).r;
    shadowDepth[1].x = texCUBE(shadowMap, shadowCoord[1] ).r;
    shadowDepth[1].y = texCUBE(shadowMap, shadowCoord[3] ).r;
    shadowDepth[1].z = texCUBE(shadowMap, shadowCoord[5] ).r;
    shadowDepth[1].w = texCUBE(shadowMap, shadowCoord[7] ).r;

    float4 visibilityVec[2];
    visibilityVec[0] = compareDepth.xxxx < shadowDepth[0];
    visibilityVec[1] = compareDepth.xxxx < shadowDepth[1];
    
    visibilityVec[0] = lerp(visibilityVec[0], visibilityVec[1], weights.z);
    visibilityVec[0].xy = lerp(visibilityVec[0].xz, visibilityVec[0].yw, weights.y);
        
    return lerp(visibilityVec[0].x, visibilityVec[0].y, weights.x);
}

float4 RenderToScreen_Unfiltered_PS(VS_OUTPUT IN) : COLOR 
{   
    float visibility =  UnfilteredShadowComparison(ShadowMapSampler, IN.LightVec[0], length(IN.LightVec[0]));
    
    float3 irradiance = cAmbient + LambertPointLight(IN.Normal, IN.LightVec[0]);
    float3 color  = tex2D(ColorMapSampler, IN.Texture) * irradiance;
    
    return float4(color, visibility);
}

float4 RenderToScreen_Filtered_PS(VS_OUTPUT IN) : COLOR 
{          
    float visibility = FilteredShadowComparison(ShadowMapSampler, IN.LightVec[0], length(IN.LightVec[0]));
   
    float3 irradiance = cAmbient+LambertPointLight(IN.Normal, IN.LightVec[0]);
    
    float3 color  = tex2D(ColorMapSampler, IN.Texture) * irradiance;
    
    return float4(color, visibility);
}

//-----------------------------------------------------------------------------
//  Techniques
//

technique UseShadowMap_Unfiltered
{
    pass P0
    {
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        BlendOp = Add;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = None;
        ZEnable = true;
        ZFunc = equal;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable = false;
        Lighting = false;
        
        VertexShader = compile vs_2_0 RenderToScreen_VS();
        PixelShader = compile ps_2_0  RenderToScreen_Unfiltered_PS();
    }
}

technique UseShadowMap_Filtered
{
    pass P0
    {
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        BlendOp = Add;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = None;
        ZEnable = true;
        ZFunc = equal;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable = false;
        Lighting = false;
            
        VertexShader = compile vs_2_0 RenderToScreen_VS();
        PixelShader = compile ps_2_0  RenderToScreen_Filtered_PS();
    }
}

technique GenShadowMap
{
    pass P0
    {
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
        CullMode = None;
        ZEnable = true;
        ZFunc = less;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable = true;
        Lighting = false;
            
        VertexShader = compile vs_2_0 RenderToShadowMap_VS();
        PixelShader = compile ps_2_0  RenderToShadowMap_PS();
    }
}

//-----------------------------------------------------------------------------

