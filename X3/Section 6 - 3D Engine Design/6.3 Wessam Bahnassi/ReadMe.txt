This sample application illustrates how to use the SXEffect class to simplify access to D3DX Effect variables.
The sample renders a happy bouncy face. The scene is rendered using a vertex shader that "squishes" the face when it splats on the ground (along with a nice splatter sound ;).
The shader is written in an effect file that declares many variables which are accessed in the application, these include array variables.

The actual system implementation is in SXEffect.h, SXEffect.inl and SXEffect.cpp. All other files are sample files. Scene files are saved in the 'scene' directory, and it contains raw binary vertex and index data for the meshes used in the sample...

Have fun!