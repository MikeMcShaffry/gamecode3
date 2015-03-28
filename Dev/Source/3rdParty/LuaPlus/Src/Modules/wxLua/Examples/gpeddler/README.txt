
===================================================
 gpeddler 2.0 - by Enrico Colombini <erix@erix.it>
===================================================

gpeddler 2 is a primitive TSP evolutionary solver written to evaluate wxLua, a Lua interface to the wxWindows portable interface library. It contains a working wxLua example using coroutines, buffered drawing and HTML rendering.

The documentation can be read with a browser or from inside the program itself; it includes some details about:

  * A simple object-oriented approach in Lua (without using metamethods).
  * Coroutines in Lua.

gpeddler 2 requires wxLua 2.1 or later, and works under the systems supported by wxLua (currently Windows and Linux). 
Lua and wxWindows are included in the wxLua executable; it can be downloaded from <http://www.luascript.thersgb.net>.

To start the program, move to the directory containing the Lua files and write:

  wxLua gpeddler2.wx.lua

The wxLua binary executable must be either in the same directory, or reachable through the path.
