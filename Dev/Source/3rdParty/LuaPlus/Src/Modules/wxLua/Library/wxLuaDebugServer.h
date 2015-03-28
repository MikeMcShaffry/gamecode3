/////////////////////////////////////////////////////////////////////////////
// Purpose:     Implements the debugger end of wxLua debugging session
// Author:      J. Winwood
// Created:     May 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_DEBUG_SERVER_H
#define WX_LUA_DEBUG_SERVER_H

#ifdef __GNUG__
    #pragma interface "wxLuaDebugServer.h"
#endif

#include "wx/process.h"
#include "wxLuaSocket.h"
#include "wxLuaDebug.h"
#include "wxLuaDebugIO.h"

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

// ----------------------------------------------------------------------------
// Forward references
class wxLuaDebugServer;
class wxLuaDebugEvent;
class wxLuaStackFrame;

extern wxString g_strProgramName;
extern wxString g_strNetworkName;

// ----------------------------------------------------------------------------
// wxLuaThread
// ----------------------------------------------------------------------------

class wxLuaThread : public wxThread
{
public:
    wxLuaThread(wxLuaDebugServer *pServer) :
        wxThread(wxTHREAD_JOINABLE)
    {
        m_pServer = pServer;
    }

protected:
    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits
    virtual void OnExit();

private:
    wxLuaDebugServer *m_pServer;
};

// ----------------------------------------------------------------------------
// wxLuaDebugServer
// ----------------------------------------------------------------------------

class wxLuaDebugServer : public wxLuaDebuggerBase, public wxLuaDebugIO
{
public:

    wxLuaDebugServer(int portNumber);
    virtual ~wxLuaDebugServer();

    bool StartServer();
    bool StopServer();
    bool StartClient();
    bool StartServerThread();
    bool WaitForConnect(int timeOut);

    bool AddBreakPoint(const wxString &fileName, int lineNumber);
    bool RemoveBreakPoint(const wxString &fileName, int lineNumber);
    bool ClearAllBreakPoints();
    bool Run(const wxString &fileName, const wxString &buffer);
    bool Step();
    bool StepOver();
    bool StepOut();
    bool Continue();
    bool Break();
    bool Reset();
    bool EnumerateStack();
    bool EnumerateStackEntry(int stackEntry);
    bool EnumerateTable(int tableRef, int nIndex, int nItemNode);
    bool CleanupDebugReferences();
    void ThreadFunction();
#if wxCHECK_VERSION(2, 3, 0)
    bool DisplayStackDialog(wxWindow *pParent);
#endif
    bool EvaluateExpr(int exprRef, const wxString &pExpression);

    static bool Compile(const wxString &fileName, const wxString &buffer);

protected:
    wxLuaSocket    *m_serverSocket;
    wxLuaSocket    *m_acceptedSocket;
    int             m_processID;
    int             m_portNumber;
    wxProcess      *m_pProcess;
    wxLuaThread    *m_pThread;
    wxLuaStackFrame  *m_pStackFrameDialog;
    bool            m_fShutdown;

private:
    static void NotifyError(const wxString &msg);
    static int LuaCompileError (lua_State *L);
    DECLARE_CLASS(wxLuaDebugServer)
};

#endif // WX_LUA_DEBUG_SERVER_H
