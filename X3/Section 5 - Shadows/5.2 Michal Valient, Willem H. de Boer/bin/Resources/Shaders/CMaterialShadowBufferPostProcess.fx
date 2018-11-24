//;------------------------------
//; Shadow map post processing
//;
//; Part of Dimension3 Renderer
//; (c)2004 Michal Valient
//;------------------------------

texture2D tShadowTex;

sampler2D smplShadowTex = sampler_state 
{
    Texture = <tShadowTex>;

    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

float tSize     = 1.0f / 512.0f;
float fOneItem  = 1.0f / 512.0f;

//-----------------------------------------------------------------------------
// Pixel Shader: PostProcessPS
// Desc: Performs post-processing effect that detects and highlights edges.
//-----------------------------------------------------------------------------
float4 PostProcessPS_Edge(float2 Tex : TEXCOORD0) : COLOR0
{
    float2 TexelKernel[4];
    TexelKernel[0] = float2(Tex.x           , Tex.y + tSize);
    TexelKernel[1] = float2(Tex.x + tSize   , Tex.y);
    TexelKernel[2] = float2(Tex.x           , Tex.y - tSize);
    TexelKernel[3] = float2(Tex.x - tSize   , Tex.y);

    float4 Orig = tex2D(smplShadowTex, Tex);
    float fMinDepth = Orig.w;

    float Sum = 0;
    for (int i=0; i<4; i++)
    {
        Sum += abs(Orig.w - tex2D(smplShadowTex, TexelKernel[i]).w);
        fMinDepth = min(fMinDepth, tex2D(smplShadowTex, TexelKernel[i]).w);
    }
    float fEdgeVal = saturate(Sum - (10.0f/256.0f)) * 256.0f;
    float fEdgeFlag = (fEdgeVal > 0.0f) ? fOneItem : 0;
    float fEdgeDepth = (fEdgeVal > 0.0f) ? fMinDepth : 1;
    float fEdgeBool = (fEdgeVal > 0.0f) ? 1.0f : 0.0f;
    float4 vRetVal = float4(Orig.w, fEdgeDepth, fEdgeFlag, fEdgeBool);
    return vRetVal;
}

//-----------------------------------------------------------------------------
// Pixel Shader: PostProcessDistanceH
// Desc: Performs distance search
//-----------------------------------------------------------------------------
/*
float4 PostProcessPS_Distance(uniform float fStart, float2 Tex : TEXCOORD0) : COLOR0
{
    float2 TexelKernel[4];
    TexelKernel[0] = float2(Tex.x           , Tex.y + tSize);
    TexelKernel[1] = float2(Tex.x + tSize   , Tex.y);
    TexelKernel[2] = float2(Tex.x           , Tex.y - tSize);
    TexelKernel[3] = float2(Tex.x - tSize   , Tex.y);

    float4 Orig = tex2D(smplShadowTex, Tex);
    float fMinDepth = Orig.y;
    float fCount = 1.0f;
    if (Orig.y == 1.0f)
    {
        fCount = 0.0f;
        fMinDepth = 0.0f;
    }
        
    float fMax = Orig.z;
    for (int i=0; i<4; i++)
    {
        float4 fVal = tex2D(smplShadowTex, TexelKernel[i]);
        fMax = max(fMax, fVal.z);
//        fMinDepth = min(fMinDepth, fVal.y);
//        if (fVal.y < 1.0f)
        if ((fVal.y < 1.0f) && (fVal.y < Orig.x))
        {
            fMinDepth += fVal.y;
            fCount += 1.0f;
        }
    }
    if (fCount == 0.0f)
        fMinDepth = 1.0f;
    else
        fMinDepth = fMinDepth / fCount;
    
    float fEdgeFlag = (fMax > 0) ? Orig.z + fOneItem : 0;
    //Preserve the egde depth!
    float fEdgeDepth = (Orig.w > 0.5f) ? Orig.y : fMinDepth;
    float4 vRetVal = float4(Orig.x, fEdgeDepth, fEdgeFlag, Orig.w);
    return vRetVal;
}
*/

float4 PostProcessPS_Distance(uniform float fStart, float2 Tex : TEXCOORD0) : COLOR0
{
    float2 TexelKernel[8];
    TexelKernel[0] = float2(Tex.x           , Tex.y + tSize);
    TexelKernel[1] = float2(Tex.x + tSize   , Tex.y);
    TexelKernel[2] = float2(Tex.x           , Tex.y - tSize);
    TexelKernel[3] = float2(Tex.x - tSize   , Tex.y);

    TexelKernel[4] = float2(Tex.x - tSize   , Tex.y - tSize);
    TexelKernel[5] = float2(Tex.x + tSize   , Tex.y - tSize);
    TexelKernel[6] = float2(Tex.x - tSize   , Tex.y + tSize);
    TexelKernel[7] = float2(Tex.x + tSize   , Tex.y + tSize);

    float4 Orig = tex2D(smplShadowTex, Tex);
    float fMinDepth = Orig.y;
    float fMax = Orig.z;
    for (int i=0; i<4; i++)
    {
        float4 fVal = tex2D(smplShadowTex, TexelKernel[i]);
        fMax = max(fMax, fVal.z);
        fMinDepth = min(fMinDepth, fVal.y);
    }
    for (int i=4; i<8; i++)
    {
        float4 fVal = tex2D(smplShadowTex, TexelKernel[i]);
        fMax = max(fMax, fVal.z);
        fMinDepth = min(fMinDepth, fVal.y);
    }
    float fEdgeFlag = (fMax > 0) ? Orig.z + fOneItem : 0;
    float fEdgeDepth = fMinDepth;
    float4 vRetVal = float4(Orig.x, fEdgeDepth, fEdgeFlag, Orig.w);
    return vRetVal;
}

//-----------------------------------------------------------------------------
// Technique: PostProcess
// Desc: Performs post-processing effect that detects and highlights edges.
//-----------------------------------------------------------------------------
technique PostProcess
{
    pass p0
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Edge();
        ZEnable = false;
    }
    pass p1
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(0);
        ZEnable = false;
    }
    pass p2
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(4);
        ZEnable = false;
    }
    pass p3
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(8);
        ZEnable = false;
    }
    pass p4
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p5
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p6
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p7
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p8
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p9
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(0);
        ZEnable = false;
    }
    pass p10
    {     
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(4);
        ZEnable = false;
    }
    pass p11
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(8);
        ZEnable = false;
    }
    pass p12
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p13
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p14
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p15
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }
    pass p16
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
        ZEnable = false;
    }

    ///next wave
//    pass xp1
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(0);
//        ZEnable = false;
//    }
//    pass xp2
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(4);
//        ZEnable = false;
//    }
//    pass xp3
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(8);
//        ZEnable = false;
//    }
//    pass xp4
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }    
//    pass xp5
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp6
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp7
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp8
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp11
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(0);
//        ZEnable = false;
//    }
//    pass xp12
//    {     
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(4);
//        ZEnable = false;
//    }
//    pass xp13
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(8);
//        ZEnable = false;
//    }
//    pass xp14
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp15
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp16
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp17
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
//    pass xp18
//    {
//        VertexShader = NULL;
//        PixelShader = compile ps_2_0 PostProcessPS_Distance(12);
//        ZEnable = false;
//    }
}
