/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxMenu and wxToolbar classes
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// wxMenu

%if wxLUA_USE_wxMenu

%include "wx/menu.h"

%define wxMB_DOCKABLE
%define wxMENU_TEAROFF

%class wxMenu, wxEvtHandler
    wxMenu(const wxString& title = "", long style = 0)
    %constructor wxCreateMenu(int table, const wxString& title = "", long style = 0)
    void Append(int id, const wxString& item, const wxString& helpString = "", const bool checkable = FALSE)
    %rename AppendMenu void Append(int id, const wxString& item, wxMenu *subMenu, const wxString& helpString = "")
    %rename AppendItem void Append(wxMenuItem* menuItem)
    void AppendSeparator()
    void Break()
    void Check(int id, const bool check)
    void Delete(int id)
    %rename DeleteItem void Delete(wxMenuItem *item)
    void Destroy(int id)
    %rename DestroyMenuItem void Destroy(wxMenuItem *item)
    void Enable(int id, const bool enable)
    int FindItem(const wxString& itemString) const
    // %rename FindItemById wxMenuItem * FindItem(int id, wxMenu **menu = NULL) const
    wxString GetHelpString(int id) const
    wxString GetLabel(int id) const
    size_t GetMenuItemCount() const
    wxMenuItemList& GetMenuItems() const
    wxString GetTitle() const
    <wxver25 bool Insert(size_t pos, wxMenuItem *item)
    %wxver25 wxMenuItem* Insert(size_t pos, wxMenuItem *item)
    bool IsChecked(int id) const
    bool IsEnabled(int id) const
    %rename RemoveById wxMenuItem * Remove(int id)
    wxMenuItem * Remove(wxMenuItem *item)
    void SetHelpString(int id, const wxString& helpString)
    void SetLabel(int id, const wxString& label)
    void SetTitle(const wxString& title)
    %property=Title, read, write
    void UpdateUI(wxEvtHandler* source = NULL) const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMenuBar

%class wxMenuBar, wxEvtHandler
    wxMenuBar(long style = 0)
    // void wxMenuBar(int n, wxMenu* menus[], const wxString titles[])
    bool Append(wxMenu *menu, const wxString& title)
    void Check(int id, const bool check)
    void Enable(int id, const bool enable)
    void EnableTop(int pos, const bool enable)
    int FindMenu(const wxString& title) const
    int FindMenuItem(const wxString& menuString, const wxString& itemString) const
    // wxMenuItem * FindItem(int id, wxMenu **menu = NULL) const
    wxString GetHelpString(int id) const
    wxString GetLabel(int id) const
    wxString GetLabelTop(int pos) const
    wxMenu* GetMenu(int menuIndex) const
    int GetMenuCount() const
    %property=MenuCount, read
    bool Insert(size_t pos, wxMenu *menu, const wxString& title)
    bool IsChecked(int id) const
    bool IsEnabled(int id) const
    void Refresh()
    wxMenu * Remove(size_t pos)
    wxMenu * Replace(size_t pos, wxMenu *menu, const wxString& title)
    void SetHelpString(int id, const wxString& helpString)
    void SetLabel(int id, const wxString& label)
    void SetLabelTop(int pos, const wxString& label)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMenuItem

%class wxMenuItem, wxObject
    %include "wx/menuitem.h"
    wxMenuItem(wxMenu *parentMenu = NULL, int id = wxID_SEPARATOR, const wxString& text = wxEmptyString, const wxString& help = wxEmptyString, bool isCheckable = FALSE, wxMenu *subMenu = NULL)
    void Check(bool check)
    void Enable(bool enable)
    %win wxColour& GetBackgroundColour() const
    // wxBitmap& GetBitmap(bool checked = TRUE) const
    %win wxFont& GetFont() const
    wxString GetHelp() const
    int GetId() const
    wxString GetLabel() const
    static wxString GetLabelFromText(const wxString& text)
    %win int GetMarginWidth() const
    // wxString GetName() const
    wxString GetText() const
    wxMenu* GetSubMenu() const
    %win wxColour& GetTextColour() const
    bool IsCheckable() const
    bool IsChecked() const
    bool IsEnabled() const
    bool IsSeparator() const
    %win void SetBackgroundColour(const wxColour& colour) const
    %win %property=BackgroundColour, read, write
    %win void SetBitmaps(const wxBitmap& checked, const wxBitmap& unchecked = wxNullBitmap) const
    %win void SetFont(const wxFont& font) const
    %win %property=Font, read, write
    void SetHelp(const wxString& helpString) const
    %property=Help, read, write
    %win void SetMarginWidth(int width) const
    %win %property=MarginWidth, read, write
    // void SetName(const wxString& text) const
    %win void SetTextColour(const wxColour& colour) const
    %win %property=TextColour, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMenuItemList

%class %noclassinfo wxMenuItemList
    void Clear()
    void DeleteContents(bool destroy)
    bool DeleteObject(wxMenuItem *pMenuItem)
    int     GetCount() const
    %property=Count, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMenuEvent

%include "wx/event.h"

%class %delete wxMenuEvent, wxEvent
    %define %event wxEVT_MENU_HIGHLIGHT
    ~%wxver22 %define %event wxEVT_MENU_OPEN
    ~%wxver22 %define %event wxEVT_MENU_CLOSE
    %define %event wxEVT_MENU_HIGHLIGHT

    wxMenuEvent(wxEventType type = wxEVT_NULL, int id = 0)
    int GetMenuId() const
%endclass

%endif wxLUA_USE_wxMenu

///////////////////////////////////////////////////////////////////////////////
// wxToolBarBase

%if wxLUA_USE_wxToolbar

%include "wx/tbarbase.h"

%define wxTB_FLAT
%define wxTB_DOCKABLE
%define wxTB_HORIZONTAL
%define wxTB_VERTICAL
%define wxTB_3DBUTTONS
~%wxver22 %define wxTB_TEXT
~%wxver22 %define wxTB_NOICONS
~%wxver22 %define wxTB_NODIVIDER
~%wxver22 %define wxTB_NOALIGN

~%wxver22 %enum wxItemKind
~%wxver22     wxITEM_SEPARATOR
~%wxver22     wxITEM_NORMAL
~%wxver22     wxITEM_CHECK
~%wxver22     wxITEM_RADIO
~%wxver22     wxITEM_MAX
~%wxver22 %end

%class %noclassinfo wxToolBarBase, wxControl
    wxToolBarToolBase *AddControl(wxControl *control)
    wxToolBarToolBase *AddSeparator()
    wxToolBarToolBase *AddTool(int id, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap, bool isToggle = FALSE, wxObject *clientData = NULL, const wxString& longHelpString = "")
    wxToolBarToolBase *InsertTool(size_t pos, int id, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap, bool isToggle = FALSE, wxObject *clientData = NULL, const wxString& shortHelpString = "", const wxString& longHelpString = "")
    wxToolBarToolBase *InsertControl(size_t pos, wxControl *control)
    wxToolBarToolBase *InsertSeparator(size_t pos)
    wxToolBarToolBase *RemoveTool(int id)
    wxSize  GetMargins()
    bool    DeleteTool(int id)
    bool    DeleteToolByPos(size_t pos)
    void    EnableTool(int id, bool enable)
    void    ToggleTool(int id, bool toggle)
    void    ClearTools()
    bool    GetToolEnabled(int id)
    %property=ToolEnabled, read
    int     GetToolPacking()
    wxSize  GetToolBitmapSize()
    wxSize  GetToolSize()
    %property=ToolSize, read
    int     GetMaxRows()
    %property=MaxRows, read
    int     GetMaxCols()
    %property=MaxCols, read
    wxSize  GetToolMargins()
    %property=ToolMargins, read
    int     GetToolSeparation()
    bool    GetToolState(int id)
    wxString GetToolShortHelp(int id)
    wxString GetToolLongHelp(int id)
    wxObject* GetToolClientData(int index)
    bool    Realize()
    %rename SetMarginsXY void SetMargins(int x, int y)
    void    SetMargins(const wxSize& size)
    %property=Margins, read, write
    void    SetToggle(int id, bool toggle)
    void    SetToolClientData(int index, wxObject* clientData)
    void    SetToolShortHelp(int id, const wxString& helpString)
    void    SetToolLongHelp(int id, const wxString& helpString)
    void    SetToolPacking(int packing)
    %property=ToolPacking, read, write
    void    SetToolSeparation(int separation)
    %property=ToolSeparation, read, write
    void    SetRows(int nRows)
    void    SetMaxRowsCols(int rows, int cols)
    void    SetToolBitmapSize(const wxSize& size)
    %property=ToolBitmapSize, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxToolBar

%include "wx/toolbar.h"

%class wxToolBar, wxToolBarBase
    wxToolBar(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTB_HORIZONTAL, const wxString &name = wxToolBarNameStr)
    %constructor wxPreToolBar()
    bool  Create(wxWindow *parent,wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTB_HORIZONTAL, const wxString &name = wxToolBarNameStr)
    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxToolBarSimple

<wxver25 %include "wx/tbarsmpl.h"

<wxver25 %class wxToolBarSimple, wxToolBarBase
<wxver25     wxToolBarSimple(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTB_HORIZONTAL, const wxString &name = wxToolBarNameStr)
<wxver25     %constructor wxPreToolBarSimple()
<wxver25     bool    Create(wxWindow *parent,wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTB_HORIZONTAL, const wxString &name = wxToolBarNameStr)
<wxver25     wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y)
<wxver25 %endclass

///////////////////////////////////////////////////////////////////////////////
// wxToolBarToolBase

// these are unused
//%define wxTOOL_BOTTOM
//%define wxTOOL_LEFT
//%define wxTOOL_RIGHT
//%define wxTOOL_TOP

%define wxTOOL_STYLE_BUTTON
%define wxTOOL_STYLE_SEPARATOR
%define wxTOOL_STYLE_CONTROL

%class %noclassinfo wxToolBarToolBase
    int     GetId()
    wxControl *GetControl()
    wxToolBarBase *GetToolBar()
    int     IsButton()
    int     IsControl()
    int     IsSeparator()
    int     GetStyle()
    bool    IsEnabled()
    bool    IsToggled()
    bool    CanBeToggled()
    %wxver22 %rename GetNormalBitmap const wxBitmap& GetBitmap1()
    ~%wxver22 const wxBitmap& GetNormalBitmap()
    %wxver22 %rename GetDisabledBitmap const wxBitmap& GetBitmap2()
    ~%wxver22 const wxBitmap& GetDisabledBitmap()
    const wxBitmap& GetBitmap()
    %property=Bitmap, read
    wxString GetShortHelp()
    wxString GetLongHelp()
    bool    Enable(bool enable)
    bool    Toggle(bool toggle)
    bool    SetToggle(bool toggle)
    bool    SetShortHelp(const wxString& help)
    %property=ShortHelp, read, write
    bool    SetLongHelp(const wxString& help)
    %property=LongHelp, read, write
    %wxver22 %rename SetNormalBitmap void    SetBitmap1(const wxBitmap& bmp)
    %wxver22 %rename SetDisabledBitmap void    SetBitmap2(const wxBitmap& bmp)
    ~%wxver22 void    SetNormalBitmap(const wxBitmap& bmp)
    ~%wxver22 %property=NormalBitmap, read, write
    ~%wxver22 void    SetDisabledBitmap(const wxBitmap& bmp)
    ~%wxver22 %property=DisabledBitmap, read, write
    void    Detach()
    void    Attach(wxToolBarBase *tbar)
    wxObject *GetClientData()
    void    SetClientData(wxObject* clientData)
    %property=ClientData, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxToolBarTool

%class %noclassinfo wxToolBarTool, wxToolBarToolBase
%endclass

%endif wxLUA_USE_wxToolbar

