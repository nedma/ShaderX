// pixel functions and variables that are used in multiple geometry shaders
// included from the actual fx file

// we store position in the fat framebuffer as post-perspective space 
// the light shader (usually) converts back into view space where lighting takes place
// the range of the position is 0-1 to maximise integer usage
float3 PackPositionForFatFramebuffer( float4 inp )
{
	float3 o;

	o.xyz = inp.xyz / inp.w;
	o.xy = (o.xy * 0.5) + 0.5;
	return o;
}

float3 PackNormalForFatFramebuffer( float3 inp )
{
	return (inp * 0.5) + 0.5;
}
