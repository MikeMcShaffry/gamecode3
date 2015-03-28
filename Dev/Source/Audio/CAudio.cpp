//========================================================================
// Audio.cpp : Defines a simple sound system.
//
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


#include "GameCodeStd.h"

#include <mmsystem.h>
#include <mmreg.h>

#include "CAudio.h"
#include "CSoundResource.h"

#pragma comment( lib, "dsound" )

//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

Audio *g_pAudio = NULL;
char *gSoundExtentions[] = { ".mp3", ".wav", ".midi", ".ogg" };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Audio::Audio():
	m_Initialized(false),
	m_AllPaused(false)
{
}

//
// Audio::VShutdown			- Chapter 12, page 377
//
void Audio::VShutdown()
{
	IAudioBuffer *audioBuffer = NULL;
	AudioBufferList::iterator i=m_AllSamples.begin();

	while (i!=m_AllSamples.end())
	{
		audioBuffer = (*i);
		audioBuffer->VStop();
		m_AllSamples.pop_front();
	}
}

//
// Audio::VPauseAllSounds			- Chapter 12, page 377
//   Pause all active sounds, including music
//
void Audio::VPauseAllSounds()
{
	IAudioBuffer *audioBuffer = NULL;

	AudioBufferList::iterator i;
	AudioBufferList::iterator end;
	for(i=m_AllSamples.begin(), end=m_AllSamples.end(); i!=end; ++i)
	{
		audioBuffer = (*i);
		audioBuffer->VPause();
	}

	m_AllPaused=true;
}

//
// Audio::VResumeAllSounds				- Chapter 12, page 377
//
void Audio::VResumeAllSounds()
{
	IAudioBuffer *audioBuffer = NULL;
	AudioBufferList::iterator i;
	AudioBufferList::iterator end;
	for(i=m_AllSamples.begin(), end=m_AllSamples.end(); i!=end; ++i)
	{
		audioBuffer = (*i);
		audioBuffer->VResume();
	}

	m_AllPaused=false;
}

//
// Audio::VStopAllSounds					- Chapter 12, page 378
//
void Audio::VStopAllSounds()
{
	IAudioBuffer *audioBuffer = NULL;

	AudioBufferList::iterator i;
	AudioBufferList::iterator end;
	for(i=m_AllSamples.begin(), end=m_AllSamples.end(); i!=end; ++i)
	{
		audioBuffer = (*i);
		audioBuffer->VStop();
	}

	m_AllPaused=false;
}



//
// Audio::FindSoundTypeFromFile			- Chapter 12
//
SoundType Audio::FindSoundTypeFromFile(char const * const fileName)
{
	int type = SOUND_TYPE_FIRST;

	while (type!=SOUND_TYPE_COUNT)
	{
		if (strstr(fileName, gSoundExtentions[type]))
			return SoundType(type);
		type++;
	}
	return SOUND_TYPE_UNKNOWN;
}



bool Audio::HasSoundCard(void)
{
	return (g_pAudio && g_pAudio->VActive());
}


