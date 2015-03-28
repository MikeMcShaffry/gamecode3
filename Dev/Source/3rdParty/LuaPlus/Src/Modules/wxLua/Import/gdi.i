/////////////////////////////////////////////////////////////////////////////
// Purpose:     GDI classes, Colour, Pen, Brush, Font, DC, Bitmap...
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// wxPoint

%if wxLUA_USE_wxPointSizeRect

%include "wx/gdicmn.h"

%class %delete %noclassinfo %encapsulate wxPoint
    %define %object wxDefaultPosition

    wxPoint(int x, int y)
    int GetX() const
    int GetY() const
    int GetXY() const
    void SetX(int x)
    %property=X, read, write
    void SetY(int y)
    %property=Y, read, write
    void Set(int x, int y)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSize

%class %delete %noclassinfo %encapsulate wxSize
    %define %object wxDefaultSize

    wxSize(int width, int height)
    int GetWidth() const
    int GetHeight() const
    void Set(int width, int height)
    void SetHeight(int height)
    %property=Height, read, write
    void SetWidth(int width)
    %property=Width, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxRect

%class %delete %noclassinfo %encapsulate wxRect
    wxRect(int x = 0, int y = 0, int w = 0, int h = 0)
    %constructor wxRectFromPoints(const wxPoint& topLeft, const wxPoint& bottomRight)
    %constructor wxRectFromPosSize(const wxPoint& pos, const wxSize& size)
    int     GetX()
    void    SetX(int X)
    %property=X, read, write
    int     GetY()
    void    SetY(int Y)
    %property=Y, read, write
    int     GetWidth()
    int     GetHeight()
    wxPoint GetPosition()
    %property=Position, read
    wxSize  GetSize()
    %property=Size, read
    int     GetLeft()
    int     GetTop()
    int     GetBottom()
    int     GetRight()
    void    SetWidth(int width)
    %property=Width, read, write
    void    SetHeight(int height)
    %property=Height, read, write
    void    SetLeft(int left)
    %property=Left, read, write
    void    SetTop(int top)
    %property=Top, read, write
    void    SetRight(int right)
    %property=Right, read, write
    void    SetBottom(int bottom)
    %property=Bottom, read, write
    void    Inflate(wxCoord dx, wxCoord dy)
    bool    Inside(wxCoord cx, wxCoord cy)
%endclass

%endif wxLUA_USE_wxPointSizeRect

///////////////////////////////////////////////////////////////////////////////
// wxGDIObject

%class %delete wxGDIObject, wxObject
    wxGDIObject()
    bool GetVisible()
    void SetVisible(bool visible)
    bool IsNull()
    %property=Visible, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxRegion

%if wxLUA_USE_wxRegion

%enum wxRegionContain
    wxOutRegion
    wxPartRegion
    wxInRegion
%end

%class wxRegion, wxGDIObject
    wxRegion(long x = 0, long y = 0, long width = 0, long height = 0)
    %constructor wxRegionFromPoints(const wxPoint& topLeft, const wxPoint& bottomRight)
    //%constructor wxRegionFromPosSize(const wxPoint& pos, const wxSize& size)
    %constructor wxRegionFromRect(const wxRect& rect)
    void Clear()
    wxRegionContain Contains(long x, long y)
    %rename ContainsPoint wxRegionContain Contains(const wxPoint& pt)
    %rename ContainsRect  wxRegionContain Contains(const wxRect& rect)
    %rename ContainsRectDim wxRegionContain Contains(long x, long y, long w, long h)
    wxRect GetBox() const
    %rename GetBoxCoords void GetBox(int &x, int &y, int &width, int &height)
    bool Intersect(long x, long y, long width, long height)
    %rename IntersectRect bool Intersect(const wxRect& rect)
    %rename IntersectRegion bool Intersect(const wxRegion& region)
    bool IsEmpty() const
    bool Subtract(long x, long y, long width, long height)
    %rename SubtractRect bool Subtract(const wxRect& rect)
    %rename SubtractRegion bool Subtract(const wxRegion& region)
    bool Union(long x, long y, long width, long height)
    %rename UnionRect bool Union(const wxRect& rect)
    %rename UnionRegion bool Union(const wxRegion& region)
    bool Xor(long x, long y, long width, long height)
    %rename XorRect bool Xor(const wxRect& rect)
    %rename XorRegion bool Xor(const wxRegion& region)
    %property=Box, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxRegionIterator

%class wxRegionIterator, wxObject
    wxRegionIterator(const wxRegion& region)
    long GetX()
    long GetY()
    long GetW()
    long GetWidth()
    long GetH()
    long GetHeight()
    wxRect GetRect()
    bool HaveRects()
    void Reset()
    //void Next() // operator++
    %property=X, read
    %property=Y, read
    %property=W, read
    %property=Width, read
    %property=H, read
    %property=Height, read
    %property=Rect, read
%endclass

%endif wxLUA_USE_wxRegion

///////////////////////////////////////////////////////////////////////////////
// wxFont

%if wxLUA_USE_wxFont

%include "wx/font.h"

%enum
    wxDEFAULT
    wxDECORATIVE
    wxROMAN
    wxSCRIPT
    wxSWISS
    wxMODERN
    wxTELETYPE

    wxVARIABLE      // unused ?
    wxFIXED         // unused ?

    wxNORMAL
    wxLIGHT
    wxBOLD
    wxITALIC
    wxSLANT
%end

%enum wxFontFamily
    wxFONTFAMILY_DEFAULT
    wxFONTFAMILY_DECORATIVE
    wxFONTFAMILY_ROMAN
    wxFONTFAMILY_SCRIPT
    wxFONTFAMILY_SWISS
    wxFONTFAMILY_MODERN
    wxFONTFAMILY_TELETYPE
    wxFONTFAMILY_MAX
~%wxver22    wxFONTFAMILY_UNKNOWN
%end

%enum wxFontStyle
    wxFONTSTYLE_NORMAL
    wxFONTSTYLE_ITALIC
    wxFONTSTYLE_SLANT
    wxFONTSTYLE_MAX
%end

%enum wxFontWeight
    wxFONTWEIGHT_NORMAL
    wxFONTWEIGHT_LIGHT
    wxFONTWEIGHT_BOLD
    wxFONTWEIGHT_MAX
%end

%enum wxFontEncoding
	wxFONTENCODING_SYSTEM
	wxFONTENCODING_DEFAULT
	wxFONTENCODING_ISO8859_1
	wxFONTENCODING_ISO8859_2
	wxFONTENCODING_ISO8859_3
	wxFONTENCODING_ISO8859_4
	wxFONTENCODING_ISO8859_5
	wxFONTENCODING_ISO8859_6
	wxFONTENCODING_ISO8859_7
	wxFONTENCODING_ISO8859_8
	wxFONTENCODING_ISO8859_9
	wxFONTENCODING_ISO8859_10
	wxFONTENCODING_ISO8859_11
	wxFONTENCODING_ISO8859_12
	wxFONTENCODING_ISO8859_13
	wxFONTENCODING_ISO8859_14
	wxFONTENCODING_ISO8859_15
	wxFONTENCODING_ISO8859_MAX
	wxFONTENCODING_KOI8
	wxFONTENCODING_ALTERNATIVE
	wxFONTENCODING_BULGARIAN
	wxFONTENCODING_CP437
	wxFONTENCODING_CP850
	wxFONTENCODING_CP852
	wxFONTENCODING_CP855
	wxFONTENCODING_CP866
	wxFONTENCODING_CP874
~%wxver22	wxFONTENCODING_CP932
~%wxver22	wxFONTENCODING_CP936
~%wxver22	wxFONTENCODING_CP949
~%wxver22	wxFONTENCODING_CP950
	wxFONTENCODING_CP1250
	wxFONTENCODING_CP1251
	wxFONTENCODING_CP1252
	wxFONTENCODING_CP1253
	wxFONTENCODING_CP1254
	wxFONTENCODING_CP1255
	wxFONTENCODING_CP1256
	wxFONTENCODING_CP1257
	wxFONTENCODING_CP12_MAX
~%wxver22	wxFONTENCODING_UTF7
~%wxver22	wxFONTENCODING_UTF8
	wxFONTENCODING_UNICODE
	wxFONTENCODING_MAX
%end

%class %delete wxFont, wxGDIObject
	%define %object wxNullFont
	%define %pointer wxNORMAL_FONT
	%define %pointer wxSMALL_FONT
	%define %pointer wxITALIC_FONT
	%define %pointer wxSWISS_FONT

	wxFont(int pointSize, int family, int style, int weight, const bool underline = FALSE, const wxString& faceName = "", wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    static wxFontEncoding GetDefaultEncoding()
    wxString GetFaceName() const
    int      GetFamily() const
    // int   GetFontId() const
    int      GetPointSize() const
    int      GetStyle() const
    bool     GetUnderlined() const
    int      GetWeight() const
    static void SetDefaultEncoding(wxFontEncoding encoding)
    void     SetFaceName(const wxString& faceName)
    void     SetFamily(int family)
    void     SetPointSize(int pointSize)
    void     SetStyle(int style)
    void     SetUnderlined(const bool underlined)
    void     SetWeight(int weight)
    %property=FaceName, read, write
    %property=Family, read, write
    %property=PointSize, read, write
    %property=Style, read, write
    %property=Underlined, read, write
    %property=Weight, read, write
    %property=DefaultEncoding, read, write
%endclass

%endif wxLUA_USE_wxFont

///////////////////////////////////////////////////////////////////////////////
// wxFontEnumerator

%if wxLUA_USE_wxFontEnumerator

%include "wx/fontenum.h"

%class %delete %noclassinfo %encapsulate wxFontEnumerator
    wxFontEnumerator()
    virtual bool EnumerateFacenames( wxFontEncoding encoding = wxFONTENCODING_SYSTEM, bool fixedWidthOnly = FALSE)
    virtual bool EnumerateEncodings( const wxString &font = "" )
    wxArrayString *GetEncodings()
    %win %property=Encodings, read
    wxArrayString *GetFacenames()
    %win %property=Facenames, read
%endclass

%endif wxLUA_USE_wxFontEnumerator

///////////////////////////////////////////////////////////////////////////////
// wxFontList

%if wxLUA_USE_wxFontList

%class %noclassinfo wxFontList, wxList
    %define %pointer wxTheFontList

    void AddFont(wxFont *font)
    wxFont *FindOrCreateFont(int pointSize, int family, int style, int weight, bool underline = FALSE, const wxString &faceName = "", wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    void RemoveFont(wxFont *font)
%endclass

%endif wxLUA_USE_wxFontList

///////////////////////////////////////////////////////////////////////////////
// wxFontMapper

%if wxLUA_USE_wxFontMapper

%include "wx/fontmap.h"

%class %noclassinfo wxFontMapper
    // %rename GetAltForEncodingInternal bool GetAltForEncoding(wxFontEncoding encoding, wxNativeEncodingInfo *info, const wxString &faceName = wxEmptyString, bool interactive = TRUE)
    bool GetAltForEncoding(wxFontEncoding encoding, wxFontEncoding *altEncoding, const wxString &faceName = wxEmptyString, bool interactive = TRUE)
    bool IsEncodingAvailable(wxFontEncoding encoding, const wxString &facename = wxEmptyString)
    wxFontEncoding CharsetToEncoding(const wxString &charset, bool interactive = TRUE)
    static wxString GetEncodingName(wxFontEncoding encoding)
    static wxString GetEncodingDescription(wxFontEncoding encoding)
    void SetDialogParent(wxWindow *parent)
    void SetDialogTitle(const wxString &title)
    void SetConfig(wxConfigBase *config = NULL)
    void SetConfigPath(const wxString &prefix)
    static wxFontMapper *Get()
    static wxFontMapper *Set(wxFontMapper *mapper)
%endclass

%endif wxLUA_USE_wxFontMapper

///////////////////////////////////////////////////////////////////////////////
// wxColour

%if wxLUA_USE_wxColourPenBrush

%include "wx/colour.h"
%include "wx/gdicmn.h"

%class %delete wxColour, wxObject
    %define %object wxNullColour
    %define %pointer wxBLACK
    %define %pointer wxWHITE
    %define %pointer wxRED
    %define %pointer wxBLUE
    %define %pointer wxGREEN
    %define %pointer wxCYAN
    %define %pointer wxLIGHT_GREY

    wxColour(const unsigned char red, const unsigned char green, const unsigned char blue)
    unsigned char Blue() const
    long GetPixel() const
    unsigned char Green() const
    bool Ok() const
    unsigned char Red() const
    void Set(const unsigned char red, const unsigned char green, const unsigned char blue)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPen

%include "wx/pen.h"

%define wxCAP_BUTT
%define wxCAP_PROJECTING
%define wxCAP_ROUND

%define wxDOT
%define wxDOT_DASH
%define wxSOLID
%define wxLONG_DASH
%define wxSHORT_DASH
%define wxUSER_DASH

%define wxJOIN_BEVEL
%define wxJOIN_MITER
%define wxJOIN_ROUND

%enum
    wxTRANSPARENT

    wxSTIPPLE_MASK_OPAQUE
    wxSTIPPLE_MASK
    wxSTIPPLE
    wxBDIAGONAL_HATCH
    wxCROSSDIAG_HATCH
    wxFDIAGONAL_HATCH
    wxCROSS_HATCH
    wxHORIZONTAL_HATCH
    wxVERTICAL_HATCH
%end

%class %delete wxPen, wxGDIObject
	%define %object wxNullPen
	%define %pointer wxRED_PEN
	%define %pointer wxCYAN_PEN
	%define %pointer wxGREEN_PEN
	%define %pointer wxBLACK_PEN
	%define %pointer wxWHITE_PEN
	%define %pointer wxTRANSPARENT_PEN
    %define %pointer wxBLACK_DASHED_PEN
	%define %pointer wxGREY_PEN
	%define %pointer wxMEDIUM_GREY_PEN
	%define %pointer wxLIGHT_GREY_PEN

	wxPen()
    int GetCap() const
    //wxColour& GetColour() const
    wxColour& GetColour() const
    // int GetDashes(wxDash** dashes) const
    int GetJoin() const
    %win wxBitmap* GetStipple() const
    int GetStyle() const
    int GetWidth() const
    bool Ok() const
    void SetCap(int capStyle)
    void SetColour(wxColour& colour)
    %rename SetColourString void SetColour(const wxString& colourName)
    %rename SetColourRGB void SetColour(int red, int green, int blue)
    //void SetDashes(int nb_dashes, const wxDash *dash)
    void SetJoin(int join_style)
    // void SetStipple(const wxBitmap* stipple)
    void SetStyle(int style)
    void SetWidth(int width)
    %property=Cap, read, write
    %property=Colour, read, write
    %property=Join, read, write
    %property=Width, read, write
    %win %property=Stipple, read
    %property=Style, read, write

    // alternative constructors
    %constructor wxPenFromColour(const wxColour& colour, int width, int style)
    %constructor wxPenFromColourName(const wxString& colourName, int width, int style)
    %win %constructor wxPenFromBitmap(const wxBitmap& stipple, int width)
    %constructor wxPenCopy(const wxPen& pen)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPenList

%if wxLUA_USE_wxPenList

%class %noclassinfo wxPenList, wxList
    void AddPen(wxPen *pen)
    void RemovePen(wxPen *pen)
    wxPen *FindOrCreatePen(const wxColour& colour, int width, int style)
%enclass

%endif wxLUA_USE_wxPenList

///////////////////////////////////////////////////////////////////////////////
// wxBrush

%include "wx/brush.h"

%class %delete wxBrush, wxGDIObject
	%define %object wxNullBrush
	%define %pointer wxBLUE_BRUSH
	%define %pointer wxGREEN_BRUSH
	%define %pointer wxWHITE_BRUSH
	%define %pointer wxBLACK_BRUSH
	%define %pointer wxGREY_BRUSH
	%define %pointer wxMEDIUM_GREY_BRUSH
	%define %pointer wxLIGHT_GREY_BRUSH
	%define %pointer wxTRANSPARENT_BRUSH
	%define %pointer wxCYAN_BRUSH
	%define %pointer wxRED_BRUSH

    wxBrush(const wxColour& colour, int style)
    %constructor wxBrushFromColourName(const wxString& colourName, int style)
    %constructor wxBrushFromBitmap(const wxBitmap& stippleBitmap)
    %constructor wxBrushCopy(const wxBrush& brush)
    wxColour GetColour() const
    wxBitmap * GetStipple() const
    int GetStyle() const
    bool Ok() const
    void SetColour(wxColour& colour)
    %rename SetColourString void SetColour(const wxString& colourName)
    %rename SetColourRGB void SetColour(const unsigned char red, const unsigned char green, const unsigned char blue)
    void SetStipple(const wxBitmap& bitmap)
    void SetStyle(int style)
    %property=Colour, read, write
    %property=Stipple, read, write
    %property=Style, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxBrushList

%if wxLUA_USE_wxBrushList

%class %noclassinfo wxBrushList, wxList
    wxBrushList()
    void AddBrush(wxBrush *brush)
    void RemoveBrush(wxBrush *brush)
    wxBrush * FindOrCreateBrush(const wxColour& colour, int style)
%endclass

%endif wxLUA_USE_wxBrushList
%endif wxLUA_USE_wxColourPenBrush

///////////////////////////////////////////////////////////////////////////////
// wxPalette

%if wxLUA_USE_wxPalette

%include "wx/palette.h"

%class %delete wxPalette, wxGDIObject
    %define %object wxNullPalette

    // wxPalette(int n, const unsigned char* red, const unsigned char* green, const unsigned char* blue)
    bool Create(int n, const unsigned char* red, const unsigned char* green, const unsigned char* blue)
    // int GetPixel(const unsigned char* red, const unsigned char* green, const unsigned char* blue) const
    // bool GetRGB(int pixel, const unsigned char* red, const unsigned char* green, const unsigned char* blue) const
    bool Ok() const
%endclass

%endif wxLUA_USE_wxPalette

///////////////////////////////////////////////////////////////////////////////
// wxIcon

%if wxLUA_USE_wxIcon

%class %delete wxIcon, wxGDIObject
    %define %object wxNullIcon

    wxIcon(const wxString& name, long type, int desiredWidth = -1, int desiredHeight = -1)
    //%constructor wxNewIcon(int width, int height, int depth = -1) // constructor does not exist
    %constructor wxDefaultIcon()
    %win void CopyFromBitmap(const wxBitmap& bmp)
    %mac void CopyFromBitmap(const wxBitmap& bmp)
    int     GetDepth()
    int     GetWidth()
    int     GetHeight()
    bool    LoadFile(const wxString& name, long flags)
//    %win long GetHandle()   // FIXME doesn't work in wx25 and for wxIcon what could you do with it
//    %win void SetHandle(long handle)
    bool    Ok()
    void    SetWidth(int w)
    %property=Width, read, write
    void    SetHeight(int h)
    %property=Height, read, write
    void    SetDepth(int d)
    %property=Depth, read, write
    %win void SetSize(const wxSize& size)
%endclass

%endif wxLUA_USE_wxIcon

///////////////////////////////////////////////////////////////////////////////
// wxBitmap

%if wxLUA_USE_wxBitmap

%include "wx/bitmap.h"

%win %class %noclassinfo wxBitmapHandler   // are these even necessary?
%endclass
%win %class %noclassinfo wxGDIImageHandler
%endclass

~<wxver25 %win %class %noclassinfo wxGDIImageHandlerList, wxList
%endclass

%class %delete wxBitmap, wxGDIObject
	%define %object wxNullBitmap

	%constructor wxDefaultBitmap()
    %win wxBitmap(void * data, int type, int width, int height, int depth = -1)
    %constructor wxBitmapCopy(const wxBitmap& bitmap)
    %constructor wxEmptyBitmap( int width, int height, int depth = -1)
    %constructor wxBitmapFromFile( const wxString& name, long type)
    %constructor wxBitmapFromXPMData(const char **data)
    ~%wxver22 %constructor wxBitmapFromImage(const wxImage &image, int depth = -1)
    %win static void AddHandler(wxBitmapHandler* handler)
    %win static void CleanUpHandlers()
    virtual bool Create(int width, int height, int depth = -1)
    int GetDepth() const
    ~<wxver25 %win static wxGDIImageHandlerList& GetHandlers()
    <wxver25 %win static wxList& GetHandlers()
    int GetHeight() const
    wxPalette* GetPalette() const
    wxMask* GetMask() const
    int GetWidth() const
    wxBitmap GetSubBitmap(const wxRect&rect) const
    %win static void InitStandardHandlers()
    %win static void InsertHandler(wxBitmapHandler* handler)
    bool LoadFile(const wxString& name, long type)
    bool Ok() const
    %win static bool RemoveHandler(const wxString& name)
    bool SaveFile(const wxString& name, int type, wxPalette* palette = NULL)
    void SetDepth(int depth)
    %property=Depth, read, write
    void SetHeight(int height)
    %property=Height, read, write
    void SetMask(wxMask* mask)
    %property=Mask, read, write
    %win void SetPalette(const wxPalette& palette)
    %win %property=Palette, read, write
    %gtk %property=Palette, read
    void SetWidth(int width)
    %property=Width, read, write
%endclass

%endif wxLUA_USE_wxBitmap

///////////////////////////////////////////////////////////////////////////////
// wxCursor

%if wxLUA_USE_wxCursor

%include "wx/cursor.h"

%enum wxStockCursor
    wxCURSOR_NONE
    wxCURSOR_ARROW
~%wxver22    wxCURSOR_RIGHT_ARROW
    wxCURSOR_BULLSEYE
    wxCURSOR_CHAR
    wxCURSOR_CROSS
    wxCURSOR_HAND
    wxCURSOR_IBEAM
    wxCURSOR_LEFT_BUTTON
    wxCURSOR_MAGNIFIER
    wxCURSOR_MIDDLE_BUTTON
    wxCURSOR_NO_ENTRY
    wxCURSOR_PAINT_BRUSH
    wxCURSOR_PENCIL
    wxCURSOR_POINT_LEFT
    wxCURSOR_POINT_RIGHT
    wxCURSOR_QUESTION_ARROW
    wxCURSOR_RIGHT_BUTTON
    wxCURSOR_SIZENESW
    wxCURSOR_SIZENS
    wxCURSOR_SIZENWSE
    wxCURSOR_SIZEWE
    wxCURSOR_SIZING
    wxCURSOR_SPRAYCAN
    wxCURSOR_WAIT
    wxCURSOR_WATCH
    wxCURSOR_BLANK
    wxCURSOR_DEFAULT
%end

%class %delete wxCursor, wxBitmap
	%define %object wxNullCursor
	%define %pointer wxSTANDARD_CURSOR
	%define %pointer wxHOURGLASS_CURSOR
	%define %pointer wxCROSS_CURSOR

    %win wxCursor(const wxString& cursorName, long type, int hotSpotX = 0, int hotSpotY = 0)
    %constructor wxDefaultCursor()
    %constructor wxStockCursor(int id)
    bool Ok()
//    %win long GetHandle()  FIXME - wxCursor what could you do with this?
//    %win void SetHandle(long handle)
//    %win %property=Handle, read, write
    %win int GetWidth()
    %win int GetHeight()
    %win int GetDepth()
    %win %property=Width, read, write
    %win void SetWidth(int width)
    %win %property=Height, read, write
    %win void SetHeight(int height)
    %win %property=Depth, read, write
    %win void SetDepth(int depth)
    %win void SetSize(const wxSize& size)
%endclass

%endif wxLUA_USE_wxCursor

///////////////////////////////////////////////////////////////////////////////
// wxMask

%if wxLUA_USE_wxMask

%class wxMask, wxObject
    wxMask(const wxBitmap& bitmap)
    %constructor wxEmptyMask()
    %constructor wxMaskColour(const wxBitmap& bitmap, const wxColour& colour)
    %win %constructor wxMaskIndex(const wxBitmap& bitmap, int index)
    bool Create(const wxBitmap& bitmap)
    %rename CreateColour bool Create(const wxBitmap& bitmap, const wxColour& colour)
    %win %rename CreateIndex bool Create(const wxBitmap& bitmap, int index)
%endclass

%endif wxLUA_USE_wxMask

///////////////////////////////////////////////////////////////////////////////
// wxImageList

%if wxLUA_USE_wxImageList

%include "wx/imaglist.h"

%define wxIMAGELIST_DRAW_NORMAL
%define wxIMAGELIST_DRAW_TRANSPARENT
%define wxIMAGELIST_DRAW_SELECTED
%define wxIMAGELIST_DRAW_FOCUSED
%define wxIMAGE_LIST_NORMAL
%define wxIMAGE_LIST_SMALL
%define wxIMAGE_LIST_STATE

%class wxImageList, wxObject
    wxImageList(int width, int height, bool mask = TRUE, int initialCount = 1)
    int     Add(const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap)
    %rename AddWithColourMask int Add(const wxBitmap& bitmap, const wxColour& maskColour)
    %rename AddIcon int Add(const wxIcon& icon)
    bool    Draw(int index, wxDC& dc, int x, int y, int flags = wxIMAGELIST_DRAW_NORMAL, const bool solidBackground = FALSE)
    int     GetImageCount()
    %property=ImageCount, read
    void    GetSize(int index, int& width, int& height)
    bool    Remove(int index)
    %win bool Replace(int index, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap)
    %gtk bool Replace(int index, const wxBitmap& bitmap)
    %mac bool Replace(int index, const wxBitmap& bitmap)
    //bool ReplaceIcon(int index, const wxIcon& icon)
    bool    RemoveAll()
%endclass

%endif wxLUA_USE_wxImageList

///////////////////////////////////////////////////////////////////////////////
// wxDC

%if wxLUA_USE_wxDC

%include "wx/dc.h"

%define wxMM_ANISOTROPIC
%define wxMM_HIENGLISH
%define wxMM_HIMETRIC
%define wxMM_ISOTROPIC
%define wxMM_LOENGLISH
%define wxMM_LOMETRIC
%define wxMM_METRIC
%define wxMM_POINTS
%define wxMM_TEXT
%define wxMM_TWIPS

%define wxROP_BLACK
%define wxROP_COPYPEN
%define wxROP_MASKNOTPEN
%define wxROP_MASKPEN
%define wxROP_MASKPENNOT
%define wxROP_MERGENOTPEN
%define wxROP_MERGEPEN
%define wxROP_MERGEPENNOT
%define wxROP_NOP
%define wxROP_NOT
%define wxROP_NOTCOPYPEN
%define wxROP_NOTMASKPEN
%define wxROP_NOTMERGEPEN
%define wxROP_NOTXORPEN
%define wxROP_WHITE
%define wxROP_XORPEN

%define wxBLIT_00220326
%define wxBLIT_007700E6
%define wxBLIT_00990066
%define wxBLIT_00AA0029
%define wxBLIT_00DD0228
%define wxBLIT_BLACKNESS
%define wxBLIT_DSTINVERT
%define wxBLIT_MERGEPAINT
%define wxBLIT_NOTSCRCOPY
%define wxBLIT_NOTSRCERASE
%define wxBLIT_SRCAND
%define wxBLIT_SRCCOPY
%define wxBLIT_SRCERASE
%define wxBLIT_SRCINVERT
%define wxBLIT_SRCPAINT
%define wxBLIT_WHITENESS

%define wxCLEAR
%define wxXOR
%define wxINVERT
%define wxOR_REVERSE
%define wxAND_REVERSE
%define wxCOPY
%define wxAND
%define wxAND_INVERT
%define wxNO_OP
%define wxNOR
%define wxEQUIV
%define wxSRC_INVERT
%define wxOR_INVERT
%define wxNAND
%define wxOR
%define wxSET

%define wxFLOOD_BORDER
%define wxFLOOD_SURFACE

%define wxODDEVEN_RULE
%define wxWINDING_RULE

%class %delete wxDC, wxObject
    %win wxDC()
    void BeginDrawing()
    bool Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC* source, wxCoord xsrc, wxCoord ysrc, int logicalFunc = wxCOPY, bool useMask = FALSE)
    void CalcBoundingBox(wxCoord x, wxCoord y)
    void Clear()
    void CrossHair(wxCoord x, wxCoord y)
    void DestroyClippingRegion()
    wxCoord DeviceToLogicalX(wxCoord x)
    wxCoord DeviceToLogicalXRel(wxCoord x)
    wxCoord DeviceToLogicalY(wxCoord y)
    wxCoord DeviceToLogicalYRel(wxCoord y)
    void DrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
    void DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent)
    void DrawCheckMark(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
    %rename DrawCheckMarkRect void DrawCheckMark(const wxRect &rect)
    void DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
    void DrawEllipticArc(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double start, double end)
    void DrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
    void DrawLines(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0)
    %rename DrawLinesList void DrawLines(wxList *points, wxCoord xoffset = 0, wxCoord yoffset = 0)
    void DrawPolygon(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE)
    %rename DrawPolygonList void DrawPolygon(wxList *points, wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE)
    void DrawPoint(wxCoord x, wxCoord y)
    void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
    void DrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
    void DrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20)
    // void DrawSpline(wxList *points)
    void DrawText(const wxString& text, wxCoord x, wxCoord y)
    void EndDoc()
    void EndDrawing()
    void EndPage()
    void FloodFill(wxCoord x, wxCoord y, const wxColour& colour, int style=wxFLOOD_SURFACE)
    // alias
    // wxBrush& GetBackground()          removed in 2.5
    const wxBrush& GetBackground() const
    int GetBackgroundMode() const
    // wxBrush& GetBrush()               removed in 2.5
    const wxBrush& GetBrush() const
    wxCoord GetCharHeight()
    wxCoord GetCharWidth()
    void GetClippingBox(wxCoord *x, wxCoord *y, wxCoord *width, wxCoord *height)
    const wxFont& GetFont() const
    int GetLogicalFunction()
    int GetMapMode()
    bool GetOptimization()
    const wxPen& GetPen() const
    // const wxPen& GetPen() const
    bool GetPixel(wxCoord x, wxCoord y, wxColour *colour)
    void GetSize(wxCoord *width, wxCoord *height)
    //        GetSize()  // Return a wxSize
    //        GetSize()  // Returns two values (width, height)
    const wxColour& GetTextBackground() const
    void GetTextExtent(const wxString& string, wxCoord *w, wxCoord *h, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL)
    const wxColour& GetTextForeground() const
    // const wxColour& GetTextForeground() const
    void GetUserScale(double *x, double *y)
    // void GetUserScale() // Return two values
    wxCoord LogicalToDeviceX(wxCoord x)
    wxCoord LogicalToDeviceXRel(wxCoord x)
    wxCoord LogicalToDeviceY(wxCoord y)
    wxCoord LogicalToDeviceYRel(wxCoord y)
    wxCoord MaxX()
    wxCoord MaxY()
    wxCoord MinX()
    wxCoord MinY()
    bool Ok()
    void ResetBoundingBox()
    void SetDeviceOrigin(wxCoord x, wxCoord y)
    void SetBackground(const wxBrush& brush)
    %property=Background, read, write
    void SetBackgroundMode(int mode)
    %property=BackgroundMode, read, write
    %rename SetClippingRegionXY void SetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
    void SetClippingRegion(const wxRegion& region)
    void SetPalette(const wxPalette& palette)
    void SetBrush(const wxBrush& brush)
    %property=Brush, read, write
    void SetFont(const wxFont& font)
    %property=Font, read, write
    // void SetMapMode(int unit)
    void SetOptimization(bool optimize)
    %property=Optimization, read, write
    void SetPen(const wxPen& pen)
    %property=Pen, read, write
    void SetTextBackground(const wxColour& colour)
    %property=TextBackground, read, write
    void SetTextForeground(const wxColour& colour)
    %property=TextForeground, read, write
    void SetUserScale(double xScale, double yScale)
    %property=UserScale, read, write
    bool StartDoc(const wxString& message)
    void StartPage()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMemoryDC

%include "wx/dcmemory.h"

%class %delete wxMemoryDC, wxDC
    wxMemoryDC()
    void SelectObject(const wxBitmap& bitmap)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxWindowDC

%include "wx/dcclient.h"

%class %delete wxWindowDC, wxDC
    wxWindowDC(wxWindow* window)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxClientDC

%include "wx/dcclient.h"

%class %delete wxClientDC, wxWindowDC
    wxClientDC(wxWindow* window)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPaintDC

%include "wx/dcclient.h"

%class %delete wxPaintDC, wxWindowDC
    wxPaintDC(wxWindow* window)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxScreenDC

%include "wx/dcscreen.h"

%class %delete wxScreenDC, wxDC
    wxScreenDC()
    static bool StartDrawingOnTop(wxWindow* window)
    %rename StartDrawingOnTopRect static bool StartDrawingOnTop(wxRect* rect = NULL)
    static bool EndDrawingOnTop()
%endclass

%endif wxLUA_USE_wxDC

///////////////////////////////////////////////////////////////////////////////
// wxCaret

%if wxLUA_USE_wxCaret

%include "wx/caret.h"

%class %noclassinfo wxCaret
    wxCaret(wxWindow* window, const wxSize& size)
    %constructor wxDefaultCaret()
    %constructor wxCaretFromWidthHeight(wxWindow* window, int width, int height)
    bool Create(wxWindow* window, const wxSize& size)
    %rename CreateFromWidthHeight bool Create(wxWindow* window, int width, int height)
    %rename GetPositionXY void GetPosition(int *x, int *y)
    wxPoint GetPosition()
    %property=Position, read
    %rename GetSizeXY void GetSize(int *x, int *y)
    wxSize GetSize()
    wxWindow *GetWindow()
    %property=Window, read
    bool IsOk()
    bool IsVisible()
    %rename MoveXY void Move(int x, int y)
    void Move(const wxPoint& pt)
    %rename SetSizeWH void SetSize(int width, int height)
    void SetSize(const wxSize& size)
    %property=Size, read, write
    void Show(bool show = TRUE)
    void Hide()
    static int GetBlinkTime()
    static void SetBlinkTime(int ms)
    %property=BlinkTime, read, write
%endclass

%endif wxLUA_USE_wxCaret
