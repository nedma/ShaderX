!!ARBfp1.0

PARAM light0color = state.light[0].diffuse;
PARAM light1color = state.light[1].diffuse;
PARAM ambient = state.lightmodel.ambient;
PARAM scale_bias = program.local[0];

TEMP eyevects;
TEMP rgb, height, temp, total;
TEMP light0tsvec, light1tsvec;
TEMP newtexcoord;

# normalize tangent space eye vector
DP3 temp, fragment.texcoord[3], fragment.texcoord[3];
RSQ temp, temp.x;
MUL eyevects, fragment.texcoord[3], temp;

# calculate offset and new texture coordinate
TEX height, fragment.texcoord[0], texture[2], 2D;
MAD height, height, scale_bias.x, scale_bias.y;  # scale and bias
MAD newtexcoord, height, eyevects, fragment.texcoord[0];

# get texture data
TEX rgb, newtexcoord, texture[0], 2D;

# normalize the light0 vector
DP3 temp, fragment.texcoord[1], fragment.texcoord[1];
RSQ temp, temp.x;
MUL light0tsvec, fragment.texcoord[1], temp;

# add light0 color
MUL_SAT total, light0tsvec.z, light0color;

# normalize the light1 vector
DP3 temp, fragment.texcoord[2], fragment.texcoord[2];
RSQ temp, temp.x;
MUL light1tsvec, fragment.texcoord[2], temp;

# add light1 color
MUL_SAT temp, light1tsvec.z, light1color;
ADD_SAT total, total, temp;

# add ambient lighting
ADD_SAT total, total, ambient;

# multiply by regular texture map color
MUL_SAT result.color, rgb, total;

END
