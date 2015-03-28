//========================================================================
// SoundProcess.cpp : Defines sound processes.
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

#include "GameCodeStd.h"

#include "..\GameCode.h"

#include "CAudio.h"
#include "CSoundProcess.h"
#include "CSoundResource.h"

//////////////////////////////////////////////////////////////////////
// SoundProcess Implementation
//////////////////////////////////////////////////////////////////////

//
// SoundProcess::SoundProcess				- Chapter 12, page 392
//
SoundProcess::SoundProcess(shared_ptr<SoundResHandle> soundResource, int typeOfSound, int volume, bool looping) :
	CProcess(typeOfSound, 0),
	m_SoundResource(soundResource),
	m_Volume(volume),
	m_isLooping(looping)
{
	InitializeVolume();
}


//
// SoundProcess::~SoundProcess				- Chapter 12, page 392
//
SoundProcess::~SoundProcess()
{
	if (m_AudioBuffer)
	{
		g_pAudio->VReleaseAudioBuffer(m_AudioBuffer.get());
	}
}


void SoundProcess::InitializeVolume()
{
	//See CProcess.h for the enum PROCESS_TYPE declaration
	int typeOfSound = GetType();
	
	// TODO: SOmewhere set an adjusted volume based on game options
	// m_volume = g_GraphicalApp->GetVolume(typeOfSound);
}

//
// SoundProcess::GetLengthMilli					- Chapter 12, page 394
//
int SoundProcess::GetLengthMilli()
{
	if ( m_SoundResource )
		return m_SoundResource->GetLengthMilli();
	else
		return 0;
}

//
// SoundProcess::VOnInitialize				- Chapter 12, page 393
//    note that the book incorrectly names this SoundProcess::OnInitialize
void SoundProcess::VOnInitialize()
{
	//If the sound has never been... you know... then Play it for the very first time
	if ( ! m_SoundResource )
		return;

	m_SoundResource->VInitialize();

	//This sound will manage it's own handle in the other thread
	IAudioBuffer *buffer = g_pAudio->VInitAudioBuffer(m_SoundResource);

	if (!buffer)
	{
		VKill();
		return;
	}

	m_AudioBuffer.reset(buffer);	

	Play(m_Volume, m_isLooping);
}

//
// SoundProcess::OnUpdate				- Chapter 12, page 393
//
void SoundProcess::VOnUpdate(const int deltaMilliseconds)
{
	// Call base
	CProcess::VOnUpdate(deltaMilliseconds);

	if ( ! m_bInitialUpdate && ! IsPlaying() )
	{
		VKill();
	}

	if ( IsDead() && IsLooping() )
	{
		Replay();
	}
}

//
// SoundProcess::IsPlaying					- Chapter 12, page 394
//
bool SoundProcess::IsPlaying()
{
	if ( ! m_SoundResource || ! m_AudioBuffer )
		return false;
	
	return m_AudioBuffer->VIsPlaying();
}

//
// SoundProcess::SetVolume				- Chapter 12, page 394
//
void SoundProcess::SetVolume(int volume)
{
	if(m_AudioBuffer==NULL)
	{
		return;
	}

	assert(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");
	m_Volume = volume;
	m_AudioBuffer->VSetVolume(volume);
}

//
// SoundProcess::GetVolume				- Chapter 12, page 395
//
int SoundProcess::GetVolume()
{
	if(m_AudioBuffer==NULL)
	{
		return 0;
	}

	m_Volume = m_AudioBuffer->VGetVolume();
	return m_Volume;
}

//
// SoundProcess::TogglePause				- Chapter 12, pages 395
//
void SoundProcess::VTogglePause()
{
	if(m_AudioBuffer)
	{
		m_AudioBuffer->VTogglePause();
	}
}

//
// SoundProcess::Kill						- Chapter 12, page 394
//
void SoundProcess::VKill()
{
	if ( IsPlaying() )
		Stop();
	CProcess::VKill();
}

//
// SoundProcess::Play						- Chapter 12, page 395
//
void SoundProcess::Play(const int volume, const bool looping)
{
	assert(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");

	if(!m_AudioBuffer)
	{
		return;
	}
	
	m_AudioBuffer->VPlay(volume, looping);
}

//
// SoundProcess::Stop						- Chapter 12, page 395
//
void SoundProcess::Stop()
{
	if(m_AudioBuffer)
	{
		m_AudioBuffer->VStop();
	}
}

//
// SoundProcess::GetProgress				- Chapter 12, page 396
//
float SoundProcess::GetProgress()
{
	if (m_AudioBuffer)
	{
		return m_AudioBuffer->VGetProgress();
	}

	return 0.0f;
}






//
// ExplosionProcess::VOnInitialize			- Chapter 12, page 398
//
void ExplosionProcess::VOnInitialize()
{
	CProcess::VOnInitialize();
	SoundResource resource("explosion.wav");
	shared_ptr<SoundResHandle> srh = boost::static_pointer_cast<SoundResHandle>(g_pApp->m_ResCache->GetHandle(&resource));
	m_Sound.reset(GCC_NEW SoundProcess(srh));

	// Imagine cool explosion graphics setup code here!!!!
	//
	//
	//
}

//
// ExplosionProcess::OnUpdate					- Chapter 12, page 398
//   note that the book incorrectly names this ExplosionProcess::OnUpdate
//
void ExplosionProcess::VOnUpdate(const int deltaMilliseconds)
{
	// Since the sound is the real pacing mechanism - we ignore deltaMilliseconds
	float progress = m_Sound->GetProgress();
	
	switch (m_Stage)
	{
		case 0:
			if (progress>0.55f)
			{
				++m_Stage;
				// Imagine secondary explosion effect launch right here!
			}
			break;

		case 1:
			if (progress>0.75f)
			{
				++m_Stage;
				// Imagine tertiary explosion effect launch right here!
			}
			break;

		default:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
// CFadeProcess Implementation 
//
//////////////////////////////////////////////////////////////////////

//
// CFadeProcess::CFadeProcess				- Chapter 12, page 400
//
CFadeProcess::CFadeProcess(shared_ptr<SoundProcess> sound, int fadeTime, int endVolume)
: CProcess(PROC_INTERPOLATOR)
{
	m_Sound = sound;
	m_TotalFadeTime = fadeTime;
	m_StartVolume = sound->GetVolume();
	m_EndVolume = endVolume;
	m_ElapsedTime = 0;

	VOnUpdate(0);
}

//
// CFadeProcess::OnUpdate					- Chapter 12, page 400
//   note that the book incorrectly names this CFadeProcess::OnUpdate
//
void CFadeProcess::VOnUpdate(const int deltaMilliseconds)
{
	if (!m_bInitialUpdate)
	{
		m_ElapsedTime += deltaMilliseconds;
	}

	CProcess::VOnUpdate(deltaMilliseconds);

	if (m_Sound->IsDead())
		VKill();

	float cooef = (float)m_ElapsedTime / m_TotalFadeTime;
	if (cooef>1.0f)
		cooef = 1.0f;
	if (cooef<0.0f)
		cooef = 0.0f;

	int newVolume = m_StartVolume + ( float(m_EndVolume - m_StartVolume) * cooef);

	if (m_ElapsedTime >= m_TotalFadeTime)
	{
		newVolume = m_EndVolume;
		VKill();
	}

	m_Sound->SetVolume(newVolume);
}

