// tLuaCOMEnumerator.h: interface for the tLuaCOMEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TLUACOMENUMERATOR_H
#define _TLUACOMENUMERATOR_H

#include <wtypes.h>
#include <oleauto.h>

extern "C"
{
#include "lua.h"
}

#include "tLuaCOMTypeHandler.h"
#include "luabeans.h"


class tLuaCOMEnumerator  
{
public:
	static void registerLuaType(lua_State* L, const char* module);
	void push();
	tLuaCOMEnumerator(lua_State* L_param, LuaBeans* lbeans_param, IEnumVARIANT* pEV_param);
	virtual ~tLuaCOMEnumerator();

protected:
	lua_State* L;
	int callCOMmethod(const char* name, int first_param, int last_param);
	static int call_method(lua_State* L);
	static int garbagecollect(lua_State* L);
	static int index(lua_State* L);

  static const char* type_name;
  static const char* module_name;
  static const char* pointer_type_name;
  IEnumVARIANT* pEV;

  tLuaCOMTypeHandler* typehandler;
  LuaBeans* lbeans;
};

#endif // _TLUACOMENUMERATOR_H
