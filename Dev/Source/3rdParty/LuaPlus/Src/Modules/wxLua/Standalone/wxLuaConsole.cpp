/////////////////////////////////////////////////////////////////////////////
// Purpose:     A console to help debug/use wxLua
// Author:      J Winwood
// Created:     14/11/2001
// Modifications: Enhanced console window functionality
// RCS-ID:      $Id: wxLuaConsole.cpp,v 1.3 2003/10/18 04:11:35 john Exp $
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaConsole.cpp"
    #pragma interface "wxLuaConsole.cpp"
#endif

#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "wxLua.xpm"
#endif

#include "wx/splitter.h"

#include "../Library/wxLua.h"
#include "wxLuaApp.h"
#include "wxLuaConsole.h"

BEGIN_EVENT_TABLE(wxDebugPanel, wxPanel)
    EVT_ERASE_BACKGROUND(wxDebugPanel::OnEraseBackground)
    EVT_SIZE(wxDebugPanel::OnSizeWindow)
END_EVENT_TABLE()

wxDebugPanel::wxDebugPanel(wxWindow *parent, lua_State *pState) :
    wxPanel(parent, -1, wxPoint(0, 0), wxSize(50, 1500)),
    m_pState(pState)
{
    m_listBox = new wxListBox( this,
                               -1,
                               wxPoint(0, 20),
                               wxSize(0, 130),
                               0, NULL,
                               wxLB_SINGLE);
}

void wxDebugPanel::OnSizeWindow(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    m_listBox->SetSize(0, 0, size.x, size.y);
}
 
void wxDebugPanel::FillListbox()
{
    int       nIndex    = 0; 
    lua_Debug luaDebug;
    wxString  buffer;   

    m_listBox->Clear();

    while (lua_getstack(m_pState, nIndex, &luaDebug) != 0)
    {
        buffer.Empty();
        if (lua_getinfo(m_pState, "Sln", &luaDebug))
        {
            int lineNumber = luaDebug.currentline;
            if (lineNumber == -1)
            {
                if (luaDebug.name != NULL)
                    buffer.Printf(wxT("function %s"), 
                                  luaDebug.name);
                else
                    buffer.Printf(wxT("{global}"));
            }
            else
            {
                if (luaDebug.name != NULL)
                    buffer.Printf(wxT("function %s line %u"), 
                                  luaDebug.name,
                                  lineNumber);
                else
                    buffer.Printf(wxT("{global} line %u"), 
                                  lineNumber);
            }

            // skip over ourselves on the stack
            if (nIndex > 0)
                m_listBox->Append(buffer, (void *) nIndex);
        }
        nIndex++;
    }
}

void wxDebugPanel::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

BEGIN_EVENT_TABLE(wxLuaConsole, wxFrame)
    EVT_CLOSE(wxLuaConsole::OnCloseWindow)
    EVT_ERASE_BACKGROUND(wxLuaConsole::OnEraseBackground)
END_EVENT_TABLE()

wxLuaConsole::wxLuaConsole(wxLuaStandaloneApp *pApp, lua_State *pState) : 
    m_pApp(pApp),
    m_fError(false),
    wxFrame(0, -1, wxT("wxLua Console"), wxPoint(0, 0), wxSize(250, 340))
{
    pApp->SetLuaConsole(this);
    
    SetIcon(wxICON(LUA));

    m_splitter = new wxSplitterWindow(this, 
                                      -1, 
                                      wxPoint(0, 0), 
                                      wxSize(0, 0), 
                                      wxSP_3DSASH);
    m_textCtrl = new wxTextCtrl( m_splitter, 
                                        -1, 
                                        wxT(""), 
                                        wxPoint(0, 0), 
                                        wxSize(0, 0), 
                                        wxTE_MULTILINE | wxTE_READONLY
#ifdef __WXMSW__    
                                        | wxTE_RICH
#endif    
                               );    
    m_panel = new wxDebugPanel( m_splitter, pState);
    m_splitter->SplitHorizontally( m_textCtrl, m_panel, 150);
    m_splitter->SetMinimumPaneSize(50);
}

void wxLuaConsole::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxLuaConsole::OnCloseWindow(wxCloseEvent&)
{
    m_pApp->SetLuaConsole(NULL);    
    Destroy();
    if (m_fError)
        wxExit();
}

void wxLuaConsole::DisplayText(wxString msg, bool fError)
{
    if (fError)
        m_fError = true;
    msg += wxString(wxT("\n"));
    m_textCtrl->AppendText(msg);
    if (fError)
        m_panel->FillListbox();
}
