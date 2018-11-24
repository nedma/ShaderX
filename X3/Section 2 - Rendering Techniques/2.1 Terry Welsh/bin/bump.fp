!!ARBfp1.0

PARAM light0color = state.light[0].diffuse;
PARAM light1color = state.light[1].diffuse;
PARAM ambient = state.lightmodel.ambient;

TEMP rgb, normal, temp, bump, total;
TEMP light0tsvec, light1tsvec;

# get texture data
TEX rgb, fragment.texcoord[0], texture[0], 2D;
TEX normal, fragment.texcoord[0], texture[1], 2D;

# remove scale and bias from the normal map
MAD normal, normal, 2.0, -1.0;

# normalize the light0 vector
DP3 temp, fragment.texcoord[1], fragment.texcoord[1];
RSQ temp, temp.x;
MUL light0tsvec, fragment.texcoord[1], temp;

# normal dot lightdir
DP3 bump, normal, light0tsvec;

# add light0 color
MUL_SAT total, bump, light0color;

# normalize the light1 vector
DP3 temp, fragment.texcoord[2], fragment.texcoord[2];
RSQ temp, temp.x;
MUL light1tsvec, fragment.texcoord[2], temp;

# normal dot lightdir
DP3 bump, normal, light1tsvec;

# add light1 color
MUL_SAT bump, bump, light1color;
ADD_SAT total, total, bump;

# add ambient lighting
ADD_SAT total, total, ambient;

# multiply by regular texture map color
MUL_SAT result.color, rgb, total;

END
