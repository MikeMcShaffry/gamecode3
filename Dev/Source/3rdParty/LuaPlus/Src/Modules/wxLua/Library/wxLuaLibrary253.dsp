# Microsoft Developer Studio Project File - Name="wxLuaLibrary253" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxLuaLibrary253 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxLuaLibrary253.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxLuaLibrary253.mak" CFG="wxLuaLibrary253 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxLuaLibrary253 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxLuaLibrary253 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe


!IF  "$(CFG)" == "wxLuaLibrary253 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw_wx25"
# PROP BASE Intermediate_Dir "vc_msw_wx25"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw_wx25"
# PROP Intermediate_Dir "vc_msw_wx25"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /GR /GX /MD /O1 /GR /GX /W4 /Fdvc_msw_wx25\wxLuaLibrary253.pdb /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /c
# ADD CPP /nologo /FD /GR /GX /MD /O1 /GR /GX /I "d:\wx253\include" /I "d:\wx253\lib\vc_lib\msw" /I "." /I "d:\wx253\contrib\include" /I "..\lua-5.0.2\include" /W4 /Fdvc_msw_wx25\wxLuaLibrary253.pdb /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Release\wxLuaLibrary253.lib"
# ADD LIB32 /nologo /out:"..\Release\wxLuaLibrary253.lib"

!ELSEIF  "$(CFG)" == "wxLuaLibrary253 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd_wx25"
# PROP BASE Intermediate_Dir "vc_mswd_wx25"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd_wx25"
# PROP Intermediate_Dir "vc_mswd_wx25"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /GR /GX /MDd /Od /GR /GX /W4 /Zi /Gm /GZ /Fdvc_mswd_wx25\wxlualibd_wx25.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /c
# ADD CPP /nologo /FD /GR /GX /MDd /Od /GR /GX /I "d:\wx253\include" /I "d:\wx253\lib\vc_lib\mswd" /I "." /I "d:\wx253\contrib\include" /I "..\lua-5.0.2\include" /W4 /Zi /Gm /GZ /Fdvc_mswd_wx25\wxlualibd_wx25.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Debug\wxLuaLibrary253.lib"
# ADD LIB32 /nologo /out:"..\Debug\wxLuaLibrary253.lib"

!ENDIF

# Begin Target

# Name "wxLuaLibrary253 - Win32 Release"
# Name "wxLuaLibrary253 - Win32 Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wxLua.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaCallback.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebug.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebuggerService.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebuggerService.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebugIO.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebugIO.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebugServer.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebugServer.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaHtmlWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaHtmlWindow.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaInternals.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaInternals.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaInterpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaInterpreter.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaLibrary.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaPrinting.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaPrinting.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaSocket.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaSplitTree.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaSplitTree.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaStackTree.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaStackTree.h
# End Source File
# End Target
# End Project
