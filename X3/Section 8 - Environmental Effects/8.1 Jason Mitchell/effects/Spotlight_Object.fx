//===================================================================================================//
// filename: Spotlight_Object.fx                                                                     //
//                                                                                                   //
// author:   Jason L. Mitchell                                                                       //
//           ATI Research, Inc.                                                                      //
//           3D Application Research Group                                                           //
//           email: JasonM@ati.com                                                                   //
//                                                                                                   //
// Description: Effect file for Spotlight object rendering                                           //
//                                                                                                   //
//===================================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                               //
//===================================================================================================//

#include "noise.fxh"
#include "stone.fxh"
#include "utilities.fxh"

// Global matrices
MATRIX matWorldViewProj;
MATRIX matLightWorldViewProj;
MATRIX matWorldView;
MATRIX matWorldLight;
MATRIX matITWorldView;
MATRIX matWorldLightProjBias;
MATRIX matWorldLightProjScroll1;
MATRIX matWorldLightProjScroll2;

VECTOR g_Lmodel;
VECTOR g_Vmodel;


FLOAT fDepthScaleFactor;

TEXTURE tCookie;
TEXTURE tShadowMap;
TEXTURE tScrollingNoise;
TEXTURE tAmbientOcclusion;
TEXTURE tAmbientCube;

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

sampler AmbientOcclusionSampler = sampler_state
{
   Texture = (tAmbientOcclusion);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
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

sampler ShadowMapSampler = sampler_state
{
   Texture = (tShadowMap);

   MinFilter = Point;
   MagFilter = Point;
   MipFilter = None;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
};

sampler AmbientCubeSampler = sampler_state
{
   Texture = (tAmbientCube);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = None;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
};



//
// Structures
//

struct VS_OUTPUT
{
   float4 Pos           : POSITION;

   // Texture coordinates
   float2 tcBase        : TEXCOORD0;
   float3 Pmodel        : TEXCOORD1;
   float4 tcProjCookie  : TEXCOORD2;
   float4 tcProjScroll1 : TEXCOORD3;
   float4 tcProjScroll2 : TEXCOORD4;
   float4 P_depth       : TEXCOORD5;
};

struct VS_OUTPUT_POS
{
   float4 Pos : POSITION;
   float3 P   : TEXCOORD0;
};


struct VS_DEPTH_OUTPUT
{
   float4 Pos   : POSITION;
   float  depth : TEXCOORD0;
};

// ===============================================================================================
//
// Vertex Shaders
//
// ===============================================================================================
// Vertex Shader which sets up for Phong lighting and a projective texture
VS_OUTPUT vs_main (float4 vPosition : POSITION, float3 vNormal : NORMAL, float2 vTexCoord0 : TEXCOORD0)
{
   VS_OUTPUT Out = (VS_OUTPUT) 0;

   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);

   // Output position and normal in eye space for subsequent Phong shading
   Out.Pmodel = vPosition;

   // Output projective coordinates for cookie
   Out.tcProjCookie = mul (matWorldLightProjBias, vPosition);

   // Output scrolling projective coordinates for noise map
   Out.tcProjScroll1 = mul (matWorldLightProjScroll1, vPosition);
   Out.tcProjScroll2 = mul (matWorldLightProjScroll2, vPosition);
   
   // Position in projective light space
   float4 lsPos = mul (matLightWorldViewProj, vPosition);

   // Output scaled model coordinates and projective light space depth
   Out.P_depth.xyz = vPosition.xyz * float3(1, 4.1, 1);  // Stretch out the marble pattern in a nice way
   Out.P_depth.w   = lsPos.z / fDepthScaleFactor;

   // Pass through the base map texture coordinates
   Out.tcBase = vTexCoord0;

   return Out;
}

// Vertex Shader which puts interpolated depth into scalar texture coordinate
VS_DEPTH_OUTPUT vs_depth (float4 vPosition : POSITION)
{
   VS_DEPTH_OUTPUT Out = (VS_DEPTH_OUTPUT) 0; 
 
   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition); // should be equivalent to matLightWorldViewProj on this use of the shader

   // Output depth
   Out.depth = Out.Pos.z / fDepthScaleFactor;

   return Out;
}


// ===============================================================================================
//
// Pixel shaders
//
// ===============================================================================================
float4 ps_main ( // Varying inputs
                 float4 Pos           : POSITION,

                 // Texture coordinates
                 float2 tcBase        : TEXCOORD0,
                 float3 Pmodel        : TEXCOORD1,
                 float4 tcProjCookie  : TEXCOORD2,
                 float4 tcProjScroll1 : TEXCOORD3,
                 float4 tcProjScroll2 : TEXCOORD4,
                 float4 P_depth       : TEXCOORD5,

                 // Uniform inputs to generate shader permutations
         uniform bool   bScrollingNoise,
         uniform bool   bShadowMapping,
         uniform bool   bUseAmbientCube,
         uniform bool   bUseAmbientOcclusion) : COLOR
{
   // These scalars are later set to something other than 1.0f, conditionally
   float compositeNoise = 1.0f;
   float shadow = 1.0f;
   float4 ambientColor = {0.4f, 0.4f, 0.4f, 1.0f};

   float3 Nmodel;             // Model space normal
   float ambientOcclusion;    // Ambient occlusion term

   // Sample normal and ambient occlusion term from map
   float4(Nmodel, ambientOcclusion) = tex2D(AmbientOcclusionSampler, tcBase);
   Nmodel = normalize(bx2(Nmodel));

   //
   // Lighting
   //

   float dist = length(g_Lmodel - Pmodel) / 20.0f;

   float3 Lmodel = normalize(g_Lmodel - Pmodel);         // Compute L in model space
   float3 Vmodel = normalize(g_Vmodel - Pmodel);         // Compute V in model space

   float3 Hmodel = normalize(Lmodel + Vmodel);           // Compute half-angle

   float NdotL   = dot(Nmodel, Lmodel);                  // N.L
   float diffuse = saturate(NdotL - 0.5f);               // Horizon scooted to hide shadow map artifacts

   float NdotH = clamp(dot(Nmodel, Hmodel), 0.0f, 1.0f); // Compute N.H

   float NdotH_2  = NdotH    * NdotH;
   float NdotH_4  = NdotH_2  * NdotH_2;
   float NdotH_8  = NdotH_4  * NdotH_4;
   float NdotH_16 = NdotH_8  * NdotH_8;

   // Conditionally perform shadow mapping
   if (bShadowMapping)
   {
      float shadowMapDepth = tex2Dproj(ShadowMapSampler, tcProjCookie);

      // If the pixel's depth is in front of that stored in the shadow map
      if (P_depth.w < (shadowMapDepth + 0.015f))
      {
         shadow = 1.0f; // Then the object is in light
      }
      else
      {
         shadow = 0.0f; // Else, it is in shadow
      }
   }

   // Sample the cookie
   float4 cookie = tex2Dproj(CookieSampler, tcProjCookie);

   // Conditionally include the scrolling noise
   if (bScrollingNoise)
   {
      float4 noise1 = tex2Dproj(ScrollingNoiseSampler, tcProjScroll1);
      float4 noise2 = tex2Dproj(ScrollingNoiseSampler, tcProjScroll2);

      compositeNoise = noise1.r + noise2.g;
   }


   // Conditionally use the ambient occlusion term
   if(!bUseAmbientOcclusion)
   {
      ambientOcclusion = 1.0f;
   }

   // Conditionally use the ambient cube map
   if(bUseAmbientCube)
   {
      ambientColor = ambientOcclusion * texCUBE(AmbientCubeSampler, Nmodel);
   }
   else
   {
      ambientColor *= ambientOcclusion;
   }

   // Compute the albedo using a standard marble function
   float4 albedo = marble(P_depth.xyz, 1.0f);       // gloss in w
   
   // Final composite of albedo and lighting
   float4 result = {1.0f, 1.0f, 1.0f, 1.0f};
   result.xyz = shadow * cookie.xyz * (diffuse * compositeNoise * albedo.xyz  + NdotH_16 * albedo.w) + 0.4f * ambientColor * albedo.xyz;
   return result;
}


// simple marble shader
float4 ps_marble (float3 P : TEXCOORD0) : COLOR
{
   return marble(P, 1.0f);
}

// Just pass through the interpolated depth
float4 ps_depth (float depth : TEXCOORD0) : COLOR
{
   return depth;
}

// Just pass red
float4 ps_red (void) : COLOR
{
   return float4(1.0f, 0.0f, 0.0f, 0.0f);
}

// Just pass black
float4 ps_black (void) : COLOR
{
   return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


// ===============================================================================================
// 
// Techniques
// 
// ===============================================================================================


//---- Marble albedo ------------------------------------------------------


technique noise_shadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(true, true, true, true);

      SrcBlend = DestColor;
      DestBlend = Zero;
      AlphaBlendEnable = FALSE;

      CullMode = CW;
   }
}

technique noise_noshadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(true, false, true, true);

      SrcBlend = DestColor;
      DestBlend = Zero;
      AlphaBlendEnable = FALSE;

      CullMode = CW;
   }
}

technique nonoise_shadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(false, true, true, true);

      SrcBlend = DestColor;
      DestBlend = Zero;
      AlphaBlendEnable = FALSE;

      CullMode = CW;
   }
}

technique nonoise_noshadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_main();
      PixelShader  = compile ps_2_0 ps_main(false, false, true, true);

      SrcBlend = DestColor;
      DestBlend = Zero;
      AlphaBlendEnable = FALSE;

      CullMode = CW;
   }
}



//
//---- Other techniques ------------------------------------------------------
//

//
// Special depth technique for rendering into shadow map
//
technique depth_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_depth();
      PixelShader  = compile ps_2_0 ps_depth();

      CullMode = CW;
   }
}

//
// Special black technique for rendering into the black "shadow map"
//
technique black_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_depth();
      PixelShader  = compile ps_2_0 ps_black();

      CullMode = CW;
   }
}
