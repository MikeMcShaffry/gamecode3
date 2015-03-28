#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include "LuaCompat.h"
}
#include "luabeans.h"


#include "tLuaCOMException.h"
#include "LuaAux.h"
#include "tUtil.h"

static void register_methods(lua_State *L, int table_pos, struct luaL_reg* list, int n)
{
   for (int i = 0; i < n; i++)
   {
      lua_pushstring(L, list[i].name);
      lua_pushcfunction(L, list[i].func);
      lua_settable(L, table_pos);
   }
}

int LuaBeans::gc(lua_State* L)
{
  LuaBeans *beans = (LuaBeans*) luaCompat_getTypedObject(L, -1);

  CHECKPRECOND(beans);

  delete beans;

  return 0;
}

LuaBeans* LuaBeans::createBeans(lua_State *L,
                                const char* module_name,
                                const char* name)
{
  LUASTACK_SET(L);

  luaCompat_moduleGet(L, module_name, "LuaBeansTable");

  if(lua_isnil(L, -1))
  {
    lua_pop(L, 1);

    lua_newtable(L);

    lua_pushvalue(L, -1);

    luaCompat_moduleSet(L, module_name, "LuaBeansTable");
  }
 
  lua_pushstring(L, (char*)name);

  LuaBeans* lbeans = new LuaBeans(L, module_name, name);
  luaCompat_pushTypeByName(L, module_name, "LuaBeansObject");

  // sets GC callback
  lua_pushcfunction(L, LuaBeans::gc);
  luaCompat_handleGCEvent(L);

  luaCompat_newTypedObject(L, lbeans);

  lua_settable(L, -3);

  lua_pop(L, 1);

  LUASTACK_CLEAN(L, 0);

  return lbeans;
}

LuaBeans* LuaBeans::getBeans(lua_State *L,
                             const char* module,
                             const char* name)
{
  LUASTACK_SET(L);

  luaCompat_moduleGet(L, module, "LuaBeansTable");

  lua_pushstring(L, (char*)name);
  lua_gettable(L, -2);

  LuaBeans* lbeans = (LuaBeans*) luaCompat_getTypedObject(L, -1);

  lua_pop(L, 2);

  LUASTACK_CLEAN(L, 0);

  return lbeans;
}

LuaBeans::LuaBeans(lua_State *lua_state, const char* p_module, const char* name )
{
   char lua_name[50];

   this->mlist = NULL;

   L = lua_state;
   module_name = p_module;

   sprintf(lua_name,"%s",name);
   tag_name = tUtil::strdup(lua_name);
   luaCompat_newLuaType(L, module_name, tag_name);

   sprintf(lua_name,"%s_UDTAG",name);
   udtag_name = tUtil::strdup(lua_name);
   luaCompat_newLuaType(L, module_name, udtag_name);
}

LuaBeans::~LuaBeans()
{
  if (this->mlist)
    delete this->mlist;

  free(tag_name);
  free(udtag_name);
}

void LuaBeans::registerObjectEvents(class Events& events)
{
  LUASTACK_SET(L);

  luaCompat_pushTypeByName(L, module_name, tag_name);

  if(events.gettable)
  {
    lua_pushcfunction(L, events.gettable);
    luaCompat_handleGettableEvent(L);
  }

  if(events.settable)
  {
    lua_pushcfunction(L, events.settable);
    luaCompat_handleSettableEvent(L);
  }

  if(events.noindex)
  {
    lua_pushcfunction(L, events.noindex);
    luaCompat_handleNoIndexEvent(L);
  }

  if(events.call)
  {
    lua_pushcfunction(L, events.call);
    luaCompat_handleFuncCallEvent(L);
  }

  if(events.gc)
  {
    lua_pushcfunction(L, events.gc);
    luaCompat_handleGCEvent(L);
  }

  lua_pop(L, 1);

  LUASTACK_CLEAN(L, 0);
}

void LuaBeans::registerPointerEvents(class Events& events)
{
  LUASTACK_SET(L);

  luaCompat_pushTypeByName(L, module_name, udtag_name);

  if(events.gettable)
  {
    lua_pushcfunction(L, events.gettable);
    luaCompat_handleGettableEvent(L);
  }

  if(events.settable)
  {
    lua_pushcfunction(L, events.settable);
    luaCompat_handleSettableEvent(L);
  }

  if(events.noindex)
  {
    lua_pushcfunction(L, events.noindex);
    luaCompat_handleNoIndexEvent(L);
  }

  if(events.gc)
  {
    lua_pushcfunction(L, events.gc);
    luaCompat_handleGCEvent(L);
  }

  lua_pop(L, 1);

  LUASTACK_CLEAN(L, 0);
}

void LuaBeans::reg_methods(struct luaL_reg* table, int n)
{
   if (this->mlist == NULL)
   {
      this->mlist = new LuaBeans::MethodsList;
   }
   this->mlist->n = n;
   this->mlist->table = table;
}

void LuaBeans::push( void* userdata )
{
  LUASTACK_SET(L);

  lua_newtable(L);

  lua_pushstring(L, "_USERDATA_REF_");

  luaCompat_pushTypeByName(L, module_name, udtag_name);
  luaCompat_newTypedObject(L, userdata);

  lua_settable(L, -3);

  if (this->mlist)
    register_methods(L, -1, this->mlist->table, this->mlist->n);

  luaCompat_pushTypeByName(L, module_name, tag_name);
  luaCompat_setType(L, -2);

  LUASTACK_CLEAN(L, 1);
}

void* LuaBeans::check_tag(int index)
{
  void* userdata = this->from_lua(index);

  luaL_arg_check(L, (userdata!=NULL), index, "Object type is wrong");

  return userdata;
}

void* LuaBeans::from_lua(int index)
{
  LUASTACK_SET(L);

  void *obj = NULL;

  lua_pushvalue(L, index);
  if (lua_istable(L, -1) && luaCompat_isOfType(L, module_name, tag_name))
  {
    lua_pushstring(L, "_USERDATA_REF_");
    lua_gettable(L, index);
    obj = luaCompat_getTypedObject(L, -1);
    lua_pop(L, 1);
  }
  lua_pop(L, 1);

  LUASTACK_CLEAN(L, 0);
  
  return obj;
}


lua_State* LuaBeans::getLuaState()
{
  return L;
}

