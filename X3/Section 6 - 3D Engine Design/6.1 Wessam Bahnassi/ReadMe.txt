This sample application illustrates how to use the SXProbe and SXResource classes to manage any kind of Direct3D resource in an easy way.
The executable here just renders a teapot, but the teapot is just one kind of resource used in the application (see SXSampleResources.h/.cpp for more resource types). The application logs SXProbe messages to any attached debug output window. Use dbmon.exe to see these messages as they show you what the management system is doing.
The actual system implementation is in SXDevice.h and SXDevice.cpp, all other files are unneccessary.

Have fun!