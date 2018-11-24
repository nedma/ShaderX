//-----------------------------------------------------------------------------
// Path:  SDK\MEDIA\programs\PracticalCubeShadowMaps
// File:  VSDCT.fx
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

float4   cLightPositionWorld;           // world-space light position
float4x4 cWorld;                        // object->world space matrix
float4x4 cWorldIT;                      // inverse transpose of object->world matrix
float4x4 cScreenViewProjection;         // world->clip space matrix
float4x4 cCubemapViewProjection;        // light->texture matrix
float4   cLightColor_Atten;             // [ Cr, Cg, Cb, quadratic attenuation ]
float4   cLightQ;                       // [ Zf/(Zf-Zn), ZnZf/(Zf-Zn), 0, 0 ]
float4   cIndirectionScaleBias;         // [ N/N+1, 1/2N, 0, 0 ]

sampler ShadowMapSampler : register(s0);
sampler ColorMapSampler  : register(s1);
sampler IndirectionSampler : register(s2);
sampler FaceSelectionSampler : register(s3);

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
    float4 Diffuse   : COLOR0;
};

//----------------------------------------------------------------------------
//  RenderToShadowMap_[PS/VS]
//    Renders to the shadow map

VS_OUTPUT RenderToShadowMap_VS(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    float4 positionWorld = mul(IN.Position, cWorld);
    float3 worldNormal   = normalize(mul(IN.Normal, (float3x3)cWorldIT));   
    float4 positionLight = positionWorld - cLightPositionWorld;
    OUT.LightVec[0] = positionLight.xyz;
    OUT.Position = mul(positionLight, cCubemapViewProjection);
    OUT.Diffuse = abs(dot(normalize(positionLight.xyz), worldNormal));
    return OUT;
}

//-----------------------------------------------------------------------------
//  RenderToScreen_VS
//    Renders to the screen, using either a point-sampled shadow map or a filtered shadow map

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


///////////////////////////////////////////////////////////////////
//  RenderToScreen_FullIndirectPS
//    uses a full-size (e.g., 512x512) indirection texture
//    simplest implementation.
float4 RenderToScreen_FullIndirectPS(VS_OUTPUT IN) : COLOR
{
    float3 LightVecAbs = abs(IN.LightVec[0]);
    //  map from cubemap coordinates into 2D depth texture coordinates -- simple straight lookup
    float4 shadowIndirectCoord = texCUBE(IndirectionSampler, IN.LightVec[0]);
    
    float MA = max(max(LightVecAbs.r, LightVecAbs.g), LightVecAbs.b);   
    
    //  project eye-space Z into window-space Z
    shadowIndirectCoord.b = (-1.f/MA)*cLightQ.x + cLightQ.y;
    //  preserve A, since we need a 4D lookup (tex2Dproj, tex2Dlod) to keep HLSL from clobbering .b
    shadowIndirectCoord.a = 1.f;
    
    float  visibility = tex2Dproj(ShadowMapSampler, shadowIndirectCoord).r;

    float3 irradiance = cAmbient + LambertPointLight(IN.Normal, IN.LightVec[0]);
    float3 color  = tex2D(ColorMapSampler, IN.Texture) * irradiance;
    
    return float4(color, visibility);
}

////////////////////////////////////////////////////////////////////
//  RenderToScreen_LimitedIndirect_PS
//    Uses a limited (reduced-resolution) indirection texture.
//    Because there are (much) fewer operations required to update the indirection texture
//    (e.g., when viewports change), this should be used when face resolutions can change dynamically
//    and independently
float4 RenderToScreen_LimitedIndirect_PS(VS_OUTPUT IN) : COLOR 
{   
    float4 faceVector = texCUBE(FaceSelectionSampler, IN.LightVec[0]);
    
    //  magic -- since faceVector is just +- 1.0 on the MA face, a simple dot product with LightVec will give MA
    float  MA = dot(IN.LightVec[0], faceVector);
    
    //  s and t in indirectCoord need to be fudged a little bit (calculated based on the indirection texture size),
    //  to avoid clamping artifacts.  We are performing a linear mapping so that 1.0 on the virtual shadow map cube
    //  maps to exactly 1.0 in the indirection texture
    //  faceVector is +- 1.0 to compute MA.  abs(faceVector) corresponds to the r axis after face selection.
    //  so, leave r alone, and fudge s and t
    
    float3 indirectCoord = IN.LightVec[0] / MA;    
    indirectCoord = (abs(faceVector)) ? indirectCoord : indirectCoord*cIndirectionScaleBias.x;
       
    float4 shadowIndirectCoord = texCUBE(IndirectionSampler, indirectCoord);    
    shadowIndirectCoord.b = (-1.f/MA)*cLightQ.x + cLightQ.y;
    shadowIndirectCoord.a = 1.f;
    
    float  visibility = tex2Dproj(ShadowMapSampler, shadowIndirectCoord).r;

    float3 irradiance = cAmbient + LambertPointLight(IN.Normal, IN.LightVec[0]);
    float3 color  = tex2D(ColorMapSampler, IN.Texture) * irradiance;
    
    return float4(color, visibility);
}

//------------------------------------------------------------------------------
//  Debug is used to draw a screen-aligned quad with the debug texture applied

struct DEBUG_INPUT
{
    float4 pos : POSITION;
};

struct DEBUG_OUTPUT
{
    float4 pos : POSITION;
    float4 tex : TEXCOORD0;
};


DEBUG_OUTPUT Debug_VS(DEBUG_INPUT IN)
{
    DEBUG_OUTPUT OUT;
    OUT.pos = float4(IN.pos.xyz*0.2 + 0.75, 1.0);
    OUT.tex = float4(IN.pos.xyz*0.5 + 0.5, 1.0);
    OUT.tex.y = 1.0f - OUT.tex.y;
    return OUT;
}

float4 Debug_PS(DEBUG_OUTPUT IN) : COLOR
{
    return tex2D( ShadowMapSampler, IN.tex );
}

//-----------------------------------------------------------------------------
//  Techniques
//

technique UseShadowMap_Full
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
        PixelShader = compile ps_2_0  RenderToScreen_FullIndirectPS();
    }
}

technique UseShadowMap_Limited
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
        PixelShader = compile ps_2_0  RenderToScreen_LimitedIndirect_PS();
    }
}

technique GenShadowMap_Color
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
        ColorOp[0] = SELECTARG1;
        ColorArg1[0] = DIFFUSE;
        ColorArg2[0] = DIFFUSE;
        AlphaOp[0] = SELECTARG1;
        AlphaArg1[0] = DIFFUSE;
        AlphaArg2[0] = DIFFUSE;
        ColorOp[1] = DISABLE;
        AlphaOp[1] = DISABLE;
        PixelShader = NULL;
    }
}

technique GenShadowMap_Nocolor
{
    pass P0
    {
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
        CullMode = None;
        ZEnable = true;
        ZFunc = less;
        ColorWriteEnable = 0;
        ZWriteEnable = true;
        Lighting = false;
            
        VertexShader = compile vs_2_0 RenderToShadowMap_VS();
        ColorOp[0] = SELECTARG1;
        ColorArg1[0] = DIFFUSE;
        ColorArg2[0] = DIFFUSE;
        AlphaOp[0] = SELECTARG1;
        AlphaArg1[0] = DIFFUSE;
        AlphaArg2[0] = DIFFUSE;
        ColorOp[1] = DISABLE;
        AlphaOp[1] = DISABLE;
        PixelShader = NULL;
    }
}

technique DebugTechnique
{
    pass P0
    {
        ZEnable = false;
        ZWriteEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable =false;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        CullMode= None;
        Lighting = false;
    
        VertexShader = compile vs_1_1 Debug_VS();
        PixelShader  = compile ps_2_0 Debug_PS();
    }
}


//-----------------------------------------------------------------------------

