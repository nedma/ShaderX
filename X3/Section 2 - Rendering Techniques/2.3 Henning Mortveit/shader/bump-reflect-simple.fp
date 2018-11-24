!!ARBfp1.0

# 
# Assumes: same as reflect.fp
#

ATTRIB texCoord = fragment.texcoord[0]; 
ATTRIB x0       = fragment.texcoord[1]; # world coords of fragment
ATTRIB t        = fragment.texcoord[2]; # world tangent of fragment

PARAM MTexGen[4]= { program.local[0..3] };  # -
PARAM n         = program.local[4];     # plane coefficients, world
PARAM x         = program.local[5];     # viewpoint
#do not need h in this case
PARAM fT        = program.local[7];     # tangent 
PARAM fB        = program.local[8];     # bitangent
PARAM fN        = program.local[9];     # normal 

TEMP r0, r1; # scratch space

#
# no guarantee for w = 1 in interpolation, so make sure.

TEMP X0;           # fragment position with w = 1
RCP X0.w, x0.w;
MUL X0, x0, X0.w;

#
# TexGen for reflection texture. Store in r1.

DP4 r1.x, MTexGen[0], X0;
DP4 r1.y, MTexGen[1], X0;
DP4 r1.z, MTexGen[2], X0;
DP4 r1.w, MTexGen[3], X0;

#
# texture lookup.

TXP r0, r1, texture[2], 2D;        # reflection
TEX r1, texCoord, texture[0], 2D;  # base

#
#

TEMP normal;
TXP normal.xyz, texCoord, texture[1], 2D;
MAD normal.xyz, normal, 2.0, -1.0; 

#
# Transform normal to world coordinates

TEMP N1;
MUL N1.xyz, normal.x, fT;
MAD N1.xyz, normal.y, fB, N1;
MAD N1.xyz, normal.z, fN, N1;  # world aligned normal at fragment

PARAM lv = {0.57735, 0.57735, 0.57735, 0.0};
TEMP r2;
DP3 r2.w, lv, N1;

LRP r0, 0.70, r0, r1;
MUL result.color, r2.w, r0;

END 
