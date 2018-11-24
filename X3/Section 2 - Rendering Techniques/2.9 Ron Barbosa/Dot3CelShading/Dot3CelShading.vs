// Vertex shader input structure
struct VS_INPUT
{
	float4	Pos:		POSITION;
	float3	vNorm:		NORMAL;
};

// Vertex shader output structure
struct VS_OUTPUT 
{
	float4	Pos:		POSITION;		// Clip space vertex position
	float	Scale:		TEXCOORD;		// Color scaling factor
};

// Constant registers
float4x4	view_proj_matrix:	register(c0);	// Concatenated View-Projection matrix
float4		view_position:		register(c4);	// World space camera position
float4x4	world_matrix:		register(c5);	// Object-to-world matrix

// Vertex Shader entry point
VS_OUTPUT vs_main(VS_INPUT inData)
{
	// Declare output structure
	VS_OUTPUT Out;

	// Transform the input data into world space
	inData.Pos   = mul(world_matrix, inData.Pos);
	inData.vNorm = mul((float3x3)world_matrix, inData.vNorm);

	// Calculate and normalize the eye vector.  The dot product
	// of the normalized eye vector and the vertex normal will
	// be used to scale the color such that vertices facing the
	// camera appear brighter and taper off towards the edges.
	float3 vEye = normalize(view_position - inData.Pos);

	// Store the scale as a texture coordinate (float) in the
	// output structure.
	Out.Scale = dot(inData.vNorm, vEye);

	// Transform the vertex into clip-space
	Out.Pos = mul(view_proj_matrix, inData.Pos);
	
	// Return the clip-space vertex and the color scale
	return Out;
}