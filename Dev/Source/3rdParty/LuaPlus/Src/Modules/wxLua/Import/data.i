/////////////////////////////////////////////////////////////////////////////
// Purpose:     data classes, wxObject, arrays, lists, hash
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// wxObject

%if wxLUA_USE_wxObject

%include "wx/object.h"

%builtin wxObject * wxCreateDynamicObject(const wxString& className)

%class %delete wxObject
    wxObject()
    wxObjectRefData* m_refData
    // Err: doesn't exist
    //void Dump(ostream& stream)
    wxClassInfo * GetClassInfo()
    wxObjectRefData* GetRefData() const
    bool  IsKindOf(wxClassInfo *info)
    void  Ref(const wxObject& clone)
    void  SetRefData(wxObjectRefData* data)
    void  UnRef()
    void *DynamicCast(const wxString &class)
%endclass

%class %noclassinfo wxObjectRefData
    int GetRefCount() const
%endclass

%endif wxLUA_USE_wxObject

///////////////////////////////////////////////////////////////////////////////
// wxClassInfo

%if wxLUA_USE_wxClassInfo

%include "wx/object.h"

%class %noclassinfo wxClassInfo, wxObject
    wxClassInfo(const wxString &name)
    wxObject* CreateObject()
    static wxClassInfo * FindClass(const wxString &name)
    wxString GetBaseClassName1() const
    %property=BaseClassName1, read
    wxString GetBaseClassName2() const
    %property=BaseClassName2, read
    wxString GetClassName() const
    %property=ClassName, read
    int GetSize() const
    %property=Size, read
    bool IsKindOf(wxClassInfo* info)
%endclass

%endif wxLUA_USE_wxClassInfo

///////////////////////////////////////////////////////////////////////////////
// wxValidator

%if wxLUA_USE_wxValidator

%include "wx/validate.h"

%class wxValidator, wxObject
	%define %object wxDefaultValidator

	wxWindow* GetWindow() const
    static void SetBellOnError(bool doIt = TRUE)
    void SetWindow(wxWindow* window)
    %property=Window, read, write
    virtual bool TransferFromWindow()
    virtual bool TransferToWindow()
    virtual bool Validate(wxWindow* parent)
%endclass

%endif wxLUA_USE_wxValidator

///////////////////////////////////////////////////////////////////////////////
// wxTextValidator

%if wxLUA_USE_wxTextValidator

%include "wx/valtext.h"

%define wxFILTER_NONE
%define wxFILTER_ASCII
%define wxFILTER_ALPHA
%define wxFILTER_ALPHANUMERIC
%define wxFILTER_NUMERIC
%define wxFILTER_INCLUDE_LIST
%define wxFILTER_EXCLUDE_LIST

%class %delete wxTextValidator, wxValidator
    wxTextValidator(long style = wxFILTER_NONE, wxString *valPtr = NULL)
//    wxStringList& GetExcludeList() const
//    wxStringList& GetIncludeList() const
    long GetStyle() const
    void SetStyle(long style)
    %property=Style, read, write
    void SetExcludeList(const wxStringList& stringList)
    void SetIncludeList(const wxStringList& stringList)
    %property=ExcludeList, read, write
    %property=IncludeList, read, write
%endclass

%endif wxLUA_USE_wxTextValidator

///////////////////////////////////////////////////////////////////////////////
// wxGenericValidator

%if wxLUA_USE_wxGenericValidator

%include "wx/valgen.h"

%class %delete %noclassinfo wxGenericValidator, wxValidator
    // for wxCheckBox and wxRadioButton
    %constructor wxGenericValidatorBool(bool *boolPtr)
    // for wxButton and wxComboBox, wxStaticText and wxTextCtrl
    %constructor wxGenericValidatorString(wxString *valPtr)
    // for wxGauge, wxScrollBar, wxRadioBox, wxSpinButton, wxChoice
    %constructor wxGenericValidatorInt(int *valPtr)
    // for wxListBox and wxCheckListBox
    %constructor wxGenericValidatorArray(wxArrayInt *valPtr)
%endclass

%endif wxLUA_USE_wxGenericValidator

///////////////////////////////////////////////////////////////////////////////
// wxList

%if wxLUA_USE_wxList

%include "wx/list.h"

%enum wxKeyType
    wxKEY_NONE
    wxKEY_INTEGER
    wxKEY_STRING
%end

%class %noclassinfo wxList, wxObject
    wxList()
    %constructor wxListFromTable()
    wxNode *Append(wxObject *object)
    %rename AppendNumKey wxNode *Append(long key, wxObject *object)
    %rename AppendStringKey wxNode *Append(const wxString& key, wxObject *object)
    void Clear()
    void DeleteContents(bool destroy)
    bool DeleteNode(wxNode *pNode)
    bool DeleteObject(wxObject *pObject)
    %rename FindNumKey wxNode *Find(long key)
    %rename FindStringKey wxNode *Find(const wxString &key)
    int     GetCount() const
    wxNode *GetFirst()
    wxNode *GetLast()
    wxNode *Insert(wxObject *pObject)
    %rename InsertAt wxNode *Insert(int position, wxObject *pObject)
    %rename InsertBefore wxNode *Insert(wxNode *pNode, wxObject *pObject)
    wxNode *Item(int iIndex) const
    wxNode *Member(wxObject *pObject)
    %property=Count, read
    %property=First, read
    %property=Last, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxNode

%class %noclassinfo wxNode
    wxObject *GetData()
    wxNode *GetNext()
    wxNode *GetPrevious()
    void SetData(wxObject *Data)
    //int IndexOf()
    %property=Data, read, write
    %property=Previous, read
    %property=Next, read
%endclass

%endif wxLUA_USE_wxList

///////////////////////////////////////////////////////////////////////////////
// wxArrayInt

%if wxLUA_USE_wxArrayInt

%include "wx/dynarray.h"

%class %delete %noclassinfo %encapsulate wxArrayInt
    void Clear()
    int  GetCount() const
    %property=Count, read
    void Empty()
    bool IsEmpty() const
    void Shrink()
    void RemoveAt(size_t index)
    int Item( int n )
    void Add( int num )
    void Insert( int num, int n, int copies = 1 )
%endclass

%endif wxLUA_USE_wxArrayInt

///////////////////////////////////////////////////////////////////////////////
// wxArrayString

%if wxLUA_USE_wxArrayString

%include "wx/string.h"

%class %delete %noclassinfo %encapsulate wxArrayString
    wxArrayString()
    %constructor wxArrayStringCopy(const wxArrayString& array)
    size_t Add(const wxString& str)
    void Alloc(size_t nCount)
    void Clear()
    int Count() const
    void Empty()
    int GetCount() const
    int Index(const wxString &sz, bool bCase = TRUE, bool bFromEnd = FALSE)
    void Insert(const wxString& str, int nIndex)
    bool IsEmpty()
    wxString& Item(size_t nIndex) const
    wxString& Last()
    void Remove(const wxString &sz)
    void RemoveAt(size_t nIndex)
    void Shrink()
    void Sort(bool reverseOrder = FALSE)
%endclass

%endif wxLUA_USE_wxArrayString

///////////////////////////////////////////////////////////////////////////////
// wxStringList

%if wxLUA_USE_wxStringList

%include "wx/list.h"

%class %noclassinfo wxStringList, wxList
    wxStringList()
    wxNode *Add(const wxString& s)
    void Clear()
    void Delete(const wxString& s)
    bool Member(const wxString& s)
    void Sort()
%endclass

%endif wxLUA_USE_wxStringList

///////////////////////////////////////////////////////////////////////////////
// wxHashTable

%if wxLUA_USE_wxHashTable

%include "wx/hash.h"

%class %noclassinfo wxHashTable, wxObject
    wxHashTable(unsigned int key_type, int size = 1000)
    void BeginFind()
    void Clear()
    wxObject * Delete(long key)
    void DeleteContents(bool flag)
    %rename GetNumKey wxObject * Get(long key)
    %rename GetStringKey wxObject * Get(const wxString &key)
    long MakeKey(const wxString& string)
    wxNode * Next()
    %rename PutNumKey void Put(long key, wxObject *object)
    %rename PutStringKey void Put(const wxString & key, wxObject *object)
    int GetCount() const
    %property=Count, read
%endclass

%endif wxLUA_USE_wxHashTable

///////////////////////////////////////////////////////////////////////////////
//  wxStreamBase

%include <wx/stream.h>

%enum wxSeekMode   // also used in file.i included here for streams
    wxFromStart
    wxFromCurrent
    wxFromEnd
    wxInvalidOffset
%end

%enum wxStreamError
    wxSTREAM_NO_ERROR
    wxSTREAM_EOF
    wxSTREAM_WRITE_ERROR
    wxSTREAM_READ_ERROR 
%end

%class %noclassinfo wxStreamBase
    wxStreamBase()
    bool IsOk() const
    void Reset()
    wxStreamError GetLastError() const
    size_t GetSize() const
%endclass    

///////////////////////////////////////////////////////////////////////////////
//  wxInputStream

%include <wx/stream.h>

%class %noclassinfo wxInputStream, wxStreamBase
    char GetC()
    bool Eof()
    size_t LastRead() const
    char Peek()
    wxInputStream& Read(void *buffer, size_t size)
    %rename ReadStream wxInputStream& Read(wxOutputStream& stream_in)
    off_t SeekI(off_t pos, wxSeekMode mode)
    off_t TellI() const
    %rename Unget size_t Ungetch(const char* buffer, size_t size)
    bool Ungetch(char c)
%endclass    

///////////////////////////////////////////////////////////////////////////////
//  wxOutputStream

%include <wx/stream.h>

%class %noclassinfo wxOutputStream, wxStreamBase
    wxOutputStream()
    size_t LastWrite() const
    void PutC(char c)
    off_t SeekO(off_t pos, wxSeekMode mode)
    off_t TellO() const
    wxOutputStream& Write(const void *buffer, size_t size)
    %rename WriteStream wxOutputStream& Write(wxInputStream& stream_in)
%endclass    

///////////////////////////////////////////////////////////////////////////////
//  wxProtocol

%include <wx/protocol/protocol.h>

%enum wxProtocolError
    wxPROTO_NOERR
    wxPROTO_NETERR
    wxPROTO_PROTERR
    wxPROTO_CONNERR
    wxPROTO_INVVAL
    wxPROTO_NOHNDLR
    wxPROTO_NOFILE
    wxPROTO_ABRT
    wxPROTO_RCNCT
%end
//  wxPROTO_STREAM 

%class %noclassinfo wxProtocol, wxSocketClient
    bool Reconnect()
    wxInputStream *GetInputStream(const wxString& path)
    bool Abort()
    wxProtocolError GetError()
    wxString GetContentType()
    void SetUser(const wxString& user)
    void SetPassword(const wxString& user)
%endclass    

///////////////////////////////////////////////////////////////////////////////
//  wxHTTP

%include <wx/protocol/http.h>
%class %noclassinfo %delete wxHTTP, wxProtocol
    wxHTTP()
    wxInputStream *GetInputStream(const wxString& path)
    void SetHeader(const wxString& header, const wxString& h_data)
    wxString GetHeader(const wxString& header)
%endclass    

///////////////////////////////////////////////////////////////////////////////
//  wxFTP

%include <wx/protocol/ftp.h>

%enum wxFTP::TranferMode
    NONE
    ASCII
    BINARY
%end

%class %noclassinfo %delete wxFTP, wxProtocol
    wxFTP()
    bool Abort()
    bool CheckCommand(const wxString& command, char ret)
    char SendCommand(const wxString& command)
    const wxString& GetLastResult()
    bool ChDir(const wxString& dir)
    bool MkDir(const wxString& dir)
    bool RmDir(const wxString& dir)
    wxString Pwd()  
    bool Rename(const wxString& src, const wxString& dst)
    bool RmFile(const wxString& path)
    bool SetAscii()
    bool SetBinary()
    bool SetTransferMode(wxFTP::TransferMode mode)
    void SetUser(const wxString& user)
    void SetPassword(const wxString& passwd)
    bool FileExists(const wxString& filename)
    int GetFileSize(const wxString& filename)
    bool GetDirList(wxArrayString& files, const wxString& wildcard = "")
    bool GetFilesList(wxArrayString& files, const wxString& wildcard = "")
    wxOutputStream * GetOutputStream(const wxString& file)
    wxInputStream * GetInputStream(const wxString& path)
%endclass
