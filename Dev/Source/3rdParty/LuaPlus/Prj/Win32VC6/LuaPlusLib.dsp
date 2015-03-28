# Microsoft Developer Studio Project File - Name="LuaPlusLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LuaPlusLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LuaPlusLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LuaPlusLib.mak" CFG="LuaPlusLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LuaPlusLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaPlusLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LuaPlusLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Src\LuaPlus\include" /I "..\..\Src\LuaPlus\src" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LUA_OPNAMES" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LuaPlusLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\Src\LuaPlus\include" /I "..\..\Src\LuaPlus\src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LUA_OPNAMES" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF

# Begin Target

# Name "LuaPlusLib - Win32 Release"
# Name "LuaPlusLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlusAddons.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlusFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaStackObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\lwstrlib.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaAutoBlock.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\luace.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaLink.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaObject.inl
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlus.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlusAddons.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlusCD.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaPlusFunctions.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaStackObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaStackObject.inl
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaStackTableIterator.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaState.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaState.inl
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaStateOutFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\LuaTableIterator.h
# End Source File
# End Group
# Begin Group "Lua Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lapi.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lapi.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\lauxlib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\lbaselib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lcode.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lcode.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\ldblib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ldebug.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ldebug.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ldo.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ldo.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ldump.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lfunc.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lgc.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lgc.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\liolib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\llex.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\llex.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\llimits.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\lmathlib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lmem.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lmem.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\loadlib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lobject.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lobject.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lopcodes.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lopcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lparser.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lparser.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lstate.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lstate.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lstring.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lstring.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\lstrlib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ltable.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ltable.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lib\ltablib.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ltests.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ltm.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\ltm.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lundump.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lundump.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lvm.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lvm.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lzio.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\src\lzio.h
# End Source File
# End Group
# Begin Group "Lua Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Src\LuaPlus\include\lauxlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\include\lua.h
# End Source File
# Begin Source File

SOURCE=..\..\Src\LuaPlus\include\lualib.h
# End Source File
# End Group
# End Target
# End Project
