////////////////////////////////////////////////////////////////////////////////
// Purpose:     Provides a wxApp that you should derive yours from to use wxLua
// Author:      Francis Irving
// Created:     16/01/2002
// Modified:    J. Winwood. April 2002. Added debugging interfaces
// Copyright:   (c) 2002 Creature Labs. All rights reserved.
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
////////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_LIBRARY_H
#define WX_LUA_LIBRARY_H

#ifdef __GNUG__
    #pragma interface "wxLuaLibrary.h"
#endif

#include "wx/utils.h"
#include "wx/app.h"

// Forward references
class wxLuaDebugEvent;
class wxLuaStackFrame;

// ----------------------------------------------------------------------------
// wxLuaHandler - Allow for Custom Lua Handler (an alternative to wxLuaApp)
// ----------------------------------------------------------------------------

// Lua App Interface 
class wxLuaHandler
{
public:
    wxLuaHandler();
    virtual ~wxLuaHandler();

    virtual void AddPendingEvent(wxEvent& event) = 0;

    virtual bool ProcessEvent(wxEvent& event) = 0;

    virtual void SetStackFrame(wxLuaStackFrame *pStackFrame) = 0;

    virtual void DisplayError(const wxString &strError) const = 0;

    void EnterLuaCriticalSection()
    {
        m_luaCriticalSection.Enter();
    }

    void LeaveLuaCriticalSection()
    {
        m_luaCriticalSection.Leave();
    }

public:
    static wxLuaHandler* sm_luahandler;
    wxCriticalSection  m_luaCriticalSection;
};

// Lua App/Debugger Interface
wxLuaHandler& wxGetLuaHandler();

/////////////////////////////////////////////////////////////////////////////

#endif //WX_LUA_LIBRARY_H
