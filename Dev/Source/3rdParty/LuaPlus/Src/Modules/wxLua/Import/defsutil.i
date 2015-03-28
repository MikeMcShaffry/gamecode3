/////////////////////////////////////////////////////////////////////////////
// Purpose:     enums, defines from wx/defs.h and other places
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%include "wx/defs.h"
%include "wx/utils.h"

// This list of global functions are taken from the wxWindow's manual

///////////////////////////////////////////////////////////////////////////////
// Application initialization and termination

%builtin bool wxHandleFatalExceptions(bool doIt = TRUE)
%builtin void wxInitAllImageHandlers()
%builtin bool wxSafeYield(wxWindow* win = NULL)
%builtin bool wxYield()
%builtin void wxWakeUpIdle()

///////////////////////////////////////////////////////////////////////////////
// Process control functions

%builtin long wxExecute(const wxString& command, bool sync = FALSE, wxProcess *callback = NULL)
%builtin void wxExit()
~%wxver22 %builtin unsigned long wxGetProcessId()
%builtin bool wxShell(const wxString& command = "")

///////////////////////////////////////////////////////////////////////////////
// File functions - see file.i

///////////////////////////////////////////////////////////////////////////////
// Network, user, and OS functions

%builtin long wxGetFreeMemory()
%builtin wxString wxGetFullHostName()
//%builtin bool wxGetEmailAddress(const wxString& buf, int sz)
%builtin wxString wxGetHomeDir()
%builtin wxString wxGetHostName()
%builtin wxString wxGetUserId()
%builtin wxString wxGetOsDescription()
%builtin int wxGetOsVersion(int *major = NULL, int *minor = NULL)
%builtin wxString wxGetUserHome(const wxString& user = "")
%builtin wxString wxGetUserName()

///////////////////////////////////////////////////////////////////////////////
// Dialog functions - others see dialogs.i

%builtin void wxBell()
%builtin void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR)
%builtin void wxEndBusyCursor()
%builtin bool wxIsBusy()

///////////////////////////////////////////////////////////////////////////////
// GDI functions

~%wxver22 %builtin void wxClientDisplayRect(int *x, int *y, int *width, int *height)
%builtin bool wxColourDisplay()
%builtin int wxDisplayDepth()
%builtin void wxDisplaySize(int *width, int *height)
~%wxver22 %builtin void wxDisplaySizeMM(int *width, int *height)
%builtin void wxSetCursor(const wxCursor &cursor)
// %builtin wxIconOrCursor wxDROP_ICON(wxString name)

///////////////////////////////////////////////////////////////////////////////
// Miscellaneous functions

%builtin long wxNewId()
%builtin void wxRegisterId(long id)
%builtin void wxEnableTopLevelWindows(bool enable = TRUE)
%builtin int wxFindMenuItemId(wxFrame *frame, const wxString& menuString, const wxString& itemString)
%builtin wxWindow* wxFindWindowByLabel(const wxString& label, wxWindow *parent=NULL)
%builtin wxWindow* wxFindWindowByName(const wxString& name, wxWindow *parent=NULL)
~%wxver22 %builtin wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
~%wxver22 %builtin wxWindow* wxFindWindowAtPointer(wxPoint& pt)
%win %builtin wxWindow* wxGetActiveWindow()
// X only %builtin wxString wxGetDisplayName()
// X only %builtin void wxSetDisplayName(const wxString& displayName)
~%wxver22 %builtin wxPoint wxGetMousePosition()
// %builtin bool wxGetResource(const wxString& section, const wxString& entry, const wxString& *value, const wxString& file = "")
// %builtin bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file = "")
~%wxver22 %builtin wxWindow * wxGetTopLevelParent(wxWindow *win)
//%win %builtin wxString wxLoadUserResource(const wxString& resourceName, const wxString& resourceType="TEXT")
%builtin void wxPostEvent(wxEvtHandler *dest, wxEvent& event)

///////////////////////////////////////////////////////////////////////////////
// Time functions - see datatime.i



%if wxLUA_USE_wxID_XXX

%define wxID_ABOUT
%define wxID_APPLY
%define wxID_BACKWARD
%define wxID_CANCEL
%define wxID_CLEAR
%define wxID_CLOSE
~%wxver22 %define wxID_CONTEXT_HELP
%define wxID_COPY
%define wxID_CUT
%define wxID_DEFAULT
%define wxID_DUPLICATE
%define wxID_EXIT
%define wxID_FILE1
%define wxID_FILE2
%define wxID_FILE3
%define wxID_FILE4
%define wxID_FILE5
%define wxID_FILE6
%define wxID_FILE7
%define wxID_FILE8
%define wxID_FILE9
%define wxID_FILEDLGG
%define wxID_FIND
%define wxID_FORWARD
%define wxID_HELP
%define wxID_HELP_COMMANDS
%define wxID_HELP_CONTENTS
%define wxID_HELP_CONTEXT
%define wxID_HELP_PROCEDURES
%define wxID_HIGHEST
%define wxID_LOWEST
%define wxID_MORE
%define wxID_NEW
%define wxID_NO
%define wxID_OK
%define wxID_OPEN
%define wxID_PASTE
%define wxID_PREVIEW
%define wxID_PRINT
%define wxID_PRINT_SETUP
%define wxID_REDO
%define wxID_RESET
%define wxID_REVERT
%define wxID_SAVE
%define wxID_SAVEAS
%define wxID_SELECTALL
%define wxID_SEPARATOR
%define wxID_SETUP
%define wxID_STATIC
%define wxID_UNDO
%define wxID_YES

%endif wxLUA_USE_wxID_XXX

%enum wxDirection
    wxLEFT
    wxRIGHT
    wxUP
    wxDOWN
    wxTOP
    wxBOTTOM
    wxNORTH
    wxSOUTH
    wxWEST
    wxEAST
    wxALL
%end

%enum wxAlignment
    wxALIGN_NOT
    wxALIGN_CENTER_HORIZONTAL
    wxALIGN_CENTRE_HORIZONTAL
    wxALIGN_LEFT
    wxALIGN_TOP
    wxALIGN_RIGHT
    wxALIGN_BOTTOM
    wxALIGN_CENTER_VERTICAL
    wxALIGN_CENTRE_VERTICAL
    wxALIGN_CENTER
    wxALIGN_CENTRE
~%wxver22    wxALIGN_MASK
%end

%enum wxStretch
    wxSTRETCH_NOT
    wxSHRINK
    wxGROW
    wxEXPAND
    wxSHAPED
~%wxver22    wxADJUST_MINSIZE
~%wxver22     wxTILE
%end

// border flags: the values are chosen for backwards compatibility
~%wxver22 %enum wxBorder
~%wxver22     wxBORDER_DEFAULT
~%wxver22     wxBORDER_NONE
~%wxver22     wxBORDER_STATIC
~%wxver22     wxBORDER_SIMPLE
~%wxver22     wxBORDER_RAISED
~%wxver22     wxBORDER_SUNKEN
~%wxver22     wxBORDER_DOUBLE
~%wxver22     wxBORDER_MASK
~%wxver22 %end

%include "wx/txtstrm.h"
%enum wxEOL
    wxEOL_NATIVE
    wxEOL_UNIX
    wxEOL_MAC
    wxEOL_DOS
%end
