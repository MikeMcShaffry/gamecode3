/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxEvent and other generic event classes and types
//              events specific to a single control are with that control
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// wxEvtHandler - event handling is not accessible through wxLua

%class wxEvtHandler, wxObject
    wxEvtHandler()
    void ConnectEvent()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxEvent

%include "wx/event.h"

%class wxEvent, wxObject
    //wxEvent(int id = 0)
    wxObject* GetEventObject()
    WXTYPE GetEventType()
    int GetId()
    bool GetSkipped()
    long GetTimestamp()
    void SetEventObject(wxObject* object)
    %property=EventObject, read, write
    void SetEventType(WXTYPE typ)
    %property=EventType, read, write
    void SetId(int id)
    %property=Id, read, write
    void SetTimestamp(long timeStamp)
    %property=Timestamp, read, write
    void Skip(bool skip = TRUE)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCommandEvent

%include "wx/event.h"
~<wxver23 %include "wx/tglbtn.h"  // for wxEVT_COMMAND_TOGGLEBUTTON_CLICKED

%class %delete wxCommandEvent, wxEvent

    %define %event wxEVT_COMMAND_ENTER
    %define %event wxEVT_COMMAND_KILL_FOCUS
    %define %event wxEVT_COMMAND_LEFT_CLICK
    %define %event wxEVT_COMMAND_LEFT_DCLICK
    %define %event wxEVT_COMMAND_RIGHT_CLICK
    %define %event wxEVT_COMMAND_RIGHT_DCLICK
    %define %event wxEVT_COMMAND_SCROLLBAR_UPDATED
    %define %event wxEVT_COMMAND_SET_FOCUS
    //%define %event wxEVT_COMMAND_VLBOX_SELECTED // unused?

    %define %event wxEVT_COMMAND_MENU_SELECTED

    %define %event wxEVT_COMMAND_TOOL_CLICKED
    %define %event wxEVT_COMMAND_TOOL_ENTER
    %define %event wxEVT_COMMAND_TOOL_RCLICKED

    %define %event wxEVT_COMMAND_TEXT_ENTER
    %define %event wxEVT_COMMAND_TEXT_UPDATED
    ~%wxver22 %define %event wxEVT_COMMAND_TEXT_URL
    ~%wxver22 %define %event wxEVT_COMMAND_TEXT_MAXLEN

    %define %event wxEVT_COMMAND_SPINCTRL_UPDATED
    %define %event wxEVT_COMMAND_SLIDER_UPDATED
    %define %event wxEVT_COMMAND_RADIOBUTTON_SELECTED
    %define %event wxEVT_COMMAND_RADIOBOX_SELECTED
    %define %event wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
    %define %event wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
    %define %event wxEVT_COMMAND_LISTBOX_SELECTED
    %define %event wxEVT_COMMAND_COMBOBOX_SELECTED
    %define %event wxEVT_COMMAND_CHOICE_SELECTED
    %define %event wxEVT_COMMAND_CHECKBOX_CLICKED
    %define %event wxEVT_COMMAND_BUTTON_CLICKED
    ~<wxver23 %define %event wxEVT_COMMAND_TOGGLEBUTTON_CLICKED

    wxCommandEvent(WXTYPE commandEventType = 0, int id = 0)
    wxObject* GetClientData()
    long GetExtraLong()
    int GetInt()
    int GetSelection()
    %property=Selection, read
    wxString GetString()
    bool IsChecked() const
    bool IsSelection()
    void SetClientData(wxObject *clientData)
    %property=ClientData, read, write
    void SetExtraLong(int extraLong)
    %property=ExtraLong, read, write
    void SetInt(int intCommand)
    %property=Int, read, write
    void SetString(const wxString &string)
    %property=String, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxNotifyEvent

%include "wx/event.h"

%class %delete wxNotifyEvent, wxCommandEvent
    wxNotifyEvent(wxEventType eventType = wxEVT_NULL, int id = 0)
    bool IsAllowed() const
    void Veto()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxActivateEvent

%include "wx/event.h"

%class %delete wxActivateEvent, wxEvent
    %define %event wxEVT_ACTIVATE
    %define %event wxEVT_ACTIVATE_APP

    wxActivateEvent(WXTYPE eventType = 0, bool active = TRUE, int id = 0)
    bool GetActive() const
    %property=Active, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCloseEvent

%include "wx/event.h"

%class %delete wxCloseEvent, wxEvent
    %define %event wxEVT_CLOSE_WINDOW
    %define %event wxEVT_QUERY_END_SESSION
    %define %event wxEVT_END_SESSION

    wxCloseEvent(WXTYPE commandEventType = 0, int id = 0)
    bool CanVeto()
    bool GetLoggingOff() const
    void SetCanVeto(bool canVeto)
    void SetLoggingOff(bool loggingOff) const
    void Veto(bool veto = TRUE)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDialUpEvent - TODO - the rest of wxDialUp is missing, anyone care?

%include "wx/dialup.h"
%mac %skip
%class %noclassinfo wxDialUpEvent, wxCommandEvent
    %define %event wxEVT_DIALUP_CONNECTED
    %define %event wxEVT_DIALUP_DISCONNECTED

    wxDialUpEvent(bool isConnected, bool isOwnEvent)
    bool IsConnectedEvent() const
    bool IsOwnEvent() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxEraseEvent

%include "wx/event.h"

%class %delete wxEraseEvent, wxEvent
    %define %event wxEVT_ERASE_BACKGROUND

    wxEraseEvent(int id = 0, wxDC* dc = NULL)
    wxDC* m_dc
    wxDC* GetDC() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFocusEvent

%include "wx/event.h"

%class %delete wxFocusEvent, wxEvent
    %define %event wxEVT_SET_FOCUS
    %define %event wxEVT_KILL_FOCUS

    wxFocusEvent(WXTYPE eventType = 0, int id = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxKeyEvent

%define WXK_ADD
%define WXK_ALT
%define WXK_BACK
%define WXK_CANCEL
%define WXK_CAPITAL
%define WXK_CLEAR
%define WXK_CONTROL
%define WXK_DECIMAL
%define WXK_DELETE
%define WXK_DIVIDE
%define WXK_DOWN
%define WXK_END
%define WXK_ESCAPE
%define WXK_EXECUTE
%define WXK_F1
%define WXK_F10
%define WXK_F11
%define WXK_F12
%define WXK_F13
%define WXK_F14
%define WXK_F15
%define WXK_F16
%define WXK_F17
%define WXK_F18
%define WXK_F19
%define WXK_F2
%define WXK_F20
%define WXK_F21
%define WXK_F22
%define WXK_F23
%define WXK_F24
%define WXK_F3
%define WXK_F4
%define WXK_F5
%define WXK_F6
%define WXK_F7
%define WXK_F8
%define WXK_F9
%define WXK_HELP
%define WXK_HOME
%define WXK_INSERT
%define WXK_LBUTTON
%define WXK_LEFT
%define WXK_MBUTTON
%define WXK_MENU
%define WXK_MULTIPLY
%define WXK_NEXT
%define WXK_NUMLOCK
%define WXK_NUMPAD_ADD
%define WXK_NUMPAD_BEGIN
%define WXK_NUMPAD_DECIMAL
%define WXK_NUMPAD_DELETE
%define WXK_NUMPAD_DIVIDE
%define WXK_NUMPAD_DOWN
%define WXK_NUMPAD_END
%define WXK_NUMPAD_ENTER
%define WXK_NUMPAD_EQUAL
%define WXK_NUMPAD_F1
%define WXK_NUMPAD_F2
%define WXK_NUMPAD_F3
%define WXK_NUMPAD_F4
%define WXK_NUMPAD_HOME
%define WXK_NUMPAD_INSERT
%define WXK_NUMPAD_LEFT
%define WXK_NUMPAD_MULTIPLY
%define WXK_NUMPAD_NEXT
%define WXK_NUMPAD_PAGEDOWN
%define WXK_NUMPAD_PAGEUP
%define WXK_NUMPAD_PRIOR
%define WXK_NUMPAD_RIGHT
%define WXK_NUMPAD_SEPARATOR
%define WXK_NUMPAD_SPACE
%define WXK_NUMPAD_SUBTRACT
%define WXK_NUMPAD_TAB
%define WXK_NUMPAD_UP
%define WXK_NUMPAD0
%define WXK_NUMPAD1
%define WXK_NUMPAD2
%define WXK_NUMPAD3
%define WXK_NUMPAD4
%define WXK_NUMPAD5
%define WXK_NUMPAD6
%define WXK_NUMPAD7
%define WXK_NUMPAD8
%define WXK_NUMPAD9
%define WXK_PAGEDOWN
%define WXK_PAGEUP
%define WXK_PAUSE
%define WXK_PRINT
%define WXK_PRIOR
%define WXK_RBUTTON
%define WXK_RETURN
%define WXK_RIGHT
%define WXK_SCROLL
%define WXK_SELECT
%define WXK_SEPARATOR
%define WXK_SHIFT
%define WXK_SNAPSHOT
%define WXK_SPACE
%define WXK_START
%define WXK_SUBTRACT
%define WXK_TAB
%define WXK_UP

%include "wx/event.h"

%class %delete wxKeyEvent, wxEvent
    %define %event wxEVT_KEY_DOWN
    %define %event wxEVT_KEY_UP
    %define %event wxEVT_CHAR

    wxKeyEvent(WXTYPE keyEventType)
    bool AltDown() const
    bool ControlDown() const
    int GetKeyCode() const
    long GetX()
    long GetY() const
    bool MetaDown() const
    wxPoint GetPosition() const
    void GetPositionXY(long *x, long *y) const
    bool HasModifiers() const
    bool ShiftDown() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxIdleEvent

%include "wx/event.h"

%class %delete wxIdleEvent, wxEvent
    %define %event wxEVT_IDLE

    wxIdleEvent()
    void RequestMore(bool needMore = TRUE)
    bool MoreRequested() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxInitDialogEvent - for dialogs and panels

%include "wx/event.h"

%class %delete wxInitDialogEvent, wxEvent
    %define %event wxEVT_INIT_DIALOG

    wxInitDialogEvent(int id = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxJoystickEvent

%include "wx/event.h"

%class %delete wxJoystickEvent, wxEvent
    %define %event wxEVT_JOY_BUTTON_DOWN
    %define %event wxEVT_JOY_BUTTON_UP
    %define %event wxEVT_JOY_MOVE
    %define %event wxEVT_JOY_ZMOVE

    wxJoystickEvent(WXTYPE eventType = 0, int state = 0, int joystick = wxJOYSTICK1, int change = 0)
    bool ButtonDown(int button = wxJOY_BUTTON_ANY) const
    bool ButtonIsDown(int button = wxJOY_BUTTON_ANY) const
    bool ButtonUp(int button = wxJOY_BUTTON_ANY) const
    int GetButtonChange() const
    int GetButtonState() const
    int GetJoystick() const
    wxPoint GetPosition() const
    int GetZPosition() const
    bool IsButton() const
    bool IsMove() const
    bool IsZMove() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMouseEvent

%include "wx/event.h"

%class wxMouseEvent, wxEvent
    %define %event wxEVT_ENTER_WINDOW
    %define %event wxEVT_LEAVE_WINDOW
    %define %event wxEVT_LEFT_DCLICK
    %define %event wxEVT_LEFT_DOWN
    %define %event wxEVT_LEFT_UP
    %define %event wxEVT_MIDDLE_DCLICK
    %define %event wxEVT_MIDDLE_DOWN
    %define %event wxEVT_MIDDLE_UP
    %define %event wxEVT_MOTION
    %define %event wxEVT_NC_ENTER_WINDOW
    %define %event wxEVT_NC_LEAVE_WINDOW
    %define %event wxEVT_NC_LEFT_DCLICK
    %define %event wxEVT_NC_LEFT_DOWN
    %define %event wxEVT_NC_LEFT_UP
    %define %event wxEVT_NC_MIDDLE_DCLICK
    %define %event wxEVT_NC_MIDDLE_DOWN
    %define %event wxEVT_NC_MIDDLE_UP
    %define %event wxEVT_NC_MOTION
    %define %event wxEVT_NC_RIGHT_DCLICK
    %define %event wxEVT_NC_RIGHT_DOWN
    %define %event wxEVT_NC_RIGHT_UP
    %define %event wxEVT_RIGHT_DCLICK
    %define %event wxEVT_RIGHT_DOWN
    %define %event wxEVT_RIGHT_UP

    bool AltDown()
    bool Button(int button)
    bool ButtonDClick(int but = -1)
    bool ButtonDown(int but = -1)
    bool ButtonUp(int but = -1)
    bool ControlDown()
    bool Dragging()
    bool Entering()
    wxPoint GetPosition() const
    %property=Position, read
    void GetPositionXY(wxCoord* x, wxCoord* y) const
    wxPoint GetLogicalPosition(const wxDC& dc) const
    %property=LogicalPosition, read
    long GetX() const
    %property=X, read
    long GetY()
    %property=Y, read
    bool IsButton() const
    bool Leaving() const
    bool LeftDClick() const
    bool LeftDown() const
    bool LeftIsDown() const
    bool LeftUp() const
    bool MetaDown() const
    bool MiddleDClick() const
    bool MiddleDown() const
    bool MiddleIsDown() const
    bool MiddleUp() const
    bool Moving() const
    bool RightDClick() const
    bool RightDown() const
    bool RightIsDown() const
    bool RightUp() const
    bool ShiftDown() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMoveEvent

%include "wx/event.h"

%class %delete wxMoveEvent, wxEvent
    %define %event wxEVT_MOVE
    // %define %event wxEVT_MOVING  // in 2.5

    wxMoveEvent(const wxPoint& pt, int id = 0)
    wxPoint GetPosition() const
    %property=Position, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPaintEvent

%include "wx/event.h"

%class %delete wxPaintEvent, wxEvent
    %define %event wxEVT_PAINT

    wxPaintEvent(int id = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxProcessEvent

%include "wx/process.h"

%class %delete wxProcessEvent, wxEvent
    %define %event wxEVT_END_PROCESS

    wxProcessEvent(int id = 0, int pid = 0)
    int GetPid() const
    %property=Pid, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxScrollEvent - for independent scrollbars and sliders

%include "wx/event.h"

%class %delete wxScrollEvent, wxCommandEvent
    %define %event wxEVT_SCROLL_TOP
    %define %event wxEVT_SCROLL_BOTTOM
    %define %event wxEVT_SCROLL_LINEUP
    %define %event wxEVT_SCROLL_LINEDOWN
    %define %event wxEVT_SCROLL_PAGEUP
    %define %event wxEVT_SCROLL_PAGEDOWN
    %define %event wxEVT_SCROLL_THUMBTRACK
    %define %event wxEVT_SCROLL_THUMBRELEASE
    ~%wxver22 %define %event wxEVT_SCROLL_ENDSCROLL

    wxScrollEvent(WXTYPE commandType = 0, int id = 0, int pos = 0, int orientation = 0)
    int GetOrientation() const
    %property=Orientation, read
    int GetPosition() const
    %property=Position, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxScrollWinEvent - for wxScrolledWindows only

%include "wx/event.h"

%class %delete wxScrollWinEvent, wxEvent
    %define %event wxEVT_SCROLLWIN_BOTTOM
    %define %event wxEVT_SCROLLWIN_LINEDOWN
    %define %event wxEVT_SCROLLWIN_LINEUP
    %define %event wxEVT_SCROLLWIN_PAGEDOWN
    %define %event wxEVT_SCROLLWIN_PAGEUP
    %define %event wxEVT_SCROLLWIN_THUMBRELEASE
    %define %event wxEVT_SCROLLWIN_THUMBTRACK
    %define %event wxEVT_SCROLLWIN_TOP

    int GetOrientation() const
    %property=Orientation, read
    int GetPosition() const
    %property=Position, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSizeEvent

%include "wx/event.h"

%class %delete wxSizeEvent, wxEvent
    %define %event wxEVT_SIZE
    // %define %event wxEVT_SIZING  // in 2.5

    wxSizeEvent(const wxSize& sz, int id = 0)
    wxSize GetSize() const
    %property=Size, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSysColourChangedEvent

%include "wx/event.h"

%class %delete wxSysColourChangedEvent, wxEvent
    %define %event wxEVT_SYS_COLOUR_CHANGED

    wxSysColourChangedEvent()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTimerEvent

%include "wx/timer.h"

%class wxTimerEvent, wxObject
    %define %event wxEVT_TIMER

    int GetInterval() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxUpdateUIEvent

%include "wx/event.h"

%class %delete wxUpdateUIEvent, wxEvent
    %define %event wxEVT_UPDATE_UI

    wxUpdateUIEvent(wxWindowID commandId = 0)
    void Check(bool check)
    void Enable(bool enable)
    bool GetChecked() const
    %property=Checked, read
    bool GetEnabled() const
    %property=Enabled, read
    bool GetSetChecked() const
    %property=SetChecked, read
    bool GetSetEnabled() const
    %property=SetEnabled, read
    bool GetSetText() const
    %property=SetText, read
    wxString GetText() const
    void SetText(const wxString& text)
    %property=Text, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHelpEvent

%wxver22 %skip
%class %delete wxHelpEvent, wxCommandEvent
    %define %event wxEVT_HELP
    %define %event wxEVT_DETAILED_HELP
    %define %event wxEVT_CONTEXT_MENU

    wxHelpEvent(wxEventType type = wxEVT_NULL, wxWindowID id = 0, const wxPoint& pt = wxDefaultPosition)
    const wxPoint& GetPosition()
    void SetPosition(const wxPoint& pos)
    %property=Position, read, write
    const wxString& GetLink()
    void SetLink(const wxString& link)
    %property=Link, read, write
    const wxString& GetTarget()
    void SetTarget(const wxString& target)
    %property=Target, read, write
%endclass
