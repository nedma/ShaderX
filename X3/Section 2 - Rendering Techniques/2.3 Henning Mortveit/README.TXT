
DISCLAIMER: See the CD/BOOK.


		   Reflections from bumpy surfaces
	      ------------------------------------------

The demo (w322.exe) can be started directly or through the launch
application launch.exe. We recommend using launch.exe which allows you
to set e.g. display resolution in a straightforward way.

w322.exe recognizes the following case-sensitive command line options:

-infinity: loads shaders for the infinity approximation. 'g' will now
  toggle between the regular glsl bumpy reflection shader and an
  arb-vp/fp infinity shader pair.

-exr: Only available if your card supports GL_ATI_texture_float.
  In this case '8' cycles between and 16 bit fixed normal map, a 32
  bit float texture normal map and and 8 bit fixed normal map.

-fullScreen: This one should be pretty obvious.

All these options can be set in the launch application.


Note 1: In windowed mode the title of the application window displays
information about the current settings. You may want to start in
windowed mode to familiarize yourself with the options and then check
the fullscreen later.

Note 2: The average/approximate distance h to the environment is
depicted using a small square behind and to the left of the running
red character. The distance h is clampled to [1.0, 250.0].


Additional keys: (Use entirely at your own risk)

- 'b': dumps a 2x2 matrix of screen shots (png format) 

- 's': dumps a single screen shot (png format)

- 't'/'T': rotates camera around forward vector
  (counterclockwise/clockwise). 



Troubleshooting:

Note that this demo uses recent OpenGL extensions mainly found on
high-end GPUs (high-end at the time of writing anyway!). If you have
drivers that are not up-to-date you probably will not be able to run
the demo. The extensions used should be available on ATi 9500+ and
NVidia 5700+ video cards, although it has only been tested on ATi
hardware.

The demo and the launch application will write out log files
'w322.log' and 'launch.log' respectively. They contain system
information and some diagnostics in case of errors.



Libraries and frameworks used:

- CygWin. 
- JPEGLIB of the Independent JPEG Group (http://www.ijg.org)
- LIBPNG (http://www.libpng.org/pub/png/)
- ZLIB (http://www.gzip.org/zlib/)
- OpenEXR image library (http://www.openexr.com/)

- The skybox was generated using Terragen
  (http://www.planetside.co.uk/terragen)

Henning S. Mortveit
sfruckus at netscape.net
