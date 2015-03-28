/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxHelpController and help related classes
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// wxHelpController and derived help controller classes

%if wxLUA_USE_wxHelpController

%include "wx/help.h"
%include "wx/helpbase.h"

%class %delete wxHelpController, wxObject
    wxHelpController()
    virtual void Initialize(const wxString& file)
    // %rename InitializeServer virtual void Initialize(const wxString& file, int server)
    virtual bool DisplayBlock(long blockNo)
    virtual bool DisplayContents()
    ~%wxver22 virtual bool DisplayContextPopup(int contextId)
    virtual bool DisplaySection(int sectionNo)
    // %rename DisplaySectionString virtual bool DisplaySection(const wxString &section)
    ~%wxver22 virtual bool DisplayTextPopup(const wxString& text, const wxPoint& pos)
    virtual wxFrame * GetFrameParameters(wxSize * size = NULL, wxPoint * pos = NULL, bool *newFrameEachTime = NULL)
    virtual bool KeywordSearch(const wxString& keyWord)
    virtual bool LoadFile(const wxString& file = "")
    virtual void SetFrameParameters(const wxString & title, const wxSize & size, const wxPoint & pos = wxDefaultPosition, bool newFrameEachTime = FALSE)
    virtual void SetViewer(const wxString& viewer, long flags)
    virtual bool Quit()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxWinHelpController

%gtk %skip
%mac %skip
%class %delete wxWinHelpController, wxHelpController
    %include "wx/helpwin.h"
    wxWinHelpController()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCHMHelpController

//%class %delete wxCHMHelpController, wxHelpController
//%include "wx/msw/helpchm.h"
//wxCHMHelpController()
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxBestHelpController

%gtk %skip
%class %noclassinfo wxBestHelpController, wxHelpController
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxExtHelpController

%win %skip
%class %delete wxExtHelpController, wxHelpController
    %include "wx/generic/helpext.h"
    wxExtHelpController()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHelpControllerHtml

//%class %delete wxHelpControllerHtml, wxHelpController
//%include "wx/generic/helpwxht.h"
//wxHelpControllerHtml()
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlHelpData

%if wxLUA_USE_wxHtmlHelpController

%struct %noclassinfo wxHtmlContentsItem
    // needs access functions
%endstruct

%class %delete wxHtmlHelpData, wxObject
    %include "wx/html/helpdata.h"
    wxHtmlHelpData()
    bool AddBook(const wxString& book)
    wxString FindPageById(int id)
    wxString FindPageByName(const wxString& page)
    //const wxHtmlBookRecArray& GetBookRecArray()
    wxHtmlContentsItem* GetContents()
    %property=Contents, read
    int GetContentsCnt()
    %property=ContentsCnt, read
    wxHtmlContentsItem* GetIndex()
    %property=Index, read
    int GetIndexCnt()
    %property=IndexCnt, read
    void SetTempDir(const wxString& path)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxHtmlHelpController

%include "wx/html/helpctrl.h"

%class %delete wxHtmlHelpController, wxHelpController
    %define wxHF_TOOLBAR
    ~%wxver22 %define wxHF_FLAT_TOOLBAR
    %define wxHF_CONTENTS
    %define wxHF_INDEX
    %define wxHF_SEARCH
    %define wxHF_BOOKMARKS
    ~%wxver22 %define wxHF_OPEN_FILES
    %define wxHF_PRINT
    ~%wxver22 %define wxHF_MERGE_BOOKS
    ~%wxver22 %define wxHF_ICONS_BOOK
    ~%wxver22 %define wxHF_ICONS_FOLDER
    ~%wxver22 %define wxHF_ICONS_BOOK_CHAPTER
    ~%wxver22 %define wxHF_DEFAULT_STYLE
    %wxver22 wxHtmlHelpController()
    ~%wxver22 wxHtmlHelpController(int style = wxHF_DEFAULT_STYLE)
    bool AddBook(const wxString& book, bool show_wait_msg)
    //virtual wxHtmlHelpFrame* CreateHelpFrame(wxHtmlHelpData * data)
    void Display(const wxString& x)
    %rename DisplayId void Display(const int id)
    void DisplayContents()
    void DisplayIndex()
    bool KeywordSearch(const wxString& keyword)
    void ReadCustomization(wxConfigBase* cfg, wxString path = wxEmptyString)
    void SetTempDir(const wxString& path)
    void SetTitleFormat(const wxString& format)
    void UseConfig(wxConfigBase* config, const wxString& rootpath = wxEmptyString)
    void WriteCustomization(wxConfigBase* cfg, wxString path = wxEmptyString)
    %rename DisplaySectionString virtual bool DisplaySection(const wxString &section)
%endclass

%endif wxLUA_USE_wxHtmlHelpController
%endif wxLUA_USE_wxHelpController

