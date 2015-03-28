/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaDebuggee.cpp
// Purpose:     wxLua Debuggee - App Debugging Service using wxSockets
// Author:      Ray Gilbert
// Modified by:
// Created:     2003-06-23
// RCS-ID:      $Id: wxLuaDebuggerService.cpp,v 1.3 2004/06/03 05:41:58 john Exp $
// Copyright:   (c) Ray Gilbert
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
//
// Based heavily on:
//      wxLuaDebugTarget.cpp and other wxLua files
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
//      wxWidgets Socket Sample

#ifdef __GNUG__
    #pragma implementation "wxLuaDebuggerService.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxCHECK_VERSION(2,3,0)
    #include "wx/filename.h"
#endif

#include "wxLuaDebuggerService.h"
#include "wxLuaDebugServer.h"

#include <wx/listimpl.cpp>

static const char *debugName = "__wxLuaDebug__";

WX_DEFINE_LIST(LuaBreakPointList);
WX_DEFINE_LIST(wxLuaDebuggeeList);
WX_DEFINE_LIST(wxLuaDebugSocketList);

/////////////////////////////////////////////////////////////
// Project BreakPoint
//
// BreakPoint - a filename/line pair
LuaBreakPoint::LuaBreakPoint()
:   m_enabled(true), m_line(0)
{
}

LuaBreakPoint::LuaBreakPoint(const wxFileName& fileName, int line, bool enabled)
:   m_fileName(fileName), m_enabled(enabled), m_line(line)
{
}

void LuaBreakPoint::Copy(const LuaBreakPoint& breakPoint)
{
    m_fileName = breakPoint.m_fileName;
    m_line = breakPoint.m_line;
    m_enabled = breakPoint.m_enabled;
}

////////////////////////////////////////////////////////////////////////
// wxLuaDebugSocket
//
// Handles Debugger/Debuggee IO
unsigned char  wxLuaDebugSocket::ReadByte()
{
    unsigned char value;
    m_Socket->Read((char *) &value, sizeof(value));
    return value;
}

bool wxLuaDebugSocket::WaitForRead()
{
    return m_Socket->WaitForRead(0, 300);
}

short wxLuaDebugSocket::ReadWord()
{
    short value;
    m_Socket->Read((char *)&value, sizeof(value));
    return value;
}

unsigned short wxLuaDebugSocket::ReadUWord()
{
    unsigned short value;
    m_Socket->Read((char *)&value, sizeof(value));
    return value;
}

int wxLuaDebugSocket::ReadInt()
{
    int value;
    m_Socket->Read((char *) &value, sizeof(value));
    return value;
}

unsigned int wxLuaDebugSocket::ReadUInt()
{
    unsigned int value;
    m_Socket->Read((char *) &value, sizeof(value));
    return value;
}

long wxLuaDebugSocket::ReadLong()
{
    long value;
    m_Socket->Read((char *) &value, sizeof(value));
    return value;
}

unsigned long wxLuaDebugSocket::ReadULong()
{
    unsigned long value;
    m_Socket->Read((char *) &value, sizeof(value));
    return value;
}

wxString wxLuaDebugSocket::ReadString()
{
    wxString value;
    unsigned int length = ReadInt();
    if (length > 0)
    {
        char *buffer = new char[length + 1];
        if (buffer)
        {
            m_Socket->Read(buffer, length);
            buffer[length] = 0;
            value = lua2wx(buffer);
            delete[] buffer;
        }
    }

    return value;
}

wxLuaDebugData *wxLuaDebugSocket::ReadDebugData()
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();

    size_t idx, idxMax;
    m_Socket->Read((char *) &idxMax, sizeof(idxMax));

    for (idx = 0; idx < idxMax; ++idx)
    {
        int bufferLength;
        m_Socket->Read((char *) &bufferLength, sizeof(bufferLength));
        if (bufferLength > 0)
        {
            char *pBuffer = new char[bufferLength];
            char *pMemory = pBuffer;
            m_Socket->Read(pMemory, bufferLength);
            int nReference = *(int *) pMemory;
            pMemory += sizeof(int);
            int nIndex = *(int *) pMemory;
            pMemory += sizeof(int);
            bool fExpanded = (0 != *pMemory++);

            const char *pNamePtr = pMemory;
            pMemory += strlen(pNamePtr) + 1;
            const char *pTypePtr = pMemory;
            pMemory += strlen(pTypePtr) + 1;
            const char *pValuePtr = pMemory;
            pMemory += strlen(pValuePtr) + 1;
            const char *pSourcePtr = pMemory;

            wxLuaDebugDataItem *pItem = new wxLuaDebugDataItem(lua2wx(pNamePtr),
                                                               lua2wx(pTypePtr),
                                                               lua2wx(pValuePtr),
                                                               lua2wx(pSourcePtr),
                                                               nReference,
                                                               nIndex,
                                                               fExpanded);
            pSortedList->Add(pItem);

            delete[] pBuffer;
        }
    }

    return pSortedList;
}

bool wxLuaDebugSocket::WriteByte(unsigned char value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteWord(short value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteUWord(unsigned short value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteInt(int value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteUInt(unsigned int value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteLong(long value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteULong(unsigned long value)
{
    m_Socket->Write((const char *) &value, sizeof(value));
    return !m_Socket->Error();
}

bool wxLuaDebugSocket::WriteString(const wxString &value)
{
    LuaCharBuffer buf(value);
    int buflen = buf.Length();
    m_Socket->Write((const char *) &buflen, sizeof(buflen));
    if (m_Socket->Error())
        return false;

    if (buflen > 0)
    {
        m_Socket->Write(buf.GetData(), buflen);
        if (m_Socket->Error())
            return false;
    }

    return true;
}

bool wxLuaDebugSocket::WriteDebugData(const wxLuaDebugData *pSortedList)
{
    size_t idx, idxMax = pSortedList->Count();

    m_Socket->Write((const char *) &idxMax, sizeof(idxMax));
    if (m_Socket->Error())
        return false;

    for (idx = 0; idx < idxMax; ++idx)
    {
        const wxLuaDebugDataItem *item = pSortedList->Item(idx);

        int nameLength = item->GetName().Length() + 1;
        int typeLength = item->GetType().Length() + 1;
        int valueLength = item->GetValue().Length() + 1;
        int sourceLength = item->GetSource().Length() + 1;

        int bufferLength = (2 * sizeof(int)) +
                            sizeof(unsigned char) +
                            nameLength +
                            typeLength +
                            valueLength +
                            sourceLength;

        m_Socket->Write((const char *) &bufferLength, sizeof(bufferLength));
        if (m_Socket->Error())
            return false;

        unsigned char *pBuffer = new unsigned char[bufferLength];
        unsigned char *pMemory = pBuffer;

        *(int *) pMemory = item->GetReference();
        pMemory += sizeof(int);

        *(int *) pMemory = item->GetIndex();
        pMemory += sizeof(int);

        *pMemory++ = item->IsExpanded();

        memcpy(pMemory, wx2lua(item->GetName()), nameLength);
        pMemory += nameLength;

        memcpy(pMemory, wx2lua(item->GetType()), typeLength);
        pMemory += typeLength;

        memcpy(pMemory, wx2lua(item->GetValue()), valueLength);
        pMemory += valueLength;

        memcpy(pMemory, wx2lua(item->GetSource()), sourceLength);

        m_Socket->Write((const char *) pBuffer, bufferLength);

        delete[] pBuffer;

        if (m_Socket->Error())
            return false;
    }

    return true;
}


////////////////////////////////////////////////////////////////////////
// wxLuaDebugService
//
// App Debugging Service
BEGIN_EVENT_TABLE(wxLuaDebugService, wxEvtHandler)
  EVT_SOCKET(SERVER_ID,  wxLuaDebugService::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  wxLuaDebugService::OnSocketEvent)
END_EVENT_TABLE()

wxLuaDebugService::wxLuaDebugService(int portNumber)
: m_serverSocket(NULL), m_portNumber(portNumber)
{
    StartService();
}

wxLuaDebugService::~wxLuaDebugService()
{
    if (m_serverSocket)
    {
        wxSocketBase* sock = m_serverSocket;
        m_serverSocket = NULL;

        sock->Destroy();
        delete sock;
    }

    // Cleanup Debuggee List
    wxLuaDebuggeeList::Node* nodeDebuggee;
    wxLuaDebuggeeList::Node* nextDebuggee;

    for (nodeDebuggee = m_debuggeeList.GetFirst(); nodeDebuggee; nodeDebuggee = nextDebuggee)
    {
        nextDebuggee = nodeDebuggee->GetNext();

        wxLuaDebuggee* debuggee = nodeDebuggee->GetData();
        m_debuggeeList.DeleteNode(nodeDebuggee);
        delete debuggee;
    }

    // Cleanup Debugger Socket List
    wxLuaDebugSocketList::Node* nodeDebuggerSocket;
    wxLuaDebugSocketList::Node* nextDebuggerSocket;

    for (nodeDebuggerSocket = m_debuggerSocketList.GetFirst(); nodeDebuggerSocket; nodeDebuggerSocket = nextDebuggerSocket)
    {
        nextDebuggerSocket = nodeDebuggerSocket->GetNext();

        wxLuaDebugSocket* debuggerSocket = nodeDebuggerSocket->GetData();
        m_debuggerSocketList.DeleteNode(nodeDebuggerSocket);
        delete debuggerSocket;
    }
}

// Start Debugging Service
bool wxLuaDebugService::StartService()
{
    if (!m_serverSocket)
    {
        // Create the address - defaults to localhost:0 initially
        wxIPV4address addr;
        addr.Service(m_portNumber);

        // Create the server socket
        m_serverSocket = new wxSocketServer(addr, wxSOCKET_NOWAIT|wxSOCKET_BLOCK);
        m_serverSocket->SetEventHandler(*this, SERVER_ID);
        m_serverSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
        m_serverSocket->SetFlags(wxSOCKET_BLOCK);
        m_serverSocket->Notify(TRUE);
    }

    return m_serverSocket->Ok();
}

// Add Lua Interpreter to Debug
void wxLuaDebugService::DebugLua(lua_State* state)
{
    wxCriticalSectionLocker locker(m_debuggeeListCriticalSection);

    m_debuggeeList.Append(new wxLuaDebuggee(this, state));
}

// When a Debugger Connects to our Debug Service
void wxLuaDebugService::OnServerEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        wxLuaDebugSocket* debugSocket;
        wxSocketBase *sock = m_serverSocket->Accept(FALSE);
        if (!sock)
        {
            // Error
            return;
        }

        sock->SetFlags(wxSOCKET_NOWAIT);

        // Add New Debugger Connection to Socket List
        {
            wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

            debugSocket = new wxLuaDebugSocket(sock);

            m_debuggerSocketList.Append(debugSocket);
        }

        // Setup Handler
        sock->SetEventHandler(*this, SOCKET_ID);
        sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
        sock->Notify(TRUE);

        // Notify Connected Debugger of Existing BreakPoints
        wxLuaDebuggee* debuggee = GetActiveDebuggee();
        if (debuggee)
            debuggee->NotifyBreakPoints(debugSocket);
        break;
    }

    default:
        // Error
        break;
    }
}

// Handle Commands from debugger (and lost connections)
void wxLuaDebugService::OnSocketEvent(wxSocketEvent& event)
{
    wxSocketBase *sock = event.GetSocket();

    // Now we process the event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        // We disable input events, so that the test doesn't trigger
        // wxSocketEvent again.
        sock->SetNotify(wxSOCKET_LOST_FLAG);

        wxLuaDebugSocket* debugSocket = GetDebugSocket(sock);
        wxLuaDebuggee* debuggee = GetActiveDebuggee();

        if (debugSocket && debuggee)
        {
            debugCommands debugCommand = (debugCommands)debugSocket->ReadByte();
            switch(debugCommand)
            {
            case CMD_ADD_BREAKPOINT:
                {
                    wxString fileName   = debugSocket->ReadString();
                    int      lineNumber = debugSocket->ReadInt();

                    debuggee->AddBreakPoint(fileName, lineNumber);
                }
                break;

            case CMD_REMOVE_BREAKPOINT:
                {
                    wxString fileName   = debugSocket->ReadString();
                    int      lineNumber = debugSocket->ReadInt();

                    debuggee->RemoveBreakPoint(fileName, lineNumber);
                }
                break;

            case CMD_DISABLE_BREAKPOINT:
                {
                    wxString fileName   = debugSocket->ReadString();
                    int      lineNumber = debugSocket->ReadInt();

                    debuggee->DisableBreakPoint(fileName, lineNumber);
                }
                break;


            case CMD_ENABLE_BREAKPOINT:
                {
                    wxString fileName   = debugSocket->ReadString();
                    int      lineNumber = debugSocket->ReadInt();

                    debuggee->EnableBreakPoint(fileName, lineNumber);
                }
                break;

            case CMD_CLEAR_ALL_BREAKPOINTS:
                debuggee->ClearAllBreakPoints();
                break;

            case CMD_DEBUG_STEP:
                debuggee->Step();
                break;

            case CMD_DEBUG_STEPOVER:
                debuggee->StepOver();
                break;

            case CMD_DEBUG_STEPOUT:
                debuggee->StepOut();
                break;

            case CMD_DEBUG_CONTINUE:
                debuggee->Continue();
                break;

            case CMD_DEBUG_BREAK:
                debuggee->Break();
                break;

            case CMD_ENUMERATE_STACK:
                debuggee->EnumerateStack();
                break;

            case CMD_ENUMERATE_STACK_ENTRY:
                {
                    int stackRef = debugSocket->ReadInt();
                    debuggee->EnumerateStackEntry(stackRef);
                }
                break;

            case CMD_ENUMERATE_TABLE_REF:
                {
                    int tableRef  = debugSocket->ReadInt();
                    int index     = debugSocket->ReadInt();
                    int itemNode  = debugSocket->ReadInt();
                    debuggee->EnumerateTable(tableRef, index, itemNode);
                }
                break;

            case CMD_RESET:
                debuggee->Reset();
                break;

            case CMD_EVALUATE_EXPR:
                {
                    int      exprRef  = debugSocket->ReadInt();
                    wxString buffer   = debugSocket->ReadString();
                    debuggee->EvaluateExpr(exprRef, buffer);
                }
                break;

            case CMD_CLEAR_DEBUG_REFERENCES:
                {
                    debuggee->ClearReferences();
                }
                break;
            case CMD_RUN_BUFFER: break; // FIXME - not impemented
            }
        }

        // Enable input events again.
        sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
        break;
    }

    case wxSOCKET_LOST:
    {
        bool sockdestroyed = false;

        // Remove Debugger Connection from Socket List
        {
            wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

            for ( wxLuaDebugSocketList::Node *node = m_debuggerSocketList.GetFirst(); node; node = node->GetNext() )
            {
                if (node->GetData()->GetSocket() == sock)
                {
                    wxLuaDebugSocket* debugSocket = node->GetData();

                    m_debuggerSocketList.DeleteNode(node);

                    debugSocket->Destroy();
                    delete debugSocket;
                    sockdestroyed = true;
                    break;
                }
            }
        }

        // All Debuggers Closed
        if (!IsDebugger())
        {
            // Debugger Closed Unexpectedly
            // InteruptWait By Continue()
            wxLuaDebuggee* debuggee = GetActiveDebuggee();
            if (debuggee)
                debuggee->Continue();

            wxLuaDebugEvent debugEvent(wxEVT_DEBUG_STOPDEBUGGER);
            wxGetLuaHandler().ProcessEvent(debugEvent);
        }

        if (!sockdestroyed)
        {
            sock->Destroy();
//          delete sock;
        }
    }
        break;

    default:
        // Error
        break;
    }
}

// Get Lua Debug Socket
wxLuaDebugSocket* wxLuaDebugService::GetDebugSocket(wxSocketBase* sock)
{
    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        if (m_debuggerSocketList[i]->GetSocket() == sock)
            return m_debuggerSocketList[i];
    }

    return NULL;
}

// Get Active Debuggee Service
wxLuaDebuggee* wxLuaDebugService::GetActiveDebuggee()
{
    wxCriticalSectionLocker locker(m_debuggeeListCriticalSection);

    // TODO: Show Interpreter List, allow a way to switch command between interpreters
    if (m_debuggeeList.GetCount() > 0)
        return m_debuggeeList[0];

    return NULL;
}

// Is Debugger
bool wxLuaDebugService::IsDebugger()
{
    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    return (m_debuggerSocketList.GetCount() != 0);
}

// Debugger Notification
bool wxLuaDebugService::NotifyBreak(const wxFileName &fileName, int lineNumber)
{
    bool result = false;

    wxCriticalSectionLocker lockerNotifyBreak(m_notifyBreakCriticalSection);

    // Attempt Start Debugger
    if (!IsDebugger())
    {
        wxLuaDebugEvent debugEvent(wxEVT_DEBUG_STARTDEBUGGER);

        // started debug process
        if (!wxGetLuaHandler().ProcessEvent(debugEvent))
            return false;

        // Wait for connect
        while (!IsDebugger())
        {
            wxYield();
        }
    }

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_BREAK);
#if wxCHECK_VERSION(2,3,0)
        m_debuggerSocketList[i]->WriteString(fileName.GetFullPath());
#else
        m_debuggerSocketList[i]->WriteString(fileName);
#endif
        m_debuggerSocketList[i]->WriteInt(lineNumber);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyPrint(const wxString &errorMsg)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_ERROR);
        m_debuggerSocketList[i]->WriteString(errorMsg);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyError(const wxString &errorMsg)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_ERROR);
        m_debuggerSocketList[i]->WriteString(errorMsg);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyExit()
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_EXIT);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyStackEnumeration(wxLuaDebugData *pDebugData)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_STACK_ENUM);
        m_debuggerSocketList[i]->WriteDebugData(pDebugData);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    delete pDebugData;

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyStackEntryEnumeration(int entryRef, wxLuaDebugData *pDebugData)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_STACK_ENTRY_ENUM);
        m_debuggerSocketList[i]->WriteInt(entryRef);
        m_debuggerSocketList[i]->WriteDebugData(pDebugData);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    delete pDebugData;

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyTableEnumeration(int itemNode, wxLuaDebugData *pDebugData)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_TABLE_ENUM);
        m_debuggerSocketList[i]->WriteInt(itemNode);
        m_debuggerSocketList[i]->WriteDebugData(pDebugData);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    delete pDebugData;

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyEvaluateExpr(int exprRef, const wxString &strResult)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_EVALUATE_EXPR);
        m_debuggerSocketList[i]->WriteInt(exprRef);
        m_debuggerSocketList[i]->WriteString(strResult);

        if (!m_debuggerSocketList[i]->Error())
            result = true;
    }

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyBreakpointAdded(LuaBreakPoint& breakPoint, wxLuaDebugSocket* debugSocket)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        if (!debugSocket || debugSocket == m_debuggerSocketList[i])
        {
            m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_BREAKPOINT_ADDED);
#if wxCHECK_VERSION(2,3,0)
            m_debuggerSocketList[i]->WriteString(breakPoint.GetFileName().GetFullPath());
#else
            m_debuggerSocketList[i]->WriteString(breakPoint.GetFileName());
#endif
            m_debuggerSocketList[i]->WriteInt(breakPoint.GetLine());
            m_debuggerSocketList[i]->WriteInt((int)breakPoint.GetEnabled());

            if (!m_debuggerSocketList[i]->Error())
                result = true;
        }
    }

    return result;
}

// Debugger Notification
bool wxLuaDebugService::NotifyBreakpointRemoved(LuaBreakPoint& breakPoint, wxLuaDebugSocket* debugSocket)
{
    bool result = false;

    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        if (!debugSocket || debugSocket == m_debuggerSocketList[i])
        {
            m_debuggerSocketList[i]->WriteByte(EVENT_DEBUG_BREAKPOINT_REMOVED);
#if wxCHECK_VERSION(2,3,0)
            m_debuggerSocketList[i]->WriteString(breakPoint.GetFileName().GetFullPath());
#else
            m_debuggerSocketList[i]->WriteString(breakPoint.GetFileName());
#endif
            m_debuggerSocketList[i]->WriteInt(breakPoint.GetLine());
            m_debuggerSocketList[i]->WriteInt((int)breakPoint.GetEnabled());

            if (!m_debuggerSocketList[i]->Error())
                result = true;
        }
    }

    return result;
}

bool wxLuaDebugService::WaitForRead()
{
    wxCriticalSectionLocker locker(m_debuggerSocketListCriticalSection);

    for (size_t i = 0; i < m_debuggerSocketList.GetCount(); i++)
    {
        if (m_debuggerSocketList[i]->WaitForRead())
            return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////
// wxLuaDebugger
//
// Lua Debugger Client
// (Client Side of Lua Debug Service)
BEGIN_EVENT_TABLE(wxLuaDebugger, wxEvtHandler)
  EVT_SOCKET(SOCKET_DEBUGGER_ID,  wxLuaDebugger::OnSocketEvent)
END_EVENT_TABLE()

wxLuaDebugger::wxLuaDebugger()
{
    m_debuggerSocket = new wxLuaDebugSocket();
}

wxLuaDebugger::~wxLuaDebugger()
{
    wxCriticalSectionLocker locker(m_debuggerSocketCriticalSection);
    delete m_debuggerSocket;
}

// Connect to Debugger Service
bool wxLuaDebugger::Connect(const wxString& hostname, int portNumber)
{
    wxIPV4address addr;

    addr.Hostname(hostname);
    addr.Service(portNumber);

    wxSocketClient* sock = new wxSocketClient();

    // Setup the event handler and subscribe to most events
    sock->SetEventHandler(*this, SOCKET_DEBUGGER_ID);
    sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
    sock->Notify(TRUE);

    if (!sock->Connect(addr, true))
    {
        // Could Not Connect
        delete sock;

        return false;
    }

    return true;
}

void wxLuaDebugger::OnSocketEvent(wxSocketEvent& event)
{
    wxSocketBase *sock = event.GetSocket();

    // Now we process the event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
        {
            wxCriticalSectionLocker locker(m_debuggerSocketCriticalSection);

            m_debuggerSocket->SetSocket(sock);
        }
        break;

    case wxSOCKET_INPUT:
    {
        wxCriticalSectionLocker locker(m_debuggerSocketCriticalSection);

        if (!m_debuggerSocket->GetSocket())
            return;

        wxASSERT_MSG(m_debuggerSocket->GetSocket() == sock,_("Debugger Socket Incorrect"));

        m_debuggerSocket->SetSocket(sock);

        // We disable input events, so that the test doesn't trigger
        // wxSocketEvent again.
        sock->SetNotify(wxSOCKET_LOST_FLAG);

        // Events from Debug Service
        int  debuggeeEvent = m_debuggerSocket->ReadByte();
        switch((debugEvents) debuggeeEvent)
        {
        case EVENT_DEBUG_BREAK:
            {
                wxString fileName   = m_debuggerSocket->ReadString();
                int      lineNumber = m_debuggerSocket->ReadInt();

                wxLuaDebugEvent debugEvent(wxEVT_DEBUG_BREAK, lineNumber, fileName);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_PRINT:
            {
                wxString strMessage = m_debuggerSocket->ReadString();

                wxLuaDebugEvent debugEvent(wxEVT_DEBUG_PRINT);
                debugEvent.SetMessage(strMessage);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_ERROR:
            {
                wxString strMessage = m_debuggerSocket->ReadString();

                wxLuaDebugEvent debugEvent(wxEVT_DEBUG_ERROR);
                debugEvent.SetMessage(strMessage);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_EXIT:
            {
                wxLuaDebugEvent debugEvent(wxEVT_DEBUG_EXIT);
                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_STACK_ENUM:
            {
                wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_STACK_ENUM);
                wxLuaDebugData     *pDebugData = m_debuggerSocket->ReadDebugData();
                if (pDebugData != NULL)
                    debugEvent.SetDebugData(-1, pDebugData);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_STACK_ENTRY_ENUM:
            {
                wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_STACK_ENTRY_ENUM);
                int              stackRef   = m_debuggerSocket->ReadInt();
                wxLuaDebugData     *pDebugData = m_debuggerSocket->ReadDebugData();
                if (pDebugData != NULL)
                    debugEvent.SetDebugData(stackRef, pDebugData);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_TABLE_ENUM:
            {
                wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_TABLE_ENUM);
                int              itemNode   = m_debuggerSocket->ReadInt();
                wxLuaDebugData     *pDebugData = m_debuggerSocket->ReadDebugData();
                if (pDebugData != NULL)
                    debugEvent.SetDebugData(itemNode, pDebugData);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_EVALUATE_EXPR:
            {
                wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_EVALUATE_EXPR);
                int              exprRef   = m_debuggerSocket->ReadInt();
                wxString         strResult = m_debuggerSocket->ReadString();

                debugEvent.SetMessage(strResult);
                debugEvent.SetDebugData(exprRef);

                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_BREAKPOINT_ADDED:
            {
                wxString         fileName = m_debuggerSocket->ReadString();
                int              line = m_debuggerSocket->ReadInt();
                bool             enabled = m_debuggerSocket->ReadInt() ? true : false;

                wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_BREAKPOINT_ADDED, line, fileName, enabled);
                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;

        case EVENT_DEBUG_BREAKPOINT_REMOVED:
            {
                wxString         fileName = m_debuggerSocket->ReadString();
                int              line = m_debuggerSocket->ReadInt();

                wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_BREAKPOINT_REMOVED, line, fileName);
                wxGetLuaHandler().AddPendingEvent(debugEvent);
            }
            break;
        }

        // Enable input events again.
        sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
        break;
    }

    case wxSOCKET_LOST:
        {
            wxCriticalSectionLocker locker(m_debuggerSocketCriticalSection);

            m_debuggerSocket->SetSocket(NULL);

            sock->Destroy();
            delete sock;
        }
        break;

    default:
        // Error
        break;
    }
}

bool wxLuaDebugger::AddBreakPoint(const wxString &fileName, int lineNumber)
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_ADD_BREAKPOINT) &&
            m_debuggerSocket->WriteString(fileName)         &&
            m_debuggerSocket->WriteInt(lineNumber));
}

bool wxLuaDebugger::RemoveBreakPoint(const wxString &fileName, int lineNumber)
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_REMOVE_BREAKPOINT) &&
            m_debuggerSocket->WriteString(fileName)            &&
            m_debuggerSocket->WriteInt(lineNumber));
}

bool wxLuaDebugger::DisableBreakPoint(const wxString &fileName, int lineNumber)
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_DISABLE_BREAKPOINT) &&
            m_debuggerSocket->WriteString(fileName)         &&
            m_debuggerSocket->WriteInt(lineNumber));
}

bool wxLuaDebugger::EnableBreakPoint(const wxString &fileName, int lineNumber)
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_ENABLE_BREAKPOINT) &&
            m_debuggerSocket->WriteString(fileName)         &&
            m_debuggerSocket->WriteInt(lineNumber));
}

bool wxLuaDebugger::ClearAllBreakPoints()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_CLEAR_ALL_BREAKPOINTS));
}

bool wxLuaDebugger::Step()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_DEBUG_STEP);
}

bool wxLuaDebugger::StepOver()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_DEBUG_STEPOVER);
}

bool wxLuaDebugger::StepOut()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_DEBUG_STEPOUT);
}

bool wxLuaDebugger::Continue()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_DEBUG_CONTINUE);
}

bool wxLuaDebugger::Break()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_DEBUG_BREAK);
}

bool wxLuaDebugger::EnumerateStack()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_ENUMERATE_STACK);
}

bool wxLuaDebugger::Reset()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return m_debuggerSocket->WriteByte(CMD_RESET);
}

bool wxLuaDebugger::EnumerateStackEntry(int stackEntry)
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_ENUMERATE_STACK_ENTRY) &&
            m_debuggerSocket->WriteInt(stackEntry));
}

bool wxLuaDebugger::EnumerateTable(int tableRef, int nIndex, int nItemNode)
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_ENUMERATE_TABLE_REF) &&
            m_debuggerSocket->WriteInt(tableRef)                 &&
            m_debuggerSocket->WriteInt(nIndex)                   &&
            m_debuggerSocket->WriteInt(nItemNode));
}

bool wxLuaDebugger::CleanupDebugReferences()
{
    if (!m_debuggerSocket->IsConnected())
        return false;

    return (m_debuggerSocket->WriteByte(CMD_CLEAR_DEBUG_REFERENCES));
}

void wxLuaDebugger::NotifyError(const wxString &msg)
{
    wxLuaDebugEvent debugEvent(wxEVT_DEBUG_ERROR);

    debugEvent.SetMessage(msg);

    wxGetLuaHandler().AddPendingEvent(debugEvent);
}

////////////////////////////////////////////////////////////////////////
// wxLuaDebuggee
//
// The Lua Interpreter Being Debugged
// (Server Side of Lua Debug Service)
wxLuaDebuggee::wxLuaDebuggee(wxLuaDebugService* debugService, lua_State* luaState) :
    m_debugService(debugService),
    m_luaState(luaState),
    m_breakPointList(wxKEY_STRING),
    m_forceBreak(false),
    m_resetRequested(false),
    m_fErrorsSeen(false),
    m_nFramesUntilBreak(0),
    m_InDebugHook(false),
    m_Wait(false)
{
    lua_pushlightuserdata(m_luaState, this);
    lua_setglobal(m_luaState, debugName);

    lua_sethook(m_luaState, LuaDebugHook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);

    lua_pushcfunction(m_luaState, LuaPrint);
    lua_setglobal(m_luaState, "print");

    wxGetLuaHandler().EnterLuaCriticalSection();
}

wxLuaDebuggee::~wxLuaDebuggee()
{
    // Cleanup Break Point List
    LuaBreakPointList::Node* nodeBreakPoint;
    LuaBreakPointList::Node* nextBreakPoint;

    for (nodeBreakPoint = m_breakPointList.GetFirst(); nodeBreakPoint; nodeBreakPoint = nextBreakPoint)
    {
        nextBreakPoint = nodeBreakPoint->GetNext();

        LuaBreakPoint* breakPoint = nodeBreakPoint->GetData();
        m_breakPointList.DeleteNode(nodeBreakPoint);
        delete breakPoint;
    }

    wxGetLuaHandler().LeaveLuaCriticalSection();

    // Unhook
    lua_sethook(m_luaState, LuaDebugHook, 0, 0);


}

// LuaDebugHook
//
// Lua Debugging is done through this Hook
// Note that this is a static function
void LUACALL wxLuaDebuggee::LuaDebugHook(lua_State *luaState, lua_Debug *pLuaDebug)
{
    // Get Reference to this Debuggee Object
    lua_getglobal(luaState, debugName);
    wxLuaDebuggee *pTarget = (wxLuaDebuggee *) lua_touserdata(luaState, -1);
    lua_pop(luaState, 1);

    // Pass Lua Debug Event to Debuggee Object
    if (pTarget != NULL)
        pTarget->DebugHook(pLuaDebug->event);
}

// Process Lua Debug Data
bool wxLuaDebuggee::DebugHook(int event)
{
    bool fWait = false;
    m_InDebugHook = true;

    if (m_forceBreak)
    {
        if (m_resetRequested)
        {
            m_debugService->NotifyExit();

            // TODO: how do we stop lua interpreter

            m_InDebugHook = false;
            return false;
        }

        int      lineNumber = 0;
        wxFileName fileName   = GetDebugInfo(lineNumber);

        if (m_debugService->NotifyBreak(fileName, lineNumber))
            fWait = true;
    }
    else
    {
        if (event == LUA_HOOKCALL) // call
            m_nFramesUntilBreak++;
        else
        if (event == LUA_HOOKRET || event == LUA_HOOKTAILRET) // return
        {
            if (m_nFramesUntilBreak > 0)
                m_nFramesUntilBreak--;
        }
        else
        if (event == LUA_HOOKLINE) // line
        {
            switch (m_nextOperation)
            {
                case DEBUG_STEP:
                    {
                        int      lineNumber = 0;
                        wxFileName fileName   = GetDebugInfo(lineNumber);

                        if (m_debugService->NotifyBreak(fileName, lineNumber))
                            fWait = true;
                    }
                    break;

                case DEBUG_STEPOVER:
                    if (m_nFramesUntilBreak == 0)
                    {
                        int      lineNumber = 0;
                        wxFileName fileName   = GetDebugInfo(lineNumber);

                        if (m_debugService->NotifyBreak(fileName, lineNumber))
                            fWait = true;
                    }
                    break;

                case DEBUG_GO:
                default:
                    {
                        int      lineNumber = 0;
                        wxFileName fileName   = GetDebugInfo(lineNumber);

                        if (AtBreakPoint(fileName, lineNumber) &&
                            m_debugService->NotifyBreak(fileName, lineNumber))
                            fWait = true;
                    }
                    break;
            }
        }
    }

    if (fWait)
    {
        m_Wait = true;

        // release the critical section so
        // the other thread can access LUA
        wxGetLuaHandler().LeaveLuaCriticalSection();

        // Wait for Debugger To Send Command
        while (m_Wait)
        {
            // Wait Until We Receive A Message From Debugger
            if (m_debugService->WaitForRead())
            {
                wxYield();
            }

            // Should Only Wait if Debugger
            if (!m_debugService->IsDebugger())
                InteruptWait();
        }

        // acquire the critical section again
        wxGetLuaHandler().EnterLuaCriticalSection();
    }

    m_InDebugHook = false;

    return fWait;
}

void wxLuaDebuggee::InteruptWait()
{
    m_Wait = false;
}

void wxLuaDebuggee::DisplayError(const wxString &strError)
{
    m_debugService->NotifyError(strError);
}

void wxLuaDebuggee::NotifyPrint(const wxString &strMsg)
{
    m_debugService->NotifyPrint(strMsg);
}

void wxLuaDebuggee::ClearReferences()
{
    size_t idx, idxMax = m_references.GetCount();
    for (idx = 0; idx < idxMax; ++idx)
    {
        int iItem = m_references.Item(idx);
        tremove(m_luaState, iItem);
    }
}

void wxLuaDebuggee::AddBreakPoint(const wxFileName &fileName, int lineNumber)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPointKey = LuaBreakPoint::GetBreakPointKey(fileName, lineNumber);

    LuaBreakPointList::Node* node = m_breakPointList.Find(breakPointKey);
    if (!node)
    {
        LuaBreakPoint* breakPoint = new LuaBreakPoint(fileName, lineNumber);

        m_breakPointList.Append(breakPointKey, breakPoint);

        // Notify Debuggers
        m_debugService->NotifyBreakpointAdded(*breakPoint);

        // Notify Debuggee
#if wxCHECK_VERSION(2,3,0)
        wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_BREAKPOINT_ADDED, lineNumber, fileName.GetFullPath());
#else
        wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_BREAKPOINT_ADDED, lineNumber, fileName);
#endif

        wxGetLuaHandler().AddPendingEvent(debugEvent);
    }
}

void wxLuaDebuggee::RemoveBreakPoint(const wxFileName &fileName, int lineNumber)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPointKey = LuaBreakPoint::GetBreakPointKey(fileName, lineNumber);

    LuaBreakPointList::Node* node = m_breakPointList.Find(breakPointKey);
    if (node)
    {
        LuaBreakPoint* breakPoint = node->GetData();

        m_breakPointList.DeleteNode(node);

        // Notify debuggers
        m_debugService->NotifyBreakpointRemoved(*breakPoint);

        // Notify Debuggee
#if wxCHECK_VERSION(2,3,0)
        wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_BREAKPOINT_REMOVED, lineNumber, fileName.GetFullPath());
#else
        wxLuaDebugEvent  debugEvent(wxEVT_DEBUG_BREAKPOINT_REMOVED, lineNumber, fileName);
#endif
        wxGetLuaHandler().AddPendingEvent(debugEvent);

        delete breakPoint;
    }
}

void wxLuaDebuggee::DisableBreakPoint(const wxFileName &fileName, int lineNumber)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPointKey = LuaBreakPoint::GetBreakPointKey(fileName, lineNumber);

    LuaBreakPointList::Node* node = m_breakPointList.Find(breakPointKey);
    if (node)
    {
        node->GetData()->SetEnabled(false);
    }
}

void wxLuaDebuggee::EnableBreakPoint(const wxFileName &fileName, int lineNumber)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPointKey = LuaBreakPoint::GetBreakPointKey(fileName, lineNumber);

    LuaBreakPointList::Node* node = m_breakPointList.Find(breakPointKey);
    if (node)
    {
        node->GetData()->SetEnabled(true);
    }
}

void wxLuaDebuggee::ClearAllBreakPoints()
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    m_breakPointList.Clear();
}

void wxLuaDebuggee::Step()
{
    m_nextOperation = DEBUG_STEP;
    if (m_InDebugHook)
        InteruptWait();
}

void wxLuaDebuggee::StepOver()
{
    m_nFramesUntilBreak = 0;
    m_nextOperation = DEBUG_STEPOVER;
    if (m_InDebugHook)
        InteruptWait();
}

void wxLuaDebuggee::StepOut()
{
    m_nFramesUntilBreak = 1;
    m_nextOperation = DEBUG_STEPOVER;
    if (m_InDebugHook)
        InteruptWait();
}

void wxLuaDebuggee::Continue()
{
    m_nextOperation = DEBUG_GO;
    if (m_InDebugHook)
        InteruptWait();
}

void wxLuaDebuggee::Break()
{
    m_forceBreak = TRUE;
}

void wxLuaDebuggee::Reset()
{
    m_forceBreak     = TRUE;
    m_resetRequested = TRUE;
    if (m_InDebugHook)
        InteruptWait();
}

bool wxLuaDebuggee::EnumerateStack()
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL)
    {
        int       nIndex = 0;
        bool      fFirst = true;
        lua_Debug luaDebug;

        wxGetLuaHandler().EnterLuaCriticalSection();

        while (lua_getstack(m_luaState, nIndex, &luaDebug) != 0)
        {
            if (lua_getinfo(m_luaState, "Sln", &luaDebug))
            {
                wxString name = lua2wx(luaDebug.name);
                wxString source = lua2wx(luaDebug.source);

                wxString stackItem;

                // skip stack frames that do not have line number
                int  currentLine = luaDebug.currentline;
                if (!fFirst || currentLine != -1)
                {
                    if (currentLine == -1)
                        currentLine = 0;

                    if (luaDebug.name != NULL)
                        stackItem = wxString::Format(_("function %s line %u"), name.c_str(), currentLine);
                    else
                        stackItem = wxString::Format(_("line %u"), currentLine);

                    wxLuaDebugDataItem *item = new wxLuaDebugDataItem(stackItem, wxT(""), wxT(""), source, nIndex, 0);
                    if (item != NULL)
                        pSortedList->Add(item);

                    fFirst = false;
                }
            }

            ++nIndex;
        }
        wxGetLuaHandler().LeaveLuaCriticalSection();
    }

    return m_debugService->NotifyStackEnumeration(pSortedList);
}

bool wxLuaDebuggee::EnumerateStackEntry(int stackRef)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData;
    if (pSortedList != NULL)
    {
        wxGetLuaHandler().EnterLuaCriticalSection();
        {
            lua_Debug luaDebug;
            if (lua_getstack(m_luaState, stackRef, &luaDebug) != 0)
            {
                int idx  = 1;
                wxString name = lua2wx(lua_getlocal(m_luaState, &luaDebug, idx));
                while (!name.IsEmpty())
                {
                    bool fIsTable = false;
                    wxString type;
                    wxString value;
                    wxString source = lua2wx(luaDebug.source);

                    switch (lua_type(m_luaState, -1))
                    {
                        case LUA_TNIL:
                            type  = wxT("Nil");
                            value = wxT("nil");
                            break;

                        case LUA_TBOOLEAN:
                            type = wxT("Boolean");
                            value = MakeBoolean(lua_toboolean(m_luaState, -1));
                            break;

                        case LUA_TNUMBER:
                            type = wxT("Number");
                            value = MakeNumber(lua_tonumber(m_luaState, -1));
                            break;

                        case LUA_TSTRING:
                            type  = wxT("String");
                            value = lua2wx(lua_tostring(m_luaState, -1));
                            break;

                        case LUA_TTABLE:
                            type  = wxT("Table");
                            value = GetTableInfo(-1);
                            fIsTable = true;
                            break;

                        case LUA_TFUNCTION:
                            if (lua_iscfunction(m_luaState, -1))
                            {
                                type  = wxT("C Function");
                                value.Printf(wxT("%p"), lua_tocfunction(m_luaState, -1));
                            }
                            else
                            {
                                type = wxT("Lua Function");
                                value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
                            }
                            break;

                        case LUA_TUSERDATA:
                            type  = wxT("User Data");
                            value = GetUserDataInfo(-1);
                            break;

                        case LUA_TLIGHTUSERDATA:
                            type  = wxT("Light User Data");
                            value = GetUserDataInfo(-1, false);
                            break;

                        case LUA_TTHREAD:
                            type = wxT("Thread");
                            value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
                            break;
                    }

                    int nRef = LUA_NOREF;

                    if (lua_istable(m_luaState, -1))
                    {
                        lua_pushvalue(m_luaState, -1);
                        nRef = tinsert(m_luaState, -1);
                        m_references.Add(nRef);
                    }

                    wxLuaDebugDataItem *item = new wxLuaDebugDataItem(name, type, value, source, nRef, 0);
                    if (item != NULL)
                        pSortedList->Add(item);

                    name = lua2wx(lua_getlocal(m_luaState, &luaDebug, ++idx));
                }
            }
        }
        wxGetLuaHandler().LeaveLuaCriticalSection();
    }

    return m_debugService->NotifyStackEntryEnumeration(stackRef, pSortedList);
}

bool wxLuaDebuggee::EnumerateTable(int tableRef, int nIndex, int nItemNode)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL)
    {
        wxGetLuaHandler().EnterLuaCriticalSection();
        {
            if (tableRef == -1)
            {
                lua_pushvalue(m_luaState, LUA_GLOBALSINDEX);
                int nRef = tinsert(m_luaState, -1);
                pSortedList->Add(new wxLuaDebugDataItem(wxT("Globals"),
                                               wxT(""),
                                               wxT(""),
                                               wxT(""),
                                               nRef,
                                               0));
                m_references.Add(nRef);
            }
            else
            {
                if (tget(m_luaState, tableRef) != 0)
                {
                    int nTop = lua_gettop(m_luaState);

                    // start iterating
                    lua_pushnil(m_luaState);
                    while (lua_next(m_luaState, nTop) != 0)
                    {
                        bool fIsTable = false;
                        wxString type;
                        wxString value;
                        wxString name;
                        wxString source;

                        // get the index
                        switch (lua_type(m_luaState, -2))
                        {
                            case LUA_TNIL:
                                name = wxT("Nil");
                                break;

                            case LUA_TBOOLEAN:
                                name = MakeBoolean(lua_toboolean(m_luaState, -2));
                                break;

                            case LUA_TNUMBER:
                                name = MakeNumber(lua_tonumber(m_luaState, -2));
                                break;

                            case LUA_TSTRING:
                                name = lua2wx(lua_tostring(m_luaState, -2));
                                break;

                            case LUA_TTABLE:
                                name = GetTableInfo(-2);
                                break;

                            case LUA_TFUNCTION:
                                if (lua_iscfunction(m_luaState, -2))
                                    name.Printf(wxT("%p"), lua_tocfunction(m_luaState, -2));
                                else
                                    name.Printf(wxT("%p"), lua_topointer(m_luaState, -2));
                                break;

                            case LUA_TUSERDATA:
                                name = GetUserDataInfo(-2);
                                break;

                            case LUA_TLIGHTUSERDATA:
                                name = GetUserDataInfo(-2, false);
                                break;

                            case LUA_TTHREAD:
                                name.Printf(wxT("%p"), lua_topointer(m_luaState, -2));
                                break;
                        }

                        // get the value
                        switch (lua_type(m_luaState, -1))
                        {
                            case LUA_TNIL:
                                type  = wxT("Nil");
                                value = wxT("nil");
                                break;

                            case LUA_TBOOLEAN:
                                value = MakeBoolean(lua_toboolean(m_luaState, -1));
                                type = wxT("Boolean");
                                break;

                            case LUA_TNUMBER:
                                value = MakeNumber(lua_tonumber(m_luaState, -1));
                                type = wxT("Number");
                                break;

                            case LUA_TSTRING:
                                type  = wxT("String");
                                value = lua2wx(lua_tostring(m_luaState, -1));
                                break;

                            case LUA_TTABLE:
                                type  = wxT("Table");
                                value = GetTableInfo(-1);
                                fIsTable = true;
                                break;

                            case LUA_TFUNCTION:
                                if (lua_iscfunction(m_luaState, -1))
                                {
                                    type  = wxT("C Function");
                                    value.Printf(wxT("%p"), lua_tocfunction(m_luaState, -1));
                                }
                                else
                                {
                                    type = wxT("Lua Function");
                                    value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
                                }
                                break;

                            case LUA_TUSERDATA:
                                type  = wxT("User Data");
                                value = GetUserDataInfo(-1);
                                break;

                            case LUA_TLIGHTUSERDATA:
                                type  = wxT("Light User Data");
                                value = GetUserDataInfo(-1, false);
                                break;

                            case LUA_TTHREAD:
                                type = wxT("Thread");
                                value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
                                break;
                        }

                        int nRef = LUA_NOREF;
                        if (fIsTable)
                        {
                            nRef = tinsert(m_luaState, -1);
                            m_references.Add(nRef);
                        }
                        else
                            lua_pop(m_luaState, 1);

                        wxLuaDebugDataItem *item = new wxLuaDebugDataItem(name, type, value, source, nRef, nIndex);
                        if (item != NULL)
                            pSortedList->Add(item);
                    }

                    // remove reference
                    lua_pop(m_luaState, 1);
                }
            }
        }
        wxGetLuaHandler().LeaveLuaCriticalSection();
    }

    return m_debugService->NotifyTableEnumeration(nItemNode, pSortedList);
}

bool wxLuaDebuggee::EvaluateExpr(int exprRef, const wxString &strExpr)
{
    wxString strResult(wxT("Error"));
    int      nReference = LUA_NOREF;

    wxGetLuaHandler().EnterLuaCriticalSection();
    {
        if (wxStrpbrk(strExpr.c_str(), wxT(" ~=<>+-*/%(){}[]:;,.\"'")) != NULL)
        {
            // an expression
            int nOldTop = lua_gettop(m_luaState);

            LuaCharBuffer charbuf(strExpr);
            int nResult = luaL_loadbuffer(m_luaState, charbuf.GetData(), charbuf.Length(), "debug");

            if (nResult == 0)
                nResult = lua_pcall(m_luaState, 0, LUA_MULTRET, 0);  /* call main */

            switch (nResult)
            {
                default:
                    if (lua_gettop(m_luaState) == nOldTop)
                    {
                        lua_pushliteral(m_luaState, "OK");
                    }
                    break;

                case LUA_ERRRUN:
                    lua_pushliteral(m_luaState, "Error while running the chunk");
                    break;

                case LUA_ERRSYNTAX:
                    lua_pushliteral(m_luaState, "Syntax error during pre-compilation");
                    break;

                case LUA_ERRMEM:
                    lua_pushliteral(m_luaState, "Memory allocation error");
                    break;

                case LUA_ERRERR:
                    lua_pushliteral(m_luaState, "Unexpected error occurred while running the error message handler");
                    break;
            }

            nReference = tinsert(m_luaState, -1);
            lua_settop(m_luaState, nOldTop);
        }
        else
        {
             lua_Debug ar;
             int       iLevel = 0;
             bool      fFound = false;

             while (lua_getstack(m_luaState, iLevel++, &ar) != 0)
             {
                int    iIndex = 0;
                wxString name = lua2wx(lua_getlocal(m_luaState, &ar, ++iIndex));
                while (!name.IsEmpty())
                {
                    if (strExpr == name)
                    {
                        nReference = tinsert(m_luaState, -1);
                        fFound = true;
                        break;
                    }
                    else
                        lua_pop(m_luaState, 1);
                }

                if (fFound)
                    break;

                name = lua2wx(lua_getlocal(m_luaState, &ar, ++iIndex));
             }

             if (!fFound)
             {
                  int nOldTop = lua_gettop(m_luaState);
                  lua_pushvalue(m_luaState, LUA_GLOBALSINDEX);
                  lua_pushnil(m_luaState);
                  while (lua_next(m_luaState, -2) != 0)
                  {
                      if (lua_type(m_luaState, -2) == LUA_TSTRING)
                      {
                          wxString name = lua2wx(lua_tostring(m_luaState, -2));
                          if (strExpr == name)
                          {
                              nReference = tinsert(m_luaState, -1); // reference value and pop it.
                              lua_pop(m_luaState, 1);   // pop index
                              fFound = true;
                              break;
                          }
                          else
                              lua_pop(m_luaState, 1);  // removes `value';
                      }
                      else
                          lua_pop(m_luaState, 1);  // removes `value';
                  }
                  lua_settop(m_luaState, nOldTop); // the table of globals.
             }
        }

        if (nReference != LUA_NOREF && tget(m_luaState, nReference) != 0)
        {
            tremove(m_luaState, nReference);
            switch (lua_type(m_luaState, -1))
            {
                case LUA_TNIL:
                    strResult = wxT("Nil");
                    break;

                case LUA_TBOOLEAN:
                    strResult = MakeBoolean(lua_toboolean(m_luaState, -1));
                    break;

                case LUA_TNUMBER:
                    strResult = MakeNumber(lua_tonumber(m_luaState, -1));
                    break;

                case LUA_TSTRING:
                    strResult = wxString::Format(wxT("\"%s\""), lua_tostring(m_luaState, -1));
                    break;

                case LUA_TTABLE:
                    strResult = wxT("table ") + GetTableInfo(-1);
                    break;

                case LUA_TFUNCTION:
                    if (lua_iscfunction(m_luaState, -1))
                        strResult = wxString::Format(wxT("C-function %p"), lua_tocfunction(m_luaState, -1));
                    else
                        strResult = wxString::Format(wxT("lua function %p"), lua_topointer(m_luaState, -1));
                    break;

                case LUA_TUSERDATA:
                    strResult = wxT("user data ") + GetUserDataInfo(-1);
                    break;

                case LUA_TLIGHTUSERDATA:
                    strResult = wxT("light user data ") + GetUserDataInfo(-1, false);
                    break;

                case LUA_TTHREAD:
                    strResult = wxString::Format(wxT("thread %p"), lua_topointer(m_luaState, -1));
                    break;

                default:
                    break;
            }

            lua_pop(m_luaState, 1);
        }
    }
    wxGetLuaHandler().LeaveLuaCriticalSection();

    return m_debugService->NotifyEvaluateExpr(exprRef, strResult);
}

wxString wxLuaDebuggee::GetTableInfo(int index)
{
    int         nItems = luaL_getn(m_luaState, index);
    const void *pItem  = lua_topointer(m_luaState, index);

    if (nItems == 0)
        return wxString::Format(wxT("%p"), pItem);

    return wxString::Format(wxT("%p (approx %u items)"), pItem, nItems);
}

wxString wxLuaDebuggee::GetUserDataInfo(int index, bool full)
{
    if (full)
    {
        int nTag = ttag(m_luaState, index);

        const char *pTagName = NULL;
        if (nTag != TLUA_NOTAG)
            pTagName = GetLuaTagName(m_luaState, nTag);

        if (pTagName != NULL)
            return wxString::Format(wxT("%p (%s)"), lua_touserdata(m_luaState, index), pTagName);

        return wxString::Format(wxT("%p (%u)"), lua_touserdata(m_luaState, index), nTag);
    }

    return wxString::Format(wxT("%p"), lua_touserdata(m_luaState, index));
}

wxString wxLuaDebuggee::MakeNumber(double dnum)
{
    long num = (long) dnum;
    if ((double) num == dnum)
    {
        if (num >= 0)
            return wxString::Format(wxT("%lu (0x%lx)"), num, num);

        return wxString::Format(wxT("%ld (0x%lx)"), num, num);
    }

    return wxString::Format(wxT("%g"), dnum);
}

wxString wxLuaDebuggee::MakeBoolean(int num)
{
    if (num)
        return wxT("True");

    return wxT("False");
}

wxFileName wxLuaDebuggee::GetDebugInfo(int &lineNumber) const
{
    lua_Debug luaDebug;
    lua_getstack(m_luaState, 0, &luaDebug);
    lua_getinfo(m_luaState, "Sln", &luaDebug);
    lineNumber = luaDebug.currentline - 1;

    wxFileName fileName;

    wxString source = lua2wx(luaDebug.source);
    if (!source.IsEmpty() && source.GetChar(0) == wxT('@'))
        fileName = source.Mid(1);
    else
        fileName = source;

#if wxCHECK_VERSION(2,3,0)
    fileName.Normalize();
#endif

    return fileName;
}

bool wxLuaDebuggee::AtBreakPoint(const wxFileName &fileName,
                                  int             lineNumber) const
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPointKey = LuaBreakPoint::GetBreakPointKey(fileName, lineNumber);

    LuaBreakPointList::Node* node = m_breakPointList.Find(breakPointKey);
    return (node && node->GetData()->GetEnabled());
}

int LUACALL wxLuaDebuggee::LuaPrint(lua_State *L)
{
    int      idx;
    wxString stream;
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    for (idx = 1; idx <= n;  idx++)
    {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, idx);   /* value to print */
        lua_call(L, 1, 1);
        wxString s = lua2wx(lua_tostring(L, -1));  /* get result */
        if (s.IsEmpty())
            return luaL_error(L, "`tostring' must return a string to `print'");
        if (idx > 1)
            stream.Append(wxT("\t"));
        stream.Append(s);
        lua_pop(L, 1);  /* pop result */
    }

    lua_getglobal(L, debugName);

    wxLuaDebuggee *pTarget = (wxLuaDebuggee *) lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (pTarget != NULL)
        pTarget->NotifyPrint(stream);

    return 0;
}

void wxLuaDebuggee::NotifyBreakPoints(wxLuaDebugSocket* debugSocket)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    // Notify Debugger of Each BreakPoint
    LuaBreakPointList::Node* nodeBreakPoint;
    LuaBreakPointList::Node* nextBreakPoint;

    for (nodeBreakPoint = m_breakPointList.GetFirst(); nodeBreakPoint; nodeBreakPoint = nextBreakPoint)
    {
        nextBreakPoint = nodeBreakPoint->GetNext();

        LuaBreakPoint* breakPoint = nodeBreakPoint->GetData();

        m_debugService->NotifyBreakpointAdded(*breakPoint, debugSocket);
    }
}
