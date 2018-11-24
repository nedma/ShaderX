# Microsoft Developer Studio Project File - Name="ExampleOne" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ExampleOne - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ExampleOne.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ExampleOne.mak" CFG="ExampleOne - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ExampleOne - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ExampleOne - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ExampleOne - Win32 Release"

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
# ADD CPP /nologo /W4 /GX /O2 /I "..\base" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 dsound.lib dinput8.lib dxerr9.lib d3dx9.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib dinput8.lib dxerr9.lib d3dx9.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"./ExampleOne.exe"

!ELSEIF  "$(CFG)" == "ExampleOne - Win32 Debug"

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
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\base" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 dsound.lib dinput8.lib dxerr9.lib d3dx9.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dsound.lib dinput8.lib dxerr9.lib d3dx9.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"./ExampleOne.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ExampleOne - Win32 Release"
# Name "ExampleOne - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\base\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=..\base\d3denumeration.cpp
# End Source File
# Begin Source File

SOURCE=..\base\d3dfile.cpp
# End Source File
# Begin Source File

SOURCE=..\base\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=..\base\d3dsettings.cpp
# End Source File
# Begin Source File

SOURCE=..\base\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=..\base\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\base\ExampleBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ExampleOne.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\base\d3dapp.h
# End Source File
# Begin Source File

SOURCE=..\base\d3denumeration.h
# End Source File
# Begin Source File

SOURCE=..\base\d3dfile.h
# End Source File
# Begin Source File

SOURCE=..\base\d3dfont.h
# End Source File
# Begin Source File

SOURCE=..\base\d3dsettings.h
# End Source File
# Begin Source File

SOURCE=..\base\d3dutil.h
# End Source File
# Begin Source File

SOURCE=..\base\dxutil.h
# End Source File
# Begin Source File

SOURCE=..\base\ExampleBase.h
# End Source File
# Begin Source File

SOURCE=.\ExampleOne.h
# End Source File
# Begin Source File

SOURCE=..\base\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\base\ExampleBase.rc
# End Source File
# End Group
# Begin Group "Shader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Charcoal.psh
# End Source File
# Begin Source File

SOURCE=.\Charcoal.vsh
# End Source File
# End Group
# End Target
# End Project
