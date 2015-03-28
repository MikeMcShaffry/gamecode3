///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDrive.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDrive.h $
// $Date: 2003/01/05 $ $Revision: #8 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#ifndef VIRTUALDRIVE_H
#define VIRTUALDRIVE_H

#include "Misc.h"
#include "zlib.h"
#include <time.h>
#include "Array.h"
#include "AnsiString.h"
#include "Map.h"
#include <stdlib.h>
#include <ctype.h>
#include "aes/fileenc.h"
#include "aes/prng.h"
#include "File.h"

namespace Misc {

class VirtualDrive;

class VirtualFileHandle
{
public:
    VirtualFileHandle();
    ~VirtualFileHandle();

    VirtualDrive* GetParentDrive() const             {  return parentDrive;  }
	bool IsValid() const                             {  return fileEntryIndex != size_t(-1) /*VirtualDrive::INVALID_FILE_ENTRY*/;  } 

private:
    VirtualFileHandle* nextOpenFile;
    VirtualFileHandle* prevOpenFile;
    VirtualDrive* parentDrive;
	size_t fileEntryIndex;
    size_t headerSize;
    z_stream stream;
    BYTE* bufferedData;
	DWORD posInBufferedData;
	ULONGLONG curCompressedFilePosition;
	ULONGLONG curUncompressedFilePosition;
	fcrypt_ctx  zcx[1];

    friend class VirtualDrive;
};


/**
	A virtual drive is housed within a physical disk file.  The disk file
	contains one or more files, known as virtual files, and a directory
	describing all the files.  Similar in concept to WAD files.

	@see VirtualFile
**/
class VirtualDrive
{
public:
	enum {  INVALID_FILE_ENTRY = (size_t)-1  };
    enum {  UNCOMPRESSED = 0, COMPRESSED = 8  };

	/**
		Represents a file entry within the virtual drive's directory.
	**/
	class FileEntry
	{
	public:
		time_t GetTimeStamp() const			        {  return m_fileTime;  }
        DWORD GetOffset() const                     {  return m_offset;  }
        DWORD GetCompressedSize() const             {  return m_compressedSize;  }
        DWORD GetUncompressedSize() const           {  return m_uncompressedSize;  }
		DWORD GetCRC() const						{  return m_crc;  }
        DWORD GetCompressionMethod() const          {  return m_compressionMethod;  }
		const char* GetFileName() const		        {  return m_fileName;  }

		void SetTimeStamp(time_t fileTime)	        {  m_fileTime = fileTime;  m_parentDrive->m_changed = true;  }
		void SetCRC(DWORD crc)						{  m_crc = crc;  m_parentDrive->m_changed = true;  }

	protected:
		time_t          m_fileTime;
		DWORD			m_offset;
		DWORD			m_compressedSize;
        DWORD           m_uncompressedSize;
		DWORD			m_crc;
        DWORD           m_compressionMethod;

		VirtualDrive*	m_parentDrive;
        char            m_fileName[1];

		friend class VirtualDrive;
	};

	///////////////////////////////////////////////////////////////////////////
	VirtualDrive();
	~VirtualDrive();

	bool Create(const char* filename, const char* defaultPassword = NULL);
	bool Create(File& parentFile, const char* fileName, const char* defaultPassword = NULL);
	bool Open(const char* filename, bool readOnly = true, const char* defaultPassword = NULL);
	bool Open(File& parentFile, const char* fileName, bool readOnly = true, const char* defaultPassword = NULL);
	bool Close(void);
	bool Flush();

	bool IsReadOnly() const					{  return m_readOnly;  }
	bool IsOpened() const					{  return m_parentFile != NULL;  }

	bool FileCreate(const char* filename, VirtualFileHandle& fileHandle, UINT compressionMethod = COMPRESSED, const time_t* fileTime = NULL);
	bool FileOpen(const char* filename, VirtualFileHandle& fileHandle);
	bool FileOpenIndex(size_t index, VirtualFileHandle& fileHandle);
	bool FileClose(VirtualFileHandle& fileHandle);
	void FileCloseAll();

    const char* FileGetFileName(VirtualFileHandle& fileHandle);
	ULONGLONG FileGetPosition(VirtualFileHandle& fileHandle);
	void FileSetLength(VirtualFileHandle& fileHandle, ULONGLONG newLength);
	ULONGLONG FileGetLength(VirtualFileHandle& fileHandle);

	LONGLONG FileSeek(VirtualFileHandle& fileHandle, LONGLONG offset, File::SeekFlags seekFlags = File::SEEKFLAG_BEGIN);
	ULONGLONG FileRead(VirtualFileHandle& fileHandle, void* buffer, ULONGLONG count);
    ULONGLONG FileWrite(VirtualFileHandle& fileHandle, const void* buffer, ULONGLONG count);

	bool FileErase(const char* filename);
	bool FileRename(const char* oldName, const char* newName);

	bool FileCopy(VirtualFileHandle& srcFileHandle);
	bool FileCopy(File& srcFile, const char* destFilename, UINT compressionMethod = COMPRESSED, const time_t* fileTime = NULL);
	bool FileCopy(const char* srcFileName, const char* destFilename, UINT compressionMethod = COMPRESSED, const time_t* fileTime = NULL);

    struct FileOrderInfo
    {
		FileOrderInfo()
			: compressionMethod(COMPRESSED)
		{
		}

		FileOrderInfo(const AnsiString& _entryName, const AnsiString& _srcPath, UINT _compressionMethod = VirtualDrive::COMPRESSED)
			: entryName(_entryName)
			, srcPath(_srcPath)
			, compressionMethod(_compressionMethod)
		{
		}

        AnsiString entryName;
        AnsiString srcPath;
		UINT compressionMethod;

	protected:
		friend class VirtualDrive;

		time_t writeTime;
		bool needUpdate;
		DWORD crc;
		UINT size;

		bool operator<(const FileOrderInfo& info) const
		{
			return this->entryName < info.entryName;
		}
	};
    typedef Array<FileOrderInfo> FileOrderInfoArray;
    bool Pack(FileOrderInfoArray* fileOrder = NULL);
    bool ProcessFileList(const char* fileListFileName);
    bool ProcessFileList(FileOrderInfoArray& fileOrder);

	File* GetParentFile() const;
	const char* GetFileName() const;

	size_t GetFileEntryCount(void) const;
	VirtualDrive::FileEntry* GetFileEntry(size_t entry);
	VirtualDrive::FileEntry* FindFileEntry(const char* filename);

	size_t FindFileEntryIndex(const char* filename);
	int	GetNumberOfFilesOpen(void) const;

	static time_t AdjustTime_t(time_t timeToAdjust);

	AnsiString errorString;

private:
	void FileCloseInternal(VirtualFileHandle& fileHandle);

	size_t  m_fileEntryCount;
	size_t  m_fileEntryMaxCount;
    ULONGLONG m_dirOffset;
	ULONGLONG m_dirSize;			// Directory size in bytes.
	UINT    m_needPack:1;
	UINT	m_readOnly:1;
	UINT	m_unused:30;

    char* m_fileName;
    size_t* m_fileEntryOffsets;
    BYTE* m_fileEntries;
	size_t m_fileEntriesSizeBytes;
	size_t m_fileEntriesMaxSizeBytes;

	File* m_parentFile;
	bool m_ownParentFile;

    bool m_changed;								//!< Whether the file needs to be flushed.
	VirtualFileHandle* m_curWriteFile;				//!< The current file being written to.

	VirtualFileHandle* m_headOpenFile;

	class SimpleString
	{
	public:
		SimpleString() : m_drive(NULL), m_index((size_t)-1), m_str(NULL) {}
		SimpleString(VirtualDrive* drive, size_t index) : m_drive(drive), m_index(index), m_str(NULL) {}
		SimpleString(const char* str) : m_drive(NULL), m_index((size_t)-1), m_str(str) {}

		VirtualDrive* m_drive;
		size_t m_index;
		const char* m_str;
	};

	class SimpleStringTypeTraits : public DefaultTypeTraits<SimpleString>
	{
	public:
		static size_t Hash(const SimpleString& str)
		{
			const char* ptr = str.m_str ? str.m_str : str.m_drive->GetFileEntry(str.m_index)->GetFileName();
			size_t l = strlen(ptr);
			size_t h = l;
			size_t step = (l >> 5) + 1;
			for (size_t l1 = l; l1 >= step; l1 -= step)
				h = h ^ ((h<<5)+(h>>2)+(unsigned char)(tolower(ptr[l1-1])));

			return h;
		}

		static bool CompareElements(const SimpleString& str1, const SimpleString& str2)
		{
			const char* ptr1 = str1.m_str ? str1.m_str : str1.m_drive->GetFileEntry(str1.m_index)->GetFileName();
			const char* ptr2 = str2.m_str ? str2.m_str : str2.m_drive->GetFileEntry(str2.m_index)->GetFileName();
#if defined(PLATFORM_WINDOWS)
			return _stricmp(ptr1, ptr2) == 0;
#else
			return strcasecmp(ptr1, ptr2) == 0;
#endif
		}
	};

	typedef Map<SimpleString, size_t, SimpleStringTypeTraits> FileNameMap;
	FileNameMap fileNameMap;

	AnsiString defaultPassword;
	fcrypt_ctx defaultzcx[1];

	friend class VirtualDriveManager;
	friend class VirtualFile;
	friend class FileEntry;
};


/**
	@return Returns the pointer to the File-derived object used for all virtual
		drive file operations.
**/
inline File* VirtualDrive::GetParentFile() const
{
	return m_parentFile;
}


/**
	@return Retrieves the filename of the virtual drive.
**/
inline const char* VirtualDrive::GetFileName() const
{
	return m_fileName;
}


/**
	@return Returns the number of file entries in the virtual drive.
**/
inline size_t VirtualDrive::GetFileEntryCount(void) const
{
	return m_fileEntryCount;
}

} // namespace Misc

#endif // VIRTUALDRIVE_H
