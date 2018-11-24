# Microsoft Developer Studio Project File - Name="shaderey" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=shaderey - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shaderey.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shaderey.mak" CFG="shaderey - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shaderey - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "shaderey - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shaderey - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/release"
# PROP Intermediate_Dir "build/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../dingus" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "NDEBUG"
# ADD RSC /l 0x427 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx9.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ole32.lib oleaut32.lib d3d9.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"shaderey.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "shaderey - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/debug"
# PROP Intermediate_Dir "build/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../dingus" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "_DEBUG"
# ADD RSC /l 0x427 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9dt.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ole32.lib oleaut32.lib d3d9.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"shaderey_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "shaderey - Win32 Release"
# Name "shaderey - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;h;c"
# Begin Group "anim"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\anim\CameraAnim.cpp
# End Source File
# Begin Source File

SOURCE=.\src\anim\CameraAnim.h
# End Source File
# Begin Source File

SOURCE=.\src\anim\SampledAnim.h
# End Source File
# End Group
# Begin Group "demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\demo\BaseEntity.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\CloudMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\CloudMesh.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\ColorOps.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Demo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Demo.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Projector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Projector.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\QuadMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\QuadMesh.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Resources.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Scattering.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Scattering.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\SimpleMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\SimpleMesh.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\SkyMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\SkyMesh.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Terrain.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\WaterMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\WaterMesh.h
# End Source File
# End Group
# Begin Group "system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\system\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\Globals.h
# End Source File
# Begin Source File

SOURCE=.\src\system\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\System.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\System.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\Demo.rc
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\stdafx.h
# End Source File
# End Group
# Begin Group "fx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\data\fx\_global.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\_library.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\_shared.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\cloud.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\cloudColorOnly.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\fallbackNormalZ.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\imgBleed.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\imgComposite.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\imgEdgeDetect.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\imgRemap3D.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\mesh.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\meshColorOnly.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\meshShadow.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\sky.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\terrain.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\terrainColorOnly.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\terrainNormalZOnly.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\water.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\waterColorOnly.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\waterNormalZOnly.fx
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\DirectX.ico
# End Source File
# Begin Source File

SOURCE=.\src\logo.bmp
# End Source File
# End Target
# End Project
