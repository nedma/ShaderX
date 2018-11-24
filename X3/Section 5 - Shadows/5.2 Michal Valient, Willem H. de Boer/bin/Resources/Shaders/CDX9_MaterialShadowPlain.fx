//;------------------------------
//; Plain shadow map without any filtering.
//;
//; Desc :
//;   Renders standard per vertex phong with shadow map.
//;
//; Part of Dimension3 Renderer
//; (c)2004 Michal Valient
//; akka em / STV3 akka  R.A.Y / EastW00D
//;------------------------------

uniform float4x4    mToClip     : D3_MATRIX_TO_CLIP;

uniform float4x4    mToWorld    : D3_MATRIX_TO_WORLD;

uniform float4x4    mToLight    : D3_MATRIX_TO_LIGHT;

uniform float4      vLightPos   : D3_LIGHT_POS;

uniform float4      vEyePos     : D3_EYE_POS;

uniform float4      vLightRanges: D3_LIGHT_RANGES;  //in X there is multiplier and in Y there is addition

uniform texture2D   tSpotTex    : D3_TEX_SPOT;

uniform texture2D   tShadowTex  : D3_TEX_SHADOW;

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
    float4 cColor       : COLOR0;
} VS_MATSHADOWPLAIN_OUT;

VS_MATSHADOWPLAIN_OUT VS_CDX9_MaterialShadowPlain(in VS_MATSHADOWPLAIN_IN input)
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
    output.cColor = dot(vWorldNormal, vLightVector);
    output.vLightCoord = mul(input.vPosition, mToLight);
    return output;
}

float4 PS_CDX9_MaterialShadowPlain(in VS_MATSHADOWPLAIN_OUT input) : COLOR
{
    float4 spotColor = tex2Dproj(smplSpotTex, input.vLightCoord);
    float4 shadow = tex2Dproj(smplShadowTex, input.vLightCoord);
    float fDist = input.vDistance.w - shadow.x;
    if (fDist<=0)
        return input.cColor * spotColor;
    else
        return float4(0,0,0,1);
}

vertexshader VS_CDX9_MaterialShadowPlain20 = compile vs_2_0 VS_CDX9_MaterialShadowPlain();

pixelshader PS_CDX9_MaterialShadowPlain20 = compile ps_2_0 PS_CDX9_MaterialShadowPlain();

technique T_CDX9_MaterialShadowPlain20 {
    pass P0
    {
        VertexShader = (VS_CDX9_MaterialShadowPlain20);
        PixelShader = (PS_CDX9_MaterialShadowPlain20);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;

        Cullmode = CCW;
    }
}
