/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaEditor.cpp
// Purpose:     Different editors, consoles for wxLua
// Author:      John Labenski
// Modified by:
// Created:     11/05/2002
// RCS-ID:
// Copyright:   (c) John Labenski
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "wxLuaEditor.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/filename.h"
#include "wx/splitter.h"

#include "wxLuaEditor.h"
#include "wxLuaInterpreter.h"
#include "wxLuaStackTree.h"
#include "wxLuaDebug.h"

#include "../wxLua.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/saveas.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/stop.xpm"

#define STC_DEF_MARGIN_WIDTH 16

DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUASHELL_wxLUA_CREATION)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_LUACONSOLE_wxLUA_CREATION)

//-----------------------------------------------------------------------------
// wxLuaShell
//-----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxLuaShell, StyledTextEditor)

BEGIN_EVENT_TABLE(wxLuaShell, StyledTextEditor)
    EVT_CHAR    (wxLuaShell::OnChar)
    EVT_KEY_DOWN(wxLuaShell::OnKeyDown)
    EVT_STC_UPDATEUI(wxID_ANY, wxLuaShell::OnSTCUpdateUI)
    EVT_LUA_CONSOLE( wxID_ANY, wxLuaShell::OnLua)
    EVT_LUA_WARNING( wxID_ANY, wxLuaShell::OnLua)
    EVT_LUA_ERROR(   wxID_ANY, wxLuaShell::OnLua)
END_EVENT_TABLE()

wxLuaShell::wxLuaShell(wxWindow *parent, int id,
                         const wxPoint& pos, const wxSize& size,
                         long style, StyledTextEditorOptions options, 
                         const wxString& name)
                         : StyledTextEditor(parent, id, pos, size, style, options, name)
{
    Init();  
    
    SetMarginWidth(0,0);
    SetMarginWidth(1,0);
    SetMarginWidth(2,0);
    SetMarginWidth(marginCaret, STC_DEF_MARGIN_WIDTH);

    SetMarginType(marginCaret, wxSTC_MARGIN_SYMBOL);
    SetMarginMask(marginCaret, 1<<markerCaret);
    MarkerDefine(markerCaret, wxSTC_MARK_ARROWS, *wxBLACK, wxColour(255,255,0));
    MarkerAdd(0, markerCaret);

    SetFileName(name, FALSE);
    SetLanguage(wxT("a.lua"));
}

void wxLuaShell::Init()
{
    m_current_line = 0;
    m_total_lines = 0;
    m_readonly = FALSE;
    m_max_lines = 10000;       // arbitrary, seems reasonable

    m_history_index = 0;
    m_historyArray.Add(wxEmptyString);
    m_historyArray.Add(wxEmptyString);    
    
    m_luaInterpreter = NULL;
    m_luaInterpreter_static = FALSE;    
}

wxLuaShell::~wxLuaShell()
{
    if (m_luaInterpreter && !m_luaInterpreter_static)
        delete m_luaInterpreter;
}

void wxLuaShell::AppendText(const wxString &text)
{
    bool ro = GetReadOnly();
    if (ro) SetReadOnly(FALSE);

    StyledTextEditor::AppendText(text);

    if (ro) SetReadOnly(TRUE);
}

void wxLuaShell::SetLuaInterpreter(wxLuaInterpreter *interp, bool is_static)
{
    if (m_luaInterpreter && !m_luaInterpreter_static)
    {
        if (m_luaInterpreter->Ok() && m_luaInterpreter->IsRunning())
            m_luaInterpreter->Break();

        delete m_luaInterpreter;
    }

    m_luaInterpreter = interp;
    m_luaInterpreter_static = is_static;
}

bool wxLuaShell::RecreateLuaInterpreter(wxEvtHandler *evtHandler, int win_id)
{
    wxLuaEvent event(wxEVT_LUASHELL_wxLUA_CREATION, GetId(), NULL);
    event.SetEventObject(this);
    event.SetInt(0);
    event.SetExtraLong(win_id);
    GetEventHandler()->ProcessEvent( event );

    if (event.GetLuaInterpreter() != NULL)
    {
        SetLuaInterpreter(event.GetLuaInterpreter(), event.GetInt() == 1);
        return TRUE;
    }

    wxLuaInterpreter *interp = new wxLuaInterpreter(evtHandler, win_id);
    interp->SetLuaDebugHook( 1000, 50, FALSE, LUA_MASKCOUNT );
    SetLuaInterpreter(interp, FALSE);

    return TRUE;
}

bool wxLuaShell::RunString(const wxString& string, bool append_text)
{
    bool ret = FALSE;
    
    if (string.IsEmpty())
        return ret;
    
    if (append_text)
    {
        LineEndDisplay();
        if (GetLine(GetLineCount()))
            AppendText(wxT("\n"));
        
        AppendText(string);
    }

    AppendText(wxT("\n"));
    
    // restart the interpreter
    if (string == wxT("reset"))
    {
        if (m_luaInterpreter_static)
        {
            AppendText(wxT("Unable to reset lua, interpreter locked by another process.\n"));
        }
        else
        {
            if (RecreateLuaInterpreter(GetEventHandler(), GetId()))
            {
                AppendText(wxT("Sucessfully reset interpreter.\n"));
                ret = TRUE;
            }
            else
                AppendText(wxT("Unsucessfully reset interpreter.\n"));
        }
    }
    else if (m_luaInterpreter)
        ret = (m_luaInterpreter->RunString(string) != 0);
    else
        AppendText(wxT("The lua interpreter is not available - try running 'reset'.\n"));
    
    MarkerAdd(GetCurrentLine(), markerCaret);
    
    return ret;
}

void wxLuaShell::OnSTCUpdateUI(wxStyledTextEvent &event)
{
    event.Skip();

    m_total_lines = wxMax(0, GetLineCount()-1);
    m_current_line = GetCurrentLine();

    // delete lines when more than m_max_lines, you'll eventually crash otherwise
    if (m_total_lines > m_max_lines)
    {
        if (m_readonly) SetReadOnly(FALSE);
        SetTargetStart(0);
        int len = 0;
        for (int n=0; n<m_total_lines-m_max_lines; n++) len += LineLength(n);
        SetTargetEnd(len);
        ReplaceTarget(wxEmptyString);
        MarkerDelete(0, markerCaret);
        if (m_readonly) SetReadOnly(m_readonly);

        m_total_lines = wxMax(0, GetLineCount()-1);
        m_current_line = GetCurrentLine();
    }

    if (((m_current_line < m_total_lines) != m_readonly) ||
        ((LineFromPosition(GetSelectionStart()) < m_total_lines) != m_readonly))
    {
        m_readonly = !m_readonly;
        SetReadOnly(m_readonly);
    }
}

wxString wxLuaShell::GetNextHistoryLine(bool fowards, const wxString &line)
{
    // m_historyArray is padded at beginning and end by wxT("")

    if (line != m_historyArray[m_history_index])
        return m_historyArray[m_history_index];

    if (fowards)
    {
        if (m_history_index >= (int)m_historyArray.GetCount() - 1)
            return wxEmptyString;

        m_history_index++;
    }
    else
    {
        if (m_history_index < 1)
            return m_historyArray[m_history_index];

        m_history_index--;
    }

    return m_historyArray[m_history_index];
}

bool wxLuaShell::CaretOnLastLine(CaretPos_Type option)
{
    if (m_readonly && ((option & move_to_last_line) != 0))
        GotoLine(m_total_lines);

    if (m_readonly && ((option & move_to_end_line) != 0))
        LineEndDisplay();

    if (m_readonly && ((option & move_to_end_text) != 0))
        GotoPos(GetLength());

    return !m_readonly;
}

void wxLuaShell::OnChar(wxKeyEvent &event)
{
    event.Skip();
}

void wxLuaShell::OnKeyDown(wxKeyEvent &event)
{
    event.Skip(FALSE);

    switch (event.GetKeyCode())
    {
        case WXK_UP :
        case WXK_DOWN :
        {
            GotoPos(GetLength());
            wxString line = GetCurLine();
            SetCurrentPos(GetLength() - line.Length());
            DelLineRight();
            AppendText(GetNextHistoryLine(event.GetKeyCode() == WXK_DOWN, line));
            return;
        }
        case WXK_PRIOR :
        case WXK_NEXT :
        case WXK_END :
        case WXK_HOME :
        case WXK_LEFT :
        case WXK_RIGHT :

        case WXK_SHIFT :
        case WXK_CONTROL :
        case WXK_ALT :
        {
            event.Skip();
            return;
        }
        case WXK_RETURN :
        {
            wxString text = GetCurLine().Strip(wxString::both);

            if (text.IsEmpty() || !CaretOnLastLine(nothing))
                return;

            LineEndDisplay();

            if (m_historyArray[m_historyArray.GetCount()-1] != text)
            {
                m_historyArray[m_historyArray.GetCount()-1] = text;
                m_historyArray.Add(wxT(""));
            }
            m_history_index = m_historyArray.GetCount() - 1;

            RunString(text, FALSE);

            return;
        }
        case WXK_BACK :
        {
            int caret_pos = 0;
            wxString linetext = GetCurLine(&caret_pos);
            if (!CaretOnLastLine(move_to_end_text) || (caret_pos < 1))
                return;
        }
        default : // move cursor to end if not already there
        {
            CaretOnLastLine(move_to_last_line);
            break;
        }
    }

    event.Skip();
}

void wxLuaShell::OnLua(wxLuaEvent &event)
{
    if (event.GetEventType() == wxEVT_LUA_CONSOLE)
    {
        AppendText(event.GetString() + wxT("\n"));
    }
    else if (event.GetEventType() == wxEVT_LUA_WARNING)
    {
        AppendText(wxT("\nWarning: ") + event.GetString() + wxT("\n"));
    }
    else if (event.GetEventType() == wxEVT_LUA_ERROR)
    {
        AppendText(wxT("\nErr: ") + event.GetString() + wxT("\n"));
    }
    else
        event.Skip();
}

//-----------------------------------------------------------------------------
// wxLuaEditor
//-----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxLuaEditor, StyledTextEditor)

BEGIN_EVENT_TABLE(wxLuaEditor, StyledTextEditor)
    EVT_STC_MARGINCLICK(wxID_ANY, wxLuaEditor::OnMarginClick)
END_EVENT_TABLE()

wxLuaEditor::wxLuaEditor(wxWindow *parent, int id,
                         const wxPoint& pos, const wxSize& size,
                         long style, StyledTextEditorOptions options, 
                         const wxString& name)
                         : StyledTextEditor(parent, id, pos, size, style, options, name)
{
    SetVisiblePolicy(wxSTC_VISIBLE_SLOP, 3);
    SetXCaretPolicy(wxSTC_CARET_SLOP, 10);
    SetYCaretPolicy(wxSTC_CARET_SLOP, 3);

    // click on margin to set breakpoint
    MarkerDefine(markerBreak, wxSTC_MARK_CIRCLE,     *wxBLACK, *wxRED);
    MarkerDefine(markerError, wxSTC_MARK_SHORTARROW, *wxBLACK, *wxRED);

    SetMarginSensitive(marginBreak, TRUE);
    SetMarginType(marginError, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(marginError, STC_DEF_MARGIN_WIDTH);
    SetMarginMask(marginError, (1<<markerError)|(1<<markerBreak));

    SetFileName(wxT("untitled.lua"));
    SetLanguage(wxT("untitled.lua"));
}

void wxLuaEditor::OnMarginClick( wxStyledTextEvent &event )
{
    if (event.GetMargin() != marginBreak)
    {
        event.Skip(TRUE);
        return;
    }
    int line = LineFromPosition(event.GetPosition());
    //int line = event.GetLine();   - not implemented
    //printf("Margin line %d %d %d %d\n", line, event.GetLine(), MarkerGet(line), 1); fflush(stdout);

    if (HasBreakpoint(line))
        DeleteBreakpoint(line);
    else
        AddBreakpoint(line);
}

bool wxLuaEditor::HasBreakpoint(int line)
{
    if (line < 0)
        return MarkerNext(markerBreak, 1<<markerBreak) >= 0;

    return (MarkerGet(line) & (1<<markerBreak)) != 0;
}
void wxLuaEditor::ClearBreakpoints()
{
    MarkerDeleteAll(markerBreak);
}
void wxLuaEditor::AddBreakpoint(int line)
{
    if (HasBreakpoint(line) || (line<0) || (line>GetLineCount()))
        return;

    (void)MarkerAdd(line, markerBreak);
}
void wxLuaEditor::DeleteBreakpoint(int line)
{
    if (!HasBreakpoint(line))
        return;

    MarkerDelete(line, markerBreak);
}

//-----------------------------------------------------------------------------
// wxLuaConsole
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxLuaConsole, wxWindow);

BEGIN_EVENT_TABLE(wxLuaConsole, wxWindow)

    EVT_MENU(ID_LUACON_LOAD_LUA,   wxLuaConsole::OnMenu)
    EVT_MENU(ID_LUACON_SAVEAS_LUA, wxLuaConsole::OnMenu)
    EVT_MENU(ID_LUACON_RUN_LUA,    wxLuaConsole::OnMenu)
    EVT_MENU(ID_LUACON_BREAK_LUA,  wxLuaConsole::OnMenu)
    EVT_MENU(ID_LUACON_SHOW_STACK, wxLuaConsole::OnMenu)

    EVT_STS_CREATE_EDITOR(wxID_ANY, wxLuaConsole::OnCreateEditor)

    EVT_LUA_CONSOLE     (wxID_ANY, wxLuaConsole::OnLua)
    EVT_LUA_WARNING     (wxID_ANY, wxLuaConsole::OnLua)
    EVT_LUA_ERROR       (wxID_ANY, wxLuaConsole::OnLua)
    EVT_LUA_DEBUG_HOOK  (wxID_ANY, wxLuaConsole::OnLua)
    EVT_LUA_SHUTDOWN    (wxID_ANY, wxLuaConsole::OnLua)
END_EVENT_TABLE()

void wxLuaConsole::Init()
{
    m_luaInterpreter = NULL;
    m_luaInterpreter_static = FALSE;

    m_splitter = NULL;
    m_editorNotebook = NULL;
    m_msgNotebook = NULL;
    m_luaOutput = NULL;
    m_luaShell = NULL;
    m_toolBar = NULL;

    m_options = 0;
    m_show_stack = FALSE;
}

bool wxLuaConsole::Create( wxWindow *parent, int id,
                           const wxPoint& pos, const wxSize& size,
                           long style, long options, const wxString& name)
{
    m_options = options;
    
    if (!wxWindow::Create(parent, id, pos, size, style, name))
        return FALSE;
    
    if ((m_options & LUACON_TOOLBAR) != 0)
    {
        m_toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        PopulateToolBar(m_toolBar, m_options);
        EnableTool(ID_LUACON_BREAK_LUA, FALSE);
    }
    
    m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(200,400));

    // Create the editor notebook
    m_editorOptions = StyledTextEditorOptions(STE_DEFAULT_OPTIONS);
    m_editorOptions.SetToolBar(m_toolBar); // NULL is fine
    m_editorOptions.SetDefaultFileName(wxT("untitled.lua"));
    m_editorNotebook = new StyledTextNotebook(m_splitter, ID_LUACON_EDITOR_NOTEBOOK, 
          wxDefaultPosition, wxSize(200,200), wxCLIP_CHILDREN, m_editorOptions);
    m_editorNotebook->InsertSplitter(-1, wxID_ANY, wxT("untitled.lua"), TRUE);

    // Create the message notebook for shell and output window
    m_shellOptions.SetSplitterOptions(STS_NO_EDITOR);
    m_shellOptions.SetEditorStyles(m_editorOptions.GetEditorStyles());
    m_shellOptions.SetEditorLangs(m_editorOptions.GetEditorLangs());
    m_shellOptions.SetMenuManager(m_editorOptions.GetMenuManager(), TRUE);
//  m_shellOptions.SetEditorPopupMenu(StyledTextEditorMenuManager(0, STE_MENU_EDIT_CUTCOPYPASTE).CreateEditMenu(), FALSE);    
    m_shellOptions.SetToolBar(m_toolBar);
    m_msgNotebook = new StyledTextNotebook(m_splitter, ID_LUACON_MSG_NOTEBOOK, 
            wxDefaultPosition, wxSize(200,200), wxCLIP_CHILDREN, m_shellOptions);

    StyledTextSplitter *splitter = new StyledTextSplitter(m_msgNotebook, wxID_ANY, 
                    wxDefaultPosition, wxDefaultSize, wxSP_3D, m_shellOptions);
    m_luaShell = new wxLuaShell(splitter, ID_LUACON_SHELL, wxDefaultPosition, 
                                wxSize(200,200), 0, m_shellOptions);
    m_luaShell->SetWrapMode(wxSTC_WRAP_WORD);
    m_luaShell->RecreateLuaInterpreter(m_luaShell->GetEventHandler(), m_luaShell->GetId());
    splitter->Initialize(m_luaShell);
    m_msgNotebook->InsertSplitter(-1, splitter, TRUE);
    
    splitter = new StyledTextSplitter(m_msgNotebook, wxID_ANY, 
                        wxDefaultPosition, wxDefaultSize, wxSP_3D, m_shellOptions);
    m_luaOutput = new wxLuaShell(splitter, ID_LUACON_OUTPUT_WIN, wxDefaultPosition, 
                                 wxSize(200,200), 0, m_shellOptions);
    splitter->Initialize(m_luaOutput);
    m_luaOutput->MarkerDeleteAll(wxLuaShell::markerCaret);
    m_luaOutput->SetLuaInterpreter(m_luaInterpreter, TRUE);
    m_luaOutput->SetWrapMode(wxSTC_WRAP_NONE);
    m_luaOutput->SetFileName(wxT("Output"));
    m_msgNotebook->InsertSplitter(-1, splitter, FALSE);

    m_splitter->SetMinimumPaneSize(50);
    m_splitter->SplitHorizontally(m_editorNotebook, m_msgNotebook, GetClientSize().y/2);

    //RecreateLuaInterpreter(GetEventHandler(), m_editorNotebook->GetPage(0)->GetId());

    return TRUE;
}

wxLuaConsole::~wxLuaConsole()
{
    delete m_luaInterpreter;
}

void wxLuaConsole::SetMenuBar(wxMenuBar *menuBar)
{
    m_editorOptions.SetMenuBar(menuBar);
    m_shellOptions.SetMenuBar(menuBar);
}

void wxLuaConsole::SetToolBar(wxToolBar *toolBar)
{
    m_editorOptions.SetToolBar(toolBar);
    m_shellOptions.SetToolBar(toolBar);
    
    m_toolBar = toolBar;
    
    if (!m_luaInterpreter || !m_luaInterpreter->IsRunning())
        EnableTool(ID_LUACON_BREAK_LUA, FALSE);
}

void wxLuaConsole::PopulateToolBar(wxToolBar *toolBar, long options)
{
    bool add_sep = FALSE;

    if ((options & LUACON_TB_FILE) != 0)
    {
        toolBar->AddTool( ID_LUACON_LOAD_LUA,   wxT("Open file"),  wxBitmap(open_xpm),   wxT("Open lua program") );
        toolBar->AddTool( ID_LUACON_SAVEAS_LUA, wxT("Save as..."), wxBitmap(saveas_xpm), wxT("Save lua program") );
        add_sep = TRUE;
    }
    if ((options & LUACON_TB_LUA) != 0)
    {
        if (add_sep) toolBar->AddSeparator();

        toolBar->AddTool( ID_LUACON_RUN_LUA,    wxT("Run Lua"),    wxBitmap(play_xpm), wxT("Run the lua program") );
        toolBar->AddTool( ID_LUACON_BREAK_LUA,  wxT("Stop Lua"),   wxBitmap(stop_xpm), wxT("Stop the running lua program") );
        toolBar->AddSeparator();
        toolBar->AddTool( ID_LUACON_SHOW_STACK, wxT("Stack Tree"), wxBitmap(LUA_xpm),  wxT("Show the lua stack tree") );
    }
    toolBar->Realize();
}

void wxLuaConsole::EnableTool(int tool_id, bool enable)
{
    if (GetToolBar()
#if wxMINOR_VERSION > 4
        && (GetToolBar()->GetToolPos(tool_id) != wxNOT_FOUND)
#endif
        )
    {
        GetToolBar()->EnableTool(tool_id, enable);
    }
}

void wxLuaConsole::SetLuaInterpreter(wxLuaInterpreter *interp, bool is_static)
{
    if (m_luaInterpreter && !m_luaInterpreter_static)
    {
        if (m_luaInterpreter->Ok() && m_luaInterpreter->IsRunning())
            m_luaInterpreter->Break();

        delete m_luaInterpreter;
    }

    m_luaInterpreter = interp;
    m_luaInterpreter_static = is_static;
}

bool wxLuaConsole::RecreateLuaInterpreter(wxEvtHandler *evtHandler, int win_id)
{
    wxLuaEvent event(wxEVT_LUACONSOLE_wxLUA_CREATION, GetId(), NULL);
    event.SetEventObject(this);
    event.SetInt(0);
    event.SetExtraLong(win_id);
    GetEventHandler()->ProcessEvent( event );

    if (event.GetLuaInterpreter() != NULL)
    {
        SetLuaInterpreter(event.GetLuaInterpreter(), event.GetInt() == 1);
        return TRUE;
    }

    wxLuaInterpreter *interp = new wxLuaInterpreter(evtHandler, win_id);
    interp->SetLuaDebugHook( 1000, 50, FALSE, LUA_MASKCOUNT );
    SetLuaInterpreter(interp, FALSE);
    return TRUE;
}

void wxLuaConsole::OnCreateEditor(wxCommandEvent &event)
{
    wxWindow *win = (wxWindow*)event.GetEventObject();
    wxLuaEditor *editor = new wxLuaEditor(win, event.GetInt(),
          wxDefaultPosition, wxDefaultSize, 0, m_editorOptions);
    event.SetEventObject(editor);
}

void wxLuaConsole::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxSize clientSize(GetClientSize());

    if (width == -1)  width  = clientSize.x;
    if (height == -1) height = clientSize.y;
    
    int h = 0;

    if (m_toolBar && (m_toolBar->GetParent() == this))
    {
        m_toolBar->SetSize(0, 1, width, -1); // y = 1 for wxGTK 2 at least
        h = m_toolBar->GetSize().y + 1;      // another +1 for bottom for wxGTK2
    }

    if (m_splitter)
    {
        m_splitter->SetSize(0, h, width, height-h);
        int sash = m_splitter->GetSashPosition();
	    if ((sash < 50) || (sash > height - 50))
            m_splitter->SetSashPosition(wxMin(50, height/2));
    }
    
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);
}

void wxLuaConsole::OnMenu(wxCommandEvent &event)
{
    wxLuaEditor *editor = wxDynamicCast(m_editorNotebook->GetEditor(), wxLuaEditor);
    wxCHECK_RET(editor, wxT("Invalid notebook page"));

    switch(event.GetId())
    {
        case ID_LUACON_LOAD_LUA :
        {
            wxString filename = editor->GetFileName();
            wxString path;

            if (!filename.IsEmpty())
            {
                wxFileName fn(filename);
                path = fn.GetPath();
                filename = fn.GetFullName();
            }

            wxFileDialog dlg(this, wxT("Open file"), path, filename,
                             wxT("Lua (*.lua)|*.lua|Any file (*)|*"),
                             wxOPEN|wxFILE_MUST_EXIST);
            if (dlg.ShowModal() != wxID_OK) return;
            filename = dlg.GetPath();

            if (!filename.IsEmpty())
                editor->LoadFile(filename);

            break;
        }
        case ID_LUACON_SAVEAS_LUA :
        {
            editor->SaveFile(TRUE);
            break;
        }
        case ID_LUACON_RUN_LUA :
        {
            wxString program = editor->GetText();
            if (program.IsEmpty()) break;

            if (m_luaInterpreter)
            {
                wxCHECK_RET(m_luaInterpreter->Destroy(FALSE), wxT("Can't destroy lua interpreter"));
            }

            // Fresh start!
            wxCHECK_RET(RecreateLuaInterpreter(GetEventHandler(), editor->GetId()), wxT("Can't recreate lua interpreter"));

            EnableTool(ID_LUACON_RUN_LUA, FALSE);
            EnableTool(ID_LUACON_BREAK_LUA, TRUE);
            EnableTool(ID_LUACON_SHOW_STACK, FALSE);

            editor->MarkerDeleteAll(wxLuaEditor::markerError);

            m_luaOutput->SetReadOnly(FALSE);
            m_luaOutput->ClearAll();
            m_luaOutput->MarkerDeleteAll(wxLuaShell::markerCaret);
            m_luaOutput->AppendText(wxT("Running lua script : ") + wxNow() + wxT("\n"));
            wxStopWatch stopWatch;
            //m_luaOutput->SetReadOnly(TRUE);

            wxSafeYield(); // allow tools to adjust (sometimes necessary?)

            m_luaOutput->SetLuaInterpreter(m_luaInterpreter, TRUE);

            if (editor->HasBreakpoint(-1))
                m_luaInterpreter->SetLuaDebugHook(1, 50, TRUE, LUA_MASKLINE);
            else
                m_luaInterpreter->SetLuaDebugHook(100, 50, FALSE);

            m_luaOutput->SetReadOnly(TRUE);
            editor->SetReadOnly(TRUE);
            m_luaInterpreter->RunString(program);
            editor->SetReadOnly(FALSE);
            m_luaOutput->SetReadOnly(FALSE);

            m_luaOutput->AppendText(wxT("End lua script : ") + wxNow() + wxT("\n"));
            m_luaOutput->AppendText(wxString::Format(wxT("Execution time : %.3lf(s)\n"), stopWatch.Time()/1000.0));

            m_luaOutput->MarkerAdd(m_luaOutput->GetCurrentLine(), wxLuaShell::markerCaret);

            EnableTool(ID_LUACON_RUN_LUA, TRUE);
            EnableTool(ID_LUACON_BREAK_LUA, FALSE);
            EnableTool(ID_LUACON_SHOW_STACK, TRUE);

            // don't delete it since it may "exist" as a frame or something
            // also allows for showing stack frame
            //if (m_luaInterpreter) { delete m_luaInterpreter; m_luaInterpreter = NULL; }
            break;
        }
        case ID_LUACON_BREAK_LUA :
        {
            if (m_luaInterpreter && m_luaInterpreter->IsRunning())
            {
                m_luaInterpreter->Break(wxT("Interpreter stopped"));
            }
            else if (m_luaShell->GetLuaInterpreter() && m_luaShell->GetLuaInterpreter()->IsRunning())
            {
                m_luaShell->GetLuaInterpreter()->Break(wxT("Interpreter stopped"));
            }

            break;
        }
        case ID_LUACON_SHOW_STACK :
        {
            m_show_stack = TRUE;
            // don't want recursive wxYields if some other interpreter is running
            bool err = FALSE;

            if (m_msgNotebook->GetSelection() == 0)
            {
                if (m_luaShell && m_luaShell->GetLuaInterpreter())
                {
                    m_luaShell->GetLuaInterpreter()->SetLuaDebugHook(1, -1, TRUE);
                    m_luaShell->GetLuaInterpreter()->RunString(wxT("print(\"Showing stack\")\n"));
                    m_luaShell->GetLuaInterpreter()->SetLuaDebugHook(100, 50, FALSE);
                    m_luaShell->MarkerAdd(m_luaShell->GetCurrentLine(), wxLuaShell::markerCaret);
                }
                else
                    err = TRUE;
            }
            else
            {
                if (m_luaInterpreter)
                {
                    m_luaInterpreter->SetLuaDebugHook(1, -1, TRUE);
                    m_luaInterpreter->RunString(wxT("print(\"Showing stack\")\n"));
                    m_luaInterpreter->SetLuaDebugHook(100, 50, FALSE);
                    m_luaOutput->MarkerAdd(m_luaOutput->GetCurrentLine(), wxLuaShell::markerCaret);
                }
                else
                    err = TRUE;
            }

            if (err)
                wxMessageBox(wxT("Invalid lua interpreter."), wxT("Error showing stack"), wxOK, this);

            break;

        }
        default : break;
    }
}

void wxLuaConsole::OnLua(wxLuaEvent &event)
{
    wxLuaShell *outEditor = NULL;

    if (event.GetLuaInterpreter() == m_luaInterpreter)
        outEditor = m_luaOutput;
    else if (event.GetLuaInterpreter() == m_luaShell->GetLuaInterpreter())
        outEditor = m_luaShell;

    OutputLuaEvent(event, outEditor);
}

void wxLuaConsole::OutputLuaEvent(wxLuaEvent &event, wxLuaShell *outEditor)
{
    // perhaps there was an error at startup, but editor isn't created yet
    if (outEditor == NULL)
    {
        return;
    }

    if (event.GetEventType() == wxEVT_LUA_DEBUG_HOOK)
    {
        if (m_show_stack)
        {
            wxLuaInterface luaInterface((lua_State*)event.GetLuaState());
            wxLuaStackFrame StackFrame(this, wxT("wxLua stack frame"),
                                       wxDefaultPosition, wxSize(500, 250),
                                       &luaInterface);
            StackFrame.ShowModal();
            m_show_stack = FALSE;
        }
        else if (event.GetEventObject() == m_luaInterpreter)
        {
            wxLuaEditor *editor = wxDynamicCast(m_editorNotebook->FindWindow(event.GetId()), wxLuaEditor);
            //wxCHECK_RET(editor, wxT("Invalid notebook page"));
            if (editor != NULL)
            {
                int line = event.GetLineNum();
                if (editor->HasBreakpoint(line-1))
                    m_luaInterpreter->Break(wxString::Format(wxT("Stopped at breakpoint, line: %d"), line));
            }
        }
    }
    else if (event.GetEventType() == wxEVT_LUA_CONSOLE)
    {
        outEditor->AppendText(event.GetString() + wxT("\n"));
    }
    else if (event.GetEventType() == wxEVT_LUA_WARNING)
    {
        outEditor->AppendText(wxT("\nWarning: ") + event.GetString() + wxT("\n"));
    }
    else if (event.GetEventType() == wxEVT_LUA_ERROR)
    {
        outEditor->AppendText(wxT("\nErr: ") + event.GetString() + wxT("\n"));

        int line = event.GetLineNum();

        // Put a marker where the error occurred
        if ((line >= 0) && (event.GetEventObject() == m_luaInterpreter))
        {
            wxLuaEditor *editor = wxDynamicCast(m_editorNotebook->FindWindow(event.GetId()), wxLuaEditor);
            //wxCHECK_RET(editor, wxT("Invalid notebook page"));
            if (editor != NULL)
            {
                editor->MarkerDeleteAll(wxLuaEditor::markerError);
                editor->MarkerAdd(line-1, wxLuaEditor::markerError);
                editor->GotoLine(line-1);
                editor->EnsureCaretVisible();
            }
        }
    }
    else if (event.GetEventType() == wxEVT_LUA_SHUTDOWN)
    {
        outEditor->AppendText(wxT("\nLua interpreter shutdown\n"));
    }
}
