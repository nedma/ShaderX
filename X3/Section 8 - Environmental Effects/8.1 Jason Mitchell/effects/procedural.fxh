//==================================================================================================//
// filename: procedural.fxh                                                                         //
//                                                                                                  //
// author:   Jason L. Mitchell                                                                      //
//           ATI Research, Inc.                                                                     //
//           3D Application Research Group                                                          //
//           email: JasonM@ati.com                                                                  //
//                                                                                                  //
// Description: Utility routines for procedural shading                                             //
//                                                                                                  //
// Contains various routines in the following areas:                                                //
//                                                                                                  //
//   smoothstep3 - vectorized smoothstep                                                            //
//                                                                                                  //
// $Header: //depot/3darg/Demos/Internal/D3DXEffectDemos_DX9/StatueDemo/effects/procedural.fxh#6 $  //
//                                                                                                  //
//==================================================================================================//
//   (C) 2003 ATI Research, Inc.  All rights reserved.                                              //
//==================================================================================================//


//////////////////////////////////////////////////////////////////////////////////////
//
// Vectorized smoothstep which performs three smoothsteps in parallel
//
// Input:
//   - Three float3s which contain {edge0, edge1, width, x} for 3 different smoothsteps
//     width is edge0 - edge1, which can sometimes pre-computed, so we leave it to the caller
//
// Return value:
//   - Result of three different smoothsteps
//
float3 smoothstep3 (float3 edge0, float3 edge1, float3 OneOverWidth, float3 x)
{
   // Scale, bias and saturate x to the range of zero to one
   x = saturate((x - edge0) * OneOverWidth);

   // Evaluate polynomial
   return x*x*(3-2*x); 
}
