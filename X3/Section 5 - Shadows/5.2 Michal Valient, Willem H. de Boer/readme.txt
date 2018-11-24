"Fractional-Disk Soft Shadows"

Michal Valient 
Dep. of Computer Graphics and Image Processing
Comenius University in Bratislava

Willem H. de Boer
Graphics Research Group
Playlogic Game Factory BV

-------------------------------

Sample code by Michal Valient

e-mail: valient@dimension3.sk
project pages: http://www.dimension3.sk

-------------------------------
Intro

This package contains examples for ShaderX3 chapter mentioned above.
Examples were tested on ATI Radeon 9700 and 9800 and DX9 reference rasterizer.
To compile the examples, expat libraries are required - http://www.libexpat.org/
To compile the examples, DirectX 9 SDK (Summer update 2004) is required - http://msdn.microsoft.com/directx
To compile the 3DS MAX .V2 exporter, Flexporter package is needed - http://www.codercorner.com/Flexporter.html

-------------------------------
Licence

This software is released under ZLIB like licence. See "Licence" file.

-------------------------------
Command line

Usage: app.exe -i <filename> -m <material> [-dp] [-nopoisson] [-1d] [-o <filenam
e>] [-s <speed>]
Example: app.exe -m spotlight -s 3 -o spot

Parameters:
-i <filename> plays specified XML scene file
   ! do not specify any path, just filename and extension !

-m <material> sets specified material
   Materials are:
     shadow         The plain (non filtered shadow mapping - PS.2.0
     shadow2       *Shadow mapping with stochastic sampling - PS.2.0 (multipass)

     shadow3       *Full shadow mapping with stochastic sampling - PS.3.0
     shadow3a      *Full shadow mapping with stochastic sampling - PS.2.a
     shadow3b      *Full shadow mapping with stochastic sampling - PS.2.b
       ! (*) Techniques require support for rendering into A16R16G16B16 map
       ! to render as intended.
       ! Please note that shadow3a and shadow3b are identical to shadow3
       ! but are recompiled for appropriate shader model
-dp disable the shadow map preprocessing and use constant disk radius.
-nopoisson use plain random values in noise texture - not Poisson disk.
-1d use 1D version of noise texture.

-o <filename> saves each screen into file '<filename>_nnnn.bmp'
   nnnn is frame number
   ! do not specify any path, just filename without extension!
-s <speed> alters playing speed - any positive number
   Speed modifier:
     1              original speed
     2              half speed
     n              1/n of original speed (n times slower)

-------------------------------
Package hierarchy:

./bin
	- precompiled examples
               app.exe is release build
               app_debug.exe is debug build and has extended output to log file.
	- Dimension3Exporter.flx - Dimension3 3DS MAX4 exporter

./bin/Resources/Shaders
	- source DX Effect files for the shadow techniques

./bin/Resources/Required
        - Required textures

./bin/Resources/Scenes
        - Rendering scenes

./Include and ./Source
        - source code of the renderer.

./Source/Utils/MaxExporter
        - MAX exporter source

./Source/Utils/MaxExporter/Flexporter
        - Here comes flexporter SDK

./Source/Utils/Tester02
        - Your place to start - an application executable project ;-)

