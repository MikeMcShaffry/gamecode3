#pragma once
//========================================================================
// MilesAudio.h : Implements audio interfaces for the Miles Sound System
//
// Part of the GameCode Application
//
// GameCode is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete" by Mike McShaffry, published by
// Paraglyph Press. ISBN: 1-932111-75-1
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the author a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: http://www.amazon.com/exec/obidos/ASIN/1932111751/gamecodecompl-20/
//
// There's also a companion web site at http://www.mcshaffry.com/GameCode/portal.php
//
// (c) Copyright 2003,2004 Michael L. McShaffry
//
// This work is licensed under the Creative Commons Attribution-ShareAlike License. 
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/1.0/ 
// or send a letter to:
//      Creative Commons
//      559 Nathan Abbott Way
//      Stanford, California 94305, USA.
//
//========================================================================

#include "CAudio.h"

// Miles sound library has some level 4 warnings
#pragma warning( push, 3 )
#include "..\Libs\mss.h"
#pragma warning( pop )

class CMilesAudioBuffer : public CAudioBuffer
{
protected:
	HSAMPLE m_Sample;

	// Plays .wav and .mp3 sounds at the right volume (0 to 100)
	bool PlayWave(int volume, bool looping);

public:
	CMilesAudioBuffer(HSAMPLE sample, shared_ptr<CSoundResource> resource) 
		: CAudioBuffer(resource) { m_Sample = sample;  }
	virtual void *VGet() { return m_Sample; }
	virtual bool VRestore() { return true; }

	virtual bool VPlay(int volume, bool looping);
	virtual bool VStop();
	virtual bool VResume();

	virtual bool VTogglePause();
	virtual bool VIsPlaying();
	virtual void VSetVolume(int volume);
	virtual float VGetProgress() { return 0.0f; }

};

class CMilesAudio : public CAudio
{
public:
	CMilesAudio();

	virtual bool VActive() { return digDriverHandle!=NULL && !m_AllPaused; }

	virtual IAudioBuffer *VInitAudioBuffer(shared_ptr<CSoundResource> soundResource);
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer);

	virtual void VStopAllSounds();

	virtual void VShutdown();

public:	

	static bool g_TellBinkToUseDirectSound;			// governs a Bink API call. Default==true.

private:
	void Initialize();

	HDIGDRIVER digDriverHandle;

	void FAR *DLSData;

	HDLSDEVICE DLSDev;
	HDLSFILEID DLSFile;

	S32 digitalRate;
	S32 digitalBits;
	S32 digitalChans;

	// Initializes the wave driver
	void InitializeWaveDriver();

};
