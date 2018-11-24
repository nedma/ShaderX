# Microsoft Developer Studio Project File - Name="daysky" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=daysky - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "daysky.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "daysky.mak" CFG="daysky - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "daysky - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "daysky - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "daysky - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "_tmp/release"
# PROP Intermediate_Dir "_tmp/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /GB /MD /W3 /GX /O2 /Ob2 /I "wxwidgets" /I "cg" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WXMSW__" /D WINVER=0x0400 /D "WXUSINGDLL" /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wxmsw24.lib opengl32.lib glu32.lib comctl32.lib cg.lib cgGL.lib /nologo /subsystem:windows /machine:I386 /out:"_bin/daysky.exe" /libpath:"wxwidgets" /libpath:"cg"

!ELSEIF  "$(CFG)" == "daysky - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "_tmp/debug"
# PROP Intermediate_Dir "_tmp/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "wxwidgets" /I "cg" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WXMSW__" /D WINVER=0x0400 /D "WXUSINGDLL" /D "STRICT" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wxmsw24.lib opengl32.lib glu32.lib comctl32.lib cg.lib cgGL.lib /nologo /subsystem:windows /debug /machine:I386 /out:"_bin/dayskyd.exe" /pdbtype:sept /libpath:"wxwidgets" /libpath:"cg"

!ENDIF 

# Begin Target

# Name "daysky - Win32 Release"
# Name "daysky - Win32 Debug"
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\math\Basics.cpp
# End Source File
# Begin Source File

SOURCE=.\math\Basics.h
# End Source File
# Begin Source File

SOURCE=.\math\CAxisAngle.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CAxisAngle.h
# End Source File
# Begin Source File

SOURCE=.\math\CEulerAngles.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CEulerAngles.h
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix3.h
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix4.h
# End Source File
# Begin Source File

SOURCE=.\math\CQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CQuaternion.h
# End Source File
# Begin Source File

SOURCE=.\math\CSphericalRotation.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CSphericalRotation.h
# End Source File
# Begin Source File

SOURCE=.\math\CVector3.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CVector3.h
# End Source File
# Begin Source File

SOURCE=.\math\CVector4.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CVector4.h
# End Source File
# End Group
# Begin Group "logging"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\logging\ConsoleHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\ConsoleHandler.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\FileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\FileHandler.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\HandlerBase.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\HandlerBase.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\IFilter.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\IFormatter.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\IHandler.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\Level.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\Level.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\Logger.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\Logger.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\LogRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\LogRecord.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\SimpleFormatter.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\SimpleFormatter.hpp
# End Source File
# Begin Source File

SOURCE=.\logging\wxTextCtrlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\logging\wxTextCtrlHandler.hpp
# End Source File
# End Group
# Begin Group "scene"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scene\HeightField.cpp
# End Source File
# Begin Source File

SOURCE=.\scene\HeightField.hpp
# End Source File
# Begin Source File

SOURCE=.\scene\SkyDome.cpp
# End Source File
# Begin Source File

SOURCE=.\scene\SkyDome.hpp
# End Source File
# Begin Source File

SOURCE=.\scene\StaticTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\scene\StaticTerrain.hpp
# End Source File
# End Group
# Begin Group "rendering"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\render\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\render\Camera.hpp
# End Source File
# Begin Source File

SOURCE=.\render\CompassRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\render\CompassRenderer.hpp
# End Source File
# Begin Source File

SOURCE=.\glfont\GLFont.cpp
# End Source File
# Begin Source File

SOURCE=.\glfont\GLFont.hpp
# End Source File
# Begin Source File

SOURCE=.\render\IRenderer.hpp
# End Source File
# Begin Source File

SOURCE=.\render\SharedCgContext.cpp
# End Source File
# Begin Source File

SOURCE=.\render\SharedCgContext.hpp
# End Source File
# Begin Source File

SOURCE=.\render\SkyRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\render\SkyRenderer.hpp
# End Source File
# Begin Source File

SOURCE=.\render\TerrainRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\render\TerrainRenderer.hpp
# End Source File
# Begin Source File

SOURCE=.\render\TextRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\render\TextRenderer.hpp
# End Source File
# End Group
# Begin Group "scattering"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scatter\HoffmanScatter.cpp
# End Source File
# Begin Source File

SOURCE=.\scatter\HoffmanScatter.hpp
# End Source File
# Begin Source File

SOURCE=.\scatter\IScatter.hpp
# End Source File
# Begin Source File

SOURCE=.\scatter\PreethamScatter.cpp
# End Source File
# Begin Source File

SOURCE=.\scatter\PreethamScatter.hpp
# End Source File
# End Group
# Begin Group "app"

# PROP Default_Filter ""
# Begin Group "dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\app\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\app\AboutDlg.hpp
# End Source File
# Begin Source File

SOURCE=.\app\HoffmanDlg.cpp

!IF  "$(CFG)" == "daysky - Win32 Release"

!ELSEIF  "$(CFG)" == "daysky - Win32 Debug"

# ADD CPP /D "NDEBUG"
# SUBTRACT CPP /D "_DEBUG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\app\HoffmanDlg.hpp
# End Source File
# Begin Source File

SOURCE=.\app\LogView.cpp
# End Source File
# Begin Source File

SOURCE=.\app\LogView.hpp
# End Source File
# Begin Source File

SOURCE=.\app\PreethamDlg.cpp

!IF  "$(CFG)" == "daysky - Win32 Release"

!ELSEIF  "$(CFG)" == "daysky - Win32 Debug"

# ADD CPP /D "NDEBUG"
# SUBTRACT CPP /D "_DEBUG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\app\PreethamDlg.hpp
# End Source File
# Begin Source File

SOURCE=.\app\RenderingDlg.cpp

!IF  "$(CFG)" == "daysky - Win32 Release"

!ELSEIF  "$(CFG)" == "daysky - Win32 Debug"

# ADD CPP /D "NDEBUG"
# SUBTRACT CPP /D "_DEBUG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\app\RenderingDlg.hpp
# End Source File
# Begin Source File

SOURCE=.\app\SunDlg.cpp

!IF  "$(CFG)" == "daysky - Win32 Release"

!ELSEIF  "$(CFG)" == "daysky - Win32 Debug"

# ADD CPP /D "NDEBUG"
# SUBTRACT CPP /D "_DEBUG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\app\SunDlg.hpp
# End Source File
# End Group
# Begin Group "custom widgets"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\app\RenderView.cpp
# End Source File
# Begin Source File

SOURCE=.\app\RenderView.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\app\AppFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\app\AppFrame.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\daysky.cpp

!IF  "$(CFG)" == "daysky - Win32 Release"

!ELSEIF  "$(CFG)" == "daysky - Win32 Debug"

# ADD CPP /D "NDEBUG"
# SUBTRACT CPP /D "_DEBUG"

!ENDIF 

# End Source File
# End Target
# End Project
