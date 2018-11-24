//-----------------------------------------------------------------------------
// File: BasicHLSL.fx
//
// Desc: The effect file for the BasicHLSL sample.  It implemetns 
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "HLSL_Function_defs.fx"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
 
// Colors for gradient 
float4 g_Scolor = { 0.0f, 0.5f, 1.0f, 1.0f };				// Gradient Color 1
float4 g_Ecolor = { 1.0f, 0.5f, 0.0f, 1.0f };				// Gradient Color 2


texture g_RenderTexture;								// Full screen render target texture 
 
float	 g_fTime;											// App's time in seconds
float4x4 g_mWorld;											// World matrix for object
float4x4 g_mWorldViewProjection;							// World * View * Projection matrix

float g_srange=0;											// seed range/radius of animation

// Constants for Smooth Coloring
float1 il=1/log(2);
float1 lp= log(log(4.0));

float g_fact=0.5f;											// zom factor
float g_speed=0.0f;											// animation speed
float2 shift;												// shift x/y
bool gJulia=false;											// julia mode true/false

float iterFac=0.1;											// Iteration fac = 1/MaxIter

float bailout=16;											// bailout = bailout^2
//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
 
sampler FormulaSampler = 
sampler_state
{
    Texture = <g_RenderTexture>;    
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
 

//-----------------------------------------------------------------------------
// Name: RenderSceneVS
// Type: Vertex shader                                      
// Desc: This shader Simply Projects coordinates and outputs it to pixel shader
//-----------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
  
	// Transform the position from object space to homogeneous projection space
    Output.Position = mul(vPos, g_mWorldViewProjection);
    
    // Transform the normal from object space to world space	
    vNormalWorldSpace = normalize(mul(vNormal, (float3x3)g_mWorld)); // normal (world space)
    
    // Compute simple directional lighting equation
    Output.Diffuse.rgb = 1.0f;   
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
// Name: projectCoords                                        
// Type: Pixel shader
// Desc: This function projects the texture coordinates to complex vie pane
//-----------------------------------------------------------------------------

float2 projectCoords(float2 pos,float2 shift, float scale){
	float2 temp;

	temp.x=(pos.x*2-1)*scale+shift.x;
	temp.y=(pos.y*2-1)*scale+shift.y;

	return temp;

}

//-----------------------------------------------------------------------------
// Name: SeedFunc                                        
// Type: Pixel shader
// Desc: This function is the animation of the fractal seed, it consists
//			of a range and a speed value ... if range is 0 no animation is visible
//-----------------------------------------------------------------------------

float2 SeedFunc(){
 
	return float2(sin(g_speed*0.7)*g_srange ,cos(g_speed*0.5 )*g_srange);

}

//-----------------------------------------------------------------------------
// Name: RenderScenePS                                        
// Type: Pixel shader
// Desc: This shader outputs the pixel's color  
//-----------------------------------------------------------------------------
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
	Output.RGBColor = tex2D(FormulaSampler, In.TextureUV) ;	 
    return Output;
}

//-----------------------------------------------------------------------------
// Name: RenderPS_FirstIter                                        
// Type: Pixel shader
// Desc: This is the initial function, initializing the z values in the texture map
//-----------------------------------------------------------------------------

PS_OUTPUT RenderPS_FirstIter( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;

	float2 Seed=projectCoords(In.TextureUV,shift,g_fact);
	float2 z;
    
	if(!gJulia){
		   z= SeedFunc();
	}else{
		   z= Seed;
	}

	// Return complex value z in r/g color channel !
	Output.RGBColor.r=z.x;
	Output.RGBColor.g=z.y;
	Output.RGBColor.b=0;
	Output.RGBColor.a=0;

    return Output;
}

//-----------------------------------------------------------------------------
// Name: RenderPS_Iter                                        
// Type: Pixel shader
// Desc: This is iterative function, called for each iteration step once
//			if ps supports more than 128 function it is possible to do 
//			the looping direct within this procedure
//-----------------------------------------------------------------------------

PS_OUTPUT RenderPS_Iter( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;

	// The only difference between Julia and Mandelbrot Type is
	// swapping the Z Var and the Seed 
   float2 Seed;
   if(!gJulia){
		 Seed =projectCoords(In.TextureUV,shift,g_fact);;
	}else{
		 Seed =SeedFunc();
	}

	// sample z complex value from last iteration step texture map
	float4 temp=tex2D(FormulaSampler, In.TextureUV).xyzw;
	float2 z= temp.xy;
	float k;
	// This loop is commented out because
	// ps20 Shaders can not compile that many ps instructions
	// ps30 Shaders could run every iteration in a single pass ! 
	//	for(k=0;k<20;k++){
	if(z.x*z.x+z.y*z.y<bailout){
			// "&Function" is translated by the d3dxinclude interface, which has been 
			// modified to include different functions at this point, see c++ source
			#include "&Function"  
			
			// the temp.z value counts the iteration
			temp.z+=iterFac;
    }
	//}

	// Return complex value z in r/g color channel !
	// and if supported iteration counter in b color channel
	Output.RGBColor.r=z.x;
	Output.RGBColor.g=z.y;
	Output.RGBColor.b=temp.z;
	Output.RGBColor.a=0; 
  
    return Output;
}
 

//-----------------------------------------------------------------------------
// Name: RenderPS_Final                                        
// Type: Pixel shader
// Desc: This function finally transforms the z value obtained from texture reads 
//			into a color value
//-----------------------------------------------------------------------------

PS_OUTPUT RenderPS_Final( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;

	// Read result of last iteration step
	float4 temp=tex2D(FormulaSampler, In.TextureUV).xyzw;
	// intialize complex variable
	float2 z=  temp.xy;
 
	if(z.x*z.x+z.y*z.y<bailout){
		// Inside Coloring
		
		// Color by Complex Value
			Output.RGBColor.r=length(z);
			Output.RGBColor.g=z.x;
			Output.RGBColor.b=z.y;
			Output.RGBColor.a=1;
		// End of color by complex value

		// Solid Coloring
		/*
			Output.RGBColor=g_Ecolor;
		*/
		// end of Solid Coloring

	}else{

		// Outside Coloring
		// Coloring by Iteration depth
		/*
			Output.RGBColor=lerp(g_Scolor,g_Ecolor,temp.z);  
		*/
		//end of Coloring 
		
		// Coloring by Complex Values 
		/*
			z=normalize(z);
			Output.RGBColor.g= z.x;
			Output.RGBColor.b= z.y;
			Output.RGBColor.a=1;
		*/
		// end of coloring by Complex values
	
		// Smooth Coloring
			float index= 0.05*(( temp.z*(1/iterFac+1))+il*lp-il*log(log(length(z))));
			Output.RGBColor=lerp(g_Scolor,g_Ecolor,index);  
		// End of Smooth Coloring

	}
 
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
	
		CullMode=none;

        VertexShader = compile vs_1_1 RenderSceneVS();
        PixelShader  = compile ps_2_0 RenderScenePS();  
    }
}
technique FirstIteration
{
    pass P0_Initialization
    {			
	   PixelShader  = compile ps_2_0 RenderPS_FirstIter(); 
    }
    pass P1_Iteration
    {		
       PixelShader  = compile ps_2_0 RenderPS_Iter(); 
    }
    pass P2_Final
    {			
	   PixelShader  = compile ps_2_0 RenderPS_Final();  
    }


}






