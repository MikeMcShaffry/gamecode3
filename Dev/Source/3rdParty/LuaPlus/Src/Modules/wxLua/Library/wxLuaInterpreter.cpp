/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaInterpreter
// Purpose:     A simple class for a C++ wxWidgets program to embed wxLua
// Author:      John Labenski
// Modified by:
// Created:     11/05/2002
// RCS-ID:
// Copyright:   (c) John Labenki
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
// references
// http://www.andreas-rozek.de/Lua/index_en.html interesing libraries perhaps

#ifdef __GNUG__
    #pragma implementation "wxLuaInterpreter.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wxLuaInterpreter.h"
#include "wxLuaInternals.h"

extern int s_wxLuaInterpreter;

wxHashMapLuaInterpreter s_wxHashMapLuaInterpreter;

extern "C"
{
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
    // See lua/src/lstate.c for added hook into luaE_newthread()
    typedef void (*wxLua_newthread_handler)(lua_State *L, lua_State *L1);
    extern void SetwxLua_newthread_handler( wxLua_newthread_handler handler );
    extern wxLua_newthread_handler GetwxLua_newthread_handler();

    // See lua/src/lstate.c for added hook into luaE_freethread()
    typedef void (*wxLua_freethread_handler)(lua_State *L, lua_State *L1);
    extern void SetwxLua_freethread_handler( wxLua_freethread_handler handler );
    extern wxLua_freethread_handler GetwxLua_freethread_handler();
}

// Hook into function calls to luaE_newthread (calls from coroutine.create/wrap)
//  and add the new lua_State as a child sharing it's wxLuaStateVariables
void wxLuaInterpreter_newthread_handler(lua_State *L, lua_State *L1)
{
    // L is the original lua_State, L1 new child thread
    wxLuaInterpreter *L_interp = wxFindLuaInterpreter(L);
    //printf("Adding new lua_State from a thread L=%ld L1=%ld\n", (long)L, (long)L1); fflush(stdout);

    if (L_interp != NULL)
        s_wxHashMapLuaInterpreter[L1] = L_interp;
}

// Hook into function calls to luaE_freethread (calls from coroutine.create/wrap)
//  and remove the child lua_State
void wxLuaInterpreter_freethread_handler(lua_State *L, lua_State *L1)
{
    // L is the original lua_State, L1 child thread, don't remove L
    //printf("Freeing lua_State from a thread L=%ld L1=%ld\n", (long)L, (long)L1); fflush(stdout);
    s_wxHashMapLuaInterpreter.erase(L1);
}

// ----------------------------------------------------------------------------
// Extra wxLua helpers that use the lua_State to find owner wxLuaInterpreter
// ----------------------------------------------------------------------------

wxLuaInterpreter *wxFindLuaInterpreter(lua_State *L)
{
    int top = lua_gettop( L );

    // try to get the state
    lua_pushstring( L, "__wxLuaInterpreter" );
    lua_gettable( L, LUA_REGISTRYINDEX );

    // if nothing was returned or it wasn't a ptr, abort
    if ( top == lua_gettop(L) || !lua_islightuserdata(L, -1) ) 
    {
        lua_settop( L, top );
        return NULL;
    }
    // get the pointer, clean the stack, return
    wxLuaInterpreter* lI = (wxLuaInterpreter*)lua_touserdata( L, -1 );
    lua_settop( L, top );
    return lI;
}

wxLuaStateVariables *wxFindLuaStateVariables(lua_State *L)
{
    wxLuaInterpreter *luaInterpreter = wxFindLuaInterpreter(L);
    if (luaInterpreter && luaInterpreter->Ok())
        return luaInterpreter->GetStateVariables();
    
    return NULL;
}

// ----------------------------------------------------------------------------
// wxFindWindowByPointer - find a window by it's pointer
//     return FALSE if doesn't exist, see wxFindWindowByID and wxFindWindowByLabel
// ----------------------------------------------------------------------------
static wxWindow *wxFindWindowPointerRecursively(const wxWindow *parent, const wxWindow *win)
{
    wxCHECK_MSG(win, NULL, wxT("invalid window in wxFindWindowPointerRecursively"));

    if ( parent )
    {
        // see if this is the one we're looking for
        if ( parent == win )
            return (wxWindow*)win;

        // It wasn't, so check all its children
        for ( wxWindowList::Node * node = parent->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *child_win = (wxWindow *)node->GetData();
            wxWindow *retwin = wxFindWindowPointerRecursively(child_win, win);
            if (retwin)
                return retwin;
        }
    }

    // Not found
    return NULL;
}

// Check to see if wxWidgets still thinks "win" is a valid window
//   parent is the window to start with, if parent=NULL check all windows
static wxWindow* wxFindWindowByPointer(const wxWindow *parent, const wxWindow *win)
{
    wxCHECK_MSG(win, NULL, wxT("Invalid window in wxFindWindowByPointer"));

    if ( parent )
    {
        // just check parent and all its children
        return wxFindWindowPointerRecursively(parent, win);
    }
    // start at very top of wx's windows
    for ( wxWindowList::Node * top_node = wxTopLevelWindows.GetFirst();
          top_node;
          top_node = top_node->GetNext() )
    {
        // recursively check each window & its children
        wxWindow *top_win = top_node->GetData();
        wxWindow *retwin = wxFindWindowPointerRecursively(top_win, win);
        if (retwin)
            return retwin;
    }

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxLuaCleanupWindows - given a wxList of wxWindows it runs wxFindWindowByPointer
//    on it to remove dead pointers from the list if only_check=TRUE or
//    Destroy() the windows and remove them from the list if !only_check.
//    This is for use with wxLuaStateVariables::m_pWindowList.
//    returns TRUE if any windows removed, ie. the list has changed
// ----------------------------------------------------------------------------
bool wxLuaCleanupWindows(wxList *list, bool only_check)
{
    bool removed = FALSE;

    wxNode *node = list->GetFirst();

    while (node != NULL)
    {
        wxWindow *win = (wxWindow *)node->GetData();

        if (!win)
        {
            wxNode *lastNode = node;
            node = node->GetNext();
            list->DeleteNode(lastNode);
        }
        else if (wxFindWindowByPointer(NULL, win) == NULL)
        {
            removed = TRUE;
            wxNode *lastNode = node;
            node = node->GetNext();
            //printf("%d %d\n", int(lastNode), int(node)); fflush(stdout);
            list->DeleteNode(lastNode);
        }
        else if (!only_check)
        {
            removed = TRUE;
            if (win->HasCapture())
                win->ReleaseMouse();

            // release capture for children since we may be abruptly ending
            for ( wxWindowList::compatibility_iterator childNode = win->GetChildren().GetFirst();
                  childNode;
                  childNode = childNode->GetNext() )
            {
                wxWindow *child = childNode->GetData();
                list->DeleteObject(child);
                if (child->HasCapture())
                    child->ReleaseMouse();
            }

			if (!win->IsBeingDeleted())
            {
                delete win;
                // FIXME - win doesn't seem to get immediately destroyed
            }
                
            // wxLuaWindowDestroyHandler should destroy this node
            //  and also delete all the children and their nodes
            //  it's probably best to start from the top again
            list->DeleteObject(win);

            wxPrintf(wxT("%d %d\n"), int(1), int(win)); fflush(stdout);

            node = list->GetFirst();
        }
        else
            node = node->GetNext();
    }
    return removed;
}

// ----------------------------------------------------------------------------
// lua hooks and function replacements for embedding into wxWidgets
// ----------------------------------------------------------------------------

int LuaPrint( lua_State *L )
{
    wxString msg;
    int i, n = lua_gettop(L);
    
    // figure out the types here in c++ land
    lua_getglobal(L, "tostring");
    for (i = 1; i <= n; i++)
    {
        if (i > 1) msg.Append(wxT(", "));

        if (lua_isstring(L, i))
            msg += lua2wx(lua_tostring(L, i));
        else if (lua_isnumber(L, i))
            msg += wxString::Format(wxT("%lf"), lua_tonumber(L, i));
        else if (lua_iscfunction(L, i))
            msg += wxString::Format(wxT("C function %ld"), (long int)lua_tocfunction(L, i));
        else if (lua_isuserdata(L, i))
            msg += wxString::Format(wxT("User data %ld"), (long int)lua_touserdata(L, i));
        else if (lua_isboolean(L, i))
            msg += lua_toboolean(L,i) ? wxT("true") : wxT("false");
        else if (lua_isnil(L,i))
            msg += wxT("nil");
        else if (lua_isnone(L,i))
            msg += wxT("unknown");
        else
            msg += wxString::Format(wxT("%s:%p"), lua_typename(L,lua_type(L,i)), lua_topointer(L,i));
    }

    if (!msg.IsEmpty())
    {
        wxLuaInterpreter *luaInterpreter = wxFindLuaInterpreter(L);

        if (luaInterpreter)
        {
            wxLuaEvent event(wxEVT_LUA_CONSOLE, luaInterpreter->GetId(), luaInterpreter);
            event.SetString(msg);
            luaInterpreter->SendEvent( event );
        }
    }

    return 0; // no items put onto stack
}

// copied from the Lua 5.0.1 debug library
#define LEVELS1 12  /* size of the first part of the stack */
#define LEVELS2 10  /* size of the second part of the stack */

// copied from the Lua 5.0.1 debug library
int LuaTraceback(lua_State *L)
{
    int level = 1;      // skip level 0 (it's this function) 
    int firstpart = 1;  // still before eventual `...' 
    lua_Debug ar;
    if (lua_gettop(L) == 0)
        lua_pushliteral(L, "");
    else if (!lua_isstring(L, 1))   // no string message
        return 1;  
    else 
        lua_pushliteral(L, "\n");
        
    lua_pushliteral(L, "stack traceback:");
    while (lua_getstack(L, level++, &ar)) 
    {
        if (level > LEVELS1 && firstpart) 
        {
            // no more than `LEVELS2' more levels? 
            if (!lua_getstack(L, level+LEVELS2, &ar))
                level--;  // keep going 
            else 
            {
                lua_pushliteral(L, "\n\t...");  // too many levels 
                while (lua_getstack(L, level+LEVELS2, &ar))  // find last levels 
                    level++;
            }
            firstpart = 0;
            continue;
        }
        lua_pushliteral(L, "\n\t");
        lua_getinfo(L, "Snl", &ar);
        lua_pushfstring(L, "%s:", ar.short_src);
        if (ar.currentline > 0)
            lua_pushfstring(L, "%d:", ar.currentline);
        switch (*ar.namewhat) 
        {
            case 'g':  // global 
            case 'l':  // local 
            case 'f':  // field 
            case 'm':  // method 
                lua_pushfstring(L, " in function `%s'", ar.name);
                break;
            default: {
                if (*ar.what == 'm')  // main? 
                    lua_pushfstring(L, " in main chunk");
                else if (*ar.what == 'C' || *ar.what == 't')
                    lua_pushliteral(L, " ?");  // C function or tail call 
                else
                    lua_pushfstring(L, " in function <%s:%d>",
                                    ar.short_src, ar.linedefined);
            }
        }
        lua_concat(L, lua_gettop(L));
    }
    lua_concat(L, lua_gettop(L));
    return 1;
}

int LuaAlert( lua_State *L )
{
    wxLuaInterpreter *luaInterpreter = wxFindLuaInterpreter(L);

    if (luaInterpreter)
    {
        wxString msg = lua2wx(lua_tostring(L, 1));

        wxLuaEvent event(wxEVT_LUA_ERROR, luaInterpreter->GetId(), luaInterpreter);
        event.SetString(msg);

        long line = -1;
        wxString lineStr = msg;

        // Why can't I fill a lua_Debug here? Try to get the line number
        // by parsing the error message that looks like this, 1 is linenumber
        // [string "a = 1("]:1: unexpected symbol near `<eof>'
        while(!lineStr.IsEmpty())
        {
            lineStr = lineStr.AfterFirst(wxT(']'));
            if ((lineStr.Length() > 0) && (lineStr.GetChar(0) == wxT(':')))
            {
                lineStr = lineStr.AfterFirst(wxT(':'));
                if (lineStr.IsEmpty() || lineStr.BeforeFirst(wxT(':')).ToLong(&line))
                    break;
            }
        }

        event.m_commandInt = (int)line;
        luaInterpreter->SendEvent( event );
    }

    return 0;
}

void LuaDebugHook(lua_State *L, lua_Debug *LDebug)
{
    wxLuaInterpreter *luaInterpreter = wxFindLuaInterpreter(L);

    // during program shutdown this can be called - ignore it
    if (luaInterpreter == NULL)
        return;

    // FIXME - for select event types we don't want to do anything
    wxEventType evtType = luaInterpreter->GetStateVariables()->m_inEventType;
    if ((evtType != -1))
        //(evtType == wxEVT_IDLE) && (evtType == wxEVT_PAINT) && 
        //(evtType == wxEVT_DESTROY) && (evtType == wxEVT_CLOSE_WINDOW))
        return;

    // they want to break the program, restore old debug hook, then error out
    if (luaInterpreter->GetBreak())
    {
        // restore hook to previous state see wxLuaInterpreter::Break()
        luaInterpreter->SetLuaDebugHook(luaInterpreter->GetLuaDebugHookCount(),
                                        luaInterpreter->GetLuaDebugHookYield(),
                                        luaInterpreter->GetLuaDebugHookSendEvt(),
                                        luaInterpreter->GetLuaDebugHook());

        terror(L, wx2lua(luaInterpreter->GetBreakMessage()));
        return;
    }

    if (luaInterpreter->GetLuaDebugHookSendEvt())
    {
        int ret = 0;
        ret = lua_getinfo(L, "l", LDebug); // line (ldebug.currentline)

        wxLuaEvent event(wxEVT_LUA_DEBUG_HOOK, luaInterpreter->GetId(), luaInterpreter);
        event.m_lua_Debug = LDebug;
        event.m_commandInt = LDebug->currentline;
        luaInterpreter->SendEvent( event );
        if (event.m_stop_interpreter)
            terror(L, "Interpreter stopped");
    }

    // Try to yield *after* sending event to allow C++ gui update
    if (luaInterpreter->GetLuaDebugHookYield() > 0)
    {
        // yield based on number of ms passed NOT every hook event
        unsigned long last_time = luaInterpreter->GetLastLuaDebugHookTime();
        unsigned long cur_time  = wxGetLocalTimeMillis().GetLo();

        if ((cur_time > last_time + luaInterpreter->GetLuaDebugHookYield()) ||
            (cur_time < last_time)) // wrapped
        {
            luaInterpreter->SetLastLuaDebugHookTime( cur_time );
            wxYieldIfNeeded();
        }
    }
}

//-----------------------------------------------------------------------------
// wxLuaEvent
//-----------------------------------------------------------------------------

DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUA_CREATION)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUA_CONSOLE)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUA_WARNING)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUA_ERROR)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUA_DEBUG_HOOK)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUA_SHUTDOWN)

wxLuaEvent::wxLuaEvent(wxEventType commandType, int id, wxLuaInterpreter *interpreter)
           : wxNotifyEvent(commandType, id)
{
    m_luaInterpreter = interpreter;
    m_lua_Debug = NULL;
    m_stop_interpreter = FALSE;
}

//-----------------------------------------------------------------------------
// wxLuaInterpreter
//-----------------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(wxLuaInterpreter, wxObject)

wxLuaInterpreter::wxLuaInterpreter() : wxObject()
{
    Init();
//	setupthreadhandlers();
}

wxLuaInterpreter::wxLuaInterpreter(lua_State *L, wxEvtHandler *handler, int id) : wxObject()
{
    Init();
    Create(L, handler, id);
}

void wxLuaInterpreter::Init()
{
//jj    if (GetwxLua_newthread_handler() == NULL)
//jj        SetwxLua_newthread_handler( wxLuaInterpreter_newthread_handler );
//jj    if (GetwxLua_freethread_handler() == NULL)
//jj        SetwxLua_freethread_handler( wxLuaInterpreter_freethread_handler );

    m_is_running = FALSE;
    m_user_break = FALSE;

    m_last_debug_hook_time = wxGetLocalTimeMillis().GetLo();

    m_id = wxID_ANY;
    m_evtHandler = NULL;
    m_lua_State = NULL;
    m_luaStateVariables = NULL;

    m_lua_debug_hook_count = 100;
    m_lua_debug_hook_yield = 50;
    m_lua_debug_hook_send_evt = FALSE;
    m_lua_debug_hook = 0;
}


static int wxLuaInterpreter__gc(lua_State* L)
{
    wxLuaInterpreter* interpreter = (wxLuaInterpreter*)lua_unboxpointer(L, 1);
    interpreter->Destroy();
    return 0;
}


bool wxLuaInterpreter::Create(lua_State* L, wxEvtHandler *handler, int id)
{
    if (Ok() && !Destroy())
        return FALSE;

    m_luaStateVariables = new wxLuaStateVariables;
    m_evtHandler = handler;
    m_id = id;

    // Stick us into the lua interpreter - push key, value
//    lua_getglobal( L, "wx" );                               // wx
    lua_pushstring(L, "wxUserData");                        // wx wxUserData
    lua_boxpointer(L, this);                                // wx wxUserData this

    lua_newtable(L);                                        // wx wxUserData this meta
    lua_pushstring(L, "__gc");                              // wx wxUserData this meta __gc
    lua_pushcclosure(L, wxLuaInterpreter__gc, 0);           // wx wxUserData this meta __gc closure
    lua_rawset(L, -3);                                      // wx wxUserData this meta

    lua_setmetatable(L, -2);                                // wx wxUserData this

    lua_rawset(L, LUA_GLOBALSINDEX);                                      // wx
//    lua_pop(L, 1);

    // create a lua instance
	m_lua_State = L;
    
    s_wxHashMapLuaInterpreter[L] = this;
    
    // Stick us into the lua interpreter - push key, value
    lua_pushstring( L, "__wxLuaInterpreter" );
    lua_pushlightuserdata( L, (void*)this );
    // set the value
    lua_settable( L, LUA_REGISTRYINDEX );

    // register handlers to send events
    RegisterFunction(LuaPrint, wxT("print"));
    RegisterFunction(LuaAlert, wxT("_ALERT"));
    RegisterFunction(LuaTraceback, wxT("_TRACEBACK"));

    // Load the wxWidgets libraries 
    RegisterWxLua(L);

    // alert people that we've been created so they can finish setting us up
    wxLuaEvent event(wxEVT_LUA_CREATION, GetId(), this);
    SendEvent( event );

    return TRUE;
}

wxLuaInterpreter::~wxLuaInterpreter()
{
    if (Ok())
    {
        m_evtHandler = NULL;  // no events here, the handler may already be gone
        Destroy(TRUE);
    }
    
    delete m_luaStateVariables;
}

bool wxLuaInterpreter::Destroy(bool force)
{
    wxCHECK_MSG(Ok(), FALSE, wxT("Interpreter not created"));
//    wxCHECK_MSG(!m_is_running, FALSE, wxT("Interpreter still running, can't destroy"));

    // remove deleted windows first
    if (m_luaStateVariables->m_pWindowList &&
        m_luaStateVariables->m_pWindowList->GetCount())
        wxLuaCleanupWindows(m_luaStateVariables->m_pWindowList, TRUE);

    // are there still windows? ask to abort deleting them if !force
    if (m_luaStateVariables->m_pWindowList &&
        m_luaStateVariables->m_pWindowList->GetCount())
    {
        int ret = wxOK;

        if (!force)
        {
            ret = wxMessageBox(wxT("Windows are still open, would you like to delete them?"),
                               wxT("Delete existing windows?"), wxOK|wxCANCEL|wxICON_QUESTION);
        }

        if (ret == wxCANCEL)
            return FALSE;

        wxPrintf(wxT("Deleting windows\n"));

        // delete windows and their eventhandler since they delete the LuaCallbacks
        //  which require a lua_State
        wxLuaCleanupWindows(m_luaStateVariables->m_pWindowList, FALSE);
        // wait for wxWindow::Destroy() to really delete the windows
        //wxYieldIfNeeded();
    }

    CleanupWxLua(m_lua_State);

    s_wxHashMapLuaInterpreter.erase(m_lua_State);

    delete m_luaStateVariables;
    m_luaStateVariables = NULL;
    m_lua_State = NULL;
    m_evtHandler = NULL;

    return TRUE;
}

bool wxLuaInterpreter::Ok() const
{
    return (m_lua_State != NULL);
}

void wxLuaInterpreter::SetEventHandler(wxEvtHandler *evtHandler)
{
    m_evtHandler = evtHandler;
}
wxEvtHandler *wxLuaInterpreter::GetEventHandler() const 
{ 
    return m_evtHandler; 
}

void wxLuaInterpreter::SetId(int id) 
{ 
    m_id = id; 
}
int  wxLuaInterpreter::GetId() const 
{ 
    return m_id; 
}

int wxLuaInterpreter::RunFile(const wxString &filename)
{
    wxCHECK_MSG(Ok(), -1, wxT("lua interpreter not created"));
    wxCHECK_MSG(!m_is_running, -1, wxT("lua interpreter is already running"));
    m_user_break = FALSE;

    m_is_running = TRUE;
//    int ret_val = lua_dofile(m_lua_State, wx2lua(filename));
    int ret_val = LuaDoFile(m_lua_State, filename); // FIXME use LuaDoFile in RunFile ?
    m_is_running = FALSE;
    if (!m_user_break)
        (void)CheckRunError(ret_val);

    m_user_break = FALSE;
    return ret_val;
}

int wxLuaInterpreter::RunString(const wxString &script, const wxString& /*name*/)
{
    wxCHECK_MSG(Ok(), -1, wxT("lua interpreter not created"));
    wxCHECK_MSG(!m_is_running, -1, wxT("lua interpreter is already running"));
    m_user_break = FALSE;

#ifdef __WXMSW__
    // FIXME - hack for \ getting stripped out by lua
    wxString s;
    size_t len = script.Length();
    for (size_t n = 0; n < len; n++)
    {
        s += script.GetChar(n);
        if (script.GetChar(n) == wxT('\\'))
            s += wxT("\\");
    }
#else
    wxString s = script;
#endif

    // This is simple code to run the string, it calls _ALERT function on error
    m_is_running = TRUE;
    //int ret_val = luaL_loadbuffer(m_lua_State, wx2lua(s), s.Len(), wx2lua(name));
    int ret_val = lua_dostring(m_lua_State, wx2lua(s));
    m_is_running = FALSE;

    // if it's been stopped then don't send this error message, already sent stopped one
    if (!m_user_break)
        (void)CheckRunError(ret_val);

    m_user_break = FALSE;
    return ret_val;
}

int wxLuaInterpreter::RunBuffer(const unsigned char buf[], size_t size, const wxString &name)
{
    wxCHECK_MSG(Ok(), -1, wxT("lua interpreter not created"));
    wxCHECK_MSG(!m_is_running, -1, wxT("lua interpreter is already running"));
    m_user_break = FALSE;

    m_is_running = TRUE;
    //int ret_val = luaL_loadbuffer(m_lua_State, wx2lua(script), script.Len(), "=hello");
    int ret_val = lua_dobuffer(m_lua_State, (const char*)buf, size, wx2lua(name));
    m_is_running = FALSE;

    if (!m_user_break)
        (void)CheckRunError(ret_val);

    m_user_break = FALSE;
    return ret_val;
}

bool wxLuaInterpreter::IsRunning() const
{
    return m_is_running;
}

void wxLuaInterpreter::Break(const wxString &msg)
{
    wxCHECK_RET(Ok(), wxT("lua interpreter not created"));
    wxCHECK_RET(m_is_running, wxT("lua interpreter not running"));

    // Lua likes to be stopped within the debug hook, you get funny wxYield
    //  recursion asserts if you call terror() within another wxYield, ie. from a gui button

    m_break_msg = msg;
    m_user_break = TRUE;
    SetLuaDebugHook(1, -1, FALSE);
    m_is_running = FALSE;
}

bool wxLuaInterpreter::GetBreak() const 
{ 
    return m_user_break; 
}
wxString wxLuaInterpreter::GetBreakMessage() const 
{ 
    return m_break_msg; 
}

void wxLuaInterpreter::SetLuaDebugHook(int count, int yield, bool send_debug_evt, int hook)
{
    wxCHECK_RET(Ok(), wxT("lua interpreter not created"));

    m_lua_debug_hook_count = count;
    m_lua_debug_hook_yield = yield;
    m_lua_debug_hook_send_evt = send_debug_evt;
    m_lua_debug_hook = hook;

    if (count > 0)
    {
        // These are the various hooks you can install
        //LUA_MASKCALL, LUA_MASKRET, LUA_MASKLINE, and LUA_MASKCOUNT
        //lua_sethook(m_lua_State, LuaDebugHook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);
        lua_sethook(m_lua_State, LuaDebugHook, hook, count);
    }
    else
        lua_sethook(m_lua_State, LuaDebugHook, 0, 0); // disable hook
}

int  wxLuaInterpreter::GetLuaDebugHookCount() const
{ 
    return m_lua_debug_hook_count; 
}
int  wxLuaInterpreter::GetLuaDebugHookYield() const             
{ 
    return m_lua_debug_hook_yield; 
}
bool wxLuaInterpreter::GetLuaDebugHookSendEvt() const
{ 
    return m_lua_debug_hook_send_evt; 
}
int wxLuaInterpreter::GetLuaDebugHook() const                  
{ 
    return m_lua_debug_hook; 
}

unsigned long wxLuaInterpreter::GetLastLuaDebugHookTime() const 
{ 
    return m_last_debug_hook_time; 
}
void wxLuaInterpreter::SetLastLuaDebugHookTime(unsigned long t) 
{ 
    m_last_debug_hook_time = t; 
}

lua_State *wxLuaInterpreter::GetLuaState() const 
{ 
    return m_lua_State; 
}

wxLuaStateVariables *wxLuaInterpreter::GetStateVariables() const 
{ 
    return m_luaStateVariables; 
}

void wxLuaInterpreter::RegisterFunction(lua_CFunction func, const wxString &funcName)
{
    wxCHECK_RET(Ok(), wxT("lua interpreter not created"));
    lua_register( GetLuaState(), wx2lua(funcName), func );
}

void wxLuaInterpreter::DisplayError( const wxString &errorMsg )
{
    wxLuaEvent event(wxEVT_LUA_ERROR, GetId(), this);
    event.SetString(errorMsg);
    SendEvent( event );
}

bool wxLuaInterpreter::CheckRunError(int rc)
{
    wxString msg;
    if (!wxLuaCheckRunError(rc, &msg))
    {
        wxLuaEvent event(wxEVT_LUA_ERROR, GetId(), this);
        event.SetString(msg);
        event.m_commandInt = -1; // don't know what line the error occured
        SendEvent( event );
        return FALSE;
    }
    return TRUE;
}

void wxLuaInterpreter::SendEvent( wxLuaEvent &event ) const
{
    if (GetEventHandler())
    {
        event.SetEventObject( (wxObject*)this );
        GetEventHandler()->ProcessEvent(event);
    }
}
