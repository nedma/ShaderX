//=============================================================================================//
// filename: stone.fxh                                                                         //
//                                                                                             //
// author:   Jason L. Mitchell                                                                 //
//           ATI Research, Inc.                                                                //
//           3D Application Research Group                                                     //
//           email: JasonM@ati.com                                                             //
//                                                                                             //
// Description: Utility routines for marble albedo generation                                  //
//                                                                                             //
// Contains various routines in the following areas:                                           //
//                                                                                             //
//   marble - Basic marble as in RenderMan Companion                                           //
//   strata - Sedimentary rock strata by Musgrave                                              //
//                                                                                             //
// $Header: //depot/3darg/Demos/Internal/D3DXEffectDemos_DX9/StatueDemo/effects/stone.fxh#6 $  //
//                                                                                             //
//=============================================================================================//
//   (C) 2003 ATI Research, Inc.  All rights reserved.                                         //
//=============================================================================================//


TEXTURE tMarbleSpline; // For marble
TEXTURE tStrataSpline; // For strata

sampler MarbleSplineSampler = sampler_state
{
   Texture = (tMarbleSpline);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

sampler StrataSplineSampler = sampler_state
{
   Texture = (tStrataSpline);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Wrap;
   AddressV  = Wrap;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// marble - Simple marble routine
//
// Input:
//   - 3D shader-space position.  Model coordinates are typically used
//   - scalar frequency applied to shader space position
//
// Return value:
//   - Marble albedo
//
float4 marble (float3 P, float fFrequency)
{
   float m = -2.0f * snoise(P * fFrequency) + 0.75f;

   // Cubic interpolation of f along color spline (gloss in alpha)
   return tex1D (MarbleSplineSampler, m);
}



//////////////////////////////////////////////////////////////////////////////////
//
// strata - surface shader for sedimentary rock strata
//
// Description:
//    Makes sedimentary rock strata, useful for rendering landscapes.
//
// Input:
//   - 3D shader-space position.  Model coordinates are typically used
//   - scalar frequency applied to shader space position
//
// Return value:
//   - Marble albedo
//
// Author:
//    C language version by F. Kenton Musgrave
//    Translation to Shading Language by Larry Gritz.
//    Translation to DirectX 9 HLSL by Jason L. Mitchell (JasonM@ati.com)
//
// References:
//    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
//    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
//    Academic Press, 1994.  ISBN 0-12-228760-6.

float4 strata (float3 P, float fFrequency, float fNoiseAmplitude)
{
   // Scale shader space
   float3 PP = fFrequency * P;

   // Sample strata color spline
   return tex1D (StrataSplineSampler, PP.y + fNoiseAmplitude * snoise(PP));
}