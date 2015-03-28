/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxApp and wxFrame
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// wxApp

%if wxLUA_USE_wxApp

%include "wx/app.h"

%class wxApp, wxEvtHandler
    %builtin wxApp* wxGetBaseApp()

    <wxver25|%wxcompat24 virtual wxLog* CreateLogTarget()
    void Dispatch()
    wxString GetAppName() const
    <wxver25&%win bool GetAuto3D() const
    wxString GetClassName() const
    bool GetExitOnFrameDelete() const
    wxWindow * GetTopWindow() const
    bool GetUseBestVisual() const
    wxString GetVendorName() const
    void ExitMainLoop()
    bool Initialized()
    int MainLoop()
    bool Pending()
    <wxver25 bool SendIdleEvents()
    ~<wxver25 bool SendIdleEvents(wxWindow* win, wxIdleEvent& event)
    void SetAppName(const wxString& name)
    %property=AppName, read, write
    <wxver24&%win|%mac void SetAuto3D(const bool auto3D)
    <wxver24&%win|%mac %property=Auto3D, read, write
    void SetClassName(const wxString& name)
    %property=ClassName, read, write
    void SetExitOnFrameDelete(bool flag)
    %property=ExitOnFrameDelete, read, write
    void SetTopWindow(wxWindow* window)
    %property=TopWindow, read, write
    void SetVendorName(const wxString& name)
    %property=VendorName, read, write
    //virtual wxIcon GetStdIcon(int which) const
    void SetUseBestVisual(bool flag)
    %property=UseBestVisual, read, write
%endclass

%endif wxLUA_USE_wxApp

///////////////////////////////////////////////////////////////////////////////
// wxFrame

%if wxLUA_USE_wxFrame

%include "wx/frame.h"

%define wxDEFAULT_FRAME_STYLE
%define wxICONIZE
%define wxCAPTION
%define wxMINIMIZE
%define wxMINIMIZE_BOX
%define wxMAXIMIZE
%define wxMAXIMIZE_BOX
~<wxver25 %define wxCLOSE_BOX
%define wxSTAY_ON_TOP
%define wxSYSTEM_MENU
%define wxSIMPLE_BORDER
%define wxRESIZE_BORDER
%define wxFRAME_TOOL_WINDOW
~%wxver22 %define wxFRAME_NO_TASKBAR
%define wxFRAME_FLOAT_ON_PARENT
~%wxver22 %define wxFRAME_EX_CONTEXTHELP
~<wxver25 %define wxFRAME_SHAPED
%define wxFRAME_TOOL_WINDOW
~%wxver22 %define wxTHICK_FRAME

%class wxFrame, wxWindow
    %constructor wxFrameDefault()
    wxFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    void Centre(int direction = wxBOTH)
    // void Command(int id) depricated since 2.2 use ProcessCommand
    bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    virtual wxStatusBar* CreateStatusBar(int number = 1, long style = 0, wxWindowID id = -1, const wxString& name = "statusBar")
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER | wxTB_HORIZONTAL, wxWindowID id = -1, const wxString& name = "toolBar")
    wxPoint GetClientAreaOrigin() const
    wxMenuBar* GetMenuBar() const
    wxStatusBar* GetStatusBar() const
    wxString GetTitle() const
    wxToolBar* GetToolBar() const
    void Iconize(bool iconize)
    bool IsIconized() const
    bool IsMaximized() const
    void Maximize(bool maximize)
    void SetIcon(const wxIcon& icon)
    void SetMenuBar(wxMenuBar* menuBar)
    %property=MenuBar, read, write
    void SetStatusBar(wxStatusBar* statusBar)
    %property=StatusBar, read, write
    virtual void SetStatusText(const wxString& text, int number = 0)
    virtual void SetStatusWidths(int n, int *widths)
    void SetToolBar(wxToolBar* toolBar)
    %property=ToolBar, read, write
    virtual void SetTitle(const wxString& title)
    %property=Title, read, write
    %win bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMiniFrame

%if wxLUA_USE_wxMiniFrame

%include "wx/minifram.h"

%define wxTINY_CAPTION_HORIZ
%define wxTINY_CAPTION_VERT

%class wxMiniFrame, wxFrame
    %constructor wxMiniFrameDefault()
    wxMiniFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
%endclass

%endif wxLUA_USE_wxMiniFrame
%endif wxLUA_USE_wxFrame

///////////////////////////////////////////////////////////////////////////////
// wxStatusBar

%if wxLUA_USE_wxStatusBar

%include "wx/statusbr.h"

%define wxST_SIZEGRIP

%class wxStatusBar, wxWindow
    wxStatusBar(wxWindow* parent, wxWindowID id, long style = 0, const wxString& name = "statusBar")
    %constructor wxStatusBarDefault()
    bool         Create(wxWindow *parent, wxWindowID id, long style, const wxString& name = wxPanelNameStr)
    virtual bool GetFieldRect(int i, wxRect& rect) const
    int          GetFieldsCount() const
    %property=FieldsCount, read
    virtual wxString GetStatusText(int ir = 0) const
    // virtual void InitColours()
    // void OnSysColourChanged(wxSysColourChangedEvent& event)
    virtual void SetFieldsCount(int number = 1, int* widths = NULL)
    void     SetMinHeight(int height)
    virtual void SetStatusText(const wxString& text, int i = 0)
    virtual void SetStatusWidths(int n, int *widths)
%endclass

%endif wxLUA_USE_wxStatusBar

///////////////////////////////////////////////////////////////////////////////
// wxTooltip

%if wxLUA_USE_wxTooltip

%include "wx/tooltip.h"

%wxver22 %class %noclassinfo wxToolTip, wxObject
~%wxver22 %class wxToolTip, wxObject
    wxToolTip(const wxString &tip)
    static void Enable(bool flag)
    static void SetDelay(long milliseconds)
    void    SetTip(const wxString& tip)
    wxString GetTip()
    %property=Tip, read, write
    wxWindow *GetWindow()
    %property=Window, read
%endclass

%endif wxLUA_USE_wxTooltip

///////////////////////////////////////////////////////////////////////////////
// wxAcceleratorTable

%if wxLUA_USE_wxAcceleratorTable

%include "wx/accel.h"

%class %delete wxAcceleratorTable, wxObject
    %define %object wxNullAcceleratorTable

    wxAcceleratorTable(int n, wxAcceleratorEntry* entries)
    bool Ok() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxAcceleratorEntry

%builtin wxAcceleratorEntry* wxGetAccelFromString(const wxString& label)

%enum
    wxACCEL_NORMAL
    wxACCEL_ALT
    wxACCEL_CTRL
    wxACCEL_SHIFT
%end

%class %delete %noclassinfo %encapsulate wxAcceleratorEntry
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0)
    int GetCommand()
    %property=Command, read
    int GetFlags()
    %property=Flags, read
    int GetKeyCode()
    %property=KeyCode, read
    void Set(int flags, int keyCode, int Cmd)
%endclass

%endif wxLUA_USE_wxAcceleratorTable

///////////////////////////////////////////////////////////////////////////////
// wxTaskBarIcon

%if wxLUA_USE_wxTaskBarIcon

%include "wx/taskbar.h"

%gtk %skip
%mac %skip
%class wxTaskBarIcon, wxEvtHandler
    wxTaskBarIcon()
    bool IsIconInstalled()
    %wxver24|%wxcompat24 bool IsOK()
    bool RemoveIcon()
    bool SetIcon(const wxIcon& icon, const wxString& tooltip)
%endclass

%gtk %skip
%mac %skip
%class %noclassinfo wxTaskBarIconEvent, wxEvent
    %define %event wxEVT_TASKBAR_MOVE
    %define %event wxEVT_TASKBAR_LEFT_DOWN
    %define %event wxEVT_TASKBAR_LEFT_UP
    %define %event wxEVT_TASKBAR_RIGHT_DOWN
    %define %event wxEVT_TASKBAR_RIGHT_UP
    %define %event wxEVT_TASKBAR_LEFT_DCLICK
    %define %event wxEVT_TASKBAR_RIGHT_DCLICK
%endclass

%endif wxLUA_USE_wxTaskBarIcon

///////////////////////////////////////////////////////////////////////////////
// wxSystemSettings

%if wxLUA_USE_wxSystemSettings

%include "wx/settings.h"

~%wxver22 %enum wxSystemScreenType
~%wxver22    wxSYS_SCREEN_NONE
~%wxver22     wxSYS_SCREEN_TINY
~%wxver22     wxSYS_SCREEN_PDA
~%wxver22     wxSYS_SCREEN_SMALL
~%wxver22    wxSYS_SCREEN_DESKTOP
~%wxver22 %end

%enum wxSystemMetric
    wxSYS_MOUSE_BUTTONS
    wxSYS_BORDER_X
    wxSYS_BORDER_Y
    wxSYS_CURSOR_X
    wxSYS_CURSOR_Y
    wxSYS_DCLICK_X
    wxSYS_DCLICK_Y
    wxSYS_DRAG_X
    wxSYS_DRAG_Y
    wxSYS_EDGE_X
    wxSYS_EDGE_Y
    wxSYS_HSCROLL_ARROW_X
    wxSYS_HSCROLL_ARROW_Y
    wxSYS_HTHUMB_X
    wxSYS_ICON_X
    wxSYS_ICON_Y
    wxSYS_ICONSPACING_X
    wxSYS_ICONSPACING_Y
    wxSYS_WINDOWMIN_X
    wxSYS_WINDOWMIN_Y
    wxSYS_SCREEN_X
    wxSYS_SCREEN_Y
    wxSYS_FRAMESIZE_X
    wxSYS_FRAMESIZE_Y
    wxSYS_SMALLICON_X
    wxSYS_SMALLICON_Y
    wxSYS_HSCROLL_Y
    wxSYS_VSCROLL_X
    wxSYS_VSCROLL_ARROW_X
    wxSYS_VSCROLL_ARROW_Y
    wxSYS_VTHUMB_Y
    wxSYS_CAPTION_Y
    wxSYS_MENU_Y
    wxSYS_NETWORK_PRESENT
    wxSYS_PENWINDOWS_PRESENT
    wxSYS_SHOW_SOUNDS
    wxSYS_SWAP_BUTTONS
%end

~%wxver22 %enum wxSystemFeature
~%wxver22     wxSYS_CAN_DRAW_FRAME_DECORATIONS
~%wxver22     wxSYS_CAN_ICONIZE_FRAME
~%wxver22 %end

%enum wxSystemColour
    wxSYS_COLOUR_SCROLLBAR
    wxSYS_COLOUR_BACKGROUND
    wxSYS_COLOUR_DESKTOP
    wxSYS_COLOUR_ACTIVECAPTION
    wxSYS_COLOUR_INACTIVECAPTION
    wxSYS_COLOUR_MENU
    wxSYS_COLOUR_WINDOW
    wxSYS_COLOUR_WINDOWFRAME
    wxSYS_COLOUR_MENUTEXT
    wxSYS_COLOUR_WINDOWTEXT
    wxSYS_COLOUR_CAPTIONTEXT
    wxSYS_COLOUR_ACTIVEBORDER
    wxSYS_COLOUR_INACTIVEBORDER
    wxSYS_COLOUR_APPWORKSPACE
    wxSYS_COLOUR_HIGHLIGHT
    wxSYS_COLOUR_HIGHLIGHTTEXT
    wxSYS_COLOUR_BTNFACE
    wxSYS_COLOUR_3DFACE
    wxSYS_COLOUR_BTNSHADOW
    wxSYS_COLOUR_3DSHADOW
    wxSYS_COLOUR_GRAYTEXT
    wxSYS_COLOUR_BTNTEXT
    wxSYS_COLOUR_INACTIVECAPTIONTEXT
    wxSYS_COLOUR_BTNHIGHLIGHT
    wxSYS_COLOUR_BTNHILIGHT
    wxSYS_COLOUR_3DHIGHLIGHT
    wxSYS_COLOUR_3DHILIGHT
    wxSYS_COLOUR_3DDKSHADOW
    wxSYS_COLOUR_3DLIGHT
    wxSYS_COLOUR_INFOTEXT
    wxSYS_COLOUR_INFOBK
    wxSYS_COLOUR_LISTBOX
~%wxver22     wxSYS_COLOUR_HOTLIGHT
~%wxver22     wxSYS_COLOUR_GRADIENTACTIVECAPTION
~%wxver22     wxSYS_COLOUR_GRADIENTINACTIVECAPTION
~%wxver22     wxSYS_COLOUR_MENUHILIGHT
~%wxver22     wxSYS_COLOUR_MENUBAR
~%wxver22     wxSYS_COLOUR_MAX
%endenum

%enum wxSystemFont
    wxSYS_OEM_FIXED_FONT
    wxSYS_ANSI_FIXED_FONT
    wxSYS_ANSI_VAR_FONT
    wxSYS_SYSTEM_FONT
    wxSYS_DEVICE_DEFAULT_FONT
    wxSYS_DEFAULT_PALETTE
    wxSYS_SYSTEM_FIXED_FONT
    wxSYS_DEFAULT_GUI_FONT
%endenum

%class %noclassinfo %encapsulate wxSystemSettings
    %wxver22 static wxColour GetSystemColour(int index)
    %wxver22 static wxFont   GetSystemFont(int index)
    %wxver22 static int GetSystemMetric(int index)
    ~%wxver22 static wxColour GetColour(wxSystemColour index)
    ~%wxver22 static wxFont   GetFont(wxSystemFont index)
    ~%wxver22 static int GetMetric(wxSystemMetric index)
%endclass

%endif wxLUA_USE_wxSystemSettings

///////////////////////////////////////////////////////////////////////////////
// wxSystemOptions

%include "wx/sysopt.h"

%class %delete %noclassinfo wxSystemOptions, wxObject
    wxSystemOptions()
    static wxString GetOption(const wxString& name) const
    static int GetOptionInt(const wxString& name) const
    static bool HasOption(const wxString& name) const
    static void SetOption(const wxString& name, const wxString& value)
    %rename SetOptionInt static void SetOption(const wxString& name, int value)
%endclass

