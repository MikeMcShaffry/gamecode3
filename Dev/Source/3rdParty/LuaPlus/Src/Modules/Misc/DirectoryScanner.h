#ifndef MISC_DIRECTORYSCANNER_H
#define MISC_DIRECTORYSCANNER_H

#include "Misc.h"
#include "AnsiString.h"
#include <time.h>
#include "List.h"

namespace Misc {

class DirectoryEntry
{
public:
	DWORD isDirectory:1;
	DWORD isReadOnly:1;
	time_t creationTime;
	time_t lastAccessTime;
	time_t lastWriteTime;
	ULONGLONG fileSize;
	AnsiString name;
	AnsiString path;
};


class DirectoryScanner : public List<DirectoryEntry>
{
public:
	enum ScanType
	{
		SCAN_FILES,
		SCAN_DIRECTORIES,
		SCAN_BOTH,
	};

	DirectoryScanner();
	DirectoryScanner(const AnsiString& path, const char* fileSpec = "*.*", ScanType scanType = SCAN_BOTH, bool recursive = false);
	void Scan(const AnsiString& path, const char* fileSpec = "*.*", ScanType scanType = SCAN_BOTH, bool recursive = false);

protected:
	void ScanHelper(const AnsiString& path, const char* fileSpec = "*.*", ScanType scanType = SCAN_BOTH, bool recursive = false);
};

} // namespace Misc

#endif // MISC_DIRECTORYSCANNER_H
