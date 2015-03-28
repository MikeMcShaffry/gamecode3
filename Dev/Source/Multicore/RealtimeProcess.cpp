//========================================================================
// RealtimeProcess.cpp : Defines process that can run in a thread
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
//========================================================================

//========================================================================
//  Content References in Game Coding Complete - 3rd Edition
// 
//  class ProtectedProcess				- Chapter 18, page 668
//========================================================================


#include "GameCodeStd.h"
#include "RealtimeProcess.h"
#include "..\Multicore\SafeStream.h"

#include <boost/format.hpp>


DWORD g_maxLoops = 100000;
DWORD g_ProtectedTotal = 0;
CRITICAL_SECTION g_criticalSection;

DWORD WINAPI ThreadProc( LPVOID lpParam )
{
	DWORD maxLoops = *static_cast<DWORD *>(lpParam);
	DWORD dwCount = 0;
    while( dwCount < maxLoops )
    { 
        ++dwCount;

		EnterCriticalSection(&g_criticalSection);
		++g_ProtectedTotal;
		LeaveCriticalSection(&g_criticalSection);
	}
	return TRUE;
}

void CreateThreads()
{
	InitializeCriticalSection(&g_criticalSection); 

	for (int i=0; i<20; i++)
	{
		HANDLE m_hThread = CreateThread( 
					 NULL,       // default security attributes
					 0,          // default stack size
					 (LPTHREAD_START_ROUTINE) ThreadProc, 
					 &g_maxLoops,  // thread parameter is how many loops
					 0, // default creation flags
					 NULL);      // receive thread identifier
	}
}







RealtimeProcess::RealtimeProcess( LPTHREAD_START_ROUTINE lpRoutine, int priority )
	: CProcess(PROC_REALTIME)
{
	m_lpRoutine = lpRoutine;
	m_ThreadID = 0;
	m_ThreadPriority = priority;
}

//------------------------------------------------------------------------------------------------
// RealtimeProcess::VOnInitialize				- Chapter 18, page 667
//------------------------------------------------------------------------------------------------
void RealtimeProcess::VOnInitialize(void)
{
	CProcess::VOnInitialize();
    m_hThread = CreateThread( 
                 NULL,         // default security attributes
                 0,            // default stack size
                 (LPTHREAD_START_ROUTINE) m_lpRoutine, 
                 this,         // thread parameter is a pointer to the process
                 0,            // default creation flags
                 &m_ThreadID); // receive thread identifier

	if( m_hThread == NULL )
    {
		assert(0 && "Could not create thread!");
		VKill();
    }

	SetThreadPriority(m_hThread, m_ThreadPriority);
}



class ProtectedProcess : public RealtimeProcess
{
public:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
	static DWORD g_ProtectedTotal;
	static CRITICAL_SECTION g_criticalSection;
	DWORD m_MaxLoops;
	ProtectedProcess(DWORD maxLoops)
		: RealtimeProcess(ThreadProc) 
		{ m_MaxLoops = maxLoops; }
};

DWORD ProtectedProcess::g_ProtectedTotal = 0;
CRITICAL_SECTION ProtectedProcess::g_criticalSection;

DWORD WINAPI ProtectedProcess::ThreadProc( LPVOID lpParam )
{ 
	ProtectedProcess *proc = static_cast<ProtectedProcess *>(lpParam);
	DWORD dwCount = 0;

    while( dwCount < proc->m_MaxLoops )
    { 
        ++dwCount;

	    // Request ownership of critical section.
		EnterCriticalSection(&g_criticalSection);
		++g_ProtectedTotal;
		LeaveCriticalSection(&g_criticalSection);
    } 

	proc->VKill();
    return TRUE; 
}




class UnprotectedProcess : public RealtimeProcess
{
public:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
	static DWORD g_UnprotectedTotal;
	DWORD m_MaxLoops;
	UnprotectedProcess(DWORD maxLoops)
		: RealtimeProcess(ThreadProc) 
		{ m_MaxLoops = maxLoops; }
};

DWORD UnprotectedProcess::g_UnprotectedTotal = 0;

DWORD WINAPI UnprotectedProcess::ThreadProc( LPVOID lpParam )
{
	ProtectedProcess *proc = static_cast<ProtectedProcess *>(lpParam);
	DWORD dwCount = 0;
    while( dwCount < proc->m_MaxLoops )
    { 
		++dwCount;
		++g_UnprotectedTotal;
	}
	proc->VKill();
	return TRUE;
}



int g_ThreadCount = 20;
DWORD g_ThreadLoops = 100000;
bool g_Condom = true;

void testThreading(CProcessManager *procMgr)
{
    int i;

	InitializeCriticalSection(&ProtectedProcess::g_criticalSection); 
    // Create worker threads

	for( i=0; i < g_ThreadCount; i++ )
	{
		if (g_Condom)
		{
			shared_ptr<CProcess> proc(GCC_NEW ProtectedProcess(g_ThreadLoops));
			procMgr->Attach(proc);
		}
		else
		{
			shared_ptr<CProcess> proc(GCC_NEW UnprotectedProcess(g_ThreadLoops));
			procMgr->Attach(proc);
		}

	}
 }

 
// Solution 1
// (note: compiles with gcc 3.2, VC6)
#include "CriticalSection.h"
#include "SafeStream.h"
 

 
 
 
 
//////////////////////////////////////////////////////////
// Test
 
#include <stdio.h> 
//#include <iostream>
//#include <fstream>
//#include <iomanip>
 
 
const int THREADS_COUNT = 20;
const int WRITES_PER_THREAD = 500;
 
LONG nRemainingThreads = THREADS_COUNT;

 




#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"



ThreadSafeEventQueue g_RealtimeEventQueue;

class EventSenderProcess : public RealtimeProcess
{
public:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
	DWORD m_MaxLoops;
	EventSenderProcess(DWORD maxLoops)
		: RealtimeProcess(ThreadProc) 
		{ m_MaxLoops = maxLoops; }
};


DWORD WINAPI EventSenderProcess::ThreadProc( LPVOID lpParam )
{ 
	ProtectedProcess *proc = static_cast<ProtectedProcess *>(lpParam);
	DWORD dwCount = 0;

    // Request ownership of critical section.
    while( dwCount < proc->m_MaxLoops )
    { 
		IEventDataPtr e(GCC_NEW EvtData_Update_Tick(timeGetTime()));
		g_RealtimeEventQueue.push(e);
		dwCount++;
    } 

	proc->VKill();
    InterlockedDecrement( &nRemainingThreads);
    return TRUE; 
}


class EventReaderProcess : public RealtimeProcess
{
public:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
	EventReaderProcess() : RealtimeProcess(ThreadProc) { }
};

DWORD g_EventsRead = 0;

DWORD WINAPI EventReaderProcess::ThreadProc( LPVOID lpParam )
{ 
    // wait for all threads to end
    while ( true)
    {
		IEventDataPtr e;	
		if (g_RealtimeEventQueue.try_pop(e))
			++g_EventsRead;
		else
		{
			InterlockedIncrement( &nRemainingThreads);
			if ( InterlockedDecrement( &nRemainingThreads) == 0)
				break;
		}
    }

	EventReaderProcess *proc = static_cast<EventReaderProcess *>(lpParam);
	proc->VKill();
	return TRUE;
}


void testRealtimeEvents(CProcessManager *procMgr)
{
    for ( int idx = 0; idx < THREADS_COUNT; ++idx)
    {
		shared_ptr<CProcess> proc(GCC_NEW EventSenderProcess(g_ThreadLoops));
		procMgr->Attach(proc);
	}
 	shared_ptr<CProcess> proc(GCC_NEW EventReaderProcess());
	procMgr->Attach(proc);
}

#include "..\ResourceCache\ZipFile.h"

//
// class RealtimeEventListener				- Chapter 18, page 672
//
class RealtimeEventListener : public IEventListener
{
	ThreadSafeEventQueue m_RealtimeEventQueue;
public:
	char const* GetName(void) { return "RealtimeEventListener"; }
	virtual bool HandleEvent( IEventData const & event )
	{
		IEventDataPtr pEvent = event.VCopy();
		g_RealtimeEventQueue.push(pEvent);	
		return true;
	};
};

//
// class DecompressionProcess				- Chapter 18, page 674
//
class DecompressionProcess : public RealtimeProcess
{
protected:

public:
	EventListenerPtr m_pListener;
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
	static void Callback(int progress, bool &cancel);

	DecompressionProcess();
};


DecompressionProcess::DecompressionProcess()
  :  RealtimeProcess(ThreadProc)
{
	m_pListener = EventListenerPtr (GCC_NEW RealtimeEventListener);
	safeAddListener( m_pListener, EvtData_Decompress_Request::sk_EventType );
}

DWORD WINAPI DecompressionProcess::ThreadProc( LPVOID lpParam )
{
	DecompressionProcess *proc = static_cast<DecompressionProcess *>(lpParam);

	while (1)
	{
		// check the queue for events we should consume
		IEventDataPtr e;	
		if (g_RealtimeEventQueue.try_pop(e))
		{
			// there's an event! Something to do....
			if (EvtData_Decompress_Request::sk_EventType == e->VGetEventType())
			{
				shared_ptr<EvtData_Decompress_Request> decomp = boost::static_pointer_cast<EvtData_Decompress_Request>(e);

				ZipFile zipFile;

				bool success = FALSE;

				if (zipFile.Init(decomp->m_zipFileName.c_str()))
				{
					int size = 0;
					optional<int> resourceNum = zipFile.Find(decomp->m_fileName.c_str());
					if (resourceNum.valid())
					{
						size = zipFile.GetFileLen(*resourceNum);
						void *buffer = GCC_NEW char[size];
						zipFile.ReadFile(*resourceNum, buffer);

						// send decompression result event
//						threadSafeQueEvent(IEventDataPtr ( GCC_NEW EvtData_Decompression_Progress (100, decomp->m_zipFileName, decomp->m_fileName, buffer) ) );
					}
				}
			}
		}
		else
		{
			Sleep(10);
		}
	}

	proc->VKill();
	return TRUE;
}


void testRealtimeDecompression(CProcessManager *procMgr)
{
	static void *buffer = NULL;
	SAFE_DELETE (buffer);

	shared_ptr<CProcess> proc(GCC_NEW DecompressionProcess());
	procMgr->Attach(proc);

	safeQueEvent(IEventDataPtr ( GCC_NEW  EvtData_Decompress_Request(L"big.zip", "big.dat")));
}




