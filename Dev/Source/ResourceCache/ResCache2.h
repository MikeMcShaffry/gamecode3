#pragma once
//========================================================================
// ResCache.h : Defines a simple resource cache.
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
//  class Resource			- Chapter 7, page 213
//  class ResourceZipFile	- Chapter 7, page 214
//  class ResHandle			- Chapter 7, page 216
//  class ResCache			- Chapter 7, page 217
//
//========================================================================


// Note: this was renamed from struct Resource in the book.

class ResHandle;
class ResCache;

class Resource
{
public:
	std::string m_name;

	Resource(std::string name) { m_name=name; }
	virtual ResHandle *VCreateHandle(const char *buffer, unsigned int size, ResCache *pResCache); 
};

class ZipFile;

class ResourceZipFile : public IResourceFile
{
	ZipFile *m_pZipFile;
	std::wstring m_resFileName;

public:
	ResourceZipFile(const _TCHAR *resFileName) { m_pZipFile = NULL; m_resFileName=resFileName; }
	virtual ~ResourceZipFile();

	virtual bool VOpen();
	virtual int VGetResourceSize(const Resource &r);
	virtual int VGetResource(const Resource &r, char *buffer);
};

class ResHandle
{
	friend class ResCache;

protected:
	Resource m_resource;
	char *m_buffer;	
	unsigned int m_size;

	ResCache *m_pResCache;

public:
	ResHandle(Resource & resource, char *buffer, unsigned int size, ResCache *pResCache);
	virtual ~ResHandle();
	virtual int VLoad(IResourceFile *file) 
		{ return file->VGetResource(m_resource, m_buffer); }

	unsigned int Size() const { return m_size; } 
	char *Buffer() const { return m_buffer; }
};

typedef std::list< shared_ptr <ResHandle > > ResHandleList;			// lru list
typedef std::map<std::string, shared_ptr < ResHandle  > > ResHandleMap;		// maps indentifiers to resource data

class ResCache
{
	friend class ResHandle;

	ResHandleList m_lru;								// lru list
	ResHandleMap m_resources;
	IResourceFile *m_file;

	unsigned int			m_cacheSize;			// total memory size
	unsigned int			m_allocated;			// total memory allocated

protected:

	bool MakeRoom(unsigned int size);
	char *Allocate(unsigned int size);
	void Free(shared_ptr<ResHandle> gonner);

	shared_ptr<ResHandle> Load(Resource * r);
	shared_ptr<ResHandle> Find(Resource * r);
	void Update(shared_ptr<ResHandle> handle);

	void FreeOneResource();
	void MemoryHasBeenFreed(unsigned int size);

public:
	ResCache(const unsigned int sizeInMb, IResourceFile *file);
	virtual ~ResCache();

	bool Init() { return m_file->VOpen(); }
	shared_ptr<ResHandle> GetHandle(Resource * r);

	void Flush(void);

};



