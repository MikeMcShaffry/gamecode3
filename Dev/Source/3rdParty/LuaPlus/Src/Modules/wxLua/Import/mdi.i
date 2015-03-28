/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxMDI classes
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_MDI

///////////////////////////////////////////////////////////////////////////////
// wxMDIClientWindow

%class wxMDIClientWindow, wxWindow
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMDIParentFrame

%class wxMDIParentFrame, wxFrame
    %constructor wxMDIParentFrameDefault()
    wxMDIParentFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL, const wxString& name = "frame")
    void ActivateNext()
    void ActivatePrevious()
    void ArrangeIcons()
    void Cascade()
    bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL, const wxString& name = "frame")
    virtual void GetClientSize(int* width, int* height) const
    wxMDIChildFrame* GetActiveChild() const
    %property=ActiveChild, read
    wxMDIClientWindow* GetClientWindow() const
    %property=ClientWindow, read
    virtual wxWindow* GetToolBar() const
    %win wxMenu* GetWindowMenu() const
    virtual void SetToolBar(wxToolBar* toolbar)
    %property=ToolBar, read, write
    %win void SetWindowMenu(wxMenu* menu)
    %win %property=WindowMenu, read, write
    void Tile()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMDIChildFrame

%class wxMDIChildFrame, wxFrame
    %constructor wxMDIChildFrameDefault()
    wxMDIChildFrame(wxMDIParentFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    void Activate()
    bool Create(wxMDIParentFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    %win void Maximize()
    void Restore()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocChildFrame

%include "wx/docview.h"

%class wxDocChildFrame, wxFrame
    wxDocChildFrame(wxDocument* doc, wxView* view, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    wxDocument* GetDocument() const
    wxView* GetView() const
    void SetDocument(wxDocument *doc)
    void SetView(wxView *view)
    %property=View, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocManager

%class wxDocManager, wxEvtHandler
    %define wxDEFAULT_DOCMAN_FLAGS
    %define wxDOC_NEW
    %define wxDOC_SILENT
    wxDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = TRUE)
    ~<wxver25 void ActivateView(wxView* view, bool activate)
    <wxver25 void ActivateView(wxView* view, bool activate, bool deleting)
    void AddDocument(wxDocument *doc)
    void AddFileToHistory(const wxString& filename)
    void AssociateTemplate(wxDocTemplate *temp)
    wxDocument* CreateDocument(const wxString& path, long flags)
    wxView* CreateView(wxDocument*doc, long flags)
    void DisassociateTemplate(wxDocTemplate *temp)
    void FileHistoryAddFilesToMenu()
    %rename FileHistoryAddFilesToGivenMenu void FileHistoryAddFilesToMenu(wxMenu* menu)
    void FileHistoryLoad(wxConfigBase& config)
    void FileHistoryRemoveMenu(wxMenu* menu)
    void FileHistorySave(wxConfigBase& resourceFile)
    void FileHistoryUseMenu(wxMenu* menu)
    wxDocTemplate * FindTemplateForPath(const wxString& path)
    wxDocument * GetCurrentDocument()
    wxView * GetCurrentView()
    wxList& GetDocuments()
    wxFileHistory * GetFileHistory()
    wxString GetLastDirectory() const
    int GetMaxDocsOpen()
    int GetNoHistoryFiles()
    bool Initialize()
    bool MakeDefaultName(const wxString& buf)
    wxFileHistory * OnCreateFileHistory()
    void OnFileClose(wxCommandEvent &event)
    void OnFileNew(wxCommandEvent &event)
    void OnFileOpen(wxCommandEvent &event)
    void OnFileSave(wxCommandEvent &event)
    void OnFileSaveAs(wxCommandEvent &event)
    //void OnMenuCommand(int cmd)
    void RemoveDocument(wxDocument *doc)
    //wxDocTemplate * SelectDocumentPath(wxDocTemplate **templates, int noTemplates, const wxString& path, const wxString& bufSize, long flags, bool save)
    //wxDocTemplate * SelectDocumentType(wxDocTemplate **templates, int noTemplates, bool sort = FALSE)
    //wxDocTemplate * SelectViewType(wxDocTemplate **templates, int noTemplates, bool sort = FALSE)
    void SetLastDirectory(const wxString& dir)
    %property=LastDirectory, read, write
    void SetMaxDocsOpen(int n)
    %property=MaxDocsOpen, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocMDIChildFrame

//%class wxDocMDIChildFrame, wxMDIChildFrame
//wxDocMDIChildFrame(wxDocument* doc, wxView* view, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
//wxDocument* GetDocument() const
//wxView* GetView() const
//void OnActivate(wxActivateEvent event)
//void OnCloseWindow(wxCloseEvent& event)
//void SetView(wxView *view)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocMDIParentFrame

//%class wxDocMDIParentFrame, wxMDIParentFrame
//wxDocMDIParentFrame(wxDocManager* manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
//void OnCloseWindow(wxCloseEvent& event)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocParentFrame

%class wxDocParentFrame, wxFrame
    wxDocParentFrame(wxDocManager* manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame")
    //void OnCloseWindow(wxCloseEvent& event)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocTemplate

%class wxDocTemplate, wxObject
    wxDocTemplate(wxDocManager* manager, const wxString& descr, const wxString& filter, const wxString& dir, const wxString& ext, const wxString& docTypeName, const wxString& viewTypeName, wxClassInfo* docClassInfo = NULL, wxClassInfo* viewClassInfo = NULL, long flags = wxDEFAULT_TEMPLATE_FLAGS)
    %define wxTEMPLATE_VISIBLE
    %define wxTEMPLATE_INVISIBLE
    %define wxDEFAULT_TEMPLATE_FLAGS
    wxDocument * CreateDocument(const wxString& path, long flags = 0)
    wxView * CreateView(wxDocument *doc, long flags = 0)
    wxString GetDefaultExtension()
    wxString GetDescription()
    wxString GetDirectory()
    wxDocManager * GetDocumentManager()
    wxString GetDocumentName()
    wxString GetFileFilter()
    long GetFlags()
    wxString GetViewName()
    bool IsVisible()
    void SetDefaultExtension(const wxString& ext)
    %property=DefaultExtension, read, write
    void SetDescription(const wxString& descr)
    %property=Description, read, write
    void SetDirectory(const wxString& dir)
    %property=Directory, read, write
    void SetDocumentManager(wxDocManager *manager)
    %property=DocumentManager, read, write
    void SetFileFilter(const wxString& filter)
    %property=FileFilter, read, write
    void SetFlags(long flags)
    %property=Flags, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDocument

%class wxDocument, wxEvtHandler
    wxDocument()
    virtual bool AddView(wxView *view)
    virtual bool Close()
    virtual bool DeleteAllViews()
    wxCommandProcessor* GetCommandProcessor() const
    wxDocTemplate* GetDocumentTemplate() const
    wxDocManager* GetDocumentManager() const
    wxString GetDocumentName() const
    wxWindow* GetDocumentWindow() const
    wxString GetFilename() const
    wxView * GetFirstView() const
    virtual void GetPrintableName(wxString& name) const
    wxString GetTitle() const
    wxList& GetViews() const
    virtual bool IsModified() const
    //virtual istream& LoadObject(istream& stream)
    //virtual wxInputStream& LoadObject(wxInputStream& stream)
    virtual void Modify(bool modify)
    virtual void OnChangedViewList()
    virtual bool OnCloseDocument()
    virtual bool OnCreate(const wxString& path, long flags)
    virtual wxCommandProcessor* OnCreateCommandProcessor()
    virtual bool OnNewDocument()
    virtual bool OnOpenDocument(const wxString& filename)
    virtual bool OnSaveDocument(const wxString& filename)
    virtual bool OnSaveModified()
    virtual bool RemoveView(wxView* view)
    virtual bool Save()
    virtual bool SaveAs()
    //virtual ostream& SaveObject(ostream& stream)
    //virtual wxOutputStream& SaveObject(wxOutputStream& stream)
    virtual void SetCommandProcessor(wxCommandProcessor *processor)
    void SetDocumentName(const wxString& name)
    %property=DocumentName, read, write
    void SetDocumentTemplate(wxDocTemplate* templ)
    %property=DocumentTemplate, read, write
    void SetFilename(const wxString& filename, bool notifyViews = FALSE)
    %property=Filename, read, write
    void SetTitle(const wxString& title)
    %property=Title, read, write
    void UpdateAllViews(wxView* sender = NULL, wxObject* hint = NULL)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxView

%class wxView, wxEvtHandler
    virtual void Activate(bool activate)
    virtual bool Close(bool deleteWindow = TRUE)
    wxDocument* GetDocument() const
    wxDocManager* GetDocumentManager() const
    wxWindow * GetFrame()
    wxString GetViewName() const
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView)
    virtual void OnChangeFilename()
    virtual bool OnClose(bool deleteWindow)
    virtual bool OnCreate(wxDocument* doc, long flags)
    virtual wxPrintout* OnCreatePrintout()
    virtual void OnUpdate(wxView* sender, wxObject* hint)
    void SetDocument(wxDocument* doc)
    %property=Document, read, write
    void SetFrame(wxFrame* frame)
    %property=Frame, read, write
    void SetViewName(const wxString& name)
    %property=ViewName, read, write
%endclass

%endif wxLUA_USE_MDI

///////////////////////////////////////////////////////////////////////////////
//  wxCommandProcessor

%if wxLUA_USE_wxCommandProcessor

~%wxver22 %include "wx/cmdproc.h"

%class wxCommandProcessor, wxObject
    wxCommandProcessor(int maxCommands = -1)
    virtual bool Submit(wxCommand *command, bool storeIt = TRUE)
    ~%wxver22 virtual void Store(wxCommand *command)
    virtual bool Undo()
    virtual bool Redo()
    virtual bool CanUndo() const
    virtual bool CanRedo() const
    virtual void Initialize()
    virtual void SetMenuStrings()
    void SetEditMenu(wxMenu *menu)
    wxMenu *GetEditMenu() const
    wxList& GetCommands() const
    ~%wxver22 wxCommand *GetCurrentCommand() const
    int GetMaxCommands() const
    virtual void ClearCommands()
%endclass

///////////////////////////////////////////////////////////////////////////////
//  wxCommand

%class wxCommand, wxObject
    //wxCommand(bool canUndo = FALSE, const wxString& name = "")
    bool CanUndo()
    bool Do()
    wxString GetName()
    bool Undo()
%endclass

%endif wxLUA_USE_wxCommandProcessor

///////////////////////////////////////////////////////////////////////////////
// wxFileHistory

%if wxLUA_USE_wxFileHistory

%include "wx/docview.h"

%class wxFileHistory, wxObject
    wxFileHistory(int maxFiles = 9)
    void AddFileToHistory(const wxString& filename)
    void AddFilesToMenu()
    %rename AddFilesToGivenMenu void AddFilesToMenu(wxMenu* menu)
    wxString GetHistoryFile(int index) const
    int GetMaxFiles() const
    <wxver23 int GetNoHistoryFiles() const
    void Load(wxConfigBase& config)
    void RemoveMenu(wxMenu* menu)
    void Save(wxConfigBase& config)
    void UseMenu(wxMenu* menu)
%endclass

%endif wxLUA_USE_wxFileHistory

