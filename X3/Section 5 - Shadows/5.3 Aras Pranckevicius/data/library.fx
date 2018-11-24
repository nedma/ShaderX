#ifndef __LIBRARY_FX
#define __LIBRARY_FX


// Given position, output normalized "to ligth" direction in xyz, distance in w
float4 tolight( float3 pos, float3 lightpos )
{
	float4 tol;
	tol.xyz = lightpos - pos;			// from here to light vector
	float d2 = dot( tol.xyz, tol.xyz );	// distance to light ^2
	tol.w = sqrt( d2 );					// distance to light
	tol.xyz /= tol.w;					// normalized to light direction
	return tol;
}


// Compute lighting
float4 lighting( float3 normal, float4 light, float4 lcolor )
{
	// diffuse
	float diff = saturate( dot(normal, light.xyz) );
	
	// attenuation
	const float MAXDIST = 12;
	float atten;
	if( light.w >= MAXDIST )
		atten = 0;
	else
		atten = 1 - light.w/MAXDIST;

	return (diff*atten) * lcolor;
}


#endif
