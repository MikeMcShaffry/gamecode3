#pragma once
/////////////////////////////////////////////////////////////////////////////
// File CSoundProcess.h
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
//  Content References in Game Coding Complete 3rd Edition
// 
//  class SoundProcess					- Chapter 12, page 391
//  class ExplosionProcess				- Chapter 12, page 397
//  class CFadeProcess					- Chapter 12, page 400
//



/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "..\MainLoop\CProcess.h"

// Miles sound library has some level 4 warnings
//#pragma warning( push, 3 )
//#include "..\Libs\mss.h"
//#pragma warning( pop )

#include "CAudio.h"
#include "CSoundResource.h"

/////////////////////////////////////////////////////////////////////////////
// SoundProcess Description
//
//    A Sound Process, not to be confused with a Sound Resource (SoundResource)
//	  manages a sound as it is being played. You can use this class to manage
//    timing between sounds & animations.
//
/////////////////////////////////////////////////////////////////////////////

class SoundProcess : public CProcess  
{
public:
	SoundProcess(shared_ptr<SoundResHandle> soundResource, int typeOfSound=PROC_SOUNDFX, int volume=100, bool looping=false);
	
	virtual ~SoundProcess();

	virtual void	VOnUpdate(const int deltaMilliseconds);
	virtual void	VOnInitialize();
	virtual void	VKill();

	virtual void	VTogglePause();

	void Play(const int volume, const bool looping);
	void Stop();

	void			SetVolume(int volume);
	int				GetVolume();
	int				GetLengthMilli();
	bool			IsSoundValid() { return m_SoundResource!=NULL; }
	bool			IsPlaying();
	bool			IsLooping() { return m_AudioBuffer && m_AudioBuffer->VIsLooping(); }
	float			GetProgress();
	
protected:
	SoundProcess();	//Disable Default Construction

	void			InitializeVolume();
	void			Replay() { m_bInitialUpdate = true; };

	shared_ptr<SoundResHandle> m_SoundResource;		// this is the raw sound data
	shared_ptr<IAudioBuffer> m_AudioBuffer;			//handle to the implementation dependant audio buffer (DSound, Miles) 

	int				m_Volume;						// these hold the initial setting until the sound is actually launched.
	bool			m_isLooping;
};



/////////////////////////////////////////////////////////////////////////////
// ExplosionProcess Description
//
// This is an example of a process that uses a simple state machine
// to control itself. 
//
/////////////////////////////////////////////////////////////////////////////

class ExplosionProcess : public CProcess
{
  public:
  	ExplosionProcess() : CProcess(PROC_GAMESPECIFIC) { m_Stage=0; }
   
  protected:
  	int m_Stage;
  	shared_ptr<SoundProcess> m_Sound;
   
  	virtual void	VOnUpdate(const int deltaMilliseconds);
 	virtual void	VOnInitialize();
};

/////////////////////////////////////////////////////////////////////////////
// CFadeProcess Description
//
// Fades sound volume in or out over time and then kills itself.
// This should be useful for groups of sound effects, too - such as when
//   an AI barks and it must be heard above the other effects like too much freaking thunder.
//
/////////////////////////////////////////////////////////////////////////////

class CFadeProcess : public CProcess
{

protected:
	shared_ptr<SoundProcess> m_Sound;
	
	int m_TotalFadeTime;
	int m_ElapsedTime;
	int m_StartVolume;
	int m_EndVolume;
public:
	CFadeProcess(shared_ptr<SoundProcess> sound, int fadeTime, int endVolume);
	virtual void	VOnUpdate(const int deltaMilliseconds);	
};




