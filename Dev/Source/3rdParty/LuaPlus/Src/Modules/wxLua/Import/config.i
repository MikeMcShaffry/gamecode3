/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxConfig and wxConfigBase classes
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// TODO - add wxConfigFile and Reg

///////////////////////////////////////////////////////////////////////////////
// wxConfigBase

%if wxLUA_USE_wxConfig

%include "wx/confbase.h"
%include "wx/config.h"
%include "wx/fileconf.h"

%enum wxConfigBase::EntryType
    Type_Unknown
    Type_String
    Type_Boolean
    Type_Integer
    Type_Float
%end

%class %delete %noclassinfo wxConfigBase
    %constructor wxConfigGet(bool CreateOnDemand = TRUE)
    //static wxConfigBase * Get(bool CreateOnDemand = TRUE)
    %constructor wxConfigCreate()
    //static wxConfigBase * Create()
    static wxConfigBase * Set(wxConfigBase *pConfig = NULL)
    static void DontCreateOnDemand()
    void Destroy()
    bool DeleteAll()
    bool DeleteEntry(const wxString& key, bool bDeleteGroupIfEmpty = TRUE)
    bool DeleteGroup(const wxString& key)
    bool Exists(wxString& strName) const
    bool Flush(bool bCurrentOnly = FALSE)
    wxString GetAppName() const
    %win %property=AppName, read
    wxConfigBase::EntryType GetEntryType(const wxString& name) const
    bool GetFirstGroup(wxString& str, long& index) const
    bool GetFirstEntry(wxString& str, long& index) const
    bool GetNextGroup(wxString& str, long& index) const
    bool GetNextEntry(wxString& str, long& index) const
    unsigned int GetNumberOfEntries(bool bRecursive = FALSE) const
    unsigned int GetNumberOfGroups(bool bRecursive = FALSE) const
    const wxString& GetPath() const
    wxString GetVendorName() const
    %win %property=VendorName, read
    bool HasEntry(wxString& strName) const
    bool HasGroup(const wxString& strName) const
    bool IsExpandingEnvVars() const
    bool IsRecordingDefaults() const
    wxString Read(const wxString& key, const wxString& defaultVal = "") const
    %rename ReadInt long Read(const wxString& key, long defaultVal = 0) const
    %rename ReadFloat double Read(const wxString& key, double defaultVal = 0) const
    bool RenameEntry(const wxString& oldName, const wxString& newName)
    bool RenameGroup(const wxString& oldName, const wxString& newName)
    void SetExpandEnvVars (bool bDoIt = TRUE)
    void SetPath(const wxString& strPath)
    %property=Path, read, write
    void SetRecordDefaults(bool bDoIt = TRUE)
    bool Write(const wxString& key, wxString &value)
    %rename WriteInt bool Write(const wxString &key, long value)
    %rename WriteFloat bool Write(const wxString &key, double value)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxConfig

%class %noclassinfo wxConfig, wxConfigBase
    wxConfig(const wxString& appName = wxEmptyString, const wxString& vendorName = wxEmptyString, const wxString& localFilename = wxEmptyString, const wxString& globalFilename = wxEmptyString, long style = 0)
    %gtk void SetUmask(int mode)
    %mac void SetUmask(int mode)
%endclass

%endif wxLUA_USE_wxConfig
