//============================================================================================//
// filename: Spotlight.fx                                                                     //
//                                                                                            //
// author:   Jason L. Mitchell                                                                //
//           ATI Research, Inc.                                                               //
//           3D Application Research Group                                                    //
//           email: JasonM@ati.com                                                            //
//                                                                                            //
// Description: Effect file for Spotlight rendering                                           //
//                                                                                            //
//============================================================================================//
//   (C) 2004 ATI Research, Inc.  All rights reserved.                                        //
//============================================================================================//

// Global matrices
MATRIX matWorldViewProj;
MATRIX matWorldView;
MATRIX matITWorldView;
VECTOR g_Leye;

VECTOR lightColor;

// Textures
TEXTURE tCookie;

// Samplers
sampler CookieSampler = sampler_state
{
   Texture = (tCookie);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Clamp;
   AddressV  = Clamp;
   AddressW  = Clamp;
//   MaxAnisotropy = 8;
};


//
// Vertex shader output structures
//

// Shell of "light" geometry
struct VS_FRUSTUM_OUTPUT
{
   float4 Pos   : POSITION;
   float3 Peye  : TEXCOORD0;
   float3 Neye  : TEXCOORD1;
};

// Textured "cookie" of light geometry
struct VS_FRUSTUM_FRONT_OUTPUT
{
   float4 Pos : POSITION;
   float2 tc  : TEXCOORD0;
};

// Solid wireframe (position only)
struct VS_WIRE_FRUSTUM_OUTPUT
{
   float4 Pos   : POSITION;
   float4 Color : COLOR0;
};


// Vertex Shader for shaded "light source" at tip of frustum
VS_FRUSTUM_OUTPUT frustum_vs (float4 vPosition : POSITION, float3 vNormal : NORMAL)
{
   VS_FRUSTUM_OUTPUT Out = (VS_FRUSTUM_OUTPUT) 0; 

   // Output position and normal in eye space for subsequent Phong shading
   Out.Peye = mul (matWorldView, vPosition);
   Out.Neye = mul (matITWorldView, vNormal);

   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);

   return Out;
}


// Vertex Shader for textured front of frustum
VS_FRUSTUM_FRONT_OUTPUT frustum_front_vs (float4 vPosition : POSITION, float3 vNormal : NORMAL, float2 tc : TEXCOORD0)
{
   VS_FRUSTUM_FRONT_OUTPUT Out = (VS_FRUSTUM_FRONT_OUTPUT) 0; 

   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);

   // Pass texture coordinates through
   Out.tc = tc;

   return Out;
}


// Vertex Shader for wireframe frustum
VS_WIRE_FRUSTUM_OUTPUT wire_frustum_vs (float4 vPosition : POSITION, float4 vColor : COLOR0)
{
   VS_WIRE_FRUSTUM_OUTPUT Out = (VS_WIRE_FRUSTUM_OUTPUT) 0; 

   // Output clip-space position
   Out.Pos = mul (matWorldViewProj, vPosition);

   Out.Color = vColor;

   return Out;
}

//
//
// Pixel shaders
//
//

float4 frustum_ps (float3 Peye : TEXCOORD0, float3 Neye : TEXCOORD1) : COLOR
{
   float3 Leye = normalize(g_Leye - Peye);            // Compute normalized vector from vertex to light in eye space  (Leye)
                                                      
   Neye = normalize(Neye);                            // Normalize interpolated normal
                                                      
   float3 Veye = normalize(-Peye);                    // Compute Veye

   float3 Heye = normalize(Leye + Veye);              // Compute half-angle

   float NdotL = dot(Neye, Leye);                     // N.L

   float diffuse = NdotL * 0.5f + 0.5f;               // "Half-Lambert" technique for more pleasing diffuse term

   float NdotH = clamp(dot(Neye, Heye), 0.0f, 1.0f);  // Compute N.H

   float NdotH_2  = NdotH    * NdotH;
   float NdotH_4  = NdotH_2  * NdotH_2;
   float NdotH_8  = NdotH_4  * NdotH_4;
   float NdotH_16 = NdotH_8  * NdotH_8;
   float NdotH_32 = NdotH_16 * NdotH_16;

   return NdotH_32 * NdotH_32 + diffuse * lightColor;
}

float4 frustum_front_ps (float2 tc  : TEXCOORD0) : COLOR
{
   return lightColor * tex2D (CookieSampler, tc);
}

float4 wire_frustum_ps (float4 color : COLOR0) : COLOR
{
   return color;
}


//
//
//  Techniques which use the shaders defined above
//
//

// Used on the outside of the frustum
technique frustum_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 frustum_vs();
      PixelShader  = compile ps_2_0 frustum_ps();

      CullMode = CCW;
      ZFunc = LESSEQUAL;
   }
}

// Used on the "cookie" covering the light source
technique frustum_front_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 frustum_front_vs();
      PixelShader  = compile ps_2_0 frustum_front_ps();

      CullMode = CCW;
      ZFunc = LESSEQUAL;
   }
}

// Used on the wireframe frustum
technique wire_frustum_technique
{
   pass P0
   {
      VertexShader = compile vs_1_1 wire_frustum_vs();
      PixelShader  = compile ps_2_0 wire_frustum_ps();

      CullMode = CCW;
      ZFunc = LESSEQUAL;
      ZWriteEnable = FALSE;
   }
}
