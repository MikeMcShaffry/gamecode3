/////////////////////////////////////////////////////////////////////////////
// File SoundResource.cpp
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

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include    <vorbis/codec.h>            // from the vorbis sdk
#include    <vorbis/vorbisfile.h>       // also :)

#include "..\GameCode.h"

#include "CSoundResource.h"
#include "CAudio.h"

	
//////////////////////////////////////////////////////////////////////
//
// SoundResource Implementation
//
//////////////////////////////////////////////////////////////////////

ResHandle *SoundResource::VCreateHandle(const char *buffer, unsigned int size, ResCache *pResCache) 
{ 
	return GCC_NEW SoundResHandle(*this, (unsigned char *)buffer, size, pResCache); 
}




//
// SoundResource::SoundResource			- Chapter 12, page 362
//
SoundResHandle::SoundResHandle(Resource &r, unsigned char *buffer, unsigned int size, ResCache *pResCache)
:	ResHandle(r, (char *)buffer, size, pResCache),
	m_PCMBuffer(NULL),
	m_PCMBufferSize(0),
	m_SoundType(SOUND_TYPE_UNKNOWN),
	m_SoundFile(r.m_name),
	m_bInitialized(false),
	m_LengthMilli(0)
{	
	// don't do anything yet - timing sound Initialization is important!
	m_bFromFile = (buffer==NULL);
}


//
// SoundResource::~SoundResource			- Chapter 12, page 363
//
SoundResHandle::~SoundResHandle()
{
	// Note - since SoundResources have a lifetime controlled by shared_ptr, 
	// it's safe for us to nuke the memory without checking the sound system first.

	//Once the buffer is deleted, anything trying to read from it will result
	//in an Access Violation, like if the sound is still being played in another thread
	//So... that means don't try to play this sound anymore after you SAFE_DELETE this buffer

	SAFE_DELETE_ARRAY(m_PCMBuffer);
}

//
// SoundResource::VInitialize				- Chapter 12, page 363
//
bool SoundResHandle::VInitialize()
{
	if (!m_bInitialized)
	{
		m_SoundType = Audio::FindSoundTypeFromFile(m_SoundFile.c_str());
		if (m_bFromFile)
		{
			FILE *file = NULL;

			file = fopen(m_SoundFile.c_str(), "rb");

			if (file ==  NULL )
			{
				return FALSE;
			}

			switch (m_SoundType)
			{
				case SOUND_TYPE_WAVE:
					ParseWave(file);
					break;

				case SOUND_TYPE_OGG:
					ParseOgg(file);
					break;

				default:
					assert(0 && _T("Sound Type Not Supported"));
			}

			fclose(file);
		}
		else 
		{
			// initializing from a memory buffer
			switch (m_SoundType)
			{
				case SOUND_TYPE_WAVE:
					ParseWave(m_buffer, m_size);
					break;

				case SOUND_TYPE_OGG:
					ParseOgg(m_buffer, m_size);
					break;

				default:
					assert(0 && _T("Sound Type Not Supported"));
			}

		}
		m_bInitialized = true;
	}
	return true;
}

//
// SoundResHandle::ParseWave				- Chapter 12, page 365
//
// There is an interesting discussion of handling WAV file formats here:
// http://www.mcshaffry.com/GameCode/thread.php?sid=&postid=6975#post6975
//
bool SoundResHandle::ParseWave(FILE *fd)
{
	DWORD		file = 0; 
	DWORD		fileEnd = 0; 
	
	DWORD		length = 0;     
	DWORD		type = 0;									

	fseek(fd, 0, SEEK_SET);

	// mmioFOURCC -- converts four chars into a 4 byte integer code.
	// The first 4 bytes of a valid .wav file is 'R','I','F','F'
	fread(&type, 1, sizeof(DWORD), fd);
	if(type != mmioFOURCC('R', 'I', 'F', 'F'))
		return false;	
	
	fread(&length, 1, sizeof(DWORD), fd); // The first integer after RIFF is the length
	fread(&type, 1, sizeof(DWORD), fd);   // The second is the block type - another code, expect.
							// 'W','A','V','E' for a legal .wav file

	if(type != mmioFOURCC('W', 'A', 'V', 'E'))
		return false;		//not a WAV

	// Find the end of the file
	fileEnd = length - 4;
	
	memset(&m_WavFormatEx, 0, sizeof(WAVEFORMATEX));

	// Load the .wav format and the .wav data
	// Note that these blocks can be in either order.
	while(file < fileEnd)
	{
		fread(&type, 1, sizeof(DWORD), fd);   
		file += sizeof(DWORD);

		fread(&length, 1, sizeof(DWORD), fd);   
		file += sizeof(DWORD);

		switch(type)
		{
			case mmioFOURCC('f', 'a', 'c', 't'):
			{
				assert(false && "This wav file is compressed.  We don't handle compressed wav at this time");
				break;
			}

			case mmioFOURCC('f', 'm', 't', ' '):
			{
				fread(&m_WavFormatEx, 1, length, fd);   
				m_WavFormatEx.cbSize = length;
				break;
			}

			case mmioFOURCC('d', 'a', 't', 'a'):
			{
				m_PCMBuffer = GCC_NEW char[length];
				m_PCMBufferSize = length;
				size_t bytesRead = fread(m_PCMBuffer, 1, (unsigned int)length, fd);   
				if (bytesRead < (unsigned int)length)
				{
					assert(0 && _T("Couldn't read the sound data!"));
					return false;
				}
				break;
			}
		} 

		file += length;

		// If both blocks have been seen, we can return true.
		if( (m_PCMBuffer != 0 ) && (m_PCMBufferSize != 0) )
		{
			m_LengthMilli = ( GetPCMBufferSize() * 1000 ) / GetFormat()->nAvgBytesPerSec;
			return true;
		}

		// Increment the pointer past the block we just read,
		// and make sure the pointer is byte aliged.
		if (length & 1)
		{
			char temp;
			fread(&temp, 1, 1, fd);   
			file ++;
		}
	} 

	// If we get to here, the .wav file didn't contain all the right pieces.
	return false; 
}


bool SoundResHandle::ParseWave(char *wavStream, size_t bufferLength)
{
	DWORD		file = 0; 
	DWORD		fileEnd = 0; 
	
	DWORD		length = 0;     
	DWORD		type = 0;									

	DWORD pos = 0;

	// mmioFOURCC -- converts four chars into a 4 byte integer code.
	// The first 4 bytes of a valid .wav file is 'R','I','F','F'

	type = *((DWORD *)(wavStream+pos));		pos+=sizeof(DWORD);
	if(type != mmioFOURCC('R', 'I', 'F', 'F'))
		return false;	
	
	length = *((DWORD *)(wavStream+pos));	pos+=sizeof(DWORD);
	type = *((DWORD *)(wavStream+pos));		pos+=sizeof(DWORD);

	// 'W','A','V','E' for a legal .wav file
	if(type != mmioFOURCC('W', 'A', 'V', 'E'))
		return false;		//not a WAV

	// Find the end of the file
	fileEnd = length - 4;
	
	memset(&m_WavFormatEx, 0, sizeof(WAVEFORMATEX));

	// Load the .wav format and the .wav data
	// Note that these blocks can be in either order.
	while(file < fileEnd)
	{
		type = *((DWORD *)(wavStream+pos));		pos+=sizeof(DWORD);
		file += sizeof(DWORD);

		length = *((DWORD *)(wavStream+pos));	pos+=sizeof(DWORD);
		file += sizeof(DWORD);

		switch(type)
		{
			case mmioFOURCC('f', 'a', 'c', 't'):
			{
				assert(false && "This wav file is compressed.  We don't handle compressed wav at this time");
				break;
			}

			case mmioFOURCC('f', 'm', 't', ' '):
			{
				memcpy(&m_WavFormatEx, wavStream+pos, length);		pos+=length;   
				m_WavFormatEx.cbSize = length;
				break;
			}

			case mmioFOURCC('d', 'a', 't', 'a'):
			{
				m_PCMBuffer = GCC_NEW char[length];
				m_PCMBufferSize = length;
				memcpy(m_PCMBuffer, wavStream+pos, length);			pos+=length;
				break;
			}
		} 

		file += length;

		// If both blocks have been seen, we can return true.
		if( (m_PCMBuffer != 0 ) && (m_PCMBufferSize != 0) )
		{
			m_LengthMilli = ( GetPCMBufferSize() * 1000 ) / GetFormat()->nAvgBytesPerSec;
			return true;
		}

		// Increment the pointer past the block we just read,
		// and make sure the pointer is word aliged.
		if (length & 1)
		{
			++pos;
			++file;
		}
	} 

	// If we get to here, the .wav file didn't contain all the right pieces.
	return false; 
}




//
// SoundResource::ParseOgg				- Chapter 12, page 398
//
bool SoundResHandle::ParseOgg(FILE *fd)
{
    OggVorbis_File vf;                     // for the vorbisfile interface

    ov_open(fd, &vf, NULL, 0);

    // ok now the tricky part
    // the vorbis_info struct keeps the most of the interesting format info
    vorbis_info *vi = ov_info(&vf,-1);

    memset(&m_WavFormatEx, 0, sizeof(m_WavFormatEx));

    m_WavFormatEx.cbSize          = sizeof(m_WavFormatEx);
    m_WavFormatEx.nChannels       = vi->channels;
    m_WavFormatEx.wBitsPerSample  = 16;                    // ogg vorbis is always 16 bit
    m_WavFormatEx.nSamplesPerSec  = vi->rate;
    m_WavFormatEx.nAvgBytesPerSec = m_WavFormatEx.nSamplesPerSec*m_WavFormatEx.nChannels*2;
    m_WavFormatEx.nBlockAlign     = 2*m_WavFormatEx.nChannels;
    m_WavFormatEx.wFormatTag      = 1;

	DWORD   size = 4096 * 16;
    DWORD   pos = 0;
    int     sec = 0;
    int     ret = 1;
    
	//Thanks to Kain for fixing this bug!
	//DWORD bytes = (DWORD)ov_pcm_total(&vf, -1);			// get the total number of PCM samples
	DWORD bytes = (DWORD)ov_pcm_total(&vf, -1);    
	bytes *= 2 * vi->channels;
	m_PCMBuffer = GCC_NEW char[bytes];
	m_PCMBufferSize = bytes;

    // now read in the bits
    while(ret && pos<bytes)
    {
        ret = ov_read(&vf, m_PCMBuffer+pos, size, 0, 2, 1, &sec);
        pos += ret;
		if (bytes - pos < size)
		{
			size = bytes - pos;
		}
    }

	m_LengthMilli = 1000.f * ov_time_total(&vf, -1);
	
	ov_clear(&vf);
	fclose(fd);
	return true;
}


// struct OggMemoryFile							- Chapter 12, page 368
//
struct OggMemoryFile
{
  unsigned char*  dataPtr;// Pointer to the data in memory
  size_t    dataSize;     // Size of the data
  size_t    dataRead;     // Bytes read so far
}; 

// VorbisRead									- Chapter 12, page 368
//
size_t VorbisRead(void* data_ptr, size_t byteSize, size_t sizeToRead, void* data_src)                               
{
   OggMemoryFile *pVorbisData = static_cast<OggMemoryFile *>(data_src);
   if (NULL == pVorbisData) 
   {
	   return -1;
   }

   size_t actualSizeToRead, spaceToEOF = 
       pVorbisData->dataSize - pVorbisData->dataRead;
   if ((sizeToRead*byteSize) < spaceToEOF)
   {
       actualSizeToRead = (sizeToRead*byteSize);
   }
   else
   {
       actualSizeToRead = spaceToEOF;  
   }
  
   if (actualSizeToRead)
   {
     memcpy(data_ptr, 
		 (char*)pVorbisData->dataPtr + pVorbisData->dataRead, actualSizeToRead);
     pVorbisData->dataRead += actualSizeToRead;
   }

   return actualSizeToRead;
}


// VorbisSeek									- Chapter 12, page 369
//
int VorbisSeek(void* data_src, ogg_int64_t offset, int origin)            
{
	OggMemoryFile *pVorbisData = static_cast<OggMemoryFile *>(data_src);
	if (NULL == pVorbisData) 
	{
		return -1;
	}

	switch (origin)
	{
		case SEEK_SET: 
		{ 
		  ogg_int64_t actualOffset; 
		  actualOffset = (pVorbisData->dataSize >= offset) ? offset : pVorbisData->dataSize;
		  pVorbisData->dataRead = static_cast<size_t>(actualOffset);
		  break;
		}

		case SEEK_CUR: 
		{
		  size_t spaceToEOF = 
			 pVorbisData->dataSize - pVorbisData->dataRead;

		  ogg_int64_t actualOffset; 
		  actualOffset = (offset < spaceToEOF) ? offset : spaceToEOF;  

		  pVorbisData->dataRead += static_cast<LONG>(actualOffset);
		  break;
		}

    case SEEK_END: 
		pVorbisData->dataRead = pVorbisData->dataSize+1;
		break;

    default:
      assert(false && "Bad parameter for 'origin', requires same as fseek.");
      break;
  };

  return 0;
}


// VorbisClose									- Chapter 12, page 370
//
int VorbisClose(void *src)
{
	// Do nothing - we assume someone else is managing the raw buffer
    return 0;
}


// VorbisTell									- Chapter 12, page 370
//
long VorbisTell(void *data_src) 
{
	OggMemoryFile *pVorbisData = static_cast<OggMemoryFile *>(data_src);
	if (NULL == pVorbisData) 
	{
		return -1L;
	}

	return static_cast<long>(pVorbisData->dataRead);
}
//
// SoundResource::ParseOgg				- Chapter 12, page 3370
//
bool SoundResHandle::ParseOgg(char *oggStream, size_t length)
{
    OggVorbis_File vf;                     // for the vorbisfile interface

	ov_callbacks oggCallbacks;

	OggMemoryFile *vorbisMemoryFile = GCC_NEW OggMemoryFile; 
	vorbisMemoryFile->dataRead = 0;
	vorbisMemoryFile->dataSize = length; 
	vorbisMemoryFile->dataPtr = (unsigned char *)oggStream;

	oggCallbacks.read_func = VorbisRead;
	oggCallbacks.close_func = VorbisClose;
	oggCallbacks.seek_func = VorbisSeek;
	oggCallbacks.tell_func = VorbisTell;

	int ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, NULL, 0, oggCallbacks);
	assert(ov_ret>=0);

    // ok now the tricky part
    // the vorbis_info struct keeps the most of the interesting format info
    vorbis_info *vi = ov_info(&vf,-1);

    memset(&m_WavFormatEx, 0, sizeof(m_WavFormatEx));

    m_WavFormatEx.cbSize          = sizeof(m_WavFormatEx);
    m_WavFormatEx.nChannels       = vi->channels;
    m_WavFormatEx.wBitsPerSample  = 16;                    // ogg vorbis is always 16 bit
    m_WavFormatEx.nSamplesPerSec  = vi->rate;
    m_WavFormatEx.nAvgBytesPerSec = m_WavFormatEx.nSamplesPerSec*m_WavFormatEx.nChannels*2;
    m_WavFormatEx.nBlockAlign     = 2*m_WavFormatEx.nChannels;
    m_WavFormatEx.wFormatTag      = 1;

	DWORD   size = 4096 * 16;
    DWORD   pos = 0;
    int     sec = 0;
    int     ret = 1;
    
	//Thanks to Kain for fixing this bug!
	//DWORD bytes = (DWORD)ov_pcm_total(&vf, -1);			// get the total number of PCM samples
	DWORD bytes = (DWORD)ov_pcm_total(&vf, -1);    
	bytes *= 2 * vi->channels;
	m_PCMBuffer = GCC_NEW char[bytes];
	m_PCMBufferSize = bytes;

    // now read in the bits
    while(ret && pos<bytes)
    {
        ret = ov_read(&vf, m_PCMBuffer+pos, size, 0, 2, 1, &sec);
        pos += ret;
		if (bytes - pos < size)
		{
			size = bytes - pos;
		}
    }

	m_LengthMilli = 1000.f * ov_time_total(&vf, -1);
	
	ov_clear(&vf);

	delete vorbisMemoryFile;

	return true;
}







