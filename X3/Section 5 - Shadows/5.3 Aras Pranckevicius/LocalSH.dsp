# Microsoft Developer Studio Project File - Name="LocalSH" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LocalSH - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LocalSH.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LocalSH.mak" CFG="LocalSH - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LocalSH - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LocalSH - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LocalSH - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "NDEBUG"
# ADD RSC /l 0x427 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib d3d9.lib d3dx9.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"LocalSH.exe"

!ELSEIF  "$(CFG)" == "LocalSH - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "_DEBUG"
# ADD RSC /l 0x427 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib d3d9.lib d3dx9dt.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"LocalSH_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LocalSH - Win32 Release"
# Name "LocalSH - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;h;c"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\common\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\src\common\d3denumeration.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\d3denumeration.h
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dsettings.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dsettings.h
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\src\common\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\dxutil.h
# End Source File
# Begin Source File

SOURCE=.\src\common\winmain.rc
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\math\Constants.h
# End Source File
# Begin Source File

SOURCE=.\src\math\Matrix4x4.h
# End Source File
# Begin Source File

SOURCE=.\src\math\Vector2.h
# End Source File
# Begin Source File

SOURCE=.\src\math\Vector3.h
# End Source File
# Begin Source File

SOURCE=.\src\math\Vector4.h
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\utils\fastvector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\Effect.h
# End Source File
# Begin Source File

SOURCE=.\src\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Mesh.h
# End Source File
# Begin Source File

SOURCE=.\src\SHProjection.h
# End Source File
# Begin Source File

SOURCE=.\src\SphericalSamples.h
# End Source File
# Begin Source File

SOURCE=.\src\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\src\VisibilityCalc.h
# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\common\directx.ico
# End Source File
# End Group
# Begin Group "fx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\data\bulb.fx
# End Source File
# Begin Source File

SOURCE=.\data\func.fx
# End Source File
# Begin Source File

SOURCE=.\data\library.fx
# End Source File
# Begin Source File

SOURCE=.\data\localSH.fx
# End Source File
# Begin Source File

SOURCE=.\data\shadowdepth.fx
# End Source File
# Begin Source File

SOURCE=.\data\shadowmap.fx
# End Source File
# End Group
# End Target
# End Project
