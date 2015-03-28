/////////////////////////////////////////////////////////////////////////////
// Purpose:     Class to let hook back wx events to Lua
// Author:      Francis Irving
// Created:     21/01/2002
// Copyright:   (c) 2002 Creature Labs. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_CALLBACK_H
#define WX_LUA_CALLBACK_H

extern "C"
{
    typedef struct lua_State lua_State;
}

class wxLuaStateVariables;

// ----------------------------------------------------------------------------
// wxLuaCallback - proxy class to pass a reference to a lua function
//                 through the wxWidgets event system.
// ----------------------------------------------------------------------------

class wxLuaCallback : public wxEvtHandler
{
    DECLARE_CLASS(wxLuaCallback);

public:
    wxLuaCallback( lua_State    *state,
                   int           theRoutine,
                   int           id,
                   wxEventType   eventType,
                   wxEvtHandler *pHandler);

    virtual ~wxLuaCallback();

    void ClearState() { m_luaState = NULL; }
    void ClearStateVars() { m_luaStateVars = NULL; }

    lua_State* GetLuaState() const { return m_luaState; }
    int GetId() const { return m_id; }
    wxEventType GetEventType() const { return m_eventType; }
    wxEvtHandler *GetEvtHandler() const { return m_pHandler; }

    // central event handler
    void EventHandler(wxEvent& event);

protected:
    // event handler dispatcher back to Lua
    void CallFunction(wxEvent *pEvent);

    // reference to the routine to call
    int                  m_routine;
    // Lua state instance
    lua_State           *m_luaState;
    // store the state variables since we're added to a list of callbacks.
    // The lua_State is NULLed when the window (handler) is destroyed,
    // but we still want to remove us from the state vars callback list.
    wxLuaStateVariables *m_luaStateVars;
    wxEvtHandler        *m_pHandler;
    int                  m_id;
    wxEventType          m_eventType;
};

// ----------------------------------------------------------------------------
// LuaDestroyCallback - proxy class to handle the destruction of wxWindow
//                      derived objects
// ----------------------------------------------------------------------------

class wxLuaDestroyCallback : public wxEvtHandler
{
    DECLARE_CLASS(wxLuaDestroyCallback)

public:
    wxLuaDestroyCallback( lua_State    *state,
                          int           id,
                          wxEvtHandler *pHandler,
                          int           iTag);

    virtual ~wxLuaDestroyCallback();

    void ClearState() { m_luaState = NULL; }
    void ClearStateVars() { m_luaStateVars = NULL; }

    lua_State* GetLuaState() const { return m_luaState; }
    wxEvtHandler* GetEvtHandler() const { return m_pHandler; }

    // central event handler
    void EventHandler(wxEvent& event);

protected:
    void OnDestroy();

    // Lua state instance
    lua_State           *m_luaState;
    // store the state variables see wxLuaCallback for reasoning
    wxLuaStateVariables *m_luaStateVars;
    wxEvtHandler        *m_pHandler;
    int                  m_id;
};

#endif //WX_LUA_CALLBACK_H
