/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxDialog and all dialog classes and functions
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%define wxICON_ASTERISK
%define wxICON_ERROR
%define wxICON_EXCLAMATION
%define wxICON_HAND
%define wxICON_INFORMATION
%define wxICON_MASK
%define wxICON_QUESTION
%define wxICON_STOP
%define wxICON_WARNING

%enum    // for wxFileSelector and wxFileDialog
    wxOPEN
    wxSAVE
    wxOVERWRITE_PROMPT
    wxHIDE_READONLY
    wxFILE_MUST_EXIST
    wxMULTIPLE
~%wxver22    wxCHANGE_DIR
%end

%builtin wxString wxFileSelector(const wxString& message, const wxString& default_path = "", const wxString& default_filename = "", const wxString& default_extension = "", const wxString& wildcard = "*.*", int flags = 0, wxWindow *parent = NULL, int x = -1, int y = -1)
%builtin wxColour wxGetColourFromUser(wxWindow *parent, const wxColour& colInit)
%builtin long wxGetNumberFromUser( const wxString& message, const wxString& prompt, const wxString& caption, long value, long min = 0, long max = 100, wxWindow *parent = NULL, const wxPoint& pos = wxDefaultPosition)
%builtin wxString wxGetPasswordFromUser(const wxString& message, const wxString& caption = "Input text", const wxString& default_value = "", wxWindow *parent = NULL)
%builtin wxString wxGetTextFromUser(const wxString& message, const wxString& caption = "Input text", const wxString& default_value = "", wxWindow *parent = NULL, int x = -1, int y = -1, bool centre = TRUE)
//%builtin int wxGetMultipleChoices(const wxString& message, const wxString& caption, int n, const wxString& choices[], int nsel, int *selection, wxWindow *parent = NULL, int x = -1, int y = -1, bool centre = TRUE, int width=150, int height=200)
//%builtin wxString wxGetSingleChoiceData(const wxString& message, const wxString& caption, int n, const wxString& choices[], wxWindow *parent = NULL, int x = -1, int y = -1, bool centre = TRUE, int width=150, int height=200)
//%builtin int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption, int n, const wxString& choices[], wxWindow *parent = NULL, int x = -1, int y = -1, bool centre = TRUE, int width=150, int height=200)
%builtin int wxMessageBox(const wxString& message, const wxString& caption = "Message", int style = wxOK | wxCENTRE, wxWindow *parent = NULL, int x = -1, int y = -1)
// bool wxShowTip(wxWindow *parent, wxTipProvider *tipProvider, bool showAtStartup = TRUE)

///////////////////////////////////////////////////////////////////////////////
// wxDialog

%if wxLUA_USE_wxDialog

%include "wx/dialog.h"

%define wxDIALOG_MODAL
%define wxDIALOG_MODELESS

%define wxCAPTION
%define wxDEFAULT_DIALOG_STYLE
%define wxRESIZE_BORDER
%define wxSYSTEM_MENU
%define wxTHICK_FRAME
%define wxSTAY_ON_TOP
%define wxNO_3D
~%wxver22 %define wxDIALOG_NO_PARENT
~%wxver22 %define wxDIALOG_EX_CONTEXTHELP

%class wxDialog, wxPanel
    wxDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "dialogBox")
    %constructor wxEmptyDialog()
    void Centre(int direction = wxBOTH)
    bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "dialogBox")
    void EndModal(int retCode)
    int GetReturnCode()
    wxString GetTitle() const
    void Iconize(const bool iconize)
    bool IsIconized() const
    bool IsModal() const
    void SetModal(const bool flag)
    void SetReturnCode(int retCode)
    %property=ReturnCode, read, write
    void SetTitle(const wxString& title)
    %property=Title, read, write
    bool Show(const bool show)
    int ShowModal()
    ~%wxver22 void SetIcon(const wxIcon& icon)
%endclass

%endif wxLUA_USE_wxDialog

///////////////////////////////////////////////////////////////////////////////
// wxColourDialog

%if wxLUA_USE_wxColourDialog

%include "wx/colordlg.h"

%class wxColourDialog, wxDialog
    wxColourDialog(wxWindow* parent, wxColourData* data = NULL)
    bool Create(wxWindow* parent, wxColourData* data = NULL)
    wxColourData& GetColourData()
    %property=ColourData, read
    int ShowModal()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxColourData

%include "wx/cmndata.h"

%class %delete wxColourData, wxObject
    wxColourData()
    bool GetChooseFull() const
    wxColour& GetColour() const
    wxColour GetCustomColour(int i) const
    void SetChooseFull(bool flag)
    void SetColour(wxColour &colour)
    %property=Colour, read, write
    %property=ChooseFull, read, write
    void SetCustomColour(int i, wxColour &colour)
%endclass

%endif wxLUA_USE_wxColourDialog

///////////////////////////////////////////////////////////////////////////////
// wxFileDialog

%if wxLUA_USE_wxFileDialog

%include "wx/filedlg.h"

%class wxFileDialog, wxDialog
    wxFileDialog(wxWindow* parent, const wxString& message = "Choose a file", const wxString& defaultDir = "", const wxString& defaultFile = "", const wxString& wildcard = "*.*", long style = 0, const wxPoint& pos = wxDefaultPosition)
    wxString GetDirectory() const
    wxString GetFilename() const
    void GetFilenames(wxArrayString& filenames) const
    int GetFilterIndex() const
    wxString GetMessage() const
    wxString GetPath() const
    void GetPaths(wxArrayString& paths) const
    long GetStyle() const
    wxString GetWildcard() const
    void SetDirectory(const wxString& directory)
    %property=Directory, read, write
    void SetFilename(const wxString& setfilename)
    %property=Filename, read, write
    void SetFilterIndex(int filterIndex)
    %property=FilterIndex, read, write
    void SetMessage(const wxString& message)
    %property=Message, read, write
    void SetPath(const wxString& path)
    %property=Path, read, write
    void SetStyle(long style)
    %property=Style, read, write
    void SetWildcard(const wxString& wildCard)
    %property=Wildcard, read, write
    int ShowModal()
%endclass

%endif wxLUA_USE_wxFileDialog

///////////////////////////////////////////////////////////////////////////////
// wxDirDialog

%if wxLUA_USE_wxDirDialog

%class %noclassinfo wxDirDialog, wxDialog
    wxDirDialog(wxWindow* parent, const wxString& message = "Choose a directory", const wxString& defaultPath = "", long style = 0, const wxPoint& pos = wxDefaultPosition)
    wxString GetPath() const
    wxString GetMessage() const
    long GetStyle() const
    void SetMessage(const wxString& message)
    %property=Message, read, write
    void SetPath(const wxString& path)
    %property=Path, read, write
    void SetStyle(long style)
    %property=Style, read, write
    int ShowModal()
%endclass

%endif wxLUA_USE_wxDirDialog

///////////////////////////////////////////////////////////////////////////////
// wxMessageDialog

%if wxLUA_USE_wxMessageDialog

%win %class wxMessageDialog, wxDialog
%gtk %class %noclassinfo wxMessageDialog, wxDialog
%mac %class %noclassinfo wxMessageDialog, wxDialog
    wxMessageDialog(wxWindow* parent, const wxString& message, const wxString& caption = "Message box", long style = wxOK | wxCANCEL | wxCENTRE, const wxPoint& pos = wxDefaultPosition)
    int ShowModal()
%endclass

%endif wxLUA_USE_wxMessageDialog

///////////////////////////////////////////////////////////////////////////////
// wxMultipleChoiceDialog
//
// %class wxMultipleChoiceDialog, wxDialog

///////////////////////////////////////////////////////////////////////////////
// wxSingleChoiceDialog

%if wxLUA_USE_wxSingleChoiceDialog

%class wxSingleChoiceDialog, wxDialog
    int GetSelection() const
    wxString GetStringSelection() const
    %property=StringSelection, read
    void SetSelection(int selection) const
    %property=Selection, read, write
    int ShowModal()
%endclass

%endif wxLUA_USE_wxSingleChoiceDialog

///////////////////////////////////////////////////////////////////////////////
// wxTextEntryDialog

%if wxLUA_USE_wxTextEntryDialog

%class wxTextEntryDialog, wxDialog
    wxTextEntryDialog(wxWindow* parent, const wxString& message, const wxString& caption = "Please enter text", const wxString& defaultValue = "", long style = wxOK | wxCANCEL | wxCENTRE, const wxPoint& pos = wxDefaultPosition)
    wxString GetValue() const
    void SetValue(const wxString& value)
    %property=Value, read, write
    int ShowModal()
%endclass

%endif wxLUA_USE_wxTextEntryDialog

///////////////////////////////////////////////////////////////////////////////
// wxFontDialog

%if wxLUA_USE_wxFontDialog

%include "wx/fontdlg.h"

%class wxFontDialog, wxDialog
    wxFontDialog(wxWindow* parent, wxFontData* data = NULL)
    wxFontData& GetFontData()
    %win %property=FontData, read
    int ShowModal()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFontData

%include "wx/cmndata.h"

%class %delete %noclassinfo wxFontData
    wxFontData()
    void EnableEffects(bool enable)
    bool GetAllowSymbols()
    wxColour& GetColour()
    wxFont GetChosenFont()
    bool GetEnableEffects()
    wxFont GetInitialFont()
    %win bool GetShowHelp()
    void SetAllowSymbols(bool allowSymbols)
    %property=AllowSymbols, read, write
    void SetChosenFont(const wxFont &font)
    %property=ChosenFont, read, write
    void SetColour(const wxColour &colour)
    %property=Colour, read, write
    void SetInitialFont(const wxFont &font)
    void SetRange(int minimum, int maximum)
    %win void SetShowHelp(bool showHelp)
    %win %property=ShowHelp, read, write
#endclass

%endif wxLUA_USE_wxFontDialog

///////////////////////////////////////////////////////////////////////////////
// wxFindReplaceDialog

%if wxLUA_USE_wxFindReplaceDialog

%include "wx/fdrepdlg.h"

%enum wxFindReplaceDialogStyles
    wxFR_REPLACEDIALOG
    wxFR_NOUPDOWN
    wxFR_NOMATCHCASE
    wxFR_NOWHOLEWORD
%end

%class wxFindReplaceDialog, wxDialog
    wxFindReplaceDialog(wxWindow *parent, wxFindReplaceData *findData, const wxString &title, int style = 0)
    %constructor wxFindReplaceDialogDefault()
    bool    Create(wxWindow *parent, wxFindReplaceData *findData, const wxString &title, int style = 0)
    const wxFindReplaceData *GetData()
    void    SetData(wxFindReplaceData *findData)
    %property=Data, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFindReplaceData

%enum wxFindReplaceFlags
    wxFR_DOWN
    wxFR_WHOLEWORD
    wxFR_MATCHCASE
%end

%class %delete %noclassinfo wxFindReplaceData
    wxFindReplaceData(int flags = 0)
    int     GetFlags()
    const wxString& GetFindString()
    const wxString& GetReplaceString()
    void    SetFlags(int flags)
    void    SetFindString(const wxString& string)
    %property=FindString, read, write
    void    SetReplaceString(const wxString& string)
    %property=ReplaceString, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFindDialogEvent

%class %delete wxFindDialogEvent, wxCommandEvent
    %define %event wxEVT_COMMAND_FIND
    %define %event wxEVT_COMMAND_FIND_NEXT
    %define %event wxEVT_COMMAND_FIND_REPLACE
    %define %event wxEVT_COMMAND_FIND_REPLACE_ALL
    %define %event wxEVT_COMMAND_FIND_CLOSE

    wxFindDialogEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
    int     GetFlags()
    wxString GetFindString()
    const wxString& GetReplaceString()
    void    SetFlags(int flags)
    %property=Flags, read, write
    void    SetFindString(const wxString& str)
    %property=FindString, read, write
    void    SetReplaceString(const wxString& str)
    %property=ReplaceString, read, write
    wxFindReplaceDialog *GetDialog()
    %property=Dialog, read
%endclass

%endif wxLUA_USE_wxFindReplaceDialog

///////////////////////////////////////////////////////////////////////////////
// wxProgressDialog

%if wxLUA_USE_wxProgressDialog

%include "wx/generic/progdlgg.h"

%define wxPD_APP_MODAL
%define wxPD_AUTO_HIDE
%define wxPD_CAN_ABORT
%define wxPD_ELAPSED_TIME
%define wxPD_ESTIMATED_TIME
%define wxPD_REMAINING_TIME

%class wxProgressDialog, wxDialog
    wxProgressDialog(const wxString& title, const wxString& message, int maximum = 100, wxWindow* parent = NULL, int style = wxPD_AUTO_HIDE | wxPD_APP_MODAL)
    bool    Update(int value = -1, const wxString &newmsg = "")
    void    Resume()
%endclass

%endif wxLUA_USE_wxProgressDialog

///////////////////////////////////////////////////////////////////////////////
// wxTabbedDialog                deprecated; use wxNotebook instead
//
// %class wxTabbedDialog, wxDialog
// %include "wx/wxtab.h"
// wxTabbedDialog(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style=wxDEFAULT_DIALOG_STYLE, const wxString& name="dialogBox")
// void SetTabView(wxTabView *view)
// wxTabView * GetTabView()
// %endclass

///////////////////////////////////////////////////////////////////////////////
// wxWizard

// TODO - wxWizard is not fully implemented or tested

%if wxLUA_USE_wxWizard

//%include "wx/wizard.h"

//%define wxWIZARD_EX_HELPBUTTON

//%class wxWizard, wxDialog
//	static wxWizard* Create(wxWindow* parent, int id = -1, const wxString& title = "", const wxBitmap& bitmap = wxNullBitmap, const wxPoint& pos = wxDefaultPosition)
//	bool RunWizard(wxWizardPage* firstPage)
//	wxWizardPage* GetCurrentPage() const
//	wxSize GetPageSize() const
//	void SetPageSize(const wxSize& sizePage)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxWizardPage - this has virtual functions so it can't be used?

//%class wxWizardPage, wxPanel
//	wxWizardPage(wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap)
//	wxWizardPage* GetPrev() const
//	wxWizardPage* GetNext() const
//	wxBitmap GetBitmap() const
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxWizardPageSimple
//%class wxWizardPageSimple, wxWizardPage
//	wxWizardPageSimple(wxWizard* parent = NULL, wxWizardPage* prev = NULL, wxWizardPage* next = NULL)
//	void SetPrev(wxWizardPage* prev)
//	void SetNext(wxWizardPage* next)
//	static void Chain(wxWizardPageSimple* first, wxWizardPageSimple* second)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxWizardEvent

//%class %delete wxWizardEvent, wxNotifyEvent
//    %define %event wxEVT_WIZARD_CANCEL
//    %define %event wxEVT_WIZARD_PAGE_CHANGED
//    %define %event wxEVT_WIZARD_PAGE_CHANGING
//    %define %event wxEVT_WIZARD_HELP
//
//    wxWizardEvent(wxEventType type = wxEVT_NULL, int id = -1, bool direction = TRUE)
//    bool GetDirection() const
//    %property=Direction, read
//%endclass

%endif wxLUA_USE_wxWizard
