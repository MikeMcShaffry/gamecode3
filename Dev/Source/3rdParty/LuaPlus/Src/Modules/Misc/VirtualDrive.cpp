///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDrive.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDrive.cpp $
// $Date: 2003/01/05 $ $Revision: #10 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "Misc_InternalPch.h"
#include "VirtualDrive.h"
#include "DiskFile.h"
#include "VirtualFile.h"
#include <time.h>
#include "zlib.h"
#include <assert.h>
#include <stdio.h>
#if defined(PLATFORM_WINDOWS)
#include <io.h>
#include <windows.h>
#elif defined(PLATFORM_MAC)
#include <unistd.h>
#endif
#include "List.h"
#include "Map.h"
#include "aes/fileenc.h"
#include "aes/prng.h"

namespace Misc {

#define Z_BUFSIZE 16384

#if defined(_MSC_VER)
#include <pshpack1.h>
#endif

struct ZipDirHeader
{
	enum
	{
		SIGNATURE = 0x06054b50,
	};
	UINT   signature;
	WORD   number_disk;
	WORD   number_disk_with_CD;
	WORD   number_entry_this_disk;
	WORD    number_entry_CD;
	UINT   size_central_dir;
	UINT   offset_central_dir;
	WORD    size_comment;
#if defined(PLATFORM_MAC)
} __attribute__((packed));
#else
};
#endif


struct ZipFileHeader
{
	enum
	{
		SIGNATURE   = 0x02014b50
	};

	UINT signature;
    WORD version;               /* version made by                 2 bytes */
    WORD version_needed;        /* version needed to extract       2 bytes */
    WORD flag;                  /* general purpose bit flag        2 bytes */
    WORD compression_method;    /* compression method              2 bytes */
    UINT dosDate;              /* last mod file date in Dos fmt   4 bytes */
    UINT crc;                  /* crc-32                          4 bytes */
    UINT compressed_size;      /* compressed size                 4 bytes */
    UINT uncompressed_size;    /* uncompressed size               4 bytes */
    WORD size_filename;         /* fileName length                 2 bytes */
    WORD size_file_extra;       /* extra field length              2 bytes */
    WORD size_file_comment;     /* file comment length             2 bytes */

    WORD disk_num_start;        /* disk number start               2 bytes */
    WORD internal_fa;           /* internal file attributes        2 bytes */
    UINT external_fa;          /* external file attributes        4 bytes */

    UINT offset_curfile;       /* file offset                     4 bytes */

	const char* GetName() const		{  return (const char *)(this + 1);  }
	const char* GetExtra() const	{  return GetName() + size_filename;  }
	const char* GetComment() const	{  return GetExtra() + size_file_extra;  }
#if defined(PLATFORM_MAC)
} __attribute__((packed));
#else
};
#endif


struct ZipLocalHeader
{
	enum
	{
		SIGNATURE = 0x04034b50
	};
	UINT signature;
	WORD version_needed;		/* version needed to extract       2 bytes */
    WORD flag;                  /* general purpose bit flag        2 bytes */
    WORD compression_method;    /* compression method              2 bytes */
    UINT dosDate;              /* last mod file date in Dos fmt   4 bytes */
    UINT crc;                  /* crc-32                          4 bytes */
    UINT compressed_size;      /* compressed size                 4 bytes */
    UINT uncompressed_size;    /* uncompressed size               4 bytes */
    WORD size_filename;         /* fileName length                 2 bytes */
    WORD size_file_extra;       /* extra field length              2 bytes */
#if defined(PLATFORM_MAC)
} __attribute__((packed));
#else
};
#endif

#if defined(_MSC_VER)
#include <poppack.h>
#endif


static DWORD ConvertTime_tToDosTime(time_t time)
{
    struct tm* ptm = localtime(&time);
	DWORD year = (DWORD)ptm->tm_year;
	if (year > 1980)
		year -= 1980;
	else if (year > 80)
		year -= 80;
	return (DWORD) (((ptm->tm_mday) + (32 * (ptm->tm_mon+1)) + (512 * year)) << 16) |
			((ptm->tm_sec/2) + (32* ptm->tm_min) + (2048 * (DWORD)ptm->tm_hour));
}


static time_t ConvertDosDateToTime_t(DWORD dosDate)
{
	struct tm atm;
    ULONG uDate;
    uDate = (ULONG)(dosDate>>16);
    atm.tm_mday = (UINT)(uDate&0x1f);
    atm.tm_mon =  (UINT)(((uDate&0x1E0)/0x20)-1);
    atm.tm_year = (UINT)(((uDate&0x0FE00)/0x0200)+1980)-1900;

	atm.tm_hour = (UINT) ((dosDate & 0xF800)/0x800);
    atm.tm_min =  (UINT) ((dosDate & 0x7E0)/0x20);
    atm.tm_sec =  (UINT) (2*(dosDate & 0x1f));

	atm.tm_isdst = -1;

	return mktime(&atm);
}


///////////////////////////////////////////////////////////////////////////////
/**
	\internal Used with qsort().
**/
static int MISC_CDECL Compare(VirtualDrive::FileEntry* item1, VirtualDrive::FileEntry* item2)
{
#if defined(PLATFORM_WINDOWS)
	return (stricmp(item1->GetFileName(), item2->GetFileName()));
#else
	return (strcasecmp(item1->GetFileName(), item2->GetFileName()));
#endif
}


VirtualFileHandle::VirtualFileHandle() :
    nextOpenFile(NULL),
    prevOpenFile(NULL),
    parentDrive(NULL),
	curUncompressedFilePosition(0),
	fileEntryIndex(VirtualDrive::INVALID_FILE_ENTRY),
    bufferedData(NULL)
{
}


VirtualFileHandle::~VirtualFileHandle()
{
	if (fileEntryIndex != VirtualDrive::INVALID_FILE_ENTRY  &&  parentDrive)
		parentDrive->FileClose(*this);
}


///////////////////////////////////////////////////////////////////////////////
/* simple entropy collection function that uses the fast timer      */
/* since we are not using the random pool for generating secret     */
/* keys we don't need to be too worried about the entropy quality   */

static int entropy_fun(unsigned char buf[], unsigned int len)
{
	unsigned __int64    pentium_tsc[1];
	unsigned int        i;

	QueryPerformanceCounter((LARGE_INTEGER *)pentium_tsc);
	for(i = 0; i < 8 && i < len; ++i)
	buf[i] = ((unsigned char*)pentium_tsc)[i];
	return i;
}

/**
	The constructor.
**/
VirtualDrive::VirtualDrive() :
	m_ownParentFile(false),
	m_parentFile(NULL),
	m_changed(false),
	m_curWriteFile(NULL),
	m_readOnly(false),
    m_headOpenFile(NULL),
    m_fileEntryOffsets(NULL),
    m_fileEntries(NULL),
    m_fileEntriesSizeBytes(0),
	m_fileEntriesMaxSizeBytes(0),
	m_fileEntryCount(0),
	m_fileEntryMaxCount(0),
    m_fileName(NULL),
	m_dirOffset(0xffffffff),
	m_dirSize(0),
	m_needPack(false),
	m_unused(0)
{
//	prng_init(entropy_fun, rng);                /* initialise RNG   */
} // VirtualDrive()


///////////////////////////////////////////////////////////////////////////////
/**
	The destructor automatically closes all open virtual files and the virtual
	drive.
**/
VirtualDrive::~VirtualDrive()
{
	Close();
} // ~VirtualDrive()


///////////////////////////////////////////////////////////////////////////////
static void OpenHeaderCryptContext(const AnsiString& password, fcrypt_ctx zcx[1])
{
	int passwordLen = (int)password.Length();
	unsigned char encryptionMode = (passwordLen < 32 ? 1 : passwordLen < 48 ? 2 : 3);

	unsigned char salt[16];
//	prng_ctx rng[1];    /* the context for the random number pool   */
//	prng_init(entropy_fun, rng);                /* initialise RNG   */
//	prng_rand(salt, SALT_LENGTH(encryptionMode), rng);    /* and the salt     */

	unsigned int value = 255;
	unsigned int subtractValue = 255 / SALT_LENGTH(encryptionMode);
	for (int i = 0; i < SALT_LENGTH(encryptionMode); ++i)
	{
		salt[i] = value;
		value -= subtractValue;
	}

	unsigned char tmp_buf1[16];
	fcrypt_init(encryptionMode, (unsigned char*)(const char*)password, (unsigned int)passwordLen, salt, tmp_buf1, zcx);
}


/**
	Creates a new virtual drive on the disk.  The new virtual drive has no
	file entries.

	@param fileName The full path of the virtual drive.
	@return Returns true if the virtual drive was created successfully, false
		otherwise.
**/
bool VirtualDrive::Create(const char* fileName, const char* password)
{
	DiskFile* parentFile = new DiskFile;
    if (!parentFile->Open(fileName, File::MODE_CREATE | File::MODE_TRUNCATE | File::MODE_READWRITE))
	{
		delete parentFile;
		return false;
	}

	bool ret = Create(*parentFile, fileName, password);
	m_ownParentFile = true;
	return ret;
} // Create()


bool VirtualDrive::Create(File& parentFile, const char* fileName, const char* _defaultPassword)
{
	Close();

	// Save the fileName.
    delete[] m_fileName;
	m_fileName = new char[strlen(fileName) + 1];
    strcpy(m_fileName, fileName);

	this->defaultPassword = _defaultPassword;
	if (this->defaultPassword.Length() > 0)
	{
		OpenHeaderCryptContext(this->defaultPassword, this->defaultzcx);
	}

	// Create the virtual drive disk file.
	m_ownParentFile = false;
	m_parentFile = &parentFile;

	m_readOnly = false;

	// Remove all directory entries.
	delete[] m_fileEntries;
    m_fileEntries = NULL;
	m_curWriteFile = NULL;

	// Write the header.
	m_dirOffset = 0;
	m_dirSize = 0;
	m_needPack = false;
	this->fileNameMap.Clear();
	this->fileNameMap.SetBlockSize(100);
	this->fileNameMap.SetHashTableSize(100);
    m_changed = true;

    Flush();

	// Created successfully!
	return true;
} // Create()


///////////////////////////////////////////////////////////////////////////////
/**
	Opens an existing virtual drive from the disk.

	@param fileName The full path of the virtual drive.
	@return Returns true if the virtual drive was opened successfully, false
		otherwise.
**/
bool VirtualDrive::Open(const char* fileName, bool readOnly, const char* password)
{
	DiskFile* parentFile = new DiskFile();
	if (!parentFile->Open(fileName, File::MODE_READWRITE))
	{
		if (!parentFile->Open(fileName, File::MODE_READONLY))
		{
			// Couldn't open the file!
			delete parentFile;

			return false;
		}
		
		readOnly = true;
	}

	bool ret = Open(*parentFile, fileName, readOnly, password);
	m_ownParentFile = true;
	return ret;
}


bool VirtualDrive::Open(File& parentFile, const char* fileName, bool readOnly, const char* _defaultPassword)
{
	Close();

	// Save the fileName.
	m_fileName = new char[strlen(fileName) + 1];
    strcpy(m_fileName, fileName);

	// Open the file.
	this->defaultPassword = _defaultPassword;
	m_ownParentFile = false;
	m_parentFile = &parentFile;
	m_readOnly = readOnly;

	// Initialize decryption, if needed.
	fcrypt_ctx zcx[1];
	if (this->defaultPassword.Length() > 0)
	{
		OpenHeaderCryptContext(this->defaultPassword, this->defaultzcx);
		zcx[0] = this->defaultzcx[0];
	}

	// See if it is a .zip file.
	ZipDirHeader zipDirHeader;
	m_parentFile->Seek(m_parentFile->GetLength() - sizeof(ZipDirHeader));
	m_parentFile->Read(&zipDirHeader, sizeof(ZipDirHeader));
	if (this->defaultPassword.Length() > 0)
	{
		zcx[0] = this->defaultzcx[0];
		fcrypt_decrypt((unsigned char*)&zipDirHeader, sizeof(ZipDirHeader), zcx);
	}

	if (zipDirHeader.signature != ZipDirHeader::SIGNATURE)
        return false;

	// Allocate the data buffer, and read the whole thing.
	// TODO: Allocate this up top so it doesn't fragment.
	BYTE* zipDirBuffer = new BYTE[zipDirHeader.size_central_dir];
	m_parentFile->Seek(zipDirHeader.offset_central_dir);
	m_parentFile->Read(zipDirBuffer, zipDirHeader.size_central_dir);

	if (this->defaultPassword.Length() > 0)
	{
		zcx[0] = this->defaultzcx[0];
		fcrypt_decrypt((unsigned char*)zipDirBuffer, zipDirHeader.size_central_dir, zcx);
	}

	BYTE* zipDirPtr = zipDirBuffer;

	m_dirOffset = zipDirHeader.offset_central_dir;
	m_dirSize = zipDirHeader.size_central_dir;

	DWORD totalSize = 0;
    m_fileEntryCount = zipDirHeader.number_entry_CD;
	m_fileEntryMaxCount = m_fileEntryCount;
    if (m_fileEntryCount > 0)
        m_fileEntryOffsets = new size_t[m_fileEntryCount];
	this->fileNameMap.SetBlockSize(m_fileEntryCount + 1);
	this->fileNameMap.SetHashTableSize(m_fileEntryCount / 5);
	for (size_t i = 0; i < m_fileEntryCount; ++i)
	{
		ZipFileHeader* zipFileHeader = (ZipFileHeader*)zipDirPtr;
		if (zipFileHeader->signature != ZipFileHeader::SIGNATURE)
		{
			// Bad zip file.
			delete[] zipDirBuffer;
			Close();
			return false;
		}

        m_fileEntryOffsets[i] = totalSize;
		totalSize += sizeof(FileEntry) + zipFileHeader->size_filename;
		zipDirPtr += sizeof(ZipFileHeader) + zipFileHeader->size_filename + zipFileHeader->size_file_extra + zipFileHeader->size_file_comment;
	}
	zipDirPtr = zipDirBuffer;

	m_fileEntries = new BYTE[totalSize];
    m_fileEntriesSizeBytes = totalSize;
	m_fileEntriesMaxSizeBytes = totalSize;

    BYTE* fileEntryPtr = m_fileEntries;

	for (size_t i = 0; i < zipDirHeader.number_entry_CD; ++i)
	{
		ZipFileHeader* zipFileHeader = (ZipFileHeader*)zipDirPtr;

		FileEntry& fileEntry = *(FileEntry*)fileEntryPtr;
		fileEntry.m_fileTime = ConvertDosDateToTime_t(zipFileHeader->dosDate);
		fileEntry.m_offset = zipFileHeader->offset_curfile;
		fileEntry.m_compressedSize = zipFileHeader->compressed_size;
		fileEntry.m_uncompressedSize = zipFileHeader->uncompressed_size;
		fileEntry.m_crc = zipFileHeader->crc;
		fileEntry.m_compressionMethod = zipFileHeader->compression_method;
        fileEntry.m_parentDrive = this;

		WORD stringLen = zipFileHeader->size_filename;
		memcpy(fileEntry.m_fileName, zipFileHeader->GetName(), stringLen);
        fileEntry.m_fileName[stringLen] = 0;

		this->fileNameMap[SimpleString(this, i)] = i;

		fileEntryPtr += sizeof(FileEntry) + stringLen;

		zipDirPtr += sizeof(ZipFileHeader) + zipFileHeader->size_filename + zipFileHeader->size_file_extra + zipFileHeader->size_file_comment;
	}

	delete[] zipDirBuffer;

	this->fileNameMap.SetBlockSize(50);

	// Finish setting it up.
	m_changed = false;
	m_curWriteFile = NULL;

	// Opened successfully!
	return true;
} // Open()


///////////////////////////////////////////////////////////////////////////////
/**
	Closes the virtual drive, flushing any unsaved changed to disk.  All open
	virtual files are closed.

	@return Returns true if successful.
**/
bool VirtualDrive::Close()
{
	if (!IsOpened())
		return true;

	// Flush unsaved changed.
	Flush();

	// Close all open virtual files.
	FileCloseAll();

	// Remove all entries from the directory.
	delete[] m_fileEntries;
    m_fileEntries = NULL;
    m_fileEntriesSizeBytes = 0;
	m_fileEntriesMaxSizeBytes = 0;

    delete[] m_fileEntryOffsets;
    m_fileEntryOffsets = NULL;
    m_fileEntryCount = 0;
	m_fileEntryMaxCount = 0;

	this->fileNameMap.Clear();

	// Clear out the drive's fileName.
	delete[] m_fileName;
    m_fileName = NULL;

	// Clear the password.
	this->defaultPassword.Clear();

	// Destroy the File.
	if (m_ownParentFile)
	{
		delete m_parentFile;
	}
	m_parentFile = NULL;
	m_ownParentFile = false;

    m_dirOffset = 0;
    m_dirSize = 0;

	return true;
} // Close()


///////////////////////////////////////////////////////////////////////////////
/**
	Sorts the directory entries by fileName and writes it all to disk.
**/
bool VirtualDrive::Flush()
{
	// If there is no parent file, then the drive has never been opened.  Exit.
	if (!m_parentFile)
		return false;
	
	// If there have been no changes made, exit successfully.
	if (!m_changed)
		return true;

	fcrypt_ctx zcx[1];
	if (this->defaultPassword.Length() > 0)
		zcx[0] = this->defaultzcx[0];

	// Write the directory header id.
    LONGLONG offset = m_dirOffset;

	// Write all the directory entries.
    for (size_t i = 0; i < m_fileEntryCount; ++i)
	{
		FileEntry& entry = *GetFileEntry(i);
		ZipFileHeader zipFileHeader;
		zipFileHeader.signature = ZipFileHeader::SIGNATURE;
		zipFileHeader.version = 20;
		zipFileHeader.version_needed = 20;

		zipFileHeader.flag = 0;
		zipFileHeader.compression_method = (WORD)entry.m_compressionMethod;
		zipFileHeader.dosDate = ConvertTime_tToDosTime(entry.m_fileTime);
//		time_t t = ConvertDosDateToTime_t(zipFileHeader.dosDate);
		zipFileHeader.crc = entry.m_crc;
		zipFileHeader.compressed_size = entry.m_compressedSize;
		zipFileHeader.uncompressed_size = entry.m_uncompressedSize;
		int size_filename = (int)strlen(entry.GetFileName());
		zipFileHeader.size_filename = (WORD)size_filename;
		int size_file_extra = 0;
		zipFileHeader.size_file_extra = size_file_extra;
		zipFileHeader.size_file_comment = 0;
		zipFileHeader.disk_num_start = 0;
		zipFileHeader.internal_fa = 0;
		zipFileHeader.external_fa = 0;
		zipFileHeader.offset_curfile = entry.m_offset;

		m_parentFile->Seek(offset);
		if (this->defaultPassword.Length() > 0)
			fcrypt_encrypt((unsigned char*)&zipFileHeader, sizeof(ZipFileHeader), zcx);
		m_parentFile->Write(&zipFileHeader, sizeof(ZipFileHeader));
		if (this->defaultPassword.Length() > 0)
		{
			AnsiString tempFileName = entry.GetFileName();
			fcrypt_encrypt((unsigned char*)(const char*)tempFileName, (unsigned int)tempFileName.Length(), zcx);
			m_parentFile->Write((const char*)tempFileName, tempFileName.Length());
		}
		else
		{
			m_parentFile->Write((const char*)entry.GetFileName(), zipFileHeader.size_filename);
		}
		offset += sizeof(ZipFileHeader) + size_filename + size_file_extra;
	}

	m_dirSize = offset - m_dirOffset;

	if (this->defaultPassword.Length() > 0)
		zcx[0] = this->defaultzcx[0];

	ZipDirHeader zipDirHeader;
	zipDirHeader.signature = ZipDirHeader::SIGNATURE;
	zipDirHeader.number_disk = 0;
	zipDirHeader.number_disk_with_CD = 0;
	zipDirHeader.number_entry_this_disk = (WORD)m_fileEntryCount;
	zipDirHeader.number_entry_CD = (WORD)m_fileEntryCount;
	zipDirHeader.size_central_dir = (UINT)m_dirSize;
	zipDirHeader.offset_central_dir = (UINT)m_dirOffset;
	zipDirHeader.size_comment = 0;
	m_parentFile->Seek(offset);
	if (this->defaultPassword.Length() > 0)
		fcrypt_encrypt((unsigned char*)&zipDirHeader, sizeof(ZipDirHeader), zcx);
	m_parentFile->Write(&zipDirHeader, sizeof(ZipDirHeader));

	m_parentFile->SetLength(offset + sizeof(ZipDirHeader));

	// Reset to no changes.
	m_changed = false;

	return true;
} // Flush()


///////////////////////////////////////////////////////////////////////////////
/**
	Creates a new virtual file within the virtual drive for writing. Only one
	virtual file may be written to at any one time, because the new file is
	appended to the end of the virtual drive.

	If the new virtual file already exists within the directory entries, the
	old file of the same name is invalidated. The space the old file took
	becomes unused and not retrievable.  To "retrieve" the space, the virtual
	drive must be packed (see Pack()).

	@param fileName The fileName of the new virtual file to create.
	@param fileHandle A reference to the VirtualFileHandle object to be filled in
		with the newly created virtual file's information.
	@param fileTime A CTime object describing the virtual file's creation date
		and time.  The current date and time is used if fileTime is NULL,
	@return Returns true if the virtual file was created successfully, false
		otherwise.
**/
bool VirtualDrive::FileCreate(const char* fileName, VirtualFileHandle& fileHandle,
							  UINT compressionMethod, const time_t* fileTime)
{	
	// If the drive is read-only, then exit.
	if (m_readOnly)
		return false;

	// If there is a file currently being written to, then it must be closed
	// first.  Abort the file creation.
	if (m_curWriteFile)
		return false;

	// Close it if already opened.
	FileClose(fileHandle);

	// Retrieve the index of the file entry called fileName.
	size_t index = FindFileEntryIndex(fileName);

	// Does the entry exist?
	if (index == INVALID_FILE_ENTRY)
	{
		// No. It needs to be added.  Increase the file entry count by 1.
        index = m_fileEntryCount;

        size_t* origOffsets = m_fileEntryOffsets;
		if (m_fileEntryCount >= m_fileEntryMaxCount)
		{
			m_fileEntryMaxCount += 100;
	        m_fileEntryOffsets = new size_t[m_fileEntryMaxCount + 1];
			if (origOffsets)
			{
	            memcpy(m_fileEntryOffsets, origOffsets, m_fileEntryCount * sizeof(UINT));
				delete[] origOffsets;
			}
		}

		m_fileEntryOffsets[m_fileEntryCount] = m_fileEntriesSizeBytes;
		m_fileEntryCount++;

		size_t stringLen = strlen(fileName);
		BYTE* origBuf = m_fileEntries;
		size_t origSize = m_fileEntriesSizeBytes;

        m_fileEntriesSizeBytes += sizeof(FileEntry) + stringLen;
		if (m_fileEntriesSizeBytes >= m_fileEntriesMaxSizeBytes)
		{
			m_fileEntriesMaxSizeBytes += sizeof(FileEntry) + stringLen + 10 * 1024;
			m_fileEntries = new BYTE[m_fileEntriesMaxSizeBytes];
			if (origSize > 0)
				memcpy(m_fileEntries, origBuf, origSize);
			if (origBuf)
				delete[] origBuf;
		}
        FileEntry* destEntry = (FileEntry*)(m_fileEntries + origSize);
		memcpy(destEntry->m_fileName, fileName, stringLen + 1);
		
		this->fileNameMap[SimpleString(this, m_fileEntryCount - 1)] = index;
	}
	else
	{
		m_needPack = true;
	}

	// Fill in the new (or existing file entry).
	FileEntry& fileEntry = *GetFileEntry(index);

	// Add the proper file time.
	if (!fileTime)
	{
		fileEntry.m_fileTime = AdjustTime_t(time(NULL));
	}
	else
	{
		fileEntry.m_fileTime = AdjustTime_t(*fileTime);
	}

	fileEntry.m_offset = (UINT)m_dirOffset;
    fileEntry.m_compressedSize = 0;
    fileEntry.m_uncompressedSize = 0;
	fileEntry.m_crc = 0;
	fileEntry.m_compressionMethod = compressionMethod == UNCOMPRESSED ? 0 : Z_DEFLATED;
    fileEntry.m_parentDrive = this;

	// Assign the appropriate information to the virtual file object.
    fileHandle.parentDrive = this;
	fileHandle.fileEntryIndex = index;

    fileHandle.curUncompressedFilePosition = 0;
    fileHandle.curCompressedFilePosition = 0;

	if (compressionMethod == COMPRESSED)
	{
		fileHandle.bufferedData = new BYTE[Z_BUFSIZE];
		fileHandle.posInBufferedData = 0;

		fileHandle.stream.avail_in = 0;
		fileHandle.stream.avail_out = Z_BUFSIZE;
		fileHandle.stream.next_out = fileHandle.bufferedData;
		fileHandle.stream.total_in = 0;
		fileHandle.stream.total_out = 0;
		fileHandle.stream.opaque = 0;
		fileHandle.stream.zalloc = Z_NULL;
		fileHandle.stream.zfree = Z_NULL;

		deflateInit2(&fileHandle.stream, compressionMethod, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, 0);
	}

	fileHandle.headerSize = sizeof(ZipLocalHeader) + strlen(fileEntry.GetFileName()) + 0;

	if (this->defaultPassword.Length() > 0)
	{
		fileHandle.zcx[0] = this->defaultzcx[0];

		fileHandle.curCompressedFilePosition += 0; //SALT_LENGTH(mode) + PWD_VER_LENGTH;
	}

    // Assign this file to be our current write file.
	m_curWriteFile = &fileHandle;

	// Add this virtual file to the open files list.
	fileHandle.nextOpenFile = m_headOpenFile;
	if (m_headOpenFile)
		m_headOpenFile->prevOpenFile = &fileHandle;
	fileHandle.prevOpenFile = NULL;
	m_headOpenFile = &fileHandle;

	// A change to the directory has occurred.
	m_changed = true;

	return true;
} // FileCreate()


///////////////////////////////////////////////////////////////////////////////
/**
	Opens an existing virtual file within the virtual drive.

	@param fileName The fileName of the existing virtual file to open.
	@param fileHandle A reference to the VirtualFileHandle object to be filled in
		with the newly opened virtual file's information.
	@return Returns true if the virtual file was opened successfully, false
		otherwise.
**/
bool VirtualDrive::FileOpen(const char* fileName, VirtualFileHandle& fileHandle)
{
	// Close it if already opened.
	FileClose(fileHandle);

	// Retrieve the index of the file entry called fileName.
	size_t index = FindFileEntryIndex(fileName);
    if (index == INVALID_FILE_ENTRY)
        return false;

    return FileOpenIndex(index, fileHandle);
} // FileOpen()


bool VirtualDrive::FileOpenIndex(size_t index, VirtualFileHandle& fileHandle)
{
	// If the entry doesn't exist, then exit.
	if (index == INVALID_FILE_ENTRY)
		return false;

	// Close it if already opened.
	FileClose(fileHandle);

	// Grab the entry.
	FileEntry& fileEntry = *GetFileEntry(index);

	// Assign the internal virtual file information.
    fileHandle.parentDrive = this;
	fileHandle.fileEntryIndex = index;
    fileHandle.curUncompressedFilePosition = 0;

	ZipLocalHeader localHeader;

	m_parentFile->Seek(fileEntry.m_offset);
	m_parentFile->Read(&localHeader, sizeof(ZipLocalHeader));

	fcrypt_ctx headerzcx[1];
	if (this->defaultPassword.Length() > 0)
	{
		headerzcx[0] = this->defaultzcx[0];
		fcrypt_decrypt((unsigned char*)&localHeader, sizeof(ZipLocalHeader), headerzcx);
	}

	assert(localHeader.signature == ZipLocalHeader::SIGNATURE);

	fileHandle.headerSize = sizeof(ZipLocalHeader) + localHeader.size_filename + localHeader.size_file_extra;

    fileHandle.curCompressedFilePosition = 0;

	if (fileEntry.m_compressionMethod != 0)
	{
		fileHandle.stream.avail_in = 0;
	    fileHandle.stream.total_out = 0;

	    fileHandle.stream.opaque = 0;
        fileHandle.bufferedData = new BYTE[Z_BUFSIZE];
        fileHandle.stream.zalloc = Z_NULL;
        fileHandle.stream.zfree = Z_NULL;

		inflateInit2(&fileHandle.stream, -MAX_WBITS);
    }

	fileHandle.headerSize = sizeof(ZipLocalHeader) + strlen(fileEntry.GetFileName()) + 0;

	if (this->defaultPassword.Length() > 0)
	{
		fileHandle.zcx[0] = this->defaultzcx[0];

		fileHandle.curCompressedFilePosition += 0; //SALT_LENGTH(fileEntry.m_encryptionMode) + PWD_VER_LENGTH;
	}

    // Add this virtual file to the open files list.
	fileHandle.nextOpenFile = m_headOpenFile;
	if (m_headOpenFile)
		m_headOpenFile->prevOpenFile = &fileHandle;
	fileHandle.prevOpenFile = NULL;
	m_headOpenFile = &fileHandle;

	return true;
} // FileOpen()


///////////////////////////////////////////////////////////////////////////////
/**
	Closes an open virtual file.  This function should never really need to be
	called directory, as VirtualFileHandle::Close() can be used instead.

	@param fileHandle The VirtualFileHandle object to be closed.
	@return Returns true if the file was closed successfully.
**/
bool VirtualDrive::FileClose(VirtualFileHandle& fileHandle)
{
	if (fileHandle.fileEntryIndex == INVALID_FILE_ENTRY)
		return true;

	// Is the virtual file being closed the current virtual file being written
	// to?
	if (&fileHandle == m_curWriteFile)
	{
        VirtualDrive::FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);

		if (fileEntry->m_compressionMethod == COMPRESSED)
		{
			int err;
			do
			{
				ULONG uTotalOutBefore;
				if (fileHandle.stream.avail_out == 0)
				{
					m_parentFile->Seek(fileHandle.curCompressedFilePosition + fileEntry->m_offset + fileHandle.headerSize);
					if (this->defaultPassword.Length() > 0)
						fcrypt_encrypt_offset(fileHandle.bufferedData, fileHandle.posInBufferedData, fileHandle.zcx, (unsigned long)fileHandle.curCompressedFilePosition);
					m_parentFile->Write(fileHandle.bufferedData, fileHandle.posInBufferedData);
					fileHandle.curCompressedFilePosition += fileHandle.posInBufferedData;
					fileHandle.posInBufferedData = 0;
					fileHandle.stream.avail_out = (UINT)Z_BUFSIZE;
					fileHandle.stream.next_out = fileHandle.bufferedData;
				}
				uTotalOutBefore = fileHandle.stream.total_out;
				err=deflate((::z_stream*)&fileHandle.stream,  Z_FINISH);
				fileHandle.posInBufferedData += (UINT)(fileHandle.stream.total_out - uTotalOutBefore);
			} while (err == Z_OK);

			if (fileHandle.posInBufferedData > 0  &&  (err == Z_OK  ||  err == Z_STREAM_END))
			{
				m_parentFile->Seek(fileHandle.curCompressedFilePosition + fileEntry->m_offset + fileHandle.headerSize);
				if (this->defaultPassword.Length() > 0)
					fcrypt_encrypt_offset(fileHandle.bufferedData, fileHandle.posInBufferedData, fileHandle.zcx, (unsigned long)fileHandle.curCompressedFilePosition);
				m_parentFile->Write(fileHandle.bufferedData, fileHandle.posInBufferedData);
				fileHandle.curCompressedFilePosition += fileHandle.posInBufferedData;
			}

			fileEntry->m_uncompressedSize = fileHandle.stream.total_in;
			fileEntry->m_compressedSize = (DWORD)fileHandle.curCompressedFilePosition;  //stream.total_out;
		}
		else
		{
			fileEntry->m_compressedSize = (DWORD)fileHandle.curCompressedFilePosition;
			fileEntry->m_uncompressedSize = (DWORD)fileHandle.curUncompressedFilePosition;
		}
	}

	FileCloseInternal(fileHandle);

	return true;
}


void VirtualDrive::FileCloseInternal(VirtualFileHandle& fileHandle)
{
	VirtualDrive::FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);

	if (&fileHandle == m_curWriteFile)
	{
		// Yes.  Reassign the file header's directory offset.
        m_dirOffset = fileEntry->m_offset + fileHandle.curCompressedFilePosition + fileHandle.headerSize;

		// Write the changed parts of the header.
		ZipLocalHeader localHeader;
		localHeader.signature = ZipLocalHeader::SIGNATURE;
		localHeader.version_needed = 20;

		localHeader.flag = 0;
		localHeader.compression_method = (WORD)fileEntry->m_compressionMethod;
		localHeader.dosDate = ConvertTime_tToDosTime(fileEntry->m_fileTime);
		localHeader.crc = fileEntry->m_crc;
		localHeader.compressed_size = fileEntry->m_compressedSize;
		localHeader.uncompressed_size = fileEntry->m_uncompressedSize;
		localHeader.size_filename = (WORD)strlen(fileEntry->GetFileName());
		localHeader.size_file_extra = 0;

		fileHandle.headerSize = sizeof(ZipLocalHeader) + localHeader.size_filename + localHeader.size_file_extra;

		fcrypt_ctx headerzcx[1];
		m_parentFile->Seek(fileEntry->m_offset);
		if (this->defaultPassword.Length() > 0)
		{
			headerzcx[0] = this->defaultzcx[0];
			fcrypt_encrypt((unsigned char*)&localHeader, sizeof(ZipLocalHeader), headerzcx);
		}
		m_parentFile->Write(&localHeader, sizeof(ZipLocalHeader));
		if (this->defaultPassword.Length() > 0)
		{
			AnsiString tempFileName = fileEntry->GetFileName();
			fcrypt_encrypt((unsigned char*)(const char*)tempFileName, (unsigned int)tempFileName.Length(), headerzcx);
			m_parentFile->Write((const char*)tempFileName, tempFileName.Length());
		}
		else
		{
			m_parentFile->Write((const char*)fileEntry->GetFileName(), localHeader.size_filename);
		}

		// Turn off the current write file.
		m_curWriteFile = NULL;

    	if (fileEntry->m_compressionMethod == COMPRESSED)
			deflateEnd(&fileHandle.stream);
	}
    else
    {
    	if (fileEntry->m_compressionMethod == COMPRESSED)
    		inflateEnd(&fileHandle.stream);
    }

    delete[] fileHandle.bufferedData;
    fileHandle.bufferedData = NULL;
	fileHandle.fileEntryIndex = VirtualDrive::INVALID_FILE_ENTRY;

	// Remove the file from the open files list.
	if (&fileHandle == m_headOpenFile)
	{
		m_headOpenFile = fileHandle.nextOpenFile;
	}
	else
	{
		if (fileHandle.prevOpenFile)
			fileHandle.prevOpenFile->nextOpenFile = fileHandle.nextOpenFile;
	}
	if (fileHandle.nextOpenFile)
		fileHandle.nextOpenFile->prevOpenFile = fileHandle.prevOpenFile;
} // FileClose()


///////////////////////////////////////////////////////////////////////////////
/**
	Closes all open virtual files for the current virtual drive.
**/
void VirtualDrive::FileCloseAll()
{
	// Wander the open files list, closing each file.
    VirtualFileHandle* openFile = m_headOpenFile;
    while (openFile)
    {
        VirtualFileHandle* nextOpenFile = openFile->nextOpenFile;
        FileClose(*openFile);
        openFile = nextOpenFile;
    }

	// Remove all files from the open files list.
    m_headOpenFile = NULL;
}


const char* VirtualDrive::FileGetFileName(VirtualFileHandle& fileHandle)
{
    return GetFileEntry(fileHandle.fileEntryIndex)->m_fileName;
}

    
ULONGLONG VirtualDrive::FileGetPosition(VirtualFileHandle& fileHandle)
{
    return fileHandle.curUncompressedFilePosition;
}


void VirtualDrive::FileSetLength(VirtualFileHandle& fileHandle, ULONGLONG newLength)
{
	if (m_readOnly)
        return;

    assert(0);

    FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);

    if (newLength <= fileEntry->m_uncompressedSize)
	{
		fileEntry->m_uncompressedSize = (DWORD)newLength;
		m_changed = true;
		m_needPack = true;
	}
}


ULONGLONG VirtualDrive::FileGetLength(VirtualFileHandle& fileHandle)
{
    FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);
	return fileEntry->m_uncompressedSize;
}


LONGLONG VirtualDrive::FileSeek(VirtualFileHandle& fileHandle, LONGLONG offset, File::SeekFlags seekFlags)
{
	FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);

	if (fileEntry->GetCompressionMethod() == UNCOMPRESSED)
	{
		switch (seekFlags)
		{
			case File::SEEKFLAG_BEGIN:
				if (offset < 0  ||  (DWORD)offset > fileEntry->m_uncompressedSize)
					return -1;
				fileHandle.curCompressedFilePosition = offset;
				fileHandle.curUncompressedFilePosition = offset;
				break;

			case File::SEEKFLAG_CURRENT:
				if (this->defaultPassword.Length() > 0)
					assert(0);
				if ((offset + (LONG)fileHandle.curUncompressedFilePosition < 0)  ||
					(offset + (LONG)fileHandle.curUncompressedFilePosition > (LONG)fileEntry->m_uncompressedSize))
					return -1;
				fileHandle.curUncompressedFilePosition = fileHandle.curCompressedFilePosition = offset + fileHandle.curUncompressedFilePosition;
				break;

			case File::SEEKFLAG_END:
				if (this->defaultPassword.Length() > 0)
					assert(0);
				if (offset > 0  ||  (LONG)fileHandle.curUncompressedFilePosition + offset < 0)
					return -1;
				fileHandle.curUncompressedFilePosition = fileHandle.curCompressedFilePosition = fileEntry->m_uncompressedSize - offset;
				break;
		}
	}
	else
	{
		assert(0);
	}

	return fileHandle.curUncompressedFilePosition;
}


ULONGLONG VirtualDrive::FileRead(VirtualFileHandle& fileHandle, void* buffer, ULONGLONG count)
{
    FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);

	ULONGLONG endPos = (UINT)fileHandle.curUncompressedFilePosition + count;
	if (endPos > fileEntry->m_uncompressedSize)
		count -= endPos - fileEntry->m_uncompressedSize;

	if (fileEntry->GetCompressionMethod() == UNCOMPRESSED)
	{
		if (fileHandle.curUncompressedFilePosition >= fileEntry->m_uncompressedSize)
			return 0;

		m_parentFile->Seek(fileHandle.curCompressedFilePosition + fileEntry->m_offset + fileHandle.headerSize);
		count = m_parentFile->Read(buffer, count);
		if (this->defaultPassword.Length() > 0)
			fcrypt_decrypt_offset((unsigned char*)buffer, (unsigned int)count, fileHandle.zcx, (unsigned long)fileHandle.curCompressedFilePosition);
		fileHandle.curCompressedFilePosition += count;
		fileHandle.curUncompressedFilePosition += count;

		return count;
	}

    z_stream& stream = fileHandle.stream;
	stream.next_out = (BYTE*)buffer;
	stream.avail_out = (UINT)count;

	ULONGLONG rest_read_compressed = fileEntry->m_compressedSize - fileHandle.curCompressedFilePosition;
    ULONGLONG numRead = 0;

	while (stream.avail_out > 0)
	{
		if (stream.avail_in == 0  &&  rest_read_compressed > 0)
		{
			UINT uReadThis = Z_BUFSIZE;
			if (rest_read_compressed < uReadThis)
				uReadThis = (UINT)rest_read_compressed;
			if (uReadThis == 0)
				break;

			m_parentFile->Seek(fileHandle.curCompressedFilePosition + fileEntry->m_offset + fileHandle.headerSize);
			if (m_parentFile->Read(fileHandle.bufferedData, uReadThis) != uReadThis)
				return numRead;

			if (this->defaultPassword.Length() > 0)
				fcrypt_decrypt_offset((unsigned char*)fileHandle.bufferedData, (unsigned int)uReadThis, fileHandle.zcx, (unsigned long)fileHandle.curCompressedFilePosition);

			fileHandle.curCompressedFilePosition += uReadThis;

			rest_read_compressed -= uReadThis;

			stream.next_in = (BYTE*)fileHandle.bufferedData;
			stream.avail_in = (UINT)uReadThis;
		}

		ULONG uTotalOutBefore = stream.total_out;

		int err = inflate(&stream, Z_SYNC_FLUSH);

		size_t bytesInflated = stream.total_out - uTotalOutBefore;
        fileHandle.curUncompressedFilePosition += bytesInflated;
		numRead += bytesInflated;

		if (err != Z_OK)
			break;
	}

	return count;
}


ULONGLONG VirtualDrive::FileWrite(VirtualFileHandle& fileHandle, const void* buffer, ULONGLONG count)
{
	if (m_readOnly)
        return 0;

    VirtualDrive::FileEntry* fileEntry = GetFileEntry(fileHandle.fileEntryIndex);

	if (this->defaultPassword.Length() == 0)
		fileEntry->m_crc = crc32(fileEntry->m_crc, (BYTE*)buffer, (uInt)count);

	if (fileEntry->m_compressionMethod == UNCOMPRESSED)
	{
		m_parentFile->Seek(fileHandle.curCompressedFilePosition + fileEntry->m_offset + fileHandle.headerSize);

		if (this->defaultPassword.Length() > 0)
		{
			// Operate in 4k buffers.
			const UINT BUFFER_SIZE = 4 * 1024;
			BYTE cryptBuffer[BUFFER_SIZE];
			BYTE* inPtr = (BYTE*)buffer;
			ULONGLONG cryptCount = count;
			ULONGLONG curOffset = fileHandle.curCompressedFilePosition;
			while (cryptCount > 0)
			{
				// Copy the minimum of BUFFER_SIZE or the fileSize.
				UINT readSize = (UINT)(BUFFER_SIZE < cryptCount ? BUFFER_SIZE : cryptCount);
				memcpy(cryptBuffer, inPtr, readSize);
				fcrypt_encrypt_offset(cryptBuffer, readSize, fileHandle.zcx, (unsigned long)curOffset);
				m_parentFile->Write(cryptBuffer, readSize);
				cryptCount -= readSize;
				curOffset += readSize;
				inPtr += readSize;
			}
		}
		else
		{
			count = m_parentFile->Write(buffer, count);
		}
		fileHandle.curCompressedFilePosition += count;
		fileHandle.curUncompressedFilePosition += count;
		if (fileHandle.curUncompressedFilePosition > fileEntry->m_uncompressedSize)
			fileEntry->m_uncompressedSize = (DWORD)fileHandle.curUncompressedFilePosition;
		return count;
	}

	int err = Z_OK;

    z_stream& stream = fileHandle.stream;
	stream.next_in = (BYTE*)buffer;
	stream.avail_in = (uInt)count;

	while (err == Z_OK  &&  stream.avail_in > 0)
	{
		if (stream.avail_out == 0)
		{
			m_parentFile->Seek(fileHandle.curCompressedFilePosition + fileEntry->m_offset + fileHandle.headerSize);
			if (this->defaultPassword.Length() > 0)
				fcrypt_encrypt_offset(fileHandle.bufferedData, fileHandle.posInBufferedData, fileHandle.zcx, (unsigned long)fileHandle.curCompressedFilePosition);
			m_parentFile->Write(fileHandle.bufferedData, fileHandle.posInBufferedData);
			fileHandle.curCompressedFilePosition += fileHandle.posInBufferedData;
			fileHandle.posInBufferedData = 0;
			stream.avail_out = (UINT)Z_BUFSIZE;
			stream.next_out = fileHandle.bufferedData;
		}

		ULONG uTotalOutBefore = stream.total_out;
		err = deflate(&stream, Z_NO_FLUSH);
		fileHandle.posInBufferedData += (UINT)(stream.total_out - uTotalOutBefore);
	}

    return count;
}


///////////////////////////////////////////////////////////////////////////////
/**
	Erases the virtual file called [fileName].  The space the virtual file
	occupied is now unused and irretrievable, without using the Pack()
	function.

	@param fileName The fileName of the virtual file to be erased.
	@return Returns true if the file was erased or false if an error occured
		(such as the file specified by fileName not existing or being open
		already).
**/
bool VirtualDrive::FileErase(const char* fileName)
{
	if (m_readOnly)
		return false;

	// Retrieve the index of the file entry called fileName.
	size_t index = FindFileEntryIndex(fileName);

	// If the entry doesn't exist, then exit.
	if (index == INVALID_FILE_ENTRY)
		return false;

	// Wander the list of open files, checking if the file is not already open.
    for (VirtualFileHandle* openFile = m_headOpenFile; openFile; openFile = openFile->nextOpenFile)
    {
        FileEntry* fileEntry = GetFileEntry(openFile->fileEntryIndex);
        if (strcmp(fileEntry->GetFileName(), fileName) == 0)
            return false;
    }

	this->fileNameMap.Remove(SimpleString(this, index));

    // Before removing from the offsets, first resize our structure.
	if (index < m_fileEntryCount - 1)
	{
		// Reinsert all following entries into the map.
		for (size_t i = index + 1; i < m_fileEntryCount; ++i)
		{
			this->fileNameMap.Remove(SimpleString(this, i));
		}

		size_t sizeToRemove = m_fileEntryOffsets[index + 1] - m_fileEntryOffsets[index];
		memcpy(GetFileEntry(index), GetFileEntry(index + 1), m_fileEntriesSizeBytes - m_fileEntryOffsets[index + 1]);
        memcpy(m_fileEntryOffsets + index, m_fileEntryOffsets + index + 1, (m_fileEntryCount - index - 1) * sizeof(UINT));
		m_fileEntryCount--;

		// Reinsert all following entries into the map.
		for (size_t i = index; i < m_fileEntryCount; ++i)
		{
			m_fileEntryOffsets[i] -= sizeToRemove;
			this->fileNameMap[SimpleString(this, i)] = i;
		}

		// Fixup pointers.
		VirtualFileHandle* openFile = m_headOpenFile;
		while (openFile)
		{
			if (openFile->fileEntryIndex > index)
			{
				openFile->fileEntryIndex--;
			}
			openFile = openFile->nextOpenFile;
		}
	}
	else
	{
		m_fileEntryCount--;
	}

	// A directory change has occurred.
	m_changed = true;
	m_needPack = true;

	return true;
} // FileErase()


///////////////////////////////////////////////////////////////////////////////
/**
	Renames a virtual file from [oldFilename] to [newFilename].

	@param oldFilename The fileName of the virtual file to rename.
	@param newFilename The fileName to rename [oldFilename] to.
	@return Returns true if successful, false if an error occurred.
**/
bool VirtualDrive::FileRename(const char* oldFileName, const char* newFileName)
{
	if (m_readOnly)
		return false;

	// Retrieve the index of the file entry called oldFilename.
	size_t index = FindFileEntryIndex(oldFileName);

	// If it exists, then an entry called oldFilename is already there.
	if (index == INVALID_FILE_ENTRY)
		return false;

	// Get the file entry.
	FileEntry& fileEntry = *GetFileEntry(index);  (void)fileEntry;

	// Assign the new fileName.
//	m_fileNameMap.erase(oldFileName);
//	m_fileNameMap[newFileName] = index;

	// Detach the old buffer.
	size_t origSize = m_fileEntriesSizeBytes;  (void)origSize;
	BYTE* origBuffer = m_fileEntries;  (void)origBuffer;

	// Figure out the new size.
    size_t newFileNameLen = strlen(newFileName);
    size_t oldFileNameLen = strlen(oldFileName);
	int sizeDifference = (int)newFileNameLen - (int)oldFileNameLen;
	size_t newFileEntriesSize = ((int)m_fileEntriesSizeBytes + sizeDifference);
	if (newFileEntriesSize >= m_fileEntriesMaxSizeBytes)
	{
		m_fileEntriesMaxSizeBytes += sizeDifference + 10 * 1024;
		BYTE* newFileEntries = new BYTE[m_fileEntriesMaxSizeBytes];
		memcpy(newFileEntries, m_fileEntries, m_fileEntriesSizeBytes);
		delete[] m_fileEntries;
		m_fileEntries = newFileEntries;
	}

	if (index < m_fileEntryCount)
		memmove(m_fileEntries + m_fileEntryOffsets[index + 1] + sizeDifference, m_fileEntries + m_fileEntryOffsets[index + 1], m_fileEntriesSizeBytes - m_fileEntryOffsets[index + 1]);

	m_fileEntriesSizeBytes = ((int)m_fileEntriesSizeBytes + sizeDifference);

	BYTE* dest = m_fileEntries + m_fileEntryOffsets[index];
	memcpy(((FileEntry*)dest)->m_fileName, newFileName, newFileNameLen + 1);

	// Adjust all the file entry offsets.
	this->fileNameMap.Remove(SimpleString(this, index));
	this->fileNameMap[SimpleString(this, index)] = index;
	for (size_t i = index + 1; i < m_fileEntryCount; ++i)
	{
		m_fileEntryOffsets[i] += sizeDifference;
	}

	// The directory changed.
	m_changed = true;

	return true;
} // FileRename()


///////////////////////////////////////////////////////////////////////////////
/**
	Copies a file from [srcFile] to a virtual file called [destFilename].

	@param srcFile An open File object to copy the data from.
	@param destFilename The fileName of the virtual file to copy the data from
		[srcFile] to.
	@param fileTime A CTime object describing the virtual file's creation date
		and time.  The current date and time is used if fileTime is NULL,
**/	
bool VirtualDrive::FileCopy(VirtualFileHandle& srcFileHandle)
{
	if (m_readOnly)
		return false;

	// Operate in 16k buffers.
	const UINT BUFFER_SIZE = 16 * 1024;

    FileEntry* srcFileEntry = srcFileHandle.GetParentDrive()->GetFileEntry(srcFileHandle.fileEntryIndex);
    
    // Get the source file's size.
	UINT fileSize = srcFileEntry->m_compressedSize;

	// Create the destination virtual file.
	VirtualFileHandle destFileHandle;
	if (!FileCreate(srcFileEntry->GetFileName(), destFileHandle, srcFileEntry->m_compressionMethod, &srcFileEntry->m_fileTime))
		return false;
    FileEntry* destFileEntry = GetFileEntry(destFileHandle.fileEntryIndex);

	// Allocate the buffer space.
	BYTE* buffer = new BYTE[BUFFER_SIZE];

	// Go to the beginning of the source file.
    LONG srcOffset = srcFileEntry->m_offset + srcFileHandle.headerSize;
    LONG destOffset = destFileEntry->m_offset + destFileHandle.headerSize;

	// Keep copying until there is no more file left to copy.
	while (fileSize > 0)
	{
		// Copy the minimum of BUFFER_SIZE or the fileSize.
        UINT readSize = BUFFER_SIZE < fileSize ? BUFFER_SIZE : fileSize;
		srcFileHandle.parentDrive->m_parentFile->Seek(srcOffset);
        srcFileHandle.parentDrive->m_parentFile->Read(buffer, readSize);
		m_parentFile->Seek(destOffset);
        m_parentFile->Write(buffer, readSize);
		fileSize -= readSize;
        srcOffset += readSize;
        destOffset += readSize;
	}

	destFileEntry->m_crc = srcFileEntry->m_crc;
	destFileEntry->m_compressedSize = srcFileEntry->m_compressedSize;
	destFileEntry->m_uncompressedSize = srcFileEntry->m_uncompressedSize;

	destFileHandle.curUncompressedFilePosition = destFileEntry->m_compressedSize;
    destFileHandle.curCompressedFilePosition = destFileEntry->m_compressedSize;
	destFileHandle.stream.total_in = destFileEntry->m_uncompressedSize;
	destFileHandle.stream.total_out = destFileEntry->m_compressedSize;
	destFileHandle.stream.avail_out = 0;
	destFileHandle.posInBufferedData = 0;

	// Close the destination virtual file.
	FileCloseInternal(destFileHandle);

	// Destroy the buffer.
	delete [] buffer;

	return true;
} // FileCopy()


bool VirtualDrive::FileCopy(File& srcFile, const char* destFilename, UINT compressionMethod, const time_t* fileTime)
{
	if (m_readOnly)
		return false;

	// Operate in 32k buffers.
	const UINT BUFFER_SIZE = 32 * 1024;

	// Get the source file's size.
	UINT fileSize = (UINT)srcFile.GetLength();

	// See if the destination file exists.
	VirtualFileHandle destFileHandle;
	bool opened = false;
	FileEntry* fileEntry = FindFileEntry(destFilename);  (void)fileEntry;

	// Was the file opened yet?
	if (!opened)
	{
		// Create the destination virtual file.
		if (!FileCreate(destFilename, destFileHandle, compressionMethod, fileTime))
			return false;
	}

	// Allocate the buffer space.
	BYTE* buffer = new BYTE[BUFFER_SIZE];

	// Go to the beginning of the source file.
    LONG offset = 0;

	// Keep copying until there is no more file left to copy.
	while (fileSize > 0)
	{
		// Copy the minimum of BUFFER_SIZE or the fileSize.
        UINT readSize = BUFFER_SIZE < fileSize ? BUFFER_SIZE : fileSize;
		srcFile.Seek(offset);
		srcFile.Read(buffer, readSize);
		FileWrite(destFileHandle, buffer, readSize);
		fileSize -= readSize;
        offset += readSize;
	}

	// Close the destination virtual file.
	FileClose(destFileHandle);

	// Destroy the buffer.
	delete [] buffer;

	return true;
} // FileCopy()


/**
**/
bool VirtualDrive::FileCopy(const char* srcFileName, const char* destFileName, UINT compressionMethod, const time_t* inFileTime)
{
	DiskFile file;
    if (!file.Open(srcFileName, File::MODE_READONLY))
		return false;

	time_t fileTime = inFileTime ? *inFileTime : file.GetLastWriteTime();
	return FileCopy(file, destFileName, compressionMethod, &fileTime) != false;
}


///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/**
	Retrieves the directory file entry at [index].

	@param index The index of the file entry, range 0 to GetFileEntryCount() - 1.
	@return Returns the FileEntry object describing the requested file entry or
		NULL if the index is out of range.
**/
VirtualDrive::FileEntry* VirtualDrive::GetFileEntry(size_t index)
{
	// See if the requested index is in range.
	if (index >= (size_t)m_fileEntryCount)
		return NULL;

	// Return the appropriate file entry.
    return (FileEntry*)(m_fileEntries + m_fileEntryOffsets[index]);
}


///////////////////////////////////////////////////////////////////////////////
/**
	Retrieves the FileEntry object of the directory file entry called [fileName].

	@param fileName The non-case sensitive fileName of the file entry to find.
	@return Returns the FileEntry object describing the requested file entry or
		NULL if it isn't found.
**/
VirtualDrive::FileEntry* VirtualDrive::FindFileEntry(const char* fileName)
{
	// Lookup the fileName.
	return GetFileEntry(FindFileEntryIndex(fileName));
}


///////////////////////////////////////////////////////////////////////////////
/**
	Retrieves the index of the directory file entry called [fileName].

	@param fileName The non-case sensitive fileName of the file entry to find.
	@return Returns the index of the found file entry or INVALID_FILE_ENTRY if
		it wasn't found.
**/

size_t VirtualDrive::FindFileEntryIndex(const char* fileName)
{
	// If there are no file entries, it is pointless to perform the rest of the
	// search.
	if (m_fileEntryCount == 0)
		return INVALID_FILE_ENTRY;

	FileNameMap::Node* node = this->fileNameMap.Find(SimpleString(fileName));
	if (!node)
		return INVALID_FILE_ENTRY;
	return this->fileNameMap.Value(node);
} // FindFileEntryIndex


///////////////////////////////////////////////////////////////////////////////
/**
	Packs a virtual drive, removing all unused space.
**/	
bool VirtualDrive::Pack(FileOrderInfoArray* fileOrder)
{	
	if (m_readOnly)
		return false;

	// Flush.
	Flush();

	// See if it is already in order.
    if (fileOrder)
    {
		if (fileOrder->Count() != GetFileEntryCount())
		{
			m_needPack = true;
		}
		else
		{
			size_t curOffset = 0;
			for (size_t i = 0; i < fileOrder->Count(); ++i)
			{
				AnsiString& fileOrderFileName = (*fileOrder)[i].entryName;
				FileEntry* fileEntry = GetFileEntry(i);
				if (fileOrderFileName != fileEntry->GetFileName()  ||  fileEntry->GetOffset() != curOffset)
				{
					m_needPack = true;
					break;
				}

				curOffset += sizeof(ZipLocalHeader) + strlen(fileEntry->GetFileName()) + fileEntry->GetCompressedSize();
			}
		}
	}

	// Do we need to pack it?
	if (!m_needPack)
		return true;

	// Generate a unique temporary name.
#if defined(_MSC_VER)
	char* asciiTempName = _tempnam(NULL, "VDRIVEPACK_");
#else
	char* asciiTempName = tempnam(NULL, "VDRIVEPACK_");
#endif
    AnsiString newFileName = asciiTempName;
	free(asciiTempName);

	// Create a new virtual drive.
	VirtualDrive newDrive;
	AnsiString oldPassword = this->defaultPassword;
	newDrive.Create(newFileName, oldPassword);
	newDrive.fileNameMap.SetHashTableSize(this->fileNameMap.GetHashTableSize());
	newDrive.fileNameMap.SetBlockSize(this->fileNameMap.Count());
	newDrive.m_fileEntryMaxCount = this->m_fileEntryMaxCount + 1;		// +1 to avoid a realloc.
	newDrive.m_fileEntryOffsets = new size_t[newDrive.m_fileEntryMaxCount];
	newDrive.m_fileEntriesMaxSizeBytes = this->m_fileEntriesMaxSizeBytes + 1;	// +1 to avoid a realloc.
	newDrive.m_fileEntries = new BYTE[newDrive.m_fileEntriesMaxSizeBytes];

    if (fileOrder)
    {
        for (size_t i = 0; i < fileOrder->Count(); ++i)
        {
            AnsiString& fileName = (*fileOrder)[i].entryName;

	        VirtualFileHandle srcFileHandle;
	        if (FileOpen(fileName, srcFileHandle))
	        {
	            if (!newDrive.FileCopy(srcFileHandle))
                {
                    FileClose(srcFileHandle);
                    newDrive.Close();
#if defined(_MSC_VER)
                    _unlink(newFileName);
#else
					unlink(newFileName);
#endif
                    return false;
                }
                FileClose(srcFileHandle);
            }
        }
    }
	else
	{
		// Go through the entire directory.
		for (UINT i = 0; i < m_fileEntryCount; i++)
		{
			// Grab the file entry.
			FileEntry& fileEntry = *GetFileEntry(i);
			
			// Already processed?
			if (newDrive.FindFileEntry(fileEntry.GetFileName()))
				continue;
			
			// Open the source file.
			VirtualFileHandle srcFile;
			if (!FileOpen(fileEntry.GetFileName(), srcFile))
			{
				newDrive.Close();
#if defined(_MSC_VER)
				_unlink(newFileName);
#else
				unlink(newFileName);
#endif
				return false;
			}

			// Copy the source file to the new virtual drive's destination virtual
			// file.
			if (!newDrive.FileCopy(srcFile))
			{
				newDrive.Close();
#if defined(_MSC_VER)
				_unlink(newFileName);
#else
				unlink(newFileName);
#endif
				return false;
			}

			// Close the source file.
			FileClose(srcFile);
		}
	}

	// Close the new virtual drive.
	newDrive.fileNameMap.SetBlockSize(50);
	newDrive.Close();

	// Close the current virtual drive.
	AnsiString oldDriveName = m_fileName;
	Close();

#if defined(_MSC_VER)
	// Copy the packed drive.
	if (!::MoveFile(newFileName, oldDriveName))
		::CopyFile(newFileName, oldDriveName, false);
	
	_unlink(newFileName);
#else
	assert(0);
	unlink(newFileName);
#endif

	this->m_needPack = false;

	// Open it with the new 
	return Open(oldDriveName, false, oldPassword);
} // Pack()


/**
**/
bool VirtualDrive::ProcessFileList(const char* fileListFileName)
{
	FileOrderInfoArray fileOrder;

    FILE* file = fopen(fileListFileName, "rt");
    if (!file)
        return false;

    while (true)
	{
		const size_t MISC_MAX_PATH = 1000;
        char srcPath[MISC_MAX_PATH];
        if (!fgets(srcPath, MISC_MAX_PATH, file)  ||  srcPath[0] == 0)
            break;
        srcPath[strlen(srcPath) - 1] = 0;

        char entryName[MISC_MAX_PATH];
        if (!fgets(entryName, MISC_MAX_PATH, file)  ||  entryName[0] == 0)
            break;
        entryName[strlen(entryName) - 1] = 0;

        // Change to forward slashes.
        char* ptr = srcPath;
        while (*ptr)
        {
            if (*ptr == '\\')
                *ptr = '/';
            ptr++;
        }

#if defined(PLATFORM_WINDOWS)
		if (_access(srcPath, 0) == 0)
#else
		if (access(srcPath, 0) == 0)
#endif
        {
			FileOrderInfo fileOrderInfo;
			fileOrderInfo.srcPath = srcPath;
			fileOrderInfo.entryName = entryName;
            fileOrder.Add(fileOrderInfo);
		}
	}

    fclose(file);

	bool ret = true;
    if (fileOrder.Count() > 0)
	{
		ret = ProcessFileList(fileOrder);
	}

    return ret;
}


#if defined(PLATFORM_WINDOWS)

time_t ConvertFILETIME_To_time_t(const FILETIME& fileTime)
{
	SYSTEMTIME universalSystemTime;
	FileTimeToSystemTime(&fileTime, &universalSystemTime);

	SYSTEMTIME sysTime;
	SystemTimeToTzSpecificLocalTime(NULL, &universalSystemTime, &sysTime);

	// then convert the system time to a time_t (C-runtime local time)
	if (sysTime.wYear < 1900)
	{
		return 0;
	}

	TIME_ZONE_INFORMATION timeZone;
	int dst = GetTimeZoneInformation(&timeZone) == TIME_ZONE_ID_STANDARD;
	
	struct tm atm;
	atm.tm_sec = sysTime.wSecond & ~1;		// Zip files are only accurate to 2 seconds.
	atm.tm_min = sysTime.wMinute;
	atm.tm_hour = sysTime.wHour;
	atm.tm_mday = sysTime.wDay;
	atm.tm_mon = sysTime.wMonth - 1;        // tm_mon is 0 based
	atm.tm_year = sysTime.wYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = -1;
	time_t t = mktime(&atm);
	return t;
}

#endif


/**
**/
static DWORD GetFileCRC(FILE* file, UINT startOffset)
{
	const DWORD FILE_BLOCK_SIZE = 32768;
	BYTE* buffer = new BYTE[FILE_BLOCK_SIZE];
	DWORD fileCRC = 0;
    fseek(file, 0, SEEK_END);
	UINT fileLen = ftell(file);
    fseek(file, startOffset, SEEK_SET);
	DWORD bytesToDo = fileLen - startOffset;
	while (bytesToDo > 0)
	{
		DWORD bytesToRead = Min(bytesToDo, FILE_BLOCK_SIZE);

        fread(buffer, bytesToRead, 1, file);
		bytesToDo -= bytesToRead;
		fileCRC = crc32(fileCRC, buffer, bytesToRead);
	}

    fseek(file, 0, SEEK_SET);

	delete [] buffer;

	return fileCRC;
}


bool VirtualDrive::ProcessFileList(VirtualDrive::FileOrderInfoArray& fileOrder)
{
    typedef Map<AnsiString, FileOrderInfo*> FileNameMap;
	FileNameMap fileNameMap;

	// The lists of files and directories we need.
    for (size_t i = 0; i < fileOrder.Count(); ++i)
	{
#if defined(PLATFORM_WINDOWS)
		FileOrderInfo& fileOrderInfo = fileOrder[i];
		
		WIN32_FIND_DATA fd;
		if (GetFileAttributesEx(fileOrderInfo.srcPath, GetFileExInfoStandard, &fd))
		{
			fileOrderInfo.writeTime = ConvertFILETIME_To_time_t(fd.ftLastWriteTime);
			fileOrderInfo.needUpdate = true;
			fileOrderInfo.crc = 0;
			fileOrderInfo.size = fd.nFileSizeLow;

			fileNameMap[fileOrderInfo.entryName.Lower()] = &fileOrderInfo;
		}
		else
		{
			this->errorString = "Unable to open file [" + fileOrderInfo.srcPath + "].";
			return false;
		}
#else
		assert(0);
#endif
	}

	for (UINT i = 0; i < GetFileEntryCount(); ++i)
	{
		FileEntry* entry = GetFileEntry(i);

		AnsiString lowerEntryName = entry->GetFileName();
		lowerEntryName = lowerEntryName.Lower();

		FileNameMap::Node* node = fileNameMap.Find(lowerEntryName);
		if (!node)
		{
			printf("  Erasing %s... done!\n", entry->GetFileName());
			if (FileErase(entry->GetFileName()))
				--i;
            m_needPack = true;

			continue;
		}

		FileOrderInfo* info = fileNameMap.Value(node);

        // Check the time stamps and CRCs.
        if (strcmp(entry->GetFileName(), info->entryName) != 0)
        {
			FileRename(entry->GetFileName(), info->entryName);
		}

        time_t entryTimeStamp = entry->GetTimeStamp();
		struct tm atm1 = *localtime(&entryTimeStamp);
		struct tm atm2 = *localtime(&info->writeTime);

		info->needUpdate = atm1.tm_year != atm2.tm_year  ||  atm1.tm_mday != atm2.tm_mday  ||  atm1.tm_mon != atm2.tm_mon;
        if (!info->needUpdate)
        {
			int seconds1 = atm1.tm_hour * 3600 + atm1.tm_min * 60 + atm1.tm_sec;
			int seconds2 = atm2.tm_hour * 3600 + atm2.tm_min * 60 + atm2.tm_sec;
            info->needUpdate = (abs(seconds2 - seconds1) > 2);
		}

        if (info->needUpdate)
        {
            FILE* file = fopen(info->srcPath, "rb");
            if (file)
			{
				info->crc = GetFileCRC(file, 0);
                fclose(file);
			}

		    if (info->crc == entry->GetCRC())
		    {
                entry->SetTimeStamp(info->writeTime);

			    info->needUpdate = false;
			    continue;
		    }
        }
	}

	for (size_t index = 0; index < fileOrder.Count(); ++index)
	{
		if (index >= GetFileEntryCount())
		{
			// The rest of the files will be appended at the end.
			break;
		}

		FileEntry* entry = GetFileEntry(index);
		if (fileOrder[index].entryName.CompareNoCase(entry->GetFileName()) != 0)
		{
			m_needPack = true;
            break;
		}
	}

	for (size_t index = 0; index < fileOrder.Count(); ++index)
	{
		FileOrderInfo& info = fileOrder[index];

		if (!info.needUpdate)
			continue;

		// Create the data file.
		DiskFile diskFile;
		diskFile.Open(info.srcPath);
        FileCopy(diskFile, info.entryName, info.compressionMethod, &info.writeTime);
	}

	Pack(&fileOrder);

	return true;
}


time_t VirtualDrive::AdjustTime_t(time_t timeToAdjust)
{
	DWORD convertedTime = ConvertTime_tToDosTime(timeToAdjust);
	return ConvertDosDateToTime_t(convertedTime);
}


} // namespace Misc
