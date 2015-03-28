/////////////////////////////////////////////////////////////////////////////
// Purpose:     printing related classes
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxPrint

%typedef wxPreviewWindow wxScrolledWindow

%include "wx/print.h"

///////////////////////////////////////////////////////////////////////////////
// wxPrintout

%class wxPrintout, wxObject
    wxDC * GetDC()
    void GetPageSizeMM(int *w, int *h)
    void GetPageSizePixels(int *w, int *h)
    void GetPPIPrinter(int *w, int *h)
    void GetPPIScreen(int *w, int *h)
    bool IsPreview()
    bool HasPage(int pageNum)
    bool OnBeginDocument(int startPage, int endPage)
    void OnEndDocument()
    void OnBeginPrinting()
    void OnEndPrinting()
    void OnPreparePrinting()
    bool OnPrintPage(int pageNum)
%endclass

%if wxLUA_USE_wxLuaPrintout

///////////////////////////////////////////////////////////////////////////////
// wxPrinter

%enum wxPrinterError
    wxPRINTER_NO_ERROR
    wxPRINTER_CANCELLED
    wxPRINTER_ERROR
%end

%class %delete %noclassinfo wxPrinter, wxObject
    wxPrinter(wxPrintDialogData* data = NULL)
    //bool Abort()
    void CreateAbortWindow(wxWindow* parent, wxLuaPrintout* printout)
    static wxPrinterError GetLastError()
    wxPrintDialogData& GetPrintDialogData()
    bool Print(wxWindow *parent, wxLuaPrintout *printout, bool prompt=TRUE)
    wxDC* PrintDialog(wxWindow *parent)
    ~%wxver22 void ReportError(wxWindow *parent, wxLuaPrintout *printout, const wxString& message)
    bool Setup(wxWindow *parent)
%endclass

%endif wxLUA_USE_wxLuaPrintout

///////////////////////////////////////////////////////////////////////////////
// wxPrintData

%define wxPORTRAIT
%define wxLANDSCAPE

%enum wxDuplexMode
    wxDUPLEX_HORIZONTAL
    wxDUPLEX_SIMPLEX
    wxDUPLEX_VERTICAL
%end

%enum wxPaperSize
    wxPAPER_10X11
    wxPAPER_10X14
    wxPAPER_11X17
    wxPAPER_15X11
    wxPAPER_9X11
    wxPAPER_A_PLUS
    wxPAPER_A2
    wxPAPER_A3
    wxPAPER_A3_EXTRA
    wxPAPER_A3_EXTRA_TRANSVERSE
    wxPAPER_A3_TRANSVERSE
    wxPAPER_A4
    wxPAPER_A4_EXTRA
    wxPAPER_A4_PLUS
    wxPAPER_A4_TRANSVERSE
    wxPAPER_A4SMALL
    wxPAPER_A5
    wxPAPER_A5_EXTRA
    wxPAPER_A5_TRANSVERSE
    wxPAPER_B_PLUS
    wxPAPER_B4
    wxPAPER_B5
    wxPAPER_B5_EXTRA
    wxPAPER_B5_TRANSVERSE
    wxPAPER_CSHEET
    wxPAPER_DSHEET
    wxPAPER_ENV_10
    wxPAPER_ENV_11
    wxPAPER_ENV_12
    wxPAPER_ENV_14
    wxPAPER_ENV_9
    wxPAPER_ENV_B4
    wxPAPER_ENV_B5
    wxPAPER_ENV_B6
    wxPAPER_ENV_C3
    wxPAPER_ENV_C4
    wxPAPER_ENV_C5
    wxPAPER_ENV_C6
    wxPAPER_ENV_C65
    wxPAPER_ENV_DL
    wxPAPER_ENV_INVITE
    wxPAPER_ENV_ITALY
    wxPAPER_ENV_MONARCH
    wxPAPER_ENV_PERSONAL
    wxPAPER_ESHEET
    wxPAPER_EXECUTIVE
    wxPAPER_FANFOLD_LGL_GERMAN
    wxPAPER_FANFOLD_STD_GERMAN
    wxPAPER_FANFOLD_US
    wxPAPER_FOLIO
    wxPAPER_ISO_B4
    wxPAPER_JAPANESE_POSTCARD
    wxPAPER_LEDGER
    wxPAPER_LEGAL
    wxPAPER_LEGAL_EXTRA
    wxPAPER_LETTER
    wxPAPER_LETTER_EXTRA
    wxPAPER_LETTER_EXTRA_TRANSVERSE
    wxPAPER_LETTER_PLUS
    wxPAPER_LETTER_TRANSVERSE
    wxPAPER_LETTERSMALL
    wxPAPER_NONE
    wxPAPER_NOTE
    wxPAPER_QUARTO
    wxPAPER_STATEMENT
    wxPAPER_TABLOID
    wxPAPER_TABLOID_EXTRA
%end

%enum wxPrintQuality
    wxPRINT_QUALITY_DRAFT
    wxPRINT_QUALITY_HIGH
    wxPRINT_QUALITY_LOW
    wxPRINT_QUALITY_MEDIUM
%end

%enum wxPrintMode
    wxPRINT_MODE_FILE
    wxPRINT_MODE_NONE
    wxPRINT_MODE_PREVIEW
    wxPRINT_MODE_PRINTER
%end

%class %delete wxPrintData, wxObject
    wxPrintData()
    wxPrintData *Copy()
    bool GetCollate()
    bool GetColour()
    wxDuplexMode GetDuplex()
    int GetNoCopies()
    int  GetOrientation()
    wxPaperSize GetPaperId()
    const wxString& GetPrinterName()
    wxPrintQuality GetQuality()
    const wxSize& GetPaperSize()
    const wxString& GetPrinterCommand()
    const wxString& GetPrinterOptions()
    const wxString& GetPreviewCommand()
    const wxString& GetFilename()
    const wxString& GetFontMetricPath()
    double GetPrinterScaleX()
    double GetPrinterScaleY()
    long GetPrinterTranslateX()
    long GetPrinterTranslateY()
    wxPrintMode GetPrintMode()
    void SetCollate(bool flag)
    %property=Collate, read, write
    void SetColour(bool flag)
    %property=Colour, read, write
    void SetDuplex(wxDuplexMode duplex)
    %property=Duplex, read, write
    void SetNoCopies(int v)
    %property=NoCopies, read, write
    void SetOrientation(int orientation)
    %property=Orientation, read, write
    void SetPaperId(wxPaperSize sizeId)
    %property=PaperId, read, write
    void SetPrinterName(const wxString& printerName)
    %property=PrinterName, read, write
    void SetQuality(wxPrintQuality quality)
    %property=Quality, read, write
    void SetPaperSize(const wxSize& paperSize)
    %property=PaperSize, read, write
    void SetPrinterCommand(const wxString& command)
    %property=PrinterCommand, read, write
    void SetPrinterOptions(const wxString& options)
    %property=PrinterOptions, read, write
    void SetPreviewCommand(const wxString& command)
    %property=PreviewCommand, read, write
    void SetFilename(const wxString& filename)
    %property=Filename, read, write
    void SetFontMetricPath(const wxString& path)
    %property=FontMetricPath, read, write
    void SetPrinterScaleX(double x)
    %property=PrinterScaleX, read, write
    void SetPrinterScaleY(double y)
    %property=PrinterScaleY, read, write
    void SetPrinterScaling(double x, double y)
    void SetPrinterTranslateX(long x)
    %property=PrinterTranslateX, read, write
    void SetPrinterTranslateY(long y)
    %property=PrinterTranslateY, read, write
    void SetPrinterTranslation(long x, long y)
    %property=PrintMode, read, write
    void SetPrintMode(wxPrintMode printMode)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPageSetupDialogData

%class %delete wxPageSetupDialogData, wxObject
    wxPageSetupDialogData()
    void EnableHelp(bool flag)
    void EnableMargins(bool flag)
    void EnableOrientation(bool flag)
    void EnablePaper(bool flag)
    void EnablePrinter(bool flag)
    bool GetDefaultMinMargins()
    bool GetEnableMargins()
    %win %property=EnableMargins, read
    bool GetEnableOrientation()
    %win %property=EnableOrientation, read
    bool GetEnablePaper()
    %win %property=EnablePaper, read
    bool GetEnablePrinter()
    %win %property=EnablePrinter, read
    bool GetEnableHelp()
    %win %property=EnableHelp, read
    bool GetDefaultInfo()
    wxPoint GetMarginTopLeft()
    wxPoint GetMarginBottomRight()
    wxPoint GetMinMarginTopLeft()
    wxPoint GetMinMarginBottomRight()
    wxPaperSize GetPaperId()
    wxSize GetPaperSize()
    const wxPrintData& GetPrintData()
    void SetDefaultInfo(bool flag)
    %property=DefaultInfo, read, write
    void SetDefaultMinMargins(bool flag)
    %property=DefaultMinMargins, read, write
    void SetMarginTopLeft(const wxPoint& pt)
    %property=MarginTopLeft, read, write
    void SetMarginBottomRight(const wxPoint& pt)
    %property=MarginBottomRight, read, write
    void SetMinMarginTopLeft(const wxPoint& pt)
    %property=MinMarginTopLeft, read, write
    void SetMinMarginBottomRight(const wxPoint& pt)
    %property=MinMarginBottomRight, read, write
    void SetPaperId(wxPaperSize id)
    %property=PaperId, read, write
    void SetPaperSize(const wxSize& size)
    %property=PaperSize, read, write
    void SetPrintData(const wxPrintData& printData)
    %property=PrintData, read, write
    wxPageSetupDialogData *Copy()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPageSetupDialog

%include "wx/printdlg.h"

%class wxPageSetupDialog, wxObject
    wxPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data = NULL)
    wxPageSetupDialogData& GetPageSetupData()
    %property=PageSetupData, read
    int ShowModal()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPrintDialog

%class wxPrintDialog, wxObject
    wxPrintDialog(wxWindow* parent, wxPrintDialogData* data = NULL)
    wxPrintDialogData& GetPrintDialogData()
    %property=PrintDialogData, read
    wxDC* GetPrintDC()
    %property=PrintDC, read
    int ShowModal()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPrintDialogData

%class %delete wxPrintDialogData, wxObject
    wxPrintDialogData()
    %constructor wxPrintDialogDataFromDialogData(wxPrintDialogData& dialogData)
    %constructor wxPrintDialogDataFromPrintData(wxPrintData& data)
    void EnableHelp(bool flag)
    void EnablePageNumbers(bool flag)
    void EnablePrintToFile(bool flag)
    void EnableSelection(bool flag)
    bool GetAllPages()
    %property=AllPages, read
    bool GetCollate()
    int GetFromPage()
    int GetMinPage()
    int GetMaxPage()
    int GetNoCopies()
    const wxPrintData& GetPrintData()
    bool GetPrintToFile()
    int GetToPage()
    void SetCollate(bool flag)
    %property=Collate, read, write
    void SetFromPage(int page)
    %property=FromPage, read, write
    void SetMinPage(int page)
    %property=MinPage, read, write
    void SetMaxPage(int page)
    %property=MaxPage, read, write
    void SetNoCopies(int n)
    %property=NoCopies, read, write
    void SetPrintData(const wxPrintData& printData)
    %property=PrintData, read, write
    void SetPrintToFile(bool flag)
    %property=PrintToFile, read, write
    void SetSelection(bool flag)
    void SetSetupDialog(bool flag)
    void SetToPage(int page)
    %property=ToPage, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPreviewCanvas

%class %noclassinfo wxPreviewCanvas, wxScrolledWindow
    wxPreviewCanvas(wxPrintPreview *preview, wxWindow *parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "canvas")
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPrintPreview
%if wxLUA_USE_wxLuaPrintout

%class %noclassinfo wxPrintPreview, wxObject
    wxPrintPreview(wxLuaPrintout* printout, wxLuaPrintout* printoutForPrinting, wxPrintData* data=NULL)
    bool DrawBlankPage(wxPreviewCanvas* window, wxDC& dc)
    wxWindow* GetCanvas()
    int GetCurrentPage()
    wxFrame * GetFrame()
    int GetMaxPage()
    int GetMinPage()
    //const wxPrintData& GetPrintData()
    wxPrintout * GetPrintout()
    wxPrintout * GetPrintoutForPrinting()
    bool Ok()
    bool PaintPage(wxPreviewCanvas* window, wxDC &dc)
    bool Print(bool prompt)
    bool RenderPage(int pageNum)
    void SetCanvas(wxPreviewCanvas* window)
    %property=Canvas, read, write
    void SetCurrentPage(int pageNum)
    void SetFrame(wxFrame *frame)
    %property=Frame, read, write
    void SetPrintout(wxLuaPrintout *printout)
    %property=Printout, read, write
    void SetZoom(int percent)
%endclass

////////////////////////////////////////////////////////////////////////////////
// wxPreviewFrame

%class wxPreviewFrame, wxFrame
    wxPreviewFrame(wxPrintPreview *preview, wxFrame *parent, const wxString &title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString &name = "frame")
    void CreateControlBar()
    void CreateCanvas()
    void Initialize()
%endclass

%endif wxLUA_USE_wxWindowList

///////////////////////////////////////////////////////////////////////////////
// wxPostScriptDC

//%class %delete wxPostScriptDC, wxDC
//  %include "wx/dcps.h"
//  wxPostScriptDC(const wxPrintData& printData)
//  static void SetResolution(int ppi)
//  static int GetResolution()
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxPrinterDC

%gtk %skip 
%class %delete wxPrinterDC, wxDC
    %include "wx/dcprint.h"
    wxPrinterDC(const wxPrintData& printData)
%endclass

%endif wxLUA_USE_wxPrint

