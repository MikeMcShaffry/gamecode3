#include "Misc_InternalPch.h"
#include "DirectoryScanner.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#elif defined(PLATFORM_MAC)
#endif

namespace Misc {

#if defined(PLATFORM_WINDOWS)
time_t ConvertFILETIME_To_time_t(const FILETIME& fileTime);
#endif

namespace {

class DirectoryEntrySort
{
public:
	bool operator()(DirectoryEntry& left, DirectoryEntry& right)
	{
		return left.name.CompareNoCase(right.name) == -1;
	}
};


} // private namespace

DirectoryScanner::DirectoryScanner()
{
}


DirectoryScanner::DirectoryScanner(const AnsiString& path, const char* fileSpec, ScanType scanType, bool recursive)
{
	Scan(path, fileSpec, scanType, recursive);
}


void DirectoryScanner::Scan(const AnsiString& inPath, const char* fileSpec, ScanType scanType, bool recursive)
{
	this->Empty();

	ScanHelper(inPath, fileSpec, scanType, recursive);
	
	Sort(DirectoryEntrySort());
}


void DirectoryScanner::ScanHelper(const AnsiString& inPath, const char* fileSpec, ScanType scanType, bool recursive)
{
	AnsiString path = inPath.Replace('\\', '/');
	if (path[path.Len() - 1] != '/')
		path += "/";

#if defined(PLATFORM_WINDOWS)
	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile(path + fileSpec, &fd);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	List<AnsiString> directories;

	do
	{
		bool isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		bool isDotDot = isDirectory  &&  (strcmp(fd.cFileName, ".") == 0  ||  strcmp(fd.cFileName, "..") == 0);
		bool skip = isDotDot  || (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM));
		if (scanType == SCAN_FILES)
			skip |= isDirectory;
		else if (scanType == SCAN_DIRECTORIES)
			skip |= !isDirectory;
		if (!skip)
		{
			DirectoryEntry& entry = this->Value(this->InsertAfterTail());
			entry.isDirectory = isDirectory;
			entry.isReadOnly = fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
			entry.creationTime = ConvertFILETIME_To_time_t(fd.ftCreationTime);
			entry.lastAccessTime = ConvertFILETIME_To_time_t(fd.ftLastAccessTime);
			entry.lastWriteTime = ConvertFILETIME_To_time_t(fd.ftLastWriteTime);
			entry.fileSize = ((ULONGLONG)fd.nFileSizeHigh * ((ULONGLONG)MAXDWORD+1)) + fd.nFileSizeLow;
			entry.name = fd.cFileName;
			entry.path = path;
		}

		if (isDirectory  &&  recursive  &&  !isDotDot)
			directories.InsertAfterTail(path + fd.cFileName);
	} while (FindNextFile(handle, &fd));

	for (List<AnsiString>::Node* node = directories.Head(); node; node = directories.Next(node))
	{
		ScanHelper(directories.Value(node) + "/", fileSpec, scanType, recursive);
	}
#else
	assert(0);
#endif
}

} // namespace Misc
