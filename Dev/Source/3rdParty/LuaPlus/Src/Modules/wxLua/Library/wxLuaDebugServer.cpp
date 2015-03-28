/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaDebugServer.cpp
// Purpose:     Provide remote debugging support for wxLua.
// Author:      J. Winwood
// Created:     May 2002.
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaDebugServer.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/thread.h"
#include "wxLuaInternals.h"
#include "wxLuaLibrary.h"
#include "wxLuaDebugServer.h"

#if wxCHECK_VERSION(2, 3, 0)
#include "wxLuaStackTree.h"
#endif

wxString g_strProgramName;
wxString g_strNetworkName;

// ----------------------------------------------------------------------------
// wxLuaDebugServer
// ----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxLuaDebugServer, wxLuaDebuggerBase)


wxLuaDebugServer::wxLuaDebugServer(int portNumber) :
    m_serverSocket(NULL),
    m_acceptedSocket(NULL),
    m_processID(-1),
    m_portNumber(portNumber),
    m_pProcess(NULL),
    m_pThread(NULL),
    m_pStackFrameDialog(NULL),
    m_fShutdown(false)
{
    StartServer();
}

bool wxLuaDebugServer::StartServer()
{
    m_serverSocket = new wxLuaSocket;
    if (m_serverSocket != NULL)
    {
        try
        {
            // listen on the specified port
            m_serverSocket->Listen(m_portNumber);
        }
        catch(wxLuaSocketException & e)
        {

            NotifyError(wxString::Format(_("Error while opening listening socket. %s"), e.description().c_str()));
            m_fShutdown = true;
        }
    }

    return (m_serverSocket != NULL);
}

bool wxLuaDebugServer::StartClient()
{
    if ((m_pProcess == NULL) && StartServerThread())
    {
        m_pProcess = new wxProcess();
        wxString command = wxString::Format(wxT("%s -d%s:%u"),
                                            g_strProgramName.c_str(),
#ifdef  __WXMSW__
                                            g_strNetworkName.c_str(),
#else
                                            wxT("localhost"),
#endif
                                            m_portNumber);
        m_processID = wxExecute(command, FALSE, m_pProcess);
        return (m_processID != -1);
    }
    return false;
}

bool wxLuaDebugServer::StartServerThread()
{
    if (!m_fShutdown)
    {
        m_pThread = new wxLuaThread(this);

        return ((m_pThread != NULL)                        &&
                (m_pThread->Create() == wxTHREAD_NO_ERROR) &&
                (m_pThread->Run()    == wxTHREAD_NO_ERROR));
    }
    return false;
}

bool wxLuaDebugServer::StopServer()
{
    // Set the shutdown flag
    m_fShutdown = true;

    // close the session socket
    if (m_acceptedSocket != NULL)
    {
        try
        {
            m_acceptedSocket->Shutdown(SD_BOTH);
        }
        catch(wxLuaSocketException & /*e*/)
        {
            NotifyError(_("Error while shutting down session socket"));
        }

        wxUsleep(100);

        wxLuaSocket *acceptedSocket = m_acceptedSocket;
        m_acceptedSocket = NULL;
        delete acceptedSocket;
    }

    // close the server socket
    if (m_serverSocket != NULL)
    {
        // close the server socket by connecting
        // to the socket, thus completing the
        // 'accept'. If a client has not connected,
        // this code will satisfy the accept
        // the m_fShutdown flag will be set
        // so the thread will not loop and instead
        // will just destroy the session socket
        // object and return.
        try
        {
            wxLuaSocket closeSocket;
#ifdef  __WXMSW__
            closeSocket.Connect(g_strNetworkName,
                                m_portNumber);
#else
            closeSocket.Connect(wxT("localhost"),
                                m_portNumber);
#endif
            closeSocket.Shutdown(SD_BOTH);

            wxUsleep(100);

            wxLuaSocket *serverSocket = m_serverSocket;
            m_serverSocket = NULL;
            delete serverSocket;
        }
        catch(wxLuaSocketException & /*e*/)
        {
            NotifyError(_("Error while shutting down server socket"));
        }
    }

    // One of the above two operations
    // terminates the thread.
    // Wait for it to stop.
    if (m_pThread != NULL)
        m_pThread->Wait();

    return true;
}

wxLuaDebugServer::~wxLuaDebugServer()
{
    StopServer();

    if (m_pThread != NULL)
        delete m_pThread;

#if wxCHECK_VERSION(2, 3, 0)
    if (m_pProcess != NULL && m_processID != -1)
        m_pProcess->Kill(m_processID, wxSIGKILL);
#endif
}

bool wxLuaDebugServer::AddBreakPoint(const wxString &fileName, int lineNumber)
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_ADD_BREAKPOINT) &&
                WriteString(m_acceptedSocket, fileName)         &&
                WriteInt(m_acceptedSocket, lineNumber))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::RemoveBreakPoint(const wxString &fileName, int lineNumber)
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_REMOVE_BREAKPOINT) &&
                WriteString(m_acceptedSocket, fileName)            &&
                WriteInt(m_acceptedSocket, lineNumber))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::ClearAllBreakPoints()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_CLEAR_ALL_BREAKPOINTS))
                return true;
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

void wxLuaDebugServer::NotifyError(const wxString &msg)
{
    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_ERROR);

    debugEvent.SetMessage(msg);

    wxGetLuaHandler().AddPendingEvent(debugEvent);
}

bool wxLuaDebugServer::Compile(const wxString &fileName, const wxString &buffer)
{
    bool       fErrorsSeen = false;
    lua_State *luaState    = lua_open();

    luaopen_base(luaState);
    luaopen_table(luaState);
    luaopen_string(luaState);
    luaopen_math(luaState);
    luaopen_debug(luaState);
    luaopen_io(luaState);
//    luaopen_loadlib(luaState);

    LuaCharBuffer charbuf(buffer);
    int status = luaL_loadbuffer(luaState,
                                 charbuf.GetData(),
                                 charbuf.Length(),
                                 wx2lua(fileName));
    switch (status)
    {
        case LUA_ERRRUN:
            NotifyError(_("Error while compiling chunk\n"));
            fErrorsSeen = true;
            break;
        case LUA_ERRSYNTAX:
            {
                wxString strError(_("Syntax error during pre-compilation: "));
                strError += lua2wx(lua_tostring(luaState, -1));
                strError += wxT("\n");
                NotifyError(strError);
            }
            fErrorsSeen = true;
            break;
        case LUA_ERRMEM:
            NotifyError(_("Memory allocation error\n"));
            fErrorsSeen = true;
            break;
        case LUA_ERRERR:
            NotifyError(_("Generic error or an error occurred while running the error handler\n"));
            fErrorsSeen = true;
            break;
        case LUA_ERRFILE:
            {
                wxString strError(_("Error occurred while opening file: "));
                strError += lua2wx(lua_tostring(luaState, -1));
                strError += wxT("\n");
                NotifyError(strError);
            }
            fErrorsSeen = true;
            break;
        default:
            NotifyError(_("Compiled OK\n"));
            break;
    }

    lua_close(luaState);

    return !fErrorsSeen;
}

bool wxLuaDebugServer::Run(const wxString &fileName, const wxString &buffer)
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_RUN_BUFFER) &&
                WriteString(m_acceptedSocket, fileName) &&
                WriteString(m_acceptedSocket, buffer))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::Step()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_DEBUG_STEP);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::StepOver()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_DEBUG_STEPOVER);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::StepOut()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_DEBUG_STEPOUT);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::Continue()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_DEBUG_CONTINUE);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::Break()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_DEBUG_BREAK);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::EnumerateStack()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_ENUMERATE_STACK);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::Reset()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            return WriteByte(m_acceptedSocket, CMD_RESET);
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::EnumerateStackEntry(int stackEntry)
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_ENUMERATE_STACK_ENTRY) &&
                WriteInt(m_acceptedSocket, stackEntry))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::EnumerateTable(int tableRef, int nIndex, int nItemNode)
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_ENUMERATE_TABLE_REF) &&
                WriteInt(m_acceptedSocket, tableRef)                 &&
                WriteInt(m_acceptedSocket, nIndex)                   &&
                WriteInt(m_acceptedSocket, nItemNode))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

bool wxLuaDebugServer::CleanupDebugReferences()
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_CLEAR_DEBUG_REFERENCES))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

void *wxLuaThread::Entry()
{
    m_pServer->ThreadFunction();
    return 0;
}

void wxLuaDebugServer::ThreadFunction()
{
    try
    {
        m_acceptedSocket = new wxLuaSocket(m_serverSocket->Accept());
        if (m_acceptedSocket != NULL)
        {
            wxLuaSocket *serverSocket = m_serverSocket;
            m_serverSocket = NULL;
            delete serverSocket;

#if wxCHECK_VERSION(2, 3, 3)
            wxUsleep(500);  // why ??
#endif

            // if not shutdown requested
            if (!m_fShutdown)
            {
                try
                {
                    // Notify that a client has connected and we are ready to debug
                    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_CLIENT_CONNECTED);

                    wxGetLuaHandler().AddPendingEvent(debugEvent);

                    // Enter the debug loop
                    while (!m_fShutdown)
                    {
                        int  debugEvent = ReadByte(m_acceptedSocket);
                        switch((debugEvents) debugEvent)
                        {
                            case EVENT_DEBUG_BREAK:
                                {
                                    wxString fileName   = ReadString(m_acceptedSocket);
                                    int      lineNumber = ReadInt(m_acceptedSocket);

                                    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_BREAK, lineNumber, fileName);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;

                            case EVENT_DEBUG_PRINT:
                                {
                                    wxString strMessage = ReadString(m_acceptedSocket);

                                    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_PRINT);
                                    debugEvent.SetMessage(strMessage);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;

                            case EVENT_DEBUG_ERROR:
                                {
                                    wxString strMessage = ReadString(m_acceptedSocket);

                                    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_ERROR);
                                    debugEvent.SetMessage(strMessage);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;

                            case EVENT_DEBUG_EXIT:
                                {
                                    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_EXIT);
                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                    m_fShutdown = true;
                                }
                                break;

                            case EVENT_DEBUG_STACK_ENUM:
                                {
                                    wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_STACK_ENUM);
                                    wxLuaDebugData  *pDebugData = ReadDebugData(m_acceptedSocket);
                                    if (pDebugData != NULL)
                                        debugEvent.SetDebugData(-1, pDebugData);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;

                            case EVENT_DEBUG_STACK_ENTRY_ENUM:
                                {
                                    wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_STACK_ENTRY_ENUM);
                                    int              stackRef   = ReadInt(m_acceptedSocket);
                                    wxLuaDebugData  *pDebugData = ReadDebugData(m_acceptedSocket);
                                    if (pDebugData != NULL)
                                        debugEvent.SetDebugData(stackRef, pDebugData);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;

                            case EVENT_DEBUG_TABLE_ENUM:
                                {
                                    wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_TABLE_ENUM);
                                    int              itemNode   = ReadInt(m_acceptedSocket);
                                    wxLuaDebugData     *pDebugData = ReadDebugData(m_acceptedSocket);
                                    if (pDebugData != NULL)
                                        debugEvent.SetDebugData(itemNode, pDebugData);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;

                            case EVENT_DEBUG_EVALUATE_EXPR:
                                {
                                    wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_EVALUATE_EXPR);
                                    int              exprRef   = ReadInt(m_acceptedSocket);
                                    wxString         strResult = ReadString(m_acceptedSocket);

                                    debugEvent.SetMessage(strResult);
                                    debugEvent.SetDebugData(exprRef);

                                    wxGetLuaHandler().AddPendingEvent(debugEvent);
                                }
                                break;
                        case EVENT_DEBUG_BREAKPOINT_ADDED : break;   // FIXME - unused
                        case EVENT_DEBUG_BREAKPOINT_REMOVED : break; // FIXME - unused

                        }
                    }
                }
                catch(wxLuaSocketException & /*e*/)
                {
                }
            }

            // delete the accepted socket
            if (m_acceptedSocket != NULL)
            {
                wxLuaSocket *acceptedSocket = m_acceptedSocket;
                m_acceptedSocket = NULL;
                delete acceptedSocket;
            }
        }
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
}

void wxLuaThread::OnExit()
{
}

#if wxCHECK_VERSION(2, 3, 0)
bool wxLuaDebugServer::DisplayStackDialog(wxWindow *pParent)
{
    if (m_pStackFrameDialog == NULL)
    {
        m_pStackFrameDialog = new wxLuaStackFrame(pParent,
                                             _("Stack frame"),
                                             wxDefaultPosition,
                                             wxSize(500, 250),
                                             this);
        m_pStackFrameDialog->ShowModal();
        m_pStackFrameDialog->Destroy();
        m_pStackFrameDialog = NULL;
        return true;
    }
    return false;
}
#endif

bool wxLuaDebugServer::EvaluateExpr(int exprRef, const wxString &strExpression)
{
    if (m_acceptedSocket != NULL)
    {
        try
        {
            if (WriteByte(m_acceptedSocket, CMD_EVALUATE_EXPR) &&
                WriteInt(m_acceptedSocket, exprRef)            &&
                WriteString(m_acceptedSocket, strExpression))
            {
                return true;
            }
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }
    }
    return false;
}

