/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxSocket and network functions
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxSocket

///////////////////////////////////////////////////////////////////////////////
// wxSocketBase

%include "wx/socket.h"

%enum wxSocketError
    wxSOCKET_NOERROR
    wxSOCKET_INVOP
    wxSOCKET_IOERR
    wxSOCKET_INVADDR
    wxSOCKET_INVSOCK
    wxSOCKET_NOHOST
    wxSOCKET_INVPORT
    wxSOCKET_WOULDBLOCK
    wxSOCKET_TIMEDOUT
    wxSOCKET_MEMERR
%end

%enum wxSocketFlags
    wxSOCKET_NONE
    wxSOCKET_NOWAIT
    wxSOCKET_WAITALL
    wxSOCKET_BLOCK
%end

// %enum wxSocketEventFlags %end

%enum wxSocketNotify
    wxSOCKET_INPUT
    wxSOCKET_OUTPUT
    wxSOCKET_CONNECTION
    wxSOCKET_LOST
%end

%class wxSocketBase, wxObject
    wxSocketBase()
    void Close()
    bool Destroy()
    void Discard()
    bool Error() const
    wxObject * GetClientData() const
    bool GetLocal(wxSockAddress& addr) const
    wxSocketFlags GetFlags() const
    bool GetPeer(wxSockAddress& addr) const
    void InterruptWait()
    bool IsConnected() const
    bool IsData() const
    bool IsDisconnected() const
    unsigned long LastCount() const
    %gtk wxSocketError LastError() const // causes link error with Borland C++ w/DLL not exported?
    %mac wxSocketError LastError() const // causes link error with Borland C++ w/DLL not exported?
    void Notify(bool notify)
    bool Ok() const
    void RestoreState()
    void SaveState()
    void SetClientData(wxObject *data)
    %property=ClientData, read, write
    void SetFlags(wxSocketFlags flags)
    %property=Flags, read, write
    void SetNotify(wxSocketEventFlags flags)
    void SetTimeout(int seconds)
    void Peek(void * buffer, unsigned long nbytes)
    void Read(void * buffer, unsigned long nbytes)
    void ReadMsg(void * buffer, unsigned long nbytes)
    void Unread(const void * buffer, unsigned long nbytes)
    bool Wait(long seconds = -1, long millisecond = 0)
    bool WaitForLost(long seconds = -1, long millisecond = 0)
    bool WaitForRead(long seconds = -1, long millisecond = 0)
    bool WaitForWrite(long seconds = -1, long millisecond = 0)
    void Write(const void * buffer, unsigned long nbytes)
    void WriteMsg(const void * buffer, wxUint32 nbytes)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSocketClient

%class wxSocketClient, wxSocketBase
    wxSocketClient(wxSocketFlags flags = wxSOCKET_NONE)
    bool Connect(wxSockAddress& address, bool wait = TRUE)
    bool WaitOnConnect(long seconds = -1, long milliseconds = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSocketClient

%class wxSocketServer, wxSocketBase
    wxSocketServer(wxSockAddress& address, wxSocketFlags flags = wxSOCKET_NONE)
    wxSocketBase * Accept(bool wait = TRUE)
    bool AcceptWith(wxSocketBase& socket, bool wait = TRUE)
    bool WaitForAccept(long seconds = -1, long millisecond = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSocketEvent

%class %delete wxSocketEvent, wxEvent
    %define %event wxEVT_SOCKET

    wxSocketEvent(int id = 0)
    wxObject * GetClientData()
    %property=ClientData, read
    wxSocketBase * GetSocket() const
    %property=Socket, read
    wxSocketNotify GetSocketEvent() const
    %property=SocketEvent, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxSockAddress

%class wxSockAddress, wxObject
    void Clear()
    //int  SockAddrLen() // Does not exist
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxIPV4address

%class wxIPV4address, wxSockAddress
    wxIPV4address()
    %rename SetHostname bool Hostname(const wxString& hostname)
    %rename SetHostnameAddr bool Hostname(unsigned long addr)
    wxString Hostname()
    %rename SetService bool Service(const wxString& service)
    %rename SetServicePort bool Service(unsigned short service)
    unsigned short Service()
    bool AnyAddress()
    bool LocalHost()
%endclass

%endif wxLUA_USE_wxSocket

