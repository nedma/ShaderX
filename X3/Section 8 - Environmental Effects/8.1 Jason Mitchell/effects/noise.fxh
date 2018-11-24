//=============================================================================================//
// filename: noise.fxh                                                                         //
//                                                                                             //
// author:   Jason L. Mitchell                                                                 //
//           ATI Research, Inc.                                                                //
//           3D Application Research Group                                                     //
//           email: JasonM@ati.com                                                             //
//                                                                                             //
// Description: Utility routines for generating various types of noise                         //
//                                                                                             //
// Contains various routines including:                                                        //
//                                                                                             //
//   snoise - Signed 3D noise                                                                  //
//                                                                                             //
// $Header: //depot/3darg/Demos/Internal/D3DXEffectDemos_DX9/StatueDemo/effects/noise.fxh#6 $  //
//                                                                                             //
//=============================================================================================//
//   (C) 2003 ATI Research, Inc.  All rights reserved.                                         //
//=============================================================================================//


TEXTURE tVolumeNoise;

sampler NoiseSampler = sampler_state
{
   Texture = (tVolumeNoise);

   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;
   AddressU  = Wrap;
   AddressV  = Wrap;
   AddressW  = Wrap;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// Unsigned noise routine
//
// Input:
//   - 3D position in space of noise
//
// Return value:
//   - Unsigned scalar noise value in 0..1 range
//
float noise (float3 x)
{
    return tex3D (NoiseSampler, x);
}



//////////////////////////////////////////////////////////////////////////////////////
//
// Signed noise routine
//
// Input:
//   - 3D position in space of noise
//
// Return value:
//   - Signed scalar noise value in -1..1 range
//
float snoise (float3 x)
{
    return 2.0f * tex3D (NoiseSampler, x) - 1.0f;
}


