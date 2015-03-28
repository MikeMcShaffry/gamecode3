/*
* luuid.c
* uuid interface for Lua 5.0
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 24 Jun 2004 14:56:29
* This code is hereby placed in the public domain.
*/

#include <stdio.h>

#include "wxGUID.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#define MYNAME		"uuid"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / Jun 2004"

static int Lnew(lua_State *L)			/** new([s]) */
{
	wxGUID guid;
	guid.Generate();
	char s[2*16+4+2+1];
	guid.GuidToStr(s);
	lua_pushlstring(L,s,sizeof(s)-1);
	return 1;
}

static int Lisvalid(lua_State *L)		/** isvalid(s) */
{
	wxGUID guid;
	const char *s=luaL_checkstring(L,1);
	lua_pushboolean(L, guid.StrToGuid(s));
	return 1;
}

static const luaL_reg R[] =
{
	{ "isvalid",	Lisvalid	},
	{ "new",	Lnew		},
	{ NULL,		NULL	}
};

extern "C" LUAMODULE_API int luaopen_uuid(lua_State *L)
{
 luaL_openlib(L,MYNAME,R,0);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_settable(L,-3);
 return 1;
}
