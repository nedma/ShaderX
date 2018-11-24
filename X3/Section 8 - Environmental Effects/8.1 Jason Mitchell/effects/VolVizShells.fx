//===============================================================================================//
// filename: VolVizShells.fx                                                                     //
//                                                                                               //
// author:   Jason L. Mitchell                                                                   //
//           ATI Research, Inc.                                                                  //
//           3D Application Research Group                                                       //
//           email: JasonM@ati.com                                                               //
//                                                                                               //
// Description: Effect file for vol viz light shaft rendering                                    //
//                                                                                               //
//===============================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                           //
//===============================================================================================//

// Global variables

MATRIX matProj;                      // Viewer's current projection
MATRIX matWorldViewProj;             // Viewer's current wvp

MATRIX matInvView;                   // From light space to world space

MATRIX matViewWorldLight;            // View-space to light space matrix
MATRIX matViewWorldLightProj;        // From view-space to projective light space
MATRIX matViewWorldLightProjBias;    // From view-space to biased projective light space
MATRIX matViewWorldLightProjScroll1; // From view-space to scrolling projective light space
MATRIX matViewWorldLightProjScroll2; // From view-space to scrolling projective light space

FLOAT fDepthScaleFactor;

VECTOR wireColor;                    // Color of lines for debug/wireframe techniques
VECTOR lightColor;                   // Color of light to be modulated with cookie
FLOAT  fFractionOfMaxShells;         // Scale factor based on number of shells actually drawn

VECTOR vMinBounds;                   // View-space bounding volume minimum vert
VECTOR vMaxBounds;                   // View-space bounding volume minimum vert

TEXTURE tCookie;                     // Textures
TEXTURE tScrollingNoise;
TEXTURE tShadowMap;

// Spotlight controls



// Samplers
sampler CookieSampler = sampler_state
{
   Texture = (tCookie);

   MinFilter = Anisotropic;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
   MaxAnisotropy = 16;
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
   MaxAnisotropy = 16;
};


sampler ShadowMapSampler = sampler_state
{
   Texture = (tShadowMap);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = None;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
   MaxAnisotropy = 16;
};



// Structures
struct VS_OUTPUT_1
{
   float4 Pos         : POSITION;
   float4 tcProj      : TEXCOORD0;
   float4 lsPos_depth : TEXCOORD3;
};

struct VS_OUTPUT_SCROLLING_NOISE_1
{
   float4 Pos           : POSITION;
   float4 tcProj        : TEXCOORD0;
   float4 tcProjScroll1 : TEXCOORD1;
   float4 tcProjScroll2 : TEXCOORD2;
   float4 lsPos_depth   : TEXCOORD3;
   float4 ChannelMask   : COLOR0;
};


// Structures
struct VS_WIRE_OUTPUT
{
   float4 Pos    : POSITION;
   float4 Color  : COLOR0;
};



// Vertex Shader for displaying slice planes in view space
VS_WIRE_OUTPUT vs_span_bounds (float4 vPosition : POSITION)
{
   VS_WIRE_OUTPUT Out = (VS_WIRE_OUTPUT) 0; 

   // Must stretch the planes in z to compensate for drawing fewer than all of the planes in the VB
   float4 vStretchedMaxBounds = vMaxBounds;
   vStretchedMaxBounds.z = vStretchedMaxBounds.z / fFractionOfMaxShells;

   // Trilerp position within view-space-axis-aligned bounding volume of light's frustum
   float4 pos = vMinBounds * vPosition + (vStretchedMaxBounds * (1.0f - vPosition));
   pos.w = 1.0f;

   // Output clip-space position
   Out.Pos = mul (matProj, pos);

   Out.Color = vPosition;

   return Out;
}


// Vertex Shader for displaying slice planes frozen to a specific point of view
VS_WIRE_OUTPUT vs_span_frozen_bounds (float4 vPosition : POSITION)
{
   VS_WIRE_OUTPUT Out = (VS_WIRE_OUTPUT) 0; 

   // Must stretch the planes in z to compensate for drawing fewer than all of the planes in the VB
   float4 vStretchedMaxBounds = vMaxBounds;
   vStretchedMaxBounds.z = vStretchedMaxBounds.z / fFractionOfMaxShells;

   // Trilerp position within view-space-axis-aligned bounding volume of light's frustum
   float4 pos = vMinBounds * vPosition + (vStretchedMaxBounds * (1.0f - vPosition));
   pos.w = 1.0f;

   // Go back to world space
   pos = mul (matInvView, pos);

   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, pos);

   Out.Color = vPosition;

   return Out;
}


// Vertex Shader which generates proper light-space texture coordinates
VS_OUTPUT_1 vs_project_1 (float4 vPosition : POSITION)
{
   VS_OUTPUT_1 Out = (VS_OUTPUT_1) 0; 

   // Must stretch the planes in z to compensate for drawing fewer than all of the planes in the VB
   float4 vStretchedMaxBounds = vMaxBounds;
   vStretchedMaxBounds.z = vStretchedMaxBounds.z / fFractionOfMaxShells;

   // Trilerp position within view-space-axis-aligned bounding volume of light's frustum
   float4 pos = vMinBounds * vPosition + (vStretchedMaxBounds * (1.0f - vPosition));
   pos.w = 1.0f;

   // Output clip-space position
   Out.Pos = mul (matProj, pos);

   // Texture coordinates are projected and biased light-space position
   Out.tcProj = mul (matViewWorldLightProjBias, pos);

   // Position in projective light space
   float4 lsPos = mul (matViewWorldLightProj, pos);

   // Output scalar depth in projective light space
   Out.lsPos_depth.w = lsPos.z / fDepthScaleFactor;

   // Light space position for scattering equations
   Out.lsPos_depth.xyz = mul (matViewWorldLight, pos);

   return Out;
}

// Vertex Shader which generates proper light-space texture coordinates
VS_OUTPUT_SCROLLING_NOISE_1 vs_project_scrolling_noise_1 (float4 vPosition : POSITION, float4 vChannelMask : COLOR0)
{
   VS_OUTPUT_SCROLLING_NOISE_1 Out = (VS_OUTPUT_SCROLLING_NOISE_1) 0; 

   // Must stretch the planes in z to compensate for drawing fewer than all of the planes in the VB
   float4 vStretchedMaxBounds = vMaxBounds;
   vStretchedMaxBounds.z = vStretchedMaxBounds.z / fFractionOfMaxShells;

   // Trilerp position within view-space-axis-aligned bounding volume of light's frustum
   float4 pos = vMinBounds * vPosition + (vStretchedMaxBounds * (1.0f - vPosition));
   pos.w = 1.0f;

   // Output clip-space position
   Out.Pos = mul (matProj, pos);

   // Texture coordinates are projected and biased light-space position
   Out.tcProj = mul (matViewWorldLightProjBias, pos);

   // Texture coordinates are projected and scrolled light-space position
   Out.tcProjScroll1 = mul (matViewWorldLightProjScroll1, pos);
   Out.tcProjScroll2 = mul (matViewWorldLightProjScroll2, pos);

   // Position in projective light space
   float4 lsPos = mul (matViewWorldLightProj, pos);

   // Output scalar depth in projective light space
   Out.lsPos_depth.w = lsPos.z / fDepthScaleFactor; // normalize so 1.0f is at far plane

   // Light space position for scattering equations
   Out.lsPos_depth.xyz = mul (matViewWorldLight, pos);

   // Channel mask for rendering monochrome smoke into multiple channels of an RGBA buffer
   Out.ChannelMask = vChannelMask;

   return Out;
}



//
// Pixel shaders
//

// Use a solid color
float4 ps_solid_color (float4 color : COLOR0) : COLOR
{
   return float4(0.05f, 0.05f, 0.05f, 0.05f);
}

// Just use interpolated color
float4 ps_debug_color (float4 color : COLOR0) : COLOR
{
   return color;
}


float4 ps_main ( // Varying inputs
                 float4 tcProj        : TEXCOORD0,
                 float4 tcProjScroll1 : TEXCOORD1,
                 float4 tcProjScroll2 : TEXCOORD2,
                 float4 lsPos_depth   : TEXCOORD3,
                 float4 ChannelMask   : COLOR0,

                 // Uniform inputs to generate shader permutations
         uniform bool   bScrollingNoise,
         uniform bool   bShadowMapping) : COLOR
{

   // These scalars are later conditionally set to something other than 1.0f
   float compositeNoise = 0.015f;
   float shadow = 1.0f;
   float4 cookie = {1.0f, 1.0f, 1.0f, 1.0f};

   float shadowMapDepth;
   float4 output;

   // Sample the cookie and shadow map using same texture coordinates
   cookie         = tex2Dproj(CookieSampler, tcProj);
   shadowMapDepth = tex2Dproj(ShadowMapSampler, tcProj);

   if (bScrollingNoise)
   {
      float4 noise1 = tex2Dproj(ScrollingNoiseSampler, tcProjScroll1);
      float4 noise2 = tex2Dproj(ScrollingNoiseSampler, tcProjScroll2);

      compositeNoise = noise1.r * noise2.g * 0.05f;
   }

   if (bShadowMapping)
   {
      // If the pixel's depth is in front of depth sampled from the 2nd-depth shadow map
      if (lsPos_depth.w < shadowMapDepth)
      {
         shadow = 1.0f; // The pixel is in light
      }
      else
      {
         shadow = 0.0f; // The pixel is occluded
      }
   }

   // Compute attenuation 1/(s^2)
   float atten = 0.25f + 20000.0f / dot(lsPos_depth.xyz, lsPos_depth.xyz);
   float scale = 9.0f / fFractionOfMaxShells;

   // Composite scalar result and route to appropriate channel
   float intensity = compositeNoise * cookie.r * scale * atten * shadow;
   output = intensity * ChannelMask;

   return output;
}



// ===============================================================================================
// 
// Techniques
// 
// ===============================================================================================

technique wireframe_technique_clip
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_span_bounds();
      PixelShader  = compile ps_2_0 ps_debug_color();

      CullMode = None;
//      FillMode = Wireframe;

      // Enable additive blending
      AlphaBlendEnable = FALSE;
      DestBlend = ONE;
      SrcBlend = ONE;

      ZWriteEnable = FALSE;

      // Clip to light frustum
      ClipPlaneEnable = CLIPPLANE0 | CLIPPLANE1 | CLIPPLANE2 | CLIPPLANE3 | CLIPPLANE4 | CLIPPLANE5;
   }
}

technique wireframe_technique_noclip
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_span_bounds();
      PixelShader  = compile ps_2_0 ps_debug_color();

      CullMode = None;
//      FillMode = Wireframe;

      AlphaBlendEnable = FALSE;     // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

      ZWriteEnable = FALSE;

      ClipPlaneEnable = 0;          // No clipping
   }
}


technique frozen_wireframe_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_span_frozen_bounds();
      PixelShader  = compile ps_2_0 ps_debug_color();

      CullMode = None;
//      FillMode = Wireframe;

      ZWriteEnable = FALSE;

      AlphaBlendEnable = FALSE;  // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

      ClipPlaneEnable = CLIPPLANE0 | CLIPPLANE1 | CLIPPLANE2 | CLIPPLANE3 | CLIPPLANE4 | CLIPPLANE5;
   }
}





//
// Clipping cases
//
technique clip_nonoise_noshadow_cookie_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_scrolling_noise_1();

      PixelShader  = compile ps_2_0 ps_main(false,    // noise
                                            false);   // shadow

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;

      AlphaRef = 0x00000000;
      AlphaFunc = Greater;
      
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

//      ClipPlaneEnable = 0;
      ClipPlaneEnable = CLIPPLANE0 | CLIPPLANE1 | CLIPPLANE2 | CLIPPLANE3 | CLIPPLANE4 | CLIPPLANE5;
   }
}


technique clip_nonoise_shadow_cookie_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_scrolling_noise_1();
      PixelShader  = compile ps_2_0 ps_main(false,    // noise
                                            true);    // shadow

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
      
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

//      ClipPlaneEnable = 0;          // No clipping
      ClipPlaneEnable = CLIPPLANE0 | CLIPPLANE1 | CLIPPLANE2 | CLIPPLANE3 | CLIPPLANE4 | CLIPPLANE5;
   }
}


technique clip_noise_shadow_cookie_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_scrolling_noise_1();
      PixelShader  = compile ps_2_0 ps_main(true,    // noise
                                            true);   // shadow

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
      
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

//      ClipPlaneEnable = 0;          // No clipping
      ClipPlaneEnable = CLIPPLANE0 | CLIPPLANE1 | CLIPPLANE2 | CLIPPLANE3 | CLIPPLANE4 | CLIPPLANE5;
   }
}

technique clip_noise_noshadow_cookie_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_scrolling_noise_1();
      PixelShader  = compile ps_2_0 ps_main(true,    // noise
                                            false);  // shadow

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
     
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;
      
//      ClipPlaneEnable = 0;          // No clipping
      ClipPlaneEnable = CLIPPLANE0 | CLIPPLANE1 | CLIPPLANE2 | CLIPPLANE3 | CLIPPLANE4 | CLIPPLANE5;
   }
}



//
// Four no clipping cases
//
technique noclip_nonoise_noshadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_1();
      PixelShader  = compile ps_2_0 ps_main(false, false);

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
     
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

      ClipPlaneEnable = 0;          // No clipping
   }
}


technique noclip_nonoise_shadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_1();
      PixelShader  = compile ps_2_0 ps_main(false, true);

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
      
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

      ClipPlaneEnable = 0;          // No clipping
   }
}


technique noclip_noise_shadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_scrolling_noise_1();
      PixelShader  = compile ps_2_0 ps_main(true, true);

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
      
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

      ClipPlaneEnable = 0;          // No clipping
   }
}

technique noclip_noise_noshadow_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 vs_project_scrolling_noise_1();
      PixelShader  = compile ps_2_0 ps_main(true, false);

      CullMode = None;
      FillMode = Solid;
      ZWriteEnable = FALSE;
     
      AlphaBlendEnable = TRUE;      // Enable additive blending
      DestBlend = ONE;
      SrcBlend = ONE;

      ClipPlaneEnable = 0;          // No clipping
   }
}