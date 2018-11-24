Efficient Omnidirectional Shadow Maps Demo
Gary King
Will Newhall

This program implements the concepts presented in the article.  See the errata section below for a
description of some changes that occurred to the demo after the article was submitted.


Requirements:  
DirectX 9.0c
PS 2.0-compliant video card with support for R32F cubemaps or depth textures
Tested on: GeForce 6800, QuadroFX Go 700, GeForce FX 5900 Ultra, Radeon 9800XT.  
           ForceWare 61.71 or later, Catalyst 3.9 or later.

File List:
Readme.txt:  This file
Bounding.h / Bounding.cpp:  Various bounding volume classes, helper functions, and intersection tests
BoundingUtil.h:  Auxiliary functions called from Bounding.cpp (linked directly)
LookUpTables.h:  Precomputed lookup tables, used by Bounding.cpp
PracticalCubeShadowMaps.h / PracticalCubeShadowMaps.cpp:  Main program
Framework.h:  Auxiliary functions called from PracticalCubeShadowMaps.cpp
MouseUIs.h:  Camera control
OmniDirectionalShadowMap.h:  Abstract OmniDirectionalShadowMap class declaration
R32FCubeShadowMap.h / R32FCubeShadowMap.cpp:  Implementation of OmniDirectionalShadowMap, using R32F cubemaps
VSDCT.h / VSDCT.cpp:  Implementation of OmniDirectionalShadowMap, using VSDCT shadow maps
NVBScene9.h / NVBScene9_DS.cpp:  Utilities to read the NVB data files used by the demo
*.fx:  D3DX Effect files containing render state, pixel and vertex shaders for the various render modes in the demo


NOTE: Hardware shadow maps (VSDCTs) only render correctly with the Direct3D retail runtime.  
Shadows will not appear with the debug runtime.  R32F cubemap shadows render correctly in both.

Usage:
The control scheme is similar to most first-person shooters.  Hold down the left mouse button to look freely.
W, A, S, D, Page Up, and Page Down will move the camera.  Shift+key "runs" 10x faster than normal.  Other keys are:

1: Select R32F Cubemap Shadow Technique (on supported video cards)
2: Select VSDCT Shadow Technique (on supported video cards)
SPACE: Toggle Animation
~: Toggle Debug View (only supported in VSDCT mode)
F1: Show Help
F2: Show Stats (framerate and culling)
F3: Toggle Shadow Map Filtering
F4: Display Scissor Rectangle
F5: Toggle Shadow Caster Culling
F6: Toggle Dynamic Shadow Map Resizing
[/]: Increase light intensity
+/-: Change depth bias
</>: Change slope-scale depth bias (only affects VSDCT)

Debug visualization shows the relative sizes of all rendered shadow faces; however, it is only supported when
rendering with VSDCTs.

Errata

1.  The shadow caster vs. frustum culling algorithm has changed from the one described in the article.  Instead
    of building a bounding frustum for the shadow caster, the demo now computes the silhouette edges of the bounding
    box (using half-space marking, and some precomputed lookup tables).  The intersection test is similar to 
    Frustum vs. Frustum, but it must handle 4 or 6 planes (as there are either 4 or 6 silhouette edges per
    bounding box).  This change provides higher culling quality than the algorithm presented in the article.
    
2.  The extruded bounding box vs. frustum test is not very efficient, and the demo scene includes lots of tiny
    shadow casters (200+, depending on the view position & direction).  As a result, it is possible to become CPU
    limited, especially on fast GPUs.  The source code includes a "magic number" ExtrusionTestThreshold that
    determines when to just render tiny casters without doing full intersection tests.  In Debug builds, caster culling
    can cause the overall framerate to decrease.  Also, the extrusion test isn't capped to the "near plane" of the
    caster in all cases, so better culling could be achieved if this were fixed.
    
3.  The VSDCT implementation comes in 2 varieties: Full Indirection and Limited Indirection.  These refer to the
    resolution of the indirection cubemap (full-res and reduced-res (compared to a shadow map face), respectively).
    The demo only implements equally-sized faces, so the limited indirection implementation isn't strictly required
    (it could be implemented using a mipmapped full indirection cubemap); however, we chose to implement the demo
    this way to show how a program could dynamically alter cubemap face resolutions arbitrarily -- the limited
    indirection cubemaps give the program flexibility to change shadow map edge length per-face, so that more
    "important" faces occupy larger sections of the VSDCT.
    
4.  Recreating shadow maps (swapping between VSDCT & R32F, or between dynamic & static VSDCT) may cause 1 frame
    of graphical corruption.
    
5.  There seems to be a bug with the handling of D3DSAMP_MIPMAPLODBIAS on ATI Radeon GPUs.  
    An alternate .fx file for R32F cubemaps, R32FCubemapRadeon (which uses texCUBEbias) 
    is used on all Radeons to work around this problem.