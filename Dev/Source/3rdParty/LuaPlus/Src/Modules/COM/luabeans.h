#ifndef LUABEANS_H
#define LUABEANS_H

class LuaBeans
{
public:

  class Events
  {
  public:
    lua_CFunction gettable;
    lua_CFunction settable;
    lua_CFunction noindex;
    lua_CFunction call;
    lua_CFunction gc;

    Events()
    {
      gettable = settable = noindex = call = gc = NULL;
    }
  };

	 lua_State* getLuaState(void);
   static LuaBeans* createBeans(
     lua_State *L,
     const char* module_name,
     const char* name);

   static LuaBeans* getBeans(lua_State *lua_state, const char* module, const char* name);

   void registerObjectEvents(class Events& events);
   void registerPointerEvents(class Events& events);
   void  reg_methods(struct luaL_reg* table, int n);
   void  push(void* userdata);
   void* check_tag(int index);
   void* from_lua(int index);


  protected:

   static char* init_code;

   lua_State *L;

   struct MethodsList
   {
      int n;
      struct luaL_reg* table;
   } *mlist;

   char* tag_name;
   char* udtag_name;
   Events* pEvents;
   const char* module_name;

   LuaBeans(lua_State *lua_state, const char* p_module, const char* name );
   ~LuaBeans();

   static int gc(lua_State* L);

};
#endif
