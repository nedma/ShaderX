This application requires at least:

	ARB_vertex_program

It will take advantage, if it finds support for:

	ARB_vertex_shader
	ARB_fragment_shader
	ARB_shader_objects
	ARB_shader_language100

During runtime, use the following keys:

Arrow keys	- camera movement (front, back, strafe)
H		- turn on/off Spherical Harmonics lighting
T		- turn on/off texturing
+/-		- (from the numerical keyboard) decrease/increase the geometry complexity
F		- turn on/off wireframe mode
space		- turn on/off rotation around center
C		- turn on/off colored balls
Ins/Del		- add/remove light
PgUp/PgDn	- add/remove Phong model light (only works when SH lighting is on)
F12		- takes a screenshot

Note: Environment maps lighting only works in SH mode (ie Phong lighting is performed using analytical lights while to see the diffuse component comming from the cube map use SH mode)

For questions and problems please contact:

vladstamate@hotmail.com