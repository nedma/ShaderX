This document is included to detail the workings of the vertex
shader code for Dot3 Cel Shading with Dynamic Lighting.  This
effect was not discussed in the text of the Dot3 Cel Shading
article, therefore, a thorough examination of the code will be
given here.

The first block of code defines the vertex shader input structure.
The vertex shader only requires the vertex model-space position and
normal vectors.

// Vertex shader input structure
struct VS_INPUT
{
	float4	Pos:	POSITION;
	float3	vNorm:	NORMAL;
};

The following code block defines the output structure.  This
structure includes many members whose details will become clear
as the examination continues:
	Pos:	Clip-space vertex position used for rasterization.
	Scale:	This value is used by the pixel shader code to scale
			the thickness of the silhouette.  A value of 0 will
			result in no silhouette, while a value of 1 will
			produce a render with no color bands and all pixels
			drawn using the edge_color constant.
	NdotL:	This is the value of N.L and is used for lighting.
	vNorm:	This is the world-space vertex normal to be passed
			to the pixel shader for interpolation across the
			face of the polygon.
	vEye:	The world-space "eye" vector.  This direction vector
			is required for calculating specular highlights.
	vLight:	The world-space light direction vector.  This vector
			is also used for specular highlighting.

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

The constant registers defined below are standard transformation matrices and
the world space light position.

// Constant registers
float4x4	view_proj_matrix:	register(c0);
float4		view_position:		register(c4);
float4x4	world_matrix:		register(c5);
float4		light_position:		register(c9);

Below is the vertex shader entry point.  It accepts a VS_INPUT structure as
input and returns a VS_OUTPUT structure, which is declard on the first line
of code.

// Vertex Shader entry point
VS_OUTPUT vs_main(VS_INPUT inData)
{
	// Declare output structure
	VS_OUTPUT Out;

The body of the function works in the following manner.  First, all vertex
data is transformed into world space.

	// Transform the vertex and normal into world space.
	inData.Pos = mul(inData.Pos, world_matrix);
	Out.vNorm  = mul(inData.vNorm, (float3x3)world_matrix);

Next, the eye vector is calculated and normalized.  This normalization is
required because the eye vector will be dotted with the vertex normal to
determine the orientation of the vertices (and pixels) relative to the
viewing vector.  Vertices whose normals are orthogonal to the viewing are
typically edge vertices or vertices on the outskirts of harsh model details.

	// Calculate the eye vector.  The dot product of the eye
	// vector and the vertex normal will be used with a
	// threshold value to determine whether or not a pixel
	// qualifies as being an edge pixel.
	Out.vEye   = normalize(view_position  - inData.Pos);

The code that follows calculates and normalizes the light vector.  This
light vector will be used to provide the standard N.L style lighting, as
well as the per-pixel specular highlight.

	// Calculate the light vector.  This will be used to
	// determine the color of pixels that are not considered
	// to be edge pixels.
	Out.vLight = normalize(light_position - inData.Pos);

Below, the vertex normal is dotted with the eye and light vectors for
passage to the pixel shader.  The pixel shader will use these values to
determine whether a pixel lies on the edge of the model's details and
to provide proper color scaling for the N.L lighting component.

	// Store the scale and the N.L values as texture coordinates
	// in the output structure.  These values are required for
	// determining the final color of the rendered pixels.
	Out.Scale = dot(Out.vNorm, Out.vEye);
	Out.NdotL = dot(Out.vNorm, Out.vLight);

The remainder of the vertex shader simply transforms the vertex into
clip-space and returns the fully populated VS_OUTPUT structure.
	
	// Transform the vertex into clip-space
	Out.Pos = mul(view_proj_matrix, inData.Pos);
	
	// Return the output structure.
	return Out;
}

The entire vertex shader code will be duplicated here without interruption
for clarity.  If any sections of the shader code are unclear, refer to the
Dot3 Cel Shading article in the text for a more detailed description.

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