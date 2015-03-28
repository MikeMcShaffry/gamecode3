/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML Resource system from contribs
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxXMLResource

<wxver25 %include "wx/xrc/xml.h"
~<wxver25 %include "wx/xml/xml.h"
%include "wx/xrc/xmlres.h"

%class %delete %noclassinfo %encapsulate wxXmlNode

    %enum wxXmlNodeType
        wxXML_ELEMENT_NODE
        wxXML_ATTRIBUTE_NODE
        wxXML_TEXT_NODE
        wxXML_CDATA_SECTION_NODE
        wxXML_ENTITY_REF_NODE
        wxXML_ENTITY_NODE
        wxXML_PI_NODE
        wxXML_COMMENT_NODE
        wxXML_DOCUMENT_NODE
        wxXML_DOCUMENT_TYPE_NODE
        wxXML_DOCUMENT_FRAG_NODE
        wxXML_NOTATION_NODE
        wxXML_HTML_DOCUMENT_NODE
    %end

    wxXmlNode(wxXmlNodeType type, const wxString& name, const wxString& content = wxEmptyString);
    %constructor wxXmlDefaultNode()
    %constructor wxXmlNodeEntry(wxXmlNode *parent, wxXmlNodeType type, const wxString& name, const wxString& content, wxXmlProperty *props, wxXmlNode *next);
    void AddChild(wxXmlNode *child);
    void InsertChild(wxXmlNode *child, wxXmlNode *before_node);
    bool RemoveChild(wxXmlNode *child);
    void AddProperty(const wxString& name, const wxString& value);
    bool DeleteProperty(const wxString& name);
    wxXmlNodeType GetType() const;
    wxString GetName() const;
    wxString GetContent() const;
    wxXmlNode *GetParent() const;
    wxXmlNode *GetNext() const;
    wxXmlNode *GetChildren() const;
    wxXmlProperty *GetProperties() const;
    %rename GetPropValPtr bool GetPropVal(const wxString& propName, wxString *value) const;
    wxString GetPropVal(const wxString& propName, const wxString& defaultVal) const;
    bool HasProp(const wxString& propName) const;
    void SetType(wxXmlNodeType type);
    %property=Type, read, write
    void SetName(const wxString& name);
    %property=Name, read, write
    void SetContent(const wxString& con);
    %property=Content, read, write
    void SetParent(wxXmlNode *parent);
    %property=Parent, read, write
    void SetNext(wxXmlNode *next);
    %property=Next, read, write
    void SetChildren(wxXmlNode *child);
    %property=Children, read, write
    void SetProperties(wxXmlProperty *prop);
    %property=Properties, read, write
    %rename AddPropertyEntry void AddProperty(wxXmlProperty *prop);
%endclass

%class %delete %noclassinfo %encapsulate wxXmlProperty
    wxXmlProperty(const wxString& name, const wxString& value, wxXmlProperty *next);
    %constructor wxXmlPropertyDefault();
    wxString GetName();
    wxString GetValue();
    wxXmlProperty *GetNext();
    void SetName(const wxString& name);
    void SetValue(const wxString& value);
    void SetNext(wxXmlProperty *next);
    %property=Name, read, write
    %property=Value, read, write
    %property=Next, read, write
%endclass

%class %delete %noclassinfo wxXmlDocument
    %constructor wxXmlDocumentDefault();
    wxXmlDocument(const wxString& filename, const wxString& encoding = "UTF-8");
    //wxXmlDocument(wxInputStream& stream, const wxString& encoding = "UTF-8");
    bool Load(const wxString& filename, const wxString& encoding = "UTF-8");
    //bool Load(wxInputStream& stream, const wxString& encoding = "UTF-8");
    bool Save(const wxString& filename) const;
    //bool Save(wxOutputStream& stream) const;
    bool IsOk() const;
    wxXmlNode *GetRoot() const;
    %property=Root, read, write
    wxString GetVersion() const;
    %property=Version, read, write
    wxString GetFileEncoding() const;
    %property=FileEncoding, read, write
    void SetRoot(wxXmlNode *node);
    void SetVersion(const wxString& version);
    void SetFileEncoding(const wxString& encoding);
    // These two are not for unicode
    //wxString GetEncoding() const;
    //void SetEncoding(const wxString& enc);
    //%property=Encoding, read, write
%endclass

%class %noclassinfo wxXmlResourceHandler, wxObject
%endclass

%class %delete %noclassinfo wxXmlResource, wxObject

    %enum wxXmlResourceFlags
        wxXRC_USE_LOCALE
        wxXRC_NO_SUBCLASSING
    %end

    wxXmlResource(const wxString& filemask, int flags = wxXRC_USE_LOCALE);
    %constructor wxXmlResourceGetDefault();
    %constructor wxXmlResourceDefault(int flags = wxXRC_USE_LOCALE);
    void AddHandler(wxXmlResourceHandler* handler);
    bool AttachUnknownControl(const wxString& name, wxWindow* control, wxWindow* parent = NULL);
    void ClearHandlers();
    int CompareVersion(int major, int minor, int release, int revision) const;
    wxXmlResource* Get();
    long GetVersion() const;
    %property=Version,read
    int GetXRCID(const wxString &stringID);
    void InitAllHandlers();
    bool Load(const wxString& filemask);
    wxBitmap LoadBitmap(const wxString& name);
    wxDialog* LoadDialog(wxWindow* parent, const wxString& name);
    %rename LoadDialogCreate bool LoadDialog(wxDialog* dlg, wxWindow *parent, const wxString &name);
    wxDialog* CreateDialog(wxWindow* parent, const wxString& name);
    bool LoadFrame(wxFrame* frame, wxWindow* parent, const wxString& name);
    wxFrame* CreateFrame(wxWindow* parent, const wxString& name);
    wxIcon LoadIcon(const wxString& name);
    wxMenu* LoadMenu(const wxString& name);
    %rename LoadMenuBarWindow wxMenuBar* LoadMenuBar(wxWindow* parent, const wxString& name);
    wxMenuBar* LoadMenuBar(const wxString& name);
    wxPanel* LoadPanel(wxWindow* parent, const wxString &name);
    %rename LoadPanelCreate bool LoadPanel(wxPanel *panel, wxWindow *parent, const wxString &name);
    wxToolBar* LoadToolBar(wxWindow *parent, const wxString& name);
    wxXmlResource* Set(wxXmlResource *res);
    int GetFlags();
    void SetFlags(int flags);
    %property=Flags, read
%endclass

%endif wxLUA_USE_wxXMLResource
