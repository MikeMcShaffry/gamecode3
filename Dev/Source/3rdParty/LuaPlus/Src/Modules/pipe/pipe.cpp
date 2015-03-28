#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "LuaPlus/LuaPlus.h"
extern "C" {
#include "LuaPlus/src/lua.h"
}

using namespace LuaPlus;

#define FILEHANDLE		"FILE*"

// From some MSDN sample, I think.
static int CreatePipeChild(HANDLE& child, HANDLE* inH, HANDLE* outH, HANDLE* errH, LPCTSTR Command)
{
    static int PCount = 0;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);                        // Security descriptor for INHERIT.
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle       = 0;

    SECURITY_ATTRIBUTES lsa;     
    HANDLE ChildIn;
	HANDLE ChildOut;
	HANDLE ChildErr;
	lsa.nLength=sizeof(SECURITY_ATTRIBUTES);
	lsa.lpSecurityDescriptor=NULL;
	lsa.bInheritHandle=TRUE;
	
	// Create Parent_Write to ChildStdIn Pipe
	if (!CreatePipe(&ChildIn,inH,&lsa,0))
	{
		// Error.
	}

	// Create ChildStdOut to Parent_Read pipe
	if (!CreatePipe(outH,&ChildOut,&lsa,0))
	{
		// Error.
	}

	// Create ChildStdOut to Parent_Read pipe
	if (!CreatePipe(errH,&ChildErr,&lsa,0))
	{
		// Error.
	}

	// Lets Redirect Console StdHandles - easy enough
    PROCESS_INFORMATION pi;
    STARTUPINFO             si;
    HANDLE hNul;

	// Dup the child handle to get separate handles for stdout and err,
    hNul = CreateFile("NUL",
                      GENERIC_READ | GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL, OPEN_EXISTING,
                      0,
                      NULL);

    if (hNul != NULL)
	{
        // Set up members of STARTUPINFO structure.
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
        si.hStdOutput = ChildOut;
        si.hStdError    = ChildErr;
        si.hStdInput    = ChildIn;
        if (CreateProcess(NULL, (char*)Command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == TRUE)
        {
            CloseHandle(pi.hThread);        // Thread handle not needed
            //fprintf(stderr, "create process success\n");
            child = pi.hProcess;  // Return process handle (to get RC)
        } else
			return -1;
        CloseHandle(hNul);                                      // Close error handle,
		CloseHandle(ChildOut);
		CloseHandle(ChildErr);
		CloseHandle(ChildIn);
    }
    else
	{
		// Error.
	}

    return 0;
}


static LuaStackObject newfile (LuaState* state, FILE *f) {
  FILE **pf = (FILE **)lua_newuserdata(state->GetCState(), sizeof(FILE *));
  *pf = f;
  LuaStackObject obj = state->StackTop();
  luaL_getmetatable(state->GetCState(), FILEHANDLE);
  lua_setmetatable(*state, obj);
  return obj;
}


static int LS_popen( LuaState* state )
{
	LuaStack args(state);
	if (!args[1].IsString())
	{
		state->PushNil();
		return 1;
	}

	LPCSTR str = args[1].GetString();

	char cmd[1024];
	strcpy(cmd, "cmd /C ");
	strcat(cmd, str);

	HANDLE child;
	HANDLE hIn, hOut, hErr;
	int rc = CreatePipeChild(child, &hIn, &hOut, &hErr, cmd);
	if (rc == -1)
	{
		state->PushNil();
		return 1;
	}
	
	state->CreateTable();
    LuaStackObject outTableObj = state->StackTop();
	int in = _open_osfhandle((long)hIn, _O_WRONLY | _O_TEXT);
	FILE* fIn = fdopen(in, "wt");
	int err = _open_osfhandle((long)hErr, _O_RDONLY | _O_TEXT);
	FILE* fErr = fdopen(err, "rt");
	int out = _open_osfhandle((long)hOut, _O_RDONLY | _O_TEXT);
	FILE* fOut = fdopen(out, "rt");
	
	setvbuf(fIn, NULL, _IONBF, 0);
	setvbuf(fOut, NULL, _IONBF, 0);
	setvbuf(fErr, NULL, _IONBF, 0);

	{
		LuaAutoBlock autoBlock(state);
		LuaStackObject inObj = newfile(state, fIn);
		outTableObj.SetObject("stdin", inObj);
		LuaStackObject outObj = newfile(state, fOut);
		outTableObj.SetObject("stdout", outObj);
		LuaStackObject errObj = newfile(state, fErr);
		outTableObj.SetObject("stderr", errObj);
	}

	CloseHandle(child);

	return 1;
}


static int LS_pclose( LuaState* state )
{
	LuaStack args(state);
	LuaStackObject pipeObj = args[1];
	if (!pipeObj.IsTable())
		return 0;

	// Close down the pipe.
	/*
	state->PushString("close");
	LuaStackObject closeObj(state, state->GetTop());
	
	closeObj.Push();
	LuaStackObject inObj = pipeObj["stdin"];
	*/

	state->PushBoolean(true);
	return 1;
}


extern "C" LUAMODULE_API int luaopen_pipe(lua_State *L)
{
	LuaState* state = LuaState::CastState(L);
	LuaObject pipeObj = state->GetGlobals().CreateTable( "pipe" );
	pipeObj.Register( "popen", LS_popen );
	pipeObj.Register( "pclose", LS_pclose );

	state->DoString(
"function pipe.lines(file)\n"
"	if type( file ) == 'string' then\n"
"		file = pipe.popen( file )\n"
"	end\n"
"	return function ()\n"
"		return file.stdout:read() or (assert(pipe:pclose(file)) and nil), i\n"
"	end\n"
"end\n"
	);

	return 0;
}


