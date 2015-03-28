//========================================================================
// ResCache.cpp : Defines a simple resource cache
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
//  class Resource			- Chapter 7, page 191
//  class ResHandle			- Chapter 7, page 193
//  class ResCache			- Chapter 7, page 195
//
//========================================================================


#include "GameCodeStd.h"
#include <assert.h>
#include <list>
#include <map>

#include "ResCache2.h"
#include "ZipFile.h"




ResHandle *Resource::VCreateHandle(const char *buffer, unsigned int size, ResCache *pResCache)
{ 
	return GCC_NEW ResHandle(*this, (char *)buffer, size, pResCache); 
}


ResourceZipFile::~ResourceZipFile() 
{ 
	SAFE_DELETE(m_pZipFile); 
}

bool ResourceZipFile::VOpen()
{
	m_pZipFile = GCC_NEW ZipFile;
    if (m_pZipFile)
    {
		return m_pZipFile->Init(m_resFileName.c_str());
	}
	return false;	
}

int ResourceZipFile::VGetResourceSize(const Resource &r)
{
	int size = 0;
	optional<int> resourceNum = m_pZipFile->Find(r.m_name.c_str());
	if (resourceNum.valid())
	{
		size = m_pZipFile->GetFileLen(*resourceNum);
	}
	return size;	
}

int ResourceZipFile::VGetResource(const Resource &r, char *buffer)
{
	int size = 0;
	optional<int> resourceNum = m_pZipFile->Find(r.m_name.c_str());
	if (resourceNum.valid())
	{
		size = m_pZipFile->GetFileLen(*resourceNum);
		m_pZipFile->ReadFile(*resourceNum, buffer);
	}
	return 0;	
}



ResHandle::ResHandle(Resource & resource, char *buffer, unsigned int size, ResCache *pResCache)
: m_resource(resource)
{
	m_buffer = buffer;
	m_size = size;
	m_pResCache = pResCache;
}

ResHandle::~ResHandle()
{
	if (m_buffer) delete [] m_buffer;
	m_pResCache->MemoryHasBeenFreed(m_size);
}

ResCache::ResCache(const unsigned int sizeInMb, IResourceFile *resFile )
{
	m_cacheSize = sizeInMb * 1024 * 1024;				// total memory size
	m_allocated = 0;									// total memory allocated
	m_file = resFile;
}

ResCache::~ResCache()
{
	while (!m_lru.empty())
	{
		FreeOneResource();
	}
	SAFE_DELETE(m_file);
}




shared_ptr<ResHandle> ResCache::GetHandle(Resource * r)
{
	shared_ptr<ResHandle> handle(Find(r));
	if (handle==NULL)
	{
		handle = Load(r);
	}
	else
	{
		Update(handle);
	}
	return handle;
}

shared_ptr<ResHandle> ResCache::Load(Resource *r)
{
	int size = m_file->VGetResourceSize(*r);
	char *buffer = Allocate(size);
	if (buffer==NULL)
	{
		return shared_ptr<ResHandle>();		// ResCache is out of memory!
	}

	// Create a new resource and add it to the lru list and map
	shared_ptr<ResHandle> handle (r->VCreateHandle(buffer, size, this));
	handle->VLoad(m_file);

	m_lru.push_front(handle);
	m_resources[r->m_name] = handle;

	return handle;
}


shared_ptr<ResHandle> ResCache::Find(Resource * r)
{
	ResHandleMap::iterator i = m_resources.find(r->m_name);
	if (i==m_resources.end())
		return shared_ptr<ResHandle>();

	return (*i).second;
}

void ResCache::Update(shared_ptr<ResHandle> handle)
{
	m_lru.remove(handle);
	m_lru.push_front(handle);
}




char *ResCache::Allocate(unsigned int size)
{
	if (!MakeRoom(size))
		return NULL;

	char *mem = GCC_NEW char[size];
	if (mem)
	{
		m_allocated += size;
	}

	return mem;
}


void ResCache::FreeOneResource()
{
	ResHandleList::iterator gonner = m_lru.end();
	gonner--;

	shared_ptr<ResHandle> handle = *gonner;

	m_lru.pop_back();							
	m_resources.erase(handle->m_resource.m_name);
	//m_allocated -= handle->m_resource.m_size ;
	//delete handle;
	// This is where the reshandle should be freed....
}




void ResCache::Flush()
{
	while (!m_lru.empty())
	{
		shared_ptr<ResHandle> handle = *(m_lru.begin());
		Free(handle);
		m_lru.pop_front();
	}
}



bool ResCache::MakeRoom(unsigned int size)
{
	if (size > m_cacheSize)
	{
		return false;
	}

	// return null if there's no possible way to allocate the memory
	while (size > (m_cacheSize - m_allocated))
	{
		// The cache is empty, and there's still not enough room.
		if (m_lru.empty())
			return false;

		FreeOneResource();
	}

	return true;
}



void ResCache::Free(shared_ptr<ResHandle> gonner)
{
	m_lru.remove(gonner);
	m_resources.erase(gonner->m_resource.m_name);
	// Note - the resource might still be in use by something,
	// so the cache can't actually count the memory freed until the
	// ResHandle pointing to it is destroyed.

	//m_allocated -= gonner->m_resource.m_size;
	//delete gonner;
}

void ResCache::MemoryHasBeenFreed(unsigned int size)
{
	m_allocated -= size;
}


