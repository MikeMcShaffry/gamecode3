#pragma once
//========================================================================
// DirectSoundAudio.h : Implements audio interfaces for DirectSound
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
//  class DirectSoundAudio				- Chapter 12, page 378
//  class DirectSoundAudioBuffer		- Chapter 12, page 384
//

#include "CAudio.h"

// DirectSound includes
#include <dsound.h>
#include <mmsystem.h>


//////////////////////////////////////////////////////////////////////
// DirectSoundAudioBuffer Description
//
// Implements the rest of the IAudioBuffer interface left out by AudioBuffer.  
// If you are interested in implementing a sound system using OpenAL 
// you'd create a class OpenALAudioBuffer from AudioBuffer.
// 
//////////////////////////////////////////////////////////////////////

class DirectSoundAudioBuffer : public AudioBuffer
{
protected:
	LPDIRECTSOUNDBUFFER m_Sample;

public:
	DirectSoundAudioBuffer(LPDIRECTSOUNDBUFFER sample, shared_ptr<SoundResHandle> resource);
	virtual void *VGet();
	virtual bool VOnRestore();

	virtual bool VPlay(int volume, bool looping);
	virtual bool VPause();
	virtual bool VStop();
	virtual bool VResume();

	virtual bool VTogglePause();
	virtual bool VIsPlaying();
	virtual void VSetVolume(int volume);

	virtual float VGetProgress();

private:
	HRESULT FillBufferWithSound( );
	HRESULT RestoreBuffer( BOOL* pbWasRestored );
};



//////////////////////////////////////////////////////////////////////
// DirectSoundAudio Description
//
// Implements the rest of the IAudio interface left out by Audio.  
// If you are interested in implementing a sound system using OpenAL 
// you'd create a class OpenALAudioBuffer from AudioBuffer.
// 
//////////////////////////////////////////////////////////////////////

class DirectSoundAudio : public Audio
{
public:
	DirectSoundAudio() { m_pDS = NULL; }
	virtual bool VActive() { return m_pDS!=NULL; }

	virtual IAudioBuffer *VInitAudioBuffer(shared_ptr<SoundResHandle> soundResource);
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer);

	virtual void VShutdown();
	virtual bool VInitialize(HWND hWnd);

protected:

    IDirectSound8* m_pDS;

	HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
									DWORD dwPrimaryFreq, 
									DWORD dwPrimaryBitRate );
};
