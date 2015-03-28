#pragma once
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
//  class IAudioBuffer				- Chapter 12, page 372
//  class AudioBuffer				- Chapter 12, page 374
//  class IAudio					- Chapter 12, page 375
//  class Audio						- Chapter 12, page 376




class SoundResource;
class SoundResHandle;

//////////////////////////////////////////////////////////////////////
// SoundType Description
//
// This is an enum that represents the different kinds of sound data
// streams the sound system can handle.
//
//////////////////////////////////////////////////////////////////////

enum SoundType
{
	SOUND_TYPE_FIRST,
	SOUND_TYPE_MP3 = SOUND_TYPE_FIRST,
	SOUND_TYPE_WAVE,
	SOUND_TYPE_MIDI,
	SOUND_TYPE_OGG,

	// This needs to be the last sound type
	SOUND_TYPE_COUNT,
	SOUND_TYPE_UNKNOWN,
};

extern char *gSoundExtentions[];

//////////////////////////////////////////////////////////////////////
// IAudioBuffer Description
//
// The interface class that defines the public API for audio buffers.
// An audio buffer maps to one instance of a sound being played, 
// which ISNT the sound data. Two different sounds can be played from
// the same source data - such as two explosions in two different places.
//////////////////////////////////////////////////////////////////////

class IAudioBuffer
{
public:
	virtual ~IAudioBuffer() { }

	virtual void *VGet()=0;
	virtual shared_ptr<SoundResHandle> VGetResource()=0;
	virtual bool VOnRestore()=0;

	virtual bool VPlay(int volume, bool looping)=0;
	virtual bool VPause()=0;
	virtual bool VStop()=0;
	virtual bool VResume()=0;

	virtual bool VTogglePause()=0;
	virtual bool VIsPlaying()=0;
	virtual bool VIsLooping() const=0;
	virtual void VSetVolume(int volume)=0;
	virtual int VGetVolume() const=0;
	virtual float VGetProgress()=0;
};

//////////////////////////////////////////////////////////////////////
// AudioBuffer Description
//
// Implements IAudiobuffer interface using a smart pointer to SoundResource.
//
//////////////////////////////////////////////////////////////////////

class AudioBuffer : public IAudioBuffer
{
public: 
	virtual shared_ptr<SoundResHandle> VGetResource() { return m_Resource; }
	virtual bool VIsLooping() const { return m_isLooping; }
	virtual int VGetVolume() const { return m_Volume; }
protected:
	AudioBuffer(shared_ptr<SoundResHandle >resource) 
		{ m_Resource = resource; 
		  m_isPaused = false;
		  m_isLooping = false;
		  m_Volume = 0;
		}	// disable public construction

	shared_ptr<SoundResHandle> m_Resource;

	// Is the sound paused
	bool m_isPaused;

	// Is the sound looping
	bool m_isLooping;

	//the volume
	int m_Volume;
};


//////////////////////////////////////////////////////////////////////
// IAudio Description
//
// This interface class describes the public interface for 
// a game's audio system. 
//////////////////////////////////////////////////////////////////////

class IAudio
{
public:
	virtual bool VActive()=0;

	virtual IAudioBuffer *VInitAudioBuffer(shared_ptr<SoundResHandle> soundResource)=0;
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer)=0;

	virtual void VStopAllSounds()=0;
	virtual void VPauseAllSounds()=0;
	virtual void VResumeAllSounds()=0;

	virtual bool VInitialize(HWND hWnd)=0;
	virtual void VShutdown()=0;
};

//////////////////////////////////////////////////////////////////////
// AudioBufferList Description
//
// This is the data structure of audio buffers managed by Audio.
//////////////////////////////////////////////////////////////////////

typedef std::list<IAudioBuffer *> AudioBufferList;

//////////////////////////////////////////////////////////////////////
// Audio Description
//
// Implements IAudio interface - but not all the way - this is 
// still a base class. See class DirectSoundAudio.
// 
//////////////////////////////////////////////////////////////////////

class Audio : public IAudio
{
public:
	Audio();
	~Audio() { VShutdown(); }
	virtual void VStopAllSounds();
	virtual void VPauseAllSounds();
	virtual void VResumeAllSounds();

	virtual void VShutdown();

	static char const * const FindExtFromSoundType(SoundType type) 
		{ return gSoundExtentions[type]; }
	static SoundType FindSoundTypeFromFile(char const * const ext);
	static bool HasSoundCard(void);

protected:
	
	AudioBufferList m_AllSamples;	// List of all currently allocated
	bool m_AllPaused;				// Has the sound system been paused?
	bool m_Initialized;				// Has the sound system been initialized?
};

extern Audio *g_pAudio;


