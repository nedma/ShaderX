// Simple effect to output different parameters to multiple render targets

struct PS_MULTIOUT
{
	float4 Buffer0 : COLOR0;
	float4 Buffer1 : COLOR1;
	float4 Buffer2 : COLOR2;
	float4 Buffer3 : COLOR3;
};

PS_MULTIOUT MultiOut( float4 Diffuse : COLOR0, float4 Specular : COLOR1 )
{
	PS_MULTIOUT o;
	o.Buffer0 = Diffuse;
	o.Buffer1 = Specular;
	
	float difInt = Diffuse.r * 0.29 + Diffuse.g * 0.577 + Diffuse.b * 0.133;
	o.Buffer2 = (float4) difInt;
	float specInt = Specular.r * 0.29 + Specular.g * 0.577 + Specular.b * 0.133;
	o.Buffer3 = (float4) specInt;
	
	return o;
}


technique T0
{
	pass P0
	{
		PixelShader = compile ps_2_0 MultiOut();
		SpecularEnable = true;
		ZEnable = true;
		Ambient = 0x000F0F0F;
	}
}