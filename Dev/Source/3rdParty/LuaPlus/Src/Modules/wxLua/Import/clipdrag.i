/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxClipboard and drag & drop and their wxDataFormat
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// TODO - Missing wxClipboardLocker

///////////////////////////////////////////////////////////////////////////////
// wxClipboard

%if wxLUA_USE_wxClipboard

%include "wx/clipbrd.h"

%class wxClipboard, wxObject
    <wxver25 %define %pointer wxTheClipboard

    void UsePrimarySelection( bool primary = FALSE )
    bool Flush()
    void Clear()
    bool GetData( wxDataObject& data )
    bool IsSupported( const wxDataFormat& format )
    bool SetData( wxDataObject *data )
    bool AddData( wxDataObject *data )
    bool IsOpened() const
    void Close()
    bool Open()
    <wxver24 %skip static wxClipboard *Get()
%endclass

%endif wxLUA_USE_wxClipboard

///////////////////////////////////////////////////////////////////////////////
// wxDataFormat

%if wxLUA_USE_wxDataObject

%include "wx/dataobj.h"

%enum wxDataFormatId
    wxDF_INVALID
    wxDF_TEXT
    wxDF_BITMAP
    wxDF_METAFILE
    wxDF_SYLK
    wxDF_DIF
    wxDF_TIFF
    wxDF_OEMTEXT
    wxDF_DIB
    wxDF_PALETTE
    wxDF_PENDATA
    wxDF_RIFF
    wxDF_WAVE
    wxDF_UNICODETEXT
    wxDF_ENHMETAFILE
    wxDF_FILENAME
    wxDF_LOCALE
    wxDF_PRIVATE
~%wxver22   wxDF_HTML
~%wxver22   wxDF_MAX
%end

%class %delete %noclassinfo %encapsulate wxDataFormat
    %define %object wxFormatInvalid

    wxDataFormat(wxDataFormatId format = wxDF_INVALID)
    %constructor wxDataFormatUser(const wxString &format)
    wxString GetId() const
    wxDataFormatId GetType() const
    void SetId(const wxString &format)
    void SetType(wxDataFormatId format)
    %property=Id, read, write
    %property=Type, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDataObject

%enum wxDataObject::Direction
    Get
    Set
%end

%class %noclassinfo wxDataObject
    virtual void GetAllFormats( wxDataFormat *formats, wxDataObject::Direction dir = wxDataObject::Get) const
    virtual bool GetDataHere(const wxDataFormat& format, void *buf ) const
    virtual int GetDataSize(const wxDataFormat& format ) const
    virtual int GetFormatCount(wxDataObject::Direction dir = wxDataObject::Get) const
    virtual wxDataFormat GetPreferredFormat(wxDataObject::Direction dir = wxDataObject::Get) const
    virtual bool SetData( const wxDataFormat& format, int len, const void *buf )
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDataObjectSimple

%class %delete %noclassinfo %encapsulate wxDataObjectSimple, wxDataObject
    wxDataObjectSimple(const wxDataFormat& format = wxFormatInvalid)
    const wxDataFormat& GetFormat() const
    void SetFormat(const wxDataFormat& format)
    %property=Format, read, write
    virtual size_t GetDataSize() const
    %property=DataSize, read
    virtual bool GetDataHere(void *buf) const
    virtual bool SetData(size_t len, const void *buf)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDataObjectComposite

%class %delete %noclassinfo %encapsulate wxDataObjectComposite, wxDataObject
    wxDataObjectComposite()
    void Add( wxDataObjectSimple *dataObject, bool preferred = FALSE)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFileDataObject

%class %delete %noclassinfo %encapsulate wxFileDataObject, wxDataObjectSimple
    wxFileDataObject()
    const wxArrayString& GetFilenames() const
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTextDataObject

%class %noclassinfo %encapsulate wxTextDataObject, wxDataObjectSimple
    wxTextDataObject(const wxString& text = "")
    virtual size_t GetTextLength() const
    virtual wxString GetText() const
    virtual void SetText(const wxString& text)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxBitmapDataObject

%class %noclassinfo %encapsulate wxBitmapDataObject, wxDataObjectSimple
    wxBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap)
    virtual wxBitmap GetBitmap() const
    virtual void SetBitmap(const wxBitmap& bitmap)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCustomDataObject

%class %noclassinfo %encapsulate wxCustomDataObject, wxDataObjectSimple
    wxCustomDataObject(const wxDataFormat& format = wxFormatInvalid)
    void TakeData(size_t size, void *data);
    virtual void *Alloc(size_t size);
    virtual void Free();
    virtual size_t GetSize() const { return m_size; }
    virtual void *GetData() const { return m_data; }
%endclass

%endif wxLUA_USE_wxDataObject

///////////////////////////////////////////////////////////////////////////////
// wxDropTarget

%if wxLUA_USE_wxDragDrop

%include "wx/dnd.h"

~%wxver22 %enum
~%wxver22     wxDrag_CopyOnly
~%wxver22     wxDrag_AllowMove
~%wxver22     wxDrag_DefaultMove
~%wxver22 %end

%enum wxDragResult
    wxDragError
    wxDragNone
    wxDragCopy
    wxDragMove
    wxDragCancel
%end

%builtin bool wxIsDragResultOk(wxDragResult res)

%class %noclassinfo wxDropTarget
    virtual void GetData()
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def)
    virtual bool OnDrop(wxCoord x, wxCoord y)
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def)
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
    virtual void OnLeave()
    void SetDataObject(wxDataObject* data)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFileDropTarget

//%class %noclassinfo wxFileDropTarget, wxDropTarget
//    wxFileDropTarget()
//    virtual bool OnDropFiles(wxCoord x, wxCoord y,const wxArrayString& filenames)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxTextDropTarget

//%class %noclassinfo wxTextDropTarget, wxDropTarget
//    wxTextDropTarget()
//    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text)
//%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDropSource

%class %delete %noclassinfo %encapsulate wxDropSource
    %win wxDropSource(wxWindow* win = NULL, const wxCursor& iconCopy = wxNullCursor, const wxCursor& iconMove = wxNullCursor, const wxCursor& iconStop = wxNullCursor)
    %gtk wxDropSource(wxWindow* win = NULL, const wxIcon& iconCopy = wxNullIcon, const wxIcon& iconMove = wxNullIcon, const wxIcon& iconStop = wxNullIcon)
    %mac wxDropSource(wxWindow* win = NULL, const wxIcon& iconCopy = wxNullIcon, const wxIcon& iconMove = wxNullIcon, const wxIcon& iconStop = wxNullIcon)
    %win %constructor wxDropSourceData(wxDataObject& data, wxWindow* win = NULL, const wxCursor& iconCopy = wxNullCursor, const wxCursor& iconMove = wxNullCursor, const wxCursor& iconStop = wxNullCursor)
    %gtk %constructor wxDropSourceData(wxDataObject& data, wxWindow* win = NULL, const wxIcon& iconCopy = wxNullIcon, const wxIcon& iconMove = wxNullIcon, const wxIcon& iconStop = wxNullIcon)
    %mac %constructor wxDropSourceData(wxDataObject& data, wxWindow* win = NULL, const wxIcon& iconCopy = wxNullIcon, const wxIcon& iconMove = wxNullIcon, const wxIcon& iconStop = wxNullIcon)
    void SetData(wxDataObject& data)
    virtual wxDragResult DoDragDrop(bool allowMove = FALSE)
    virtual bool GiveFeedback(wxDragResult effect)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxDropFilesEvent

%include "wx/event.h"

%class %delete wxDropFilesEvent, wxEvent
    %define %event wxEVT_DROP_FILES

    wxString* GetFiles() const
    int GetNumberOfFiles() const
    wxPoint GetPosition() const
%endclass

%endif wxLUA_USE_wxDragDrop

///////////////////////////////////////////////////////////////////////////////
// wxMetafile

%if wxLUA_USE_wxMetafile

%include "wx/metafile.h"

//%builtin bool wxMakeMetafilePlaceable(const wxString& filename, int minX, int minY, int maxX, int maxY, float scale = 1.0)

%gtk %skip
%class %noclassinfo wxMetafile, wxObject
    wxMetafile(const wxString& filename = "")
    bool Ok()
    bool Play(wxDC *dc)
    bool SetClipboard(int width = 0, int height = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxMetafileDC

%gtk %skip
%class %delete %noclassinfo wxMetafileDC, wxDC
    wxMetafileDC(const wxString& filename = "")
    %win wxMetafile * Close()
%endclass

%endif wxLUA_USE_wxMetafile
