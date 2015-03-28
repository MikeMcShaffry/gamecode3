//========================================================================
// GameCode.cpp : Defines the entry point for the application.
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
// There are two classes implemented in this file:
//
// GameCodeApp - the main application class, encapsulates hooking everything
//               the game needs into the Windows operating system.
//
// BaseGameLogic - the main game application, serious attempts are made to make
//            this class OS agnositic
//========================================================================


//========================================================================
//  Content References in Game Coding Complete 3rd Edition
// 
// class HumanView - Chapter 9, page 235



#include "GameCodeStd.h"
#include <atlconv.h>

#include "Dbt.h"						// required for DBT_DEVICEREMOVECOMPLETE


#include "GameCode.h"
#include "MainLoop\Initialization.h"
#include "DumbStuff\GameCodeError.h"
#include "DumbStuff\CMath.h"
#include "DumbStuff\String.h"
#include "ResourceCache\ResCache2.h"
#include "Debugging\MiniDump.h"
#include "Graphics2D\Font.h"
#include "Lang\Strings.h"
#include "Audio\DirectSoundAudio.h"
#include "EventManager\EventManagerImpl.h"
#include "Network\Network.h"
#include "Scripting\LuaStateManager.h"
#include "ai/Pathing.h"
#include "ai/AiEvents.h"

// All event type headers
#include "Physics\PhysicsEventListener.h"
#include "EventManager\Events.h"

#define MAX_LOADSTRING 100




GameCodeApp *g_pApp = NULL;

// You should leave this global - it does wacky things otherwise.
CDXUTDialogResourceManager GameCodeApp::g_DialogResourceManager;

//========================================================================
//
// GameCodeApp implementation
//
//========================================================================

GameCodeApp::GameCodeApp()
{
	g_pApp = this;
	m_pGame = NULL;

	m_rcWindow.bottom = m_rcWindow.left = m_rcWindow.right = m_rcWindow.top = 0;
	m_rcDesktop.bottom = m_rcDesktop.left = m_rcDesktop.right = m_rcDesktop.top = 0;
	m_iColorDepth = 32;

	m_pLuaStateManager = NULL;
	m_pEventManager = NULL;
	m_ResCache = NULL;

	m_bQuitRequested = false;
	m_bQuitting = false;
	m_HasModalDialog = 0;
}


HWND GameCodeApp::GetHwnd()
{ 
	return DXUTGetHWND();
}


//===================================================================
// Win32 Specific Stuff
//
// InitInstance - this checks system resources, creates your window, and launches the game
//
// preprocessor macro setting in the C++ options of the project provides the base macro
// C preprocessor string concatenation takes care of the rest.
//
// GameCodeApp::InitInstance - Chapter 5 - page 123
//
//===================================================================

#ifdef DEBUG
	char const * const kpLangDllName = "Langd.dll";
#else
	char const * const kpLangDllName = "Lang.dll";
#endif

bool GameCodeApp::InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight)
{
	// Check for existing instance of the same window
	// 
#ifndef _DEBUG
	// Note - it can be really useful to debug network code to have
	// more than one instance of the game up at one time - so
	// feel free to comment these lines in or out as you wish!
	if (!IsOnlyInstance(VGetGameTitle()))
		return false;
#endif

	// We don't need a mouse cursor by default, let the game turn it on
	SetCursor( NULL );

	// Load the string table from the language resource dll
	// Note: Only load it from same dir as exe ...
	char appPath[MAX_PATH+1] = {0};

	memset( appPath, 0, sizeof(appPath) );
	
	GetModuleFileNameA( NULL, appPath, MAX_PATH );
	
	char const * pSep = strrchr( appPath, _T('\\') );
	
	if ( pSep == NULL )
		strcpy( appPath, kpLangDllName );
	else
		strcpy( appPath + (pSep - appPath) + 1, kpLangDllName );

	m_LangDll = LoadLibraryA(appPath);

	if (!m_LangDll)
	{
		TCHAR msg[4096];

		_stprintf( msg, _T("Error 6502: Language DLL not found ( %s ).\r\n\r\nPlease reinstall from your original CD."), kpLangDllName );

		MessageBox(NULL, msg, _T("Error 6502"), MB_OK);

		return false;
	}

	// Check for adequate machine resources.
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		try 
		{
			const DWORDLONG physicalRAM = 32 * MEGABYTE;
			const DWORDLONG virtualRAM = 64 * MEGABYTE;
			CheckMemory(physicalRAM, virtualRAM);

			const DWORDLONG diskSpace = 10 * MEGABYTE;
			CheckHardDisk(diskSpace);

			const int minCpuSpeed = 266;
			extern int GetCPUSpeed();
			int thisCPU = GetCPUSpeed();
			if (thisCPU < minCpuSpeed)
				throw GameCodeError(GCERR_INIT_CPU_TOO_SLOW);

			// check DirectX version

			// vram
			// const int vram = 4 * MEGABYTES;
			// CheckVRAM(vram);
		}
	
		catch (GameCodeError err)
		{
			if (err.Handle()==ERROR_RETRY)
				continue;
			else if (err.Handle()==GCERR_INIT_NOT_ENOUGH_VIRT_RAM)
			{
				OutputDebugStringA("WARNING:  INSUFFICIENT VIRTUAL RAM REPORTED.  CONTINUING...");
			}
			else
				return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

	m_pOptions = GCC_NEW GameOptions("m_pOptions->ini");
	ParseCommandLine(lpCmdLine);


	//
	// Initialize the ResCache - Chapter 5 - page 137
	//
	m_ResCache = GCC_NEW ResCache(30, GCC_NEW ResourceZipFile(_T("data\\GameCode3.zip")));
	if (!m_ResCache->Init())
	{
		return false;
	}


	// Rez up the Lua State manager now, and run the initial script.
	m_pLuaStateManager = GCC_NEW LuaStateManager();
	if (!m_pLuaStateManager)
	{
		return false;
	}

	// event manager should be created first so that subsystems
	// can hook in as desired.
	
	m_pEventManager = GCC_NEW EventManager("GameCodeApp Event Mgr", true );
	if (!m_pEventManager)
	{
		return false;
	}

	RegisterBaseGameEvents();	//Register all base event types.

	// Now that the event manager and the Lua State manager are init'd, let's run the initialization file.
	const bool bLuaInitSuccess = m_pLuaStateManager->Init( "data\\Scripts\\init.lua" );
	if ( false == bLuaInitSuccess )
	{
		return false;
	}

	// The socket stuff used to initialize right here - 
	// It has been moved to the game logic change state routine
	// BGS_WaitingForPlayers is the time to open sockets for attachments or to a host.

	// DXUTInit, DXUTCreateWindow - Chapter 5 - page 138	
	DXUTInit( true, true, lpCmdLine, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

	if (hWnd==NULL)
	{
	    DXUTCreateWindow( VGetGameTitle(), hInstance, VGetIcon() );
	}
	else
	{
	    DXUTSetWindow( hWnd, hWnd, hWnd );
	}

	if (!GetHwnd())
	{
		return FALSE;
	}
	SetWindowText(GetHwnd(), VGetGameTitle());

	// initialize user settable game options - including finding the profiles directory
	_tcscpy(m_saveGameDirectory, GetSaveGameDirectory(GetHwnd(), VGetGameAppDirectory()));

	// DXUTCreateDevice - Chapter 5 - page 139
	DXUTCreateDevice( true, screenWidth, screenHeight);

	// You usually must have an HWND to initialize your game views...
	m_pGame = VCreateGameAndView();
	if (!m_pGame)
		return false;

	// initialize the font system
	m_pFontHandler = GCC_NEW FontHandler();


	CheckForJoystick(GetHwnd());

	m_bIsRunning = true;

	return TRUE;
}



void GameCodeApp::ParseCommandLine(LPWSTR lpCmdLine)
{
	WCHAR *playersCmd = _T("-players");
	WCHAR *gameHostCmd = _T("-gamehost");
	WCHAR *listenCmd = _T("-listen");
	WCHAR *numAICmd = _T("-numai");
	WCHAR *c;
	WCHAR temp[_MAX_PATH];

	if (c=_tcsstr(lpCmdLine, playersCmd))
	{
		_tcscpy(temp, c);
		c = temp;
		c = _tcstok(temp, _T(" "));		// get -players command
		c = _tcstok(NULL, _T(" "));
		int players = _tstoi(c);
		if (players>6 || players<1)
		{
			assert(0 && "Invalid number of players on the command line");
			players = 1;
		}

		m_pOptions->m_expectedPlayers = players;
	}

	if (c=_tcsstr(lpCmdLine, gameHostCmd))
	{
		_tcscpy(temp, c);
		c = temp;
		c = _tcstok(temp, _T(" "));		// get -hostname command
		c = _tcstok(NULL, _T(" "));		// get to hostname
		if (c)
		{
			CHAR gameHost[256];
			GenericToAnsiCch(gameHost, c, static_cast<int>(_tcslen(c)+1));
			m_pOptions->m_gameHost = gameHost;
		}
		c = _tcstok(NULL, _T(" "));		// get to port number
		if (c)
		{
			int listenPort = _tstoi(c);
			if (listenPort>0 && listenPort<65535)
			{
				// the game options already has the default port set...
				m_pOptions->m_listenPort = listenPort;
			}
		}		
		assert(m_pOptions->m_listenPort!=-1 && _T("Must set server port in command line"));
	}

	if (c=_tcsstr(lpCmdLine, listenCmd))
	{
		_tcscpy(temp, c);
		c = temp;
		c = _tcstok(temp, _T(" "));		// get -listed command
		c = _tcstok(NULL, _T(" "));		// get to port number
		int listenPort = _tstoi(c);
		if (listenPort>0 && listenPort<65535)
		{
			m_pOptions->m_listenPort = listenPort;
		}
	}

	if (c=_tcsstr(lpCmdLine, numAICmd))
	{
		_tcscpy(temp, c);
		c = temp;
		c = _tcstok(temp, _T(" "));		// get -numai command
		c = _tcstok(NULL, _T(" "));		// get to port number
		int numAIs = _tstoi(c);
		if (numAIs>=0 && numAIs<=8)
		{
			m_pOptions->m_numAIs = numAIs;
		}
	}

}


//----------------------------------------------------------
// GetString
//
// Bonus code - not described in the book.
//
// creates a string from a string resource ID in the string table
// stored in a special DLL, LANG.DLL, so game text strings
// can be language independant
//
std::wstring GameCodeApp::GetString(const int nID)
{
	std::wstring s = _T("");
	BOOL idFound = false;
	if (m_LangDll)
	{
		TCHAR temp[2048];
		// first search in the lang_dll resources, if one has been specified
		idFound = ::LoadString(m_LangDll, nID, temp, 2048);
		if (idFound)
		{
			s = temp;
		}
	}

	assert(s.length() && _T("String not found!"));
	return s;
}


//----------------------------------------------------------
// Win32 Specific Message Handlers
//
// WndProc - the main message handler for the window class
//
// OnNcCreate - this is where you can set window data before it is created
// OnMove - called whenever the window moves; used to update members of g_App
// OnDeviceChange - called whenever you eject the CD-ROM.
// OnDisplayChange - called whenever the user changes the desktop settings
// OnPowerBroadcast - called whenever a power message forces a shutdown
// OnActivate - called whenever windows on the desktop change focus.
//
// Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
LRESULT CALLBACK GameCodeApp::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_pApp->g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	LRESULT result = 0;

	switch (uMsg) 
	{
		/**************
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_ACTIVATE:
		//  Changes made post-press:
		//    This message is actually handled by the DirectX 9 Framework.
		//    You could still intercept it, and handle things like pausing 
		//    your game audio - but you should make sure DirectX 9 gets it too, 
		//    or wierd things will happen.
			break;

		case WM_MOVE:
		//  Changes made post-press:
		//    The WM_MOVE message as described in the book had a little more to do
		//    with a particular quirk of the sprite engine used in the first edition.
		//    You don't have to handle it if you don't want to.
			break;

		**************************/

		case WM_DEVICECHANGE:
		{
			int event = (int)wParam;
			result = g_pApp->OnDeviceChange(event);
			break;
		}

		case WM_POWERBROADCAST:
		{
			int event = (int)wParam;
			result = g_pApp->OnPowerBroadcast(event);
			break;
		}

		case WM_DISPLAYCHANGE:
		{
			int colorDepth = (int)wParam;
			int width = (int)(short) LOWORD(lParam);
			int height = (int)(short) HIWORD(lParam);

			result = g_pApp->OnDisplayChange(colorDepth, width, height);
			break;
		}

		case WM_SYSCOMMAND: 
		{
			result = g_pApp->OnSysCommand(wParam, lParam);
			if (result)
			{
				*pbNoFurtherProcessing = true;
			}
			break;
		}

		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_RETURN)
			{
				*pbNoFurtherProcessing = true;
				return g_pApp->OnAltEnter();
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}


		case WM_CLOSE:
		{
			// DXUT apps choose ESC key as a default exit command.
			// GameCode3 doesn't like this so we disable it by checking 
			// the m_bQuitting bool, and if we're not really quitting
			// set the "no further processing" parameter to true.
			if (g_pApp->m_bQuitting)
			{
				result = g_pApp->OnClose();
			}
			else
			{
				*pbNoFurtherProcessing = true;
			}
			break;
		}


		case WM_KEYDOWN:
        case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case MM_JOY1BUTTONDOWN:
		case MM_JOY1BUTTONUP:
		case MM_JOY1MOVE:
		case MM_JOY1ZMOVE:
		case MM_JOY2BUTTONDOWN:
		case MM_JOY2BUTTONUP:
		case MM_JOY2MOVE:
		case MM_JOY2ZMOVE:
		{
			//
			// See Chapter 9, page 265 for more explanation of this code.
			//
			if (g_pApp->m_pGame)
			{
				BaseGameLogic *pGame = g_pApp->m_pGame;
				// Note the reverse order! User input is grabbed first from the view that is on top, 
				// which is the last one in the list.
				AppMsg msg;
				msg.m_hWnd = hWnd;
				msg.m_uMsg = uMsg;
				msg.m_wParam = wParam;
				msg.m_lParam = lParam;
				for(GameViewList::reverse_iterator i=pGame->m_gameViews.rbegin(); i!=pGame->m_gameViews.rend(); ++i)
				{
					if ( (*i)->VOnMsgProc( msg ) )
					{
						result = true;
						break;				// WARNING! This breaks out of the for loop.
					}
				}
			}
			break;
		}

		/**********************
		WARNING!!!!! You MIGHT think you need this, but if you use the DirectX
		Framework the DefWindowProc is called for you....

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

		***********************/
	}

	return result;
}


//=========================================================
// GameCodeApp::OnNcCreate
//
// Handles the WM_NCCREATE message
//
// Not described in the book
// It sets a few members of the CREATESTRUCT, so the 
// window looks like we want it to look.
//=========================================================

LRESULT GameCodeApp::OnNcCreate(LPCREATESTRUCT cs)
{
	// TODO: If you want to override something in the CREATESTRUCT, do it here!
	// You'll usually do something like change window borders, etc.
	return true;
}


//=========================================================
// GameCodeApp::OnDeviceChange - Chapter 17, page 654
//
// Handles the WM_DEVICECHANGE message
//=========================================================

LRESULT GameCodeApp::OnDeviceChange(int eventType)
{			
	if ( IsMinimumInstall() )		// reads game data to find out
	{
		if ( (UINT) eventType == DBT_DEVICEREMOVECOMPLETE )
		{
			TCHAR *searchFile = CDCheckFile();		
			while (!FileExists(searchFile))
			{
				if (GameCodeApp::Ask(QUESTION_WHERES_THE_CD)==IDCANCEL)
				{
					AbortGame();
				}
			}
			return BROADCAST_QUERY_DENY;	// denied!
		}
	}
	
	return TRUE;
}


//=========================================================
// GameCodeApp::OnDisplayChange - Chapter 17, page 656
//
// Handles the WM_DISPLAYCHANGE message
//
//=========================================================

LRESULT GameCodeApp::OnDisplayChange(int colorDepth, int width, int height)
{
	m_rcDesktop.left = 0;
	m_rcDesktop.top = 0; 
	m_rcDesktop.right = width;
	m_rcDesktop.bottom = height;
	m_iColorDepth = colorDepth;
	return 0;
}

//=========================================================
// GameCodeApp::OnPowerBroadcast - Chapter 17, page 656
//
// Handles the WM_POWERBROADCAST message
//
//=========================================================

LRESULT GameCodeApp::OnPowerBroadcast(int event)
{
	// Don't allow the game to go into sleep mode
	if ( event == PBT_APMQUERYSUSPEND )
		return BROADCAST_QUERY_DENY;
	else if ( event == PBT_APMBATTERYLOW )
	{
		AbortGame();
	}

	return true;
}

//=========================================================
// GameCodeApp::OnSysCommand - Chapter 17, page 653
//
// Handles the WM_SYSCOMMAND message
//
//=========================================================

LRESULT GameCodeApp::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case SC_MAXIMIZE :
		{
			// If windowed and ready...
			if ( m_bWindowedMode && IsRunning() )
			{
				// Make maximize into FULLSCREEN toggle
				OnAltEnter();
			}
		}
		return 0;

		case SC_CLOSE :
		{
			// The quit dialog confirmation would appear once for
			// every SC_CLOSE we get - which happens multiple times
			// if modal dialogs are up.  This now uses the QUIT_NO_PROMPT
			// flag to only prompt when receiving a SC_CLOSE that isn't
			// generated by us (identified by QUIT_NO_PROMPT).
			
			// If closing, prompt to close if this isn't a forced quit
			if ( lParam != QUIT_NO_PROMPT )
			{
				// ET - 05/21/01 - Bug #1916 - Begin
				// We were receiving multiple close dialogs
				// when closing again ALT-F4 while the close
				// confirmation dialog was up.
				// Eat if already servicing a close
				if ( m_bQuitRequested )
					return true;

				// Wait for the application to be restored
				// before going any further with the new 
				// screen.  Flash until the person selects
				// that they want to restore the game, then
				// reinit the display if fullscreen.  
				// The reinit is necessary otherwise the game
				// will switch to windowed mode.

				// TODO MrMike: Need a message eater, message saver
				//if (postableMessageBacklog.valid())
				//{
				//	postableMessageBacklog->Add( PostableMessage(WM_SYSCOMMAND, wParam, MAKELPARAM(0, 0) ) );
				//	return true;
				//}
		
				// Quit requested
				m_bQuitRequested = true;
				// Prompt
				if ( GameCodeApp::Ask(QUESTION_QUIT_GAME) == IDNO )
				{
					// Bail - quit aborted
					
					// Reset quit requested flag
					m_bQuitRequested = false;
					
					return true;
				}
			}
	
			m_bQuitting = true;

			// Is there a game modal dialog up?
			if ( HasModalDialog() )
			{
				// Close the modal
				// and keep posting close to the app
				ForceModalExit();

				// Reissue the close to the app
				
				// Issue the new close after handling the current one,
				// but send in QUIT_NO_PROMPT to differentiate it from a 
				// regular CLOSE issued by the system.
				PostMessage( GetHwnd(), WM_SYSCOMMAND, SC_CLOSE, QUIT_NO_PROMPT );

				m_bQuitRequested = false;
				
				// Eat the close
				return true;
			}

			// Reset the quit after any other dialogs have popped up from this close
			m_bQuitRequested = false;
		}
		return 0;

		default:
			// return non-zero of we didn't process the SYSCOMMAND message
			return DefWindowProc(GetHwnd(), WM_SYSCOMMAND, wParam, lParam);
	}

	return 0;
}

//=========================================================
// GameCodeApp::OnClose - Chapter 5, page 154
//
// Handles the WM_CLOSE message
//
//=========================================================

LRESULT GameCodeApp::OnClose()
{
	// release all the game systems in reverse order from which they were created

	SAFE_DELETE(m_pGame);

	SAFE_DELETE(m_pFontHandler);

	SAFE_DELETE(g_pAudio);

	DestroyWindow(GetHwnd());

	SAFE_DELETE(m_pBaseSocketManager);

	SAFE_DELETE(m_pEventManager);

	SAFE_DELETE(m_pLuaStateManager);

	SAFE_DELETE(m_ResCache);

	SAFE_DELETE(m_pOptions);

	// MrMike: 12-Apr-2009
	//    This is a good spot to remove the views....


	return 0;
}



//=========================================================
// GameCodeApp::FlashWhileMinimized - Chapter 5, page 144
//
// Wait for the application to be restored
// before going any further with the new 
// screen.  Flash until the person selects
// that they want to restore the game.
//
//=========================================================

void GameCodeApp::FlashWhileMinimized()
{
	// Flash the application on the taskbar
	// until it's restored.
	if ( ! GetHwnd() )
		return;
	
	// Blink the application if we are minimized,
	// waiting until we are no longer minimized
	if (IsIconic(GetHwnd()) )
	{
		// Make sure the app is up when creating a new screen
		// this should be the case most of the time, but when
		// we close the app down, minimized, and a confirmation
		// dialog appears, we need to restore
		DWORD now = timeGetTime();
		DWORD then = now;
		MSG msg;
		
		FlashWindow( GetHwnd(), true );
		
		for (;;)
		{
			if ( PeekMessage( &msg, NULL, 0, 0, 0 ) )
			{
				if ( msg.message != WM_SYSCOMMAND || msg.wParam != SC_CLOSE )
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				
				// Are we done?
				if ( ! IsIconic(GetHwnd()) )
				{
					FlashWindow( GetHwnd(), false );
					break;
				}
			}
			else
			{
				now = timeGetTime();
				DWORD timeSpan = now > then ? (now - then) : (then - now);
				if ( timeSpan > 1000 )
				{
					then = now;
					FlashWindow( GetHwnd(), true );
				}
			}
		}
	}
}


int GameCodeApp::Ask(const int question)
{
	std::wstring msg;
	std::wstring title;
	int buttonFlags;
	int defaultAnswer = IDOK;

	switch(question)
	{
		case QUESTION_WHERES_THE_CD:
		{
			msg = g_pApp->GetString(IDS_QUESTION_WHERES_THE_CD);
			title = g_pApp->GetString(IDS_ALERT);
			buttonFlags = MB_RETRYCANCEL|MB_ICONEXCLAMATION;
			defaultAnswer = IDCANCEL;
			break;
		}
		case QUESTION_QUIT_GAME:
		{
			msg = g_pApp->GetString(IDS_QUESTION_QUIT_GAME);
			title = g_pApp->GetString(IDS_QUESTION);
			buttonFlags = MB_YESNO|MB_ICONQUESTION;
			defaultAnswer = IDNO;
			break;
		}
		default:
			assert(0 && _T("Undefined question in Game::Ask"));
			return IDCANCEL;
	}

	if (g_pApp && g_pApp->IsRunning())
	{
		ShowCursor(true);
		shared_ptr<CMessageBox> pMessageBox(new CMessageBox(msg, title, buttonFlags));
		int result = g_pApp->Modal(pMessageBox, defaultAnswer);
		ShowCursor(false);
		return result;
	}

	return MessageBox(g_pApp ? g_pApp->GetHwnd() : NULL, msg.c_str(), title.c_str(), buttonFlags);
}


//=========================================================
// GameCodeApp::OnAltEnter
//
// Called when the player hits Alt-Enter to flip the 
// display mode.
//
// Not discussed in the book.
//=========================================================

LRESULT GameCodeApp::OnAltEnter()
{
	DXUTToggleFullScreen();
	return 0;
}


//
// class GameCodeApp::Modal - Chapter 9, page 255
//
int GameCodeApp::Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer)
{
	// If we're going to display a dialog box, we need a human view 
	// to interact with.
	HumanView *pView;
	for(GameViewList::iterator i=m_pGame->m_gameViews.begin(); i!=m_pGame->m_gameViews.end(); ++i)
	{
		if ((*i)->VGetType()==GameView_Human)
		{
			shared_ptr<IGameView> pIGameView(*i);
			pView = static_cast<HumanView *>(&*pIGameView);
		}
	}

	if (!pView)
	{
		// Whoops! There's no human view attached.
		return defaultAnswer;
	}

	if (m_HasModalDialog & 0x10000000)
	{
		assert(0 && "Too Many nested dialogs!");
		return defaultAnswer;
	}
	
	assert(GetHwnd() != NULL && _T("Main Window is NULL!"));
	if ( ( GetHwnd() != NULL ) && IsIconic(GetHwnd()) )
	{
		FlashWhileMinimized();
	}

	m_HasModalDialog <<= 1;
	m_HasModalDialog |= 1;

	pView->VPushElement(pModalScreen);

	LPARAM lParam = 0;
	int result = PumpUntilMessage(MSG_END_MODAL, NULL, &lParam);
	
	if (lParam != 0)
	{
		if (lParam==QUIT_NO_PROMPT)
			result = defaultAnswer;
		else	
			result = (int)lParam;
	}

	pView->VPopElement(pModalScreen);
	m_HasModalDialog >>= 1;

	return result;
}

//
// class GameCodeApp::PumpUntilMessage - Chapter 9, page 257-258
//
int GameCodeApp::PumpUntilMessage (UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam)
{
	int currentTime = timeGetTime();
	MSG msg;
	for ( ;; )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if (msg.message == WM_CLOSE)
			{
				m_bQuitting = true;
				GetMessage(& msg, NULL, 0, 0);
				break;
			}
			else
			{
				// Default processing
				if (GetMessage(&msg, NULL, NULL, NULL))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Are we done?
				if ( msg.message == msgEnd)
					break;
			}
		}
		else
		{
			// Update the game views, but nothing else!
			// Remember this is a modal screen.
			if (m_pGame)
			{
				int timeNow = timeGetTime();
				int deltaMilliseconds = timeNow - currentTime;
				for(GameViewList::iterator i=m_pGame->m_gameViews.begin(); i!=m_pGame->m_gameViews.end(); ++i)
				{
					(*i)->VOnUpdate( deltaMilliseconds );
				}
				currentTime = timeNow;
				DXUTRender3DEnvironment();
			}
		}
	}
	if (pLParam)
		*pLParam = msg.lParam;
	if (pWParam)
		*pWParam = msg.wParam;

	return 0;
}


//This function removes all of a *SPECIFIC* type of message from the queue...
int	GameCodeApp::EatSpecificMessages( UINT msgType, optional<LPARAM> lParam, optional<WPARAM> wParam)
{
	bool done = false;

	while (!done)
	{
		MSG msg;

		if ( PeekMessage( &msg, NULL, msgType, msgType, PM_NOREMOVE ) )
		{
			bool valid = true;

			if (lParam.valid())
			{
				valid &= (*lParam==msg.lParam);
			}

			if (wParam.valid())
			{
				valid &= (*wParam==msg.wParam);
			}

			if (valid)
			{
				//Soak!
				GetMessage(& msg, NULL, msgType, msgType);
			}
			else
			{
				done = true;
			}
		}
		else
		{
			done = true;	//No more messages!
		}
	}

	return 0;
}




//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK GameCodeApp::OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC*, void* pUserContext  )
{
	HRESULT hr;
	V_RETURN( g_pApp->g_DialogResourceManager.OnD3D9ResetDevice( ) );

	if (g_pApp->m_pGame)
	{
		BaseGameLogic *pGame = g_pApp->m_pGame;
		for(GameViewList::iterator i=pGame->m_gameViews.begin(); i!=pGame->m_gameViews.end(); ++i)
		{
			V_RETURN ( (*i)->VOnRestore() );
		}
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all 
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for 
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK GameCodeApp::OnLostDevice(void* pUserContext )
{
    g_pApp->g_DialogResourceManager.OnD3D9LostDevice();

	if (g_pApp->m_pGame)
	{
		BaseGameLogic *pGame = g_pApp->m_pGame;
		for(GameViewList::iterator i=pGame->m_gameViews.begin(); i!=pGame->m_gameViews.end(); ++i)
		{
			(*i)->VOnLostDevice();
		}
	}
}




//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//
// See Game Coding Complete - 3rd Edition - Chapter 6 - page 153
//--------------------------------------------------------------------------------------
void CALLBACK GameCodeApp::OnUpdateGame( double fTime, float fElapsedTime, void* pUserContext  )
{
	if (g_pApp->HasModalDialog())
	{	
		// don't update the game if a modal dialog is up.
		return;
	}

	if (g_pApp->m_bQuitting)
	{
		PostMessage(g_pApp->GetHwnd(), WM_CLOSE, 0, 0);		
	}

	if (g_pApp->m_pGame)
	{
		safeTickEventManager( 20 ); // allow event queue to process for up to 20 ms

		if (g_pApp->m_pBaseSocketManager)
			g_pApp->m_pBaseSocketManager->DoSelect(0);	// pause 0 microseconds

		g_pApp->m_pGame->VOnUpdate(fTime, fElapsedTime);
	}
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//
// See Game Coding Complete - 3rd Edition - Chapter 6 - page 154
//--------------------------------------------------------------------------------------
void CALLBACK GameCodeApp::OnRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext  )
{
	BaseGameLogic *pGame = g_pApp->m_pGame;

	for(GameViewList::iterator i=pGame->m_gameViews.begin(),
		end=pGame->m_gameViews.end(); i!=end; ++i)
	{
		(*i)->VOnRender(fTime, fElapsedTime);
	}

	g_pApp->m_pGame->VRenderDiagnostics();
}





HRESULT CALLBACK GameCodeApp::OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;
    V_RETURN( g_pApp->g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
	return S_OK;
}

void CALLBACK GameCodeApp::OnDestroyDevice( void* pUserContext )
{
	g_pApp->g_DialogResourceManager.OnD3D9DestroyDevice( );
}

// This function registers all base game event types.
// This limits the accessability of an event, such as
// whether it can be created by script.
void GameCodeApp::RegisterBaseGameEvents( void )
{
	if ( NULL == m_pEventManager )
	{
		assert( 0 && "Cannot register base game events until the event manager exists!" );
		return;
	}

	//Physics events...
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_PhysTrigger_Enter::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_PhysTrigger_Leave::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_PhysCollision::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_PhysSeparation::sk_EventType );

	//Actor events...
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_New_Actor::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Destroy_Actor::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Move_Actor::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Game_State::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Remote_Client::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Network_Player_Actor_Assignment::sk_EventType );
	
	// AI events....
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_AiSteer::sk_EventType);

	//General game events...
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Update_Tick::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Debug_String::sk_EventType );

	// Decompression process events...
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Decompress_Request::sk_EventType );
//	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Decompression_Progress::sk_EventType );
}


//========================================================================
//
// BaseActor implementation
//
// GROSSSSS!!!!! Whoa I can't put that in the book!!!!!
// TODO FIX THIS POOP!
//
//    Note from MrMike - LOL its funny how stuff like this always ends up
//                       in the shipping source code. 
//                       If you figure out why this is a pile of POOP, post
//                       this on the forums.
//========================================================================
const float ACTOR_ANGULAR_VELOCITY = 3.0f;  // 3 radians per second
void BaseActor::VRotateY(float angleRadians)
{
	angleRadians = WrapPi(angleRadians);
	shared_ptr<IGamePhysics> pPhysics = g_pApp->m_pGame->VGetGamePhysics();
	float diff = angleRadians - pPhysics->VGetOrientationY(m_id);
	float timeMs = fabs(diff / ACTOR_ANGULAR_VELOCITY);

	EvtData_AiSteer* pEventData = GCC_NEW EvtData_AiSteer(m_id,diff,timeMs);
	safeQueEvent(IEventDataPtr(pEventData));
}


//========================================================================
//
// BaseGameLogic implementation
//
//========================================================================

#include <mmsystem.h>
#include "MainLoop\CProcess.h"

const unsigned int SCREEN_REFRESH_RATE(1000/60);


BaseGameLogic::BaseGameLogic(struct GameOptions const &options)
{
	m_LastActorId = 0;
	m_pProcessManager = GCC_NEW CProcessManager;
	m_random.Randomize();
	m_State = BGS_Initializing;
	m_bProxy = false;
	m_RenderDiagnostics = false;
	m_ExpectedPlayers = 0;
	m_ExpectedRemotePlayers = 0;
	m_ExpectedAI = 0;
	m_pPathingGraph.reset(CreatePathingGraph());

	m_pAiEventListener = EventListenerPtr (GCC_NEW AiEventListener ( ));
	safeAddListener(m_pAiEventListener, EvtData_AiSteer::sk_EventType);
}


BaseGameLogic::~BaseGameLogic()
{
	// MrMike: 12-Apr-2009 
	//   Added this to explicity remove views from the game logic list.

	while (!m_gameViews.empty())
		m_gameViews.pop_front();

	SAFE_DELETE(m_pProcessManager);

	assert (m_ActorList.empty() && _T("You should destroy the actor list in the inherited class!") );
}

ActorId BaseGameLogic::GetRandomActor(optional<ActorId> ignoreMe)
{
	// [rez] This function is very lame, but it's past 5am and I have a deadline.  Welcome to gaming industry.  ;)
	
	// choose a random element
	unsigned int count = (unsigned int)m_ActorList.size();
	unsigned int choice = m_random.Random(count);
	
	// since this is a map, we have to walk the tree to find the item in question
	ActorMap::iterator it = m_ActorList.begin();
	for (unsigned int i = 0; i < choice; i++)
		++it;
	
	// If we're ignoring an actor, choose the next one.  incidentally, this makes the next actor the most likely 
	// choice to be chosen, although the player won't notice in a game of a dozen or so actors.
	if (ignoreMe.valid() && it->first == *ignoreMe)
	{
		++it;
		if (it == m_ActorList.end())
			it = m_ActorList.begin();
	}
	
	// found someone
	return it->first;
}

//
// BaseGameLogic::VAddActor				- Chapter 19, page 750
//
void BaseGameLogic::VAddActor(shared_ptr<IActor> actor, ActorParams *p)
{
	optional< ActorId > destActorID = p->m_Id;
	assert( destActorID.valid() && "Attempted to add an actor with no actor ID!" );
	m_ActorList[ *destActorID ] = actor;
	actor->VSetID( *destActorID );
}


//
// BaseGameLogic::VRemoveActor				- Chapter 19, page 751
//
void BaseGameLogic::VRemoveActor(ActorId aid)
{
	if (m_bProxy)
		return;

	if( NULL!=VGetActor(aid).get() )
	{
		m_pPhysics->VRemoveActor(aid);
		m_ActorList.erase(aid);
	}
	else
	{
		assert(0 && _T("Unknown actor!"));
	}
}


void BaseGameLogic::VMoveActor(const ActorId id, Mat4x4 const &mat)
{
	shared_ptr<IActor> pActor = VGetActor(id);
	if (pActor)
	{
		pActor->VSetMat(mat);
	}
}

shared_ptr<IActor> BaseGameLogic::VGetActor(const ActorId id)
{
	ActorMap::iterator i = m_ActorList.find(id);
	if (i==m_ActorList.end())
	{
		shared_ptr<IActor> null;
		return null;
	}

	return (*i).second;
}

void BaseGameLogic::VOnUpdate(float time, float elapsedTime)
{
	int deltaMilliseconds = int(elapsedTime * 1000.0f);

	switch(m_State)
	{
		case BGS_Initializing:
			// If we get to here we're ready to attach players
			VChangeState(BGS_MainMenu);
			break;

		case BGS_LoadingGameEnvironment:
			if (g_pApp->VLoadGame())
			{	
				VChangeState(BGS_SpawnAI);
			}
			else
			{
				assert(0 && _T("The game failed to load."));
				g_pApp->AbortGame();
			}
			break;

		case BGS_MainMenu:
			// do nothing
			break;

		case BGS_WaitingForPlayers:
			// do nothing - the game class should handle this one.
			break;

		case BGS_SpawnAI:
			if (this->m_ExpectedAI == 0)
			{
				// the base game logic doesn't spawn AI - your child class will do that.
				// in the case no AI are coming, the base game logic will go ahead and move to 
				// the next state.
				VChangeState(BGS_Running);
			}
			break;

		case BGS_Running:
			m_pProcessManager->UpdateProcesses(deltaMilliseconds);
			break;

		default:
			assert(0 && _T("Unrecognized state."));
			// Not a bad idea to throw an exception here to 
			// catch this in a release build...
	}

	GameViewList::iterator i=m_gameViews.begin();
	GameViewList::iterator end=m_gameViews.end();
	while (i != end)
	{
		(*i)->VOnUpdate( deltaMilliseconds );
		++i;
	}

	//Trigger an update event for anybody else (script functions, etc.).

}

//
// BaseGameLogic::VChangeState				- Chapter 19, page 710
//
void BaseGameLogic::VChangeState(BaseGameState newState)
{
	if (newState==BGS_WaitingForPlayers)
	{
		// Get rid of the Main Menu...
		m_gameViews.pop_front();

		// Note: Split screen support would require this to change!
		m_ExpectedPlayers = 1;
		m_ExpectedRemotePlayers = g_pApp->m_pOptions->m_expectedPlayers - 1;
		m_ExpectedAI = g_pApp->m_pOptions->m_numAIs;

		if (!g_pApp->m_pOptions->m_gameHost.empty())
		{
			VSetProxy();
			m_ExpectedAI = 0;			// the server will create these
			m_ExpectedRemotePlayers = 0;	// the server will create these
			ClientSocketManager *pClient = GCC_NEW ClientSocketManager(g_pApp->m_pOptions->m_gameHost, g_pApp->m_pOptions->m_listenPort);
			if (!pClient->Connect())
			{
				// Throw up a main menu
				VChangeState(BGS_MainMenu);
				return;
			}
			g_pApp->m_pBaseSocketManager = pClient;
		}
		else if (m_ExpectedRemotePlayers > 0)
		{
			BaseSocketManager *pServer = GCC_NEW BaseSocketManager();
			if (!pServer->Init())
			{
				// Throw up a main menu
				VChangeState(BGS_MainMenu);	
				return;
			}

			pServer->AddSocket(new GameServerListenSocket(g_pApp->m_pOptions->m_listenPort));
			g_pApp->m_pBaseSocketManager = pServer;
		}
	}

	m_State = newState;
	if (!m_bProxy)
	{
		safeQueEvent( IEventDataPtr(GCC_NEW EvtData_Game_State(m_State)) );
	}
}


// Chapter 19/20 refactor work
//  -- refactor VBuildInitialScene into BaseGameLogic class
void BaseGameLogic::VBuildInitialScene()
{
	//Execute our startup script file first.
	const bool bStartupScriptSuccess = g_pApp->m_pLuaStateManager->DoFile( "data\\Scripts\\startup.lua" );
	if ( false == bStartupScriptSuccess )
	{
		assert( 0 && "Unable to execute startup.lua!" );
	}
}

// Chapter 19/20 refactor work
//   - move physics pointer into BaseGameLogic
//   - but make the inherited logic choose the implementation
void BaseGameLogic::VRenderDiagnostics() 
{ 
	if (m_RenderDiagnostics)
	{
		m_pPhysics->VRenderDiagnostics();
	}
}




void BaseGameLogic::TogglePause(bool active)
{
	// TODO This whole body of code belongs in the view, not the game!
	// AND it should fire off a pause event to all listeners.

	// Pause or resume audio	
	if ( active )
	{
		//ResetTimer();
		if (g_pAudio) 
			g_pAudio->VPauseAllSounds();
	}
	else
	{
		if (g_pAudio)
			g_pAudio->VResumeAllSounds();
		//SaveBackground();
	}
}


void BaseGameLogic::VAddView(shared_ptr<IGameView> pView, optional<ActorId> actorId)
{
	// This makes sure that all views have a non-zero view id.
	int viewId = static_cast<int>(m_gameViews.size());
	m_gameViews.push_back(pView);
	pView->VOnAttach(viewId, actorId);
	pView->VOnRestore();
}


void BaseGameLogic::VRemoveView(shared_ptr<IGameView> pView)
{
	m_gameViews.remove(pView);
}


//
// class CMessageBox::OnGUIEvent - Chapter 9, page 254 and page 259
//
//    Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
void CALLBACK CMessageBox::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext )
{
	PostMessage(g_pApp->GetHwnd(), MSG_END_MODAL, 0, nControlID);
}

//
// class CMessageBox::CMessageBox - Chapter 9, page 250
//
CMessageBox::CMessageBox(std::wstring msg, std::wstring title, int buttonFlags)
{
	// Initialize dialogs
	m_UI.Init( &g_pApp->g_DialogResourceManager );
    m_UI.SetCallback( OnGUIEvent ); 

	// Find the dimensions of the message
 	RECT rc;
	SetRect( &rc, 0,0,0,0);
	m_UI.CalcTextRect( msg.c_str(), m_UI.GetDefaultElement(DXUT_CONTROL_STATIC,0), &rc );
    int msgWidth = rc.right - rc.left;
    int msgHeight = rc.bottom - rc.top;

	int numButtons = 2;
	if ( (buttonFlags == MB_ABORTRETRYIGNORE) ||
		(buttonFlags == MB_CANCELTRYCONTINUE) ||
		(buttonFlags == MB_CANCELTRYCONTINUE) )
	{
		numButtons = 3;
	}
	else if (buttonFlags == MB_OK)
	{
		numButtons = 1;
	}

	int btnWidth = 125;
	int btnHeight = 22;

	int border = 35;
	m_Width = std::max(msgWidth + 2 * border, btnWidth + 2 * border);
	m_Height = msgHeight + (numButtons * (btnHeight+border) ) + (2 * border);

	m_PosX = (DXUTGetD3D9BackBufferSurfaceDesc()->Width-m_Width)/2;
	m_PosY = (DXUTGetD3D9BackBufferSurfaceDesc()->Height-m_Height)/2;
	m_UI.SetLocation( m_PosX, m_PosY );

	m_UI.SetSize( m_Width, m_Height );
	m_UI.SetBackgroundColors(g_Gray40);

	/***
	RECT bg;
	SetRect( &bg, 0, 0, m_Width, m_Height );
	m_UI.AddStatic( -1, _T(""), bg.left, bg.top, bg.right, bg.bottom);
	CDXUTStatic *pCreated = m_UI.GetStatic(-1);
	if (pCreated) 
	{
		CDXUTElement *element = pCreated->GetElement(0);
		element->SetTexture( 0, &bg );
	}
	***/

	int iY = border; 
	int iX = (m_Width - msgWidth) / 2; 

	m_UI.AddStatic( 0, msg.c_str(), iX, iY, msgWidth, msgHeight);

	iX = (m_Width - btnWidth) / 2;
	iY = m_Height - btnHeight - border;

	buttonFlags &= 0xF;
	if ( (buttonFlags == MB_ABORTRETRYIGNORE) ||
		 (buttonFlags == MB_CANCELTRYCONTINUE) )

	{
		// The message box contains three push buttons: Cancel, Try Again, Continue. 
		// This is the new standard over Abort,Retry,Ignore
		m_UI.AddButton( IDCONTINUE, g_pApp->GetString(IDS_CONTINUE).c_str(), iX, iY - (2*border), btnWidth, btnHeight );
		m_UI.AddButton( IDTRYAGAIN, g_pApp->GetString(IDS_TRYAGAIN).c_str(), iX, iY - border, btnWidth, btnHeight );
		m_UI.AddButton( IDCANCEL, g_pApp->GetString(IDS_CANCEL).c_str(), iX, iY, btnWidth, btnHeight );
	}
	else if (buttonFlags == MB_OKCANCEL)
	{
		//The message box contains two push buttons: OK and Cancel.
		m_UI.AddButton( IDOK, g_pApp->GetString(IDS_OK).c_str(), iX, iY - border, btnWidth, btnHeight );
		m_UI.AddButton( IDCANCEL, g_pApp->GetString(IDS_CANCEL).c_str(), iX, iY, btnWidth, btnHeight );
	}
	else if (buttonFlags == MB_RETRYCANCEL)
	{
		//The message box contains two push buttons: Retry and Cancel.
		m_UI.AddButton( IDRETRY, g_pApp->GetString(IDS_RETRY).c_str(), iX, iY - border, btnWidth, btnHeight );
		m_UI.AddButton( IDCANCEL, g_pApp->GetString(IDS_CANCEL).c_str(), iX, iY, btnWidth, btnHeight );
	}
	else if (buttonFlags == MB_YESNO)
	{
		//The message box contains two push buttons: Yes and No.
		m_UI.AddButton( IDYES, g_pApp->GetString(IDS_YES).c_str(), iX, iY - border, btnWidth, btnHeight );
		m_UI.AddButton( IDNO, g_pApp->GetString(IDS_NO).c_str(), iX, iY, btnWidth, btnHeight );
	}
	else if (buttonFlags == MB_YESNOCANCEL)
	{
		//The message box contains three push buttons: Yes, No, and Cancel.
		m_UI.AddButton( IDYES, g_pApp->GetString(IDS_YES).c_str(), iX, iY - (2*border), btnWidth, btnHeight );
		m_UI.AddButton( IDNO, g_pApp->GetString(IDS_NO).c_str(), iX, iY - border, btnWidth, btnHeight );
		m_UI.AddButton( IDCANCEL, g_pApp->GetString(IDS_CANCEL).c_str(), iX, iY, btnWidth, btnHeight );
	}
	else //if (buttonFlags & MB_OK)
	{
        // The message box contains one push button: OK. This is the default.
		m_UI.AddButton( IDOK, g_pApp->GetString(IDS_OK).c_str(), iX, iY, btnWidth, btnHeight );
	}
}

CMessageBox::~CMessageBox()
{
	assert(1);
}


//
// class CMessageBox::VOnRestore  - Chapter 9, page 253
//
HRESULT CMessageBox::VOnRestore()
{
    m_UI.SetLocation( m_PosX, m_PosY );
    m_UI.SetSize( m_Width, m_Height );
	return S_OK;
}


//
// class CMessageBox::VOnRender - Chapter 9, page 254
//
HRESULT CMessageBox::VOnRender(double fTime, float fElapsedTime)
{
	HRESULT hr;
	V( m_UI.OnRender( fElapsedTime ) );
	return S_OK;
};


//
// class CMessageBox::VOnMsgProc - Chapter 9, page 254
//
LRESULT CALLBACK CMessageBox::VOnMsgProc( AppMsg msg )
{
    return m_UI.MsgProc( msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam );
}



//
// HumanView::HumanView - Chapter 9
//
HumanView::HumanView()
{
	InitAudio(); 

	m_pProcessManager = GCC_NEW CProcessManager;

	m_pFont = NULL;         // Font for drawing text
	m_pTextSprite = NULL;   // Sprite for batching draw text calls
}


//
// HumanView::~HumanView - Chapter 9
//

HumanView::~HumanView()
{
	while (!m_ScreenElements.empty())
	{
		m_ScreenElements.pop_front();		
	}

    SAFE_RELEASE( m_pTextSprite );
	SAFE_RELEASE( m_pFont );

	SAFE_DELETE(m_pProcessManager);

	// post press - I changed the g_pAudio pointer to a shared_ptr
	// since there can be multiple human views
}


//
// HumanView::VOnRender - Chapter 9, page 237
//

void HumanView::VOnRender(double fTime, float fElapsedTime )
{
	m_currTick = timeGetTime();
	if (m_currTick == m_lastDraw)
		return;

	HRESULT hr;

	// It is time to draw ?
	if( m_runFullSpeed || ( (m_currTick - m_lastDraw) > SCREEN_REFRESH_RATE) )
	{
	    // Clear the render target and the zbuffer 
		V( DXUTGetD3D9Device()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );


	    // Render the scene
		if( SUCCEEDED( DXUTGetD3D9Device()->BeginScene() ) )
		{
			// The helper object simply helps keep track of text position, and color
			// and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
			// If NULL is passed in as the sprite object, then it will work however the 
			// pFont->DrawText() will not be batched together.  Batching calls will improves performance.

			CDXUTTextHelper txtHelper( m_pFont, m_pTextSprite, 15 );			
			VRenderText(txtHelper);

			m_ScreenElements.sort(SortBy_SharedPtr_Content<IScreenElement>());

			for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
			{
				if ( (*i)->VIsVisible() )
				{
					(*i)->VOnRender(fTime, fElapsedTime);
				}
			}

			// Let the console render.
			m_Console.Render(txtHelper);

			// record the last successful paint
			m_lastDraw = m_currTick;
		}

        V( DXUTGetD3D9Device()->EndScene() );
    }
}

//
// HumanView::VOnRestore - Chapter 9, page 238
//
HRESULT HumanView::VOnRestore()
{
	HRESULT hr = S_OK;

    if( !m_pFont )
	{
	    // Initialize the font
	    D3DXCreateFont( DXUTGetD3D9Device(), 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &m_pFont );
	}
	else
	{
        V_RETURN( m_pFont->OnResetDevice() );
	}

	if (!m_pTextSprite)
	{
		// Create a sprite to help batch calls when drawing many lines of text
		V_RETURN( D3DXCreateSprite( DXUTGetD3D9Device(), &m_pTextSprite ) );
	}
	else
	{
        V_RETURN( m_pTextSprite->OnResetDevice() );		
	}
	
	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		V_RETURN ( (*i)->VOnRestore() );
	}

	return hr;
}


//
// HumanView::VOnLostDevice - Chapter 9, page 239
//
void HumanView::VOnLostDevice() 
{

	if( m_pFont )
        m_pFont->OnLostDevice();
    SAFE_RELEASE( m_pTextSprite );
}



//
// HumanView::InitAudio - Chapter 9
//
bool HumanView::InitAudio()
{
	if (!g_pAudio)
	{
		g_pAudio = GCC_NEW DirectSoundAudio();		// use this line for DirectSound
		//g_pAudio = GCC_NEW CMilesAudio();			// use this line for Miles Audio
	}

	if (!g_pAudio)
		return false;

	if (!g_pAudio->VInitialize(g_pApp->GetHwnd()))
		return false;

	return true;
}


//
// HumanView::VOnMsgProc - Chapter 9, page 242
//
LRESULT CALLBACK HumanView::VOnMsgProc( AppMsg msg )
{
	// Iterate through the screen layers first
	// In reverse order since we'll send input messages to the 
	// screen on top
	for(ScreenElementList::reverse_iterator i=m_ScreenElements.rbegin(); i!=m_ScreenElements.rend(); ++i)
	{
		if ( (*i)->VIsVisible() )
		{
			if ( (*i)->VOnMsgProc( msg ) )
			{
				return 1;
			}
		}
	}

	LRESULT result = 0;
	switch (msg.m_uMsg) 
	{
        case WM_KEYDOWN:
			if (m_Console.IsActive())
			{
				// Let the console eat this.
			}
			else if (m_KeyboardHandler)
				result = m_KeyboardHandler->VOnKeyDown(static_cast<const BYTE>(msg.m_wParam));
			break;
	
        case WM_KEYUP:
			if (m_Console.IsActive())
			{
				// Let the console eat this.
			}
			else if (m_KeyboardHandler)
				result = m_KeyboardHandler->VOnKeyUp(static_cast<const BYTE>(msg.m_wParam));
			break;

		case WM_MOUSEMOVE:
			if (m_MouseHandler)
				result = m_MouseHandler->VOnMouseMove(CPoint(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)));
			break;

		case WM_LBUTTONDOWN:
			if (m_MouseHandler)
			{
				SetCapture(msg.m_hWnd);
				result = m_MouseHandler->VOnLButtonDown(CPoint(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)));
			}	
			break;

		case WM_LBUTTONUP:
			if (m_MouseHandler)
			{
				SetCapture(NULL);
				result = m_MouseHandler->VOnLButtonUp(CPoint(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)));
			}
			break;

		case WM_RBUTTONDOWN:
			if (m_MouseHandler)
			{
				SetCapture(msg.m_hWnd);
				result = m_MouseHandler->VOnRButtonDown(CPoint(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)));
			}
			break;

		case WM_RBUTTONUP:
			if (m_MouseHandler)
			{
				SetCapture(NULL);
				result = m_MouseHandler->VOnRButtonUp(CPoint(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)));
			}
			break;
		case WM_CHAR:
			if (m_Console.IsActive())
			{
				const unsigned int oemScan = int( msg.m_lParam & ( 0xff << 16 ) ) >> 16;
				m_Console.HandleKeyboardInput( msg.m_wParam, MapVirtualKey( oemScan, 1 ), true );
			}
			else
			{
				//See if it was the console key.
				if (('~'==msg.m_wParam) || ('`'==msg.m_wParam))
				{
					m_Console.SetActive(true);
				}
			}
			break;
		default:
			return 0;
	}

	return 0;
}

//
// HumanView::VOnRestore - Chapter 9, page 239
//
void HumanView::VOnUpdate( int deltaMilliseconds )
{
	m_pProcessManager->UpdateProcesses(deltaMilliseconds);

	m_Console.Update(deltaMilliseconds);

	// This section of code was added post-press. It runs through the screenlist
	// and calls VOnUpdate. Some screen elements need to update every frame, one 
	// example of this is a 3D scene attached to the human view.
	//
	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		(*i)->VOnUpdate(deltaMilliseconds);
	}
}

//
// HumanView::VPushElement - Chapter 9
// (note - this was renamed from HumanView::VPushScreen)
//
void HumanView::VPushElement(shared_ptr<IScreenElement> pElement)
{
	m_ScreenElements.push_front(pElement);
}

//
// HumanView::VPopElement - Chapter 9
// (note - this was renamed from HumanView::VPopScreen)
//    MrMike: 11-Apr-2009 - changed to solve a bug with modal dialogs.
void HumanView::VPopElement(shared_ptr<IScreenElement> pElement)
{
	// m_ScreenElements.pop_front();
	m_ScreenElements.remove(pElement);
}

// Begin definition of the HumanView::Console class.
const int kCursorBlinkTimeMS = 500;

char const * const kExitString = "exit";
char const * const kClearString = "clear";

HumanView::Console::Console()
: m_bActive( false )
, m_bExecuteStringOnUpdate( false )
{
	m_ConsoleInputSize = 48;

	m_CurrentInputString = std::string("");

	m_InputColor = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_OutputColor = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );

	m_CursorBlinkTimer = kCursorBlinkTimeMS;
	m_bCursorOn = true;
}

HumanView::Console::~Console()
{
}

void HumanView::Console::AddDisplayText( const std::string & newText )
{
	m_CurrentOutputString += newText;
	m_CurrentOutputString += '\n';
}

void HumanView::Console::SetDisplayText( const std::string & newText )
{
	m_CurrentOutputString = newText;
}

void HumanView::Console::Update( const int deltaMilliseconds )
{
	//Don't do anything if not active.
	if ( !m_bActive )
	{
		return;	//Bail!
	}

	//Do we have a string to execute?
	if ( true == m_bExecuteStringOnUpdate )
	{
		const std::string renderedInputString = std::string(">") + m_CurrentInputString;
		if ( 0 == m_CurrentInputString.compare( kExitString ) )
		{
			SetActive( false );
			m_CurrentInputString.clear();
		}
		else if ( 0 == m_CurrentInputString.compare( kClearString ) )
		{
			m_CurrentOutputString = renderedInputString;	//clear
			m_CurrentInputString.clear();
		}
		else
		{
			//Put the input string into the output window.
			AddDisplayText( renderedInputString );

			//Attempt to execute the current input string...
			const int retVal = g_pApp->m_pLuaStateManager->ExecuteString( m_CurrentInputString.c_str() );
			//Clear the input string
			m_CurrentInputString.clear();

			if ( 0 == retVal )
			{
			}
			else
			{
				AddDisplayText( "ERROR!\n" );
			}
		}

		//We're accepting input again.
		m_bExecuteStringOnUpdate = false;
	}

	//Update the cursor blink timer...
	m_CursorBlinkTimer -= deltaMilliseconds;

	if ( m_CursorBlinkTimer < 0 )
	{
		m_CursorBlinkTimer = 0;

		m_bCursorOn = !m_bCursorOn;

		m_CursorBlinkTimer = kCursorBlinkTimeMS;
	}
}

void HumanView::Console::Render( CDXUTTextHelper &txtHelper )
{
	//Don't do anything if not active.
	if ( !m_bActive )
	{
		return;	//Bail!
	}

	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
	txtHelper.Begin();
	const D3DXCOLOR white( 1.0f, 1.0f, 1.0f, 1.0f );
	const D3DXCOLOR black( 0.0f, 0.0f, 0.0f, 1.0f );
	RECT inputTextRect, outputTextRect, shadowRect;

	//Display the console text at screen top, below the other text displayed.
	const std::string finalInputString = std::string( ">" ) + m_CurrentInputString + ( m_bCursorOn ? '\xa0' : '_' );
	inputTextRect.left = 10;
	inputTextRect.right = pd3dsdBackBuffer->Width - 10;
	inputTextRect.top = 100;
	inputTextRect.bottom = pd3dsdBackBuffer->Height - 10;

	const int kNumWideChars = 4096;
	WCHAR wideBuffer[ kNumWideChars ];
	AnsiToWideCch( wideBuffer, finalInputString.c_str(), kNumWideChars );

	txtHelper.DrawTextLine( inputTextRect, DT_LEFT | DT_TOP | DT_CALCRECT, wideBuffer );

	//Draw with shadow first.
	shadowRect = inputTextRect;
	++shadowRect.left;
	++shadowRect.top;
	txtHelper.SetForegroundColor( black );
	txtHelper.DrawTextLine( shadowRect, DT_LEFT | DT_TOP, wideBuffer );

	//Now bright text.
	txtHelper.SetForegroundColor( white );
	txtHelper.DrawTextLine( inputTextRect, DT_LEFT | DT_TOP, wideBuffer );

	//Now display the output text just below the input text.
	outputTextRect.left = inputTextRect.left + 15;
	outputTextRect.top = inputTextRect.bottom + 15;
	outputTextRect.right = pd3dsdBackBuffer->Width - 10;
	outputTextRect.bottom = pd3dsdBackBuffer->Height - 10;
	AnsiToWideCch( wideBuffer, m_CurrentOutputString.c_str(), kNumWideChars );

	//Draw with shadow first.
	shadowRect = outputTextRect;
	++shadowRect.left;
	++shadowRect.top;
	txtHelper.SetForegroundColor( black );
	txtHelper.DrawTextLine( shadowRect, DT_LEFT | DT_TOP, wideBuffer );

	//Now bright text.
	txtHelper.SetForegroundColor( white );
	txtHelper.DrawTextLine( outputTextRect, DT_LEFT | DT_TOP, wideBuffer );

	txtHelper.End();
}

void HumanView::Console::HandleKeyboardInput( const unsigned int keyVal, const unsigned int oemKeyVal, const bool bKeyDown )
{
	if ( true == m_bExecuteStringOnUpdate )
	{
		//We've already got a string to execute; ignore.
		return;
	}

	//See if it's a valid key press that we care about.
	switch( oemKeyVal )
	{
	case VK_BACK:
		{
			const size_t strSize = m_CurrentInputString.size();
			if ( strSize > 0 )
			{
				m_CurrentInputString.erase( ( strSize - 1 ), 1 );
			}
		}
		break;
	case VK_RETURN:
		{
			m_bExecuteStringOnUpdate = true;	//Execute this string.
		}
		break;
	default:
		m_CurrentInputString += (char)keyVal;
		break;
	}
}
