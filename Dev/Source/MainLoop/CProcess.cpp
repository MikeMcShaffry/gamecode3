//========================================================================
// CProcess.cpp : Defines a simple cooperative multitasker
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
// There are three classes implemented in this file:
//
// CProcess - a base class for processes attached to the cooperative multitasker.
//
// CProcessManager - a cooperative multitasker that manages a list of objects derived
//                   from CProcess, and calls OnUpdate() for each one each game loop.
//
// CWaitProcess - an example of a process you can use to time delay things
//========================================================================

//========================================================================
//  Content References in Game Coding Complete - 3rd Edition
// 
//  class CProcess			- Chapter 6, page 157-159
//  class CProcessManager	- Chapter 6, page 160-161
//  class CWaitProcess		- Chapter 6, page 162-163
//========================================================================


#include "GameCodeStd.h"
#include "CProcess.h"

//////////////////////////////////////////////////////////////
// CProcess constructor
//
CProcess::CProcess( int ntype, unsigned int uOrder ) :
	m_iType( ntype ),
	m_bKill( false ),
	m_bActive( true ),
	m_uProcessFlags( 0 ),
	m_bPaused( false ),
	m_bInitialUpdate( true )
{
}

//////////////////////////////////////////////////////////////
// CProcess destructor - 
//   Note: always call Kill(), never delete.
//
CProcess::~CProcess()
{
	// note - an explicit reset() of m_pNext is not necessary here....
}

//////////////////////////////////////////////////////////////
// CProcess::Kill() - marks the process for cleanup
// 
void CProcess::VKill()
{
	m_bKill=true;
}

//////////////////////////////////////////////////////////////
// CProcess::SetNext - sets a process dependancy
//
// A->SetNext(B)  means that B will wait until A is finished
//
void CProcess::SetNext(shared_ptr<CProcess> nnext)
{
	// note - an explicit reset() of m_pNext is not necessary here....

	m_pNext = nnext;
}


//////////////////////////////////////////////////////////////
// CProcess attachement methods
//
// IsAttached() - Is this process attached to the manager?
// SetAttached() - Marks it as attached. Called only by the manager.
//
bool CProcess::IsAttached() const
{
	return (m_uProcessFlags & PROCESS_FLAG_ATTACHED) ? true : false;
}

void CProcess::SetAttached(const bool wantAttached)
{
	if(wantAttached)
	{
		m_uProcessFlags |= PROCESS_FLAG_ATTACHED;
	}
	else
	{
		m_uProcessFlags &= ~PROCESS_FLAG_ATTACHED;
	}
}

//////////////////////////////////////////////////////////////
// CProcessManager::Attach - gets a process to run
//
void CProcessManager::Attach( shared_ptr<CProcess> pProcess )
{
	m_ProcessList.push_back(pProcess);
	pProcess->SetAttached(true);
}

//////////////////////////////////////////////////////////////
// CProcessManager::Detach 
//  - Detach a process from the process list, but don't delete it
//
void CProcessManager::Detach( shared_ptr<CProcess> pProcess )
{
	m_ProcessList.remove(pProcess);
	pProcess->SetAttached(false);
}

//////////////////////////////////////////////////////////////
// CProcessManager::IsProcessActive 
//  - Are there any active processes of this type?
//
bool CProcessManager::IsProcessActive( int nType )
{
	for(ProcessList::iterator i=m_ProcessList.begin(); i!=m_ProcessList.end(); ++i)
	{
		// Check for living processes.  If they are dead, make sure no children 
		// are attached as they will be brought to life on next cycle.
		if ( ( *i )->GetType() == nType && 
			 ( ( *i )->IsDead() == false || ( *i )->GetNext() ) )
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
// CProcessManager::HasProcesses 
//  - Are there any processes at all?
//
bool CProcessManager::HasProcesses()
{
	return !m_ProcessList.empty();
}

//////////////////////////////////////////////////////////////
// CProcessManager::~CProcessManager 
//  - run through the list of processes and detach them
//
CProcessManager::~CProcessManager()
{
	for(ProcessList::iterator i = m_ProcessList.begin(); i != m_ProcessList.end(); )
	{
		Detach(* (i++) );
	}
}

//////////////////////////////////////////////////////////////
// CProcessManager::UpdateProcesses
//  - run through the list of processes and update them
//
void CProcessManager::UpdateProcesses(int deltaMilliseconds)
{
	ProcessList::iterator i = m_ProcessList.begin();
	ProcessList::iterator end = m_ProcessList.end();
	shared_ptr<CProcess> pNext;

	while ( i != end )
	{
		shared_ptr<CProcess> p( *i );
		++i;

		if ( p->IsDead() )
		{
			// Check for a child process and add if exists
			pNext = p->GetNext();
			if ( pNext )
			{
				p->SetNext(shared_ptr<CProcess>((CProcess *)NULL));
				Attach( pNext );
			}
			Detach( p );
		}
		else if ( p->IsActive() && !p->IsPaused() )
		{
			p->VOnUpdate( deltaMilliseconds );
		}
	}
}


//////////////////////////////////////////////////////////////
// CWaitProcess
//  - wait a certain amount of time, then kill myself
//
CWaitProcess::CWaitProcess(unsigned int iNumMill ) :
	CProcess( PROC_WAIT, 0 ),
	m_uStart( 0 ),
	m_uStop( iNumMill )
{
}

void CWaitProcess::VOnUpdate( const int deltaMilliseconds )
{
	CProcess::VOnUpdate( deltaMilliseconds );

	if ( m_bActive )
	{
		m_uStart += deltaMilliseconds;
		
		if ( m_uStart >= m_uStop )
			VKill();
	}
}
