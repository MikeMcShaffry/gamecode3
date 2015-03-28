#pragma once
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

//========================================================================
//  Content References in Game Coding Complete 3rd Edition
// 
//  class IPacket					- Chapter 16, page 587
//  class BinaryPacket				- Chapter 16, page 588
//  class TextPacket				- Chapter 16, page 589
//  class NetSocket					- Chapter 16, page 590
//  class NetListenSocket			- Chapter 16, page 599
//  class BaseSocketManager			- Chapter 16, page 601
//  class ClientSocketManager		- Chapter 16, page 610
//  class GameServerListenSocket	- Chapter 16, page 612
//  class RemoteEventSocket			- Chapter 16, page 614
//  class NetworkEventForwarder		- Chapter 16, page 617
//  class NetworkGameView			- Chapter 16, page 618


#include <sys/types.h>
#include <Winsock2.h>
#include "..\EventManager\EventManager.h"


#define MAX_PACKET_SIZE (256)
#define RECV_BUFFER_SIZE (MAX_PACKET_SIZE * 512)
#define MAX_QUEUE_PER_PLAYER (10000)

#define MAGIC_NUMBER		(0x1f2e3d4c)
#define IPMANGLE(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|((d)))

class NetSocket;

typedef std::list<NetSocket *> SocketList;
typedef std::map<int, NetSocket *> SocketIdMap;


////////////////////////////////////////////////////
//
// IPacket Description
//
//    The interface class that defines a public API for 
//    packet objects - data that is either about to be
//    sent to or just been recieved from the network
//
////////////////////////////////////////////////////

class IPacket
{
public:
	virtual char const * const VGetType() const=0;
	virtual char const * const VGetData() const=0;
	virtual u_long VGetSize() const =0;
	virtual ~IPacket() { }
};

typedef std::list< shared_ptr <IPacket> > PacketList;


////////////////////////////////////////////////////
//
// BinaryPacket Description
//
//   A packet object that can be constructed all at once,
//   or with repeated calls to MemCpy
////////////////////////////////////////////////////

class BinaryPacket : public IPacket
{
protected:
	char *m_Data;

public:
	inline BinaryPacket(char const * const data, u_long size);
	inline BinaryPacket(u_long size);
	virtual ~BinaryPacket() { SAFE_DELETE(m_Data); }
	virtual char const * const VGetType() const { return g_Type; }
	virtual char const * const VGetData() const { return m_Data; }
	virtual u_long VGetSize() const { return ntohl(*(u_long *)m_Data); }
	inline void MemCpy(char const *const data, size_t size, int destOffset);

	static const char *g_Type;
};


inline BinaryPacket::BinaryPacket(char const * const data, u_long size)
{
	// BinaryPacket::BinaryPacket		- Chapter 16, page 606
	m_Data = GCC_NEW char[size + sizeof(u_long)];
	assert(m_Data);
	*(u_long *)m_Data = htonl(size+sizeof(u_long));
	memcpy(m_Data+sizeof(u_long), data, size);
}

inline BinaryPacket::BinaryPacket(u_long size)
{
	// BinaryPacket::BinaryPacket		- Chapter 16, page 607
	m_Data = GCC_NEW char[size + sizeof(u_long)];
	assert(m_Data);
	*(u_long *)m_Data = htonl(size+sizeof(u_long));
}

inline void BinaryPacket::MemCpy(char const *const data, size_t size, int destOffset)
{
	// BinaryPacket::MemCpy				- Chapter 16, page 607
	assert(size+destOffset <= VGetSize()-sizeof(u_long));
	memcpy(m_Data + destOffset + sizeof(u_long), data, size);
}

////////////////////////////////////////////////////
//
// TextPacket Description
//
//   A packet object that takes a text string.
////////////////////////////////////////////////////

class TextPacket : public BinaryPacket
{
public:
	TextPacket(char const * const text);
	virtual char const * const VGetType() const { return g_Type; }

	static const char *g_Type;
};


////////////////////////////////////////////////////
//
// NetSocket Description
//
//   A base class for a socket connection.
////////////////////////////////////////////////////

class NetSocket 
{
	friend class BaseSocketManager;

public:
	NetSocket();
	NetSocket(SOCKET new_sock, unsigned int hostIP);
	virtual ~NetSocket();

	virtual void HandleInput();
	virtual void HandleOutput();
	virtual int  HasOutput() { return !m_OutList.empty(); }
	virtual void TimeOut() { m_timeOut=0; }

	bool Connect(unsigned int ip, unsigned int port, int fCoalesce = 0);

	void HandleException() { m_deleteFlag |= 1; }
	void SetBlocking(int block);
	void Send(shared_ptr<IPacket> pkt, bool clearTimeOut=1);

	void SetTimeOut(int ms=45*1000) { m_timeOut = timeGetTime() + ms; }

	int GetIpAddress() { return m_ipaddr; }


protected:
    SOCKET m_sock;
	int m_id;				// a unique ID given by the socket manager

	// note: if deleteFlag has bit 2 set, exceptions only close the
	//   socket and set to INVALID_SOCKET, and do not delete the NetSocket
    int m_deleteFlag;
 
	PacketList m_OutList;
	PacketList m_InList;

	char m_recvBuf[RECV_BUFFER_SIZE];
	unsigned int m_recvOfs, m_recvBegin;
	bool m_bBinaryProtocol;

    int m_sendOfs;
	unsigned int m_timeOut;
	unsigned int m_ipaddr;

	int m_internal;
	int m_timeCreated;
};



class BaseSocketManager
{
	friend class NetSocket;

protected:
	WSADATA m_WsaData;

	SocketList m_SockList;
	SocketIdMap m_SockMap;

	int m_NextSocketId;
	int m_Inbound;
	int m_Outbound;
	int m_MaxOpenSockets;
	unsigned int m_SubnetMask;
	unsigned int m_Subnet;

	NetSocket *FindSocket(int sockId);

public:
	BaseSocketManager();
	virtual ~BaseSocketManager() { Shutdown(); }

	void DoSelect(int pauseMicroSecs, int handleInput = 1);

	bool Init();
	void Shutdown();
	void PrintError();

	int AddSocket(NetSocket *socket); 
	void RemoveSocket(NetSocket *socket);

	unsigned int GetHostByName(const std::string &hostName);
	const char *GetHostByAddr(unsigned int ip);

	int GetIpAddress(int sockId);

	void SetSubnet(unsigned int subnet, unsigned int subnetMask)
	{
		m_Subnet = subnet;
		m_SubnetMask = subnetMask;
	}
	bool IsInternal(unsigned int ipaddr);

	bool Send(int sockId, shared_ptr<IPacket> packet);
};

extern BaseSocketManager *g_pSocketManager;




class ClientSocketManager : public BaseSocketManager
{
	std::string m_HostName;
	unsigned int m_Port;

public:
	ClientSocketManager(const std::string &hostName, unsigned int port)
	{
		m_HostName = hostName;
		m_Port = port;
	}

	bool Connect();
};






class NetListenSocket: public NetSocket
{
public:
	NetListenSocket() { };
	NetListenSocket(int portnum);

	void Init(int portnum);
	void InitScan(int portnum_min, int portnum_max);
	SOCKET AcceptConnection(unsigned int *pAddr);

	unsigned short port;
};




class GameServerListenSocket: public NetListenSocket 
{
public:
	GameServerListenSocket(int portnum) { Init(portnum); }

	void HandleInput();
};


class RemoteEventSocket: public NetSocket 
{
public:
	enum
	{
		NetMsg_Event,
		NetMsg_PlayerLoginOk,
	};

	// server accepting a client
	RemoteEventSocket(SOCKET new_sock, unsigned int hostIP)		
	: NetSocket(new_sock, hostIP)
	{
	}

	// client attach to server
	RemoteEventSocket() { };										

	virtual void HandleInput();

protected:
	void CreateEvent(std::istrstream &in);
};




// The only thing a network game view does is 
// act as an intermediary between the server and the 'real' remote view
// it listens to the same messages as a a game view
// and sends them along via TCP/IP

class NetworkEventForwarder : public IEventListener
{
public:
	// IEventListener
	NetworkEventForwarder(int sockId) { m_sockId = sockId; }
	bool HandleEvent( IEventData const & event );
	char const * GetName(void) { return "NetworkEventForwarder"; }

protected:
	int m_sockId;
};

class NetworkGameView : public IGameView
{

public:
	// IGameView Implementation - everything is stubbed out.
	virtual HRESULT VOnRestore() { return S_OK; }
	virtual void VOnRender(double fTime, float fElapsedTime) { }
	virtual void VOnLostDevice() { }
	virtual GameViewType VGetType() { return GameView_Remote; }
	virtual GameViewId VGetId() const { return m_ViewId; }
	virtual void VOnAttach(GameViewId vid, optional<ActorId> aid);
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg ) { return 0; }
	virtual void VOnUpdate( int deltaMilliseconds ) { };

	void SetPlayerActorId(ActorId actorId) { m_PlayerActorId = actorId; }

	NetworkGameView(int sockId)
	{ 
		m_SockId = sockId;
	}

protected:
	GameViewId m_ViewId;
	optional<ActorId> m_PlayerActorId;
	int m_SockId;
};










