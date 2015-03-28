#! ../lua50/bin/lua

fVerbose = 1

luasetupFile    = "../Standalone/luasetup.h.in"
luasetupFileOut = "../Standalone/luasetup.h"

inputPaths = { "../Import/" }
outputFile = "../Standalone/wxLuaWrap.cpp"

dofile("../Import/wxLuaWrap.lua")

