/////////////////////////////////////////////////////////////////////////////
// Name:            wxLuaSocket.h
// Purpose:         wxLua Socket interfaces
// Author:          J. Winwood
// Created:         March 2002
// Copyright:       (c) 2002 Lomtick Software. All rights reserved.
// Licence:         wxWidgets licence.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_SOCKET_H_
#define WX_LUA_SOCKET_H_

#ifdef __GNUG__
    #pragma interface "wxLuaSocket.h"
#endif

#ifdef WIN32
typedef int socklen_t;
#include <winsock.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <netdb.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#endif

#ifndef SD_RECEIVE
#define SD_RECEIVE      0
#define SD_SEND         1
#define SD_BOTH         2
#endif

// ----------------------------------------------------------------------------
// wxLuaSocket
// ----------------------------------------------------------------------------

class wxLuaSocket
{
#ifdef WIN32
    typedef SOCKET socket_type;
#else
    typedef int socket_type;
#endif

    // proxy class for accept
    class wxLuaAcceptedSocket
    {
        public:
            // constructor
            wxLuaAcceptedSocket(socket_type socket, sockaddr_in address);

            wxLuaAcceptedSocket(const wxLuaAcceptedSocket &);
            wxLuaAcceptedSocket& operator=(const wxLuaAcceptedSocket &);

        private:
            socket_type m_sock;
            sockaddr_in m_addr;

            friend class wxLuaSocket;
    };

public:

    enum SocketState
    {
        SOCKET_CLOSED,
        SOCKET_LISTENING,
        SOCKET_ACCEPTING,
        SOCKET_ACCEPTED,
        SOCKET_CONNECTED,
        SOCKET_WRITE_BLOCKED
    };

    // constructors
    wxLuaSocket();
    wxLuaSocket(const wxLuaAcceptedSocket &acceptedSocket);

    // destructor
    ~wxLuaSocket();

    // server: bind and listen for client connections
    void Listen(u_short port, int backLog = 100);

    // server: accept a connection from a client
    wxLuaAcceptedSocket Accept();

    // client: connect a client to a server
    void Connect(const wxString &address, u_short port);

    // Get the socket state
    SocketState GetState() const
    {
        return m_sockstate;
    }

    // Get the socket handle
    int GetSocket() const
    {
        return m_sock;
    }

    // Test if the socket is readable
    bool IsReadable () const
    {
        return (m_sockstate == SOCKET_CONNECTED ||
                m_sockstate == SOCKET_ACCEPTING);
    }

    // Test if the socket is writeable
    bool IsWritable () const
    {
        return (m_sockstate != SOCKET_CONNECTED ||
                m_sockstate == SOCKET_WRITE_BLOCKED);
    }

    // Get the address of the socket
    wxString GetAddress() const;
    // Get the port number of the socket
    int         GetPort() const;

    // Read the whole buffer of size length into buffer buffer from the socket
    int         Read(char *buffer, int length);

    // Write the whole buffer of size length to the socket
    void        Write(const char *buffer, int length);

    // Shutdown the socket
    void        Shutdown(int how);
    // Close the socket
    void        Close();

private:
    // Prevent copying and assignment of this class
    wxLuaSocket(const wxLuaSocket&);
    wxLuaSocket& operator=(const wxLuaSocket&);

    socket_type  m_sock;
    sockaddr_in  m_sockaddress;
    SocketState  m_sockstate;
};

// ----------------------------------------------------------------------------
// wxLuaSocketException
// ----------------------------------------------------------------------------

class wxLuaSocketException
{
public:
    enum SocketErrorCode { SOCKET_NOT_CLOSED,
                           SOCKET_NOT_CONNECTED,
                           SOCKET_NOT_LISTENING,
                           SOCKET_INVALID_SOCKET,
                           SOCKET_ADDRESS_NOT_RESOLVED,
                           SOCKET_INCOMPATIBLE_TYPE,
                           SOCKET_SET_BLOCKING_FAILED,
                           SOCKET_ACCEPT_FAILED,
                           SOCKET_CONNECT_FAILED,
                           SOCKET_BIND_FAILED,
                           SOCKET_LISTEN_FAILED,
                           SOCKET_SEND_FAILED,
                           SOCKET_RECEIVE_FAILED,
                           SOCKET_CLOSE_FAILED
                         };

    // constructors
    wxLuaSocketException(SocketErrorCode code) : m_code(code)
    {
#ifdef WIN32
        m_errnum = ::WSAGetLastError();
        switch(m_errnum)
        {
        case WSANOTINITIALISED: 
            m_description = _("A successful WSAStartup must occur before using this function."); 
            break;

        case WSAENETDOWN:
            m_description = _("The network subsystem or the associated service provider has failed.");
            break;

        case WSAEAFNOSUPPORT:
            m_description = _("The specified address family is not supported.");
            break;

        case WSAEINPROGRESS:
            m_description = _("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
            break;

        case WSAEMFILE:
            m_description = _("No more socket descriptors are available.");
            break;

        case WSAENOBUFS:
            m_description = _("No buffer space is available. The socket cannot be created.");
            break;

        case WSAEPROTONOSUPPORT:
            m_description = _("The specified protocol is not supported.");
            break;

        case WSAEPROTOTYPE:
            m_description = _("The specified protocol is the wrong type for this socket.");
            break;

        case WSAESOCKTNOSUPPORT:
            m_description = _("The specified socket type is not supported in this address family.");
            break;
        };
#else
        m_errnum = errno;
#endif
        if (m_description.IsEmpty())
            m_description = _("Socket Error");
    }

    wxLuaSocketException(const wxLuaSocketException &e)
        : m_code(e.m_code), m_errnum(e.m_errnum)
    {
    }

    SocketErrorCode GetCode() const
    {
        return m_code;
    }

    int errnum() const
    {
        return m_errnum;
    }

    const wxString& description() { return m_description; }

private:
    wxLuaSocketException& operator=(const wxLuaSocketException& e);

    SocketErrorCode m_code;
    int             m_errnum;
    wxString        m_description;
};

#endif // WX_LUA_SOCKET_H_
