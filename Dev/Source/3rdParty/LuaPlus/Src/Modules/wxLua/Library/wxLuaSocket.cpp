/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaSocket.cpp
// Purpose:     Socket class for wxLua.
// Author:      J. Winwood
// Created:     February 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaSocket.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxLuaSocket.h"

#ifdef _MSC_VER
    #pragma warning(push, 4)
#endif

extern const wxCharBuffer wx2lua(const wxString& AppString);
extern wxString lua2wx(const char * wx2lua);

// ----------------------------------------------------------------------------
// class wxLuaSocket
// ----------------------------------------------------------------------------

// Default socket constructor.
wxLuaSocket::wxLuaSocket()
    : m_sockstate(SOCKET_CLOSED)
{
}

// Socket constructor from an accepted socket
wxLuaSocket::wxLuaSocket(const wxLuaSocket::wxLuaAcceptedSocket &acceptedSocket)
    : m_sock(acceptedSocket.m_sock), 
      m_sockaddress(acceptedSocket.m_addr), 
      m_sockstate(SOCKET_ACCEPTED)
{
}

// Accepted socket constructor
wxLuaSocket::wxLuaAcceptedSocket::wxLuaAcceptedSocket(socket_type sock, sockaddr_in addr)
    : m_sock(sock), 
      m_addr(addr)
{
}

// socket destructor: close the socket if not already closed.
wxLuaSocket::~wxLuaSocket()
{
    if (m_sockstate != SOCKET_CLOSED)
    {
#ifdef WIN32
        ::closesocket(m_sock);
#else
        ::close(m_sock);
#endif
    }
}

// Create a listening socket, using the specified port number
void wxLuaSocket::Listen(u_short portNumber, int backLog)
{
    if (m_sockstate != SOCKET_CLOSED)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_CLOSED);

    m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
    
    if (m_sock == INVALID_SOCKET)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_INVALID_SOCKET);

    sockaddr_in localAddr = { 0 };

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(portNumber);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(m_sock, (sockaddr *) &localAddr, sizeof(localAddr)) == SOCKET_ERROR)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_BIND_FAILED);   

    if (::listen(m_sock, backLog) == SOCKET_ERROR)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_LISTEN_FAILED);

    memset(&m_sockaddress, 0, sizeof(m_sockaddress));
    m_sockstate = SOCKET_LISTENING;
}

// Accept a connection, returning an accepted socket.
wxLuaSocket::wxLuaAcceptedSocket wxLuaSocket::Accept()
{
    if (m_sockstate != SOCKET_LISTENING)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_LISTENING);

    sockaddr_in fromAddr = { 0 };
    socklen_t length = sizeof(fromAddr);

    socket_type acceptedSocket = ::accept(m_sock, (sockaddr *)&fromAddr, &length);
    if (acceptedSocket == INVALID_SOCKET)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_ACCEPT_FAILED);

    return wxLuaAcceptedSocket(acceptedSocket, fromAddr);
}

// Connect to a given host and port number
void wxLuaSocket::Connect(const wxString &addr, u_short portNumber)
{
    hostent *pHost = NULL;

    if (m_sockstate != SOCKET_CLOSED)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_CLOSED);

    m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == INVALID_SOCKET)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_INVALID_SOCKET);

    unsigned long  address = ::inet_addr(wx2lua(addr));    
    if (address != INADDR_NONE)
        pHost = ::gethostbyaddr((const char*) &address, 4, AF_INET);
    else
        pHost = ::gethostbyname(wx2lua(addr));

    if (pHost == NULL)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_ADDRESS_NOT_RESOLVED);

    if (pHost->h_addrtype != AF_INET)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_INCOMPATIBLE_TYPE);

    memset(&m_sockaddress, 0, sizeof(m_sockaddress));
    memcpy(&(m_sockaddress.sin_addr), pHost->h_addr_list[0], pHost->h_length);
    
    m_sockaddress.sin_family = AF_INET;
    m_sockaddress.sin_port   = htons(portNumber);

    if (::connect(m_sock, (sockaddr *) &m_sockaddress, sizeof(m_sockaddress)) == SOCKET_ERROR)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_CONNECT_FAILED);

    m_sockstate = SOCKET_CONNECTED;
}

// Get the address of an open socket
wxString wxLuaSocket::GetAddress() const
{
    if (m_sockstate != SOCKET_CONNECTED && m_sockstate != SOCKET_ACCEPTED)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_CONNECTED);

    return lua2wx(inet_ntoa(m_sockaddress.sin_addr));
}

// Get the port number of an open socket
int wxLuaSocket::GetPort() const
{
    if (m_sockstate != SOCKET_CONNECTED && m_sockstate != SOCKET_ACCEPTED)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_CONNECTED);

    return ntohs(m_sockaddress.sin_port);
}

// Write data to an open socket, repeat until all data has been sent.
void wxLuaSocket::Write(const char *buffer, int length)
{
    if (m_sockstate != SOCKET_CONNECTED && m_sockstate != SOCKET_ACCEPTED)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_CONNECTED);

    while (length > 0)
    {
        int numWritten = ::send(m_sock, buffer, length, 0);
        if (numWritten == SOCKET_ERROR)
            throw wxLuaSocketException(wxLuaSocketException::SOCKET_SEND_FAILED);

        length -= numWritten;
        buffer += numWritten;
    }
}

// Read data from an open socket, repeat reading until all data has been read
int wxLuaSocket::Read(char *buffer, int length)
{
    if (m_sockstate != SOCKET_CONNECTED && m_sockstate != SOCKET_ACCEPTED)
        throw wxLuaSocketException(wxLuaSocketException::SOCKET_NOT_CONNECTED);

    while (length > 0)
    {
        int numRead = ::recv(m_sock, buffer, length, 0);
        if (numRead == 0)
            return length;

        if (numRead == SOCKET_ERROR)
            throw wxLuaSocketException(wxLuaSocketException::SOCKET_RECEIVE_FAILED);

        length -= numRead;
        buffer += numRead;
    }
    return 0;
}

// Shutdown a socket in an orderly fashion
void wxLuaSocket::Shutdown(int how)
{
    if (m_sockstate != SOCKET_CLOSED)
    {
        ::shutdown(m_sock, how);
    }
}

// Close an open socket
void wxLuaSocket::Close()
{
    if (m_sockstate != SOCKET_CLOSED)
    {
#ifdef WIN32
        if (::closesocket(m_sock) == SOCKET_ERROR)
            throw wxLuaSocketException(wxLuaSocketException::SOCKET_CLOSE_FAILED);
#else
        if (::close(m_sock))
            throw wxLuaSocketException(wxLuaSocketException::SOCKET_CLOSE_FAILED);
#endif
        m_sockstate = SOCKET_CLOSED;
    }
}
