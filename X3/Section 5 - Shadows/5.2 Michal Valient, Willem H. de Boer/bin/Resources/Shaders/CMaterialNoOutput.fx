//;------------------------------
//; Simple constant color shader
//;
//; Desc :
//;   Renders constant colored output (i.e. for zbuffer fill)
//;
//; Part of Dimension3 Renderer
//; (c)2004 Michal Valient
//;------------------------------

uniform float4x4 mToClip : D3_MATRIX_TO_CLIP;

uniform float4 cOutColor : D3_OUTCOLOR = float4(0.0f, 0.0f, 0.0f, 0.0f);

shared float4 VS_MatNoOutput(float4 vPosition : POSITION) : POSITION
{
    return mul(vPosition, mToClip); //vertex clip position
}

shared float4 PS_MatNoOutput() : COLOR
{
    return cOutColor;
}

shared vertexshader VS_MatNoOutput11 = compile vs_1_1 VS_MatNoOutput();

shared pixelshader PS_MatNoOutput11 = compile ps_1_1 PS_MatNoOutput();

technique T_MatNoOutput11 {
    pass P0
    {
        VertexShader = (VS_MatNoOutput11);
        PixelShader = (PS_MatNoOutput11);
        
        AlphaBlendEnable = false;
        
        ZEnable = true;
        ZFunc = LESSEQUAL;
        ZWriteEnable = true;
        
        Cullmode = CCW;

        //ColorWriteEnable = 0;
    }
}
