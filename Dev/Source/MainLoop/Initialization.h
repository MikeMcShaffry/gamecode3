#pragma once
//========================================================================
// Initialization.h : Defines utility functions for game initialization
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

extern void CheckHardDisk(const DWORDLONG diskSpaceNeeded);
extern void CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern bool IsOnlyInstance(LPCTSTR gameTitle);
extern const TCHAR *GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory);
extern bool CheckForJoystick(HWND hWnd);

//extern DWORD GetFreeVRAM();		// deprecated!!!!

struct GameOptions
{
	bool m_useHardwareAccel;
	bool m_usePageFlipping;
	bool m_useDithering;
	bool m_useAntialiasing;
	bool m_useEgdeAntiAliasing;
	bool m_useVRAM;
	bool m_runFullSpeed;
	bool m_useTexturePerspective;
	float m_soundEffectsVolume;			
	float m_musicVolume;				
	int m_expectedPlayers;				
	int m_listenPort;					
	std::string m_gameHost;				
	int m_numAIs;
	int m_maxAIs;
	int m_maxPlayers;

	GameOptions(const char* path);
};
