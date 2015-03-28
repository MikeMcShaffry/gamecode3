//========================================================================
// Minidump.cpp : This is a crash trapper - similar to a UNIX style core dump
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

//========================================================================
//  Content References in Game Coding Complete
// 
//  MiniDumper					- Chapter 12, page 425-429
//========================================================================

#include "GameCodeStd.h"
#include <time.h>
#include "MiniDump.h"

#pragma comment(lib, "version.lib")


// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

MiniDumper *MiniDumper::gpDumper = NULL;

//
// MiniDumper::MiniDumper			- Chapter 21, page 814
//
MiniDumper::MiniDumper(bool headless)
{
	// Detect if there is more than one MiniDumper.
	assert( !gpDumper );

	if (!gpDumper)
	{
		::SetUnhandledExceptionFilter( Handler );
		gpDumper = this;
		m_bHeadless = headless;						// doesn't throw up a dialog, just writes the file.
	}
}


//
// MiniDumper::Handler			- Chapter 21, page 814
//
LONG MiniDumper::Handler( _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;

	if (!gpDumper)
	{
		return retval;
	}

	return gpDumper->WriteMiniDump(pExceptionInfo);
}

//
// MiniDumper::WriteMiniDump	- Chapter 21, page 814
//
LONG MiniDumper::WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo )
{
    time( &m_lTime );

	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	m_pExceptionInfo = pExceptionInfo;

	// You have to find the right dbghelp.dll. 
	// Look next to the EXE first since the one in System32 might be old (Win2k)
	
	HMODULE hDll = NULL;
	TCHAR szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, m_szAppPath, _MAX_PATH ))
	{
		TCHAR *pSlash = _tcsrchr( m_szAppPath, '\\' );
		if (pSlash)
		{
#ifdef _VS2005_
			_tcscpy_s( m_szAppBaseName, pSlash + 1);
#else
			_tcscpy( m_szAppBaseName, pSlash + 1 );
#endif
			*(pSlash+1) = 0;
		}

#ifdef _VS2005_
		_tcscpy_s( szDbgHelpPath, m_szAppPath );
        _tcscat_s( szDbgHelpPath, _T("DBGHELP.DLL") );
#else
		_tcscpy( szDbgHelpPath, m_szAppPath );
        _tcscat( szDbgHelpPath, _T("DBGHELP.DLL") );
#endif
		hDll = ::LoadLibrary( szDbgHelpPath );
	}

	if (hDll==NULL)
	{
		// If we haven't found it yet - try one more time.
		hDll = ::LoadLibrary( _T("DBGHELP.DLL") );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pMiniDumpWriteDump)
		{
			TCHAR szScratch [USER_DATA_BUFFER_SIZE];

			VSetDumpFileName();

			// ask the user if they want to save a dump file
			_tcssprintf(szScratch, _T("There was an unexpected error:\n\n%s\nWould you like to save a diagnostic file?\n\nFilename: %s"), VGetUserMessage(), m_szDumpPath);
			if (m_bHeadless || (::MessageBox( NULL, szScratch, NULL, MB_YESNO )==IDYES))
			{
				// create the file
				HANDLE hFile = ::CreateFile( m_szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pMiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, VGetUserStreamArray(), NULL );
					if (bOK)
					{
						szResult = NULL;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						_tcssprintf( szScratch, _T("Failed to save dump file to '%s' (error %d)"), m_szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					_tcssprintf( szScratch, _T("Failed to create dump file '%s' (error %d)"), m_szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = _T("DBGHELP.DLL too old");
		}
	}
	else
	{
		szResult = _T("DBGHELP.DLL not found");
	}

	if (szResult && !m_bHeadless)
		::MessageBox( NULL, szResult, NULL, MB_OK );

	TerminateProcess(GetCurrentProcess(), 0);
	
	// MLM Note: ExitThread will work, and it allows the MiniDumper to kill a crashed thread
	// without affecting the rest of the application. The question of the day:
	//   Is That A Good Idea??? Answer: ABSOLUTELY NOT!!!!!!!
	//
	//ExitThread(0);

	return retval;
}

//
// MiniDumper::VSetDumpFileName				- Chapter 21, page 817
//
void MiniDumper::VSetDumpFileName(void)
{
	_tcssprintf(m_szDumpPath, _T("%s%s.%ld.dmp"), m_szAppPath, m_szAppBaseName, m_lTime);
}

