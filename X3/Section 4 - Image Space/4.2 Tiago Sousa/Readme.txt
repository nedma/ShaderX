//-----------------------------------------------------------------------------
// Name: Adaptive Glare Sample
//-----------------------------------------------------------------------------

Description
===========
	The sample AdaptiveGlare, is a simple demonstration of the article "Adaptive 
	Glare", presented in ShaderX 3.
	
	Basically it first renders the scene into a texture. This texture is then 
	resized into a very small resolution texture and mipmaps are generated for it.
	Then glare amount is computed, followed by a bright pass to extract the 
	brightest pixels, and at the end, the final image composition is done, as 
	explained in article.
	
	Demo created in Visual Studio 2003, compiled using Dx9.b sdk and NVidia CG compiler 1.2

User's Guide
============
   The following keys are implemented. 
   
      <Esq>        Quit application
      <W, S, A, D> Fly foward, back, strafe left, strafe right
      <F1, F2>     Enables glare, disables glare
      <F5>         Reloads shaders
      <Mouse>      Camera orientation


Credits
=======

Tiago Sousa
email: tiago@crytek.de (tiagosousa@yahoo.com)
http: http://realtimecg.gamedev-pt.net