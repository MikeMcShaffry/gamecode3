#if !defined(AFX_SYNCHSOUNDPROCESS_H__DFFEB32E_6268_4B20_8E9B_DC3E8DDEE84E__INCLUDED_)
#define AFX_SYNCHSOUNDPROCESS_H__DFFEB32E_6268_4B20_8E9B_DC3E8DDEE84E__INCLUDED_
/////////////////////////////////////////////////////////////////////////////
// File SynchSoundProcess.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "CSoundProcess.h"


class SyncSoundProcess : public CProcess
{
	IPacResource m_ipacsound;
	int m_type;
	bool m_loop;
	CScreen *m_screen;
	//CSoundResource *m_soundResource;

public:
	CSoundProcess* m_pSoundProc;

public:
	SyncSoundProcess(
		CScreen* screen, 
		IPacResource &nipacsound, 
		int typeOfSound=PROC_SOUNDVOICE,
		bool bLoop = false );

	/*
	SyncSoundProcess(
		CScreen* screen, 
		CSoundResource *soundResource, 
		int typeOfSound=PROC_SOUNDVOICE,
		bool bLoop = false );
	*/

	virtual ~SyncSoundProcess();

	virtual void	OnUpdate(const int deltaMilliseconds); 
};

#endif // !defined(AFX_SYNCHSOUNDPROCESS_H__DFFEB32E_6268_4B20_8E9B_DC3E8DDEE84E__INCLUDED_)
