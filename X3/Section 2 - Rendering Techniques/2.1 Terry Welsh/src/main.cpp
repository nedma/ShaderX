#include <stdlib.h>
//#include <iostream.h>
#include <math.h>
#include "extgl.h"
#include <GL/glut.h>
#include "ltga.h"

#include <sys/stat.h>
#include <sys/types.h>




//int w, h;  // width and height of window
int oldwinx, oldwiny;
int mb[3] = {0, 0, 0};  // mouse buttons

// each material has 3 texture maps:  rgb, normal map, height map
#define NUM_TEXTURES 7
uint texture[NUM_TEXTURES][3];
int current_texture = 0;
// per-texture data for fragment programs
float scale_bias[NUM_TEXTURES][4];

int light_move = 1;

// vertex and fragment programs
uint vpid[3];
uint fpid[3];





char* readProgram(const char* name){
	FILE *in = fopen(name, "r");
   
	if (!in)
		return 0;
 
	char *buffer = 0;
   
	// get file size
	long size = 0, curpos;
	curpos = ftell(in);
	fseek(in, 0L, SEEK_END);
	size = ftell(in);
	fseek(in, curpos, SEEK_SET);
   
	if (!size)
		return 0;   
	if (!(buffer = new char[size + 1]))
		return 0;

	memset(buffer, '\0', size + 1);
	if (!buffer)
		return 0;
	fread(buffer, 1, size, in); // check return with size?
	fclose(in);

	return buffer;
}


void setParameters(){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void initOpengl(){
	// extensions
	extgl_Initialize();
	if(!extgl_ExtensionSupported("GL_ARB_multitexture")){
		cout << "GL_ARB_multitexture not supported.  Exiting..." << endl;
		exit(0);
	}
	if(!extgl_ExtensionSupported("GL_ARB_vertex_program")){
		cout << "GL_ARB_vertex_program not supported.  Exiting..." << endl;
		exit(0);
	}
	if(!extgl_ExtensionSupported("GL_ARB_fragment_program")){
		cout << "GL_ARB_fragment_program not supported.  Exiting..." << endl;
		exit(0);
	}

	// miscellaneous
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	float diffuse0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	float diffuse1[] = {0.5f, 0.5f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	
	// textures
	for(int i=0; i<NUM_TEXTURES; i++)
		glGenTextures(3, texture[i]);
	LTGA tga;

	tga.LoadFromFile("brick.tga");
	glBindTexture(GL_TEXTURE_2D, texture[0][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("brick_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[0][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("brick_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[0][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());

	tga.LoadFromFile("bridgewall.tga");
	glBindTexture(GL_TEXTURE_2D, texture[1][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("bridgewall_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[1][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("bridgewall_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[1][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());

	tga.LoadFromFile("rockwall.tga");
	glBindTexture(GL_TEXTURE_2D, texture[2][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("rockwall_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[2][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("rockwall_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[2][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());

	tga.LoadFromFile("sand.tga");
	glBindTexture(GL_TEXTURE_2D, texture[3][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("sand_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[3][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("sand_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[3][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());
	
	tga.LoadFromFile("metal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[4][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("metal_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[4][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("metal_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[4][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());

	tga.LoadFromFile("roots.tga");
	glBindTexture(GL_TEXTURE_2D, texture[5][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("roots_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[5][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("roots_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[5][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());
	
	tga.LoadFromFile("bark.tga");
	glBindTexture(GL_TEXTURE_2D, texture[6][0]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("bark_normal.tga");
	glBindTexture(GL_TEXTURE_2D, texture[6][1]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_RGB, GL_UNSIGNED_BYTE, tga.GetPixels());
	tga.LoadFromFile("bark_height.tga");
	glBindTexture(GL_TEXTURE_2D, texture[6][2]);
	setParameters();
   gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tga.GetImageWidth(), tga.GetImageHeight(),
       GL_LUMINANCE, GL_UNSIGNED_BYTE, tga.GetPixels());

	// load vertex and fragment programs
	char* program;
	glGenProgramsARB(3, vpid);
	glGenProgramsARB(3, fpid);
	program = readProgram("bump.vp");
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid[0]);
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);
	program = readProgram("bump.fp");
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid[0]);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);
	program = readProgram("parallax_bump.vp");
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid[1]);
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);
	program = readProgram("parallax.fp");
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid[1]);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);
	program = readProgram("parallax_bump.vp");
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid[2]);
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);
	program = readProgram("parallax_bump.fp");
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid[2]);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);

	// per-texture data for fragment programs
	scale_bias[0][0] = 0.03f;  scale_bias[0][1] = -0.015f;  // brick
	scale_bias[1][0] = 0.06f;  scale_bias[1][1] = -0.03f;  // bridgewall
	scale_bias[2][0] = 0.04f;  scale_bias[2][1] = -0.02f;  // rockwall
	scale_bias[3][0] = 0.05f;  scale_bias[3][1] = -0.025f;  // sand
	scale_bias[4][0] = 0.05f;  scale_bias[4][1] = -0.025f;  // metal
	scale_bias[5][0] = 0.04f;  scale_bias[5][1] = -0.02f;  // roots
	scale_bias[6][0] = 0.04f;  scale_bias[6][1] = -0.02f;  // bark
}


void reshape(int width, int height){
	//w = width;
	//h = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, float(width)/float(height), 0.01f, 1000.0f);
}


void mouse(int button, int state, int winx, int winy){
	static float xyz[3] = {0.0f, 0.0f, -10.0f};
	static float hpr[3] = {0.0f, 0.0f, 0.0f};

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		mb[0] = 1;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mb[0] = 0;
	if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		mb[1] = 1;
	if(button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
		mb[1] = 0;
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		mb[2] = 1;
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		mb[2] = 0;

	oldwinx = winx;
	oldwiny = winy;
}


int invertMatrix(float* mat, float* im){
	float rmat[4][8];
	float a, b;
	int i, j, k;

	// initialize reduction matrix
	rmat[0][0] = mat[0];
	rmat[1][0] = mat[1];
	rmat[2][0] = mat[2];
	rmat[3][0] = mat[3];
	rmat[0][1] = mat[4];
	rmat[1][1] = mat[5];
	rmat[2][1] = mat[6];
	rmat[3][1] = mat[7];
	rmat[0][2] = mat[8];
	rmat[1][2] = mat[9];
	rmat[2][2] = mat[10];
	rmat[3][2] = mat[11];
	rmat[0][3] = mat[12];
	rmat[1][3] = mat[13];
	rmat[2][3] = mat[14];
	rmat[3][3] = mat[15];
	rmat[0][4] = 1.0f;
	rmat[1][4] = 0.0f;
	rmat[2][4] = 0.0f;
	rmat[3][4] = 0.0f;
	rmat[0][5] = 0.0f;
	rmat[1][5] = 1.0f;
	rmat[2][5] = 0.0f;
	rmat[3][5] = 0.0f;
	rmat[0][6] = 0.0f;
	rmat[1][6] = 0.0f;
	rmat[2][6] = 1.0f;
	rmat[3][6] = 0.0f;
	rmat[0][7] = 0.0f;
	rmat[1][7] = 0.0f;
	rmat[2][7] = 0.0f;
	rmat[3][7] = 1.0f;

	// perform reductions
	for(i=0; i<4; i++){
		a = rmat[i][i];
		if(a == 0.0f)  // matrix is singular, can't be reduced
			return 0;
		else{
			a = 1.0f / a;
			for(j=0; j<8; j++)
				rmat[i][j] = rmat[i][j] * a;
			for(k=0; k<4; k++){
				if((k-i) != 0){
					b = rmat[k][i];
					for(j=0; j<8; j++)
						rmat[k][j] = rmat[k][j] - b * rmat[i][j];
				}
			}
		}
	}

	// extract the inverted matrix
	im[0] = rmat[0][4];
	im[1] = rmat[1][4];
	im[2] = rmat[2][4];
	im[3] = rmat[3][4];
	im[4] = rmat[0][5];
	im[5] = rmat[1][5];
	im[6] = rmat[2][5];
	im[7] = rmat[3][5];
	im[8] = rmat[0][6];
	im[9] = rmat[1][6];
	im[10] = rmat[2][6];
	im[11] = rmat[3][6];
	im[12] = rmat[0][7];
	im[13] = rmat[1][7];
	im[14] = rmat[2][7];
	im[15] = rmat[3][7];

	return 1;
}


void motion(int winx, int winy){
	static float xyz[3] = {0.0f, 0.0f, -10.0f};
	static float hpr[3] = {0.0f, 0.0f, 0.0f};
	float rotmat[16];
	float rotmatinv[16];

	if(mb[0]){  // rotate
		hpr[0] += float(winx - oldwinx) * 0.2f;
		hpr[1] += float(winy - oldwiny) * 0.2f;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(hpr[2], 0.0f, 0.0f, 1.0f);
	glRotatef(hpr[1], 1.0f, 0.0f, 0.0f);
	glRotatef(hpr[0], 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotmat);
	invertMatrix(rotmat, rotmatinv);

	if(mb[1]){  // pan
		xyz[0] += (float(winx - oldwinx) * rotmatinv[0]
			- float(winy - oldwiny) * rotmatinv[4]) * 0.01f;
		xyz[1] += (float(winx - oldwinx) * rotmatinv[1]
			- float(winy - oldwiny) * rotmatinv[5]) * 0.01f;
		xyz[2] += (float(winx - oldwinx) * rotmatinv[2]
			- float(winy - oldwiny) * rotmatinv[6]) * 0.01f;
	}
	if(mb[2]){  // dolly
		xyz[0] += float(winy - oldwiny) * rotmatinv[8] * 0.01f;
		xyz[1] += float(winy - oldwiny) * rotmatinv[9] * 0.01f;
		xyz[2] += float(winy - oldwiny) * rotmatinv[10] * 0.01f;
	}

	glTranslatef(xyz[0], xyz[1], xyz[2]);

	oldwinx = winx;
	oldwiny = winy;
}


void keyboard(unsigned char key, int winx, int winy){
	switch(key){
	case 'l':
	case 'L':
		// start and stop light movements
		light_move ++;
		light_move %= 2;
		break;
	case 't':
	case 'T':
		// change texture
		current_texture ++;
		current_texture %= NUM_TEXTURES;
		break;
	}
}


void drawGeometry(){
	int i;
	float n;

	glBegin(GL_TRIANGLE_STRIP);
		glMultiTexCoord3fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f, 0.0f);  // tangent
		glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 0.0f, 1.0f, 0.0f);  // binormal
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-3.0f, -0.5f, 0.0f);
		glTexCoord2f(3.0f, 0.0f);
		glVertex3f(0.0f, -0.5f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-3.0f, 0.5f, 0.0f);
		glTexCoord2f(3.0f, 1.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);
	glEnd();

	glPushMatrix();
		glTranslatef(2.0f, 0.0f, 0.0f);
		glBegin(GL_TRIANGLE_STRIP);
			glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 0.0f, 1.0f, 0.0f);  // binormal
			for(i=0; i<=50; i++){
				n = (float(i) / 50.0f) * 6.283185307f;
				glMultiTexCoord3fARB(GL_TEXTURE1_ARB, -sinf(n), 0.0f, cosf(n));  // tangent
				glNormal3f(cosf(n), 0.0f, sinf(n));
				glTexCoord2f(6.0f * float(i) / 50.0f, 0.0f);
				glVertex3f(cosf(n), -0.5f, sinf(n));
				glTexCoord2f(6.0f * float(i) / 50.0f, 1.0f);
				glVertex3f(cosf(n), 0.5f, sinf(n));
			}
		glEnd();
	glPopMatrix();
}


void drawText(char* text){
	int i = 0;
	char character = text[i++];
	while(character != '\0'){
		glutStrokeCharacter(GLUT_STROKE_ROMAN, character);
		character = text[i++];
	}
}


void display(){
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// lighting
	static float angle0 = 0.0f;
	static float angle1 = 0.0f;
	if(light_move){
		angle0 += 0.01111f;
		angle1 += 0.013f;
	}
	float position0[] = {2.0f * cosf(angle0), 2.0f * sinf(angle0), 1.5f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	float position1[] = {0.0f, 3.0f * cosf(angle1), 1.5f * sinf(angle1), 1.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glPointSize(3.0f);
	glBegin(GL_POINTS);
		glVertex3f(2.0f * cosf(angle0), 2.0f * sinf(angle0), 1.5f);
		glVertex3f(0.0f, 3.0f * cosf(angle1), 1.5f * sinf(angle1));
	glEnd();
	glEnable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, texture[current_texture][0]);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, texture[current_texture][1]);
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glBindTexture(GL_TEXTURE_2D, texture[current_texture][2]);

	glPushMatrix();
		// plain texturing
		glTranslatef(0.0f, 2.25f, 0.0f);
		drawGeometry();
		// bump mapping
		glEnable(GL_VERTEX_PROGRAM_ARB);
		glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid[0]);
		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid[0]);
		glTranslatef(0.0f, -1.5f, 0.0f);
		drawGeometry();
		// parallax mapping
		glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid[1]);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid[1]);
		glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, scale_bias[current_texture]);
		glTranslatef(0.0f, -1.5f, 0.0f);
		drawGeometry();
		// bump and parallax mapping
		glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid[2]);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid[2]);
		glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, scale_bias[current_texture]);
		glTranslatef(0.0f, -1.5f, 0.0f);
		drawGeometry();
	glPopMatrix();

	glActiveTextureARB(GL_TEXTURE0_ARB);  // why do I need this line?
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_LIGHTING);

	glPushMatrix();
		// text
		glPushMatrix();
			glTranslatef(-2.0f, 1.55f, 0.0f);
			glScalef(0.0015f, 0.0015f, 0.0015f);
			drawText("regular texture mapping");
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.0f, 0.05f, 0.0f);
			glScalef(0.0015f, 0.0015f, 0.0015f);
			drawText("bump mapping");
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.0f, -1.45f, 0.0f);
			glScalef(0.0015f, 0.0015f, 0.0015f);
			drawText("parallax mapping");
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.0f, -2.95f, 0.0f);
			glScalef(0.0015f, 0.0015f, 0.0015f);
			drawText("parallax mapping and bump mapping");
		glPopMatrix();
	glPopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}


int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(512, 400);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int windowID = glutCreateWindow("parallax mapping demo");

	glutSetWindow(windowID);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);

	initOpengl();
	motion(0, 0);  // call this once to create initial modelview matrix

	glutMainLoop();

	return 0;
}
