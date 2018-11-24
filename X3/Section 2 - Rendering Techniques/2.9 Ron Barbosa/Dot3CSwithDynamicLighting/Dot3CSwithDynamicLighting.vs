// Vertex shader input structure
struct VS_INPUT
{
	float4	Pos:	POSITION;
	float3	vNorm:	NORMAL;
};

// Vertex shader output structure
struct VS_OUTPUT 
{
	float4	Pos:	POSITION;
	float	Scale:	TEXCOORD0;			// Edge width scale factor
	float	NdotL:	TEXCOORD1;			// N.L for lighting calculation
	float3	vNorm:	TEXCOORD2;			// World space normal
	float3	vEye:	TEXCOORD3;			// Eye vector for specular highlight
	float3	vLight:	TEXCOORD4;			// Light direction vector
};

// Constant registers
float4x4	view_proj_matrix:	register(c0);
float4		view_position:		register(c4);
float4x4	world_matrix:		register(c5);
float4		light_position:		register(c9);

// Vertex Shader entry point
VS_OUTPUT vs_main(VS_INPUT inData)
{
	// Declare output structure
	VS_OUTPUT Out;

	// Transform the vertex and normal into world space.
	inData.Pos = mul(inData.Pos, world_matrix);
	Out.vNorm  = mul(inData.vNorm, (float3x3)world_matrix);

	// Calculate the eye vector.  The dot product of the eye
	// vector and the vertex normal will be used with a
	// threshold value to determine whether or not a pixel
	// qualifies as being an edge pixel.
	Out.vEye   = normalize(view_position  - inData.Pos);

	// Calculate the light vector.  This will be used to
	// determine the color of pixels that are not considered
	// to be edge pixels.
	Out.vLight = normalize(light_position - inData.Pos);

	// Store the scale and the N.L values as texture coordinates
	// in the output structure.  These values are required for
	// determining the final color of the rendered pixels.
	Out.Scale = dot(Out.vNorm, Out.vEye);
	Out.NdotL = dot(Out.vNorm, Out.vLight);
	
	// Transform the vertex into clip-space
	Out.Pos = mul(view_proj_matrix, inData.Pos);
	
	// Return the output structure.
	return Out;
}