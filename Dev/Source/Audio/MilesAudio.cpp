//========================================================================
// CAudio.cpp : Defines a simple sound system.
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
// (c) Copyright 2003 Michael L. McShaffry
//
// This work is licensed under the Creative Commons Attribution-ShareAlike License. 
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/1.0/ 
// or send a letter to:
//      Creative Commons
//      559 Nathan Abbott Way
//      Stanford, California 94305, USA.
//
//========================================================================

/////////////////////////////////////////////////////////////////////////////
// Description:
//
// class CAudio - A C++ wrapper around the Miles sound system.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSoundResource.h"
#include "MilesAudio.h"

#ifdef USE_BINK

#include "Bink.h"
#pragma comment( lib, "binkw32.lib")

#endif

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////
static const int BESTSOUNDRATE(44100);

#ifdef USE_BINK
bool CMilesAudio::g_TellBinkToUseDirectSound = true;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMilesAudio::CMilesAudio():
	digDriverHandle(NULL),
	DLSData(0),
	DLSDev(0),
	DLSFile(0)
{
	Initialize();
}



// Initializes the wave driver
void CMilesAudio::InitializeWaveDriver()
{
   static PCMWAVEFORMAT sPCMWF;
   HDIGDRIVER            DIG;
   DWORD                 r;
   char                  buf[128];

   //
   // default sound output
   //

   //
   // try to use DirectSound with MSS mixer
   //

   AIL_set_preference(DIG_USE_WAVEOUT, NO);


   //
   // Attempt waveOutOpen()
   //

   while (digitalRate>=11025) 
   {

     sPCMWF.wf.wFormatTag      = WAVE_FORMAT_PCM;
     sPCMWF.wf.nChannels       = (U16)digitalChans;
     sPCMWF.wf.nSamplesPerSec  = digitalRate;
     sPCMWF.wf.nAvgBytesPerSec = digitalRate * (digitalBits / 8) * digitalChans;
     sPCMWF.wf.nBlockAlign     = (U16)( (digitalBits / 8) * digitalChans);
     sPCMWF.wBitsPerSample     = (U16)digitalBits;

     r = AIL_waveOutOpen(&DIG,0,WAVE_MAPPER,(LPWAVEFORMAT) &sPCMWF);

     if (r==0)
       {

       //
       // the waveOut succeeded!
       //

       //
       // get the text description of the sound driver
       //

       strcpy(buf,"Device: ");
       AIL_digital_configuration(DIG,0,0,buf+strlen(buf));


       //
       // if we're using DirectSound, check to see if it is in emulated
       // mode.  if so, then close the device and re-open with waveOut
       //

       if (AIL_get_preference(DIG_USE_WAVEOUT)==FALSE)
         {

           //
           // see if DirectSound is emulated
           //

           if ( strstr(buf, "Emulated") )
             {

             //
             // we're emulated, so close down the driver and reset the
             // preference
             //

             AIL_waveOutClose(DIG);

             AIL_set_preference(DIG_USE_WAVEOUT, TRUE);

             goto DSerror;

             }

         }


	   digDriverHandle=DIG;
       return;
       }

	if (AIL_get_preference(DIG_USE_WAVEOUT)==FALSE)
	{

		DSerror:

		//
		// DirectSound didn't work, so let's try waveOut
		//

		AIL_set_preference(DIG_USE_WAVEOUT, TRUE);

     }

     else
     {
		//
		// this rate didn't work, try a slower rate
		//

		digitalRate=digitalRate/2;

		//
		// if all of the 16-bit opens have failed, try 8-bit
		//

		if ((digitalRate<11025) && (digitalBits==8)) 
		{
			digitalRate=BESTSOUNDRATE;
			digitalBits=8;
		}
     }
   }

   //
   // no drivers could be loaded
   //

   digDriverHandle = NULL;
}//end InitializeWaveDriver


//Allocate a sample handle for the newborn sound (used by CSoundResource) and tell you it's length
IAudioBuffer *CMilesAudio::VInitAudioBuffer(shared_ptr<CSoundResource> soundResource)//const
{
	const char* fileExtension = CAudio::FindExtFromSoundType(soundResource->GetSoundType());

	if (digDriverHandle == NULL)
		return NULL;

	switch(soundResource->GetSoundType())
	{
		case SOUND_TYPE_WAVE:
			// WAVs are easy to support.
			break;

		case SOUND_TYPE_MP3:
			// Miles supports MP3s
			break;

		case SOUND_TYPE_OGG:
		case SOUND_TYPE_MIDI:	//If it's a midi file, then do nothin at this time... maybe we will support this in the future
			assert(false && "OGGs and MIDI are not supported");
			return NULL;
			break;
		default:
			assert(false && "Unknown sound type");
			return NULL;
	}//end switch

	HSAMPLE sampleHandle = AIL_allocate_sample_handle(digDriverHandle);
	//KIS(6-13-02) | Analysis of a watson crash indicates that a NULL sample handle
	//was passed into AIL_init_sample, which caused the crash.
	if(NULL==sampleHandle)
	{
		assert(false && "AIL_allocate_sample_handle is expected to return a non-NULL handle");
		return NULL;
	}
	AIL_init_sample (sampleHandle);

	// Configure sample handle to play data from file image
	if (!AIL_set_named_sample_file (sampleHandle, fileExtension, (U32 FAR *)soundResource->GetPCMBuffer(),(S32 FAR) soundResource->GetPCMBufferSize(),0))
	{
//		MessageBox(hwnd,"No ASI provider(decoder) available for file type","Error", MB_OK);
		AIL_release_sample_handle(sampleHandle);
		return(NULL);
	}

	// Set initial volume and rate
	int pitch=64;   //pitch
	U32 smpRate=AIL_sample_playback_rate(sampleHandle);	
	AIL_set_sample_playback_rate(sampleHandle, ((pitch + 1L) * smpRate) / 64L);

	//long lengthMilli;
	//AIL_sample_ms_position(sampleHandle, &lengthMilli, NULL);

	// Add handle to the list
	IAudioBuffer *audioBuffer = (IAudioBuffer *)(new CMilesAudioBuffer(sampleHandle, soundResource));
	m_AllSamples.insert( m_AllSamples.begin(), audioBuffer);
	
	return audioBuffer;

}//end AllocateSampleHandle


//Allocate a sample handle for the newborn sound (used by CSoundResource)
void CMilesAudio::VReleaseAudioBuffer(IAudioBuffer *sampleHandle)//const
{
	AIL_end_sample(HSAMPLE(sampleHandle->VGet()));
	m_AllSamples.remove(sampleHandle);
	AIL_release_sample_handle(HSAMPLE(sampleHandle->VGet()));
}//end ReleaseSampleHandle


// Initializes the sound system
void CMilesAudio::Initialize()
{
	if(m_Initialized)
	{
		return;
	}

	m_Initialized=true;

	digitalRate=BESTSOUNDRATE;
	digitalBits=16;
	digitalChans=2;

	//
	// Initialize the Miles Sound System
	//

	AIL_startup();

	//This would double the size of the secondary buffer used by DirectSound
	//AIL_set_preference(DIG_DS_SECONDARY_SIZE, 65536);

	//This would double the size of the output buffer used by everything but DirectSound
	//AIL_set_preference(DIG_OUTPUT_BUFFER_SIZE, 98304);

	InitializeWaveDriver();

	if(digDriverHandle==NULL)
	{
		return;
	}

#ifdef USE_BINK
	if (g_TellBinkToUseDirectSound)
	{
		//Specify which sound system Bink will use
		BinkSoundUseDirectSound(NULL);
		//BinkSoundUseMiles(digDriverHandle);
	}
#endif

	m_AllSamples.clear();

}//end Initialize


// Plays .wav and .mp3 sounds, volume falls between 0 and 100
bool CMilesAudioBuffer::PlayWave(int volume, bool looping)
{
	HSAMPLE sampleHandle = (HSAMPLE)VGet();
	if (!g_Audio->VActive())
	{
		return(false);
	}

	assert(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");
	//Sample volume is adjusted to a number between 0 and 120
	AIL_set_sample_volume(sampleHandle, volume*120/100);

	// Activate sample	
	AIL_start_sample(sampleHandle);

	if(looping)
	{
		AIL_set_sample_loop_count(sampleHandle,0);
	}
	else
	{
		AIL_set_sample_loop_count(sampleHandle,1);
	}

	return(true);
}//end PlayWave



//Stop all active sounds, including music
void CMilesAudio::VStopAllSounds()
{
	if (DLSData)
	{
		AIL_mem_free_lock(DLSData);
		DLSData=NULL;
	}

	m_AllPaused=false;
	
}//end StopAllSounds


//Play a sound
bool CMilesAudioBuffer::VPlay(int volume, bool looping)
{
	if (!g_Audio->VActive())
		return false;

	m_Volume = volume;
	m_isLooping = looping;

	//Initialize();
	VStop();

	if ( PlayWave(volume, looping ) )
	{
		return true;
	}

	return false;

}//end Play


//Stop a sound
bool CMilesAudioBuffer::VStop()
{
	HSAMPLE sampleHandle = (HSAMPLE)VGet();

	if(!g_Audio->VActive())
	{
		return false;
	}
	m_isPaused = true;

	AIL_stop_sample(sampleHandle);

	return true;
}

//Resume a sound
bool CMilesAudioBuffer::VResume()
{
	HSAMPLE sampleHandle = (HSAMPLE)VGet();

	if (g_Audio->VActive())
	{
		AIL_resume_sample(sampleHandle);
		m_isPaused = false;
	}

	return true;
}

//Pause a sound or Resume a Paused sound
bool CMilesAudioBuffer::VTogglePause()
{
	HSAMPLE sampleHandle = (HSAMPLE)VGet();

	if(m_isPaused)
	{
		m_isPaused=false;
		AIL_resume_sample(sampleHandle);
	}
	else
	{
		m_isPaused=true;
		AIL_stop_sample(sampleHandle);
	}
	return true;
}//end TogglePause


// Uninitializes the sound system
void CMilesAudio::VShutdown()
{
	if(m_Initialized)
	{
		CAudio::VShutdown();
		AIL_shutdown();
	}
}//end Uninitialize


bool CMilesAudioBuffer::VIsPlaying()
{
	HSAMPLE sampleHandle = (HSAMPLE)VGet();

	if (sampleHandle)
	{
		if(sampleHandle)
		{
			if(SMP_PLAYING==AIL_sample_status(sampleHandle))
			{
				return(true);
			}
		}
		return(false);
	}
	else
	{
		return(false);
	}
}


void CMilesAudioBuffer::VSetVolume(int volume)
{
	m_Volume = volume;
	HSAMPLE sampleHandle = (HSAMPLE)VGet();

	assert(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");

	//Sample volume is adjusted to a number between 0 and 120
	AIL_set_sample_volume(sampleHandle, volume*120/100);
}//end SetVolume


