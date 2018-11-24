//-----------------------------------------------------------------------------
// File: BasicHLSL.fx
//
// Desc: The effect file for the BasicHLSL sample.  It implemetns 
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4 g_MaterialAmbientColor;	// Material's ambient color
float4 g_MaterialDiffuseColor;	// Material's diffuse color

// This effect file uses a single directional light
float3 g_LightDir = normalize(float3(1.0f, 1.0f, 1.0f));	// Light's direction in world space
float4 g_LightAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };			// Light's ambient color
float4 g_LightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };			// Light's diffuse color

texture g_RenderTargetTexture;	// Full screen render target texture 
texture g_MeshTexture;			// Color texture for mesh

float	 g_fTime;					// App's time in seconds
float4x4 g_mWorld;					// World matrix for object
float4x4 g_mWorldViewProjection;	// World * View * Projection matrix

float g_fact=0.5f;
float g_speed=1.0f;

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------


sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;    
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 

    float4 Diffuse    : COLOR0;     // vertex diffuse color
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};

float4 VS_DoTwistY(float4 vPos,float t){
		
	float st=sin(t);
	float ct=cos(t);
	float4 vNewPos;
	
	vNewPos.x=vPos.x*ct-vPos.z*st;
	vNewPos.y=vPos.y;
	vNewPos.z=vPos.x*st-vPos.z*ct;
	vNewPos.w=vPos.w;

	return vNewPos;
}
float4 VS_DoTwistX(float4 vPos,float t){
		
	float st=sin(t);
	float ct=cos(t);
	float4 vNewPos;
	
	vNewPos.x=vPos.x;
	vNewPos.y=vPos.y*st-vPos.z*ct;
	vNewPos.z=vPos.y*ct-vPos.z*st;
	vNewPos.w=vPos.w;

	return vNewPos;
}
float4 VS_DoTwistZ(float4 vPos,float t){
		
	float st=sin(t);
	float ct=cos(t);
	float4 vNewPos;
	vNewPos.x=vPos.x*ct-vPos.y*st;
	vNewPos.y=vPos.x*st-vPos.y*ct;
	vNewPos.z=vPos.z;
	vNewPos.w=vPos.w;

	return vNewPos;
}



// This is the method which deforms a single point.
float3 SpherifyDeformer( float3 p) 
{
	float x, y, z;
	float xw,yw,zw,vdist,mfac;
	float dx, dy, dz;
	
        float cx,cy,cz;
cx=0;
cy=0;
cz=0;
float size=2;
float percent=g_fact;

 

	// Spherify the point
	x = p.x; y = p.y; z = p.z;
	xw= x-cx; yw= y-cy; zw= z-cz;
	if(xw==0.0 && yw==0.0 && zw==0.0)xw=yw=zw=1.0f;

	vdist=(float) sqrt(xw*xw+yw*yw+zw*zw);

	mfac=size/vdist;
	dx = xw+sign(xw)*((float) (abs(xw*mfac)-abs(xw))*percent);
	dy = yw+sign(yw)*((float) (abs(yw*mfac)-abs(yw))*percent);
	dz = zw+sign(zw)*((float) (abs(zw*mfac)-abs(zw))*percent);

	// Add The Global Center
	x=dx+cx; y=dy+cy; z=dz+cz;


	p.x = x; p.y = y; p.z = z;

	return p;
}



float4 VS_DoSpherify(float4 vPos,float t){
 
	t=1;

	float4 vnorm;

	float3 vnorm2=SpherifyDeformer(float3(vPos.x,vPos.y,vPos.z));

	return float4(vnorm2,vPos.w);
}
//-----------------------------------------------------------------------------
// Name: RenderSceneVS
// Type: Vertex shader                                      
// Desc: This shader computes standard transform and lighting
//-----------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
  
	// Animation the vertex based on time and the vertex's object space position
//    float4 vAnimatedPos = vPos + float4( sin(g_fTime) * vPos.x/2, 0, 0, 0 );
    //  float4 vAnimatedPos = vPos ; 
   float4 vAnimatedPos = VS_DoTwistZ(vPos,(g_speed+vPos.z*g_fact)) ;
    float4 vAnimatedNormal = VS_DoTwistZ(float4(vNormal,0),(g_speed+vPos.z*g_fact)) ;
   // Output.Normal=vAnimatedNormal;
    // Transform the position from object space to homogeneous projection space
    Output.Position = mul(vAnimatedPos, g_mWorldViewProjection);
    
    // Transform the normal from object space to world space	
    vNormalWorldSpace = normalize(mul(vAnimatedNormal, (float3x3)g_mWorld)); // normal (world space)
    
    // Compute simple directional lighting equation
    Output.Diffuse.rgb = g_MaterialDiffuseColor * g_LightDiffuse * max(0,dot(vNormalWorldSpace, g_LightDir)) + 
                         g_MaterialAmbientColor * g_LightAmbient;   
    Output.Diffuse.a = 1.0f; 
    
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}


//-----------------------------------------------------------------------------
// Pixel shader output structure
//-----------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


//-----------------------------------------------------------------------------
// Name: RenderScenePS                                        
// Type: Pixel shader
// Desc: This shader outputs the pixel's color by modulating the texture's
//		 color with diffuse material color
//-----------------------------------------------------------------------------
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;

    // Lookup mesh texture and modulate it with diffuse
    Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV)* In.Diffuse ;

    return Output;
}



//-----------------------------------------------------------------------------
// Name: RenderScene
// Type: Technique                                     
// Desc: Renders scene to render target
//-----------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {			
		CullMode=CCW; 


        VertexShader = compile vs_1_1 RenderSceneVS();
        PixelShader  = compile ps_1_1 RenderScenePS(); // trival pixel shader (could use FF instead if desired)
    }
}
