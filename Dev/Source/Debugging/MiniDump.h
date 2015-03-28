#pragma once
//========================================================================
// Minidump.h : This is a crash trapper - similar to a UNIX style core dump
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
//  class MiniDumper			- Chapter 21, page 813
//
//

#include <Tlhelp32.h>
#include "dbghelp.h"

#include <list>

class MiniDumper
{
protected:
	static MiniDumper *gpDumper;
	static LONG WINAPI Handler( struct _EXCEPTION_POINTERS *pExceptionInfo );

	_EXCEPTION_POINTERS *m_pExceptionInfo;
	time_t m_lTime;

	TCHAR m_szDumpPath[_MAX_PATH];
	TCHAR m_szAppPath[_MAX_PATH];
	TCHAR m_szAppBaseName[_MAX_PATH];
	LONG WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );
	BOOL m_bHeadless;

	virtual void VSetDumpFileName(void);
	virtual MINIDUMP_USER_STREAM_INFORMATION *VGetUserStreamArray() { return NULL; }
	virtual const TCHAR *VGetUserMessage() { return _T(""); }

public:
	MiniDumper(bool headless);
};

class ModuleEntry
{
protected:
	MODULEENTRY32 m_me32;
	TCHAR *m_pFileVersionInfo;
	TCHAR *m_pProductVersion;

	void ReadFileVersionInfo(void);
	void FindProductVersion(void);

public:
	ModuleEntry(const MODULEENTRY32 &me);
	const MODULEENTRY32 *Get() const { return &m_me32; }
	const TCHAR *GetProductVersion() const { return m_pProductVersion; }
	virtual ~ModuleEntry();
};

#define USER_DATA_BUFFER_SIZE (4096)

