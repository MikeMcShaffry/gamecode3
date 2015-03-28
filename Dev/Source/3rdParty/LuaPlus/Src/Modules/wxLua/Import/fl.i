/////////////////////////////////////////////////////////////////////////////
// Purpose:     "FL" Docking Library from contribs
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_FL

%define wxCBAR_DOCKED_HORIZONTALLY
%define wxCBAR_DOCKED_VERTICALLY
%define wxCBAR_FLOATING
%define wxCBAR_HIDDEN
%define MAX_BAR_STATES
%define FL_ALIGN_TOP
%define FL_ALIGN_BOTTOM
%define FL_ALIGN_LEFT
%define FL_ALIGN_RIGHT
%define FL_ALIGN_TOP_PANE
%define FL_ALIGN_BOTTOM_PANE
%define FL_ALIGN_LEFT_PANE
%define FL_ALIGN_RIGHT_PANE
%define wxALL_PANES

%enum CB_HITTEST_RESULT
    CB_NO_ITEMS_HITTED
    CB_UPPER_ROW_HANDLE_HITTED
    CB_LOWER_ROW_HANDLE_HITTED
    CB_LEFT_BAR_HANDLE_HITTED
    CB_RIGHT_BAR_HANDLE_HITTED
    CB_BAR_CONTENT_HITTED
%end

%class %delete wxFrameLayout, wxEvtHandler
    %constructor wxFrameLayoutDefault()
    wxFrameLayout(wxWindow* pParentFrame, wxWindow* pFrameClient = NULL, bool activateNow = TRUE)
    void EnableFloating(bool enable = TRUE)
    void Activate()
    void Deactivate()
    void HideBarWindows()
    void DestroyBarWindows()
    void SetFrameClient(wxWindow* pFrameClient)
    wxWindow* GetFrameClient()
    %property=FrameClient, read, write
    wxWindow& GetParentFrame()
    %property=ParentFrame, read
    //cbDockPane** GetPanesArray()
    cbDockPane* GetNthPane(int nth)
    cbDockPane* GetPane(int alignment)
    void AddBar( wxWindow* pBarWnd, const cbDimInfo& dimInfo, int alignment = FL_ALIGN_TOP, int rowNo = 0, int columnPos = 0, const wxString& name="bar", bool spyEvents = FALSE, int state = wxCBAR_DOCKED_HORIZONTALLY)
    bool RedockBar(cbBarInfo* pBar, const wxRect& shapeInParent, cbDockPane* pToPane = NULL, bool updateNow = TRUE)
    cbBarInfo* FindBarByName(const wxString& name)
    cbBarInfo* FindBarByWindow(const wxWindow* pWnd)
    //BarArrayT& GetBars();
    int GetNumBars()
    %property=NumBars, read
    cbBarInfo *GetNthBar(int nth)
    void SetBarState(cbBarInfo* pBar, int newStatem, bool updateNow)
    void InverseVisibility(cbBarInfo* pBar)
    void ApplyBarProperties(cbBarInfo* pBar)
    void RemoveBar(cbBarInfo* pBar)
    void RecalcLayout(bool repositionBarsNow = FALSE)
    int  GetClientHeight()
    %property=ClientHeight, read
    int  GetClientWidth()
    %property=ClientWidth, read
    wxRect GetClientRect()
    %property=ClientRect, read
    cbUpdatesManagerBase& GetUpdatesManager()
    void SetUpdatesManager(cbUpdatesManagerBase* pUMgr)
    void GetPaneProperties(cbCommonPaneProperties& props, int alignment = FL_ALIGN_TOP)
    void SetPaneProperties(const cbCommonPaneProperties& props, int paneMask = wxALL_PANES)
    void SetMargins(int top, int bottom, int left, int right, int paneMask = wxALL_PANES)
    void SetPaneBackground(const wxColour& colour)
    void RefreshNow(bool recalcLayout = TRUE)
    void FirePluginEvent(cbPluginEvent& event)
    void CaptureEventsForPlugin (cbPluginBase* pPlugin)
    void ReleaseEventsFromPlugin(cbPluginBase* pPlugin)
    void CaptureEventsForPane(cbDockPane* toPane)
    void ReleaseEventsFromPane(cbDockPane* fromPane)
    cbPluginBase& GetTopPlugin()
    void SetTopPlugin(cbPluginBase* pPlugin)
    %property=TopPlugin, read, write
    void PushPlugin(cbPluginBase* pPugin)
    void PopPlugin()
    void PopAllPlugins()
    void PushDefaultPlugins()
    void AddPlugin(wxClassInfo* pPlInfo, int paneMask = wxALL_PANES)
    void AddPluginBefore(wxClassInfo* pNextPlInfo, wxClassInfo* pPlInfo, int paneMask = wxALL_PANES)
    void RemovePlugin(wxClassInfo* pPlInfo)
    cbPluginBase* FindPlugin(wxClassInfo* pPlInfo)
    bool HasTopPlugin()
%endclass

%include "wx/fl/controlbar.h"

%class %noclassinfo cbBarSpy, wxEvtHandler
    %constructor cbBarSpyDefault()
    cbBarSpy( wxFrameLayout* pPanel)
    void SetBarWindow(wxWindow* pWnd)
%endclass

%class %noclassinfo cbUpdateMgrData, wxObject
    cbUpdateMgrData()
    void StoreItemState(const wxRect& boundsInParent)
    void SetDirty(bool isDirty = TRUE)
    void SetCustomData(wxObject* pCustomData)
    bool IsDirty()
%endclass

%class %noclassinfo cbBarDimHandlerBase, wxObject
    void AddRef();
    void RemoveRef();
%endclass

%class %noclassinfo %delete cbDimInfo, wxObject
    %constructor cbDimInfoDefault()
    %constructor cbDimInfoFixed(cbBarDimHandlerBase* pDimHandler, bool isFixed)
    cbDimInfo(int dh_x, int dh_y, int dv_x, int dv_y, int f_x, int f_y, bool isFixed = TRUE, int horizGap = 6, int vertGap = 6, cbBarDimHandlerBase* pDimHandler = NULL)
    %constructor cbDimInfoXY( int x, int y, bool isFixed = TRUE, int gap = 6, cbBarDimHandlerBase *pDimHandler = NULL)
    cbBarDimHandlerBase* GetDimHandler()
    %property=DimHandler, read
%endclass

%class %noclassinfo cbRowInfo, wxObject
    cbRowInfo()
    cbBarInfo* GetFirstBar()
    %property=FirstBar, read
%endclass

%class %noclassinfo cbBarInfo, wxObject
    cbBarInfo();
    bool IsFixed() const
    bool IsExpanded() const
%endclass

%class %noclassinfo cbBarShapeData, wxObject
%endclass

%class %noclassinfo wxBarIterator
    //wxBarIterator( RowArrayT& rows )
    void Reset()
    bool Next()
    cbBarInfo& BarInfo()
    cbRowInfo& RowInfo()
%endclass

%class %noclassinfo cbCommonPaneProperties, wxObject
    cbCommonPaneProperties()
%endclass

%class %delete %noclassinfo cbLuaCommonPaneProperties, cbCommonPaneProperties
    cbLuaCommonPaneProperties()
    void SetRealTimeUpdates(bool fOn = true)
    void SetOutOfPaneDrag(bool fOn = true)
    void SetExactDockPrediction(bool fOn = true)
    void SetNonDestructFriction(bool fOn = true)
    void SetShow3DPaneBorder(bool fOn = true)
%endclass

%class %noclassinfo cbDockPane, wxObject
    %constructor cbDockPaneDefault()
    cbDockPane(int alignment, wxFrameLayout* pPanel)
    void SetMargins(int top, int bottom, int left, int right)
    void RemoveBar(cbBarInfo* pBar)
    %rename InsertBarRect void InsertBar(cbBarInfo* pBar, const wxRect& rect)
    %rename InsertBarInto void InsertBar(cbBarInfo* pBar, cbRowInfo* pIntoRow)
    void InsertBar(cbBarInfo* pBarInfo)
    void RemoveRow(cbRowInfo* pRow)
    void InsertRow(cbRowInfo* pRow, cbRowInfo* pBeforeRow)
    void SetPaneWidth(int width)
    void SetBoundsInParent(const wxRect& rect)
    wxRect& GetRealRect()
    %property=RealRect, read
    //RowArrayT& GetRowList()
    wxBarIterator* GetRowList();
    %property=RowList, read
    cbRowInfo* GetFirstRow()
    %property=FirstRow, read
    bool BarPresent(cbBarInfo* pBar)
    int GetPaneHeight()
    %property=PaneHeight, read
    int GetAlignment()
    %property=Alignment, read
    bool MatchesMask(int paneMask)
    bool IsHorizontal()
    void RecalcLayout()
    int GetDockingState()
    %property=DockingState, read
    int HitTestPaneItems(const wxPoint& pos, cbRowInfo** ppRow, cbBarInfo** ppBar)
    void GetBarResizeRange(cbBarInfo* pBar, int* from, int *till, bool forLeftHandle)
    void GetRowResizeRange(cbRowInfo* pRow, int* from, int* till, bool forUpperHandle)
    cbBarInfo* GetBarInfoByWindow(wxWindow* pBarWnd)
%endclass

%class %noclassinfo cbUpdatesManagerBase, wxObject
    %define %event cbEVT_PL_LEFT_DOWN
    %define %event cbEVT_PL_LEFT_UP
    %define %event cbEVT_PL_RIGHT_DOWN
    %define %event cbEVT_PL_RIGHT_UP
    %define %event cbEVT_PL_MOTION
    %define %event cbEVT_PL_LEFT_DCLICK
    %define %event cbEVT_PL_LAYOUT_ROW
    %define %event cbEVT_PL_RESIZE_ROW
    %define %event cbEVT_PL_LAYOUT_ROWS
    %define %event cbEVT_PL_INSERT_BAR
    %define %event cbEVT_PL_RESIZE_BAR
    %define %event cbEVT_PL_REMOVE_BAR
    %define %event cbEVT_PL_SIZE_BAR_WND
    %define %event cbEVT_PL_DRAW_BAR_DECOR
    %define %event cbEVT_PL_DRAW_ROW_DECOR
    %define %event cbEVT_PL_DRAW_PANE_DECOR
    %define %event cbEVT_PL_DRAW_BAR_HANDLES
    %define %event cbEVT_PL_DRAW_ROW_HANDLES
    %define %event cbEVT_PL_DRAW_ROW_BKGROUND
    %define %event cbEVT_PL_DRAW_PANE_BKGROUND
    %define %event cbEVT_PL_START_BAR_DRAGGING
    %define %event cbEVT_PL_DRAW_HINT_RECT
    %define %event cbEVT_PL_START_DRAW_IN_AREA
    %define %event cbEVT_PL_FINISH_DRAW_IN_AREA
    %define %event cbEVT_PL_CUSTOMIZE_BAR
    %define %event cbEVT_PL_CUSTOMIZE_LAYOUT
    %define %event wxCUSTOM_CB_PLUGIN_EVENTS_START_AT

    void SetLayout( wxFrameLayout* pLayout )
%endclass

%class %noclassinfo cbPluginEvent, wxEvent
    cbPluginEvent(wxEventType eventType, cbDockPane* pPane)
%endclass

%class %noclassinfo cbPluginBase, wxEvtHandler
    %constructor cbPluginBaseDefault()
    cbPluginBase(wxFrameLayout* pPanel, int paneMask = wxALL_PANES)
    int GetPaneMask()
    %property=PaneMask, read
    void OnInitPlugin()
    bool IsReady()
%endclass

%class %noclassinfo cbLeftDownEvent, cbPluginEvent
    cbLeftDownEvent(const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbLeftUpEvent, cbPluginEvent
    cbLeftUpEvent(const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbRightDownEvent, cbPluginEvent
    cbRightDownEvent(const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbRightUpEvent, cbPluginEvent
    cbRightUpEvent(const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbMotionEvent, cbPluginEvent
    cbMotionEvent( const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbLeftDClickEvent, cbPluginEvent
    cbLeftDClickEvent(const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbLayoutRowEvent, cbPluginEvent
    cbLayoutRowEvent(cbRowInfo* pRow, cbDockPane* pPane)
%endclass

%class %noclassinfo cbResizeRowEvent, cbPluginEvent
    cbResizeRowEvent(cbRowInfo* pRow, int handleOfs, bool forUpperHandle, cbDockPane* pPane)
%endclass

%class %noclassinfo cbLayoutRowsEvent, cbPluginEvent
    cbLayoutRowsEvent(cbDockPane* pPane)
%endclass

%class %noclassinfo cbInsertBarEvent, cbPluginEvent
    cbInsertBarEvent(cbBarInfo* pBar, cbRowInfo* pIntoRow, cbDockPane* pPane)
%endclass

%class %noclassinfo cbResizeBarEvent, cbPluginEvent
    cbResizeBarEvent(cbBarInfo* pBar, cbRowInfo* pRow, cbDockPane* pPane)
%endclass

%class %noclassinfo cbRemoveBarEvent, cbPluginEvent
    cbRemoveBarEvent(cbBarInfo* pBar, cbDockPane* pPane)
%endclass

%class %noclassinfo cbSizeBarWndEvent, cbPluginEvent
    cbSizeBarWndEvent( cbBarInfo* pBar, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawBarDecorEvent, cbPluginEvent
    cbDrawBarDecorEvent(cbBarInfo* pBar, wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawRowDecorEvent, cbPluginEvent
    cbDrawRowDecorEvent(cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawPaneDecorEvent, cbPluginEvent
    cbDrawPaneDecorEvent(wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawBarHandlesEvent, cbPluginEvent
    cbDrawBarHandlesEvent(cbBarInfo* pBar, wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawRowHandlesEvent, cbPluginEvent
    cbDrawRowHandlesEvent(cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawRowBkGroundEvent, cbPluginEvent
    cbDrawRowBkGroundEvent(cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawPaneBkGroundEvent, cbPluginEvent
    cbDrawPaneBkGroundEvent(wxDC& dc, cbDockPane* pPane)
%endclass

%class %noclassinfo cbStartBarDraggingEvent, cbPluginEvent
    cbStartBarDraggingEvent(cbBarInfo* pBar, const wxPoint& pos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbDrawHintRectEvent, cbPluginEvent
    cbDrawHintRectEvent(const wxRect& rect, bool isInClient, bool eraseRect, bool lastTime)
%endclass

%class %noclassinfo cbStartDrawInAreaEvent, cbPluginEvent
    //cbStartDrawInAreaEvent(const wxRect& area, wxDC** ppDCForArea, cbDockPane* pPane)
%endclass

%class %noclassinfo cbFinishDrawInAreaEvent, cbPluginEvent
    cbFinishDrawInAreaEvent( const wxRect& area, cbDockPane* pPane );
%endclass

%class %noclassinfo cbCustomizeBarEvent, cbPluginEvent
    cbCustomizeBarEvent(cbBarInfo* pBar, const wxPoint& clickPos, cbDockPane* pPane)
%endclass

%class %noclassinfo cbCustomizeLayoutEvent, cbPluginEvent
    cbCustomizeLayoutEvent( const wxPoint& clickPos )
%endclass

%include "wx/fl/updatesmgr.h"
    %class cbSimpleUpdatesMgr, cbUpdatesManagerBase
    %constructor cbSimpleUpdatesMgrDefault()
    cbSimpleUpdatesMgr(wxFrameLayout* pPanel)
    virtual void UpdateNow()
%endclass

%include "wx/fl/gcupdatesmgr.h"
%class cbGCUpdatesMgr, cbSimpleUpdatesMgr
    %constructor cbGCUpdatesMgrDefault()
    cbGCUpdatesMgr(wxFrameLayout* pPanel)
%endclass

%include "wx/fl/barhintspl.h"
%class cbBarHintsPlugin, cbPluginBase
    %constructor cbBarHintsPluginDefault()
    cbBarHintsPlugin(wxFrameLayout* pLayout, int paneMask = wxALL_PANES)
    void SetGrooveCount(int nGrooves)
%endclass

%include "wx/fl/hintanimpl.h"
%class cbHintAnimationPlugin, cbPluginBase
    %constructor cbHintAnimationPluginDefault()
    cbHintAnimationPlugin(wxFrameLayout* pPanel, int paneMask = wxALL_PANES)
%endclass

%include "wx/fl/rowdragpl.h"
%class cbRowDragPlugin, cbPluginBase
    %constructor cbRowDragPluginDefault()
    cbRowDragPlugin(wxFrameLayout* pLayout, int paneMask = wxALL_PANES)
    virtual void OnInitPlugin()
    virtual void DrawCollapsedRowIcon( int index, wxDC& dc, bool isHighlighted )
    virtual void DrawCollapsedRowsBorder( wxDC& dc )
    virtual void DrawRowsDragHintsBorder( wxDC& dc )
    virtual void DrawRowDragHint( cbRowInfo* pRow, wxDC& dc, bool isHighlighted )
    virtual void DrawEmptyRow( wxDC& dc, wxRect& rowBounds )
    virtual int GetCollapsedRowIconHeight()
    virtual int GetRowDragHintWidth()
    virtual void SetPaneMargins()
    virtual bool HitTestCollapsedRowIcon( int iconIdx, const wxPoint& pos )
    virtual bool HitTestRowDragHint( cbRowInfo* pRow, const wxPoint& pos )
%endclass

%include "wx/fl/antiflickpl.h"
%class cbAntiflickerPlugin, cbPluginBase
    %constructor cbAntiflickerPluginDefault()
    cbAntiflickerPlugin(wxFrameLayout* pPanel, int paneMask = wxALL_PANES)
%endclass

%include "wx/fl/cbcustom.h"
%class cbSimpleCustomizationPlugin, cbPluginBase
    %constructor cbSimpleCustomizationPluginDefault()
    cbSimpleCustomizationPlugin(wxFrameLayout* pPanel, int paneMask = wxALL_PANES)
%endclass

%include "wx/fl/dyntbarhnd.h"
%class cbDynToolBarDimHandler, cbBarDimHandlerBase
    cbDynToolBarDimHandler()
%endclass

%include "wx/fl/dyntbar.h"
%class wxToolLayoutItem, wxObject
    wxToolLayoutItem()
%endclass

%class wxDynToolInfo, wxToolLayoutItem
    wxDynToolInfo()
%endclass

%define LO_HORIZONTAL
%define LO_VERTICAL
%define LO_FIT_TO_WINDOW

%class wxDynamicToolBar, wxToolBarBase
    %constructor wxDynamicToolBarDefault()
    wxDynamicToolBar(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const long style = wxNO_BORDER, const int orientation = wxVERTICAL, const int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr)
    bool Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const long style = wxNO_BORDER, const int orientation = wxVERTICAL, const int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr)
    %rename AddToolWindow virtual void AddTool( int toolIndex, wxWindow* pToolWindow, const wxSize& size = wxDefaultSize )
    %rename AddToolIndex virtual void AddTool(int toolIndex, const wxString& imageFileName, wxBitmapType imageFileType = wxBITMAP_TYPE_BMP, const wxString& labelText = "", bool alignTextRight = FALSE, bool isFlat = TRUE )
    virtual void AddTool( int toolIndex, const wxBitmap &labelBmp, const wxString& labelText = "", bool alignTextRight = FALSE, bool isFlat = TRUE )
    %rename AddToolCompatible virtual wxToolBarToolBase *AddTool(const int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap, const bool toggle = FALSE, const long xPos = -1, const long yPos = -1, wxObject *clientData = NULL, const wxString& helpString1 = "", const wxString& helpString2 = "")
    virtual void AddSeparator( wxWindow* pSeparatorWnd = NULL )
    wxDynToolInfo* GetToolInfo( int toolIndex )
    void RemveTool( int toolIndex )
    virtual void DrawSeparator( wxDynToolInfo& info, wxDC& dc )
    virtual bool Layout()
    virtual void GetPreferredDim( const wxSize& givenDim, wxSize& prefDim )
    //virtual LayoutManagerBase* CreateDefaultLayout()
    //virtual void SetLayout( LayoutManagerBase* pLayout )
    virtual void EnableTool(const int toolIndex, const bool enable = TRUE)
    virtual wxToolBarToolBase* FindToolForPosition(wxCoord x, wxCoord y) const
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool)
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool)
    virtual void DoEnableTool(wxToolBarToolBase* tool, bool enable)
    virtual void DoToggleTool(wxToolBarToolBase* tool, bool toggle)
    virtual void DoSetToggle(wxToolBarToolBase* tool, bool toggle)
    virtual wxToolBarToolBase* CreateTool(int id, const wxString& label, const wxBitmap& bmpNormal, const wxBitmap& bmpDisabled, wxItemKind kind, wxObject *clientData, const wxString& shortHelp, const wxString& longHelp)
    %rename CreateToolWindow virtual wxToolBarToolBase* CreateTool(wxControl *control)
%endclass

%endif wxLUA_USE_FL
