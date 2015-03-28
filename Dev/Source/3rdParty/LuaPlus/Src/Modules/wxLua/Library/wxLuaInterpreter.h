/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaInterpreter
// Purpose:     A simple class for a C++ wxWidgets program to embed wxLua
// Author:      John Labenski
// Modified by:
// Created:     11/05/2002
// Copyright:   (c) John Labenski
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LUA_INTERPRETER_H_
#define _WX_LUA_INTERPRETER_H_

#ifdef __GNUG__
    #pragma interface "wxLuaInterpreter.h"
#endif

extern "C"
{
    #include "lua.h"  // for DebugHook defines

    // To not include "lua.h" use these
    //typedef struct lua_State lua_State;
    //typedef struct lua_Debug lua_Debug;
    //typedef int (*lua_CFunction)(lua_State *);
}

class WXDLLEXPORT wxEvtHandler;
class WXDLLEXPORT wxLuaEvent;
class WXDLLEXPORT wxLuaStateVariables;
class WXDLLEXPORT wxLuaInterpreter;

#include "wx/hashmap.h"
WX_DECLARE_VOIDPTR_HASH_MAP( wxLuaInterpreter *, wxHashMapLuaInterpreter );
extern wxHashMapLuaInterpreter s_wxHashMapLuaInterpreter;

//-----------------------------------------------------------------------------
// Extra wxLua helpers that use the lua_State to find owner wxLuaInterpreter
//-----------------------------------------------------------------------------

extern WXDLLEXPORT wxLuaInterpreter    *wxFindLuaInterpreter(lua_State *L);
extern WXDLLEXPORT wxLuaStateVariables *wxFindLuaStateVariables(lua_State *L);

//-----------------------------------------------------------------------------
// wxLuaInterpreter
//
// notes : create instance, call RunString("some lua code")
//         Events are sent to the evthander if !null with the given id #
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaInterpreter : public wxObject
{
public:
    // default constructor - must call Create in order to use
    wxLuaInterpreter();
    // Create a wxLua instance, handler and id can be null/wxID_ANY for don't care
    wxLuaInterpreter(lua_State* L, wxEvtHandler *handler, wxWindowID id = wxID_ANY);

    virtual ~wxLuaInterpreter();

    // (re)Create a new wxLua instance, handler and id can be null/wxID_ANY for don't care
    //   don't forget to ShutDown the last one first (if you want to)
    bool Create(lua_State* L, wxEvtHandler *handler=NULL, wxWindowID id = wxID_ANY);
    // destroy and cleanup the interpreter, returns sucess
    // if 'force' then make sure all wxWindows are destroyed.
    bool Destroy(bool force = TRUE);

    // has the interpreter been sucessfully created
    bool Ok() const;

    // Set the event handler that the events will be sent to, can be null
    void SetEventHandler(wxEvtHandler *evtHandler);
    wxEvtHandler *GetEventHandler() const;

    // Get/Set the id that the events will be sent with
    void SetId(int id);
    int  GetId() const;

    // Run a lua file
    int RunFile(const wxString &fileName);
    // Run a string that contains lua code
    int RunString(const wxString &script, const wxString& name = wxEmptyString);
    // Run a char array #include(d) from bin2c compilation
    int RunBuffer(const unsigned char buf[], size_t size, const wxString &name = wxT("= lua"));

    // Is a program running now
    bool IsRunning() const;
    // Break a currently running lua program
    void Break(const wxString &message = wxT("Interpreter stopped"));
    // has Break() been called
    bool GetBreak() const;
    wxString GetBreakMessage() const;

    // Have lua run an internal hook function with this mask
    //   hook = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT
    // Every count hook mask a wxEVT_LUA_DEBUG_HOOK event is sent if send_debug_evt
    //   if yield > 0 then wxYield is called every yield milliseconds
    //      I don't know what resonable values are
    // turn the hook off with count < 1
    void SetLuaDebugHook(int count = 1000,
                         int yield = 50,
                         bool send_debug_evt = FALSE,
                         int hook = LUA_MASKCOUNT);
    int  GetLuaDebugHookCount() const;
    int  GetLuaDebugHookYield() const;
    bool GetLuaDebugHookSendEvt() const;
    int  GetLuaDebugHook() const;

    // registers a new function for Lua
    void RegisterFunction(lua_CFunction func, const wxString &funcName);

    // Get a pointer to the lua_State that "is" lua
    lua_State *GetLuaState() const;
    // Get a pointer to additional wxLua variables used for this lua_State
    wxLuaStateVariables *GetStateVariables() const;

    // -----------------------------------------------------------------------
    // implementation

    // generic function to generate a wxEVT_LUA_ERROR with the string message
    void DisplayError( const wxString &errorMsg );

    // checks return val of 'lua_load' and `lua_pcall', if error send wxEVT_LUA_ERROR
    bool CheckRunError(int rc);

    // internally used so debug hook events are sent no more often than yield time
    unsigned long GetLastLuaDebugHookTime() const;
    void SetLastLuaDebugHookTime(unsigned long t);

    void SendEvent( wxLuaEvent &event ) const;

    // operators
    operator lua_State*() { return GetLuaState(); }

protected:
    bool m_is_running;

    unsigned long m_last_debug_hook_time;

    bool m_user_break;
    wxString m_break_msg;

    wxLuaStateVariables *m_luaStateVariables;
    lua_State *m_lua_State;

    int  m_lua_debug_hook_count;
    int  m_lua_debug_hook_yield;
    bool m_lua_debug_hook_send_evt;
    int  m_lua_debug_hook;

    wxEvtHandler *m_evtHandler;
    int m_id;    

private :    
    void Init();
    DECLARE_ABSTRACT_CLASS(wxLuaInterpreter)
};

//-----------------------------------------------------------------------------
// wxLuaEvent
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaEvent: public wxNotifyEvent
{
public:
    wxLuaEvent(wxEventType commandType = wxEVT_NULL, wxWindowID id = wxID_ANY,
               wxLuaInterpreter *intrepreter=NULL);

    wxLuaEvent( const wxLuaEvent &event ) : wxNotifyEvent(event)
    {
        m_stop_interpreter = event.m_stop_interpreter;
        m_luaInterpreter = event.m_luaInterpreter;
        m_lua_Debug = event.m_lua_Debug;
    }

    virtual wxEvent *Clone() const { return new wxLuaEvent(*this); }

    // use GetString method to retrieve info

    // Get the line number in the code, -1 if unknown
    int GetLineNum() const { return m_commandInt; }

    wxLuaInterpreter *GetLuaInterpreter() const { return m_luaInterpreter; }
    void SetLuaInterpreter(wxLuaInterpreter *interp) { m_luaInterpreter = interp; }

    lua_State *GetLuaState() const { return m_luaInterpreter ? m_luaInterpreter->GetLuaState() : NULL; }
    // non null only for EVT_LUA_DEBUG_HOOK
    lua_Debug *GetLuaDebug() const { return m_lua_Debug; }

    // If called from a wxEVT_LUA_DEBUG_HOOK the interpreter will stop
    void StopInterpreter(bool stop) { m_stop_interpreter = stop; }

    bool m_stop_interpreter;
    wxLuaInterpreter *m_luaInterpreter;
    lua_Debug *m_lua_Debug;
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUA_CREATION,   0) // a wxLuaInterpreter is being created
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUA_CONSOLE,    0) // print statements and such, check GetString()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUA_WARNING,    0) // unused, lua doesn't seem to warn for anything
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUA_ERROR,      0) // error occurred, check GetString()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUA_DEBUG_HOOK, 0) // see LuaDebugHook function
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUA_SHUTDOWN,   0) // unused, I used this in some other interpreter, not sure why it'd be useful
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxLuaEventFunction)(wxLuaEvent&);

#define EVT_LUA_CREATION(id, fn)   DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUA_CREATION,   id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LUA_CONSOLE(id, fn)    DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUA_CONSOLE,    id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LUA_WARNING(id, fn)    DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUA_WARNING,    id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LUA_ERROR(id, fn)      DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUA_ERROR,      id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LUA_DEBUG_HOOK(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUA_DEBUG_HOOK, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LUA_SHUTDOWN(id, fn)   DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUA_SHUTDOWN,   id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),

#endif  // _WX_LUA_INTERPRETER_H_
