/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaEditor.h
// Purpose:     A shell, editor and IDE widget for wxLua
// Author:      John Labenski
// Modified by:
// Created:     11/05/2002
// Copyright:   (c) John Labenski
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LUAEDITOR_H_
#define _WX_LUAEDITOR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "wxLuaEditor.h"
#endif

class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxNotebook;
class WXDLLEXPORT wxSplitterWindow;
class WXDLLEXPORT wxToolBar;
class WXDLLEXPORT wxLuaInterpreter;
class WXDLLEXPORT wxLuaEvent;

#include "stedit/stedit.h"

enum
{
    // buttons
    ID_LUACON_LOAD_LUA = ID_STE__LAST + 1,
    ID_LUACON_SAVEAS_LUA,
    ID_LUACON_RUN_LUA,
    ID_LUACON_BREAK_LUA,
    ID_LUACON_SHOW_STACK,

    // windows
    ID_LUACON_EDITOR_NOTEBOOK,
    ID_LUACON_MSG_NOTEBOOK,
    ID_LUACON_SHELL,
    ID_LUACON_OUTPUT_WIN
};

//-----------------------------------------------------------------------------
// Events for the wxLuaShell and wxLuaConsole when a wxLua interpreter is 
// being created. You may intercept this event to adjust or run strings
// to finish creating the wxLua interpreter.
//
// EVT_LUASHELL_wxLUA_CREATION(id, wxLuaEvent& fn)
// EVT_LUACONSOLE_wxLUA_CREATION(id, wxLuaEvent& fn)
//-----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUASHELL_wxLUA_CREATION,   7000) 
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_LUACONSOLE_wxLUA_CREATION, 7001) 
END_DECLARE_EVENT_TYPES()

#define EVT_LUASHELL_wxLUA_CREATION(id, fn)   DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUASHELL_wxLUA_CREATION,   id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LUACONSOLE_wxLUA_CREATION(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_LUACONSOLE_wxLUA_CREATION, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxLuaEventFunction) & fn, (wxObject *) NULL ),

//-----------------------------------------------------------------------------
// wxLuaShell - a basic console type interactive interpreter widget
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaShell : public StyledTextEditor
{
public :
    wxLuaShell( wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                StyledTextEditorOptions options = StyledTextEditorOptions(),
                const wxString& name = wxT("wxLuaShell"));

    virtual ~wxLuaShell();

    // The preferred method to append text to the shell. 
    virtual void AppendText(const wxString &text);

    enum margin_markerType
    {
        marginCaret = StyledTextEditor::marginMarker,
        markerCaret = 1
    };

    enum CaretPos_Type
    {
        nothing           = 0,
        move_to_last_line = 0x001,
        move_to_end_line  = 0x002,
        move_to_end_text  = 0x004
    };

    // is the caret on the last line? option sets what to do with it
    bool CaretOnLastLine(CaretPos_Type option = nothing);

    // get the previous/next string last typed in, for up/dn arrows
    //   line is the current string at the cursor
    wxString GetNextHistoryLine(bool fowards, const wxString &line);

    // Get the interpreter this shell uses
    wxLuaInterpreter *GetLuaInterpreter() const { return m_luaInterpreter; }
    // Set this shell to use another interpreter, will delete it if !is_static
    //   note shell has built in function
    virtual void SetLuaInterpreter(wxLuaInterpreter *interp, bool is_static);
    // Used to create/recreate local interpreter
    //   Can be subclassed to use your own interpreter derived from a wxLuaInterpreter
    //   or a new interpreter can be set in by catching the wxEVT_LUASHELL_wxLUA_CREATION
    //   wxLuaEvent that's sent with this control's id and *this as the GetEventObject
    //   the preferred window ID is in event.GetExtraLong()
    //   if you call event.SetLuaInterpreter(new wxLuaInterpreter(...))
    //   and event.SetInt(1 for static interpreter or 0 for not static)
    //   then SetLuaIntpreter(event.GetLuaInterpreter(), event.GetInt() == 1)
    //   is called. You'll probably want to set the lua interpreter's eventhandler
    //   to this as well as it's win id
    virtual bool RecreateLuaInterpreter(wxEvtHandler *evtHandler, int win_id);

    // Run a string, append the string if append_text
    bool RunString(const wxString& string, bool append_text = FALSE);
    
    // implementation
    void OnChar(wxKeyEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnSTCUpdateUI(wxStyledTextEvent &event);
    void OnLua(wxLuaEvent& event);

protected:
    wxLuaInterpreter *m_luaInterpreter;
    bool m_luaInterpreter_static;

    wxArrayString m_historyArray;
    int m_history_index;

    int  m_current_line;
    int  m_total_lines;
    bool m_readonly;
    int  m_max_lines;

private:
    void Init();
    DECLARE_EVENT_TABLE();
    DECLARE_CLASS(wxLuaShell);
};

//-----------------------------------------------------------------------------
// wxLuaEditor - a notebook page for editing a lua program
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaEditor : public StyledTextEditor
{
public :
    wxLuaEditor(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0, 
                StyledTextEditorOptions options = StyledTextEditorOptions(),
                const wxString& name = wxT("wxLuaEditor"));

    virtual ~wxLuaEditor() {}

    enum mark_Type
    {
        marginError = StyledTextEditor::marginMarker,
        marginBreak = StyledTextEditor::marginMarker,
        markerError = 1,
        markerBreak = 2
    };

    void ClearBreakpoints();
    bool HasBreakpoint(int line); // if line = -1 means has any breakpoints
    void AddBreakpoint(int line);
    void DeleteBreakpoint(int line);

protected:
    void OnMarginClick( wxStyledTextEvent &event );

private:
    DECLARE_EVENT_TABLE();
    DECLARE_CLASS(wxLuaEditor);
};

//-----------------------------------------------------------------------------
// wxLuaConsole - toolbar at the top for open/save/run/stop lua programs
//            notebook with editors for editing lua programs
//            notebook with a console and an output window for above editors
//-----------------------------------------------------------------------------

enum wxLuaConsole_Styles
{
    LUACON_TB_FILE = 0x0001,
    LUACON_TB_LUA  = 0x0002,
    LUACON_TOOLBAR = LUACON_TB_FILE|LUACON_TB_LUA,
    LUACON_DEFAULT = LUACON_TOOLBAR
};

class WXDLLEXPORT wxLuaConsole : public wxWindow
{
public:
    wxLuaConsole() : wxWindow() { Init(); }
    wxLuaConsole( wxWindow *parent, wxWindowID id,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  long options = LUACON_DEFAULT,
                  const wxString& name = wxT("wxLuaConsole"))
                  : wxWindow()
                  //: wxWindow(parent, id, pos, size, style, name)
    {
        Init();
        Create(parent, id, pos, size, style, options, name);
    }

    bool Create( wxWindow *parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 long options = LUACON_DEFAULT,
                 const wxString& name = wxT("wxLuaConsole"));
   
    virtual ~wxLuaConsole();

    // Get the interpreter this shell uses
    wxLuaInterpreter *GetLuaInterpreter() const { return m_luaInterpreter; }
    // Set this shell to use another interpreter, will delete it if !is_static
    //   note shell has built in function
    virtual void SetLuaInterpreter(wxLuaInterpreter *interp, bool is_static);
    // Used to create/recreate local interpreter
    //   Can be subclassed to use your own interpreter derived from a wxLuaInterpreter
    //   or a new interpreter can be set in by catching the wxEVT_LUASHELL_wxLUA_CREATION
    //   wxLuaEvent that's sent with this control's id and *this as the GetEventObject
    //   the preferred window ID is in event.GetExtraLong()
    //   if you call event.SetLuaInterpreter(new wxLuaInterpreter(...))
    //   and event.SetInt(1 for static interpreter or 0 for not static)
    //   then SetLuaIntpreter(event.GetLuaInterpreter(), event.GetInt() == 1)
    //   is called. You'll probably want to set the lua interpreter's eventhandler
    //   to this as well as it's win id
    virtual bool RecreateLuaInterpreter(wxEvtHandler *evtHandler, int win_id);

    virtual void OutputLuaEvent(wxLuaEvent &event, wxLuaShell *outputWin);

    wxSplitterWindow *GetSplitterWin() const { return m_splitter; }
    StyledTextNotebook *GetEditorNotebook() const { return m_editorNotebook; }
    StyledTextNotebook *GetMsgNotebook() const { return m_msgNotebook; }
    wxLuaShell *GetLuaOutputWin() const { return m_luaOutput; }
    wxLuaShell *GetLuaShellWin() const { return m_luaShell; }

    // don't take ownership, just use
    void SetMenuBar(wxMenuBar *menuBar);
    wxMenuBar* GetMenuBar() const { return m_editorOptions.GetMenuBar(); } // may be NULL
    // don't take ownership, just use
    void SetToolBar(wxToolBar *toolbar);
    wxToolBar *GetToolBar() const { return m_editorOptions.GetToolBar(); } // may be NULL
    void PopulateToolBar(wxToolBar *toolBar, long options=LUACON_TOOLBAR);
    void EnableTool(int tool_id, bool enable);

    // implementation

    void OnMenu(wxCommandEvent &event);
    void OnLua(wxLuaEvent &event);

    void OnCreateEditor(wxCommandEvent &event);

protected:
    
    virtual void DoSetSize(int x, int y, int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    
    wxLuaInterpreter *m_luaInterpreter;
    bool m_luaInterpreter_static;

    wxSplitterWindow *m_splitter;
    StyledTextNotebook *m_editorNotebook;
    StyledTextNotebook *m_msgNotebook;
    wxLuaShell *m_luaOutput;
    wxLuaShell *m_luaShell;
    wxToolBar *m_toolBar;
    StyledTextEditorOptions m_editorOptions;
    StyledTextEditorOptions m_shellOptions;

    long m_options;

private:
    bool m_show_stack;
    void Init();

    DECLARE_EVENT_TABLE();
    DECLARE_DYNAMIC_CLASS(wxLuaConsole);
};

#endif  // _WX_LUAEDITOR_H_
