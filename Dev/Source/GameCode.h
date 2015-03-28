#pragma once
//========================================================================
// GameCode.h : Defines the entry point for the application.
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
// 
//  Intro to Game Architecture	- Chapter 2
//  class GameCodeApp 			- Chapter 5, page 127-129
//  class BaseGameLogic			- Chapter 6, page 164-166
//  class BaseActor				- Chapter 19, page 682
//  class ScreenElementScene	- Chapter 19, page 723
//  
//  
//========================================================================


//#include "resource.h"
#include "DumbStuff/CMath.h"
#include "MainLoop/CProcess.h"
#include "Actors.h"
#include "SceneGraph/SceneNodes.h"

#include "EventManager/EventManager.h"


#define QUIT_NO_PROMPT MAKELPARAM(-1,-1)
#define MSG_END_MODAL (WM_USER+100)

enum
{
	// Lower numbers get drawn first
	ZORDER_BACKGROUND, 
	ZORDER_LAYER1, 
	ZORDER_LAYER2, 
	ZORDER_LAYER3, 
	ZORDER_TOOLTIP
};


enum Questions {
	QUESTION_WHERES_THE_CD,
	QUESTION_QUIT_GAME,
};

class BaseGameLogic;
class FontHandler;
class CBaseUI;
class EventManager;
class LuaStateManager;
class BaseSocketManager;
class PathingGraph;
class StateMachine;

class GameCodeApp
{
protected:
	HINSTANCE m_hInstance;					// the module instance
	bool m_bWindowedMode;					// true if the app is windowed, false if fullscreen
	bool m_bIsRunning;						// true if everything is initialized and the game is in the main loop
	bool m_bQuitRequested;					// true if the app should run the exit sequence
	bool m_bQuitting;						// true if the app is running the exit sequence
	CRect m_rcWindow, m_rcDesktop;			// current client window size, and desktop size
	int m_iColorDepth;						// current color depth (16 or 32)

	HMODULE m_LangDll;						// the string table resource - loaded from LANG.DLL
	FontHandler *m_pFontHandler;			// the font handler
	int m_HasModalDialog;					// determines if a modal dialog is up

	int Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);
	int PumpUntilMessage (UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam);
	int	EatSpecificMessages( UINT msgType, optional<LPARAM> lParam, optional<WPARAM> wParam);
	void FlashWhileMinimized();

public:

	GameCodeApp();

	// Game Application Data
	// You must define these in an inherited
	// class - see TeapotWarsApp for an example
	virtual TCHAR *VGetGameTitle()=0;
	virtual TCHAR *VGetGameAppDirectory()=0;
	virtual HICON VGetIcon()=0;

	// Win32 Specific Stuff
	HWND GetHwnd();
	HINSTANCE GetInstance() { return m_hInstance; }
	virtual bool InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL, int screenWidth = SCREEN_WIDTH, int screenHeight = SCREEN_HEIGHT);	
	void ParseCommandLine(LPWSTR lpCmdLine);

	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void *pUserContext );
	static int Ask(const int question);
	bool HasModalDialog() { return m_HasModalDialog!=0; }
	void ForceModalExit() { PostMessage(GetHwnd(), MSG_END_MODAL, 0, QUIT_NO_PROMPT);	}

	LRESULT OnDeviceChange(int eventType);
	LRESULT OnDisplayChange(int colorDepth, int width, int height);
	LRESULT OnPowerBroadcast(int event);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnClose();

	// Game Application actions
	LRESULT OnAltEnter();
	LRESULT OnNcCreate(LPCREATESTRUCT cs);

	std::wstring GetString(const int nID);				
	FontHandler const *GetFontHandler() { return m_pFontHandler; }

	// DirectX Specific Stuff
	static HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void *pUserContext  );
	static void    CALLBACK OnLostDevice(void *pUserContext);
	static bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
	static bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
	static void CALLBACK OnUpdateGame( double fTime, float fElapsedTime, void *pUserContext );
	static void CALLBACK OnRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void *pUserContext );
	static HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
	static void CALLBACK OnDestroyDevice( void* pUserContext );

	static CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs

	// GameCode Specific Stuff
	BaseGameLogic *m_pGame;
	struct GameOptions *m_pOptions;

	// You must define these functions to initialize your game.
	// Look for them in TeapotWars.h and TeapotWars.cpp
	virtual BaseGameLogic *VCreateGameAndView()=0;
	virtual bool VLoadGame()=0;

	// File and Resource System
	class ResCache *m_ResCache;
	TCHAR m_saveGameDirectory[MAX_PATH];
	bool IsMinimumInstall() { return 0; }
	TCHAR *CDCheckFile() { return _T("GameCode3.mrk"); }		
	bool FileExists(TCHAR *searchFile) { return true; }

	// Script manager
	LuaStateManager *m_pLuaStateManager;

	// Event manager
	EventManager *m_pEventManager;
	// Registers all base game events with the event manager.
	void RegisterBaseGameEvents( void );

	// Socket manager - could be server or client
	BaseSocketManager *m_pBaseSocketManager;

	// Main loop processing
	//bool MainLoop();
	void AbortGame() { m_bQuitting = true; }
	int GetExitCode() { return DXUTGetExitCode(); }
	bool IsRunning() { return m_bIsRunning; }
	void SetQuitting(bool quitting) { m_bQuitting = quitting; }
};



//
// enum BaseGameState				- Chapter 19, page 710
//
enum BaseGameState
{
	BGS_Initializing,
	BGS_LoadingGameEnvironment,
	BGS_MainMenu,
	BGS_WaitingForPlayers,
	BGS_SpawnAI,
	BGS_Running
};

typedef std::map<ActorId, shared_ptr<IActor> > ActorMap;

class BaseGameLogic : public IGameLogic
{
	friend class GameCodeApp;						// This is only to gain access to the view list

protected:
	CProcessManager *m_pProcessManager;				// a game logic entity
	CRandom m_random;								// our RNG
	ActorMap m_ActorList;
	ActorId m_LastActorId;
	BaseGameState m_State;							// game state: loading, running, etc.
	int m_ExpectedPlayers;							// how many local human players
	int m_ExpectedRemotePlayers;					// expected remote human players
	int m_ExpectedAI;								// how man AI players
	GameViewList m_gameViews;						// views that are attached to our game
	shared_ptr<PathingGraph> m_pPathingGraph;		// the pathing graph
	EventListenerPtr m_pAiEventListener;			// AI event listener
    

	bool m_bProxy;									// set if this is a proxy game logic, not a real one
	int m_remotePlayerId;							// if we are a remote player - what is out socket number on the server

	// Refactor Chapter 19/20 work.
	bool m_RenderDiagnostics;						// Are we rendering diagnostics?
	shared_ptr<IGamePhysics> m_pPhysics;

public:

	BaseGameLogic(struct GameOptions const &optionss);
	virtual ~BaseGameLogic();

	// Pausing
	void TogglePause(bool active);
	void SetProxy(bool isProxy) { m_bProxy = isProxy; }

	ActorId GetNewActorID( void )
	{
		return ++m_LastActorId;
	}
	
	ActorId GetRandomActor(optional<ActorId> ignoreMe);
	
	shared_ptr<PathingGraph> GetPathingGraph(void) { return m_pPathingGraph; }
	CRandom& GetRNG(void) { return m_random; }

	virtual void VBuildInitialScene();

	virtual void VAddView(shared_ptr<IGameView> pView, optional<ActorId> actorId=optional_empty());
	virtual void VRemoveView(shared_ptr<IGameView> pView);

	virtual void VAddActor(shared_ptr<IActor> actor, ActorParams *p);
	virtual shared_ptr<IActor> VGetActor(const ActorId id);				// TODO: Make Protected...
	virtual void VRemoveActor(ActorId id);
	virtual void VMoveActor(const ActorId id, Mat4x4 const &mat);

	virtual bool VLoadGame(std::string gameName) { return true; }
	virtual void VSetProxy() { m_bProxy = true; }

	// Logic Update
	virtual void VOnUpdate(float time, float elapsedTime);

	// Changing Game Logic State
	virtual void VChangeState(BaseGameState newState);

	// Render Diagnostics
	virtual void VRenderDiagnostics();
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) { return m_pPhysics; }
	
	void AttachProcess(shared_ptr<CProcess> pProcess) { if (m_pProcessManager) {m_pProcessManager->Attach(pProcess);} }
};


// This class is a group of user interface controls.
// It can be modal or modeless.

//
// class CBaseUI - This was factored to create a common class that
//                 implements some of the IScreenElement class common
//                 to modal/modeless dialogs
//
class CBaseUI : public IScreenElement
{
protected:
	int					m_PosX, m_PosY;
	int					m_Width, m_Height;
	optional<int>		m_Result;
	bool				m_bIsVisible;
public:
	CBaseUI() { m_bIsVisible = true; m_PosX = m_PosY = 0; m_Width = 100; m_Height = 100; }
	virtual void VOnUpdate(int) { };
	virtual bool VIsVisible() const { return m_bIsVisible; }
	virtual void VSetVisible(bool visible) { m_bIsVisible = visible; }
};



//
// class CMessageBox - Chapter 9, page 249
//
class CMessageBox : public CBaseUI
{
protected:
	CDXUTDialog m_UI;             // DirectX 9 dialog
	int m_ButtonId;

public:
	CMessageBox(std::wstring msg, std::wstring title, int buttonFlags=MB_OK);
	~CMessageBox();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 99; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
};



class ScreenElementScene : public IScreenElement, public Scene
{
public:
	ScreenElementScene() : Scene() { }

	// IScreenElement Implementation
	virtual void VOnUpdate(int deltaMS) { OnUpdate(deltaMS); };
	virtual HRESULT VOnRestore() 
		{ OnRestore(); return S_OK; }
	virtual HRESULT VOnRender(double fTime, float fElapsedTime)
		{ OnRender(); return S_OK; }
	virtual int VGetZOrder() const { return 0; }
	virtual void VSetZOrder(int const zOrder) { }

	// Don't handle any messages
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg ) { return 0; }

	virtual bool VIsVisible() const { return true; }
	virtual void VSetVisible(bool visible) { }
	virtual bool VAddChild(optional<ActorId> id, shared_ptr<ISceneNode> kid) { return Scene::AddChild(id, kid); }
};




//
// class HumanView - Chapter 9, page 235
//
// This is a view onto the game - which includes anything visual or audible.
// It gets messages about the changes in game state.
//

class HumanView : public IGameView
{
protected:
	GameViewId		m_ViewId;
	optional<ActorId> m_ActorId;

	CProcessManager *m_pProcessManager;				// strictly for things like button animations, etc.

	DWORD m_currTick;		// time right now
	DWORD m_lastDraw;		// last time the game rendered
	bool m_runFullSpeed;	// set to true if you want to run full speed

	ID3DXFont*			        m_pFont;
	ID3DXSprite*				m_pTextSprite;

	virtual void VRenderText(CDXUTTextHelper &txtHelper) { };


public:
	// Implement the IGameView interface
	virtual HRESULT VOnRestore();
	virtual void VOnRender(double fTime, float fElapsedTime );
	virtual void VOnLostDevice();
	virtual GameViewType VGetType() { return GameView_Human; }
	virtual GameViewId VGetId() const { return m_ViewId; }

	virtual void VOnAttach(GameViewId vid, optional<ActorId> aid)
	{
		m_ViewId = vid; 
		m_ActorId = aid;
	}
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	virtual void VOnUpdate( int deltaMilliseconds );

	// Virtual methods to control the layering of interface elements
	virtual void VPushElement(shared_ptr<IScreenElement> pScreen);
	virtual void VPopElement(shared_ptr<IScreenElement> pElement);


	virtual ~HumanView();
	HumanView();

	ScreenElementList m_ScreenElements;						// a game screen entity

	// Interface sensitive objects
	shared_ptr<IMouseHandler> m_MouseHandler;
	shared_ptr<IKeyboardHandler> m_KeyboardHandler;

	// Audio
	bool InitAudio();

	//Camera adjustments.
	virtual void VSetCameraOffset(const Vec4 & camOffset ) { }

public:
	// This class offers a method for entering script commands at runtime.
	// Activate it by pressing the Tilde (~) key during runtime, and type
	// "exit" or "~" and pressing enter.
	class Console
	{
	public:
		Console(void);

		~Console(void);

		void AddDisplayText( const std::string & newText );
		void SetDisplayText( const std::string & newText );

		void SetActive( const bool bIsActive )  { m_bActive = bIsActive; }
		bool IsActive(void) const  { return m_bActive; }

		void HandleKeyboardInput( const unsigned int keyVal, const unsigned int oemKeyVal, const bool bKeyDown );

		void Update( const int deltaMilliseconds );

		void Render( CDXUTTextHelper &txtHelper );

	private:
		bool m_bActive;

		std::queue<std::string> m_DisplayStrings;

		RECT m_ConsoleOutputRect;	//Where results get shown
		RECT m_ConsoleInputRect;	//Where input is entered

		Color m_InputColor;
		Color m_OutputColor;

		std::string m_CurrentOutputString;	//What's the current output string?
		std::string m_CurrentInputString;	//What's the current input string?

		int m_ConsoleInputSize;	//Height of the input console window

		int m_CursorBlinkTimer;	//Countdown to toggle cursor blink state
		bool m_bCursorOn;	//Is the cursor currently displayed?

		bool m_bShiftDown;	//Is the shift button down?
		bool m_bCapsLockDown;	//Is the caps lock button down?

		//If this is true, we have a string to execute on our next update cycle.  
		//We have to do this because otherwise the user could interrupt in the midst
		//of an ::Update() call in another system.  This causes problems.
		bool m_bExecuteStringOnUpdate;
	};

	Console & GetConsole( void )
	{
		return m_Console;
	}

protected:
	Console m_Console;
};


extern GameCodeApp *g_pApp;
