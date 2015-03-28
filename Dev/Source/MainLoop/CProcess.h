#pragma once
//========================================================================
// CProcess.h : Defines a simple cooperative multitasker
//
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
//  Content References in Game Coding Complete - 3rd Edition
// 
//  class CProcess			- Chapter 6, page 157-159
//  class CProcessManager	- Chapter 6, page 160-161
//  class CWaitProcess		- Chapter 6, page 162-163
//========================================================================


#include <list>

//////////////////////////////////////////////////////////////////////
// Enums
//////////////////////////////////////////////////////////////////////

// This process type enumeration is obviously subject to changes
// based on the game design, for example, we are assuming the game will
// have separate behaviors for voice, music, and sound effects
// when in actuality, this engine will play all sound processes the same way

enum PROCESS_TYPE
{
	PROC_NONE,
	PROC_WAIT,
	PROC_CONTROL,
	PROC_SCREEN,
	PROC_MUSIC,
	PROC_SOUNDFX,
	PROC_ACTOR,
	PROC_INTERPOLATOR,
	// These AI state proc id's shouldn't live here!  Game-specific stuff in the engine is bad!  I promise I'll move them....
	PROC_AISTATE_ATTACK,
	PROC_AISTATE_CHASETARGET,
	PROC_AISTATE_WANDER,
	PROC_AISTATE_SPIN,
	PROC_AISTATE_WAIT,
	PROC_GAMESPECIFIC,

	PROC_REALTIME,		// added for Chapter 18!
};


//////////////////////////////////////////////////////////////////////
// Flags
//////////////////////////////////////////////////////////////////////

static const int PROCESS_FLAG_ATTACHED		= 0x00000001;


//////////////////////////////////////////////////////////////////////
// CProcess Description
//
// This is a base class whose instantiations attach to the CProcessManager,
// which updates them once per game loop. Useful for creating animations,
// game objects that need ticking, etc.
//
//////////////////////////////////////////////////////////////////////

class CProcess : public boost::noncopyable
{
	friend class CProcessManager;

protected:
	int					m_iType;		// type of process running
	bool				m_bKill;		// tells manager to kill and remove
	bool				m_bActive;
	bool				m_bPaused;
	bool				m_bInitialUpdate;	// initial update?
	shared_ptr<CProcess>	m_pNext;
	
private:
	unsigned int	m_uProcessFlags;

public:
	CProcess(int ntype, unsigned int uOrder = 0);
	virtual ~CProcess();	

public:

	bool			IsDead(void) const { return(m_bKill);};
	virtual void	VKill();
	
	int				GetType(void) const { return(m_iType); };
	void			SetType(const int t) { m_iType = t; };

	bool			IsActive(void) const { return m_bActive; };
	void			SetActive(const bool b) { m_bActive = b; };
	bool			IsAttached()const;
	void			SetAttached(const bool wantAttached);

	bool			IsPaused(void) const { return m_bPaused; };
	virtual void	VTogglePause() {m_bPaused = !m_bPaused;}	// call to pause a process
	
	bool			IsInitialized()const { return ! m_bInitialUpdate; };

	shared_ptr<CProcess> const GetNext(void) const { return(m_pNext);}
	void			SetNext(shared_ptr<CProcess> nnext);
	
	virtual void			VOnUpdate(const int deltaMilliseconds);
	virtual void			VOnInitialize(){};
};

inline void CProcess::VOnUpdate( const int deltaMilliseconds )
{
	if ( m_bInitialUpdate )
	{
		VOnInitialize();
		m_bInitialUpdate = false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ProcessList Description
//
// ProcessList is a list of smart CProcess pointers.
//
//////////////////////////////////////////////////////////////////////

typedef std::list<shared_ptr<CProcess> > ProcessList;


/////////////////////////////////////////////////////////////////////////////
// CProcessManager Description
//
// CProcessManager is a container for CProcess objects.
//
//////////////////////////////////////////////////////////////////////

class CProcessManager
{
public:
	void UpdateProcesses(int deltaMilliseconds);
	bool IsProcessActive( int nType );
	void Attach( shared_ptr<CProcess> pProcess );
	bool HasProcesses();

	~CProcessManager();

protected:
	ProcessList	m_ProcessList;	

private:
	void Detach( shared_ptr<CProcess> pProcess );
};


//////////////////////////////////////////////////////////////////////
// CWaitProcess Description
//
// This process forces a delayed execution of a dependant process.
//
// CWaitProcess Example
//   CWaitProcess *wait = new CWaitProcess(2000);
//	 MyOtherProcess *other = new MyOtherProcess;
//   wait->SetNext(other);
// 
//////////////////////////////////////////////////////////////////////

class CWaitProcess : public CProcess
{
protected:
	unsigned int	m_uStart;
	unsigned int	m_uStop;

public:
	CWaitProcess(unsigned int iNumMill );
	virtual void VOnUpdate(const int deltaMilliseconds);
};



