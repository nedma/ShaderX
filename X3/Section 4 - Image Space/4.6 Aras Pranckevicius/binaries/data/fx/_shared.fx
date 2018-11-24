// shared effect parameters

#ifndef __SHARED_FX
#define __SHARED_FX

static const float PI = 3.1415926535897932384626433832795029f;

shared float4x4 mWorld;
shared float4x4 mWVP;
shared float4x4 mWorldView;

// view matrix
shared float4x4	mView;
// projection matrix
shared float4x4 mProj;
// view*projection matrix
shared float4x4 mViewProj;
// eye position
shared float3   vEye;

// light direction
shared float3	vLightDir = -normalize( float3(0.5,-0.7,0.3) );

// time
shared float	fTime;

// shadow map projection matrices for 4xRGAA
shared float4x4 mShadowProj0;
shared float4x4 mShadowProj1;
shared float4x4 mShadowProj2;
shared float4x4 mShadowProj3;

// texture projection matrix
shared float4x4 mTexProj;

// x,y - offset for full-screen quads; z,w - zeros
// the offsets must be 0.5/screenX, 0.5/screenY
shared float4	vQuadOffset;

// scattering params
shared float3	vScatterBetaR;
shared float3	vScatterBetaM;
shared float3	vScatterBetaR_;
shared float3	vScatterBetaM_;
shared float3	vScatterBetaSum;
//shared float3	vScatterInvBetaSum;
shared float3	vScatterSunColor;
shared float3	vScatterSunColorDivBetaSum;
shared float4	vScatterHGg;
shared float	fScatterExtMult = 1;
shared float	fScatterInscatMult = 1;


#endif
