#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#include "LuaPlus/LuaPlus.h"
extern "C" {
#include "LuaPlus/src/lua.h"
}
using namespace LuaPlus;

#include "Misc/DiskFile.h"
#include "Misc/VirtualDrive.h"
#include "Misc/VirtualFile.h"
#include <shlwapi.h>
#include <malloc.h>

#include <time.h>

using namespace Misc;

/*
	VDriveMetaTable =
	{
		NO_COMPRESSION = 0,
		BEST_SPEED = 1,
		BEST_COMPRESSION = 9,

		function Create(fileName, defaultPassword) : VirtualDrive
		function Open(fileName, defaultPassword) : VirtualDrive
		function Close()
		function Flush()
		function FileCreate(fileName, compressionLevel, fileTime) : VirtualFileHandle
		function FileOpen(fileName) : VirtualFileHandle
		function FileOpen(index) : VirtualFileHandle
		function FileCloseAll()
		function FileErase(fileName)
		function FileRename(oldName, newName)
		function FileInsert(srcFileName, destFileName, fileTime)
		function FileExtract(srcFileName, destFileName)

		function GetNeedPack()
		function SetNeedPack(needPack)

		function Pack(FileNameOrderTable)

		function GetFileName()
		function GetFileEntryCount()
		function GetFileEntry(entry)
		function ForceChange()
	}


	VFileMetaTable =
	{
		function GetFileName()
		function GetPosition()
		function SetLength(newLength)
		function GetLength()
		function Read(buffer, count)
		function Write(buffer, count)
		function Close()
	}
*/

static VirtualDrive* VirtualDriveFromLua(LuaState* state, LuaObject obj)
{
	LuaObject tagObj = state->GetRegistry()["VDriveMetaTable"];
	if (obj.IsUserData()  &&  state->Equal(obj.GetMetaTable(), tagObj))
	{
		return (VirtualDrive*)obj.GetUserData();
	}
	else
	{
		luaL_argcheck(*state, NULL, 1, "Object type is wrong");
		return NULL;
	}
}


static VirtualFileHandle* VirtualFileHandleHandleFromLua(LuaState* state, LuaObject obj)
{
	LuaObject tagObj = state->GetRegistry()["VDriveFileHandleMetaTable"];
	if (obj.IsUserData()  &&  state->Equal(obj.GetMetaTable(), tagObj))
	{
        return (VirtualFileHandle*)obj.GetUserData();
	}
	else
	{
		luaL_argcheck(*state, NULL, 1, "Object type is wrong");
		return NULL;
	}
}


static int VirtualDrive_gc(LuaState* state, LuaStackObject* args)
{
	if ( !args[1].IsUserData() )
		assert( 0 );		// How did we get here?
	VirtualDrive* drive = (VirtualDrive*)args[1].GetUserData();
	delete drive;

	return 0;
}


static int VirtualFileHandle_gc(LuaState* state, LuaStackObject* args)
{
    VirtualFileHandle* fileHandle = (VirtualFileHandle*)args[1].GetUserData();
	delete fileHandle;
	return 0;
}


VirtualFileHandle* PushVirtualFileHandleObj(LuaState* state)
{
	LuaObject metaTableObj = state->GetRegistry()["VDriveFileHandleMetaTable"];

	VirtualFileHandle* fileHandle = new VirtualFileHandle;
	LuaObject outObj = state->NewUserDataBox(fileHandle);
	outObj.SetMetaTable(metaTableObj);

	outObj.Push();
	return fileHandle;
}



int LS_VirtualDrive_Create(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* fileName = luaL_checkstring(*state, 2);
	const char* defaultPassword = luaL_optstring(*state, 3, NULL);
	state->PushBoolean(drive->Create(fileName, defaultPassword) != 0);
	return 1;
}


int LS_VirtualDrive_Open(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* fileName = luaL_checkstring(*state, 2);
	const char* defaultPassword = luaL_optstring(*state, 3, NULL);
	state->PushBoolean(drive->Open(fileName, false, defaultPassword) != 0);
	return 1;
}


int LS_VirtualDrive_Close(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	drive->Close();
	return 0;
}


int LS_VirtualDrive_Flush(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	drive->Flush();
	return 0;
}


int LS_VirtualDrive_FileCreate(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* fileName = luaL_checkstring(*state, 2);
	UINT compressionLevel = (UINT)luaL_optnumber(*state, 3, 8);
	time_t fileTime = (UINT)luaL_optnumber(*state, 4, (lua_Number)time(NULL));
    VirtualFileHandle* fileHandle = PushVirtualFileHandleObj(state);
	if (!drive->FileCreate(fileName, *fileHandle, compressionLevel, &fileTime))
		return 0;
	return 1;
}


int LS_VirtualDrive_FileOpen(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* fileName = NULL;
	int entryIndex = -1;
	if (args[2].IsString())
		fileName = args[2].GetString();
	else if (args[2].IsNumber())
		entryIndex = args[2].GetInteger();
	else
		luaL_error(*state, "Argument 2 is not a string or integer.");

	VirtualFileHandle* file = PushVirtualFileHandleObj(state);
	if (fileName)
	{
		if (!drive->FileOpen(fileName, *file))
			return 0;
	}
	else
	{
		if (!drive->FileOpenIndex((size_t)entryIndex, *file))
			return 0;
	}

	return 1;
}


int LS_VirtualDrive_FileClose(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* fileHandle = VirtualFileHandleHandleFromLua(state, args[2]);
	state->PushBoolean(fileHandle->GetParentDrive()->FileClose(*fileHandle) != 0);
	return 1;
}


int LS_VirtualDrive_FileCloseAll(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	drive->FileCloseAll();
	return 0;
}


int LS_VirtualDrive_FileGetFileName(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* fileHandle = VirtualFileHandleHandleFromLua(state, args[2]);
	state->PushString(drive->FileGetFileName(*fileHandle));
	return 1;
}


int LS_VirtualDrive_FileGetPosition(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* fileHandle = VirtualFileHandleHandleFromLua(state, args[2]);
	state->PushNumber((lua_Number)drive->FileGetPosition(*fileHandle));
	return 1;
}


int LS_VirtualDrive_FileSetLength(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* fileHandle = VirtualFileHandleHandleFromLua(state, args[2]);
	long len = luaL_checklong(*state, 3);
	drive->FileSetLength(*fileHandle, len);
	return 0;
}


int LS_VirtualDrive_FileGetLength(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* fileHandle = VirtualFileHandleHandleFromLua(state, args[2]);
	state->PushNumber((lua_Number)drive->FileGetLength(*fileHandle));
	return 1;
}


int LS_VirtualDrive_FileRead(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* fileHandle = VirtualFileHandleHandleFromLua(state, args[2]);
	long len = luaL_optlong(*state, 3, -1);
	if (len == 0)
		len = (long)drive->FileGetLength(*fileHandle);

	char* buffer = new char[len];
	UINT nr = (UINT)drive->FileRead(*fileHandle, buffer, len);
	state->PushLString(buffer, nr);
	delete[] buffer;
	return 1;
}


int LS_VirtualDrive_FileWrite(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	VirtualFileHandle* file = VirtualFileHandleHandleFromLua(state, args[2]);
	size_t len;
    const char* buffer = luaL_checklstring(*state, 3, &len);
	len = luaL_optlong(*state, 4, len);
	drive->FileWrite(*file, buffer, len);
	return 0;
}


int LS_VirtualDrive_FileErase(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* fileName = luaL_checkstring(*state, 2);
	state->PushBoolean(drive->FileErase(fileName) != 0);
	return 1;
}


int LS_VirtualDrive_FileRename(LuaState* state)
{
    LuaStack args(state);
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* srcFileName = luaL_checkstring(*state, 2);
	const char* destFileName = luaL_checkstring(*state, 3);
	state->PushBoolean(drive->FileRename(srcFileName, destFileName) != 0);
	return 1;
}


int LS_VirtualDrive_FileInsert(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* srcFileName = luaL_checkstring(*state, 2);
	const char* destFileName = luaL_checkstring(*state, 3);
    int compressionMethod = (UINT)luaL_optnumber(*state, 4, VirtualDrive::COMPRESSED);

	DiskFile file;
    if (!file.Open(srcFileName, File::MODE_READONLY))
	{
		state->PushBoolean(false);
		return 1;
	}

	time_t fileTime = (UINT)luaL_optnumber(*state, 5, (lua_Number)file.GetLastWriteTime());

	state->PushBoolean(drive->FileCopy(file, destFileName, compressionMethod, &fileTime) != 0);
	return 1;
}


bool Copy(File& srcFile, const char* destFilename)
{
	// Operate in 16k buffers.
	const DWORD BUFFER_SIZE = 16 * 1024;

	DWORD fileSize = (DWORD)srcFile.GetLength();

	// See if the destination file exists.
	DiskFile destFile;
	if (!destFile.Open(destFilename, File::MODE_CREATE | File::MODE_WRITEONLY))
		return false;

	// Allocate the buffer space.
	BYTE* buffer = (BYTE*)_alloca(BUFFER_SIZE);

	// Keep copying until there is no more file left to copy.
	while (fileSize > 0)
	{
		// Copy the minimum of BUFFER_SIZE or the fileSize.
		DWORD readSize = min(BUFFER_SIZE, fileSize);
		srcFile.Read(buffer, readSize);
		destFile.Write(buffer, readSize);
		fileSize -= readSize;
	}

	// Close the destination virtual file.
	destFile.Close();

	return true;
}


int LS_VirtualDrive_FileExtract(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	const char* srcFileName = luaL_checkstring(*state, 2);
	const char* destFileName = luaL_checkstring(*state, 3);
	VirtualDrive::FileEntry* entry = drive->FindFileEntry(srcFileName);
	if (!entry)
	{
		state->PushBoolean(false);
		return 1;
	}

	VirtualFile file;
    if (!file.Open(*drive, srcFileName))
	{
		state->PushBoolean(false);
		return 1;
	}

	bool ret = Copy(file, destFileName);

/*	FILETIME createTime;
	FILETIME accessTime;
	FILETIME writeTime;
	HANDLE handle = CreateFile(destFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileTime(handle, &createTime, &accessTime, &writeTime);
	writeTime.dwLowDateTime = args[ 2 ].GetInteger();
	writeTime.dwHighDateTime = args[ 3 ].GetInteger();
	SetFileTime(handle, &createTime, &accessTime, &writeTime);
	CloseHandle(handle);
*/
	state->PushBoolean(ret);
	return 1;
}


int LS_VirtualDrive_Pack(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
    VirtualDrive::FileOrderInfoArray fileOrder;
	if (args[2].IsTable())
	{
        VirtualDrive::FileOrderInfo info;
		LuaObject tableObj = args[2];
        int index = 1;
		while (true)
		{
			LuaObject entryNameObj = tableObj[index++];
			if (!entryNameObj.IsString())
				break;
			info.entryName = entryNameObj.GetString();

			LuaObject srcPathObj = tableObj[index++];
			if (srcPathObj.IsNil())
				break;
			info.srcPath = srcPathObj.GetString();

			info.compressionMethod = 8;
			LuaObject compressionMethodObj = tableObj[index];
			if (compressionMethodObj.IsNumber())
			{
				info.compressionMethod = compressionMethodObj.GetInteger();
				index++;
			}

            fileOrder.Add(info);
		}
	}
	state->PushBoolean(drive->Pack(&fileOrder) != 0);
	return 1;
}


int LS_VirtualDrive_ProcessFileList(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	if (args[2].IsTable())
	{
        VirtualDrive::FileOrderInfoArray fileOrder;
        VirtualDrive::FileOrderInfo info;
		LuaObject tableObj = args[2];
        int index = 1;
		while (true)
		{
			LuaObject entryNameObj = tableObj[index++];
			if (!entryNameObj.IsString())
				break;
			info.entryName = entryNameObj.GetString();

			LuaObject srcPathObj = tableObj[index++];
			if (srcPathObj.IsNil())
				break;
			info.srcPath = srcPathObj.GetString();

			info.compressionMethod = 8;
			LuaObject compressionMethodObj = tableObj[index];
			if (compressionMethodObj.IsNumber())
			{
				info.compressionMethod = compressionMethodObj.GetInteger();
				index++;
			}

            fileOrder.Add(info);
		}
    	bool ret = drive->ProcessFileList(fileOrder);
		state->PushBoolean(ret);
		if (!ret)
		{
			state->PushString(drive->errorString);
			return 2;
		}
        return 1;
	}

    if (args[2].IsString())
    {
        state->PushBoolean(drive->ProcessFileList(args[2].GetString()) != 0);
	    return 1;
    }

    return 0;
}


int LS_VirtualDrive_GetFileName(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	state->PushString(drive->GetFileName());
	return 1;
}


int LS_VirtualDrive_GetFileEntryCount(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);
	state->PushNumber(drive->GetFileEntryCount());
	return 1;
}


static void BuildFileEntry(LuaState* state, VirtualDrive::FileEntry* entry)
{
	LuaObject entryObj;
	entryObj.AssignNewTable(state);
	entryObj.SetString("FileName", entry->GetFileName());
	entryObj.SetNumber("TimeStamp", (lua_Number)entry->GetTimeStamp());
	entryObj.SetNumber("CRC", entry->GetCRC());
	entryObj.SetNumber("Offset", entry->GetOffset());
	entryObj.SetNumber("UncompressedSize", entry->GetUncompressedSize());
	entryObj.SetNumber("CompressedSize", entry->GetCompressedSize());
	entryObj.SetNumber("CompressionMethod", entry->GetCompressionMethod());
	entryObj.Push();
}


int LS_VirtualDrive_GetFileEntry(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);

	int entryIndex = (int)luaL_optlong(*state, 2, -1);

	VirtualDrive::FileEntry* entry;

	entry = drive->GetFileEntry(entryIndex - 1);

	if (!entry)
		return 0;

	BuildFileEntry(state, entry);
	return 1;
}


int LS_VirtualDrive_FindFileEntry(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);

	const char* fileName = luaL_checkstring(*state, 2);

	VirtualDrive::FileEntry* entry = drive->FindFileEntry(fileName);

	if (!entry)
		return 0;

	BuildFileEntry(state, entry);
	return 1;
}


int LS_VirtualDrive_GetFileEntryIndex(LuaState* state, LuaStackObject* args)
{
	VirtualDrive* drive = VirtualDriveFromLua(state, args[1]);

	const char* fileName = luaL_checkstring(*state, 2);
	state->PushNumber(drive->FindFileEntryIndex(fileName));
	return 1;
}


int LS_VirtualDrive(LuaState* state, LuaStackObject* args)
{
	LuaObject metaTableObj = state->GetRegistry()["VDriveMetaTable"];
	VirtualDrive* drive = new VirtualDrive;
	LuaObject outObj = state->NewUserDataBox(drive);
	outObj.SetMetaTable(metaTableObj);

	outObj.Push();
	return 1;
}


int LS_Help(LuaState* state, LuaStackObject* args)
{
	char help[] =
"drive = vdrive.VirtualDrive() - Create virtual drive\n"
"time = vdrive.AdjustTime_t(timeToAdjust) - Adjusts a timestamp to zip format\n"
"\n"
"Drive commands:\n"
"\n"
"	opened = drive:Create(fileName [, defaultPassword])\n"
"	opened = drive:Open(fileName [, defaultPassword])\n"
"	drive:Close()\n"
"	drive:Flush()\n"
"\n"
"	vfile = drive:FileCreate(fileName, compressionMethod:0,8{compressed}, time_t fileTime)\n"
"	vfile = drive:FileOpen(fileName)\n"
"	drive:FileCloseAll()\n"
"	bool ret = drive:FileErase(fileName)\n"
"	bool ret = drive:FileRename(oldFileName, newFileName)\n"
"	bool ret = drive:FileInsert(srcFileName, destVirtualFileHandleName)\n"
"	bool ret = drive:FileExtract(srcVirtualFileHandleName, destFileName)\n"
"\n"
"	drive:Pack(fileNameTable)\n"
"   drive:ProcessFileList(fileNameTable|fileListFileName)\n"
"\n"
"	string fileName = drive:GetFileName()\n"
"	int entryCount = drive:GetFileEntryCount()\n"
"	FileEntry fileEntry = drive:GetFileEntry(index)\n"
"	FileEntry fileEntry = drive:FindFileEntry(fileName)\n"
"		FileEntry =\n"
"		{\n"
"			string FileName\n"
"			time_t TimeStamp\n"
"			DWORD CRC\n"
"			DWORD Offset\n"
"			DWORD Size\n"
"			DWORD UncompressedSize\n"
"			DWORD CompressedSize\n"
"           DWORD CompressionMethod\n"
"		}\n"
"	int entryIndex = drive:GetFileEntryIndex(fileName)\n"
"\n"
"File commands:\n"
"\n"
"	string fileName = drive:FileGetFileName(vfile)\n"
"	long position = drive:FileGetPosition(vfile)\n"
"	drive:FileSetLength(vfile, newLength)\n"
"	long len = drive:FileGetLength(vfile)\n"
"	buffer = drive:FileRead(vfile, optionalSize)  If 0, the whole file reads in.\n"
"	drive:FileWrite(vfile, buffer, optionalSize)  If 0, the whole buffer writes out.\n"
"	drive:Close(vfile)\n";

	state->PushString(help);
	return 1;
}


int LS_crc32(LuaState* state)
{
	LuaStack args(state);
	if (args[1].IsString())
	{
		state->PushNumber(crc32(0, (BYTE*)args[1].GetString(), args[1].StrLen()));
		return 1;
	}

	if (args[1].IsNumber()  &&  args[2].IsString())
	{
		state->PushNumber(crc32(args[1].GetInteger(), (BYTE*)args[2].GetString(), args[2].StrLen()));
		return 1;
	}

	return 0;
}


int LS_AdjustTime_t(LuaState* state)
{
	LuaStack args(state);
	state->PushNumber(VirtualDrive::AdjustTime_t(args[1].GetInteger()));
	return 1;
}


extern "C" LUAMODULE_API int luaopen_vdrive(lua_State* L)
{
	LuaState* state = LuaState::CastState(L);

	LuaObject metaTableObj = state->GetRegistry().CreateTable("VDriveFileHandleMetaTable");
	metaTableObj.Register("__gc",			VirtualFileHandle_gc);

	metaTableObj = state->GetRegistry().CreateTable("VDriveMetaTable");
	metaTableObj.Register("Create",				LS_VirtualDrive_Create);
	metaTableObj.Register("Open",				LS_VirtualDrive_Open);
	metaTableObj.Register("Close",				LS_VirtualDrive_Close);
	metaTableObj.Register("Flush",				LS_VirtualDrive_Flush);
	metaTableObj.Register("FileCreate",			LS_VirtualDrive_FileCreate);
	metaTableObj.Register("FileOpen",			LS_VirtualDrive_FileOpen);
	metaTableObj.Register("FileClose",			LS_VirtualDrive_FileClose);
	metaTableObj.Register("FileCloseAll",		LS_VirtualDrive_FileCloseAll);
	metaTableObj.Register("FileGetFileName",    LS_VirtualDrive_FileGetFileName);
	metaTableObj.Register("FileGetPosition",    LS_VirtualDrive_FileGetPosition);
	metaTableObj.Register("FileSetLength",      LS_VirtualDrive_FileSetLength);
	metaTableObj.Register("FileGetLength",      LS_VirtualDrive_FileGetLength);
	metaTableObj.Register("FileRead",           LS_VirtualDrive_FileRead);
	metaTableObj.Register("FileWrite",          LS_VirtualDrive_FileWrite);
    metaTableObj.Register("FileErase",			LS_VirtualDrive_FileErase);
	metaTableObj.Register("FileRename",			LS_VirtualDrive_FileRename);
	metaTableObj.Register("FileInsert",			LS_VirtualDrive_FileInsert);
	metaTableObj.Register("FileExtract",		LS_VirtualDrive_FileExtract);
	metaTableObj.Register("Pack",				LS_VirtualDrive_Pack);
	metaTableObj.Register("ProcessFileList",	LS_VirtualDrive_ProcessFileList);
	metaTableObj.Register("GetFileName",		LS_VirtualDrive_GetFileName);
	metaTableObj.Register("GetFileEntryCount",	LS_VirtualDrive_GetFileEntryCount);
	metaTableObj.Register("GetFileEntry",		LS_VirtualDrive_GetFileEntry);
	metaTableObj.Register("FindFileEntry",		LS_VirtualDrive_FindFileEntry);
	metaTableObj.Register("GetFileEntryIndex",	LS_VirtualDrive_GetFileEntryIndex);
	metaTableObj.Register("__gc",				VirtualDrive_gc);
	metaTableObj.SetObject("__index",			metaTableObj);

	LuaObject obj = state->GetGlobals().CreateTable( "vdrive" );
	obj.Register("VirtualDrive", LS_VirtualDrive);
	obj.Register("AdjustTime_t", LS_AdjustTime_t);
	obj.Register("help", LS_Help);
	obj.Register("crc32", LS_crc32);
	return 0;
}


