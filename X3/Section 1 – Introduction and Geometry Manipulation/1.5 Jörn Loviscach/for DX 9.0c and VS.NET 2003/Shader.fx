float4x4 WorldViewProj;
float4 LightPosition;
float4 AmbientColor;
float4 DiffuseColor;
float4 SpecularColor;
float SpecularPower;
float4x4 ViewI;
float4 CurvedSilhouetteMode;

struct EdgesAppData
{
	float3 p  : POSITION;
	float3 n  : NORMAL;
	float3 pa : TEXCOORD0; // only for curved silhouettes, not for halo or shadow volume
	float3 n1 : TEXCOORD1;
	float3 n2 : TEXCOORD2;
	float2 uv : TEXCOORD3;
};

struct SubPolyAppData
{
	float3 p1 : POSITION;
	float3 p2 : TEXCOORD0;
	float3 p3 : TEXCOORD1;
	float3 n1 : TEXCOORD2;
	float3 n2 : TEXCOORD3;
	float3 n3 : TEXCOORD4;
	float2 uv : TEXCOORD5;
};

struct VertexOutput
{
	float4 p : POSITION;
	float2 uv : TEXCOORD0;
};

struct CurvedSilhouetteVertexOutput
{
	float4 p : POSITION;
	float4 c : COLOR0;
	float3 uv : TEXCOORD0;
};

struct PixelOutput
{
	float4 c : COLOR0;
};

VertexOutput EdgesVertexShader(EdgesAppData IN)
{
	VertexOutput OUT;
	
	OUT.uv = IN.uv;

	half3 v = ViewI[3] - IN.p;
	half n1v = dot(IN.n1, v);
	half n2v = dot(IN.n2, v);
	half3 nPerp;		
	if(n1v*n2v > 0.0) // no silhouette
	{
		OUT.p = float4(100.0, 100.0, 100.0, 1.0);
		return OUT;	
	}
	else
	{
		nPerp = normalize(IN.n - dot(IN.n,v)*v/dot(v,v));
		half3 pos = IN.p + 0.1*IN.uv.y*nPerp;
		OUT.p = mul(half4(pos, 1.0), WorldViewProj);
		return OUT;		
	}
}

VertexOutput SubPolyVertexShader(SubPolyAppData IN)
{
	VertexOutput OUT;
	
	OUT.uv = IN.uv;
	
	half nv1 = dot(IN.n1, normalize(ViewI[3] - IN.p1));
	half nv2 = dot(IN.n2, normalize(ViewI[3] - IN.p2));
	half nv3 = dot(IN.n3, normalize(ViewI[3] - IN.p3));
	
	half3 pa = IN.p1;
	half3 na = IN.n1;
	half nva = nv1;
	bool foundSilhouette = true;
	if(nv1*nv2 > 0.0) // no silhouette on the edge from vertex 1 to 2
	{
		if(nv2*nv3 > 0.0) // no silhouette at all
		{
			foundSilhouette = false;
			// With DirectX 9.0b we may not yet write:
			// OUT.p = float4(100.0, 100.0, 100.0, 1.0);
			// return OUT;		
		}
		else
		{
			pa = IN.p3;
			na = IN.n3;
			nva = nv3;
		}
	}

	if(foundSilhouette)
	{	
		half3 p = nv2*pa - nva*IN.p2;
		half3 n = nv2*na - nva*IN.n2;
		half3 pos = (p + 0.1*IN.uv.y*n)/(nv2-nva);
		OUT.p = mul(half4(pos, 1.0), WorldViewProj);		
	}
	else
	{
		OUT.p = float4(100.0, 100.0, 100.0, 1.0);	
	}
	return OUT;
}

PixelOutput HaloPixelShader(VertexOutput IN)
{
	PixelOutput OUT;
	OUT.c = half4(0.0, 1.0, 1.0, 1.0-IN.uv.y);
	return OUT;
}

VertexOutput EdgesShadowVolumeVertexShader(EdgesAppData IN)
{
	VertexOutput OUT;
	
	OUT.uv = IN.uv;
	
	half3 v = LightPosition.xyz - IN.p;
	half n1v = dot(IN.n1, v);
	half n2v = dot(IN.n2, v);	
	if(n1v*n2v > 0.0) // no silhouette
	{
		OUT.p = float4(100.0, 100.0, 100.0, 1.0);
		return OUT;	
	}
	else
	{
		if(n1v > 0.0)
		{
			OUT.uv = float2(1.0, 1.0)-IN.uv; // change orientation of quad
		}
		half4 pos = float4(IN.p, 1.0) - OUT.uv.y*float4(LightPosition.xyz, 1.0);
		OUT.p = mul(pos, WorldViewProj);
		return OUT;		
	}
}

VertexOutput SubPolyShadowVolumeVertexShader(SubPolyAppData IN)
{
	VertexOutput OUT;
	
	OUT.uv = IN.uv;
	
	half nv1 = dot(IN.n1, normalize(LightPosition.xyz - IN.p1));
	half nv2 = dot(IN.n2, normalize(LightPosition.xyz - IN.p2));
	half nv3 = dot(IN.n3, normalize(LightPosition.xyz - IN.p3));

	half3 pa = IN.p1;
	half nva = nv1;
	bool foundSilhouette = true;
	if(nv1*nv2 > 0.0) // no silhouette on the edge from vertex 1 to 2
	{
		if(nv2*nv3 > 0.0) // no silhouette at all
		{
			foundSilhouette = false;
			// With DirectX 9.0b we may not yet write:
			// OUT.p = float4(100.0, 100.0, 100.0, 1.0);
			// return OUT;		
		}
		else
		{
			pa = IN.p3;
			nva = nv3;
		}
	}

	if(foundSilhouette)
	{
		half3 p = (nv2*pa - nva*IN.p2)/(nv2-nva);
		if(nv1 > 0.0)
		{
			OUT.uv = float2(1.0, 1.0)-IN.uv; // change orientation of quad
		}		
		half4 pos = float4(p, 1.0) - OUT.uv.y*float4(LightPosition.xyz, 1.0);
		OUT.p = mul(pos, WorldViewProj);	
	}
	else
	{
		OUT.p = float4(100.0, 100.0, 100.0, 1.0);	
	}
	return OUT;
}

PixelOutput SimplePixelShader(VertexOutput IN)
{
	PixelOutput OUT;
	OUT.c = half4(IN.uv.x, IN.uv.y, 0.0, 1.0);
	return OUT;
}

CurvedSilhouetteVertexOutput CurvedSilhouetteVertexShader(EdgesAppData IN)
{
	CurvedSilhouetteVertexOutput OUT;
	
	OUT.uv = float3(0.0, 0.0, 0.0);
	OUT.c = float4(0.0, 0.0, 0.0, 0.0);
    
	half3 v = ViewI[3] - IN.p;
	half n1v = dot(IN.n1, v);
	half n2v = dot(IN.n2, v);	
	if(n1v*n2v > 0.0) // no silhouette
	{
		OUT.p = float4(100.0, 100.0, 100.0, 1.0);
		return OUT;	
	}
	else
	{	
		half3 dp = IN.p - IN.pa;
		half3 view = normalize(v);
		OUT.uv = float3(IN.uv.x, IN.uv.y, dot(IN.n, dp - dot(view, dp)*view));
		half3 light = normalize(LightPosition.xyz - IN.p);
		half4 illu = lit(dot(IN.n, light), dot(IN.n, normalize(light+view)), SpecularPower);
		OUT.c = AmbientColor + DiffuseColor*illu.y + SpecularColor*illu.z;		
		half3 e = normalize(n2v*(0.5-IN.uv.x)*cross(dp, v));
		half3 pos = IN.p + 0.1*IN.uv.y*e;
		OUT.p = mul(half4(pos, 1.0), WorldViewProj);
		return OUT;		
	}
}

PixelOutput CurvedSilhouettePixelShader(CurvedSilhouetteVertexOutput IN)
{
	PixelOutput OUT;
	if(CurvedSilhouetteMode.x + CurvedSilhouetteMode.z > 0.5)
		clip(float4(IN.uv.x*(1.0-IN.uv.x)*IN.uv.z - 0.1*IN.uv.y, 1.0, 1.0, 1.0));
	OUT.c = CurvedSilhouetteMode.x * IN.c + (CurvedSilhouetteMode.y + CurvedSilhouetteMode.z) * float4(1,0,0,1);
	return OUT;
}

technique edges
{
	pass p0 
	{
		VertexShader = compile vs_2_0 EdgesVertexShader();
		PixelShader = compile ps_2_0 HaloPixelShader();
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZWriteEnable = false;
	}
}

technique subPoly
{
	pass p0 
	{
		VertexShader = compile vs_2_0 SubPolyVertexShader();
		PixelShader = compile ps_2_0 HaloPixelShader();
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZWriteEnable = false;
	}
}

technique edgesShadowVolume
{
	pass p0 
	{
		VertexShader = compile vs_2_0 EdgesShadowVolumeVertexShader();
		PixelShader = compile ps_2_0 SimplePixelShader();
	}
}

technique subPolyShadowVolume
{
	pass p0 
	{
		VertexShader = compile vs_2_0 SubPolyShadowVolumeVertexShader();
		PixelShader = compile ps_2_0 SimplePixelShader();
	}
}

technique curvedSilhouette
{
	pass p0 
	{
		VertexShader = compile vs_2_0 CurvedSilhouetteVertexShader();
		PixelShader = compile ps_2_0 CurvedSilhouettePixelShader();
	}
}