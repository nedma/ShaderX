// Fallback for non-MRT cards. Normal/Z only part.
// Versions: vs1.1, ps1.1

#include "_library.fx"

// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass P0 {
		VertexShader = <vshNormalZNoMRT>;
		PixelShader = <pshNormalZNoMRT>;
	}
	pass PLast {
	}
}
