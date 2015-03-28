#! ../lua50/bin/lua

fVerbose = 1

luasetupFile    = "../Embedded/luasetup.h.in"
luasetupFileOut = "../Embedded/luasetup.h"

inputPaths = { "../Import/" }
outputFile = "../Embedded/wxLuaWrap.cpp"

dofile("../Import/wxLuaWrap.lua")

