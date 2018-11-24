#ifndef __RESOURCES_H
#define __RESOURCES_H


enum eRenderMode {
	BASE = 0,	// color+normal/z, uses mrt
	COLOR,		// color only - fallback for no-mrt case
	NORMALZ,	// normal/z only - fallback for no-mrt case
	SHADOW,		// shadow
	MAX_MODES
};


/// Shadow map texture size
const int SHADOW_MAP_SIZE = 1024;
/// Lake reflection texture size
const int LAKE_REFL_SIZE = 256;

#define RID_HEIGHTMAP "data/tex/heightmap.png"

/// Shadow map RT
#define RID_SHADOWMAP "shadowMap"


/// Lake reflection RT
#define RID_LAKEREFL "lakeRefl"
#define RID_LAKEREFLZ "lakeReflZ"

#define RID_RGB2HSV "RGB2HSV"
#define RID_HSV2RGB "HSV2RGB"


/// Main screen-sized RT
#define RID_BASERT "baseRT"
/// 512x512 RT
#define RID_FIXEDRT512 "fixRT512"
/// Screen-sized RT to hold normals/depth (RGB-normals, A-depth)
#define RID_NORMALZ "normalz"
/// Screen-sized RT to hold detected edges
#define RID_EDGES "edges"


#define RID_BLEEDACC "bleedAcc"
#define RID_QUANTIZED "quantized"


#define RID_TERRAIN "terrain"
#define FX_TERRAIN "terrain.fx"


#endif
