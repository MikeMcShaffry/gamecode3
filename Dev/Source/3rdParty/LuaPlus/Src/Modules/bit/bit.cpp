#include "LuaPlus/LuaPlus.h"

using namespace LuaPlus;

#pragma warning(disable: 4244)

/* Bitwise operations library */
/* Reuben Thomas   Nov00-13May03 */

typedef __int64 Integer;
typedef unsigned __int64 UInteger;

#define luaL_checkbit(L, n)  ((Integer)luaL_checknumber(L, n))
#define luaL_checkubit(L, n) ((UInteger)luaL_checkbit(L, n))

#define TDYADIC(name, op, t1, t2) \
  static int int_ ## name(lua_State* L) { \
    lua_pushnumber(L, \
      luaL_check ## t1 ## bit(L, 1) op luaL_check ## t2 ## bit(L, 2)); \
    return 1; \
  }

#define DYADIC(name, op) \
  TDYADIC(name, op, , )

#define MONADIC(name, op) \
  static int int_ ## name(lua_State* L) { \
    lua_pushnumber(L, op luaL_checkbit(L, 1)); \
    return 1; \
  }

#define VARIADIC(name, op) \
  static int int_ ## name(lua_State *L) { \
    int n = lua_gettop(L), i; \
    Integer w = luaL_checkbit(L, 1); \
    for (i = 2; i <= n; i++) \
      w op ## = luaL_checkbit(L, i); \
    lua_pushnumber(L, w); \
    return 1; \
  }


static int LS_help(LuaState* state)
{
	LuaFunctionVoid print = state->GetGlobal("print");
	print("bit\n"
"Provides bitwise logical operations:\n"
"bit.band(w1,...)\n"
"returns the bitwise and of the w's\n"
"bit.bor(w1,...)\n"
"returns the bitwise or of the w's \n"
"bit.bxor(w1,...)\n"
"returns the bitwise exclusive or of the w's \n"
"bit.bnot(a)\n"
"returns the one's complement of a \n"
"bit.lshift(a,b)\n"
"returns a shifted left b places \n"
"rshift(a,b)\n"
"returns a shifted logically right b places \n"
"arshift(a,b)\n"
"returns a shifted arithmetically right b places \n"
"mod(a,b)\n"
"returns the integer remainder of a divided by b \n"
"The arguments to all functions should be integral numbers. The number of bits available for logical operations depends on the data type used to represent Lua numbers; this is typically 8-byte IEEE floats, which give 53 bits (the size of the mantissa).\n"
);
	return 0;
}

MONADIC(bnot,     ~)
VARIADIC(band,    &)
VARIADIC(bor,     |)
VARIADIC(bxor,    ^)
TDYADIC(lshift,  <<, , u)
TDYADIC(rshift,  >>, u, u)
TDYADIC(arshift, >>, , u)
DYADIC(mod,      %)

static const struct luaL_reg bitlib[] = {
  {"bnot",    int_bnot},
  {"band",    int_band},
  {"bor",     int_bor},
  {"bxor",    int_bxor},
  {"lshift",  int_lshift},
  {"rshift",  int_rshift},
  {"arshift", int_arshift},
  {"mod",     int_mod},
  {NULL, NULL}
};


extern "C" LUAMODULE_API int luaopen_bit(lua_State* L)
{
	LuaState* state = LuaState::CastState(L);
	LuaObject obj = state->GetGlobals().CreateTable( "bit" );
	obj.Register("bnot", int_bnot);
	obj.Register("band", int_band);
	obj.Register("bor", int_bor);
	obj.Register("bxor", int_bxor);
	obj.Register("lshift", int_lshift);
	obj.Register("rshift", int_rshift);
	obj.Register("arshift", int_arshift);
	obj.Register("mod", int_mod);
	obj.Register("help", LS_help);
	return 0;
}

