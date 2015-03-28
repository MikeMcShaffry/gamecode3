/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaDebuggerService.h
// Purpose:     wxLua Debuggee - App Debugging Service using wxSockets
// Author:      Ray Gilbert
// Modified by:
// Created:     2003-06-23
// RCS-ID:      $Id: wxLuaDebuggerService.h,v 1.4 2004/06/03 05:41:58 john Exp $
// Copyright:   (c) Ray Gilbert
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
//
// Based heavily on:
//    wxLuaDebugTarget.cpp by J. Winwood
//    wxWidgets Socket Sample

#ifndef WX_LUA_DEBUGGERSERVICE_H
#define WX_LUA_DEBUGGERSERVICE_H

#ifdef __GNUG__
    #pragma interface "wxLuaDebuggerService.h"
#endif

#include <wx/dynarray.h>
#include "wx/socket.h"

#if wxCHECK_VERSION(2,3,0)
    #include "wx/filename.h"
#else
    // FIXME: for wxWidgets 2.2.9 which does not implement wxFileName
    // This is a quick hack to test if I can compile the
    // lua wrapper for 2.2.9. I am not going to do anything
    // more for 2.2.9, but if anyone requires it, they may wish
    // fix and test the wxWidgets lua wrapper. RG
    typedef wxString wxFileName;
#endif

#include "../Library/wxLua.h"
#include "../Library/wxLuaInternals.h"
#include "../Library/wxLuaLibrary.h"
#include "../Library/wxLuaDebugServer.h"

#define SERVER_ID          1200
#define SOCKET_ID          1201
#define SOCKET_DEBUGGER_ID 1202

#define PORT_LUADEBUG      1561

class wxLuaDebugService;
class wxLuaDebuggee;


////////////////////////////////////////////////////////////////////////
// wxLuaDebugSocket
//
// Handles Debugger/Debuggee IO
class wxLuaDebugSocket
{
public:
    wxLuaDebugSocket(const wxLuaDebugSocket& debugSocket)
    {
        m_Socket = debugSocket.m_Socket;
    }

    wxLuaDebugSocket(wxSocketBase* sock)
    {
        m_Socket = sock;
    }

    wxLuaDebugSocket()
    {
        m_Socket = NULL;
    }

    ~wxLuaDebugSocket()
    {
        if (m_Socket)
        {
            m_Socket->Destroy();
            delete m_Socket;
            m_Socket = NULL;
        }
    }

    bool operator==(wxLuaDebugSocket& debugSocket)
    {
        return debugSocket.m_Socket == m_Socket;
    }

    // Client Socket
    wxSocketBase* GetSocket() const { return m_Socket; }
    void SetSocket(wxSocketBase* sock) { m_Socket = sock; }

    // Socket Error
    bool Error() { return !m_Socket || m_Socket->Error(); }

    // Is Socket Connected
    bool IsConnected() { return m_Socket && m_Socket->IsConnected(); }

    // Safely Destroy Socket
    bool Destroy()
    {
        if (m_Socket)
        {
            wxSocketBase* sock = m_Socket;
            m_Socket = NULL;
            return sock->Destroy();
        }

        return true;
    }

    // Read Data
    unsigned char  ReadByte();
    short ReadWord();
    unsigned short ReadUWord();
    int ReadInt();
    unsigned int ReadUInt();
    long ReadLong();
    unsigned long ReadULong();
    wxString ReadString();
    wxLuaDebugData *ReadDebugData();

    // Write Data
    bool WriteByte(unsigned char value);
    bool WriteWord(short value);
    bool WriteUWord(unsigned short value);
    bool WriteInt(int value);
    bool WriteUInt(unsigned int value);
    bool WriteLong(long value);
    bool WriteULong(unsigned long value);
    bool WriteString(const wxString &value);
    bool WriteDebugData(const wxLuaDebugData *pSortedList);

    bool WaitForRead();

private:
    wxSocketBase* m_Socket;
};

WX_DECLARE_LIST(wxLuaDebugSocket, wxLuaDebugSocketList);

/////////////////////////////////////////////////////////////
// Project BreakPoint
//
// BreakPoint - a filename/line pair
class LuaBreakPoint : public wxObject
{
public:
    LuaBreakPoint();
    LuaBreakPoint(const LuaBreakPoint& breakPoint) { Copy(breakPoint); }
    LuaBreakPoint(const wxFileName& fileName, int line, bool enabled = true);
    ~LuaBreakPoint() {}

    /// Assignment operator.
    void operator= (const LuaBreakPoint& data) { Copy(data); }

    // Filename
    wxFileName& GetFileName() { return m_fileName; }
    void SetFileName(wxFileName& fileName) { m_fileName = fileName; }

    // Line
    int GetLine() { return m_line; }
    void SetLine(int line) { m_line = line; }

    // Breakpoint Enabled
    int GetEnabled() { return m_enabled; }
    void SetEnabled(bool enable) { m_enabled = enable; }

    static wxString GetBreakPointKey(const wxFileName& fileName, int line)
    {
#if wxCHECK_VERSION(2,3,0)
        return wxString::Format(wxT("%u:%s"), line, fileName.GetFullPath().c_str());
#else
        return wxString::Format(wxT("%u:%s"), line, fileName.c_str());
#endif
    }

private:
    void Copy(const LuaBreakPoint& breakPoint);

    wxFileName  m_fileName;
    bool m_enabled;
    int m_line;
};

WX_DECLARE_LIST(LuaBreakPoint, LuaBreakPointList);

////////////////////////////////////////////////////////////////////////
// wxLuaDebuggee
//
// Accepts Commands from Debugger, Redirects Lua Events to Debugger
class wxLuaDebuggee : public wxObject
{
    friend class wxLuaDebugService;

public:
    wxLuaDebuggee(wxLuaDebugService * debugService, lua_State *state);
    ~wxLuaDebuggee();

    void AddBreakPoint(const wxFileName &fileName, int lineNumber);
    void RemoveBreakPoint(const wxFileName &fileName, int lineNumber);
    void DisableBreakPoint(const wxFileName &fileName, int lineNumber);
    void EnableBreakPoint(const wxFileName &fileName, int lineNumber);

    void InteruptWait();

    void NotifyBreakPoints(wxLuaDebugSocket* debugSocket = NULL);
protected:

    enum debugOperations
    {
        DEBUG_STEP,
        DEBUG_STEPOVER,
        DEBUG_STEPOUT,
        DEBUG_GO
    };

    wxLuaDebugService* m_debugService;
    lua_State          *m_luaState;

    LuaBreakPointList   m_breakPointList;

    debugOperations     m_nextOperation;
    bool                m_forceBreak;
    bool                m_resetRequested;
    bool                m_fErrorsSeen;
    int                 m_nFramesUntilBreak;
    wxArrayInt          m_references;
    bool                m_InDebugHook;
    bool                m_Wait;

    mutable wxCriticalSection m_breakPointListCriticalSection;

    // Do Debug Commands
    void ClearAllBreakPoints();
    void Step();
    void StepOver();
    void StepOut();
    void Continue();
    void Break();
    void Reset();
    bool EnumerateStack();
    bool EnumerateStackEntry(int stackRef);
    bool EnumerateTable(int tableRef, int nIndex, int itemNode);
    bool EvaluateExpr(int exprRef, const wxString &strExpr);
    void ClearReferences();

    // Some Outputs
    void DisplayError(const wxString &strError);
    void NotifyPrint(const wxString &strMsg);

    // Debug Events from LuaDebugHook
    bool DebugHook(int event);

private:
    bool AtBreakPoint(const wxFileName &fileName, int lineNumber) const;
    wxString GetTableInfo(int index);
    wxString GetUserDataInfo(int index, bool full = true);
    wxString MakeBoolean(int num);
    wxString MakeNumber(double dnum);

    wxFileName GetDebugInfo(int &lineNumber) const;


    // Debug Events from Lua Interpreter
    static void LUACALL LuaDebugHook(lua_State *state, lua_Debug *debug);
    static int  LUACALL LuaPrint (lua_State *L);
};


WX_DECLARE_LIST(wxLuaDebuggee, wxLuaDebuggeeList);

////////////////////////////////////////////////////////////////////////
// wxLuaDebugService
//
// App Debugging Service
class wxLuaDebugService : public wxEvtHandler
{
public:
    wxLuaDebugService(int portNumber = PORT_LUADEBUG);
    ~wxLuaDebugService();

    // Start Service
    bool StartService();

    // Debug Lua Interpreter
    void DebugLua(lua_State* state);

    bool IsDebugger();

    // Send Data to Debugger
    bool NotifyBreak(const wxFileName &fileName, int lineNumber);
    bool NotifyPrint(const wxString &errorMsg);
    bool NotifyError(const wxString &errorMsg);
    bool NotifyExit();
    bool NotifyStackEnumeration(wxLuaDebugData *pDebugData);
    bool NotifyStackEntryEnumeration(int stackRef, wxLuaDebugData *pDebugData);
    bool NotifyTableEnumeration(int itemNode, wxLuaDebugData *pDebugData);
    bool NotifyEvaluateExpr(int exprRef, const wxString &strResult);
    bool NotifyBreakpointAdded(LuaBreakPoint& breakPoint, wxLuaDebugSocket* debugSocket = NULL);
    bool NotifyBreakpointRemoved(LuaBreakPoint& breakPoint, wxLuaDebugSocket* debugSocket = NULL);

    wxLuaDebuggee* GetActiveDebuggee();

    bool WaitForRead();

protected:
    void OnServerEvent(wxSocketEvent& event);
    void OnSocketEvent(wxSocketEvent& event);
    void OnSocketLostEvent(wxSocketEvent& event);

    wxLuaDebugSocket* GetDebugSocket(wxSocketBase* sock);

private:
    wxSocketServer* m_serverSocket;
    int m_portNumber;

    // Debuggers connected to Service
    mutable wxCriticalSection m_debuggerSocketListCriticalSection;
    wxLuaDebugSocketList m_debuggerSocketList;

    // Lua Interpreters Hooked by Service
    mutable wxCriticalSection m_debuggeeListCriticalSection;
    wxLuaDebuggeeList m_debuggeeList;

    mutable wxCriticalSection m_notifyBreakCriticalSection;

    DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////
// wxLuaDebugger
//
// Lua Debugger Client
// (Client Side of Lua Debug Service)
class wxLuaDebugger : public wxLuaDebuggerBase
{
public:
    wxLuaDebugger();
    ~wxLuaDebugger();

    bool Connect(const wxString& hostname = wxT("localhost"), int portNumber = PORT_LUADEBUG);

    // Debugger Commands
    bool AddBreakPoint(const wxString &fileName, int lineNumber);
    bool RemoveBreakPoint(const wxString &fileName, int lineNumber);
    bool DisableBreakPoint(const wxString &fileName, int lineNumber);
    bool EnableBreakPoint(const wxString &fileName, int lineNumber);
    bool ClearAllBreakPoints();
    bool Step();
    bool StepOver();
    bool StepOut();
    bool Continue();
    bool Break();
    bool EnumerateStack();
    bool Reset();
    bool EnumerateStackEntry(int stackEntry);
    bool EnumerateTable(int tableRef, int nIndex, int nItemNode);
    bool CleanupDebugReferences();

protected:
    void OnSocketEvent(wxSocketEvent& event);
    void NotifyError(const wxString &msg);

    // Not Required for Debug Service
    virtual bool Run(const wxString &WXUNUSED(fileName), 
                     const wxString &WXUNUSED(buffer)) { return false; }

private:
    mutable wxCriticalSection m_debuggerSocketCriticalSection;
    wxLuaDebugSocket* m_debuggerSocket;
    int m_portNumber;

    DECLARE_EVENT_TABLE()
};

#endif // WX_LUA_DEBUGGERSERVICE_H
