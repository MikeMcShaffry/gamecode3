//========================================================================
// Initialization.cpp : Defines utility functions for game initialization
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
//  CheckHardDisk					- Chapter 5, page 133
//  CheckMemory						- Chapter 5, page 133
//  class GameOptions				- Chapter 5, page 142 
//  IsOnlyInstance					- Chapter 5, page 132
//  GetSaveGameDirectory			- Chapter 5, page 141-142
//========================================================================

#include "GameCodeStd.h"
#include <shlobj.h>
#include <direct.h>
#include "..\Dumbstuff\GameCodeError.h"
#include "Initialization.h"


void CheckHardDisk(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	int const drive = _getdrive();
	struct _diskfree_t diskfree;

	_getdiskfree(drive, &diskfree);

	unsigned __int64 const neededClusters = 
		diskSpaceNeeded /(diskfree.sectors_per_cluster*diskfree.bytes_per_sector);

	if (diskfree.avail_clusters < neededClusters)
	{
		// if you get here you don’t have enough disk space!
		throw GameCodeError(GCERR_INIT_NOT_ENOUGH_DISK_SPACE);
	}
}


void CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);
	if (status.ullTotalPhys < (physicalRAMNeeded))
	{
		// you don’t have enough physical memory. Tell the player to go get a real 
		// computer and give this one to his mother. 
		throw GameCodeError(GCERR_INIT_NOT_ENOUGH_PHYS_RAM);
	}

	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{
		// you don’t have enough virtual memory available. 
		// Tell the player to shut down the copy of Visual Studio running in the
		// background, or whatever seems to be sucking the memory dry.
		
		throw GameCodeError(GCERR_INIT_NOT_ENOUGH_VIRT_RAM);
	}

	char *buff = GCC_NEW char[virtualRAMNeeded];
	if (buff)
		delete[] buff;
	else
	{
		// The system lied to you. When you attempted to grab a block as big
		// as you need the system failed to do so. Something else is eating 
		// memory in the background; tell them to shut down all other apps 
		// and concentrate on your game.
		
		throw GameCodeError(GCERR_INIT_NOT_ENOUGH_CONTIG_RAM);
	}
}


DWORD GetFreeVRAM()
{
/***
	// NOTE: This method is deprecated, and unfortunately not really replaced with
	// anything useful.....


	DDSCAPS2 ddsCaps;
	ZeroMemory(&ddsCaps, sizeof(ddsCaps));
		
	ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
	DWORD dwUsedVRAM = 0;
	DWORD dwTotal=0;
	DWORD dwFree=0;

	// lp_DD points to the IDirectDraw object	
	HRESULT hr = g_pDisplay->GetDirectDraw()->GetAvailableVidMem(&ddsCaps, &dwTotal, &dwFree); 
		
	// dwUsedVRAM holds the number of bytes of VRAM used
	dwUsedVRAM = dwTotal-dwFree;
	return dwUsedVRAM;								
****/
	return 0;
}





GameOptions::GameOptions(const char* path)
{
	m_useHardwareAccel = ::GetPrivateProfileIntA( 
		"DISPLAY", "Hardware_Acceleration", false, path ) ? true : false;

	m_usePageFlipping = ::GetPrivateProfileIntA( 
		"DISPLAY", "PageFlip", true, path ) ? true : false;

	m_useDithering = ::GetPrivateProfileIntA( 
		"DISPLAY", "Dithering", true, path ) ? true : false;

	m_useAntialiasing = ::GetPrivateProfileIntA(
		"DISPLAY", "AntiAliasing", true, path ) ? true : false;

	m_useEgdeAntiAliasing = ::GetPrivateProfileIntA( 
		"DISPLAY", "Edge_AntiAliasing", false, path ) ? true : false;

	m_useVRAM = ::GetPrivateProfileIntA(
		"DISPLAY", "Enable_VRAM", true, path ) ? true : false;

	m_runFullSpeed = ::GetPrivateProfileIntA( 
		"DISPLAY", "Fullspeed", false, path ) ? true : false;

	m_useTexturePerspective = ::GetPrivateProfileIntA( 
		"DISPLAY", "Texture_Perspective", true, path ) ? true : false;

	m_soundEffectsVolume = ::GetPrivateProfileIntA( 
		"SOUND", "Sound_Effects_Volume", false, path ) / 100.0f;

	m_musicVolume = ::GetPrivateProfileIntA( 
		"SOUND", "Music_Volume", false, path ) / 100.0f;

	m_expectedPlayers = ::GetPrivateProfileIntA( 
		"MULTIPLAYER", "Expected_Players", 1, path );

	m_listenPort = ::GetPrivateProfileIntA( 
		"MULTIPLAYER", "Listen_Port", -1, path );

	char buffer[256];
	::GetPrivateProfileStringA( 
		"MULTIPLAYER", "Game_Host", "", buffer, 256, path );
	m_gameHost = buffer;

	m_numAIs = ::GetPrivateProfileIntA( 
		"MULTIPLAYER", "Expected_AIs", 1, path );

	m_maxAIs = ::GetPrivateProfileIntA( 
		"MULTIPLAYER", "Max_AIs", 4, path );

	m_maxPlayers = ::GetPrivateProfileIntA( 
		"MULTIPLAYER", "Max_Players", 4, path );	
}





bool IsOnlyInstance(LPCTSTR gameTitle)
{
	// Find the window.  If active, set and return false
	// Only one game instance may have this mutex at a time...

	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);

	// Does anyone else think 'ERROR_SUCCESS' is a bit of a dichotomy?
	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd) 
		{
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}
	return true;
}

//
// GetSaveGameDirectory - Chapter 5 - page 146

const TCHAR *GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory)
{
	HRESULT hr;

	static TCHAR m_SaveGameDirectory[MAX_PATH];
	TCHAR userDataPath[MAX_PATH];

	hr = SHGetSpecialFolderPath(hWnd, userDataPath, CSIDL_APPDATA, true);

#ifdef _VS2005_
	_tcscpy_s(m_SaveGameDirectory, userDataPath);
	_tcscat_s(m_SaveGameDirectory, _T("\\"));
	_tcscat_s(m_SaveGameDirectory, gameAppDirectory);
#else
	_tcscpy(m_SaveGameDirectory, userDataPath);
	_tcscat(m_SaveGameDirectory, _T("\\"));
	_tcscat(m_SaveGameDirectory, gameAppDirectory);
#endif


	// Does our directory exist?
	if (0xffffffff == GetFileAttributes(m_SaveGameDirectory))
	{
		// Nope - we have to go make a new directory to store application data.
		//
		// On Win32 systems you could call SHCreateDirectoryEx to create an
		// entire directory tree, but this code is included for ease of portability to other 
		// systems without that.
		//
		// Game Coding Complete reference - Chapter 11, page 388
		//
		TCHAR current[MAX_PATH];
		TCHAR myAppData[MAX_PATH];

#ifdef _VS2005_
		_tcscpy_s(current, userDataPath);
		_tcscpy_s(myAppData, gameAppDirectory);
#else
		_tcscpy(current, userDataPath);
		_tcscpy(myAppData, gameAppDirectory);
#endif

		TCHAR token[MAX_PATH];
		token[0] = 0;

		do {
			TCHAR *left = _tcschr(myAppData, '\\');
			if (left==NULL)
			{
				#ifdef _VS2005_
					_tcscpy_s(token, myAppData);
				#else
					_tcscpy(token, myAppData);
				#endif
				myAppData[0] = 0;
			}
			else
			{
				#ifdef _VS2005_
					_tcsncpy_s(token, myAppData, left-myAppData);
					token[left-myAppData] = 0;
					_tcscpy_s(myAppData, left+1);
				#else
					_tcsncpy(token, myAppData, left-myAppData);
					token[left-myAppData] = 0;
					_tcscpy(myAppData, left+1);
				#endif
			}

			if (_tcslen(token))
			{

				#ifdef _VS2005_
					_tcscat_s(current, _T("\\"));
					_tcscat_s(current, token);
				#else
					_tcscat(current, _T("\\"));
					_tcscat(current, token);
				#endif
				if (false == CreateDirectory(current, NULL))
				{
					int error = GetLastError();
					if (error != ERROR_ALREADY_EXISTS)
					{
						return false;
					}
				}
			}

		} while (_tcslen(myAppData));
	}
		
#ifdef _VS2005_
	_tcscat_s(m_SaveGameDirectory, _T("\\"));
#else
	_tcscat(m_SaveGameDirectory, _T("\\"));
#endif
	return m_SaveGameDirectory;
}




bool CheckForJoystick(HWND hWnd)
{
	JOYINFO joyinfo; 
	UINT wNumDevs; 
	BOOL bDev1Attached, bDev2Attached; 
 
    if((wNumDevs = joyGetNumDevs()) == 0) 
        return false; 
    bDev1Attached = joyGetPos(JOYSTICKID1,&joyinfo) != JOYERR_UNPLUGGED; 
    bDev2Attached = joyGetPos(JOYSTICKID2,&joyinfo) != JOYERR_UNPLUGGED; 
    if(bDev1Attached)
		joySetCapture(hWnd, JOYSTICKID1, 1000/30, true);   
	if (bDev2Attached)   
		joySetCapture(hWnd, JOYSTICKID2, 1000/30, true);   

	return true;
}