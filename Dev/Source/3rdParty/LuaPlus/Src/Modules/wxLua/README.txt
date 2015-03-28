*******************************************************************************
This is NOT meant for public consumption, though you can use it if you like. 
It is probably only be useful for C++ developers.

This is a modified/derivitive version of wxLua that differs from the original
by providing a class wxLuaInterpreter that can be run in the main C++ thread.
It can also operate using sockets as wxLua does. The wrapper files are 
separated and you can choose what portions of wxWidgets to use by editing a 
lua_setup.h file.

Changes: John Labenski, Ray Gilbert.
*******************************************************************************

How to compile wxLua

--------------------------------------------------
Linux w/ GTK
--------------------------------------------------

Make sure that wx-config is working and points to the
correct version of wxWindows you have.

Type "make" in the root dir to make lua50,  
the wxLua/Library library, the Standalone and Embedded programs.

The output of the wxLua/Library is to the lib/ dir of 
wxWindows. It creates both a static and shared library.

The output of lua50 is in wxLua/lua50/bin

--------------------------------------------------
Microsoft Visual Studio
--------------------------------------------------

You MUST set the OS environment variable WXWIN to
the root directory of your wxWindows library
For example in your autoexec.bat put
set WXWIN=c:\wxWindows-2.4.2

In windows 2000 right click on "My Computer"     
select "Properties" menu item and "Advanced" notebook
tab. Click "Environment Variables..." button and
add or edit WXWIN=c:\wxWindows-2.4.2. You probably
have to restart Visual Studio for the setting to take.

If a specifc library cannot be found, it may be because
the projects are being compiled in the wrong order. 
Just to "Batch build" are "Build" again once the 
first compilation is complete.

In the root directory there are two dsw files.

wxLua_wx24.dsp for using wxWindows 2.4.2 
   Perhaps 2.4.1 and 2.4.0 would work?

wxLua_wx25.dsp for using wxWindows CVS HEAD
   as of Oct 11, 2003 

Do "build batch..."

Build these in this order (or see note above about missing libs)

lua50_wx2X - for the lua library
             output in wxLua/Debug/ or wxLua/Release/ 
             as lua50(d)_wx2X.lib

lua 
    for the lua.exe standalone lua program
    output in wxLua/Debug/ or wxLua/Release/
    you probably only want the release version

luac 
    for luac.exe the standalone lua compilier
    output in wxLua/Debug/ or wxLua/Release/
    you probably only want the release version

bin2c 
    a tiny program to convert a text file (lua program)
    into const unsigned char filename[] = { 123, 232...};
    that can be used in lua_dobuffer
    you probably only want the release version
    output in wxLua/Debug/ or wxLua/Release/

wxLuaLib_wx2X
    This is the wxLua library that is necessary for the 
    Standalone wxLua program to link to
    output in wxLua/Debug/ or wxLua/Release/ as wxlualib(d)_wx2X.lib

wxLua_wx2X  
    This the the wxLua/Standalone main wxLua executable. 
    Use this to run, edit, debug your wxLua programs
    Output into Standalone/vc_msw(d)_2X

Embedded - for an Embedded sample program (ide like editor)
           output in Embedded/vc_msw(d)_wx2X


