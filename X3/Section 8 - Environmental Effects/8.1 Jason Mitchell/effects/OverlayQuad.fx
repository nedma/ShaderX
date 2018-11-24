//==============================================================================================//
// filename: OverlayQuad.fx                                                                     //
//                                                                                              //
// author:   Jason L. Mitchell                                                                  //
//           ATI Research, Inc.                                                                 //
//           3D Application Research Group                                                      //
//           email: JasonM@ati.com                                                              //
//                                                                                              //
// Description: Effect file for simple overlay quad                                             //
//                                                                                              //
//==============================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                          //
//==============================================================================================//

#define NUM_BLUR_TAPS 12

// Textures
TEXTURE tOverlayTexture;

VECTOR  vClearColor;

float2 filterTaps[NUM_BLUR_TAPS] = {{-0.326212f, -0.405805f},
                                    {-0.840144f, -0.07358f},
                                    {-0.695914f,  0.457137f},
                                    {-0.203345f,  0.620716f},
                                    { 0.96234f,  -0.194983f},
                                    { 0.473434f, -0.480026f},
                                    { 0.519456f,  0.767022f},
                                    { 0.185461f, -0.893124f},
                                    { 0.507431f,  0.064425f},
                                    { 0.89642f,   0.412458f},
                                    {-0.32194f,  -0.932615f},
                                    {-0.791559f, -0.597705f}};


// Samplers
sampler OverlaySampler = sampler_state
{
   Texture = (tOverlayTexture);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
};


// Structures
struct VS_OUTPUT
{
   float4 Pos : POSITION;
   float2 tc0 : TEXCOORD0;
};

//
// Vertex Shaders
//

// Vertex Shader
VS_OUTPUT vs_main (float4 vPosition : POSITION, float2 tc0 : TEXCOORD0)
{
   VS_OUTPUT Out = (VS_OUTPUT) 0; 
   
   Out.tc0 = tc0;       // Pass texture coordinates through
   Out.Pos = vPosition; // Output clip-space position

   return Out;
}


//
// Pixel shaders
//

// Just sample the texture
float4 ps_texture (float2 tc0 : TEXCOORD0) : COLOR
{
   return tex2D(OverlaySampler, tc0);
}

// Just output the clear color
float4 ps_clear (float2 tc0 : TEXCOORD0) : COLOR
{
   return vClearColor;
}


// Routine to blur monochrome image
float4 ps_shadow_blur(float2 tc0 : TEXCOORD0) : COLOR
{
   float colorSum = 0.0f;
   float fScale = 0.02f;

	// Run through all taps in the Poisson Disc
	for (int i = 0; i < NUM_BLUR_TAPS; i++)
	{
		// Compute tap coordinates
		float2 tapCoord = tc0 + filterTaps[i] * fScale * (1.0f-tc0.y);

		// Fetch tap sample
		float tapColor = tex2D(OverlaySampler, tapCoord);

		// Accumulate color and contribution
		colorSum += tapColor;
	}

	// Divide down the accumulated color
	float finalColor = colorSum / NUM_BLUR_TAPS;

	return float4 (finalColor, finalColor, finalColor, finalColor);
}

// Routine to combine four channels of fog buffer into one
float4 ps_composite_fog(float2 tc0 : TEXCOORD0) : COLOR
{
   // Sample the map (4 channels initialized by four different sets of sampling planes)
   float4 fogChannels = tex2D(OverlaySampler, tc0);

   // Sum the channels and divide by four
//   return dot(fogChannels, float4 (0.333f, 0.333f, 0.333f, 0.0f));
   return dot(fogChannels, float4 (0.25f, 0.25f, 0.25f, 0.25f));
}

// Routine to combine four channels of fog buffer into one and apply Poisson filter
float4 ps_composite_filtered_fog(float2 tc0 : TEXCOORD0) : COLOR
{
   float colorSum = 0.0f;
   float fScale = 0.002f;
   float finalColor = 0.0f;

	// Run through all taps in the Poisson Disc
	for (int i = 0; i < 9; i++)
	{
		// Compute tap coordinates
		float2 tapCoord = tc0 + filterTaps[i] * fScale;

		// Fetch tap sample
		float4 tapColor = tex2D(OverlaySampler, tapCoord);

		// Accumulate color and contribution
		colorSum += dot(tapColor, float4 (0.25f, 0.25f, 0.25f, 0.25f));
	}

   // Divide down the accumulated color
   finalColor = colorSum / 9;

   return float4 (finalColor, finalColor, finalColor, finalColor);
}





//
// Techniques
//

technique show_texture_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_texture();

      CullMode = None;

      ZFunc = Always;
      ZWriteEnable = False;
   }
}

technique blend_texture_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_texture();

      CullMode = None;

      ZFunc = Always;
      ZWriteEnable = False;

      SrcBlend = SrcAlpha;
      DestBlend = InvSrcAlpha;
      AlphaBlendEnable = TRUE;


   }
}


technique clear_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_clear();

      CullMode = None;

      ZFunc = Always;
      ZWriteEnable = False;
   }
}

technique shadow_blur_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_shadow_blur();

      CullMode = None;

      ZFunc = Always;
      ZWriteEnable = False;
   }
}

technique composite_fog_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_composite_filtered_fog();
//      PixelShader  = compile ps_2_0 ps_composite_fog();


      CullMode = None;

      ZFunc = Always;
      ZWriteEnable = False;

      SrcBlend = One;
      DestBlend = One;
      AlphaBlendEnable = TRUE;

      AlphaTestEnable = TRUE;       // Alpha test out any black pixels
      AlphaRef = 0x00000000;
      AlphaFunc = Greater;
      
   }
}


