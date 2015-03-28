#include "LuaPlus/LuaPlus.h"
using namespace LuaPlus;
extern "C" {
#include "LuaPlus/src/lualib.h"
}
//#include <math.h>
//#include <assert.h>

#if _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else // _WIN32_WCE
int __cdecl main(int argc, char* argv[])
#endif // _WIN32_WCE
{
//	LuaStateOwner state;
//	state->DoString("s = \"12345678901234567890123456789012345678901234567890\"");
//	state->DoString("s2 = L\"abcdefghijklmnopqrstuvwxyzzyxwvutsrqponmlkjihgfedcba\"");
//	lua_State* L = lua_open();
	LuaStateOwner state;
//	LuaState* state = LuaState::CastState(L);
//	luaL_openlibs(L);
//	state->DoString("s = \"12345678901234567890123456789012345678901234567890\"");
//	state->DoFile("TestAnsi.lua");
//	LuaObject sObj = state->GetGlobal("s");
//	lua_close(L);
//	luaL_dostring(state->GetCState(), "s = \"12345678901234567890123456789012345678901234567890\"");

	return 0;
}
