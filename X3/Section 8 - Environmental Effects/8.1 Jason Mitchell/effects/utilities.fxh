//====================================================================================================//
// filename: utilities.fxh                                                                            //
//                                                                                                    //
// author:   Jason L. Mitchell                                                                        //
//           ATI Research, Inc.                                                                       //
//           3D Application Research Group                                                            //
//           email: JasonM@ati.com                                                                    //
//                                                                                                    //
// Description: Contains common utility routines                                                      //
//                                                                                                    //
//            bx2() - convert values from 0..1 range to -1..1 range                                   //
//            sign_pack() - pack -1..1 values in 0..1 range                                           //
//                                                                                                    //
//  $Header: //depot/3darg/Demos/Internal/D3DXEffectDemos_DX9/StatueDemo/effects/utilities.fxh#2 $     //
//                                                                                                    //
//====================================================================================================//
//   (C) 2003 ATI Research, Inc.  All rights reserved.                                                //
//====================================================================================================//

//
// Simple routines to mimic _bx2 modifier found in ps_1_x shader models
//

// float4 version
float4 bx2(float4 x)
{
   return 2.0f * x - 1.0f;
}

// float3 version
float3 bx2(float3 x)
{
   return 2.0f * x - 1.0f;
}

// float2 version
float2 bx2(float2 x)
{
   return 2.0f * x - 1.0f;
}

// float version
float bx2(float x)
{
   return 2.0f * x - 1.0f;
}

//
// Simple pack -1..1 values into 0..1
//

// float4 version
float4 sign_pack(float4 x)
{
   return x * 0.5f + 0.5f;
}

// float3 version
float3 sign_pack(float3 x)
{
   return x * 0.5f + 0.5f;
}

// float2 version
float2 sign_pack(float2 x)
{
   return x * 0.5f + 0.5f;
}

// float version
float sign_pack(float x)
{
   return x * 0.5f + 0.5f;
}
