///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDriveCompiler.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDriveCompiler.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "Misc_InternalPch.h"
#include "VirtualDriveCompiler.h"
#include "VirtualDrive.h"
#include "VirtualFile.h"
#include <direct.h>
#include <io.h>
#include "zlib.h"

#include <pshpack1.h>

struct PackHeader
{
	char id[4];  // PACK
	DWORD packSize;
	DWORD unpackSize;
};

#include <poppack.h>

// FileInfo structure
struct FileInfo
{
	wxString m_name;
	FILETIME m_writeTime;
	bool m_needUpdate;
	DWORD m_crc;

	bool operator<(const FileInfo& info) const
	{
		return m_name < info.m_name;
	}
};


static void MakeDir(wxString path)
{
	CreateDirectory(path, NULL);
/*	// Skip the c:/ part.
	int slashPos = path.Find('/');
	slashPos = path.Find('/', slashPos + 1);

	while (slashPos != -1)
	{
		_mkdir(path.Left(slashPos));
		slashPos = path.Find('/', slashPos + 1);
	}
*/
}


///////////////////////////////////////////////////////////////////////////////
VirtualDriveCompiler::VirtualDriveCompiler() :
    m_compressFiles(false),
    m_recurseDirectories(false),
    m_recreate(false),
    m_packOnChange(false),
    m_packAlways(false),
    m_deleteNonExistentFiles(false)
{

}


VirtualDriveCompiler::~VirtualDriveCompiler()
{
}

WX_DECLARE_LIST(FileInfo, FileNamesList);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(FileNamesList);

WX_DECLARE_LIST(wxString, DirNamesList);
WX_DEFINE_LIST(DirNamesList);

        
/**
**/
void VirtualDriveCompiler::ProcessFiles(wxString srcDir, wxString origSrcDir, wxString origDestDir)
{
	bool changed = false;
	bool doPack = false;
	VirtualDrive drive;

	// The lists of files and directories we need.
    FileNamesList fileNames;
    DirNamesList dirNames;

	// Gather a list of all the files.
	WIN32_FIND_DATA fd;
	HANDLE findHandle = FindFirstFile(srcDir + "*.*", &fd);
	if (findHandle == INVALID_HANDLE_VALUE)
		return;
	
	// Go until we're done.
	while (true)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Add to the directory names list.
			if (strcmp(fd.cFileName, ".") != 0  &&  strcmp(fd.cFileName, "..") != 0)
				dirNames.Append(&wxString(fd.cFileName));
		}
		else
		{
			// Add to the file names list.
			FileInfo info;
			info.m_name = fd.cFileName;
			info.m_name.MakeLower();

			// Filter .res files.
			bool addFile = true;
			int dotPos = info.m_name.ReverseFind('.');
			if (dotPos != -1)
			{
				wxString ext = info.m_name.Mid(dotPos + 1);
				if (ext == "res")
					addFile = false;
			}

			if (addFile)
			{
				info.m_writeTime = fd.ftLastWriteTime;
				info.m_needUpdate = true;
				info.m_crc = 0;

				fileNames.AddTail(info);
			}
		}
		if (!FindNextFile(findHandle, &fd))
			break;
	}

	FindClose(findHandle);

	// Sort the filenames.
	fileNames.Sort();
	dirNames.Sort();

	// Print the directory.
	bool firstTimeDir = true;

	// Figure out the virtual drive path.
    int prevSrcOrigSlash = origSrcDir.Left(origSrcDir.GetLength() - 1).ReverseFind('/');
	wxString newOrigSrcDir = origSrcDir.Left(prevSrcOrigSlash + 1);
    int prevSlash = srcDir.Left(srcDir.GetLength() - 1).ReverseFind('/');
	wxString vdSrcPath = srcDir.Left(prevSlash + 1);
	wxString vdDestPath;
	if (newOrigSrcDir.GetLength() == vdSrcPath.GetLength())
		vdDestPath = origDestDir;
	else
		vdDestPath = origDestDir + vdSrcPath.Mid(newOrigSrcDir.GetLength());
	MakeDir(vdDestPath);
	wxString vdName = vdDestPath + srcDir.Mid(prevSlash + 1, srcDir.GetLength() - prevSlash - 2) + ".res";

	// Check that the virtual drive is writable.
	if (_access(vdName, 0) != -1)
	{
		if (_access(vdName, 2) == -1)
		{
			printf("Virtual drive file [%s] is read-only.\n", vdName);
			goto DoDirectories;
		}
	}
	
	// Open the existing virtual drive if no recreation is needed.  Create a new
	// virtual drive based on the directory name.
	if (!m_recreate)
	{
		if (!drive.Open(vdName))
		{
			drive.Create(vdName);
		}
		else
		{
			doPack = true;
		}
	}
	else
	{
		drive.Create(vdName);
	}

	// Walk the open virtual drive and flag any changed files.
	if (!m_recreate)
	{
		POSITION findPos = NULL;
		POSITION lastFindPos = findPos;
		int entryCount = drive.GetFileEntryCount();
		for (int i = 0; i < entryCount; i++)
		{
			VirtualDrive::FileEntry* entry = drive.GetFileEntry(i);

			// Get the file's extension.
			wxString entryName = entry->GetFileName();
			int dotPos = entryName.ReverseFind('.');
			wxString ext;
			if (dotPos != -1)
				ext = entryName.Mid(dotPos + 1);
			ext.MakeLower();

			// Check the ignore extensions list.
			bool ignoreFile = false;
			POSITION pos = m_ignoreExtensions.GetHeadPosition();
			while (pos)
			{
				wxString ignoreExt = m_ignoreExtensions.GetAt(pos);
				if (ext == ignoreExt)
				{
					if (m_deleteNonExistentFiles)
					{
						if (firstTimeDir)
						{
							printf("Directory %s...\n", srcDir);
							firstTimeDir = false;
						}
						printf("  Erasing %s... done!\n", entry->GetFileName());
						drive.FileErase(entry->GetFileName());
						changed = true;
					}
					ignoreFile = true;
					break;
				}
				m_ignoreExtensions.GetNext(pos);
			}

			if (ignoreFile)
				continue;

			// If the file isn't in our current list of files, then erase it
			// from the data drive.
			findPos = fileNames.GetHeadPosition();
			while (findPos)
			{
				FileInfo& info = fileNames.GetAt(findPos);
				if (entryName == info.m_name)
					break;
				fileNames.GetNext(findPos);
			}

			if (!findPos)
			{
				if (m_deleteNonExistentFiles)
				{
					if (firstTimeDir)
					{
						printf("Directory %s...\n", srcDir);
						firstTimeDir = false;
					}
					printf("  Erasing %s... done!\n", entry->GetFileName());
					drive.FileErase(entry->GetFileName());
					changed = true;
				}

				findPos = lastFindPos;
				continue;
			}

			// Check the ignore extensions list.
			bool forceFile = false;
			pos = m_forceExtensions.GetHeadPosition();
			while (pos)
			{
				wxString forceExt = m_forceExtensions.GetAt(pos);
				if (ext == forceExt)
				{
					forceFile = true;
					break;
				}
				m_forceExtensions.GetNext(pos);
			}
			
			FileInfo& info = fileNames.GetAt(findPos);
			DWORD vfCrc = entry->GetCRC();
			CTime writeTime(info.m_writeTime);
			if (!forceFile  &&  entry->GetFileTime() == writeTime)
			{
				info.m_needUpdate = false;
			}
			else
			{
				CFile file;
				if (file.Open(srcDir + info.m_name, CFile::modeRead))
				{
					info.m_crc = GetFileCRC(file, 0);
					file.Close();
				}
				else
				{
					ASSERT(0);
				}

				if (!forceFile  &&  info.m_crc == vfCrc)
					info.m_needUpdate = false;
			}

			// Start at the next filename.
			findPos++;
			lastFindPos = findPos;
		}
	}
	
	POSITION namePos = fileNames.GetHeadPosition();
	while (namePos)
	{
		FileInfo& info = fileNames.GetNext(namePos);

		// If this file doesn't need updating, then skip it.
		if (!info.m_needUpdate)
			continue;

		wxString curFile = info.m_name;

		// Get the file's extension.
		int dotPos = curFile.ReverseFind('.');
		wxString ext;
		if (dotPos != -1)
			ext = curFile.Mid(dotPos + 1);
		ext.MakeLower();
		wxString curFileNoExt = curFile.Left(dotPos);

		// Check the ignore extensions list.
		bool ignoreFile = false;
		POSITION pos = m_ignoreExtensions.GetHeadPosition();
		while (pos)
		{
			wxString ignoreExt = m_ignoreExtensions.GetAt(pos);
			if (ext == ignoreExt)
			{
				ignoreFile = true;
				break;
			}
			m_ignoreExtensions.GetNext(pos);
		}
		
		if (ignoreFile)
		{
			continue;
		}

		changed = true;

		if (firstTimeDir)
		{
			printf("Directory %s...\n", srcDir);
			firstTimeDir = false;
		}

		printf("  Processing %s... ", curFile);

		// Create the data file.
		CTime vfWriteTime(info.m_writeTime);

		CFile diskFile;
		if (!diskFile.Open(srcDir + curFile, CFile::modeRead))
		{
			// Error!
		}

		if (!m_compressFiles)
		{
			drive.FileCopy(diskFile, curFile, &vfWriteTime);
		}
		else
		{
			struct Locals
			{
				static void * _stdcall CompressAllocFunc(DWORD nMemSize)
				{
					return (void *)GlobalAlloc(GMEM_FIXED,nMemSize);
				}

				static bool _stdcall CompressDeallocFunc(void *pBuffer)
				{
					GlobalFree((HGLOBAL)pBuffer);
					return true;
				}
			};

			// Copy the file into the memory file.
			CMemFile memFile;
			DWORD size = diskFile.GetLength();
			BYTE* buf = WNEW BYTE[size];
			diskFile.Read(buf, size);
			diskFile.Close();

			memFile.Write(buf, size);

			delete [] buf;

			PackHeader header;
			strncpy(header.id, "VPAK", 4);
			header.packSize = 0;

			// Write out the images compressed
			header.unpackSize = memFile.GetLength();
			BYTE *pData = memFile.Detach();

			DWORD neededBufferSize = JCALG1_GetNeededBufferSize(header.unpackSize);
			BYTE* compressedBuffer = WNEW BYTE[neededBufferSize];
			if (header.unpackSize > 0)
			{
				header.packSize =
					JCALG1_Compress(pData, header.unpackSize, compressedBuffer,
						16 * 1024, &Locals::CompressAllocFunc, &Locals::CompressDeallocFunc,
						NULL);
			}

			VirtualFile vf;
			drive.FileCreate(curFile, vf, &vfWriteTime);
			if (header.packSize > 0)
			{
				vf.Write(&header, sizeof(PackHeader));
				vf.Write(compressedBuffer, header.packSize);
			}
			else
			{
				vf.Write(pData, header.unpackSize);
			}

			free(pData);
			delete [] compressedBuffer;

			printf("deflated %0.2f%%", 100.0 - ((float)header.packSize / (float)header.unpackSize) * 100.0);
		}

		// Weird.  Find name.
		if (info.m_crc == 0)
		{
			CFile file;
			if (file.Open(srcDir + curFile, CFile::modeRead))
			{
				info.m_crc = GetFileCRC(file, 0);
				file.Close();
			}
			else
			{
				ASSERT(0);
			}
		}

		VirtualDrive::FileEntry* entry = drive.GetFileEntry(curFile);
		if (entry)
			entry->SetCRC(info.m_crc);

		printf("\n");
		fflush(stdout);
	}

	// Done with files.
	fileNames.RemoveAll();
	
	// Delete data drives without files.
	if (drive.GetFileEntryCount() == 0)
	{
		drive.Close();
		_unlink(vdName);
	}
	
	// Pack the data drive.
	else if (((m_packOnChange  &&  changed)  ||  m_packAlways)  &&  doPack)
	{
		// Pack the drive.
		drive.Pack();
	}
	else
	{
		// Close the data drive.
		drive.Close();
	}

DoDirectories:
	// Do the directories.
	if (m_recurseDirectories)
	{
		POSITION pos = dirNames.GetHeadPosition();
		while (pos)
		{
			wxString newDir = dirNames.GetNext(pos);
			newDir = srcDir + newDir + '/';
			ProcessFiles(newDir, origSrcDir, origDestDir);
		}
	}
}


/**
**/
DWORD VirtualDriveCompiler::GetFileCRC(CFile& file, DWORD skipBytes)
{
	const DWORD BLOCK_SIZE = 32768;
	BYTE* buffer = WNEW BYTE[BLOCK_SIZE];
	DWORD fileCRC = 0;
	DWORD bytesToDo = file.GetLength() - skipBytes;

	// Skip the skip bytes.
	file.Seek(skipBytes, CFile::begin);

	// Parse the whole file.
	while (bytesToDo > 0)
	{
		DWORD bytesToRead = min(bytesToDo, BLOCK_SIZE);

		file.Read(buffer, bytesToRead);
		bytesToDo -= bytesToRead;
		fileCRC = crc32(fileCRC, buffer, bytesToRead);
	}

	file.SeekToBegin();

	delete [] buffer;

	return fileCRC;
}


