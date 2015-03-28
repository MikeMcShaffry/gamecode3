// Part of the GameCode3 Application
//
// GameCode3 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 3rd Edition" by Mike McShaffry, published by
// Charles River Media. ISBN-10: 1-58450-680-6   ISBN-13: 978-1-58450-680-5
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the author a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: 
//    http://www.amazon.com/gp/product/1584506806?ie=UTF8&tag=gamecodecompl-20&linkCode=as2&camp=1789&creative=390957&creativeASIN=1584506806
//
// There's a companion web site at http://www.mcshaffry.com/GameCode/
// 
// The source code is managed and maintained through Google Code: http://gamecode3.googlecode.com/svn/trunk/
//
// (c) Copyright 2009 Michael L. McShaffry
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License v2
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================




#include "GameCodeStd.h"
#include "..\GameCode.h"

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "Network.h"
#include "../EventManager/Events.h"
#include "../TeapotWars/TeapotEvents.h"			// GAH - Game specific dependancy! Create a factory later...
#include "../ai/AiEvents.h"
#include "../DumbStuff/String.h"

#pragma comment(lib, "Ws2_32")

#define EXIT_ASSERT assert(0);

const char *BinaryPacket::g_Type = "BinaryPacket";
const char *TextPacket::g_Type = "TextPacket";


BaseSocketManager *g_pSocketManager = NULL;

/******************************************************

void testCode()
{
	// Use of utility and conversion functions.
	unsigned long ipAddress = inet_addr("128.64.16.2");

	struct in_addr addr;
	addr.S_un.S_addr = htonl(0x88482818);

	char ipAddressString[16];
	strcpy(ipAddressString, inet_ntoa(addr));

	char buffer[256];
	sprintf(buffer, "0x%08x 0x%08x %s\n:", ipAddress, addr.S_un.S_addr, ipAddressString);
	OutputDebugStringA(buffer);


	// Use of DNS functions
	struct hostent *pHostEnt; 
	const char *host = "ftp.microsoft.com";
	pHostEnt = gethostbyname(host);

    if (pHostEnt == NULL)
    {
        fprintf(stderr, "No such host");
    }
	else
	{
	    struct sockaddr_in addr;
		memcpy(&addr.sin_addr,pHostEnt->h_addr,pHostEnt->h_length);

		char buffer[256];
		sprintf(buffer, "Address of %s is 0x%08x\n", host, ntohl(addr.sin_addr.s_addr));
		OutputDebugStringA(buffer);
	}

	unsigned int netip = inet_addr("207.46.133.140");
	pHostEnt = gethostbyaddr((const char *)&netip, 4, PF_INET);
}

******************************************************/


//
// TextPacket::TextPacket			- Chapter 16, page 607
//
TextPacket::TextPacket(char const * const text)
 :BinaryPacket(static_cast<u_long>(strlen(text) + 2))
{ 
	MemCpy(text, strlen(text), 0);
	MemCpy("\r\n", 2, 2);
	*(u_long *)m_Data = 0;
}


//-------------------------------------------------------------------
// NetSocket Implementation

//
// NetSocket::NetSocket				- Chapter 16, page 591
//
NetSocket::NetSocket() 
{ 
	m_sock = INVALID_SOCKET;
	m_deleteFlag = 0;
	m_sendOfs = 0;
	m_timeOut = 0;

	m_recvOfs = m_recvBegin = 0;
	m_internal = 0;
	m_bBinaryProtocol = 1;
}


//
// NetSocket::NetSocket				- Chapter 16, page 591
//
NetSocket::NetSocket(SOCKET new_sock, unsigned int hostIP)
{
	m_sock = INVALID_SOCKET;
	m_deleteFlag = 0;
	m_sendOfs = 0;
	m_timeOut = 0;

	m_bBinaryProtocol = 1;

	m_recvOfs = m_recvBegin = 0;
	m_internal = 0;

	m_timeCreated = timeGetTime();

	m_sock = new_sock;
	m_ipaddr = hostIP;

	m_internal = g_pSocketManager->IsInternal(m_ipaddr);

	setsockopt (m_sock, SOL_SOCKET, SO_DONTLINGER, NULL, 0);

	if (m_ipaddr)
	{
		TCHAR buffer[128];
		const char *ansiIpaddress = g_pSocketManager->GetHostByAddr(m_ipaddr);
		if (ansiIpaddress)
		{
			TCHAR genIpaddress[64];
			AnsiToGenericCch(genIpaddress, ansiIpaddress, static_cast<int>(strlen(ansiIpaddress)+1));
			_tcssprintf(buffer, _T("User connected: %s %s"), genIpaddress, (m_internal) ? _T("(internal)") : _T(""));
			OutputDebugString(buffer);
		}
	}
}


//
// NetSocket::~NetSocket				- Chapter 16, page 593
//
NetSocket::~NetSocket() 
{
	if (m_sock != INVALID_SOCKET) 
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
 }



//
// NetSocket::Connect				- Chapter 16, page 593
//
bool NetSocket::Connect(unsigned int ip, unsigned int port, int fCoalesce) 
{
	struct sockaddr_in sa;
	int x = 1;

	if ((m_sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return false;

	if (!fCoalesce)
	{
		setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&x, sizeof(x));
	}

	//	setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&x, sizeof(x));

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(ip);
	sa.sin_port = htons(port);

	if (connect(m_sock, (struct sockaddr *)&sa, sizeof(sa))) 
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return false;
	}

	return true;
 }

//
// NetSocket::Send							- Chapter 16, page 594
//
void NetSocket::Send(shared_ptr<IPacket> pkt, bool clearTimeOut)
{
	if (clearTimeOut)
		m_timeOut = 0;

	m_OutList.push_back(pkt);
}

//
// NetSocket::SetBlocking				- Chapter 16, page 594
//
void NetSocket::SetBlocking(int block) 
{
	#ifdef WIN32
		unsigned long val = block ? 0 : 1;
		ioctlsocket(m_sock, FIONBIO, &val);
	#else
		int val = fcntl(m_sock, F_GETFL, 0);
		if (block)
			val &= ~(O_NONBLOCK);
		else
			val |= O_NONBLOCK;

		fcntl(m_sock, F_SETFL, val);
	#endif
}


//
// NetSocket::HandleOutput				- Chapter 16, page 595
//
void NetSocket::HandleOutput() 
{
	int fSent = 0;
	do 
	{
		assert(!m_OutList.empty());
		PacketList::iterator i = m_OutList.begin();

		shared_ptr<IPacket> pkt = *i;
		const char *buf = pkt->VGetData();
		int len = static_cast<int>(pkt->VGetSize());

		int rc = send(m_sock, buf+m_sendOfs, len-m_sendOfs, 0);
		if (rc > 0) 
		{
			g_pSocketManager->m_Outbound += rc;
			m_sendOfs += rc;
			fSent = 1;
		}
		else if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			HandleException();
			fSent = 0;
		}
		else
		{
			fSent = 0;
		}

		if (m_sendOfs == pkt->VGetSize()) 
		{
			m_OutList.pop_front();
			m_sendOfs = 0;
		}

	} while ( fSent && !m_OutList.empty() );
}



//
// NetSocket::HandleInput				- Chapter 16, page 596
//
void NetSocket::HandleInput() 
{
	bool bPktRecieved = false;
	u_long packetSize = 0;
	int rc = recv(m_sock, m_recvBuf+m_recvBegin+m_recvOfs, RECV_BUFFER_SIZE-m_recvOfs, 0);

	char metrics[1024];
	sprintf(metrics, "Incoming: %6d bytes. Begin %6d Offset %4d\n", rc, m_recvBegin, m_recvOfs);
	OutputDebugStringA(metrics);

	if (rc==0)
	{
		return;
	}

	if (rc < 0)
	{
		m_deleteFlag = 1;
		return;
	}

	const int hdrSize = sizeof(u_long);
	unsigned int newData = m_recvOfs + rc;
	int processedData = 0;

	while (newData > hdrSize)
	{
		// There are two types of packets at the lowest level of our design:
		// BinaryPacket - Sends the size as a positive 4 byte integer
		// TextPacket - Sends 0 for the size, the parser will search for a CR

		packetSize = *(reinterpret_cast<u_long*>(m_recvBuf+m_recvBegin));
		packetSize = ntohl(packetSize);

		if (m_bBinaryProtocol)
		{
			// we don't have enough new data to grab the next packet
			if (newData < packetSize)
				break;

			if (packetSize > MAX_PACKET_SIZE)
			{
				// prevent nasty buffer overruns!
				HandleException();
				return;
			}

			if (newData >= packetSize)
			{
				// we know how big the packet is...and we have the whole thing
				char test[1024];
				memcpy(test, &m_recvBuf[m_recvBegin+hdrSize], packetSize);
				test[packetSize+1]='\r';
				test[packetSize+2]='\n';
				test[packetSize+3]=0;
				OutputDebugStringA(test);
				shared_ptr<BinaryPacket> pkt(GCC_NEW BinaryPacket(&m_recvBuf[m_recvBegin+hdrSize], packetSize-hdrSize));
				m_InList.push_back(pkt);
				bPktRecieved = true;
				processedData += packetSize;
				newData -= packetSize;
				m_recvBegin += packetSize;
			}
		}
		else
		{
			// the text protocol waits for a carraige return and creates a string
			char *cr = static_cast<char *>(memchr(&m_recvBuf[m_recvBegin], 0x0a, rc));
			if (cr)
			{
				*(cr+1) = 0;
				shared_ptr<TextPacket> pkt(GCC_NEW TextPacket(&m_recvBuf[m_recvBegin]));
				m_InList.push_back(pkt);
				packetSize = cr - &m_recvBuf[m_recvBegin];
				bPktRecieved = true;

				processedData += packetSize;
				newData -= packetSize;
				m_recvBegin += packetSize;
			}
		}
	}

	g_pSocketManager->m_Inbound += rc;
	m_recvOfs = newData;

	if (bPktRecieved)
	{
		if (m_recvOfs == 0)
		{
			m_recvOfs = 0;
			m_recvBegin = 0;
		}
		else if (m_recvBegin + m_recvOfs + MAX_PACKET_SIZE > RECV_BUFFER_SIZE)
		{
			// we don't want to overrun the buffer - so we copy the leftover bits 
			// to the beginning of the recieve buffer and start over
			int leftover = m_recvOfs;
			memcpy(m_recvBuf, &m_recvBuf[m_recvBegin], m_recvOfs);
			m_recvBegin = 0;
		}
	}	
}


//-------------------------------------------------------------------
// NetListenSocket Implementation



NetListenSocket::NetListenSocket(int portnum) 
{
	port = 0;
	Init(portnum);
}


//
// NetListenSocket::Init				- Chapter 16, page 599
//
void NetListenSocket::Init(int portnum)
{
	struct sockaddr_in sa;
	int x = 1;

	if ((m_sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		perror("NetListenSocket::Init: socket");
		EXIT_ASSERT
		exit(1);
	}

	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x))== SOCKET_ERROR) 
	{
		perror("NetListenSocket::Init: setsockopt");
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		EXIT_ASSERT
		exit(1);
	}
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = ADDR_ANY;
	sa.sin_port = htons(portnum);

	// bind to port
	if (bind(m_sock, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR) 
	{
		perror("NetListenSocket::Init: bind");
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		EXIT_ASSERT
		exit(1);
	}

	// set nonblocking - accept() blocks under some odd circumstances otherwise
	SetBlocking(0);

	// start listening
	if (listen(m_sock, 256) == SOCKET_ERROR) 
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		EXIT_ASSERT
		exit(1);
	}

	port = portnum;
}

//
// NetListenSocket::InitScan			- added post press
//   Opens multiple ports to listen for connections.
//
void NetListenSocket::InitScan(int portnum_min, int portnum_max) 
{
	struct sockaddr_in sa;
	int portnum, x = 1;

	if ((m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		EXIT_ASSERT
		exit(1);
	}

	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x)) == SOCKET_ERROR) 
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		EXIT_ASSERT
		exit(1);
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	for (portnum = portnum_min; portnum < portnum_max; portnum++) 
	{
		sa.sin_port = htons(portnum);
		// bind to port
		if (bind(m_sock, (struct sockaddr *)&sa, sizeof(sa)) != SOCKET_ERROR)
			break;
	}

	if (portnum == portnum_max) 
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		EXIT_ASSERT
		exit(1);
	}

	// set nonblocking - accept() blocks under some odd circumstances otherwise
	SetBlocking(0);

	// start listening
	if (listen(m_sock, 8) == SOCKET_ERROR) 
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		EXIT_ASSERT
		exit(1);
	}

	port = portnum;
 }

//
// NetListenSocket::AcceptConnection				- Chapter 16, page 600
//
SOCKET NetListenSocket::AcceptConnection(unsigned int *pAddr)
{
	SOCKET new_sock;
	struct sockaddr_in sock;
	int size;

	size = sizeof(sock);

	if ((new_sock = accept(m_sock, (struct sockaddr *)&sock, &size))== INVALID_SOCKET)
		return INVALID_SOCKET;

	if (getpeername(new_sock, (struct sockaddr *)&sock, &size) == SOCKET_ERROR)
	{
		closesocket(new_sock);
		return INVALID_SOCKET;
	}
	*pAddr = ntohl(sock.sin_addr.s_addr);
	return new_sock;
 }




//
// BaseSocketManager::BaseSocketManager				- Chapter 16, page 603
//
BaseSocketManager::BaseSocketManager() 
{ 
	m_Inbound = 0;
	m_Outbound = 0;
	m_MaxOpenSockets = 0;
	m_SubnetMask = 0;
	m_Subnet = 0xffffffff;
	m_NextSocketId = 0;

	g_pSocketManager = this; 
	ZeroMemory(&m_WsaData, sizeof(WSADATA)); 
}


//
// BaseSocketManager::Init						- Chapter 16, page 603
//
bool BaseSocketManager::Init()
{
	int errorCode = WSAStartup(0x0202, &m_WsaData);
	if (errorCode==0)
		return true;
	else
	{
		assert(0 && "WSAStartup failure!");
		return false;
	}
}


//
// BaseSocketManager::Shutdown					- Chapter 16, page 604
//
void BaseSocketManager::Shutdown()
{
	// Get rid of all those pesky kids...
	while (!m_SockList.empty())
	{
		delete *m_SockList.begin();
		m_SockList.pop_front();
	}

	WSACleanup();
}

//
// BaseSocketManager::AddSocket					- Chapter 16, page 604
//
int BaseSocketManager::AddSocket(NetSocket *socket) 
{ 
	socket->m_id = m_NextSocketId;
	m_SockList.push_front(socket); 
	
	int openSockets = static_cast<int>(m_SockList.size());
	
	if (openSockets > m_MaxOpenSockets)
		++m_MaxOpenSockets;

	m_SockMap[m_NextSocketId] = socket;
	++m_NextSocketId; 

	return socket->m_id; 
}

//
// BaseSocketManager::RemoveSocket					- Chapter 16, page 604
//
void BaseSocketManager::RemoveSocket(NetSocket *socket) 
{ 
	m_SockList.remove(socket); 
	m_SockMap.erase(socket->m_id);
	SAFE_DELETE(socket);
}

//
// BaseSocketManager::FindSocket					- Chapter 16, page 605
//
NetSocket *BaseSocketManager::FindSocket(int sockId)
{
	SocketIdMap::iterator i = m_SockMap.find(sockId);
	if (i==m_SockMap.end())
	{
		return NULL;
	}

	return (*i).second;
}



int BaseSocketManager::GetIpAddress(int sockId)
{
	NetSocket *socket = FindSocket(sockId);
	if (socket)
	{
		return socket->GetIpAddress();
	}
	else
	{
		return 0;
	}
}



//
// BaseSocketManager::Send							- Chapter 16, page 605
//
bool BaseSocketManager::Send(int sockId, shared_ptr<IPacket> packet)
{
	NetSocket *sock = FindSocket(sockId);
	if (!sock)
		return false;
	sock->Send(packet);
	return true;
}

//
// BaseSocketManager::DoSelect					- Chapter 16, page 605
//
void BaseSocketManager::DoSelect(int pauseMicroSecs, int handleInput) 
{
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = pauseMicroSecs;    // 100 microseconds is 0.1 milliseconds or .0001 seconds

	fd_set inp_set, out_set, exc_set;
	int maxdesc;
	NetSocket *pSock;

	FD_ZERO(&inp_set);
	FD_ZERO(&out_set);
	FD_ZERO(&exc_set);

	maxdesc = 0;

  // set everything up for the select
	for (SocketList::iterator i = m_SockList.begin(); i != m_SockList.end(); ++i)
	{
		pSock = *i;
		if ((pSock->m_deleteFlag&1) || pSock->m_sock == INVALID_SOCKET)
			continue;

		if (handleInput)
			FD_SET(pSock->m_sock, &inp_set);

		FD_SET(pSock->m_sock, &exc_set);

		if (pSock->HasOutput())
			FD_SET(pSock->m_sock, &out_set);

		if ((int)pSock->m_sock > maxdesc)
			maxdesc = (int)pSock->m_sock;

	 }
  
	int selRet = 0;

	// do the select (duration passed in as tv, NULL to block until event)
	selRet = select(maxdesc+1, &inp_set, &out_set, &exc_set, &tv) ;
	if (selRet == SOCKET_ERROR)
	{
		PrintError();
		return;
	}

	// handle input, output, and exceptions

	if (selRet)
	{
		for (SocketList::iterator i = m_SockList.begin(); i != m_SockList.end(); ++i)
		{
			pSock = *i;

			if ((pSock->m_deleteFlag&1) || pSock->m_sock == INVALID_SOCKET)
				continue;

			if (FD_ISSET(pSock->m_sock, &exc_set))
			{
				pSock->HandleException();
			}

			if (!(pSock->m_deleteFlag&1) && FD_ISSET(pSock->m_sock, &out_set))
			{
				pSock->HandleOutput();
			}

			if (   handleInput
				&& !(pSock->m_deleteFlag&1) && FD_ISSET(pSock->m_sock, &inp_set))
			{
				pSock->HandleInput();
			}
		 }	
	}

	unsigned int timeNow = timeGetTime();

	// handle deleting any sockets
	for (SocketList::iterator i = m_SockList.begin(); i != m_SockList.end(); ++i)
	{
		pSock = *i;
		if (pSock->m_timeOut) 
		{
			if (pSock->m_timeOut < timeNow)
			{
				pSock->TimeOut();
			}
		}

		if (pSock->m_deleteFlag&1) 
		{
			switch (pSock->m_deleteFlag) 
			{
			  case 1:
//					--i;
					g_pSocketManager->RemoveSocket(pSock);
					break;
				case 3:
					pSock->m_deleteFlag = 2;
					if (pSock->m_sock != INVALID_SOCKET) 
					{
						closesocket(pSock->m_sock);
						pSock->m_sock = INVALID_SOCKET;
					}
					break;
			}
		}

	 }
 }


//
// BaseSocketManager::IsInternal					- Chapter 16, page 609
//
bool BaseSocketManager::IsInternal(unsigned int ipaddr)
{
	bool internal = false;
	if (m_SubnetMask)
	{
	    unsigned int hostSubnet = ipaddr & m_SubnetMask;
		if (hostSubnet == m_Subnet)
		{
			internal = 1;
		}
	}
	return internal;
}


//
// BaseSocketManager::GetHostByName					- Chapter 16, page 609
//
unsigned int BaseSocketManager::GetHostByName(const std::string &hostName)
{
	struct hostent *pHostEnt; 
    struct sockaddr_in tmpSockAddr; //placeholder for the ip address

    //This will retrieve the ip details and put it into pHostEnt structure
	pHostEnt = gethostbyname(hostName.c_str());

    if(pHostEnt == NULL)
    {
        assert(0 && _T("Error occured"));
        return 0;
    }

    memcpy(&tmpSockAddr.sin_addr,pHostEnt->h_addr,pHostEnt->h_length);
	return ntohl(tmpSockAddr.sin_addr.s_addr);
}

//
// BaseSocketManager::GetHostByAddr					- Chapter 16, page 609
//
const char *BaseSocketManager::GetHostByAddr(unsigned int ip)
{
	static char host[32];

	int netip = htonl(ip);
	LPHOSTENT lpHostEnt = gethostbyaddr((const char *)&netip, 4, PF_INET);

	if (lpHostEnt)
	{
		strcpy(host, lpHostEnt->h_name);
		return host;
	}

	return NULL;
}

//
// BaseSocketManager::PrintError					- not in the book
//
void BaseSocketManager::PrintError()
{
	int realError = WSAGetLastError();
	TCHAR *reason;

	switch(realError)
	{
		case WSANOTINITIALISED: reason = _T("A successful WSAStartup must occur before using this API."); break;
		case WSAEFAULT: reason = _T("The Windows Sockets implementation was unable to allocated needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space."); break;
		case WSAENETDOWN: reason = _T("The network subsystem has failed."); break;
		case WSAEINVAL: reason = _T("The timeout value is not valid, or all three descriptor parameters were NULL."); break;
		case WSAEINTR: reason = _T("The (blocking) call was canceled via WSACancelBlockingCall."); break;
		case WSAEINPROGRESS: reason = _T("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."); break;
		case WSAENOTSOCK: reason = _T("One of the descriptor sets contains an entry which is not a socket."); break;
		default: reason = _T("Unknown."); 
	}

	TCHAR buffer[256];
	_tcssprintf(buffer, _T("SOCKET error: %s\n"), reason);
	OutputDebugString(buffer);
}


//
// ClientSocketManager::Connect					- Chapter 16, page 610
//
bool ClientSocketManager::Connect()
{
	if (!BaseSocketManager::Init())
		return false;

	RemoteEventSocket *pSocket = GCC_NEW RemoteEventSocket;
	
	if (!pSocket->Connect(GetHostByName(m_HostName), m_Port) )
	{
		SAFE_DELETE(pSocket);
		return false;
	}
	AddSocket(pSocket);
	return true;
}



//
// GameServerListenSocket::HandleInput				- Chapter 16, page 629
//
void GameServerListenSocket::HandleInput() 
{
	SOCKET new_sock;
	unsigned int theipaddr;

	new_sock = AcceptConnection(&theipaddr);

	int x = 1;
	setsockopt(new_sock, SOL_SOCKET, SO_DONTLINGER, (char *)&x, sizeof(x));

	if (new_sock != INVALID_SOCKET)
	{
		RemoteEventSocket * sock = GCC_NEW RemoteEventSocket(new_sock, theipaddr);
		int sockId = g_pSocketManager->AddSocket(sock);
		int ipAddress = g_pSocketManager->GetIpAddress(sockId);
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Remote_Client( sockId, ipAddress ) ) );
	}
 }







//
// RemoteEventSocket::HandleInput				- Chapter 16, page 615
//
void RemoteEventSocket::HandleInput()
{
	NetSocket::HandleInput();

	// traverse the list of m_InList packets and do something useful with them
	while (!m_InList.empty())
	{
		shared_ptr<IPacket> packet = *m_InList.begin();
		m_InList.pop_front();
		if (!strcmp(packet->VGetType(), BinaryPacket::g_Type))
		{
			const char *buf = packet->VGetData();
			int size = static_cast<int>(packet->VGetSize());

			std::istrstream in(buf+sizeof(u_long), (size-sizeof(u_long)));
			
			int type;
			in >> type;
			switch(type)
			{
				case NetMsg_Event:
					CreateEvent(in);
					break;

				case NetMsg_PlayerLoginOk:
				{
					int serverSockId, actorId;
					in >> serverSockId;
					in >> actorId;
					safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Network_Player_Actor_Assignment( actorId, serverSockId ) ) );
					break;
				}

				default:
					assert(0 && _T("Unknown message type."));
			}
		}
		else if (!strcmp(packet->VGetType(), TextPacket::g_Type))
		{
			OutputDebugStringA(packet->VGetData()+sizeof(u_long));
		}
	}
}


//
// RemoteEventSocket::CreateEvent				- Chapter 16, page 616
//
void RemoteEventSocket::CreateEvent(std::istrstream &in)
{
	// Note:  We can improve the efficiency of this by comparing the hash values instead of strings.
	// But strings are easier to debug!
	char eventType[256];
	in >> eventType;

	// This really needs to get rewritten with real streaming code.
	// Once that is done all the game specific events can be moved from Events.h & .cpp
	// back into TeapotEvents.h & .cpp.

	if ( EvtData_New_Game::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_New_Game( in ) ) );
	}
	else if ( EvtData_Game_State::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Game_State( in ) ) );
	}
	else if ( EvtData_New_Actor::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_New_Actor( in ) ) );
	}
	else if ( EvtData_Move_Actor::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Move_Actor( in ) ) );
	}
	else if ( EvtData_Destroy_Actor::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Destroy_Actor( in ) ) );
	}
	else if ( EvtData_Fire_Weapon::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Fire_Weapon( in ) ) );
	}
	else if ( EvtData_Thrust::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Thrust( in ) ) );
	}
	else if ( EvtData_Steer::sk_EventType.getStr() == eventType )
	{
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Steer( in ) ) );
	}
	else if (EvtData_AiSteer::sk_EventType.getStr() == eventType)
	{
		safeQueEvent(IEventDataPtr(GCC_NEW EvtData_AiSteer(in)));
	}
	else
	{
		char debugMessage[256];
		sprintf(debugMessage, "ERROR Unknown event type from remote: %s\n", eventType);
		OutputDebugStringA(debugMessage);
	}
}



//
// NetworkEventForwarder::HandleEvent			- Chapter 16, page 617
//
bool NetworkEventForwarder::HandleEvent( IEventData const & event )
{
	std::ostrstream out;

	out << static_cast<int>(RemoteEventSocket::NetMsg_Event) << " ";
	out << event.VGetEventType().getStr() << " ";
	event.VSerialize(out);
	out << "\r\n";

	shared_ptr<BinaryPacket> eventMsg(GCC_NEW BinaryPacket(out.rdbuf()->str(), out.pcount()));

	g_pSocketManager->Send(m_sockId, eventMsg);
	return true;
}


//
// NetworkGameView::VOnAttach					- Chapter 16, page 619
//
void NetworkGameView::VOnAttach(GameViewId viewId, optional<ActorId> aid)
{

	m_ViewId = viewId; 
	m_PlayerActorId = aid;

	// this is the first thing that happens when the 
	// network view is attached. The socket id is sent, 
	// which is how each client can be uniquely identified from other
	// clients attached to the server.

	std::ostrstream out;

	out << static_cast<int>(RemoteEventSocket::NetMsg_PlayerLoginOk) << " ";
	out << m_SockId << " ";
	out << *m_PlayerActorId << " ";
	out << "\r\n";

	shared_ptr<BinaryPacket> gvidMsg(GCC_NEW BinaryPacket(out.rdbuf()->str(), out.pcount()));
	g_pSocketManager->Send(m_SockId, gvidMsg);
}

