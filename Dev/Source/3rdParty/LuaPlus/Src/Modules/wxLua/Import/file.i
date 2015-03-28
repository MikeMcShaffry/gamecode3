/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxFile, wxDir, wxFileName and file functions
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%include "wx/filefn.h"
%include "sys/stat.h"

// global functions from the wxWindow's docs

%builtin bool wxDirExists(const wxString& dirname)
%builtin bool wxFileExists(const wxString& filename)
%builtin wxString wxDos2UnixFilename(const wxString& s)
%builtin wxString wxUnix2DosFilename(const wxString& s)
//%builtin time_t wxFileModificationTime(const wxString& filename)
//%builtin wxString wxFileNameFromPath(const wxString& path) // obsolete use wxFileName::SplitPath
%builtin wxString wxFindFirstFile(const wxString& spec, int flags = 0)
%builtin wxString wxFindNextFile()
// bool wxGetDiskSpace(const wxString& path, wxLongLong *total = NULL, wxLongLong *free = NULL)
%builtin wxString wxGetOSDirectory()
%builtin bool wxIsAbsolutePath(const wxString& filename)
%builtin wxString wxPathOnly(const wxString& path)
%builtin bool wxConcatFiles(const wxString& file1, const wxString& file2,const wxString& file3)
%wxver22 %builtin bool wxCopyFile(const wxString& file1, const wxString& file2)
~%wxver22 %builtin bool wxCopyFile(const wxString& file1, const wxString& file2, bool overwrite = TRUE)
%builtin wxString wxGetCwd()
//%builtin char * wxGetTempFileName(const wxString& prefix) // obsolete use wxFileName::CreateTempFileName
%builtin bool wxIsWild(const wxString& pattern)
%builtin bool wxMatchWild(const wxString& pattern, const wxString& text, bool dot_special)
%builtin bool wxMkdir(const wxString& dir, int perm = 0777)
%builtin bool wxRemoveFile(const wxString& file)
%builtin bool wxRenameFile(const wxString& file1, const wxString& file2)
%builtin bool wxRmdir(const wxString& dir, int flags=0)
%builtin bool wxSetWorkingDirectory(const wxString& dir)

%override builtin_wxFileSeparator
// wxString wxFileSeparator()
static int LUACALL builtin_wxFileSeparator(lua_State *L)
{    
    lua_pushstring(L, wx2lua(wxString(wxFILE_SEP_PATH)));
    return 1;
}
%end

%builtin wxString wxFileSeparator()

%override builtin_wxFileSize
// long wxFileSize()
static int LUACALL builtin_wxFileSize(lua_State *L)
{    
    wxString fileName = lua2wx(lua_tostring(L, 1));
    wxStructStat statstr;
    wxStat(fileName, &statstr);
    lua_pushnumber(L, statstr.st_size);
    return 1;
}
%end

%builtin long wxFileSize(const wxString& fileName)

///////////////////////////////////////////////////////////////////////////////
// wxFileName

%if wxLUA_USE_wxFileName

%include "wx/filename.h"

%define wxPATH_GET_VOLUME
%define wxPATH_GET_SEPARATOR
%define wxPATH_MKDIR_FULL

//%define %string wxFILE_SEP_PATH
//%define %string wxPATH_SEP
%define wxFILE
%define wxDIR

%enum wxPathFormat
    wxPATH_NATIVE
    wxPATH_UNIX
    wxPATH_MAC
    wxPATH_DOS
    wxPATH_VMS
    wxPATH_BEOS
    wxPATH_WIN
    wxPATH_OS2
%end

%enum wxPathNormalize
    wxPATH_NORM_ENV_VARS
    wxPATH_NORM_DOTS
    wxPATH_NORM_TILDE
    wxPATH_NORM_CASE
    wxPATH_NORM_ABSOLUTE
    wxPATH_NORM_LONG
    wxPATH_NORM_ALL
%end

%class %delete %noclassinfo %encapsulate wxFileName
    %constructor wxFileNameDefault()
    %constructor wxFileNameCopy(const wxFileName& filename)
    wxFileName(const wxString& fullpath, wxPathFormat format = wxPATH_NATIVE)
    %constructor wxFileNamePath(const wxString& path, const wxString& name, wxPathFormat format = wxPATH_NATIVE)
    %constructor wxFileNameVolume(const wxString& volume, const wxString& path, const wxString& name, const wxString& ext, wxPathFormat format = wxPATH_NATIVE)
    void AppendDir(const wxString& dir)
    void Assign(const wxFileName& filepath)
    %rename AssignFullName void Assign(const wxString& fullpath, wxPathFormat format = wxPATH_NATIVE)
    %rename AssignVolume void Assign(const wxString& volume, const wxString& path, const wxString& name, const wxString& ext, wxPathFormat format = wxPATH_NATIVE)
    %rename AssignPath void Assign(const wxString& path, const wxString& name, wxPathFormat format = wxPATH_NATIVE)
    %rename AssignPathNameExt void Assign(const wxString& path, const wxString& name, const wxString& ext, wxPathFormat format = wxPATH_NATIVE)
    void AssignCwd(const wxString& volume = "")
    void AssignDir(const wxString& dir, wxPathFormat format = wxPATH_NATIVE)
    void AssignHomeDir()
    void AssignTempFileName(const wxString& prefix, wxFile *fileTemp = NULL)
    void Clear()
    static wxString CreateTempFileName(const wxString& prefix, wxFile *fileTemp = NULL)
    bool DirExists()
    %rename DirExistsName static bool DirExists(const wxString& dir)
    static wxFileName DirName(const wxString& dir)
    bool FileExists()
    %rename FileExistsFile static bool FileExists(const wxString& file)
    static wxFileName FileName(const wxString& file)
    static wxString GetCwd(const wxString& volume = "")
    int GetDirCount() const
    const wxArrayString& GetDirs() const
    wxString GetExt() const
    static wxPathFormat GetFormat(wxPathFormat format = wxPATH_NATIVE)
    wxString GetFullName() const
    wxString GetFullPath(wxPathFormat format = wxPATH_NATIVE) const
    static wxString GetHomeDir()
    wxString GetLongPath() const
    wxDateTime GetModificationTime() const
    wxString GetName() const
    wxString GetPath(int flags = 0, wxPathFormat format = wxPATH_NATIVE) const
    static int GetPathSeparator(wxPathFormat format = wxPATH_NATIVE)
    static wxString GetPathSeparators(wxPathFormat format = wxPATH_NATIVE)
    wxString GetShortPath() const
    bool GetTimes(wxDateTime* dtAccess, wxDateTime* dtMod, wxDateTime* dtCreate) const
    wxString GetVolume() const
    static wxString GetVolumeSeparator(wxPathFormat format = wxPATH_NATIVE)
    bool HasExt() const
    bool HasName() const
    bool HasVolume() const
    void InsertDir(int before, const wxString& dir)
    bool IsAbsolute(wxPathFormat format = wxPATH_NATIVE)
    static bool IsCaseSensitive(wxPathFormat format = wxPATH_NATIVE)
    bool IsOk() const
    static bool IsPathSeparator(int ch, wxPathFormat format = wxPATH_NATIVE)
    bool IsRelative(wxPathFormat format = wxPATH_NATIVE)
    bool IsDir() const
    bool MakeRelativeTo(const wxString& pathBase = "", wxPathFormat format = wxPATH_NATIVE)
    bool Mkdir(int perm = 4095, int flags = 0)
    %rename MkdirName static bool Mkdir(const wxString& dir, int perm = 4095, int flags = 0)
    bool Normalize(int flags = wxPATH_NORM_ALL, const wxString& cwd = wxEmptyString, wxPathFormat format = wxPATH_NATIVE)
    void PrependDir(const wxString& dir)
    void RemoveDir(int pos)
    bool Rmdir()
    %rename RmdirName static bool Rmdir(const wxString& dir)
    bool SameAs(const wxFileName& filepath, wxPathFormat format = wxPATH_NATIVE) const
    bool SetCwd()
    %rename SetCwdName static bool SetCwd(const wxString& cwd)
    void SetExt(const wxString& ext)
    void SetFullName(const wxString& fullname)
    void SetName(const wxString& name)
    bool SetTimes(const wxDateTime* dtAccess, const wxDateTime* dtMod, const wxDateTime* dtCreate)
    void SetVolume(const wxString& volume)
    %rename SplitVolumePath static void SplitPath(const wxString& fullpath, wxString* volume, wxString* path, wxString* name, wxString* ext, wxPathFormat format = wxPATH_NATIVE)
    static void SplitPath(const wxString& fullpath, wxString* path, wxString* name, wxString* ext, wxPathFormat format = wxPATH_NATIVE)
    bool Touch()
%endclass

%endif wxLUA_USE_wxFileName

///////////////////////////////////////////////////////////////////////////////
// wxFile

%if wxLUA_USE_wxFile

%include "wx/file.h"

%enum wxFile::OpenMode
    wxFileRead
    wxFileWrite
    wxFileReadWrite
    wxFileWriteAppend
%end

%define wxS_IRUSR
%define wxS_IWUSR
%define wxS_IXUSR
%define wxS_IRGRP
%define wxS_IWGRP
%define wxS_IXGRP
%define wxS_IROTH
%define wxS_IWOTH
%define wxS_IXOTH

%class %delete %noclassinfo %encapsulate wxFile
    wxFile()
    %constructor wxFileOpen(const wxString& filename, wxFile::OpenMode mode = wxFile::read)
    static bool Access(const wxString& name, wxFile::OpenMode mode)
    static bool Exists(const wxString& name)
    bool Flush()
    bool IsOpened() const
    unsigned int Length() const
    bool Open(const wxString& filename, wxFile::OpenMode mode = wxFile::read)
    unsigned int Read(void * buffer,  unsigned int count)
    unsigned int Seek(unsigned int offset, wxSeekMode mode = wxFromStart)
    unsigned int SeekEnd(unsigned int offset = 0)
    unsigned int Tell() const
    unsigned int Write(const void * buffer, unsigned int count)
    %rename WriteString bool Write(const wxString &str)
%endclass

%endif wxLUA_USE_wxFile

///////////////////////////////////////////////////////////////////////////////
// wxDir

%if wxLUA_USE_wxDir

%include "wx/dir.h"

%define wxDIR_FILES
%define wxDIR_DIRS
%define wxDIR_HIDDEN
%define wxDIR_DOTDOT
%define wxDIR_DEFAULT

%class %delete %noclassinfo %encapsulate wxDir
    wxDir()
    %constructor wxDirOpen(const wxString& dir)
    static bool Exists(const wxString& dir)
    bool Open(const wxString& dir)
    bool IsOpened() const
    bool GetFirst(wxString * filename, const wxString& filespec = "", int flags = wxDIR_DEFAULT) const
    bool GetNext(wxString * filename) const
    ~%wxver22 static unsigned int GetAllFiles(const wxString& dirname, wxArrayString *files, const wxString& filespec = "", int flags = wxDIR_DEFAULT)
%endclass

%endif wxLUA_USE_wxDir
