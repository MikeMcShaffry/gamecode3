//========================================================================
// DirectSoundAudio.cpp : Defines a simple sound system that uses DirectSound
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

#include "..\GameCode.h"
#include "CSoundResource.h"
#include "DirectSoundAudio.h"

//////////////////////////////////////////////////////////////////////
// 
// DirectSoundAudio Implementation
//
//////////////////////////////////////////////////////////////////////



//
// DirectSoundAudio::VInitialize			- Chapter 12, page 379
//
bool DirectSoundAudio::VInitialize(HWND hWnd)
{
	if(m_Initialized)
		return true;

	m_Initialized=false;

	SAFE_RELEASE( m_pDS );

	HRESULT hr;

	// Create IDirectSound using the primary sound device
	if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
		return false;

	// Set DirectSound coop level 
	if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY) ) )
		return false;

	if( FAILED( hr = SetPrimaryBufferFormat( 8, 44100, 16 ) ) )
		return false;

	m_Initialized = true;
	m_AllSamples.clear();
	return true;
}

//
// DirectSoundAudio::SetPrimaryBufferFormat		- Chapter 12, page 380
//
HRESULT DirectSoundAudio::SetPrimaryBufferFormat( 
	DWORD dwPrimaryChannels, 
    DWORD dwPrimaryFreq, 
    DWORD dwPrimaryBitRate )
{
	// !WARNING! - Setting the primary buffer format and then using this
	// it for DirectMusic messes up DirectMusic! 
	//
	// If you want your primary buffer format to be 22kHz stereo, 16-bit
	// call with these parameters:	SetPrimaryBufferFormat(2, 22050, 16);

	HRESULT             hr;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	if( m_pDS == NULL )
		return CO_E_NOTINITIALIZED;

	// Get the primary buffer 
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;

	if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
		return DXUT_ERR( L"CreateSoundBuffer", hr );

	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
	wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wfx.nChannels       = (WORD) dwPrimaryChannels; 
	wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
	wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
	wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
		return DXUT_ERR( L"SetFormat", hr );

	SAFE_RELEASE( pDSBPrimary );

	return S_OK;
}


//
// DirectSoundAudio::VShutdown					- Chapter 12, page 381
//
void DirectSoundAudio::VShutdown()
{
	if(m_Initialized)
	{
		Audio::VShutdown();
		SAFE_RELEASE(m_pDS);
		m_Initialized = false;
	}
}


//
// DirectSoundAudio::VInitAudioBuffer					- Chapter 12, page 382
//   Allocate a sample handle for the newborn sound (used by SoundResource) and tell you it's length
//
IAudioBuffer *DirectSoundAudio::VInitAudioBuffer(shared_ptr<SoundResHandle> soundResource)//const
{
	const char * fileExtension = Audio::FindExtFromSoundType(soundResource->GetSoundType());

    if( m_pDS == NULL )
        return NULL;

	switch(soundResource->GetSoundType())
	{
		case SOUND_TYPE_OGG:
		case SOUND_TYPE_WAVE:
			// We support WAVs and OGGs
			break;

		case SOUND_TYPE_MP3:
		case SOUND_TYPE_MIDI:	//If it's a midi file, then do nothin at this time... maybe we will support this in the future
			assert(false && "MP3s and MIDI are not supported");
			return NULL;
			break;

		default:
			assert(false && "Unknown sound type");
			return NULL;
	}//end switch

    LPDIRECTSOUNDBUFFER sampleHandle;

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes   = soundResource->GetPCMBufferSize();
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.lpwfxFormat     = const_cast<WAVEFORMATEX *>(soundResource->GetFormat());

	HRESULT hr;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &sampleHandle, NULL ) ) )
    {
        return NULL;
    }

	// Add handle to the list
	IAudioBuffer *audioBuffer = GCC_NEW DirectSoundAudioBuffer(sampleHandle, soundResource);
	m_AllSamples.insert( m_AllSamples.begin(), audioBuffer);
	
	return audioBuffer;

}

//
// DirectSoundAudio::VReleaseAudioBuffer			- Chapter 12, page 383
//    Allocate a sample handle for the newborn sound (used by SoundResource)
//
void DirectSoundAudio::VReleaseAudioBuffer(IAudioBuffer *sampleHandle)//const
{
	sampleHandle->VStop();
	m_AllSamples.remove(sampleHandle);
}

//
// DirectSoundAudioBuffer::DirectSoundAudioBuffer	- Chapter 12, page 384
//
DirectSoundAudioBuffer::DirectSoundAudioBuffer(
	LPDIRECTSOUNDBUFFER sample, 
	shared_ptr<SoundResHandle> resource) 
 : AudioBuffer(resource) 
{ 
	m_Sample = sample; 
	FillBufferWithSound();
}

//
// DirectSoundAudioBuffer::VGet						- Chapter 12, page 385
//
void *DirectSoundAudioBuffer::VGet()
{
 	if (!VOnRestore())
		return NULL;

	return m_Sample;
}


//
// DirectSoundAudioBuffer::VPlay					- Chapter 12, page 385
//    Play a sound
//
bool DirectSoundAudioBuffer::VPlay(int volume, bool looping)
{
	VStop();

	m_Volume = volume;
	m_isLooping = looping;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)VGet();
	if (!pDSB)
		return false;

    pDSB->SetVolume( volume );
    
    DWORD dwFlags = looping ? DSBPLAY_LOOPING : 0L;

    return (S_OK==pDSB->Play( 0, 0, dwFlags ) );

}//end Play


//
// DirectSoundAudioBuffer::VStop					- Chapter 12, page 385
//    Stop a sound and rewind play position to the beginning.
//
bool DirectSoundAudioBuffer::VStop()
{
	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)VGet();

	if(!g_pAudio->VActive())
		return false;

   if( pDSB == NULL )
		return false;

	m_isPaused=true;
    pDSB->Stop();
	return true;
}


//
// DirectSoundAudioBuffer::VPause					- not in the book
//    Pause a sound 
//
bool DirectSoundAudioBuffer::VPause()
{
	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)VGet();

	if(!g_pAudio->VActive())
		return false;

   if( pDSB == NULL )
		return false;

	m_isPaused=true;
    pDSB->Stop();
	pDSB->SetCurrentPosition(0);	// rewinds buffer to beginning.
	return true;
}

//
// DirectSoundAudioBuffer::VResume					- Chapter 12, page 386
//    Resume a sound
bool DirectSoundAudioBuffer::VResume()
{
	m_isPaused=false;
	return VPlay(VGetVolume(), VIsLooping());
}

//
// DirectSoundAudioBuffer::VTogglePause				- Chapter 12, page 386
//    Pause a sound or Resume a Paused sound
//
bool DirectSoundAudioBuffer::VTogglePause()
{
	if(!g_pAudio->VActive())
		return false;

	if(m_isPaused)
	{
		VResume();
	}
	else
	{
		VPause();				// note that the book code calls VStop().
								//   It's better to call VPause() instead.
	}

	return true;
}//end TogglePause





//
// DirectSoundAudioBuffer::VIsPlaying				- Chapter 12, page 386
//
bool DirectSoundAudioBuffer::VIsPlaying() 
{
	if(!g_pAudio->VActive())
		return false;

	DWORD dwStatus = 0;
	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)VGet();
	pDSB->GetStatus( &dwStatus );
    bool bIsPlaying = ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );

	return bIsPlaying;
}

//
// DirectSoundAudioBuffer::VSetVolume				- Chapter 12, page 386
//


#define GCC_DSBVOLUME_MIN (DSBVOLUME_MIN)
//#define GCC_DSBVOLUME_MIN (-2000)
// GCC_DSBVOLUME_MIN is defined as -5000 - why? Because DSBVOLUME_MIN, defined in dsound.h 
// is set to as -10000, which is just way too silent for a lower bound and many programmers find 
// -5000 to be a better minimum bound for the volume range to avoid an annoying silence for the 
// lower 50% of a volume slider that uses a logarithmic scale.
// This was contributed by BystanderKain!

void DirectSoundAudioBuffer::VSetVolume(int volume)
{
	if(!g_pAudio->VActive())
		return;

	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)VGet();

	assert(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");

	// convert volume from 0-100 into range for DirectX - don't forget to use a logarithmic scale!

	float coeff = (float)volume / 100.0f;
	float logarithmicProportion = coeff >0.1f  ? 1+log10(coeff)  : 0;
	float range = (DSBVOLUME_MAX - GCC_DSBVOLUME_MIN);
	float fvolume = ( range * logarithmicProportion ) + GCC_DSBVOLUME_MIN;

	assert(fvolume>=GCC_DSBVOLUME_MIN && fvolume<=DSBVOLUME_MAX);
	HRESULT hr = pDSB->SetVolume( LONG(fvolume) );
	assert(hr==S_OK);

}


//
// DirectSoundAudioBuffer::VOnRestore		- Chapter 12, page 387
//    NOTE: Renamed from DirectSoundAudioBuffer::VRestore in the book
bool DirectSoundAudioBuffer::VOnRestore()
{
   HRESULT hr;
   BOOL    bRestored;

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( &bRestored ) ) )
        return NULL;

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( ) ) )
            return NULL;
    }

	return true;
}

//
// DirectSoundAudioBuffer::RestoreBuffer			- Chapter 12, page 388
//
//    Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//    restored.  It can also NULL if the information is not needed.
//
HRESULT DirectSoundAudioBuffer::RestoreBuffer( BOOL* pbWasRestored )
{
    HRESULT hr;

    if( m_Sample == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = m_Sample->GetStatus( &dwStatus ) ) )
        return DXUT_ERR( L"GetStatus", hr );

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = m_Sample->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( ( hr = m_Sample->Restore() ) == DSERR_BUFFERLOST );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}

//
// DirectSoundAudioBuffer::FillBufferWithSound				- Chapter 12, page 389
//
HRESULT DirectSoundAudioBuffer::FillBufferWithSound( void )
{
    HRESULT hr; 
	VOID	*pDSLockedBuffer = NULL;	 // a pointer to the DirectSound buffer
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    if( m_Sample == NULL )
        return CO_E_NOTINITIALIZED;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if( FAILED( hr = RestoreBuffer( NULL ) ) ) 
        return DXUT_ERR( L"RestoreBuffer", hr );

	int pcmBufferSize = m_Resource->GetPCMBufferSize();
    // Lock the buffer down
    if( FAILED( hr = m_Sample->Lock( 0, pcmBufferSize, 
                                 &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                 NULL, NULL, 0L ) ) )
        return DXUT_ERR( L"Lock", hr );

    if( pcmBufferSize == 0 )
    {
        // Wav is blank, so just fill with silence
        FillMemory( (BYTE*) pDSLockedBuffer, 
                    dwDSLockedBufferSize, 
                    (BYTE)(m_Resource->GetFormat()->wBitsPerSample == 8 ? 128 : 0 ) );
    }
    else 
	{
		CopyMemory(pDSLockedBuffer, m_Resource->GetPCMBuffer(), pcmBufferSize);
		if( pcmBufferSize < (int)dwDSLockedBufferSize )
		{
            // If the buffer sizes are different fill in the rest with silence 
            FillMemory( (BYTE*) pDSLockedBuffer + pcmBufferSize, 
                        dwDSLockedBufferSize - pcmBufferSize, 
                        (BYTE)(m_Resource->GetFormat()->wBitsPerSample == 8 ? 128 : 0 ) );
        }
    }

    // Unlock the buffer, we don't need it anymore.
    m_Sample->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    return S_OK;
}




//
// DirectSoundAudioBuffer::VGetProgress				- Chapter 12, page 390
//
float DirectSoundAudioBuffer::VGetProgress()
{
	LPDIRECTSOUNDBUFFER pDSB = (LPDIRECTSOUNDBUFFER)VGet();	
	DWORD progress = 0;

	pDSB->GetCurrentPosition(&progress, NULL);

	float length = (float)m_Resource->GetPCMBufferSize();

	return (float)progress / length;
}


