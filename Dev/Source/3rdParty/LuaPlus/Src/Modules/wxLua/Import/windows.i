/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxWindow and other container type windows
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// wxWindow

%define wxSIMPLE_BORDER
%define wxDOUBLE_BORDER
%define wxSUNKEN_BORDER
%define wxRAISED_BORDER
%define wxSTATIC_BORDER
%define wxTRANSPARENT_WINDOW
%define wxNO_3D
%define wxTAB_TRAVERSAL
%define wxWANTS_CHARS
%define wxNO_FULL_REPAINT_ON_RESIZE
%define wxVSCROLL
%define wxHSCROLL
%define wxCLIP_CHILDREN

%mac|%x11|%motif %typedef WXWidget void*
%gtk %typedef WXWidget unsigned long // GtkWidget* what could you do with it?
%mgl %typedef WXWidget  window_t
%msw|%os2 %typedef WXWidget  unsigned long

%class wxWindow, wxEvtHandler
    wxWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr)
    virtual void AddChild(wxWindow* child)
    virtual void CaptureMouse()
    void Center(int direction = wxBOTH)
    void CenterOnParent(int direction = wxBOTH)
    void CenterOnScreen(int direction = wxBOTH)
    void Centre(int direction = wxBOTH)
    void CentreOnParent(int direction = wxBOTH)
    void CentreOnScreen(int direction = wxBOTH)
    <wxver25 void Clear()
    ~<wxver25 void ClearBackground()
    virtual void ClientToScreen(int* x, int* y) const
    virtual bool Close(bool force = FALSE)
    %rename ConvertDialogPointToPixels wxPoint ConvertPixelsToDialog(const wxPoint& pt)
    %rename ConvertDialogSizeToPixels wxSize ConvertPixelsToDialog(const wxSize& sz)
    virtual bool Destroy()
    virtual void DestroyChildren()
    %win virtual void DragAcceptFiles(bool accept)
    virtual void Enable(bool enable)
    static wxWindow* FindFocus()
    wxWindow* FindWindow(long id)
    virtual void Fit()
    virtual wxColour GetBackgroundColour() const
    virtual wxSize GetBestSize() const
    %property=BestSize, read
    wxCaret * GetCaret() const
    virtual int GetCharHeight() const
    %property=CharHeight, read
    virtual int GetCharWidth() const
    %property=CharWidth, read
    wxWindowList& GetChildren()
    %property=Children, read
    //virtual wxSize GetClientSize() const
    virtual void GetClientSize(int* width, int* height) const
    wxLayoutConstraints* GetConstraints() const
    wxDropTarget* GetDropTarget() const
    wxEvtHandler* GetEventHandler() const
    long GetExtraStyle() const
    wxFont GetFont() const
    virtual wxColour GetForegroundColour()
    wxWindow* GetGrandParent() const
    %property=GrandParent, read
    WXWidget GetHandle() const
    int GetId() const
    virtual wxString GetLabel() const
    virtual wxString GetName() const
    virtual wxWindow* GetParent() const
    virtual void GetPosition(int* x, int* y) const
    %rename GetPositionPoint wxPoint GetPosition() const
    virtual wxRect GetRect() const
    virtual int GetScrollThumb(int orientation)
    virtual int GetScrollPos(int orientation)
    virtual int GetScrollRange(int orientation)
    virtual void GetSize(int* width, int* height) const
    // virtual void GetTextExtent(const wxString& string, int* x, int* y, int* descent = NULL, int* externalLeading = NULL, const wxFont* font = NULL, bool use16 = FALSE) const
    virtual void GetTextExtent(const wxString& string, int* x, int* y, int* descent = NULL, int* externalLeading = NULL, const wxFont* font = NULL ) const
    virtual wxString GetTitle()
    virtual wxRegion GetUpdateRegion() const
    wxValidator* GetValidator() const
    long GetWindowStyleFlag() const
    ~<wxver23 bool HasCapture() const
    void InitDialog()
    virtual bool IsEnabled() const
    bool IsExposed(int x, int y) const
    %rename IsExposedPoint bool IsExposed(wxPoint &pt) const
    // bool IsExposed(int x, int y, int w, int h) const
    %rename IsExposedRect bool IsExposed(wxRect &rect) const
    virtual bool IsRetained() const
    virtual bool IsShown() const
    bool IsTopLevel() const
    void Layout()
    void Lower()
    virtual void MakeModal(bool flag)
    void Move(int x, int y)
    %rename MoveXY void Move(const wxPoint& pt)
    wxEvtHandler* PopEventHandler(bool deleteHandler = FALSE) const
    bool PopupMenu(wxMenu* menu, const wxPoint& pos)
    %rename PopupMenuXY bool PopupMenu(wxMenu* menu, int x, int y)
    void PushEventHandler(wxEvtHandler* handler)
    void Raise()
    virtual void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL)
    virtual void ReleaseMouse()
    virtual void RemoveChild(wxWindow* child)
    virtual bool Reparent(wxWindow* newParent)
    virtual wxPoint ScreenToClient(const wxPoint& pt) const
    %rename ScreenToClientXY virtual void ScreenToClient(int* x, int* y) const
    virtual void ScrollWindow(int dx, int dy, const wxRect* rect = NULL)
    virtual void SetAcceleratorTable(const wxAcceleratorTable& accel)
    void SetAutoLayout(bool autoLayout)
    virtual void SetBackgroundColour(const wxColour& colour)
    void SetCaret(wxCaret *caret) const
    %property=Caret, read, write
    virtual void SetClientSize(const wxSize& size)
    %property=ClientSize, read, write
    %rename SetClientSizeWH virtual void SetClientSize(int width, int height)
    virtual void SetCursor(const wxCursor&cursor)
    void SetConstraints(wxLayoutConstraints* constraints)
    %property=Constraints, read, write
    void SetDropTarget(wxDropTarget* target)
    %property=DropTarget, read, write
    void SetEventHandler(wxEvtHandler* handler)
    %property=EventHandler, read, write
    void SetExtraStyle(long exStyle)
    %property=ExtraStyle, read, write
    virtual void SetFocus()
    void SetFont(const wxFont& font)
    %property=Font, read, write
    virtual void SetForegroundColour(const wxColour& colour)
    %property=ForegroundColour, read, write
    void SetId(int id)
    %property=Id, read, write
    virtual void SetName(const wxString& name)
    %property=Name, read, write
    // virtual void SetPalette(wxPalette* palette)
    virtual void SetScrollbar(int orientation, int position, int thumbSize, int range, bool refresh = TRUE)
    virtual void SetScrollPos(int orientation, int pos, bool refresh = TRUE)
    %rename SetDimensions virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO)
    virtual void SetSize(int width, int height)
    %rename SetPosition virtual void SetSize(const wxSize& size)
    virtual void SetSizeHints(int minW = -1, int minH = -1, int maxW = -1, int maxH = -1, int incW = -1, int incH = -1)
    void SetSizer(wxSizer* sizer)
    virtual void SetTitle(const wxString& title)
    %property=Title, read, write
    virtual void SetValidator(const wxValidator& validator)
    %property=Validator, read, write
    void SetToolTip(const wxString& tip)
    wxToolTip* GetToolTip() const
    %property=ToolTip, read, write
    void SetWindowStyle(long style)
    virtual void SetWindowStyleFlag(long style)
    %property=WindowStyleFlag, read, write
    virtual bool Show(bool show)
    virtual bool TransferDataFromWindow()
    virtual bool TransferDataToWindow()
    virtual bool Validate()
    void WarpPointer(int x, int y)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxWindowList

%if wxLUA_USE_wxWindowList

%class %noclassinfo wxWindowList
    void Clear()
    void DeleteContents(bool destroy)
    bool DeleteObject(wxWindow *pMenuItem)
    int  GetCount() const
    %property=Count, read
%endclass

%endif wxLUA_USE_wxWindowList

///////////////////////////////////////////////////////////////////////////////
// wxPanel

%class wxPanel, wxWindow
    wxPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel")
    %constructor wxPanelDefault()
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel")
    wxWindow* GetDefaultItem() const
    void InitDialog()
    void SetDefaultItem(wxButton *btn)
    %property=DefaultItem, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxControl

%include "wx/control.h"

%class wxControl, wxWindow
    void     Command(wxCommandEvent& event)
    wxString GetLabel()
    void     SetLabel(const wxString& label)
    %property=Label, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxNotebook

%if wxLUA_USE_wxNotebook

%include "wx/notebook.h"

%define wxNB_FIXEDWIDTH
~%wxver22 %define wxNB_TOP
%define wxNB_LEFT
%define wxNB_RIGHT
%define wxNB_BOTTOM
~%wxver22 %define wxNB_MULTILINE

%typedef wxNotebookPage wxWindow

%class wxNotebook, wxControl
    wxNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook")
    bool AddPage(wxNotebookPage* page, const wxString& text, bool select = FALSE, int imageId = -1)
    void AdvanceSelection(bool forward = TRUE)
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook")
    bool DeleteAllPages()
    bool DeletePage(int page)
    wxImageList* GetImageList() const
    wxNotebookPage* GetPage(int page)
    int GetPageCount() const
    %property=PageCount, read
    int GetPageImage(int nPage) const
    wxString GetPageText(int nPage) const
    int GetRowCount() const
    %property=RowCount, read
    int GetSelection() const
    bool InsertPage(int index, wxNotebookPage* page, const wxString& text, bool select = FALSE, int imageId = -1)
    bool RemovePage(int page)
    void SetImageList(wxImageList* imageList)
    %property=ImageList, read, write
    void SetPadding(const wxSize& padding)
    void SetPageSize(const wxSize& size)
    bool SetPageImage(int page, int image)
    bool SetPageText(int page, const wxString& text)
    int SetSelection(int page)
    %property=Selection, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxNotebookEvent

%class %delete wxNotebookEvent, wxNotifyEvent
    %define %event wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
    %define %event wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING

    wxNotebookEvent(wxEventType eventType = wxEVT_NULL, int id = 0, int sel = -1, int oldSel = -1)
    int GetOldSelection() const
    int GetSelection() const
    void SetOldSelection(int page)
    void SetSelection(int page)
    %property=OldSelection, read, write
    %property=Selection, read, write
%endclass

%endif wxLUA_USE_wxNotebook

///////////////////////////////////////////////////////////////////////////////
// wxTabCtrl

%if wxLUA_USE_wxTabCtrl

%include "wx/tabctrl.h"

%gtk %skip 
%class wxTabCtrl, wxControl
    %define wxTC_RIGHTJUSTIFY
    %define wxTC_FIXEDWIDTH
    ~%wxver22 %define wxTC_TOP
    ~%wxver22 %define wxTC_LEFT
    ~%wxver22 %define wxTC_RIGHT
    ~%wxver22 %define wxTC_BOTTOM
    %define wxTC_MULTILINE
    %define wxTC_OWNERDRAW

    %include "wx/tabctrl.h"
    wxTabCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "tabCtrl")
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "tabCtrl")
    bool DeleteAllItems()
    bool DeleteItem(int item)
    int GetCurFocus() const
    %property=CurFocus, read
    wxImageList* GetImageList() const
    int GetItemCount() const
    %property=ItemCount, read
    wxObject * GetItemData(int item) const
    int GetItemImage(int item) const
    bool GetItemRect(int item, wxRect& rect) const
    wxString GetItemText(int item) const
    int GetRowCount() const
    %property=RowCount, read
    int GetSelection() const
    int HitTest(const wxPoint& pt, long& flags)
    void InsertItem(int item, const wxString& text, int imageId = -1, wxObject *clientData = NULL)
    bool SetItemData(int item, wxObject * data)
    bool SetItemImage(int item, int image)
    void SetImageList(wxImageList* imageList)
    %property=ImageList, read, write
    void SetItemSize(const wxSize& size)
    bool SetItemText(int item, const wxString& text)
    void SetPadding(const wxSize& padding)
    int SetSelection(int item)
    %property=Selection, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTabEvent

%gtk %skip
%class %delete wxTabEvent, wxCommandEvent
    %win %define %event wxEVT_COMMAND_TAB_SEL_CHANGED
    %win %define %event wxEVT_COMMAND_TAB_SEL_CHANGING

    wxTabEvent(WXTYPE commandType = 0, int id = 0)
%endclass

%endif wxLUA_USE_wxTabCtrl

///////////////////////////////////////////////////////////////////////////////
// wxSashWindow

%if wxLUA_USE_wxSashWindow

%include "wx/sashwin.h"

%define wxSW_3D
%define wxSW_3DSASH
%define wxSW_3DBORDER
%define wxSW_BORDER

%enum wxSashEdgePosition
    wxSASH_TOP
    wxSASH_RIGHT
    wxSASH_BOTTOM
    wxSASH_LEFT
    wxSASH_NONE
%end

%enum wxSashDragStatus
    wxSASH_STATUS_OK
    wxSASH_STATUS_OUT_OF_RANGE
%end

%class wxSashWindow, wxWindow
    wxSashWindow(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSW_3D|wxCLIP_CHILDREN, const wxString& name = "sashWindow")
    bool GetSashVisible(wxSashEdgePosition edge) const
    int GetMaximumSizeX() const
    int GetMaximumSizeY() const
    int GetMinimumSizeX()
    // int GetMinimumSizeY(int min) const
    int GetMinimumSizeY() const
    bool HasBorder(wxSashEdgePosition edge) const
    void SetMaximumSizeX(int min)
    %property=MaximumSizeX, read, write
    void SetMaximumSizeY(int min)
    %property=MaximumSizeY, read, write
    void SetMinimumSizeX(int min)
    %property=MinimumSizeX, read, write
    void SetMinimumSizeY(int min)
    %property=MinimumSizeY, read, write
    void SetSashVisible(wxSashEdgePosition edge, bool visible)
    void SetSashBorder(wxSashEdgePosition edge, bool hasBorder)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSashLayoutWindow

%include "wx/laywin.h"

%enum wxLayoutAlignment
    wxLAYOUT_NONE
    wxLAYOUT_TOP
    wxLAYOUT_LEFT
    wxLAYOUT_RIGHT
    wxLAYOUT_BOTTOM
%end

%enum wxLayoutOrientation
    wxLAYOUT_HORIZONTAL
    wxLAYOUT_VERTICAL
%end

%class wxSashLayoutWindow, wxSashWindow
    wxSashLayoutWindow(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSW_3D|wxCLIP_CHILDREN, const wxString& name = "layoutWindow")
    wxLayoutAlignment GetAlignment() const
    wxLayoutOrientation GetOrientation() const
    void SetAlignment(wxLayoutAlignment alignment)
    %property=Alignment, read, write
    void SetDefaultSize(const wxSize& size)
    void SetOrientation(wxLayoutOrientation orientation)
    %property=Orientation, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxLayoutAlgorithm - for wxSashLayoutWindow

%include "wx/laywin.h"

%class %noclassinfo wxLayoutAlgorithm, wxObject
    wxLayoutAlgorithm()
%if wxLUA_USE_MDI
    bool LayoutMDIFrame(wxMDIParentFrame* frame, wxRect* rect = NULL)
%endif wxLUA_USE_MDI
    bool LayoutWindow(wxWindow* frame, wxWindow* mainWindow = NULL)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxQueryLayoutInfoEvent - for wxSashLayoutWindow

%include "wx/laywin.h"

%class %delete wxQueryLayoutInfoEvent, wxEvent
    %define %event wxEVT_QUERY_LAYOUT_INFO
    %define %event wxEVT_CALCULATE_LAYOUT

    wxQueryLayoutInfoEvent(wxWindowID id = 0)
    int GetFlags() const
    wxLayoutOrientation GetOrientation() const
    int GetRequestedLength() const
    wxSize GetSize() const
    void SetAlignment(wxLayoutAlignment alignment)
    void SetFlags(int flags)
    %property=Flags, read, write
    void SetOrientation(wxLayoutOrientation orientation)
    %property=Orientation, read, write
    void SetRequestedLength(int length)
    %property=RequestedLength, read, write
    void SetSize(const wxSize& size)
    %property=Size, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCalculateLayoutEvent - for wxSashLayoutWindow

%include "wx/laywin.h"

%class %delete wxCalculateLayoutEvent, wxEvent
    %define %event wxEVT_CALCULATE_LAYOUT

    wxCalculateLayoutEvent(wxWindowID id = 0)
    int GetFlags() const
    wxRect GetRect() const
    void SetFlags(int flags)
    %property=Flags, read, write
    void SetRect(const wxRect& rect)
    %property=Rect, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSashEvent

%class %delete wxSashEvent, wxCommandEvent
    %define %event wxEVT_SASH_DRAGGED

    wxSashEvent(int id = 0, wxSashEdgePosition edge = wxSASH_NONE)
    void SetEdge(wxSashEdgePosition edge)
    int GetEdge()
    %property=Edge, read, write
    void SetDragRect(const wxRect& rect)
    wxRect GetDragRect()
    %property=DragRect, read, write
    void SetDragStatus(wxSashDragStatus status)
    int GetDragStatus()
    %property=DragStatus, read, write
%endclass

%endif wxLUA_USE_wxSashWindow

///////////////////////////////////////////////////////////////////////////////
// wxScrolledWindow

%if wxLUA_USE_wxScrolledWindow

%class wxScrolledWindow, wxPanel
    wxScrolledWindow(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = "scrolledWindow")
    void CalcScrolledPosition( int x, int y, int *xx, int *yy) const
    void CalcUnscrolledPosition( int x, int y, int *xx, int *yy) const
    bool Create(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = "scrolledWindow")
    void EnableScrolling(const bool xScrolling, const bool yScrolling)
    void GetScrollPixelsPerUnit(int* xUnit, int* yUnit) const
    void GetVirtualSize(int* x, int* y) const
    bool IsRetained() const
    void PrepareDC(wxDC& dc)
    void Scroll(int x, int y)
    void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY, int noUnitsX, int noUnitsY, int xPos = 0, int yPos = 0, bool noRefresh = FALSE)
    void SetTargetWindow(wxWindow* window)
    void GetViewStart(int* x, int* y) const
%endclass

%endif wxLUA_USE_wxScrolledWindow

///////////////////////////////////////////////////////////////////////////////
// wxSplitterWindow

%if wxLUA_USE_wxSplitterWindow

%include "wx/splitter.h"

%define wxSP_NOBORDER
%define wxSP_NOSASH
%define wxSP_BORDER
%define wxSP_PERMIT_UNSPLIT
%define wxSP_LIVE_UPDATE
%define wxSP_3DSASH
%define wxSP_3DBORDER
%define wxSP_FULLSASH
%define wxSP_3D
~%wxver22 %define wxSP_SASH_AQUA

%class wxSplitterWindow, wxWindow
    wxSplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style=wxSP_3D, const wxString& name = "splitterWindow")
    %constructor wxSplitterWindowDefault()
    bool     Create(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_3D|wxCLIP_CHILDREN, const wxString& name = "splitter")
    int      GetMinimumPaneSize() const
    int      GetSashPosition()
    int      GetSplitMode() const
    wxWindow *GetWindow1() const
    %property=Window1, read
    wxWindow *GetWindow2() const
    %property=Window2, read
    void     Initialize(wxWindow* window)
    bool     IsSplit() const
    bool     ReplaceWindow(wxWindow * winOld, wxWindow * winNew)
    void     SetSashPosition(int position, const bool redraw = TRUE)
    %property=SashPosition, read, write
    void     SetMinimumPaneSize(int paneSize)
    %property=MinimumPaneSize, read, write
    void     SetSplitMode(int mode)
    %property=SplitMode, read, write
    bool     SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition = 0)
    bool     SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition = 0)
    bool     Unsplit(wxWindow* toRemove = NULL)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSplitterEvent

%class %delete wxSplitterEvent, wxCommandEvent
    %define %event wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING
    %define %event wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED
    %define %event wxEVT_COMMAND_SPLITTER_DOUBLECLICKED
    %define %event wxEVT_COMMAND_SPLITTER_UNSPLIT

    wxSplitterEvent(wxEventType type = wxEVT_NULL, wxSplitterWindow *splitter = NULL)
    int GetSashPosition()
    int GetX()
    %property=X, read
    int GetY()
    %property=Y, read
    wxWindow* GetWindowBeingRemoved()
    %property=WindowBeingRemoved, read
    void SetSashPosition(int pos)
    %property=SashPosition, read, write
%endclass

%endif wxLUA_USE_wxSplitterWindow

///////////////////////////////////////////////////////////////////////////////
// wxStaticBox

%if wxLUA_USE_wxStaticBox

%include "wx/statbox.h"

%class wxStaticBox, wxControl
    wxStaticBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticBox")
    bool     Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticBox")
%endclass

%endif wxLUA_USE_wxStaticBox

///////////////////////////////////////////////////////////////////////////////
// wxStaticBitmap

%if wxLUA_USE_wxStaticBitmap

%include "wx/statbmp.h"

%class wxStaticBitmap, wxControl
    wxStaticBitmap(wxWindow* parent, wxWindowID id, const wxBitmap& label = wxNullBitmap, const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticBitmap")
    bool Create(wxWindow* parent, wxWindowID id, const wxBitmap& label = wxNullBitmap, const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticBitmap")
    wxBitmap GetBitmap() const
    virtual void SetBitmap(const wxBitmap& label)
    %property=Bitmap, read, write
%endclass

%endif wxLUA_USE_wxStaticBitmap

///////////////////////////////////////////////////////////////////////////////
// wxStaticText

%if wxLUA_USE_wxStaticText

%include "wx/stattext.h"

%define wxALIGN_LEFT
%define wxALIGN_RIGHT
%define wxALIGN_CENTRE
%define wxST_NO_AUTORESIZE

%class wxStaticText, wxControl
    wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticText")
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticText")
    wxString GetLabel() const
    virtual void SetLabel(const wxString& label)
    %property=Label, read, write
%endclass

%endif wxLUA_USE_wxStaticText

///////////////////////////////////////////////////////////////////////////////
// wxStaticLine

%if wxLUA_USE_wxStaticLine

%include "wx/statline.h"

%define wxLI_HORIZONTAL
%define wxLI_VERTICAL

%class wxStaticLine, wxControl
    wxStaticLine(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLI_HORIZONTAL, const wxString& name = "staticLine")
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticLine")
    bool IsVertical() const
    static int GetDefaultSize()
    %property=DefaultSize, read
%endclass

%endif wxLUA_USE_wxStaticLine

