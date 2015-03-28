#include "../../LuaPlus/LuaPlus.h"
#ifdef WIN32
#include <windows.h>
#endif WIN32
#include <time.h>

using namespace LuaPlus;

#ifdef WIN32

struct FindFileInfo
{
	FindFileInfo() : m_handle(NULL) {}
	~FindFileInfo() {  if (m_handle)  FindClose( m_handle );  }
	HANDLE m_handle;
	WIN32_FIND_DATA m_fd;
};

#endif WIN32

static int LS_FindFirstFile( LuaState* state )
{
	LuaStack args(state);
	const char* wildcard = args[ 1 ].GetString();	wildcard;

#ifdef WIN32
	FindFileInfo* info = new FindFileInfo;
	info->m_handle = FindFirstFile(wildcard, &info->m_fd);
	if (info->m_handle == INVALID_HANDLE_VALUE)
	{
		delete info;
		return 0;
	}

	LuaObject findDataObj;
	findDataObj.AssignNewTable(state);


	state->NewUserDataBox_Stack(info);
	state->GetGlobal_Stack("FileFind");
	state->SetMetaTable(-2);

	return 1;
#else !WIN32
	return 0;
#endif WIN32
}


static int LS_Find_gc(LuaState* state)
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData())
		return 0;
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	if (info->m_handle)
		FindClose(info->m_handle);
	delete info;
	return 0;
}

	
static int LS_FindNextFile( LuaState* state )
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData())
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	BOOL ret = FindNextFile(info->m_handle, &info->m_fd);
	if (!ret)
	{
		FindClose(info->m_handle);
		info->m_handle = NULL;
		return 0;
	}

	args[1].Push();
	return 1;
#else !WIN32
	return 0;
#endif WIN32
}


static int LS_FindClose( LuaState* state )
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData() )
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	if (info->m_handle)
	{
		FindClose(info->m_handle);
		info->m_handle = NULL;
	}
#endif WIN32
	return 0;
}


static int LS_FindGetFileName( LuaState* state )
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData() )
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	state->PushString(info->m_fd.cFileName);
#endif WIN32

	return 1;
}


time_t ConvertToTime_t(const FILETIME& fileTime)
{
	FILETIME localTime;
	SYSTEMTIME sysTime;
	FileTimeToLocalFileTime(&fileTime, &localTime);
	FileTimeToSystemTime(&localTime, &sysTime);

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


static int LS_FindGetLastWriteTime(LuaState* state)
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData() )
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	state->PushInteger(ConvertToTime_t(info->m_fd.ftLastWriteTime));
	return 1;
#else !WIN32
	return 0;
#endif WIN32
}


static int LS_FindGetFileSize(LuaState* state)
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData() )
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	state->PushNumber((unsigned __int64)info->m_fd.nFileSizeLow + ((unsigned __int64)info->m_fd.nFileSizeHigh << 32));
	return 1;
#else !WIN32
	return 0;
#endif WIN32
}


static int LS_FindIsDirectory( LuaState* state )
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData() )
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	state->PushBoolean((info->m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

	return 1;
#else !WIN32
	return 0;
#endif WIN32
}


static int LS_FindIsReadOnly( LuaState* state )
{
	LuaStack args(state);
	if ( !args[ 1 ].IsUserData() )
		return 0;

#ifdef WIN32
	FindFileInfo* info = static_cast<FindFileInfo*>( args[ 1 ].GetUserData() );
	state->PushBoolean((info->m_fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0);

	return 1;
#else !WIN32
	return 0;
#endif WIN32
}


extern "C" LUAMODULE_API int luaopen_filefind(lua_State* L)
{
	LuaState* state = LuaState::CastState(L);
	LuaObject fileFindObj = state->GetGlobals().CreateTable("FileFind");
	fileFindObj.Register("First", LS_FindFirstFile);
	fileFindObj.Register("Next", LS_FindNextFile);
	fileFindObj.Register("Close", LS_FindClose);
	fileFindObj.Register("GetFileName", LS_FindGetFileName);
	fileFindObj.Register("GetLastWriteTime", LS_FindGetLastWriteTime);
	fileFindObj.Register("GetFileSize", LS_FindGetFileSize);
	fileFindObj.Register("IsDirectory", LS_FindIsDirectory);
	fileFindObj.Register("IsReadOnly", LS_FindIsReadOnly);
	fileFindObj.SetObject("__index", fileFindObj);
	fileFindObj.Register("__gc", LS_Find_gc);
	return 0;
}


