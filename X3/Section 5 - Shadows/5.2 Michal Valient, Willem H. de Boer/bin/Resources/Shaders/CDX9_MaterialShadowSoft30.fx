//;------------------------------
//; The soft shadows for PS 3.0 and PS 2.a/b HW with preprocessing
//;
//; Desc :
//;   Soft shadow sampling is performed in single pass.
//;
//; Part of Dimension3 Renderer
//; (c)2004 Michal Valient
//;------------------------------

#define PI 3.14159265358979

#ifndef D3_DEFMODEPS
    #define D3_DEFMODEPS ps_3_0
#endif

#ifndef D3_DEFMODEVS
    #define D3_DEFMODEVS vs_3_0
#endif

uniform float4x4    mToClip         : D3_MATRIX_TO_CLIP;

uniform float4x4    mToWorld        : D3_MATRIX_TO_WORLD;

uniform float4x4    mToLight        : D3_MATRIX_TO_LIGHT;

uniform float4      vLightPos       : D3_LIGHT_POS;

uniform float4      vEyePos         : D3_EYE_POS;

uniform float4      vLightRanges    : D3_LIGHT_RANGES;  //in X there is multiplier and in Y there is addition

uniform texture2D   tSpotTex        : D3_TEX_SPOT;

uniform texture2D   tShadowTex      : D3_TEX_SHADOW;

uniform texture2D   tNoiseTex       : D3_TEX_NOISE;

static float        fNoiseInc    : D3_NOISEINCREMENT = 1.0f / 256.0f;   //The size of the noise texture

static float        fLightRadius : D3_LIGHTSIZE = 16.0f / 512.0f;       //size of the light source
                                                                        //If you change the size of shadow mam 
                                                                        //(now hardcoded to 512) you have to 
                                                                        //run through the code and change this 
                                                                        //also there - i.e. postprocess scripts.

static float        fMaxOccluderRadius : D3_MAXRADIUS = 16.0f / 512.0f;

static float        fAddition = 1.0f - 17.0f/512.0f;    //This number is added to the preprocessed value from shadow map

static  int         iMaxSamples      : D3_MAXSAMPLES = 4;   //The maximum number of loop executions

static float        fShadowTexItem   : D3_SHADOWTEXSIZE = 1.0f / 512.0f;

static float        fSamplesPerItem  : D3_SAMPLESPERITEM = fMaxOccluderRadius * fMaxOccluderRadius * PI / 32.0f;

static float2       vScreenCoordMod  : D3_SCREENCOORDMOD = float2(1024.0f / 256.0f / 2.0f, 768.0f/256.0f / 2.0f);

static float2       vScreenCoordMod2 : D3_SCREENCOORDMOD2 = float2(0.5, 0.5);

sampler2D smplSpotTex = sampler_state 
{
    Texture = <tSpotTex>;

    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler2D smplShadowTex = sampler_state 
{
    Texture = <tShadowTex>;

    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler2D smplNoiseTex = sampler_state 
{
    Texture = <tNoiseTex>;

    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

//    MinFilter = LINEAR;
//    MagFilter = LINEAR;
//    MipFilter = LINEAR;

    AddressU = WRAP;
    AddressV = WRAP;
};

struct VS_MATSHADOWPLAIN_IN {
    float4 vPosition : POSITION; //position in object space
    float3 vNormal   : NORMAL;   //normal
    float2 tcCoord   : TEXCOORD; //texture coordinates
    float3 vTangent  : TANGENT;  //tangent
};

typedef struct {
    float4 vPosition    : POSITION;
    float4 vDistance    : TEXCOORD0;
    float4 vLightCoord  : TEXCOORD1;
    float4 vScreenPos   : TEXCOORD2;
    float4 cColor       : COLOR0;
} VS_MATSHADOWPLAIN_OUT;

VS_MATSHADOWPLAIN_OUT VS_CDX9_MaterialShadowPlain(in VS_MATSHADOWPLAIN_IN input)
{
    VS_MATSHADOWPLAIN_OUT output;
    output.vPosition = mul(input.vPosition, mToClip); //vertex clip position
    output.vScreenPos = output.vPosition;
    float4 vWorldPos = mul(input.vPosition, mToWorld);    //to world position

    float3 vLightVector = vLightPos - vWorldPos;
    float vLength = length(vLightVector);
    output.vDistance = vLength * vLightRanges.x + vLightRanges.y;

    float3 vWorldNormal = mul(input.vNormal, mToWorld);
    vWorldNormal = normalize(vWorldNormal);
    vLightVector = normalize(vLightVector);
    output.cColor = dot(vWorldNormal, vLightVector)/* * 0.33*/;
    output.vLightCoord = mul(input.vPosition, mToLight);
    return output;
}

//This method takes four pixels and perform PCF with additional test on it.
//Method is optimized to utilize vectorization using all four channels
float TestShadow30(inout float2 tcNoise, in float2 tcCoordOri, in float fDistance, in float fRadius)
{
    //We read four shadow samples for upcomming PCF
    float4 vShadow;
    float4 vRandomPointDistance2;
    float2 tcRNoise = tex2D(smplNoiseTex, tcNoise);
    float2 vRandomCoords = tcRNoise * fRadius;
    vShadow.x = tex2D(smplShadowTex, tcCoordOri + (tcRNoise * fRadius));
    vRandomPointDistance2.x = dot(vRandomCoords, vRandomCoords);
    tcNoise.x += fNoiseInc;

    tcRNoise = tex2D(smplNoiseTex, tcNoise);
    vRandomCoords = tcRNoise * fRadius;
    vShadow.y = tex2D(smplShadowTex, tcCoordOri + (tcRNoise * fRadius));
    vRandomPointDistance2.y = dot(vRandomCoords, vRandomCoords);
    tcNoise.x += fNoiseInc;
    
    tcRNoise = tex2D(smplNoiseTex, tcNoise);
    vRandomCoords = tcRNoise * fRadius;
    vShadow.z = tex2D(smplShadowTex, tcCoordOri + (tcRNoise * fRadius));
    vRandomPointDistance2.z = dot(vRandomCoords, vRandomCoords);
    tcNoise.x += fNoiseInc;
    
    tcRNoise = tex2D(smplNoiseTex, tcNoise);
    vRandomCoords = tcRNoise * fRadius;
    vShadow.w = tex2D(smplShadowTex, tcCoordOri + (tcRNoise * fRadius));
    vRandomPointDistance2.w = dot(vRandomCoords, vRandomCoords);
    tcNoise.x += fNoiseInc;

    //for each component of vShadow we compute appropriate local radius of the shadow disk.
    float4 vLocalRadius = fDistance - vShadow;
    vLocalRadius = vLocalRadius / vShadow;
    vLocalRadius = vLocalRadius * fRadius;

    //Now we test if our current point lies within this radius.
    //If yes, we consider the PCF test as correct (the point really affects shadow of the current pixel)    
    float4 vFuzzyPCF = (vRandomPointDistance2 - vLocalRadius <= float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 vRet = (fDistance - vShadow <= float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(1.0f, 1.0f, 1.0f, 1.0f) : vFuzzyPCF;

    //Dot - a cheap way to do a sum
    float fRet = dot(vRet, (float4)1.0f);

    return fRet;
}

float4 PS_CDX9_MaterialShadowSoft30(uniform bool bUse1D,         //boolean to use 1D texture instead of 2D
                                    uniform bool bConstantOuter, //render the "no preprocessing" mode
                                    float4 vDistance    : TEXCOORD0,
                                    float4 vLightCoord  : TEXCOORD1,
                                    float4 vScreenPos   : TEXCOORD2,
                                    float4 cColor       : COLOR0) : COLOR
{
    //Define the projective coordinates
    float2 tcCoord;
    tcCoord = vLightCoord.xy / vLightCoord.w;

    //read the random noise coordinates.
    float2 tcNoise = float2(0,0);
    if (!bUse1D) {
        tcNoise = vScreenPos.xy / vScreenPos.w;
        tcNoise = tcNoise * vScreenCoordMod + vScreenCoordMod2;
        tcNoise = tex2D(smplNoiseTex, tcNoise);
    }

    float4 shadow = tex2D(smplShadowTex, tcCoord);
    float fHardShadow = vDistance.x - shadow.x;
    float fOccluderRadius = 0.25;
    float fNoiseSum = 0.0f;
    if (fHardShadow <= 0)         //This one is lit
    {
        fNoiseSum = 1.0f;
        if (!bConstantOuter)
        {
            //Now we are computing using preprocessed value in Y
            float fDistFromVirtual = vDistance.x - shadow.y;
            fOccluderRadius = fDistFromVirtual / shadow.x;
        }
    }
    else
        fOccluderRadius = fHardShadow / shadow.x;

    fOccluderRadius *= fLightRadius;
    fOccluderRadius = clamp(fOccluderRadius, 0, fMaxOccluderRadius);

    if (!bConstantOuter) {  //When preprocessing is enabled we can skip distant texels
        float fDistFromBorder = 1.0f - (fAddition + shadow.z);
        if (fDistFromBorder > fOccluderRadius)
            fOccluderRadius = 0.0f;
    }

    float fSampleCount = fOccluderRadius * fOccluderRadius * PI / fSamplesPerItem;
    float fRealSampleCount = 1.0f;
    for (int i=0; i<iMaxSamples; i++)
    {
        if (fSampleCount >= 0.5)
        {
            fNoiseSum += TestShadow30(tcNoise, tcCoord, vDistance.x, fOccluderRadius);
            fSampleCount -= 1.0f;
            fRealSampleCount += 4.0f;
        }
    }
    fNoiseSum = fNoiseSum / fRealSampleCount;

    float4 spotColor = tex2D(smplSpotTex, tcCoord);
    float4 outColor = fNoiseSum * cColor * spotColor;
    return outColor;
}

technique T_CDX9_MaterialShadowPlain30 {
    pass P0
    {
        VertexShader = compile D3_DEFMODEVS VS_CDX9_MaterialShadowPlain();
        PixelShader = compile D3_DEFMODEPS PS_CDX9_MaterialShadowSoft30(false, false);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;
    }
}

technique T_CDX9_MaterialShadowPlain1D_30 {
    pass P0
    {
        VertexShader = compile D3_DEFMODEVS VS_CDX9_MaterialShadowPlain();
        PixelShader = compile D3_DEFMODEPS PS_CDX9_MaterialShadowSoft30(true, false);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;
    }
}

technique T_CDX9_MaterialShadowNoProcess30 {
    pass P0
    {
        VertexShader = compile D3_DEFMODEVS VS_CDX9_MaterialShadowPlain();
        PixelShader = compile D3_DEFMODEPS PS_CDX9_MaterialShadowSoft30(false, true);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;
    }
}

technique T_CDX9_MaterialShadowNoProcess1D_30 {
    pass P0
    {
        VertexShader = compile D3_DEFMODEVS VS_CDX9_MaterialShadowPlain();
        PixelShader = compile D3_DEFMODEPS PS_CDX9_MaterialShadowSoft30(true, true);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;
    }
}
