//=====================================================================================================//
// filename: Spotlight_Backdrop.fx                                                                     //
//                                                                                                     //
// author:   Jason L. Mitchell                                                                         //
//           ATI Research, Inc.                                                                        //
//           3D Application Research Group                                                             //
//           email: JasonM@ati.com                                                                     //
//                                                                                                     //
// Description: Effect file for Spotlight backdrop rendering                                           //
//                                                                                                     //
//=====================================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                                 //
//=====================================================================================================//

// Global matrices
MATRIX matWorldViewProj;
MATRIX matWorldLightProj;
MATRIX matWorldLight;
MATRIX matWorldLightProjBias;
MATRIX matWorldLightProjScroll1;
MATRIX matWorldLightProjScroll2;

// Textures
TEXTURE tBase;
TEXTURE tCookie;
TEXTURE tScrollingNoise;
TEXTURE tShadowMap;

const float gfFarPlane = 400.0f;

static const float4 gAmbient = {0.3f, 0.3f, 0.3f, 0.3f};

// Samplers
sampler BackdropSampler = sampler_state
{
   Texture = (tBase);

   MinFilter = Anisotropic;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Wrap;
   AddressV  = Wrap;
   AddressW  = Wrap;
   MaxAnisotropy = 8;
};

sampler ScrollingNoiseSampler = sampler_state
{
   Texture = (tScrollingNoise);

   MinFilter = Anisotropic;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Wrap;
   AddressV  = Wrap;
   AddressW  = Wrap;
   MaxAnisotropy = 8;
};

sampler CookieSampler = sampler_state
{
   Texture = (tCookie);

   MinFilter = Anisotropic;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
   MaxAnisotropy = 8;
};

sampler ShadowMapSampler = sampler_state
{
   Texture = (tShadowMap);

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
   float4 Pos           : POSITION;
   float4 Color         : COLOR0;
   float2 tcBase        : TEXCOORD0;
   float4 tcProjCookie  : TEXCOORD2;
   float4 tcProjScroll1 : TEXCOORD3;
   float4 tcProjScroll2 : TEXCOORD4;
   float  depth         : TEXCOORD5;
};

struct VS_DEPTH_OUTPUT
{
   float4 Pos   : POSITION;
   float  depth : TEXCOORD0;
};

struct VS_AMBIENT_OUTPUT
{
   float4 Pos    : POSITION;
   float2 tcBase : TEXCOORD0;
};

//
// Vertex Shaders
//

// Vertex Shader
VS_OUTPUT vs_main (float4 vPosition : POSITION, float3 vNormal : NORMAL, float2 tc : TEXCOORD0, float4 Color : COLOR0)
{
   VS_OUTPUT Out = (VS_OUTPUT) 0; 

   // Pass color and texture coordinates through
   Out.tcBase = tc;
   Out.Color = Color;

   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);

   // Transform to light space
   float3 PosL   = mul (matWorldLight, vPosition);

   // Normalize to max depth in Spotlight volume
   Out.depth = PosL.z / gfFarPlane;

   // Output projective coordinates for cookie
   Out.tcProjCookie = mul (matWorldLightProjBias, vPosition);

   // Output projective coordinates for scrolling noise maps
   Out.tcProjScroll1 = mul (matWorldLightProjScroll1, vPosition);
   Out.tcProjScroll2 = mul (matWorldLightProjScroll2, vPosition);

   return Out;
}


// Vertex Shader which puts interpolated depth into scalar texture coordinate
VS_DEPTH_OUTPUT vs_depth (float4 vPosition : POSITION)
{
   VS_DEPTH_OUTPUT Out = (VS_DEPTH_OUTPUT) 0; 
 
   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);  // should be equivalent to matWorldLightProj for this shader

   // Output scalar depth
   Out.depth = Out.Pos.z / gfFarPlane;

   return Out;
}

// Vertex Shader which puts interpolated depth into scalar texture coordinate
VS_AMBIENT_OUTPUT vs_ambient_only (float4 vPosition : POSITION, float2 tc : TEXCOORD0)
{
   VS_AMBIENT_OUTPUT Out = (VS_AMBIENT_OUTPUT) 0; 
 
   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);

   // Pass texture coordinates through
   Out.tcBase = tc;

   return Out;
}



// =============================================================================================================
//
// Pixel shaders
//
// =============================================================================================================

float4 ps_main (  // Interpolated (varying) inputs
                  float4 ColorTint     : COLOR0,
                  float2 tcBase        : TEXCOORD0,
                  float4 tcProjCookie  : TEXCOORD2,
                  float4 tcProjScroll1 : TEXCOORD3,
                  float4 tcProjScroll2 : TEXCOORD4,
                  float  depth         : TEXCOORD5,

                  // Uniform inputs to generate shader permutations
          uniform bool   bScrollingNoise,
          uniform bool   bShadowMapping) : COLOR
{
   float compositeNoise = 1.0f;
   float shadow = 1.0f;

   // Agressively knock out any back projection
   float fBackProjection = (depth <= 0.1f ? 0.0f : 1.0f);

   // Sample the base map
   float4 baseColor = ColorTint * tex2D(BackdropSampler, tcBase);

   float4 cookie = tex2Dproj(CookieSampler, tcProjCookie);

   // Compute noise for noisy versions of shader
   if (bScrollingNoise)
   {
      float4 noise1 = tex2Dproj(ScrollingNoiseSampler, tcProjScroll1);
      float4 noise2 = tex2Dproj(ScrollingNoiseSampler, tcProjScroll2);

      compositeNoise = noise1.r + noise2.g; // Same map, but different projections and channels used
   }

   // Not really a shadow map, but a grayscale map just for the backdrop
   if (bShadowMapping)
   {
      shadow = tex2Dproj(ShadowMapSampler, tcProjCookie);
   }

//   return fBackProjection;

   // Final composite
   return baseColor * (fBackProjection * shadow * cookie * compositeNoise + gAmbient);
}


//
// Just pass through the interpolated depth
//
float4 ps_depth (float depth : TEXCOORD0) : COLOR
{
   return depth;
}


float4 ps_ambient_only (float2 tcBase : TEXCOORD0) : COLOR
{
   return gAmbient * tex2D(BackdropSampler, tcBase);
}




// =============================================================================================================
//
// Techniques
//
// =============================================================================================================


technique noise_shadow_technique
{
   pass P0 
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(true,   // noise
                                            true);  // shadow
      CullMode = CCW;
   }

   pass P1
   {
      VertexShader = compile vs_1_1 vs_ambient_only();
      PixelShader  = compile ps_2_0 ps_ambient_only();
      CullMode = CW;
   }
}

technique noise_noshadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(true,   // noise
                                            false); // shadow
      CullMode = CCW;
   }

   pass P1
   {
      VertexShader = compile vs_1_1 vs_ambient_only();
      PixelShader  = compile ps_2_0 ps_ambient_only();
      CullMode = CW;
   }

}

technique nonoise_shadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(false, // noise
                                            true); // shadow
      CullMode = CCW;
   }

   pass P1
   {
      VertexShader = compile vs_1_1 vs_ambient_only();
      PixelShader  = compile ps_2_0 ps_ambient_only();
      CullMode = CW;
   }
}

technique nonoise_noshadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(false,  // noise
                                            false); // shadow
      CullMode = CCW;
   }

   pass P1
   {
      VertexShader = compile vs_1_1 vs_ambient_only();
      PixelShader  = compile ps_2_0 ps_ambient_only();
      CullMode = CW;
   }
}



//
// --------------- Special depth technique for rendering into shadow map -------------------------------
//

technique depth_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_depth();
      PixelShader  = compile ps_2_0 ps_depth();

      CullMode = None;
   }
}

