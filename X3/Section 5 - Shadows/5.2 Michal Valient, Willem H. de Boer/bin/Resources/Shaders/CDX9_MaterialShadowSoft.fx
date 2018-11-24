//;------------------------------
//; The soft shadows for PS 2.0 HW with preprocessing
//;
//; Desc :
//;   The soft shadows for PS 2.0 HW with preprocessing.
//;
//;   Soft shadow sampling is performed purely in image space on PS2.0 HW.
//;   It is also optimized using stencil test - non rendered pixels are rejected early. Speed gain was significant.
//;   Further optimization can be performed when also fully lit and fully opaque points are rejected by stencil.
//;   (This further optimization step was not implemented)
//;
//; Part of Dimension3 Renderer
//; (c)2004 Michal Valient
//;------------------------------

uniform texture2D   tSpotTex    : D3_TEX_SPOT;

uniform texture2D   tShadowTex  : D3_TEX_SHADOW;

uniform texture2D   tNoiseTex   : D3_TEX_NOISE;

uniform texture2D   tAccumSrc   : D3_TEX_ACCUMSRC;

uniform float4x4    mToClip     : D3_MATRIX_TO_CLIP;

uniform float4x4    mToWorld    : D3_MATRIX_TO_WORLD;

uniform float4x4    mToLight    : D3_MATRIX_TO_LIGHT;

uniform float4      vLightPos   : D3_LIGHT_POS;

uniform float4      vEyePos     : D3_EYE_POS;

uniform float4      vLightRanges : D3_LIGHT_RANGES;  //in X there is multiplier and in Y there is addition

static float        fNoiseInc    : D3_NOISEINCREMENT = 1.0f / 256.0f;   //The size of the noise texture

static float        fLightRadius : D3_LIGHTSIZE = 16.0f / 512.0f;       //size of the light source
                                                                        //If you change the size of shadow mam 
                                                                        //(now hardcoded to 512) you have to 
                                                                        //run through the code and change this 
                                                                        //also there - i.e. postprocess scripts.

static float        fMaxOccluderRadius : D3_MAXRADIUS = 16.0f / 512.0f;

static float        fAddition = 1.0f - 17.0f/512.0f;    //This number is added to the preprocessed value from shadow map

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
    MipFilter = POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler2D smplNoiseTex = sampler_state 
{
    Texture = <tNoiseTex>;

    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = WRAP;
    AddressV = WRAP;
};

sampler2D smplAccumSrc = sampler_state 
{
    Texture = <tAccumSrc>;

    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
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
} VS_MATSHADOWPLAIN_OUT;

//Vertex shader for first pass - in the first pass we render just positions to the texture
VS_MATSHADOWPLAIN_OUT VS_CDX9_MaterialShadow_Pass0(in VS_MATSHADOWPLAIN_IN input)
{
    VS_MATSHADOWPLAIN_OUT output;
    output.vPosition = mul(input.vPosition, mToClip); //vertex clip position
    float4 vWorldPos = mul(input.vPosition, mToWorld);    //to world position

    float3 vLightVector = vLightPos - vWorldPos;
    float vLength = length(vLightVector);
    output.vDistance = vLength * vLightRanges.x + vLightRanges.y;

    float3 vWorldNormal = mul(input.vNormal, mToWorld);
    vWorldNormal = normalize(vWorldNormal);
    vLightVector = normalize(vLightVector);
    output.vLightCoord = mul(input.vPosition, mToLight);
    return output;
}

//Pixel shader for first pass - in the first pass we render just positions to the texture
float4 PS_CDX9_MaterialShadow_Pass0(uniform bool bConstantOuter,
                                    VS_MATSHADOWPLAIN_OUT input) : COLOR0
{
    float4 outColor;
    float2 tcCoord;
    tcCoord = input.vLightCoord.xy / input.vLightCoord.w;

    float4 shadow = tex2D(smplShadowTex, tcCoord);
    float fHardShadow = input.vDistance.x - shadow.x;
    float fOccluderRadius;
    if (fHardShadow <= 0)
    {
        //This one is lit
        if (bConstantOuter)
        {
            fOccluderRadius = 0.25;
        }
        else
        {
            float fDistFromVirtual = input.vDistance.x - shadow.y;
            fOccluderRadius = fDistFromVirtual / shadow.x;
        }
    }
    else
    {
        fOccluderRadius = fHardShadow / shadow.x;
    }
    fOccluderRadius *= fLightRadius;
    fOccluderRadius = clamp(fOccluderRadius, 0, fMaxOccluderRadius);

    outColor = float4(tcCoord.x, tcCoord.y, input.vDistance.x, fOccluderRadius);
    return outColor;
}

//This technique renders the position info to the framebuffer
technique T_CDX9_MaterialShadowPlain20 {
    pass P0
    {
        VertexShader = compile vs_2_0 VS_CDX9_MaterialShadow_Pass0();
        PixelShader = compile ps_2_0 PS_CDX9_MaterialShadow_Pass0(false);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;
        
        //Enable stencil to write one where we have geometry
        StencilEnable = true;
        STENCILFUNC = ALWAYS;
        STENCILPASS = INCRSAT;
    }
}

//This technique renders the position info to the framebuffer
//This technique uses just constant outer penumbra radius
technique T_CDX9_MaterialShadow_ConstantOuter {
    pass P0
    {
        VertexShader = compile vs_2_0 VS_CDX9_MaterialShadow_Pass0();
        PixelShader = compile ps_2_0 PS_CDX9_MaterialShadow_Pass0(true);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;
        
        //Enable stencil to write one where we have geometry
        StencilEnable = true;
        STENCILFUNC = ALWAYS;
        STENCILPASS = INCRSAT;
    }
}

//##########################################################
//##########################################################
//
// The shadow accumulation code
//
//##########################################################
//##########################################################

//This method takes four pixels and perform PCF with additional test on it.
//Method is optimized to utilize vectorization using all four channels
float TestShadow20(inout float2 tcNoise, in float2 tcCoordOri, in float fDistance, in float fRadius)
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

//Main PS method
float4 AccumShadow(uniform bool bUse1D,         //boolean to use 1D texture instead of 2D
                   uniform float fAllPasses,    //How many samples do we process at all
                   uniform float fNoiseStart,   //Where do we start with the noise texture sampling
                   float2 Tex : TEXCOORD0,
                   float2 Tex2: TEXCOORD1) : COLOR0
{
    //load data from the previously rendered texture
    float4 vAccumSrc = tex2D(smplAccumSrc, Tex);
    float2 tcCoord = vAccumSrc.xy;
    float fDistance = vAccumSrc.z;
    float fOccluderRadius = vAccumSrc.w;

    //read the random noise coordinates.
    float2 tcNoise = float2(0,0);
    if (!bUse1D)
        tcNoise = tex2D(smplNoiseTex, Tex2).xy;
    tcNoise.x += fNoiseStart * fNoiseInc;

    //Perform the loop - do the PCF with additional test.
    float fSum = TestShadow20(tcNoise, tcCoord, fDistance, fOccluderRadius);
    
    //Output it and accumulate to the alpha
    float4 outColor = float4(0, 0, 0, fSum / fAllPasses / 4.0f);
//    float4 outColor = fSum / fAllPasses / 4.0f;
    return outColor;
}

//This technique uses 1D random distribution texture to obtain coordinates
technique D3_AccumTechnique1D
{
    pass p0
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(true, 4.0f, 0.0f);
        ZEnable = false;
        AlphaBlendEnable = false;
        ZWriteEnable = false;

        Cullmode = CCW;
        
        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
    pass p1
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(true, 4.0f, 4.0f);
        ZEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;

        Cullmode = CCW;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
    pass p2
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(true, 4.0f, 8.0f);
        ZEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;

        Cullmode = CCW;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
    pass p3
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(true, 4.0f, 12.0f);
        ZEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;

        Cullmode = CCW;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
}

//This technique uses 2D random distribution texture to obtain coordinates
technique D3_AccumTechnique 
{
    pass p0
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(false, 4.0f, 0.0f);
        ZEnable = false;
        AlphaBlendEnable = false;
        ZWriteEnable = false;

        Cullmode = CCW;
        
        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
    pass p1
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(false, 4.0f, 4.0f);
        ZEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;

        Cullmode = CCW;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
    pass p2
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(false, 4.0f, 8.0f);
        ZEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;

        Cullmode = CCW;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
    pass p3
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 AccumShadow(false, 4.0f, 12.0f);
        ZEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;

        Cullmode = CCW;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;
    }
}

//##########################################################
//##########################################################
//
// The standard Phong shading technique
//
//##########################################################
//##########################################################

typedef struct {
    float4 vPosition    : POSITION;
    float4 vLightCoord  : TEXCOORD0;
    float4 cColor       : COLOR0;
} VS_MATSHADOWPLAIN_OUT_PHNG;

VS_MATSHADOWPLAIN_OUT_PHNG VS_CDX9_Phong(in VS_MATSHADOWPLAIN_IN input)
{
    VS_MATSHADOWPLAIN_OUT_PHNG output;
    output.vPosition = mul(input.vPosition, mToClip); //vertex clip position
    float4 vWorldPos = mul(input.vPosition, mToWorld);    //to world position

    float3 vLightVector = vLightPos - vWorldPos;
    float3 vWorldNormal = mul(input.vNormal, mToWorld);
    vWorldNormal = normalize(vWorldNormal);
    vLightVector = normalize(vLightVector);
    output.cColor = dot(vWorldNormal, vLightVector)/* * 0.33*/;
    output.vLightCoord = mul(input.vPosition, mToLight);
    return output;
}

float4 PS_CDX9_Phong(VS_MATSHADOWPLAIN_OUT_PHNG input) : COLOR
{
    float4 spotColor = tex2Dproj(smplSpotTex, input.vLightCoord);
    spotColor = spotColor * input.cColor;
    spotColor.a = 0.0f;
    return spotColor;
}

float4 VS_ZFill(in VS_MATSHADOWPLAIN_IN input) : POSITION
{
    return mul(input.vPosition, mToClip); //vertex clip position
}

float4 PS_ZFill() : COLOR
{
    return float4(0, 0, 0, 0);
}

technique D3_PhongTechnique
{
    pass p1
    {
        VertexShader = compile vs_2_0 VS_CDX9_Phong();
        PixelShader = compile ps_2_0 PS_CDX9_Phong();
        AlphaBlendEnable = true;
        SrcBlend = DESTALPHA;
        DestBlend = ZERO;

        ZEnable = true;
        ZFunc = EQUAL;
        ZWriteEnable = false;

        StencilEnable = true;
        STENCILFUNC = NOTEQUAL;
        STENCILPASS = KEEP;
        STENCILREF = 0;

        Cullmode = CCW;
    }
}
