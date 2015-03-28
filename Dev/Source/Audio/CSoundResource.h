#pragma once
/////////////////////////////////////////////////////////////////////////////
// File CSoundResource.h
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
//  class SoundResource			- Chapter 12, page 361
//  class SoundResHandle		- Chapter 12, page 361



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "..\Libs\mss.h"
#include <mmsystem.h>

#include "..\ResourceCache\ResCache2.h"

/////////////////////////////////////////////////////////////////////////////
// SoundResource Description
//
//    This class is meant to house the buffer that sound players will read
//    It will encapsulate the concept of "A Sound", not to be confused
//    with "A Sound Process"(SoundProcess)
//
//////////////////////////////////////////////////////////////////////


class SoundResource : public Resource
{
public:
	SoundResource(std::string name) : Resource(name) { }
	virtual ResHandle *VCreateHandle(const char *buffer, unsigned int size, ResCache *pResCache);
};


class SoundResHandle : public ResHandle
{
public: 	
	SoundResHandle(Resource &r, unsigned char *buffer, unsigned int size, ResCache *pResCache);
	virtual ~SoundResHandle();

	char const *GetPCMBuffer() const { return m_PCMBuffer; }
	int GetPCMBufferSize() const { return m_PCMBufferSize; }
	enum SoundType GetSoundType() { return m_SoundType; }
	WAVEFORMATEX const *GetFormat() { return &m_WavFormatEx; }
	int GetLengthMilli() const { return m_LengthMilli; }

	virtual bool VInitialize();

private:
	enum SoundType m_SoundType;			// is this an Ogg, WAV, etc.?
	bool m_bInitialized;				// has the sound been initialized
	bool m_bFromFile;					// are we reading from a file or a buffer?

	char *m_PCMBuffer;					// the destination PCM buffer of playable sound
	int m_PCMBufferSize;				// the length of the PCM buffer
	WAVEFORMATEX m_WavFormatEx;			// description of the PCM format
	int m_LengthMilli;					// how long the sound is in milliseconds

	const std::string m_SoundFile;		// the name of the file or resource

	bool ParseWave(FILE *fd);			
	bool ParseOgg(FILE *fd);

	bool ParseOgg(char *oggStream, size_t length);
	bool ParseWave(char *wavStream, size_t length);
};

