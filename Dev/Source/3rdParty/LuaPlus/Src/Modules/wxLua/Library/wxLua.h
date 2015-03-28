/////////////////////////////////////////////////////////////////////////////
// Purpose:     Interface to wxLua
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_H
#define WX_LUA_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

// If you're using stdcall in Lua, then override this with
//      LUACALL=__stdcall
// in your makefile or project.
#ifndef LUACALL
    #define LUACALL
#endif

// This is the function you need to call:
void LUACALL RegisterWxLua(lua_State *L, bool registerTypes = true);
// Two cases where you might multiply call RegisterWxLua:
// 1. You have several lua_State values.  Then registerTypes
// should always be true.
// 2. You have only one lua_State, but have swapped out
// the global table.  Then you want to set registerTypes to false
// for second and subsequent registrations.
int LUACALL RegisterWxLuaFunctions(lua_State *L, bool registerTypes = true);

// Call this on application exit, to let wxLua tidy up
void LUACALL CleanupWxLua(lua_State *L, bool closeLua = true);

// Given a lua tag, return the corresponding tag name
const char * LUACALL GetLuaTagName(lua_State *L, int nTag);

// Used to create a set of standard bitmaps for wxLua
int LUACALL CreateStandaloneBitmaps(lua_State *luaState);

#endif //WX_LUA_H
