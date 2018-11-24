
Direct3D Color Grader library 1.1 for ShaderX³

©2004 Ronny Burkersroda <RBurkersroda@lightrock.biz>


    This software is property of the Ronny Burkersroda and may not be copied,
redistributed or published in any way without permission!
Abuse will immediately result in legal steps without preliminary warnings.

    You are allowed to use this software in your own products, if they are only
published as compiled programs. Please inform me about the product and add me
to the credits!


About:

    This is a library with source code and a project file for Microsoft Visual
C++ 2003. It can be used to apply color grading in realtime to programs and
games, which are using Direct3D 9.

    The source code and compiled library are part of the book ShaderX³
(www.shaderx3.com), which is published by Charles River Media
(www.charlesriver.com), and may not be distributes without the book or the
permission of the author.


Programming tips:

    1. The class uses a dynamic texture, which has to be created in default
pool. So you have to release all objects before you are able to reset the
Direct3D device. Alternatively you can also call
ID3DColorGrader9::ResetGradient(), but you are forced to reconfigure and reload
the gradient after the reset, too.
    2. ID3DColorGrader9::SaveGradientTo3DFile() stores the volume texture,
which includes the gradient, to a DDS file. Such files cannot be used in a lot
of image processing applications. So you can store the gradient as 2D image to
a BMP file using ID3DColorGrader9::SaveGradientTo2DFile(). This allows you load
and save the image with nearly all applications. If you apply a color change to
a reference image (e.g. a screenshot from your game), you can also apply that
change to the gradient image. After reloading it into the game or program, the
color grader produces the same or a similar result like the image processing
application.
