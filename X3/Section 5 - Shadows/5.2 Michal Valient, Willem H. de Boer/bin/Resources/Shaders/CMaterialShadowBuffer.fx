//;------------------------------
//; Shadow buffer filler
//;
//; Desc :
//;   Renders normalized depth value into the output buffer
//;
//; Part of Dimension3 Renderer
//; (c)2004 Michal Valient
//;------------------------------

uniform float4x4 mToClip    : D3_MATRIX_TO_CLIP;

uniform float4x4 mToWorld   : D3_MATRIX_TO_WORLD;

uniform float4  vLightPos   : D3_LIGHT_POS;

uniform float4  vLightRanges: D3_LIGHT_RANGES;  //in X there is multiplier and in Y there is addition

struct VS_MATSHADOW_IN {
    float4 vPosition : POSITION; //position in object space
    float3 vNormal   : NORMAL;   //normal
    float2 tcCoord   : TEXCOORD; //texture coordinates
    float3 vTangent  : TANGENT;  //tangent
};

typedef struct {
    float4 vPosition : POSITION;
    float4 vDistance : TEXCOORD0;
} VS_MATSHADOW_OUT;

VS_MATSHADOW_OUT VS_CMaterialShadowBuffer(in VS_MATSHADOW_IN input)
{
    VS_MATSHADOW_OUT output;
    output.vPosition = mul(input.vPosition, mToClip); //vertex clip position
    float4 vWorldPos = mul(input.vPosition, mToWorld);    //to world position
    float3 vLightVector = vLightPos - vWorldPos;
    float vLength = length(vLightVector);
    float3 vNormal = mul(input.vNormal, (float3x3)mToWorld);
//    output.vDistance.xyz = 0.5f * vNormal + 0.5f;
//    output.vDistance.w = vLength * vLightRanges.x + vLightRanges.y;
    output.vDistance = vLength * vLightRanges.x + vLightRanges.y;
    return output;
}

float4 PS_CMaterialShadowBuffer(in VS_MATSHADOW_OUT input) : COLOR
{
    return input.vDistance;
}

vertexshader VS_CMaterialShadowBuffer20 = compile vs_2_0 VS_CMaterialShadowBuffer();

pixelshader PS_CMaterialShadowBuffer20 = compile ps_2_0 PS_CMaterialShadowBuffer();

technique T_CMaterialShadowBuffer20 {
    pass P0
    {
        VertexShader = (VS_CMaterialShadowBuffer20);
        PixelShader = (PS_CMaterialShadowBuffer20);

        AlphaBlendEnable = false;

        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;

        Cullmode = CW;
//        Cullmode = CCW;
    }
}
