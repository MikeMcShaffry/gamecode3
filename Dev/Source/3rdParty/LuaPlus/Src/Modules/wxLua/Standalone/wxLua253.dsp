# Microsoft Developer Studio Project File - Name="wxLua253" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxLua253 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxLua253.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxLua253.mak" CFG="wxLua253 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxLua253 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wxLua253 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxLua253 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw_wx25"
# PROP BASE Intermediate_Dir "vc_msw_wx25"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw_wx25"
# PROP Intermediate_Dir "vc_msw_wx25"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O1 /I "d:\wx253\include" /I "d:\wx253\lib\vc_lib\msw" /I "." /I "d:\wx253\contrib\include" /I "..\Library" /I "..\lua-5.0.2\include" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /Fd"vc_msw_wx25\wxLua253.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O1 /I "d:\wx253\include" /I "d:\wx253\lib\vc_lib\msw" /I "." /I "d:\wx253\contrib\include" /I "..\Library" /I "..\lua-5.0.2\include" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /Fd"vc_msw_wx25\wxLua253.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i "d:\wx253\include" /i "d:\wx253\lib\vc_lib\msw" /i "." /d "__WXMSW__" /d "_WINDOWS"
# ADD RSC /l 0x405 /i "d:\wx253\include" /i "d:\wx253\lib\vc_lib\msw" /i "." /d "__WXMSW__" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\Release\lua-5.0.2.lib ..\Release\wxLuaLibrary253.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /subsystem:windows /machine:I386 /libpath:"d:\wx253\lib\vc_lib"
# ADD LINK32 ..\Release\lua-5.0.2.lib ..\Release\wxLuaLibrary253.lib wxmsw25_adv.lib wxmsw25_core.lib wxmsw25_dbgrid.lib wxmsw25_gl.lib wxmsw25_html.lib wxmsw25_stc.lib wxmsw25_xrc.lib wxbase25.lib wxbase25_net.lib wxbase25_odbc.lib wxbase25_xml.lib wxmsw25_fl.lib  wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /subsystem:windows /machine:I386 /libpath:"d:\wx253\lib\vc_lib"

!ELSEIF  "$(CFG)" == "wxLua253 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd_wx25"
# PROP BASE Intermediate_Dir "vc_mswd_wx25"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd_wx25"
# PROP Intermediate_Dir "vc_mswd_wx25"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "d:\wx253\include" /I "d:\wx253\lib\vc_lib\mswd" /I "." /I "d:\wx253\contrib\include" /I "..\Library" /I "..\lua-5.0.2\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /Fd"vc_mswd_wx25\wxLua253.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "d:\wx253\include" /I "d:\wx253\lib\vc_lib\mswd" /I "." /I "d:\wx253\contrib\include" /I "..\Library" /I "..\lua-5.0.2\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /Fd"vc_mswd_wx25\wxLua253.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i "d:\wx253\include" /i "d:\wx253\lib\vc_lib\mswd" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS"
# ADD RSC /l 0x405 /i "d:\wx253\include" /i "d:\wx253\lib\vc_lib\mswd" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\Debug\lua-5.0.2.lib ..\Debug\wxLuaLibrary253.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"d:\wx253\lib\vc_lib"
# ADD LINK32 ..\Debug\lua-5.0.2.lib ..\Debug\wxLuaLibrary253.lib wxmsw25d_adv.lib wxmsw25d_core.lib wxmsw25d_dbgrid.lib wxmsw25d_gl.lib wxmsw25d_html.lib wxmsw25d_stc.lib wxmsw25d_xrc.lib wxbase25d.lib wxbase25d_net.lib wxbase25d_odbc.lib wxbase25d_xml.lib wxmsw25d_fl.lib  wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"d:\wx253\lib\vc_lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "wxLua253 - Win32 Release"
# Name "wxLua253 - Win32 Debug"
# Begin Source File

SOURCE=..\Import\Overrides.hpp
# End Source File
# Begin Source File

SOURCE=.\wxLua.ico
# End Source File
# Begin Source File

SOURCE=.\wxLua.rc
# End Source File
# Begin Source File

SOURCE=.\wxLuaApp.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaApp.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaConsole.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebugTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\wxLuaDebugTarget.h
# End Source File
# Begin Source File

SOURCE=.\wxLuaWrap.cpp
# End Source File
# Begin Source File

SOURCE=..\Import\wxLuaWrap.i

!IF  "$(CFG)" == "wxLua253 - Win32 Release"

USERDEP__WXLUA="..\Import\Overrides.hpp"	
# Begin Custom Build - Generating Binding
InputPath=..\Import\wxLuaWrap.i

"wxLuaWrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Building wrapper file wxLuaWrap.cpp 
	wrap.bat 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxLua253 - Win32 Debug"

USERDEP__WXLUA="..\Import\Overrides.hpp"	
# Begin Custom Build - Generating Binding
InputPath=..\Import\wxLuaWrap.i

"wxLuaWrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Building wrapper file wxLuaWrap.cpp 
	wrap.bat 
	
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
