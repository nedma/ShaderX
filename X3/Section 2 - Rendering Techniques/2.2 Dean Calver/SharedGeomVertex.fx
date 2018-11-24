// vertex functions and variables that are used in multiple geometry shaders
// included from the actual fx file

// shared vertex parameter block
// The parameters are shared by all meshs and contain important per render data

// these will be usually changed per objects but sometimes they might not be so we share them
shared float4x4 matWorldView;			// world view matrix
shared float4x4 matWorldViewProjection;	// world view projection matrix (the 'main' one from local to projection)
shared float4x4 matTranInvWorldView;	// transpose of the inverse of the world view matrix
