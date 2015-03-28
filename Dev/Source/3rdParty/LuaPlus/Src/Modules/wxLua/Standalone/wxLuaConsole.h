/////////////////////////////////////////////////////////////////////////////
// Purpose:     Interface to a console to help debug wxLua
// Author:      Francis Irving
// Created:     16/01/2002
// Copyright:   (c) 2002 Creature Labs. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_CONSOLE_H
#define WX_LUA_CONSOLE_H

class wxLuaStandaloneApp;
#include "../Library/wxLua.h"

class wxDebugPanel : public wxPanel
{
public:
    wxDebugPanel(wxWindow *parent, lua_State *pState);

    void FillListbox();

private:
    lua_State    *m_pState;
    wxListBox    *m_listBox;

    DECLARE_EVENT_TABLE()

    void OnSizeWindow(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
};

// define our console class
class wxLuaConsole : public wxFrame
{
public:
    wxLuaConsole(wxLuaStandaloneApp *pApp, lua_State *pState);
    void DisplayText(wxString msg, bool fError = false);

private:
    DECLARE_EVENT_TABLE()

    void OnCloseWindow(wxCloseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void FillListbox();

    wxSplitterWindow   *m_splitter;
    wxTextCtrl         *m_textCtrl;
    wxLuaStandaloneApp *m_pApp;
    wxDebugPanel       *m_panel;
    bool                m_fError;
};

#endif
