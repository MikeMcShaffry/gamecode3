/////////////////////////////////////////////////////////////////////////////
// Purpose:     GUI controls like buttons, combos, etc
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// wxButton

%if wxLUA_USE_wxButton

%include "wx/button.h"

%define wxBU_LEFT
%define wxBU_RIGHT
%define wxBU_TOP
%define wxBU_BOTTOM
~%wxver22 %define wxBU_EXACTFIT

%class wxButton, wxControl
    wxButton(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxButtonNameStr)
    bool     Create(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxButtonNameStr)
    wxString GetLabel() const
    static wxSize GetDefaultSize()
    %property=DefaultSize, read
    void     SetDefault()
    void     SetLabel(const wxString& label)
    %property=Label, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxBitmapButton

%if wxLUA_USE_wxBitmapButton

%include "wx/bmpbuttn.h"

%class wxBitmapButton, wxButton
    wxBitmapButton( wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW, const wxValidator& validator = wxDefaultValidator, const wxString& name = "button")
    bool      Create(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "button")
    wxBitmap& GetBitmapDisabled() const
    wxBitmap& GetBitmapFocus() const
    wxBitmap& GetBitmapLabel() const
    wxBitmap& GetBitmapSelected() const
    void      SetBitmapDisabled(const wxBitmap& bitmap)
    void      SetBitmapFocus(const wxBitmap& bitmap)
    void      SetBitmapLabel(const wxBitmap& bitmap)
    void      SetBitmapSelected(const wxBitmap& bitmap)
    %property=BitmapDisabled, read, write
    %property=BitmapFocus, read, write
    %property=BitmapLabel, read, write
    %property=BitmapSelected, read, write
%endclass

%endif wxLUA_USE_wxBitmapButton
%endif wxLUA_USE_wxButton

///////////////////////////////////////////////////////////////////////////////
// wxCheckBox

%if wxLUA_USE_wxCheckBox

%include "wx/checkbox.h"

%class wxCheckBox, wxControl
    wxCheckBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& val = wxDefaultValidator, const wxString& name = "checkBox")
    bool     Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& val = wxDefaultValidator, const wxString& name = "checkBox")
    bool     GetValue() const
    void     SetValue(const bool state)
    %property=Value, read, write
    %wxver22 %rename IsChecked bool GetValue() const
    ~%wxver22 bool     IsChecked() const
%endclass

%endif wxLUA_USE_wxCheckBox

///////////////////////////////////////////////////////////////////////////////
// wxChoice

%if wxLUA_USE_wxChoice

%include "wx/choice.h"

%class wxChoice, wxControl
    wxChoice(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxChoiceNameStr)
    void     Append(const wxString& item)
    void     Clear()
    bool     Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxChoiceNameStr)
    int      FindString(const wxString& string) const
    void    *GetClientData(int n) const
    int      GetSelection() const
    wxString GetString(int n) const
    wxString GetStringSelection() const
    int      GetCount() const
    %property=Count, read
    void     SetClientData(int n, wxObject * data)
    %motif  int      GetColumns() const   // Motif only
    %motif  void     SetColumns(int n = 1)
    %motif  %property=Columns, read, write
    void     SetSelection(int n)
    %property=Selection, read, write
    void     SetStringSelection(const wxString& string)
    %property=StringSelection, read, write
%endclass

%endif wxLUA_USE_wxChoice

///////////////////////////////////////////////////////////////////////////////
// wxComboBox

%if wxLUA_USE_wxComboBox

%include "wx/combobox.h"

%define wxCB_DROPDOWN
%define wxCB_READONLY
%define wxCB_SIMPLE
%define wxCB_SORT

%class wxComboBox, wxControl
    wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")
    void     Append(const wxString& item)
    void     Clear()
    bool     Create(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")
    void     Copy()
    void     Cut()
    void     Delete(int n)
    int      FindString(const wxString& string)
    void    *GetClientData(int n) const
    long     GetInsertionPoint() const
    long     GetLastPosition() const
    int      GetSelection() const
    wxString GetString(int n) const
    wxString GetStringSelection() const
    %property=StringSelection, read
    wxString GetValue() const
    int      GetCount() const
    %property=Count, read
    void     Paste()
    void     Replace(long from, long to, const wxString& text)
    void     Remove(long from, long to)
    void     SetClientData(int n, wxObject * data)
    void     SetInsertionPoint(long pos)
    %property=InsertionPoint, read, write
    void     SetInsertionPointEnd()
    void     SetSelection(int n)
    %property=Selection, read, write
    %rename SetMark void SetSelection(long from, long to)
    void     SetValue(const wxString& text)
    %property=Value, read, write
%endclass

%endif wxLUA_USE_wxComboBox

///////////////////////////////////////////////////////////////////////////////
// wxGauge

%if wxLUA_USE_wxGauge

%include "wx/gauge.h"

%define wxGA_HORIZONTAL
%define wxGA_PROGRESSBAR
%define wxGA_SMOOTH
%define wxGA_VERTICAL

%class wxGauge, wxControl
    wxGauge(wxWindow* parent, wxWindowID id, int range, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxGA_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "gauge")
    bool     Create(wxWindow* parent, wxWindowID id, int range, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxGA_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "gauge")
    int      GetBezelFace() const
    int      GetRange() const
    int      GetShadowWidth() const
    int      GetValue() const
    void     SetBezelFace(int width)
    %property=BezelFace, read, write
    void     SetRange(int range)
    %property=Range, read, write
    void     SetShadowWidth(int width)
    %property=ShadowWidth, read, write
    void     SetValue(int pos)
    %property=Value, read, write
%endclass

%endif wxLUA_USE_wxGauge

///////////////////////////////////////////////////////////////////////////////
// wxListBox

%if wxLUA_USE_wxListBox

%include "wx/listbox.h"

%define wxLB_SINGLE
%define wxLB_MULTIPLE
%define wxLB_EXTENDED
%define wxLB_HSCROLL
%define wxLB_ALWAYS_SB
%define wxLB_NEEDED_SB
%define wxLB_SORT
%define wxLB_OWNERDRAW

%class wxListBox, wxControl
    wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
    void     Append(const wxString& item)
    void     Clear()
    bool     Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
    void     Delete(int n)
    void     Deselect(int n)
    int      FindString(const wxString& string)
    wxObject *GetClientData(int n) const
    int      GetSelection() const
    %property=Selection, read
    int      GetSelections(wxArrayInt& selections) const
    wxString GetString(int n) const
    wxString GetStringSelection() const
    %property=StringSelection, read
    int GetCount() const
    %property=Count, read
    bool     Selected(int n) const
    // void Set(int n, const wxString* choices)
    void     SetClientData(int n, wxObject *data)
    void     SetFirstItem(int n)
    void     SetSelection(int n, const bool select = TRUE)
    void     SetString(int n, const wxString& string)
    void     SetStringSelection(const wxString& string, const bool select = TRUE)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCheckListBox

%if wxLUA_USE_wxCheckListBox

%include "wx/checklst.h"

%class wxCheckListBox, wxListBox
    wxCheckListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
    void     Check(int item, bool check = TRUE)
    bool     IsChecked(int item) const
%endclass

%endif wxLUA_USE_wxCheckListBox
%endif wxLUA_USE_wxListBox

///////////////////////////////////////////////////////////////////////////////
// wxListCtrl

%if wxLUA_USE_wxListCtrl

%include "wx/listctrl.h"

%define wxLC_ALIGN_LEFT
%define wxLC_ALIGN_TOP
%define wxLC_AUTOARRANGE
%define wxLC_EDIT_LABELS
~%wxver22 %define wxLC_HRULES
%define wxLC_ICON
%define wxLC_LIST
%define wxLC_MASK_ALIGN
%define wxLC_MASK_SORT
%define wxLC_MASK_TYPE
%define wxLC_NO_HEADER
%define wxLC_NO_SORT_HEADER
%define wxLC_REPORT
%define wxLC_SINGLE_SEL
%define wxLC_SMALL_ICON
%define wxLC_SORT_ASCENDING
%define wxLC_SORT_DESCENDING
%define wxLC_USER_TEXT
~%wxver22 %define wxLC_VRULES

%define wxLIST_ALIGN_DEFAULT
%define wxLIST_ALIGN_LEFT
%define wxLIST_ALIGN_SNAP_TO_GRID
%define wxLIST_ALIGN_TOP
%define wxLIST_AUTOSIZE
%define wxLIST_AUTOSIZE_USEHEADER
%define wxLIST_FIND_DOWN
%define wxLIST_FIND_LEFT
%define wxLIST_FIND_RIGHT
%define wxLIST_FIND_UP
%define wxLIST_HITTEST_ABOVE
%define wxLIST_HITTEST_BELOW
%define wxLIST_HITTEST_NOWHERE
%define wxLIST_HITTEST_ONITEM
%define wxLIST_HITTEST_ONITEMICON
%define wxLIST_HITTEST_ONITEMLABEL
%define wxLIST_HITTEST_ONITEMRIGHT
%define wxLIST_HITTEST_ONITEMSTATEICON
%define wxLIST_HITTEST_TOLEFT
%define wxLIST_HITTEST_TORIGHT
%define wxLIST_MASK_DATA
%define wxLIST_MASK_FORMAT
%define wxLIST_MASK_IMAGE
%define wxLIST_MASK_STATE
%define wxLIST_MASK_TEXT
%define wxLIST_MASK_WIDTH
%define wxLIST_NEXT_ABOVE
%define wxLIST_NEXT_ALL
%define wxLIST_NEXT_BELOW
%define wxLIST_NEXT_LEFT
%define wxLIST_NEXT_RIGHT
%define wxLIST_RECT_BOUNDS
%define wxLIST_RECT_ICON
%define wxLIST_RECT_LABEL
%define wxLIST_SET_ITEM
%define wxLIST_STATE_CUT
%define wxLIST_STATE_DONTCARE
%define wxLIST_STATE_DROPHILITED
%define wxLIST_STATE_FOCUSED
%define wxLIST_STATE_SELECTED

%class wxListCtrl, wxControl
    wxListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")
    bool     Arrange(int flag = wxLIST_ALIGN_DEFAULT)
SetImageList
    %wxver22 %rename AssignImageList void SetImageList(wxImageList *imageList, int which)
    ~%wxver22 void AssignImageList(wxImageList *imageList, int which)
    void     ClearAll()
    bool     Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")
    bool     DeleteAllItems()
    bool     DeleteColumn(int col)
    bool     DeleteItem(long item)
    void     EditLabel(long item)
    bool     EnsureVisible(long item)
    long     FindItem(long start, const wxString& str, const bool partial = FALSE)
    %rename FindItemData long FindItem(long start, long data)
    %rename FindItemAtPos long FindItem(long start, const wxPoint& pt, int direction)
    bool     GetColumn(int col, wxListItem& item) const
    int      GetColumnWidth(int col) const
    int      GetCountPerPage() const
    %property=CountPerPage, read
    // wxTextCtrl& GetEditControl() const
    wxImageList* GetImageList(int which) const
    bool     GetItem(wxListItem& info) const
    long     GetItemData(long item) const
    bool     GetItemPosition(long item, wxPoint& pos) const
    bool     GetItemRect(long item, wxRect& rect, int code = wxLIST_RECT_BOUNDS) const
    int      GetItemState(long item, long stateMask) const
    int      GetItemCount() const
    %property=ItemCount, read
    int      GetItemSpacing(bool isSmall) const
    wxString GetItemText(long item) const
    long     GetNextItem(long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE) const
    int      GetSelectedItemCount() const
    %property=SelectedItemCount, read
    wxColour GetTextColour() const
    long     GetTopItem() const
    %property=TopItem, read
    long     HitTest(const wxPoint& point, int& flags)
    long     InsertColumn(long col, wxListItem& info)
    %rename  InsertColumnInfo long InsertColumn(long col, const wxString& heading, int format = wxLIST_FORMAT_LEFT, int width = -1)
    long     InsertItem(wxListItem& info)
    %rename  InsertStringItem long InsertItem(long index, const wxString& label)
    %rename  InsertImageItem long InsertItem(long index, int imageIndex)
    %rename  InsertImageStringItem long InsertItem(long index, const wxString& label, int imageIndex)
    bool     ScrollList(int dx, int dy)
    void     SetBackgroundColour(const wxColour& col)
    bool     SetColumn(int col, wxListItem& item)
    bool     SetColumnWidth(int col, int width)
    void     SetImageList(wxImageList* imageList, int which)
    bool     SetItem(wxListItem& info)
    %rename SetStringItem long SetItem(long index, int col, const wxString& label, int imageId = -1)
    bool     SetItemData(long item, long data)
    bool     SetItemImage(long item, int image, int selImage)
    bool     SetItemPosition(long item, const wxPoint& pos)
    bool     SetItemState(long item, long state, long stateMask)
    void     SetItemText(long item, const wxString& text)
    void     SetSingleStyle(long style, const bool add = TRUE)
    void     SetTextColour(const wxColour& col)
    %property=TextColour, read, write
    void     SetWindowStyleFlag(long style)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxListItemAttr - wxListCtrl

%class %delete %noclassinfo %encapsulate wxListItemAttr
    wxListItemAttr(const wxColour& colText = wxNullColour, const wxColour& colBack = wxNullColour, const wxFont& font = wxNullFont)
    void SetTextColour(const wxColour& colText)
    void SetBackgroundColour(const wxColour& colBack)
    void SetFont(const wxFont& font)
    bool HasTextColour()
    bool HasBackgroundColour()
    bool HasFont()
    const wxColour& GetTextColour()
    %property=TextColour, read, write
    const wxColour& GetBackgroundColour()
    %property=BackgroundColour, read, write
    const wxFont& GetFont()
    %property=Font, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxListItem - wxListCtrl

%enum wxListColumnFormat
    wxLIST_FORMAT_LEFT
    wxLIST_FORMAT_RIGHT
    wxLIST_FORMAT_CENTRE
    wxLIST_FORMAT_CENTER
%end

%class %delete wxListItem, wxObject
    wxListItem()
    void Clear()
    void ClearAttributes()
    void SetMask(long mask)
    void SetId(long id)
    void SetColumn(int col)
    void SetState(long state)
    void SetStateMask(long stateMask)
    void SetText(const wxString& text)
    void SetImage(int image)
    void SetData(long data)
    void SetWidth(int width)
    void SetAlign(wxListColumnFormat align)
    void SetTextColour(const wxColour& colText)
    void SetBackgroundColour(const wxColour& colBack)
    void SetFont(const wxFont& font)
    long GetMask()
    %property=Mask, read, write
    long GetId()
    %property=Id, read, write
    int GetColumn()
    %property=Column, read, write
    long GetState()
    %property=State, read, write
    const wxString& GetText()
    %property=Text, read, write
    int GetImage()
    %property=Image, read, write
    long GetData()
    %property=Data, read, write
    int GetWidth()
    %property=Width, read, write
    wxListColumnFormat GetAlign()
    %property=Align, read, write
    wxListItemAttr *GetAttributes()
    bool HasAttributes()
    wxColour GetTextColour() const
    %property=TextColour, read, write
    wxColour GetBackgroundColour() const
    %property=BackgroundColour, read, write
    wxFont GetFont() const
    %property=Font, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxListEvent - wxListCtrl

%class %delete wxListEvent, wxNotifyEvent
    %define %event wxEVT_COMMAND_LIST_BEGIN_DRAG
    %define %event wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT
    %define %event wxEVT_COMMAND_LIST_BEGIN_RDRAG
    %define %event wxEVT_COMMAND_LIST_COL_CLICK
    %define %event wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS
    %define %event wxEVT_COMMAND_LIST_DELETE_ITEM
    %define %event wxEVT_COMMAND_LIST_END_LABEL_EDIT
    %define %event wxEVT_COMMAND_LIST_GET_INFO
    %define %event wxEVT_COMMAND_LIST_INSERT_ITEM
    %define %event wxEVT_COMMAND_LIST_ITEM_ACTIVATED
    %define %event wxEVT_COMMAND_LIST_ITEM_DESELECTED
    %define %event wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK
    %define %event wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK
    %define %event wxEVT_COMMAND_LIST_ITEM_SELECTED
    %define %event wxEVT_COMMAND_LIST_KEY_DOWN
    %define %event wxEVT_COMMAND_LIST_SET_INFO
    ~%wxver22 %define %event wxEVT_COMMAND_LIST_CACHE_HINT
    ~%wxver22 %define %event wxEVT_COMMAND_LIST_COL_RIGHT_CLICK
    ~%wxver22 %define %event wxEVT_COMMAND_LIST_COL_BEGIN_DRAG
    ~%wxver22 %define %event wxEVT_COMMAND_LIST_COL_DRAGGING
    ~%wxver22 %define %event wxEVT_COMMAND_LIST_COL_END_DRAG
    ~%wxver22 %define %event wxEVT_COMMAND_LIST_ITEM_FOCUSED

    wxListEvent(WXTYPE commandType = 0, int id = 0)
    %wxver22 %rename GetKeyCode int GetCode() const
    ~%wxver22 int GetKeyCode() const
    %property=KeyCode, read
    long GetIndex() const
    %property=Index, read
    int GetColumn() const
    %property=Column, read
    wxPoint GetPoint() const
    %property=Point, read
    const wxString& GetLabel() const
    %property=Label, read
    const wxString& GetText() const
    %property=Text, read
    int GetImage() const
    %property=Image, read
    long GetData() const
    %property=Data, read
    long GetMask() const
    %property=Mask, read
    const wxListItem& GetItem() const
    %property=Item, read
%endclass

%endif wxLUA_USE_wxListCtrl

///////////////////////////////////////////////////////////////////////////////
// wxRadioBox

%if wxLUA_USE_wxRadioBox

%include "wx/radiobox.h"

%define wxRA_VERTICAL        // use wxVERTICAL
%define wxRA_HORIZONTAL      // use wxHORIZONTAL
%define wxRA_SPECIFY_COLS
%define wxRA_SPECIFY_ROWS

%class wxRadioBox, wxControl
    wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")
    void Enable(const bool enable)
    %rename EnableItem void Enable(int n, const bool enable)
    int FindString(const wxString& string) const
    %rename GetItemString wxString GetString(int n) const
    int GetSelection() const
    wxString GetStringSelection() const
    ~%wxver22 int GetCount() const
    %wxver22 %rename GetCount int Number() const
    %property=Count, read
    ~%wxver22 %rename SetItemString void SetString(int n, const wxString &label)
    void SetSelection(int n)
    %property=Selection, read, write
    void SetStringSelection(const wxString& string)
    %property=StringSelection, read, write
    void Show(const bool show)
    %rename ShowItem void Show(int item, const bool show)
    wxString GetString(int n) const
%endclass

%endif wxLUA_USE_wxRadioBox

///////////////////////////////////////////////////////////////////////////////
// wxRadioButton

%if wxLUA_USE_wxRadioButton

%include "wx/radiobut.h"

%define wxRB_GROUP
~%wxver22 %define wxRB_SINGLE

%class wxRadioButton, wxControl
    wxRadioButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioButton")
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioButton")
    bool GetValue() const
    void SetValue(const bool value)
    %property=Value, read, write
%endclass

%endif wxLUA_USE_wxRadioButton

///////////////////////////////////////////////////////////////////////////////
// wxScrollBar

%if wxLUA_USE_wxScrollBar

%include "wx/scrolbar.h"

%define wxSB_HORIZONTAL
%define wxSB_VERTICAL

%class wxScrollBar, wxControl
    wxScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSB_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "scrollBar")
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSB_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "scrollBar")
    int GetRange() const
    %property=Range, read
    int GetPageSize() const
    %property=PageSize, read
    int GetThumbPosition() const
    void SetThumbPosition(int viewStart)
    %property=ThumbPosition, read, write
    virtual void SetScrollbar(int position, int thumbSize, int range, int pageSize, const bool refresh = TRUE)
%endclass

%endif wxLUA_USE_wxScrollBar

///////////////////////////////////////////////////////////////////////////////
// wxSlider

%if wxLUA_USE_wxSlider

%include "wx/slider.h"

%define wxSL_AUTOTICKS
%define wxSL_BOTH
%define wxSL_BOTTOM
%define wxSL_HORIZONTAL
%define wxSL_LABELS
%define wxSL_LEFT
%define wxSL_NOTIFY_DRAG
%define wxSL_RIGHT
%define wxSL_SELRANGE
%define wxSL_TOP
%define wxSL_VERTICAL

%class wxSlider, wxControl
    wxSlider(wxWindow* parent, wxWindowID id, int value , int minValue, int maxValue, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSL_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "slider")
    void ClearSel()
    void ClearTicks()
    bool Create(wxWindow* parent, wxWindowID id, int value , int minValue, int maxValue, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSL_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "slider")
    int GetLineSize() const
    int GetMax() const
    %property=Max, read
    int GetMin() const
    %property=Min, read
    int GetPageSize() const
    int GetSelEnd() const
    %property=SelEnd, read
    int GetSelStart() const
    %property=SelStart, read
    int GetThumbLength() const
    int GetTickFreq() const
    int GetValue() const
    void SetRange(int minValue, int maxValue)
    void SetTickFreq(int n, int pos)
    void SetLineSize(int lineSize)
    %property=LineSize, read, write
    void SetPageSize(int pageSize)
    %property=PageSize, read, write
    void SetSelection(int startPos, int endPos)
    void SetThumbLength(int len)
    %property=ThumbLength, read, write
    void SetTick(int tickPos)
    void SetValue(int value)
    %property=Value, read, write
%endclass

%endif wxLUA_USE_wxSlider

///////////////////////////////////////////////////////////////////////////////
// wxSpinButton

%if wxLUA_USE_wxSpinButton

%include "wx/spinbutt.h"

%define wxSP_HORIZONTAL
%define wxSP_VERTICAL
%define wxSP_ARROW_KEYS
%define wxSP_WRAP

%class wxSpinButton, wxControl
    wxSpinButton(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_VERTICAL | wxSP_ARROW_KEYS, const wxString& name = "wxSpinButton")
    bool Create(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_VERTICAL | wxSP_ARROW_KEYS, const wxString& name = "wxSpinButton")
    int GetMax() const
    %property=Max, read
    int GetMin() const
    %property=Min, read
    int GetValue() const
    void SetRange(int min, int max)
    void SetValue(int value)
    %property=Value, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSpinEvent - for wxSpinButton

%include "wx/spinbutt.h"
%include "wx/spinctrl.h"

%class %delete wxSpinEvent, wxNotifyEvent
    %define %event wxEVT_SCROLL_LINEUP
    %define %event wxEVT_SCROLL_LINEDOWN
    %define %event wxEVT_SCROLL_THUMBTRACK

    wxSpinEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
    int GetPosition() const
    void SetPosition(int pos)
    %property=Position, read, write
%endclass

%endif wxLUA_USE_wxSpinButton

///////////////////////////////////////////////////////////////////////////////
// wxSpinCtrl

%if wxLUA_USE_wxSpinCtrl

%include "wx/spinctrl.h"

%define wxSP_ARROW_KEYS
%define wxSP_WRAP

%class wxSpinCtrl, wxControl
    wxSpinCtrl(wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_ARROW_KEYS, int min = 0, int max = 100, int initial = 0, const wxString& name = "wxSpinCtrl")
    bool Create(wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_ARROW_KEYS, int min = 0, int max = 100, int initial = 0, const wxString& name = "wxSpinCtrl")
    void SetValue(const wxString& text)
    %rename SetValueInt void SetValue(int iValue)
    int GetValue() const
    %property=Value, read, write
    void SetRange(int minVal, int maxVal)
    int GetMin() const
    %property=Min, read
    int GetMax() const
    %property=Max, read
%endclass

%endif wxLUA_USE_wxSpinCtrl

///////////////////////////////////////////////////////////////////////////////
// wxTextCtrl

%if wxLUA_USE_wxTextCtrl

%include "wx/textctrl.h"

%define wxTE_PROCESS_ENTER
%define wxTE_PROCESS_TAB
%define wxTE_MULTILINE
%define wxTE_PASSWORD
%define wxTE_READONLY
%define wxTE_RICH
~%wxver22 %define wxTE_RICH2
~%wxver22 %define wxTE_AUTO_URL
~%wxver22 %define wxTE_NOHIDESEL
%define wxHSCROLL
~%wxver22 %define wxTE_LEFT
~%wxver22 %define wxTE_CENTRE
~%wxver22 %define wxTE_RIGHT
~%wxver22 %define wxTE_DONTWRAP
~%wxver22 %define wxTE_LINEWRAP
~%wxver22 %define wxTE_WORDWRAP
~%wxver22 %define wxTE_AUTO_SCROLL
~%wxver22 %define wxTE_NO_VSCROLL

%class wxTextCtrl, wxControl
    wxTextCtrl(wxWindow *parent, wxWindowID id, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr)
    void AppendText(const wxString& text)
    virtual bool CanCopy()
    virtual bool CanCut()
    virtual bool CanPaste()
    virtual bool CanRedo()
    virtual bool CanUndo()
    virtual void Clear()
    virtual void Copy()
    bool Create(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "text")
    virtual void Cut()
    void DiscardEdits()
    virtual long GetInsertionPoint() const
    virtual long GetLastPosition() const
    int GetLineLength(long lineNo) const
    wxString GetLineText(long lineNo) const
    int GetNumberOfLines() const
    %property=NumberOfLines, read
    virtual void GetSelection(long* from, long* to)
    wxString GetValue() const
    bool IsModified() const
    bool LoadFile(const wxString& filename)
    virtual void Paste()
    bool PositionToXY(long pos, long *x, long *y) const
    virtual void Redo()
    virtual void Remove(long from, long to)
    virtual void Replace(long from, long to, const wxString& value)
    bool SaveFile(const wxString& filename)
    virtual void SetEditable(const bool editable)
    virtual void SetInsertionPoint(long pos)
    %property=InsertionPoint, read, write
    virtual void SetInsertionPointEnd()
    virtual void SetSelection(long from, long to)
    virtual void SetValue(const wxString& value)
    %property=Value, read, write
    void ShowPosition(long pos)
    virtual void Undo()
    void WriteText(const wxString& text)
    long XYToPosition(long x, long y)
%endclass

%endif wxLUA_USE_wxTextCtrl

///////////////////////////////////////////////////////////////////////////////
// wxTreeCtrl

%if wxLUA_USE_wxTreeCtrl

%include "wx/treectrl.h"

~%wxver22 %define wxTR_NO_BUTTONS
%define wxTR_HAS_BUTTONS
~%wxver22 %define wxTR_TWIST_BUTTONS
%define wxTR_NO_LINES
~%wxver22 %define wxTR_MAC_BUTTONS
%define wxTR_SINGLE
%define wxTR_MULTIPLE
%define wxTR_EXTENDED
%define wxTR_EDIT_LABELS
%define wxTR_LINES_AT_ROOT
~%wxver22 %define wxTR_HIDE_ROOT
~%wxver22 %define wxTR_ROW_LINES
%define wxTR_HAS_VARIABLE_ROW_HEIGHT
~%wxver22 %define wxTR_DEFAULT_STYLE

%enum wxTreeItemIcon
    wxTreeItemIcon_Normal
    wxTreeItemIcon_Selected
    wxTreeItemIcon_Expanded
    wxTreeItemIcon_SelectedExpanded
    wxTreeItemIcon_Max
%end

%define wxTREE_HITTEST_ABOVE
%define wxTREE_HITTEST_BELOW
%define wxTREE_HITTEST_NOWHERE
%define wxTREE_HITTEST_ONITEMBUTTON
%define wxTREE_HITTEST_ONITEMICON
%define wxTREE_HITTEST_ONITEMINDENT
%define wxTREE_HITTEST_ONITEMLABEL
%define wxTREE_HITTEST_ONITEMRIGHT
%define wxTREE_HITTEST_ONITEMSTATEICON
%define wxTREE_HITTEST_TOLEFT
%define wxTREE_HITTEST_TORIGHT
%define wxTREE_HITTEST_ONITEMUPPERPART
%define wxTREE_HITTEST_ONITEMLOWERPART
%define wxTREE_HITTEST_ONITEM


%class wxTreeCtrl, wxControl
    wxTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")
    wxTreeItemId AddRoot(const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL)
    wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL)
    void Collapse(const wxTreeItemId& item)
    void CollapseAndReset(const wxTreeItemId& item)
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")
    void Delete(const wxTreeItemId& item)
    void DeleteAllItems()
    void EditLabel(const wxTreeItemId& item)
    %win void EndEditLabel(const wxTreeItemId& item, bool discardChanges = FALSE)
    void EnsureVisible(const wxTreeItemId& item)
    void Expand(const wxTreeItemId& item)
    %win bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect, bool textOnly = FALSE) const
    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = TRUE) const
    int GetCount() const
    %property=Count, read
    // wxTextCtrl& GetEditControl() const
    <wxver25|%compat24 wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const
    ~<wxver25 wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
    wxTreeItemId GetFirstVisibleItem() const
    %property=FirstVisibleItem, read
    wxImageList* GetImageList() const
    int GetIndent() const
    wxTreeItemData* GetItemData(const wxTreeItemId& item) const
    int GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which = wxTreeItemIcon_Normal) const
    wxString GetItemText(const wxTreeItemId& item) const
    wxTreeItemId GetLastChild(const wxTreeItemId& item) const
    <wxver25|%compat24 wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const
    ~<wxver25 wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const
    wxTreeItemId GetNextVisible(const wxTreeItemId& item) const
    <wxver24 %rename GetItemParent wxTreeItemId GetParent(const wxTreeItemId& item) const
    ~<wxver24 wxTreeItemId GetItemParent(const wxTreeItemId& item) const
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const
    wxTreeItemId GetRootItem() const
    %property=RootItem, read
    <wxver25|%wxcompat24 int GetItemSelectedImage(const wxTreeItemId& item) const
    wxTreeItemId GetSelection() const
    %property=Selection, read
    size_t GetSelections(wxArrayTreeItemIds& selection) const
    wxImageList* GetStateImageList() const
    wxTreeItemId HitTest(const wxPoint& point, int& flags)
    wxTreeItemId InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL)
    %rename InsertItemBefore wxTreeItemId InsertItem(const wxTreeItemId& parent, size_t before, const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL)
    bool IsBold(const wxTreeItemId& item) const
    bool IsExpanded(const wxTreeItemId& item) const
    bool IsSelected(const wxTreeItemId& item) const
    bool IsVisible(const wxTreeItemId& item) const
    bool ItemHasChildren(const wxTreeItemId& item) const
    wxTreeItemId PrependItem(const wxTreeItemId& parent, const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL)
    void ScrollTo(const wxTreeItemId& item)
    void SetIndent(int indent)
    %property=Indent, read, write
    void SetImageList(wxImageList* imageList)
    %property=ImageList, read, write
    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col)
    void SetItemBold(const wxTreeItemId& item, bool bold = TRUE)
    void SetItemData(const wxTreeItemId& item, wxTreeItemData* data)
    void SetItemFont(const wxTreeItemId& item, const wxFont& font)
    void SetItemHasChildren(const wxTreeItemId& item, bool hasChildren = TRUE)
    void SetItemImage(const wxTreeItemId& item, int image, wxTreeItemIcon which = wxTreeItemIcon_Normal)
    <wxver25|%wxcompat24 void SetItemSelectedImage(const wxTreeItemId& item, int selImage)
    void SetItemText(const wxTreeItemId& item, const wxString& text)
    void SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
    void SetStateImageList(wxImageList* imageList)
    %property=StateImageList, read, write
    void SortChildren(const wxTreeItemId& item)
    void Toggle(const wxTreeItemId& item)
    void Unselect()
    void UnselectAll()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTreeItemAttr - wxTreeCtrl

%class %noclassinfo wxTreeItemAttr, wxObject
    wxTreeItemAttr(const wxColour& colText = wxNullColour, const wxColour& colBack = wxNullColour, const wxFont& font = wxNullFont)
    void    SetTextColour(const wxColour& colText)
    void    SetBackgroundColour(const wxColour& colBack)
    void    SetFont(const wxFont& font)
    bool    HasTextColour()
    bool    HasBackgroundColour()
    bool    HasFont()
    const wxColour& GetTextColour()
    %property=TextColour, read, write
    const wxColour& GetBackgroundColour()
    %property=BackgroundColour, read, write
    const wxFont& GetFont()
    %property=Font, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTreeItemIdValue - wxTreeCtrl

<wxver25 %skip
%class %noclassinfo wxTreeItemIdValue
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTreeItemId - wxTreeCtrl

%class %delete %noclassinfo %encapsulate wxTreeItemId
    wxTreeItemId()
    bool    IsOk()
    double  GetValue() const;
    void SetValue(double value);
    %property=Value, read, write);
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxArrayTreeItemIds - wxTreeCtrl

%class %noclassinfo wxArrayTreeItemIds
    void Clear()
    int  GetCount() const
    void Empty()
    bool IsEmpty() const
    void Shrink()
    void RemoveAt(size_t index)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTreeItemData - wxTreeCtrl

%class %noclassinfo wxTreeItemData, wxObject
    wxTreeItemData()
    const wxTreeItemId& GetId()
    void      SetId(const wxTreeItemId& id)
    %property=Id, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxLuaTreeItemData

%class %noclassinfo wxLuaTreeItemData, wxTreeItemData
    wxLuaTreeItemData(double value = 0);
    double  GetValue() const;
    void SetValue(double value);
    %property=Value, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTreeEvent - wxTreeCtrl

%class %delete wxTreeEvent, wxNotifyEvent
    %define %event wxEVT_COMMAND_TREE_BEGIN_DRAG
    %define %event wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT
    %define %event wxEVT_COMMAND_TREE_BEGIN_RDRAG
    %define %event wxEVT_COMMAND_TREE_DELETE_ITEM
    %define %event wxEVT_COMMAND_TREE_END_DRAG
    %define %event wxEVT_COMMAND_TREE_END_LABEL_EDIT
    %define %event wxEVT_COMMAND_TREE_GET_INFO
    %define %event wxEVT_COMMAND_TREE_ITEM_ACTIVATED
    %define %event wxEVT_COMMAND_TREE_ITEM_COLLAPSED
    %define %event wxEVT_COMMAND_TREE_ITEM_COLLAPSING
    %define %event wxEVT_COMMAND_TREE_ITEM_EXPANDED
    %define %event wxEVT_COMMAND_TREE_ITEM_EXPANDING
    %define %event wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK
    %define %event wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK
    %define %event wxEVT_COMMAND_TREE_KEY_DOWN
    %define %event wxEVT_COMMAND_TREE_SEL_CHANGED
    %define %event wxEVT_COMMAND_TREE_SEL_CHANGING
    %define %event wxEVT_COMMAND_TREE_SET_INFO

    wxTreeEvent(WXTYPE commandType = 0, int id = 0)
    wxTreeItemId GetItem() const
    %property=Item, read
    wxTreeItemId GetOldItem() const
    %property=OldItem, read
    wxPoint GetPoint() const
    %property=Point, read
    %wxver22 %rename GetKeyCode int GetCode() const
    ~%wxver22 int GetKeyCode() const
    %property=KeyCode, read
    const wxString& GetLabel() const
    %property=Label, read
%endclass

%endif wxLUA_USE_wxTreeCtrl
