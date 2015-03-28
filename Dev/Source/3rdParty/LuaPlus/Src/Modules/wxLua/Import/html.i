/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxHtmlWindow and everything from include/wx/html/*.h
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxHTML

///////////////////////////////////////////////////////////////////////////////
// wxHtmlCell

%include "wx/html/htmlcell.h"

%class %noclassinfo wxHtmlCell
    wxHtmlCell()
    virtual bool AdjustPagebreak(int * pagebreak)
    // these two have changed in wxWin 2.5
    //virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2)
    //virtual void DrawInvisible(wxDC& dc, int x, int y)
    virtual const wxHtmlCell* Find(int condition, void *param = 0)
    int GetDescent() const
    %property=Descent, read
    int GetHeight() const
    %property=Height, read
    ~%wxver22 virtual wxString GetId() const
    virtual wxHtmlLinkInfo* GetLink(int x = 0, int y = 0) const
    wxHtmlCell* GetNext() const
    wxHtmlContainerCell* GetParent() const
    int GetPosX() const
    %property=PosX, read
    int GetPosY() const
    %property=PosY, read
    int GetWidth() const
    %property=Width, read
    virtual void Layout(int w)
    //virtual void OnMouseClick(wxWindow* parent, intx, int y, const wxMouseEvent& event)
    ~%wxver22 void SetId(const wxString& id)
    ~%wxver22 %property=Id, read, write
    void SetLink(const wxHtmlLinkInfo& link)
    void SetNext(wxHtmlCell *cell)
    %property=Next, read, write
    void SetParent(wxHtmlContainerCell *p)
    %property=Parent, read, write
    void SetPos(int x, int y)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlWidgetCell

%class %noclassinfo wxHtmlWidgetCell, wxHtmlCell
    wxHtmlWidgetCell(wxWindow* wnd, int w = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlLinkInfo

%include "wx/html/htmlcell.h"

%class %noclassinfo wxHtmlLinkInfo
    wxHtmlLinkInfo(const wxString& href, const wxString& target = wxEmptyString)
    const wxMouseEvent * GetEvent()
    %property=Event, read
    const wxHtmlCell * GetHtmlCell()
    %property=HtmlCell, read
    wxString GetHref()
    %property=Href, read
    wxString GetTarget()
    %property=Target, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlTag

%include "wx/html/htmltag.h"

%class wxHtmlTag
    //wxHtmlTag(const wxString& source, int pos, int end_pos, wxHtmlTagsCache* cache)
    const wxString GetAllParams() const
    int GetBeginPos() const
    %property=BeginPos, read
    int GetEndPos1() const
    %property=EndPos1, read
    int GetEndPos2() const
    %property=EndPos2, read
    wxString GetName() const
    %property=Name, read
    wxString GetParam(const wxString& par, bool with_commas = FALSE) const
    bool HasEnding() const
    bool HasParam(const wxString& par) const
    //bool IsEnding() const
    //wxString ScanParam(const wxString& par, const wxString &format, void *value) const
    ~%wxver22 bool GetParamAsColour(const wxString& par, wxColour *clr) const
    ~%wxver22 bool GetParamAsInt(const wxString& par, int *value) const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlContainerCell

%include "wx/html/htmlcell.h"

%define wxHTML_UNITS_PIXELS
%define wxHTML_UNITS_PERCENT
%define wxHTML_INDENT_TOP
%define wxHTML_INDENT_BOTTOM
%define wxHTML_INDENT_LEFT
%define wxHTML_INDENT_RIGHT
%define wxHTML_INDENT_HORIZONTAL
%define wxHTML_INDENT_VERTICAL
%define wxHTML_INDENT_ALL
%define wxHTML_ALIGN_LEFT
%define wxHTML_ALIGN_JUSTIFY
%define wxHTML_ALIGN_CENTER
%define wxHTML_ALIGN_RIGHT
%define wxHTML_ALIGN_BOTTOM
%define wxHTML_ALIGN_CENTER
%define wxHTML_ALIGN_TOP

%class %noclassinfo wxHtmlContainerCell, wxHtmlCell
    wxHtmlContainerCell(wxHtmlContainerCell *parent)
    int GetAlignHor() const
    int GetAlignVer() const
    ~<wxver25 wxHtmlCell* GetFirstChild()
    <wxver25|%wxcompat24 wxHtmlCell* GetFirstCell()
    <wxver25|%wxcompat24 %property=FirstCell, read
    int GetIndent(int ind) const
    int GetIndentUnits(int ind) const
    void InsertCell(wxHtmlCell *cell)
    void SetAlign(const wxHtmlTag& tag)
    void SetAlignHor(int align)
    %property=AlignHor, read, write
    void SetAlignVer(int align)
    %property=AlignVer, read, write
    void SetBackgroundColour(const wxColour& clr)
    void SetBorder(const wxColour& clr1, const wxColour& clr2)
    void SetIndent(int i, int what, int units = wxHTML_UNITS_PIXELS)
    void SetMinHeight(int h, int align = wxHTML_ALIGN_TOP)
    void SetWidthFloat(int w, int units)
    %rename SetWidthFloatFromTag void SetWidthFloat(const wxHtmlTag& tag, double pixel_scale = 1.0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlWindow

%include "wx/wxhtml.h"

%define wxHW_SCROLLBAR_NEVER
%define wxHW_SCROLLBAR_AUTO

%class wxHtmlWindow, wxScrolledWindow
    wxHtmlWindow(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHW_SCROLLBAR_AUTO, const wxString& name = "htmlWindow")
    //static void AddFilter(wxHtmlFilter *filter)
    ~%wxver22 bool AppendToPage(const wxString& source)
    wxHtmlContainerCell* GetInternalRepresentation() const
    %property=InternalRepresentation, read
    wxString GetOpenedAnchor()
    %property=OpenedAnchor, read
    wxString GetOpenedPage()
    %property=OpenedPage, read
    wxString GetOpenedPageTitle()
    %property=OpenedPageTitle, read
    wxFrame* GetRelatedFrame() const
    %property=RelatedFrame, read
    ~%wxver22 bool HistoryBack()
    ~%wxver22 bool HistoryCanBack()
    ~%wxver22 bool HistoryCanForward()
    ~%wxver22 void HistoryClear()
    ~%wxver22 bool HistoryForward()
    bool LoadPage(const wxString& location)
    void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString)
    void SetBorders(int b)
    void SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
    bool SetPage(const wxString& source)
    void SetRelatedFrame(wxFrame* frame, const wxString& format)
    void SetRelatedStatusBar(int bar)
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlParser

%class %noclassinfo wxHtmlParser
    //wxHtmlParser()
    //void AddTag(const wxHtmlTag& tag)
    //void AddTagHandler(wxHtmlTagHandler *handler)
    //void AddWord(const wxString &txt)
    void DoParsing(int begin_pos, int end_pos)
    %rename DoParsingWhole void DoParsing()
    virtual void DoneParser()
    //wxString* GetSource()
    void InitParser(const wxString& source)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlWinParser

%class %noclassinfo wxHtmlWinParser, wxHtmlParser
    wxHtmlWinParser(wxHtmlWindow *wnd)
    wxHtmlContainerCell* CloseContainer()
    wxFont* CreateCurrentFont()
    wxColour GetActualColor() const
    int GetAlign() const
    int GetCharHeight() const
    int GetCharWidth() const
    wxHtmlContainerCell* GetContainer() const
    wxDC* GetDC()
    int GetFontBold() const
    wxString GetFontFace() const
    int GetFontFixed() const
    int GetFontItalic() const
    int GetFontSize() const
    int GetFontUnderlined() const
    const wxHtmlLinkInfo& GetLink() const
    wxColour GetLinkColor() const
    wxWindow* GetWindow()
    %property=Window, read
    wxHtmlContainerCell* OpenContainer()
    void SetActualColor(const wxColour& clr)
    %property=ActualColor, read, write
    void SetAlign(int a)
    %property=Align, read, write
    wxHtmlContainerCell* SetContainer(wxHtmlContainerCell *c)
    %property=Container, read, write
    void SetDC(wxDC *dc, double pixel_scale = 1.0)
    %property=DC, read, write
    void SetFontBold(int x)
    %property=FontBold, read, write
    void SetFontFace(const wxString& face)
    %property=FontFace, read, write
    void SetFontFixed(int x)
    %property=FontFixed, read, write
    void SetFontItalic(int x)
    %property=FontItalic, read, write
    void SetFontSize(int s)
    %property=FontSize, read, write
    void SetFontUnderlined(int x)
    %property=FontUnderlined, read, write
    void SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
    void SetLink(const wxHtmlLinkInfo& link)
    %property=Link, read, write
    void SetLinkColor(const wxColour& clr)
    %property=LinkColor, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlDCRenderer

%include "wx/html/htmprint.h"

%class %delete %noclassinfo wxHtmlDCRenderer, wxObject
    wxHtmlDCRenderer()
    void SetDC(wxDC* dc, double pixel_scale = 1.0)
    void SetSize(int width, int height)
    void SetHtmlText(const wxString& html, const wxString& basepath = wxEmptyString, bool isdir = TRUE)
    int Render(int x, int y, int from = 0, int dont_render = FALSE)
    int GetTotalHeight()
    %property=TotalHeight, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlEasyPrinting

%include "wx/html/htmprint.h"

%class %delete %noclassinfo wxHtmlEasyPrinting, wxObject
    wxHtmlEasyPrinting(const wxString& name = "Printing", wxFrame* parent_frame = NULL)
    bool PreviewFile(const wxString& htmlfile)
    bool PreviewText(const wxString& htmltext, const wxString& basepath = wxEmptyString)
    bool PrintFile(const wxString& htmlfile)
    bool PrintText(const wxString& htmltext, const wxString& basepath = wxEmptyString)
    void PageSetup()
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL)
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL)
    void SetFonts(wxString normal_face, wxString fixed_face, const int *sizes = 0)
    void SetStandardFonts(int size = -1,const wxString& normal_face = wxEmptyString,const wxString& fixed_face = wxEmptyString)
    wxPrintData* GetPrintData()
    %property=PrintData, read
    wxPageSetupDialogData* GetPageSetupData()
    %property=PageSetupData, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlPrintout

%include "wx/html/htmprint.h"

%class %noclassinfo wxHtmlPrintout, wxPrintout
    wxHtmlPrintout(const wxString& title = "Printout")
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL)
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL)
    void SetHtmlFile(const wxString& htmlfile)
    void SetHtmlText(const wxString& html, const wxString& basepath = wxEmptyString, bool isdir = TRUE)
    void SetMargins(float top = 25.2, float bottom = 25.2, float left = 25.2, float right = 25.2, float spaces = 5)
%endclass

%endif wxLUA_USE_wxHTML
