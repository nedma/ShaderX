This document intends to provide a detailed explanation of the pixel shader
code for Dot3 Cel Shading with Dynamic Lighting.  This effect was not
discussed as part of the Dot3 Cel Shading article in the text, therefore,
a thorough examination of the code will be provided here.

The first block of code defines the pixel shader input structure.  There are
many members of this structure, and there uses will become more clear as the
examination proceeds:
	Scale:	This scale value represents the result of the dot product of the
			eye vector and the interpolated vertex normal.  This value will
			be compared with an application-defined threshold value to
			determine if a pixel lies on the edge of the model's details.
	NdotL:	This value will be used as the material diffuse color scaling
			factor to produce N.L style lighting.  However, since this is a
			non-photorealistic render, the value will not be used directly,
			but will instead be estimated to produce the color banding effect
			common to cel shaded graphics.
	vNorm:	The interpolated vertex normal.  This value will be required when
			calculating the specular highlight.
	vEye:	The interpolated eye vector.  This value is required for producing
			the view-dependant specular highlight.
	vLight:	This light direction vector will also be used for specular lighting.

// Pixel Shader input structure
struct PS_INPUT
{
	float	Scale:	TEXCOORD0;			// Edge width scale factor
	float	NdotL:	TEXCOORD1;			// N.L for lighting calculation
	float3	vNorm:	TEXCOORD2;			// World space interpolated normal
	float3	vEye:	TEXCOORD3;			// Eye vector for specular highlight
	float3	vLight:	TEXCOORD4;			// Light direction vector
};

The constant registers below must be provided values through the application
code.  Many of these constants are detailed in the Dot3 Cel Shading article
in the text:
	shades:			The number of color bands produced by the N.L lighting
					calculation.  This value will be used as part of an
					estimating mechanism that groups pixels together to be
					drawn using a single color.  This creates the color bands
					that are the trademark of cel shaded graphics, in contrast
					to the gradual color blending evident in traditional N.L
					lighting.
	edge_width:		The edge width factor determines how thick the silhouette
					will be around the model's details.  A value of 0 results
					in no silhouette rendering.  A value of 1 eliminates the
					color banding and renders all pixels as edge pixels.  It
					should be noted that the transition is not exactly linear.
	render_color:	This constant stores the material diffuse color used for
					the N.L estimated color banding.
	edge_color:		This constant represents the color of the silhouette.
	ambient_level:	This scalar value represents the grey ambient light level.
					A value of 1.0 will result in full white ambient light.
	shininess:		The specular power component.  Increasing this value will
					focus the specular highlight creating a tighter, more
					intense specular component.

// Constant registers
float	shades:			register(c0);	// Number of color bands in render
float	edge_width:		register(c1);	// Edge width factor
float4	render_color:	register(c2);	// Material diffuse color
float4	edge_color:		register(c3);	// Silhouette color
float	ambient_level:	register(c4);	// Grey ambient light level
float	shininess:		register(c5);	// Specular power

Below is the pixel shader entry point.  It accepts a PS_INPUT structure and
returns a float4 RGBA color which is declared on the first line of code.

// Pixel shader entry point. This shader accepts, as input,
// the output from the vertex shader in the form of a
// PS_INPUT structure.
float4 ps_main( PS_INPUT inData ) : COLOR0
{
	// Output color
	float4 color;

This code block is one of the core components of Dot3 Cel Shading.  This
estimation mechanism allows for a reduction in the resolution of the N.L
color scaling component.  This reduction in resolution results in the color
banding that is evident in cel shaded renders.  While this code is discussed
in great detail in the text, it will also be covered here.

The first line of code declares and integer variable called scalefactor.
This variable is then assigned the integer component of the N.L color scale
multiplied by the number of shades.  Since NdotL will always be in the range
from 0..1 for the visible pixels of front-facing polygons, the parenthetical
expression yields a floating point value between 0 and "shades."  Using the
floor intrinsic function drops the fractional portion, assigning scalefactor
an integer value between 0 and "shades."  Performing the inverse division on
the following line of code returns the value to the 0..1 range, but since the
floor intrinsic dropped the decimal component, the new floating point value
stored in NdotL now has a resolution of 1 / shades.  This estimation process
will create a color banding effect, rather than a color blending effect.  If
this is unclear, refer to the text for a more detailed description with some
examples.

	// Reduce the total number of colors by performing
	// an integer multiplication (and truncation) of the N.L
	// value, performing the inverse division to get back a
	// value in the 0..1 range.
	int scalefactor = floor(inData.NdotL * shades);
	inData.NdotL = scalefactor / shades;

The conditional statement below has changed slightly when compared to the
version discussed in the text.  The code presented in the article tied the
light source to the camera to keep the code short and the explanation to a
minimum.  But with a dynamic light source, the edge pixels of the silhouette
will not necessarily be part of a gradual banded darkening from the interior
pixels to the edges.

Instead, a threshold value must be provided by the application.  This value
will be compared to the interpolated cosine of the angle created between
the eye vector and the vertex normal.  If the cosine of this angle is less
than the threshold value, then the pixel is considered to be part of the
hard edge silhouette.  This is a good, GPU friendly method of determining
where the edges of a model are in relation to the viewer.

As a model is rendered, the vertices aligned with the viewing vector tend
to be directly in front of the viewer.  But the vertices along the edges of
a model tend to be orthogonal to the viewing vector (with cosines
approaching 0).  Defining this threshold value (edge_width) will allow the
user to set the thickness of the hard edge.  Figure 1 in the text provides
a simple diagram that relates the orientation of vertex normals to their
positions relative to the edges of the model.

	// Determine if the eye vector scale is below the threshold
	// for rendering the edge pixels.
	if(inData.Scale < edge_width)
	{
		// This pixel is below the threshold and is part of the
		// hard edge.  Set it to the desired edge color.
		color = edge_color;
	}

The else clause below is executed when a pixel is not considered to be an
edge pixel.  The code below modulates the render color with the reduced
resolution N.L component and adds a bias of 1 color shade.  This bias is
required because the floor intrinsic always rounds down.  Failure to
compensate for this behavior will result in a render that is very dull and
dark.  The code below also adds the modulated ambient color.

	else
	{
		// This is not an edge pixel, scale it and add a bias
		// of 1 shade to avoid overdarkening the image if it
		// is facing the light source.  If it is opposing the
		// light source, color it with ambient lighting.
		color = saturate((render_color * inData.NdotL) + (render_color / shades)) +
						 (render_color * ambient_level);
	}

The code below provides a standard calculation for the per-pixel specular
component.  This combination of realistic lighting calculations with non-
photorealistic rendering creates and interesting effect, especially with an
animated light source.  There are many resources available to provide the
details of the calculations below, and, as they are beyond the scope of Dot3
Cel Shading they will not be addressed here.
	
	// Calculate per-pixel specular.  Begin by normalizing all
	// vectors that are the result of interpolation and may not
	// be of unit length.
	inData.vNorm  = normalize(inData.vNorm);
	inData.vEye   = normalize(inData.vEye);
	inData.vLight = normalize(inData.vLight);
	float3 r = 2 * dot(inData.vNorm, inData.vLight) * inData.vNorm - inData.vLight;
	
The remaining code clamps the specular highlight to avoid doubling of the
specular component and returns the color and specular components.

	// Calculate specular at full white
	float specular = pow(saturate(dot(r, inData.vEye)), shininess);
	
	// Return the scaled color or hard edge with specular component.
	return color + specular;
}

The entire pixel shader code will be duplicated here without interruption
for clarity.  If any sections of the shader code are unclear, refer to the
Dot3 Cel Shading article in the text for a more detailed description.

// Pixel Shader input structure
struct PS_INPUT
{
	float	Scale:	TEXCOORD0;			// Edge width scale factor
	float	NdotL:	TEXCOORD1;			// N.L for lighting calculation
	float3	vNorm:	TEXCOORD2;			// World space interpolated normal
	float3	vEye:	TEXCOORD3;			// Eye vector for specular highlight
	float3	vLight:	TEXCOORD4;			// Light direction vector
};

// Constant registers
float	shades:			register(c0);	// Number of color bands in render
float	edge_width:		register(c1);	// Edge width factor
float4	render_color:	register(c2);	// Material diffuse color
float4	edge_color:		register(c3);	// Silhouette color
float	ambient_level:	register(c4);	// Grey ambient light level
float	shininess:		register(c5);	// Specular power

// Pixel shader entry point. This shader accepts, as input,
// the output from the vertex shader in the form of a
// PS_INPUT structure.
float4 ps_main( PS_INPUT inData ) : COLOR0
{
	// Output color
	float4 color;

	// Reduce the total number of colors by performing
	// an integer multiplication (and truncation) of the N.L
	// value, performing the inverse division to get back a
	// value in the 0..1 range.
	int scalefactor = floor(inData.NdotL * shades);
	inData.NdotL = scalefactor / shades;

	// Determine if the eye vector scale is below the threshold
	// for rendering the edge pixels.
	if(inData.Scale < edge_width)
	{
		// This pixel is below the threshold and is part of the
		// hard edge.  Set it to the desired edge color.
		color = edge_color;
	}
	else
	{
		// This is not an edge pixel, scale it and add a bias
		// of 1 shade to avoid overdarkening the image if it
		// is facing the light source.  If it is opposing the
		// light source, color it with ambient lighting.
		color = saturate((render_color * inData.NdotL) + (render_color / shades)) +
						 (render_color * ambient_level);
	}
	
	// Calculate per-pixel specular.  Begin by normalizing all
	// vectors that are the result of interpolation and may not
	// be of unit length.
	inData.vNorm  = normalize(inData.vNorm);
	inData.vEye   = normalize(inData.vEye);
	inData.vLight = normalize(inData.vLight);
	float3 r = 2 * dot(inData.vNorm, inData.vLight) * inData.vNorm - inData.vLight;
	
	// Calculate specular at full white
	float specular = pow(saturate(dot(r, inData.vEye)), shininess);
	
	// Return the scaled color or hard edge with specular component.
	return color + specular;
}
