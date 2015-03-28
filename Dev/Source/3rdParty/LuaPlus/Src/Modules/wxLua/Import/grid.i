/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxGrid and related classes
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxGrid

%include "wx/grid.h"

~%wxver22 %enum wxGridCellAttr::wxAttrKind
~%wxver22     Any
~%wxver22     Default
~%wxver22     Cell
~%wxver22     Row
~%wxver22     Col
~%wxver22     Merged
~%wxver22 %end

%enum wxGrid::wxGridSelectionModes
    wxGridSelectCells
    wxGridSelectRows
    wxGridSelectColumns
%end

%define wxGRIDTABLE_REQUEST_VIEW_GET_VALUES
%define wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES
%define wxGRIDTABLE_NOTIFY_ROWS_INSERTED
%define wxGRIDTABLE_NOTIFY_ROWS_APPENDED
%define wxGRIDTABLE_NOTIFY_ROWS_DELETED
%define wxGRIDTABLE_NOTIFY_COLS_INSERTED
%define wxGRIDTABLE_NOTIFY_COLS_APPENDED
%define wxGRIDTABLE_NOTIFY_COLS_DELETED

%define %string wxGRID_VALUE_STRING 
%define %string wxGRID_VALUE_BOOL   
%define %string wxGRID_VALUE_NUMBER 
%define %string wxGRID_VALUE_FLOAT  
%define %string wxGRID_VALUE_CHOICE 
%define %string wxGRID_VALUE_TEXT   
%define %string wxGRID_VALUE_LONG   

///////////////////////////////////////////////////////////////////////////////
// wxGridCellRenderer

%class %noclassinfo wxGridCellRenderer
//wxGridCellRenderer()
//void SetParameters(const wxString& params)
//void IncRef()
//void DecRef()
//virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
//virtual wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellStringRenderer

%class %noclassinfo wxGridCellStringRenderer, wxGridCellRenderer
    wxGridCellStringRenderer()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellNumberRenderer

%class %noclassinfo wxGridCellNumberRenderer, wxGridCellStringRenderer
    wxGridCellNumberRenderer()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellFloatRenderer

%class %noclassinfo wxGridCellFloatRenderer, wxGridCellStringRenderer
    wxGridCellFloatRenderer(int width = -1, int precision = -1)
    int GetWidth() const
    void SetWidth(int width)
    %property=Width, read, write
    int GetPrecision() const
    void SetPrecision(int precision)
    %property=Precision, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellBoolRenderer

%class %noclassinfo wxGridCellBoolRenderer, wxGridCellRenderer
    wxGridCellBoolRenderer()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellDateTimeRenderer

//%class %noclassinfo wxGridCellDateTimeRenderer, wxGridCellStringRenderer
//  wxGridCellDateTimeRenderer(wxString outformat = "%c", wxString informat =  "%c")
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellEnumRenderer

//%class %noclassinfo wxGridCellEnumRenderer, wxGridCellStringRenderer
//  wxGridCellEnumRenderer( const wxString& choices = "" )
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellAutoWrapStringRenderer

//%class %noclassinfo wxGridCellAutoWrapStringRenderer, wxGridCellStringRenderer
//  wxGridCellAutoWrapStringRenderer()
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellAttr

%class %noclassinfo wxGridCellAttr
    wxGridCellAttr()
    void IncRef()
    void DecRef()
    void SetTextColour(const wxColour& colText)
    void SetBackgroundColour(const wxColour& colBack)
    void SetFont(const wxFont& font)
    void SetAlignment(int hAlign, int vAlign)
    void SetReadOnly(bool isReadOnly = TRUE)
    void SetRenderer(wxGridCellRenderer *renderer)
    void SetEditor(wxGridCellEditor* editor)
    bool HasTextColour() const
    bool HasBackgroundColour() const
    bool HasFont() const
    bool HasAlignment() const
    bool HasRenderer() const
    bool HasEditor() const
    wxColour GetTextColour() const
    %property=TextColour, read, write
    wxColour GetBackgroundColour() const
    %property=BackgroundColour, read, write
    wxFont GetFont() const
    %property=Font, read, write
    void GetAlignment(int *horz, int *vert) const
    wxGridCellRenderer *GetRenderer(wxGrid* grid, int row, int col) const
    wxGridCellEditor *GetEditor(wxGrid* grid, int row, int col) const
    bool IsReadOnly() const
    void SetDefAttr(wxGridCellAttr* defAttr)
    ~%wxver22 void SetKind(wxGridCellAttr::wxAttrKind kind)
    ~%wxver22 bool HasReadWriteMode() const
    ~%wxver22 void MergeWith(wxGridCellAttr *mergefrom)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellAttrProvider

%class %noclassinfo wxGridCellAttrProvider
    wxGridCellAttrProvider()
    ~%wxver22 wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind) const
    void SetAttr(wxGridCellAttr *attr, int row, int col)
    void SetRowAttr(wxGridCellAttr *attr, int row)
    void SetColAttr(wxGridCellAttr *attr, int col)
    void UpdateAttrRows( size_t pos, int numRows )
    void UpdateAttrCols( size_t pos, int numCols )
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridTableBase

%class wxGridTableBase, wxObject
    void SetAttrProvider(wxGridCellAttrProvider *attrProvider)
    int GetNumberRows()
    %property=NumberRows, read
    int GetNumberCols()
    %property=NumberCols, read
    bool IsEmptyCell( int row, int col )
    wxString GetValue( int row, int col )
    void SetValue( int row, int col, const wxString& value )
    wxString GetTypeName( int row, int col )
    wxGridCellAttrProvider *GetAttrProvider() const
    bool CanGetValueAs( int row, int col, const wxString& typeName )
    bool CanSetValueAs( int row, int col, const wxString& typeName )
    bool GetValueAsBool( int row, int col )
    long GetValueAsLong( int row, int col )
    double GetValueAsDouble( int row, int col )
    void SetValueAsBool( int row, int col, bool value )
    void SetValueAsLong( int row, int col, long value )
    void SetValueAsDouble( int row, int col, double value )
    void SetView( wxGrid *grid )
    wxGrid * GetView() const
    %property=View, read, write
    void Clear()
    bool InsertRows( size_t pos = 0, size_t numRows = 1 )
    bool AppendRows( size_t numRows = 1 )
    bool DeleteRows( size_t pos = 0, size_t numRows = 1 )
    bool InsertCols( size_t pos = 0, size_t numCols = 1 )
    bool AppendCols( size_t numCols = 1 )
    bool DeleteCols( size_t pos = 0, size_t numCols = 1 )
    wxString GetRowLabelValue( int row )
    wxString GetColLabelValue( int col )
    void SetRowLabelValue( int row, const wxString& value )
    void SetColLabelValue( int col, const wxString& value )
    ~%wxver22 wxGridCellAttr * GetAttr( int row, int col, wxGridCellAttr::wxAttrKind  kind)
    void SetAttr(wxGridCellAttr* attr, int row, int col)
    void SetRowAttr(wxGridCellAttr *attr, int row)
    void SetColAttr(wxGridCellAttr *attr, int col)
    bool CanHaveAttributes()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridStringTable

%class wxGridStringTable, wxGridTableBase
    wxGridStringTable( int numRows=0, int numCols=0 )
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridTableMessage

%class %noclassinfo wxGridTableMessage
    wxGridTableMessage( wxGridTableBase *table, int id, int comInt1 = -1, int comInt2 = -1 )
    void SetTableObject( wxGridTableBase *table )
    wxGridTableBase * GetTableObject() const
    %property=TableObject, read, write
    void SetId( int id )
    int  GetId()
    %property=Id, read, write
    void SetCommandInt( int comInt1 )
    int  GetCommandInt()
    %property=CommandInt, read, write
    void SetCommandInt2( int comInt2 )
    int  GetCommandInt2()
    %property=CommandInt2, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellCoords

%class %noclassinfo wxGridCellCoords
    %define %object wxGridNoCellRect

    wxGridCellCoords( int r = -1, int c = -1 )
    int GetRow() const
    void SetRow( int n )
    %property=Row, read, write
    int GetCol() const
    void SetCol( int n )
    %property=Col, read, write
    void Set(int row, int col)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGrid

%class wxGrid, wxScrolledWindow
    wxGrid( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString &name = "grid" )
    bool    CreateGrid( int numRows, int numCols, wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells )
    bool    InsertRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE )
    bool    AppendRows( int numRows = 1, bool updateLabels=TRUE )
    bool    DeleteRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE )
    bool    InsertCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE )
    bool    AppendCols( int numCols = 1, bool updateLabels=TRUE )
    bool    DeleteCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE )
    void    SetSelectionMode(wxGrid::wxGridSelectionModes selmode)
    void    AutoSize()
    void    AutoSizeRow( int row, bool setAsMin = TRUE )
    void    AutoSizeColumn( int col, bool setAsMin = TRUE )
    void    AutoSizeRows( bool setAsMin = TRUE )
    void    AutoSizeColumns( bool setAsMin = TRUE )
    void    BeginBatch()
    void    EndBatch()
    bool    CanDragRowSize()
    void    EnableDragRowSize( bool enable = TRUE )
    void    DisableDragRowSize()
    bool    CanDragColSize()
    void    EnableDragColSize( bool enable = TRUE )
    void    DisableDragColSize()
    bool    CanDragGridSize()
    void    EnableDragGridSize(bool enable = TRUE)
    void    DisableDragGridSize()
    int     GetNumberRows()
    %property=NumberRows, read
    int     GetNumberCols()
    %property=NumberCols, read
    bool    ProcessTableMessage( wxGridTableMessage& msg)
    wxGridTableBase * GetTable() const
    bool    SetTable( wxGridTableBase * table, bool takeOwnership = FALSE, wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells )
    void    ClearGrid()
    void    DrawCellHighlight( wxDC& dc, const wxGridCellAttr * attr )
    void    DrawTextRectangle( wxDC& dc, const wxString& text, const wxRect& rect, int horizontalAlignment = wxLEFT, int verticalAlignment = wxTOP )
    void    StringToLines( const wxString& value, wxArrayString& lines )
    void    GetTextBoxSize( wxDC& dc, wxArrayString& lines, long * width, long * height)
    int     GetBatchCount()
    ~%wxver22 void ForceRefresh()
    bool    IsEditable()
    void    EnableEditing( bool edit )
    void    EnableCellEditControl( bool enable = TRUE )
    void    DisableCellEditControl()
    bool    CanEnableCellControl() const
    bool    IsCellEditControlEnabled() const
    bool    IsCellEditControlShown() const
    bool    IsCurrentCellReadOnly() const
    void    ShowCellEditControl()
    void    HideCellEditControl()
    void    SaveEditControlValue()
    void    XYToCell( int x, int y, wxGridCellCoords& coords)
    int     XToCol( int x )
    int     YToRow( int y )
    int     XToEdgeOfCol( int x )
    int     YToEdgeOfRow( int y )
    wxRect  CellToRect( int row, int col )
    int     GetGridCursorRow()
    %property=GridCursorRow, read
    int     GetGridCursorCol()
    %property=GridCursorCol, read
    bool    IsVisible( int row, int col, bool wholeCellVisible = TRUE )
    //  bool IsVisible( const wxGridCellCoords& coords, bool wholeCellVisible = TRUE )
    void    MakeCellVisible( int row, int col )
    //  void MakeCellVisible( const wxGridCellCoords& coords )
    void    SetGridCursor( int row, int col )
    int     GetDefaultRowLabelSize()
    %property=DefaultRowLabelSize, read
    int     GetRowLabelSize()
    int     GetDefaultColLabelSize()
    %property=DefaultColLabelSize, read
    int     GetColLabelSize()
    wxColour GetLabelBackgroundColour()
    wxColour GetLabelTextColour()
    wxFont  GetLabelFont()
    void    GetRowLabelAlignment( int *horz, int *vert )
    void    GetColLabelAlignment( int *horz, int *vert )
    wxString GetRowLabelValue( int row )
    wxString GetColLabelValue( int col )
    wxColour GetGridLineColour()
    wxColour GetCellHighlightColour()
    ~%wxver22 int GetCellHighlightPenWidth()
    ~%wxver22 int GetCellHighlightROPenWidth()
    void    SetRowLabelSize( int width )
    %property=RowLabelSize, read, write
    void    SetColLabelSize( int height )
    %property=ColLabelSize, read, write
    void    SetLabelBackgroundColour( const wxColour& backColour )
    %property=LabelBackgroundColour, read, write
    void    SetLabelTextColour( const wxColour& textColour)
    %property=LabelTextColour, read, write
    void    SetLabelFont( const wxFont& labelFont)
    %property=LabelFont, read, write
    void    SetRowLabelAlignment( int horiz, int vert )
    void    SetColLabelAlignment( int horiz, int vert )
    void    SetRowLabelValue( int row, const wxString& value )
    void    SetColLabelValue( int col, const wxString& value)
    void    SetGridLineColour( const wxColour& lineColour)
    void    SetCellHighlightColour( const wxColour& highlightColour)
    %property=CellHighlightColour, read, write
    ~%wxver22 void SetCellHighlightPenWidth(int width)
    ~%wxver22 %property=CellHighlightPenWidth, read, write
    ~%wxver22 void SetCellHighlightROPenWidth(int width)
    ~%wxver22 %property=CellHighlightROPenWidth, read, write
    void    SetRowAttr(int row, wxGridCellAttr *attr)
    void    SetColAttr(int col, wxGridCellAttr *attr)
    void    SetColFormatBool(int col)
    void    SetColFormatNumber(int col)
    void    SetColFormatFloat(int col, int width = -1, int precision = -1)
    void    SetColFormatCustom(int col, const wxString& typeName)
    void    EnableGridLines( bool enable = TRUE )
    bool    GridLinesEnabled()
    int     GetDefaultRowSize()
    int     GetRowSize( int row )
    int     GetDefaultColSize()
    int     GetColSize( int col )
    wxColour GetSelectionBackground() const
    wxColour GetSelectionForeground() const
    void    SetSelectionBackground(const wxColour& c)
    %property=SelectionBackground, read, write
    void    SetSelectionForeground(const wxColour& c)
    %property=SelectionForeground, read, write
    wxColour GetDefaultCellBackgroundColour()
    wxColour GetCellBackgroundColour( int row, int col )
    wxColour GetDefaultCellTextColour()
    wxColour GetCellTextColour( int row, int col )
    wxFont  GetDefaultCellFont()
    void    SetDefaultCellFont( const wxFont& cellFont)
    wxFont  GetCellFont( int row, int col )
    void    SetCellFont( int row, int col, const wxFont& cellFont)
    void    GetDefaultCellAlignment( int *horiz, int *vert )
    void    SetDefaultCellAlignment( int horiz, int vert )
    void    GetCellAlignment( int row, int col, int *horiz, int *vert )
    void    SetCellAlignment( int row, int col, int horiz, int vert )
    void    SetDefaultColSize( int width, bool resizeExistingCols = FALSE )
    void    SetColSize( int col, int width )
    void    SetColMinimalWidth( int col, int width )
    void    SetDefaultRowSize( int height, bool resizeExistingRows = FALSE )
    void    SetRowSize( int row, int height )
    void    SetRowMinimalHeight( int row, int width )
    void    SetDefaultCellBackgroundColour( const wxColour& backColour)
    void    SetCellBackgroundColour( int row, int col, const wxColour& backColour)
    void    SetDefaultCellTextColour( const wxColour& textColour)
    void    SetCellTextColour( int row, int col, const wxColour& textColour)
    void    SetDefaultRenderer(wxGridCellRenderer *renderer)
    void    SetCellRenderer(int row, int col, wxGridCellRenderer *renderer)
    wxGridCellRenderer* GetDefaultRenderer() const
    wxGridCellRenderer* GetCellRenderer(int row, int col)
    void    SetDefaultEditor(wxGridCellEditor *editor)
    void    SetCellEditor(int row, int col, wxGridCellEditor *editor)
    wxGridCellEditor* GetDefaultEditor() const
    %property=DefaultEditor, read, write
    wxGridCellEditor* GetCellEditor(int row, int col)
    wxString GetCellValue( int row, int col )
    // wxString GetCellValue( const wxGridCellCoords& coords )
    void SetCellValue( int row, int col, const wxString& s )
    // void SetCellValue( const wxGridCellCoords& coords, const wxString& s )
    bool    IsReadOnly(int row, int col) const
    void    SetReadOnly(int row, int col, bool isReadOnly = TRUE)
    void    SelectRow( int row, bool addToSelected = FALSE )
    void    SelectCol( int col, bool addToSelected = FALSE )
    void    SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol, bool addToSelected = FALSE )
    // void SelectBlock( const wxGridCellCoords& topLeft, const wxGridCellCoords& bottomRight)
    void    SelectAll()
    bool    IsSelection()
    void    ClearSelection()
    bool    IsInSelection( int row, int col )
    // bool IsInSelection( const wxGridCellCoords& coords )
    wxRect  BlockToDeviceRect( const wxGridCellCoords & topLeft, const wxGridCellCoords & bottomRight )
    void    RegisterDataType(const wxString& typeName, wxGridCellRenderer* renderer, wxGridCellEditor* editor)
    wxGridCellEditor* GetDefaultEditorForCell(int row, int col) const
    //wxGridCellEditor* GetDefaultEditorForCell(const wxGridCellCoords& coords) const
    wxGridCellRenderer* GetDefaultRendererForCell(int row, int col) const
    wxGridCellEditor* GetDefaultEditorForType(const wxString& typeName) const
    wxGridCellRenderer* GetDefaultRendererForType(const wxString& typeName) const
    void SetMargins(int extraWidth, int extraHeight)
    ~%wxver22 wxWindow* GetGridWindow()
    ~%wxver22 %property=GridWindow, read
    ~%wxver22 wxWindow* GetGridRowLabelWindow()
    ~%wxver22 %property=GridRowLabelWindow, read
    ~%wxver22 wxWindow* GetGridColLabelWindow()
    ~%wxver22 %property=GridColLabelWindow, read
    ~%wxver22 wxWindow* GetGridCornerLabelWindow()
    ~%wxver22 %property=GridCornerLabelWindow, read
    bool    MoveCursorUp( bool expandSelection )
    bool    MoveCursorDown( bool expandSelection )
    bool    MoveCursorLeft( bool expandSelection )
    bool    MoveCursorRight( bool expandSelection )
    bool    MovePageDown()
    bool    MovePageUp()
    bool    MoveCursorUpBlock( bool expandSelection )
    bool    MoveCursorDownBlock( bool expandSelection )
    bool    MoveCursorLeftBlock( bool expandSelection )
    bool    MoveCursorRightBlock( bool expandSelection )
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellEditor

%class %noclassinfo wxGridCellEditor
    bool IsCreated()
    wxControl* GetControl()
    void SetControl(wxControl* control)
    void SetParameters(const wxString& params)
    void IncRef()
    void DecRef()
    virtual void Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler)
    virtual void BeginEdit(int row, int col, wxGrid* grid)
    virtual bool EndEdit(int row, int col, wxGrid* grid)
    virtual void Reset()
    //virtual wxGridCellEditor *Clone() const
    virtual void SetSize(const wxRect& rect)
    virtual void Show(bool show, wxGridCellAttr *attr = NULL)
    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr)
    virtual bool IsAcceptedKey(wxKeyEvent& event)
    virtual void StartingKey(wxKeyEvent& event)
    virtual void StartingClick()
    virtual void HandleReturn(wxKeyEvent& event)
    virtual void Destroy()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellTextEditor

%class %noclassinfo wxGridCellTextEditor, wxGridCellEditor
    wxGridCellTextEditor()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellNumberEditor

%class %noclassinfo wxGridCellNumberEditor, wxGridCellTextEditor
    wxGridCellNumberEditor(int min = -1, int max = -1)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellFloatEditor

%class %noclassinfo wxGridCellFloatEditor, wxGridCellTextEditor
    wxGridCellFloatEditor()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellBoolEditor

%class %noclassinfo wxGridCellBoolEditor, wxGridCellEditor
    wxGridCellBoolEditor()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellChoiceEditor

// %class %noclassinfo wxGridCellChoiceEditor, wxGridCellEditor
//wxGridCellChoiceEditor(size_t n = 0, const wxString choices[] = NULL, bool allowOthers = FALSE)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellEnumEditor

// %class %noclassinfo wxGridCellEnumEditor, wxGridCellChoiceEditor
//wxGridCellEnumEditor( const wxString& choices = "" )
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridCellAutoWrapStringEditor

// %class %noclassinfo wxGridCellAutoWrapStringEditor, wxGridCellTextEditor
//wxGridCellAutoWrapStringEditor()
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridEvent

%class %delete wxGridEvent, wxNotifyEvent
    %define %event wxEVT_GRID_CELL_LEFT_CLICK
    %define %event wxEVT_GRID_CELL_RIGHT_CLICK
    %define %event wxEVT_GRID_CELL_LEFT_DCLICK
    %define %event wxEVT_GRID_CELL_RIGHT_DCLICK
    %define %event wxEVT_GRID_LABEL_LEFT_CLICK
    %define %event wxEVT_GRID_LABEL_RIGHT_CLICK
    %define %event wxEVT_GRID_LABEL_LEFT_DCLICK
    %define %event wxEVT_GRID_LABEL_RIGHT_DCLICK

    wxGridEvent(int id, wxEventType type, wxGrid* obj, int row = -1, int col = -1, int x = -1, int y = -1, bool sel = TRUE, bool control = FALSE, bool shift = FALSE, bool alt = FALSE, bool meta = FALSE)
    virtual int GetRow()
    %property=Row, read
    virtual int GetCol()
    %property=Col, read
    wxPoint     GetPosition()
    %property=Position, read
    bool        Selecting()
    bool        ControlDown()
    bool        MetaDown()
    bool        ShiftDown()
    bool        AltDown()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridSizeEvent

%class wxGridSizeEvent, wxNotifyEvent
    %define %event wxEVT_GRID_ROW_SIZE
    %define %event wxEVT_GRID_COL_SIZE

    wxGridSizeEvent(int id, wxEventType type, wxGrid* obj, int rowOrCol = -1, int x = -1, int y = -1, bool control = FALSE, bool shift = FALSE, bool alt = FALSE, bool meta = FALSE)
    int         GetRowOrCol()
    %property=RowOrCol, read
    wxPoint     GetPosition()
    %property=Position, read
    bool        ShiftDown()
    bool        ControlDown()
    bool        AltDown()
    bool        MetaDown()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridRangeSelectEvent

%class %delete wxGridRangeSelectEvent, wxNotifyEvent
    %define %event wxEVT_GRID_RANGE_SELECT
    %define %event wxEVT_GRID_SELECT_CELL
    %define %event wxEVT_GRID_CELL_CHANGE

    wxGridRangeSelectEvent(int id, wxEventType type, wxGrid* obj, const wxGridCellCoords& topLeft, const wxGridCellCoords& bottomRight, bool sel = TRUE, bool control = FALSE, bool shift = FALSE, bool alt = FALSE, bool meta = FALSE)
    int         GetLeftCol()
    %property=LeftCol, read
    int         GetTopRow()
    %property=TopRow, read
    int         GetBottomRow()
    %property=BottomRow, read
    int         GetRightCol()
    %property=RightCol, read
    bool        Selecting()
    bool        ShiftDown()
    bool        ControlDown()
    bool        AltDown()
    bool        MetaDown()
    //  wxGridCellCoords GetTopLeftCoords()
    //  wxGridCellCoords GetBottomRightCoords()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridEditorCreatedEvent

%wxver22 %skip
%class %delete wxGridEditorCreatedEvent, wxCommandEvent
    %define %event wxEVT_GRID_EDITOR_CREATED
    %define %event wxEVT_GRID_EDITOR_SHOWN
    %define %event wxEVT_GRID_EDITOR_HIDDEN

    wxGridEditorCreatedEvent(int id, wxEventType type, wxObject* obj, int row, int col, wxControl* ctrl)
    int GetRow()
    int GetCol()
    wxControl * GetControl()
    void SetRow(int row)
    %property=Row, read, write
    void SetCol(int col)
    %property=Col, read, write
    void SetControl(wxControl * ctrl)
    %property=Control, read, write
%endclass

%endif wxLUA_USE_wxGrid
