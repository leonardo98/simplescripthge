# Microsoft Developer Studio Project File - Name="SimpleScriptHGE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SimpleScriptHGE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SimpleScriptHGE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SimpleScriptHGE.mak" CFG="SimpleScriptHGE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SimpleScriptHGE - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SimpleScriptHGE - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SimpleScriptHGE - Win32 Release"

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
# ADD CPP /nologo /w /W0 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "TIXML_USE_STL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib hge.lib hgehelp.lib /nologo /subsystem:windows /machine:I386 /out:"SimpleScriptHGE.exe"

!ELSEIF  "$(CFG)" == "SimpleScriptHGE - Win32 Debug"

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
# ADD CPP /nologo /w /W0 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "TIXML_USE_STL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib hge.lib hgehelp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"SimpleScriptHGE.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SimpleScriptHGE - Win32 Release"
# Name "SimpleScriptHGE - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Core.cpp
# End Source File
# Begin Source File

SOURCE=.\Counter.cpp
# End Source File
# Begin Source File

SOURCE=.\cursors.cpp
# End Source File
# Begin Source File

SOURCE=.\InputSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Lexem.cpp
# End Source File
# Begin Source File

SOURCE=.\LexemsReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Messager.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\OperatorsReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Tester.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Variable.cpp
# End Source File
# Begin Source File

SOURCE=.\Variables.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Core.h
# End Source File
# Begin Source File

SOURCE=.\Counter.h
# End Source File
# Begin Source File

SOURCE=.\cursors.h
# End Source File
# Begin Source File

SOURCE=.\InputSystem.h
# End Source File
# Begin Source File

SOURCE=.\Lexem.h
# End Source File
# Begin Source File

SOURCE=.\LexemsReader.h
# End Source File
# Begin Source File

SOURCE=.\Messager.h
# End Source File
# Begin Source File

SOURCE=.\Object.h
# End Source File
# Begin Source File

SOURCE=.\ObjectFactory.h
# End Source File
# Begin Source File

SOURCE=.\OperatorsReader.h
# End Source File
# Begin Source File

SOURCE=.\Parser.h
# End Source File
# Begin Source File

SOURCE=.\Tester.h
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TextureManager.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\Variable.h
# End Source File
# Begin Source File

SOURCE=.\Variables.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\code.xml
# End Source File
# Begin Source File

SOURCE=.\GUI_textures.xml
# End Source File
# Begin Source File

SOURCE=.\Level1.xml
# End Source File
# Begin Source File

SOURCE=.\start.xml
# End Source File
# Begin Source File

SOURCE=.\test1.xml
# End Source File
# Begin Source File

SOURCE=.\textures.xml
# End Source File
# Begin Source File

SOURCE=".\Игровая Копия start.xml"
# End Source File
# End Group
# Begin Group "TinyXml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\CppLib\tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CppLib\tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=..\..\CppLib\tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CppLib\tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\..\CppLib\tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CppLib\tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "TheGame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FpsCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\FpsCounter.h
# End Source File
# Begin Source File

SOURCE=.\Inventory.cpp
# End Source File
# Begin Source File

SOURCE=.\Inventory.h
# End Source File
# Begin Source File

SOURCE=.\InventoryObject.cpp
# End Source File
# Begin Source File

SOURCE=.\InventoryObject.h
# End Source File
# Begin Source File

SOURCE=.\Mask.cpp
# End Source File
# Begin Source File

SOURCE=.\Mask.h
# End Source File
# Begin Source File

SOURCE=.\MultiTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiTexture.h
# End Source File
# Begin Source File

SOURCE=.\StarsRect.cpp
# End Source File
# Begin Source File

SOURCE=.\StarsRect.h
# End Source File
# Begin Source File

SOURCE=.\StaticTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticTexture.h
# End Source File
# Begin Source File

SOURCE=.\TestObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TestObject.h
# End Source File
# Begin Source File

SOURCE=.\TextBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TextBox.h
# End Source File
# End Group
# End Target
# End Project
