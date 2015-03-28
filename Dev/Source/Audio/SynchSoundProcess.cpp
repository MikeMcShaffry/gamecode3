/////////////////////////////////////////////////////////////////////////////
// File SynchSoundProcess.cpp
//
// Copyright (C) 2001 by Compulsive Development, LLC, All Rights Reserved.
//
/////////////////////////////////////////////////////////////////////////////
// Description:
//
// class SyncSoundProcess - allows for one sound process to wait until another
//    sound process of the same type is completed before beginning to play
//
//////////////////////////////////////////////////////////////////////

#include "GameGlobals.h"
#include "SynchSoundProcess.h"
#include "CasinoApp.h"
//#include "CSoundResource.h"


/////////////////////////////////////////////////////////////////////////////
// DEBUG info
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// SyncSoundProcess 
//////////////////////////////////////////////////////////////////////

SyncSoundProcess::SyncSoundProcess(
	CScreen* screen,
	IPacResource &ipacsound, 
	int typeOfSound, 
	bool bLoop ) :
	CProcess(PROC_SYNC),
	m_ipacsound(ipacsound),
	m_type( typeOfSound ),
	m_loop( bLoop ),
	m_screen( screen ),
	//BEGIN KIS(3-20-02) | Jokerz 1614 : SyncSoundProcess was treating itself as game sound when its supposed to be dealer sound
	//This happened because member variables are not initilized in the order that they are in the init list
	m_pSoundProc(new CSoundProcess(screen, ipacsound, typeOfSound))
	//m_pSoundProc(new CSoundProcess(m_screen, m_ipacsound, m_type))
	//END KIS(3-20-02) | Jokerz 1614
{
	m_pSoundProc->AddRef();

	// Kill if no sound
	if ( gApp.GetVolume( m_type ) == 0 )
	{
		Kill();
	}
}

/*
SyncSoundProcess::SyncSoundProcess(
	CScreen* screen, 
	CSoundResource *soundResource, 
	int typeOfSound,
	bool bLoop ) :
	CProcess(PROC_SYNC),
	m_pSoundProc(NULL)
{
	m_screen = screen;
	m_soundResource = soundResource;
	m_type = typeOfSound;
	m_loop = bLoop;
	m_ipacsound = IPacResource();

	// Kill if no sound
	if ( gApp.GetVolume( m_type ) == 0 )
	{
		Kill();
	}
}
*/

SyncSoundProcess::~SyncSoundProcess()
{
	SAFE_RELEASE( m_pSoundProc );
}

void SyncSoundProcess::OnUpdate(const int delta)
{
	// Call base
	CProcess::OnUpdate(delta);

	if ( ! m_bKill )
	{
		bool readyToPlay = !m_screen->IsProcessActive(m_type);
		//The dealer processes have a special case in which they must be polite to other avatars
		if(m_type==PROC_SOUNDDEALER)
		{
			readyToPlay = readyToPlay && !m_screen->IsProcessActive(PROC_SOUNDVOICE);
		}

		if(readyToPlay)
		{
			/*
			SAFE_RELEASE( m_pSoundProc );
			if (m_soundResource)
			{
				m_pSoundProc = new CSoundProcess(m_screen, m_soundResource, m_type);
			}
			else
			{
				m_pSoundProc = new CSoundProcess(m_screen, m_ipacsound, m_type);
			}
			*/

			// Hand to screen
			m_screen->Attach(m_pSoundProc);
			
			// All done with it...
			SAFE_RELEASE( m_pSoundProc );
			Kill();
		}
	}
}






