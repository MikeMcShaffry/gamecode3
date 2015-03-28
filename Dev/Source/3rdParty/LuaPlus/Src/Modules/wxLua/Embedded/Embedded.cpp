/////////////////////////////////////////////////////////////////////////////
// Name:        Embedded.cpp
// Purpose:     wxLuaEmbedded 
// Author:      John Labenski
// Modified by:
// Created:     04/01/98
// RCS-ID:
// Copyright:   (c) John Labenski
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "stedit/stedit.h"
#include "wxLuaEditor.h"
#include "wxLuaInterpreter.h"

#include "luasetup.h"

// ----------------------------------------------------------------------------
// resources and constants
// ----------------------------------------------------------------------------

#include "../wxLua.xpm"

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:

    virtual bool OnInit();
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class LuaFrame : public StyledTextFrame
{
public:

    LuaFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
             long frame_style = wxDEFAULT_FRAME_STYLE) : StyledTextFrame()
    {
        Create(title, pos, size, frame_style);
    }

    bool Create(const wxString& title, const wxPoint& pos, const wxSize& size,
                long frame_style = wxDEFAULT_FRAME_STYLE);

    virtual bool HandleMenuEvent( wxCommandEvent &event );
    void OnAbout(wxCommandEvent& event);
    
    wxLuaConsole *m_luaConsole;

private:
    DECLARE_ABSTRACT_CLASS(LuaFrame)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    LuaFrame *frame = new LuaFrame(_T("wxLua Embedded App"),
                                   wxPoint(20, 20), wxSize(600, 400));

    long frameOpts = STF_DEFAULT_OPTIONS & (~STF_USECONFIG) & (~STF_NOTEBOOK);
    StyledTextEditorOptions steOptions(STE_DEFAULT_OPTIONS,
                                       STS_DEFAULT_OPTIONS,
                                       STN_DEFAULT_OPTIONS,
                                       frameOpts,
                                       StyledTextEditorMenuOptions(0),
                                       wxT("untitled.lua"));

    frame->CreateOptions(steOptions);
    wxIcon icon;
    icon.CopyFromBitmap(wxBitmap(LUA_xpm));
    frame->SetIcon(icon);

    frame->GetToolBar()->AddSeparator();
    frame->m_luaConsole->PopulateToolBar(frame->GetToolBar(), LUACON_TB_LUA);
    frame->m_luaConsole->SetToolBar(frame->GetToolBar());
    frame->m_luaConsole->SetMenuBar(frame->GetMenuBar());  
    
    // The size of the frame isn't set when the splitter is created, resize it
    wxSplitterWindow *splitWin = frame->m_luaConsole->GetSplitterWin();
    splitWin->SetSashPosition(splitWin->GetSize().y/2);

    frame->Show(TRUE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(LuaFrame, StyledTextFrame)

BEGIN_EVENT_TABLE(LuaFrame, StyledTextFrame)
END_EVENT_TABLE()

bool LuaFrame::Create(const wxString& title, const wxPoint& pos, const wxSize& size,
                      long frame_style)
         
{
    m_luaConsole = NULL;
    if (!StyledTextFrame::Create(NULL, -1, title, pos, size, frame_style))
        return FALSE;

    m_luaConsole = new wxLuaConsole(this, -1, wxDefaultPosition, wxSize(400,600), 0, 0);
    m_notebook = m_luaConsole->GetEditorNotebook();
    return TRUE;
}

bool LuaFrame::HandleMenuEvent(wxCommandEvent &event)
{
    wxWindow *focusWin = FindFocus();

    switch (event.GetId())
    {
        case wxID_ABOUT : 
        {
            OnAbout(event); 
            return TRUE;
        }
        case wxID_CUT       :
        case wxID_COPY      :
        case wxID_PASTE     :
        case wxID_SELECTALL :
        {
            // These windows only handle these IDs
            if (focusWin == m_luaConsole->GetLuaOutputWin())
                return m_luaConsole->GetLuaOutputWin()->HandleMenuEvent(event);
            if (focusWin == m_luaConsole->GetLuaShellWin()) 
                return m_luaConsole->GetLuaShellWin()->HandleMenuEvent(event);

            break;
        }

        default : break;
    }

    if (!StyledTextFrame::HandleMenuEvent(event))
        m_luaConsole->OnMenu(event);
        
    return TRUE;
}

void LuaFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    // since the dialog is modal, it's ok that the interpreter is created on the stack
    wxLuaInterpreter interpreter(NULL, -1);

    wxString script;
    script += wxT("local msg = \"Welcome to wxLua!\\n\"");
    script += wxT("msg = msg..string.format(\"%s\", \"This dialog is created from a wxLua script.\")");
    script += wxT("wx.wxMessageBox(msg, \"About wxLua Embedded\", wx.wxOK + wx.wxICON_INFORMATION)");

    interpreter.RunString(script);

    //wxString msg;
    //msg.Printf( _T("This is the About dialog of the minimal sample.\n")
    //            _T("Welcome to %s"), wxVERSION_STRING);
    //wxMessageBox(msg, _T("About Minimal"), wxOK | wxICON_INFORMATION, this);
}
