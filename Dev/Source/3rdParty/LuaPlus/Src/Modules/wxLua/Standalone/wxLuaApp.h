/////////////////////////////////////////////////////////////////////////////
// Purpose:     Standalone wxLua application
// Author:      Francis Irving
// Created:     16/01/2002
// Modified:    J. Winwood. Added debugging support
//              May 2002.
// Copyright:   (c) 2002 Creature Labs. All rights reserved.
// Copyright:   (c) 2002-2004 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUAAPP_H
#define WX_LUAAPP_H

#include "../Library/wxLuaLibrary.h"

#ifdef __WXMSW__
    #include "wxLuaConsole.h"
#endif

#if wxCHECK_VERSION(2, 3, 0)
    #include "wxLuaDebugTarget.h"
#endif

#include "wx/app.h"

class LuaHandler;

class wxLuaStandaloneApp : public wxApp
{
private:
    wxLuaStackFrame  *m_pStackFrame;
    LuaHandler		 *m_luaHandler;
    wxLuaInterpreter *m_luaInterpreter;
    lua_State		 *m_luaState;
#ifdef __WXMSW__
    wxLuaConsole	 *m_pConsole;
#endif
    bool              m_fMemoryBitmapAdded;
#if wxCHECK_VERSION(2, 3, 0)
    LuaDebugTarget   *m_pDebugTarget;
#endif

    DECLARE_EVENT_TABLE();

public:
// Override the base class virtual functions
    bool OnInit();
    int  OnExit();
    void DisplayError(const wxString &strError) const;

// set the lua console
#ifdef __WXMSW__
    void SetLuaConsole(wxLuaConsole *pConsole) { m_pConsole = pConsole; }
#endif

    void SetStackFrame(wxLuaStackFrame *pStackFrame) { m_pStackFrame = pStackFrame; }
    void OnLua(wxLuaEvent &event);

    static int LUACALL PanicFunction(lua_State * /*L*/);

// register all the classes, methods, etc. in the generated list
    void RegisterClasses(lua_State *L);

    int  RunEditor();

private:
	void CreateSimpleArgs();

	void OnDebugStackEnum(wxLuaDebugEvent &event);
	void OnDebugTableEnum(wxLuaDebugEvent &event);
	void OnDebugStackEntryEnum(wxLuaDebugEvent &event);
};

DECLARE_APP(wxLuaStandaloneApp)

#endif
