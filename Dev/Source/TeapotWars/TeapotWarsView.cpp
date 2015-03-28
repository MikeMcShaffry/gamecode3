//========================================================================
// TeapotWars.cpp : source file for the sample game
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


#include "TeapotWarsStd.h"

#include "GameCode.h"
#include "Actors.h"
#include "Audio\CAudio.h"
#include "Audio\CSoundProcess.h"
#include "SceneGraph\SceneNodes.h"
#include "SceneGraph\Geometry.h"
#include "Graphics2D\Sprite.h"
#include "Lang\Strings.h"
#include "MainLoop\Initialization.h"
#include "TeapotStates.h"

#include "TeapotWarsView.h"
#include "TeapotWarsNetwork.h"
#include "TeapotEvents.h"

#include "EventManager\Events.h"
#include "EventManager\EventManagerImpl.h"
#include "ai\AiEvents.h"

#include "Physics\PhysicsEventListener.h"
#include "TeapotController.h"
#include "TeapotWars.h"

#include <boost\format.hpp>

extern TeapotWarsGameApp g_TeapotWarsApp;

//========================================================================
//
// MainMenuUI & MainMenuView implementation
//
//
//========================================================================

#define CID_CREATE_GAME_RADIO	(2)
#define CID_JOIN_GAME_RADIO		(3)
#define CID_NUM_AI_SLIDER		(4)
#define CID_NUM_PLAYER_SLIDER	(5)
#define CID_HOST_LISTEN_PORT	(6)
#define CID_CLIENT_ATTACH_PORT	(7)
#define CID_START_BUTTON		(8)
#define CID_HOST_NAME			(9)
#define CID_NUM_AI_LABEL		(10)
#define CID_NUM_PLAYER_LABEL	(11)
#define CID_HOST_LISTEN_PORT_LABEL (12)
#define CID_CLIENT_ATTACH_PORT_LABEL (13)
#define CID_HOST_NAME_LABEL		(14)


const int g_SampleUIWidth = 500;
const int g_SampleUIHeight = 450;

MainMenuUI::MainMenuUI()
{
	// This line was added to comply with DirectX 9c (June 2005) 
	m_SampleUI.Init( &g_TeapotWarsApp.g_DialogResourceManager );
    m_SampleUI.SetCallback( OnGUIEvent, this ); 

	int iY = 10; 
	int iX = 35;
	int iX2 = g_SampleUIWidth / 2;
	int width = (g_SampleUIWidth/2)-10;
	int height = 20;
	int lineHeight = height + 2;

	// grab defaults from the game options.
	m_NumAIs = g_pApp->m_pOptions->m_numAIs;
	m_NumPlayers = g_pApp->m_pOptions->m_expectedPlayers;
	m_HostName = g_pApp->m_pOptions->m_gameHost;
	m_HostListenPort = g_pApp->m_pOptions->m_listenPort;
	m_ClientAttachPort = g_pApp->m_pOptions->m_listenPort;

	

	m_bCreatingGame = true;

	D3DCOLOR color = 0x50505050;
	m_SampleUI.SetBackgroundColors(color);

	m_SampleUI.AddStatic(0, L"Teapot Wars Main Menu", iX, iY, g_SampleUIWidth, height * 2);
	iY += (lineHeight * 3);

    m_SampleUI.AddRadioButton( CID_CREATE_GAME_RADIO, 1, L"Create Game", iX, iY, g_SampleUIWidth, height);
	iY += lineHeight;
	m_SampleUI.AddStatic(CID_NUM_AI_LABEL, L"Number of AIs", iX, iY, width, height);
	m_SampleUI.AddSlider( CID_NUM_AI_SLIDER, iX2, iY, width, height);
	m_SampleUI.GetSlider( CID_NUM_AI_SLIDER )->SetRange(0, g_pApp->m_pOptions->m_maxAIs);
	m_SampleUI.GetSlider( CID_NUM_AI_SLIDER )->SetValue(m_NumAIs); // should be ai options default
	iY += lineHeight;

	m_SampleUI.AddStatic(CID_NUM_PLAYER_LABEL, L"Number of Players", iX, iY, width, height);
	m_SampleUI.AddSlider( CID_NUM_PLAYER_SLIDER, iX2, iY, width, height);
	m_SampleUI.GetSlider( CID_NUM_PLAYER_SLIDER )->SetRange(1, g_pApp->m_pOptions->m_maxPlayers);
	m_SampleUI.GetSlider( CID_NUM_PLAYER_SLIDER )->SetValue(m_NumPlayers);  // should be player options default
	iY += lineHeight;

	m_SampleUI.AddStatic(CID_HOST_LISTEN_PORT_LABEL, L"Host Listen Port", iX, iY, width, height);
	m_SampleUI.AddEditBox( CID_HOST_LISTEN_PORT, L"57", iX2, iY, width, height * 2);
	CDXUTEditBox *eb = m_SampleUI.GetEditBox( CID_HOST_LISTEN_PORT );
	eb->SetVisible(false);
	iY += lineHeight * 3;

    m_SampleUI.AddRadioButton( CID_JOIN_GAME_RADIO, 1, L"Join Game", iX, iY, width, height);
    m_SampleUI.GetRadioButton( CID_JOIN_GAME_RADIO )->SetChecked( true ); 
	iY += lineHeight;

	m_SampleUI.AddStatic(CID_CLIENT_ATTACH_PORT_LABEL, L"Host Attach Port", iX, iY, width, height);
	m_SampleUI.AddEditBox( CID_CLIENT_ATTACH_PORT, L"57", iX2, iY, width, height * 2);
	iY += lineHeight * 3;


	m_SampleUI.AddStatic(CID_HOST_NAME_LABEL, L"Host Name", iX, iY, width, height);
	m_SampleUI.AddEditBox( CID_HOST_NAME, L"sunshine", iX2, iY, width, height * 2);
	iY += lineHeight * 3;

	m_SampleUI.AddButton(CID_START_BUTTON, L"Start Game",  (g_SampleUIWidth-(width/2))/2, iY += lineHeight, width/2, height);
	m_SampleUI.SetFont(0, L"Arial", height, 0);

    m_SampleUI.GetRadioButton( CID_CREATE_GAME_RADIO )->SetChecked(true);

	Set();
}

void MainMenuUI::Set()
{
	WCHAR buffer[256];
	CHAR ansiBuffer[256];

	m_NumAIs = m_SampleUI.GetSlider( CID_NUM_AI_SLIDER )->GetValue(); 
	m_SampleUI.GetSlider(CID_NUM_AI_SLIDER)->SetVisible(m_bCreatingGame);

	wsprintf( buffer, _T("%s: %d\n"), L"Number of AIs: ", m_NumAIs );
	m_SampleUI.GetStatic(CID_NUM_AI_LABEL)->SetText(buffer);
	m_SampleUI.GetStatic(CID_NUM_AI_LABEL)->SetVisible(m_bCreatingGame);
	
	m_NumPlayers = m_SampleUI.GetSlider( CID_NUM_PLAYER_SLIDER )->GetValue(); 
	m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->SetVisible(m_bCreatingGame);
	wsprintf( buffer, _T("%s: %d\n"), L"Number of Players: ", m_NumPlayers );
	m_SampleUI.GetStatic(CID_NUM_PLAYER_LABEL)->SetText(buffer);
	m_SampleUI.GetStatic(CID_NUM_PLAYER_LABEL)->SetVisible(m_bCreatingGame);

	m_SampleUI.GetStatic( CID_HOST_LISTEN_PORT_LABEL )->SetVisible(m_NumPlayers>1 && m_bCreatingGame);
	m_SampleUI.GetEditBox( CID_HOST_LISTEN_PORT )->SetVisible(m_NumPlayers>1 && m_bCreatingGame);
	if (m_bCreatingGame)
	{
		WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox( CID_HOST_LISTEN_PORT )->GetText(), 256);
		m_HostListenPort = ansiBuffer;
	}

	m_SampleUI.GetStatic( CID_HOST_NAME_LABEL )->SetVisible(!m_bCreatingGame);
	m_SampleUI.GetEditBox( CID_HOST_NAME )->SetVisible(!m_bCreatingGame);

	WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox( CID_HOST_NAME )->GetText(), 256);
	m_HostName = ansiBuffer;

	m_SampleUI.GetStatic( CID_CLIENT_ATTACH_PORT_LABEL )->SetVisible(!m_bCreatingGame);
	m_SampleUI.GetEditBox( CID_CLIENT_ATTACH_PORT )->SetVisible(!m_bCreatingGame);
	if (!m_bCreatingGame)
	{
		WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox( CID_CLIENT_ATTACH_PORT )->GetText(), 256);
		m_ClientAttachPort = ansiBuffer;
	}

}

MainMenuUI::~MainMenuUI() 
{ 

}


HRESULT MainMenuUI::VOnRestore()
{
    m_SampleUI.SetLocation( (DXUTGetD3D9BackBufferSurfaceDesc()->Width-g_SampleUIWidth)/2, (DXUTGetD3D9BackBufferSurfaceDesc()->Height-g_SampleUIHeight) / 2  );
    m_SampleUI.SetSize( g_SampleUIWidth, g_SampleUIHeight );
	return S_OK;
}

HRESULT MainMenuUI::VOnRender(double fTime, float fElapsedTime)
{
	HRESULT hr;
	DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"TeapotWarsHUD" ); // These events are to help PIX identify what the code is doing
	V( m_SampleUI.OnRender( fElapsedTime ) );
	DXUT_EndPerfEvent();
	return S_OK;
};

LRESULT CALLBACK MainMenuUI::VOnMsgProc( AppMsg msg )
{
	return m_SampleUI.MsgProc( msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam );
}



//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
//    Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
void CALLBACK MainMenuUI::_OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext )
{
    switch( nControlID )
    {
		case CID_CREATE_GAME_RADIO:
			m_bCreatingGame = true;
			break;

		case CID_JOIN_GAME_RADIO:
			m_bCreatingGame = false;
			break;

		case CID_NUM_AI_SLIDER:
		case CID_NUM_PLAYER_SLIDER:
		case CID_HOST_LISTEN_PORT:
		case CID_CLIENT_ATTACH_PORT:
		case CID_HOST_NAME:
			break;

		case CID_START_BUTTON:	
			// grab defaults from the game options.
			g_pApp->m_pOptions->m_numAIs = m_NumAIs;
			g_pApp->m_pOptions->m_expectedPlayers = m_NumPlayers;
			if (m_bCreatingGame)
			{
				g_pApp->m_pOptions->m_gameHost = "";
				g_pApp->m_pOptions->m_listenPort = atoi(m_HostListenPort.c_str());
			}
			else
			{
				g_pApp->m_pOptions->m_gameHost = m_HostName;
				g_pApp->m_pOptions->m_listenPort = atoi(m_ClientAttachPort.c_str());
			}

			VSetVisible(false);

			safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Request_Start_Game( ) ) );

			break;


		default:
			assert(0 && "Unknown control.");
    }

	Set();
}

void CALLBACK MainMenuUI::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext )
{
	MainMenuUI *ui = static_cast<MainMenuUI *>(pUserContext);
	ui->_OnGUIEvent(nEvent, nControlID, pControl, pUserContext);
}


MainMenuView::MainMenuView() : HumanView()
{
	m_MainMenuUI.reset(GCC_NEW MainMenuUI); 
	VPushElement(m_MainMenuUI);
}


MainMenuView::~MainMenuView()
{
	assert(1);
	// does nothing...
}


void MainMenuView::VRenderText(CDXUTTextHelper &txtHelper)
{
	HumanView::VRenderText(txtHelper);
}


void MainMenuView::VOnUpdate( int deltaMilliseconds )
{
	HumanView::VOnUpdate( deltaMilliseconds );
}

LRESULT CALLBACK MainMenuView::VOnMsgProc( AppMsg msg )
{
	if (m_MainMenuUI->VIsVisible() )
	{
		if (HumanView::VOnMsgProc(msg))
			return 1;
	}
	return 0;
}


//========================================================================
//
// StandardHUD implementation
//
//
//========================================================================

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
//    Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
void CALLBACK StandardHUD::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext )
{
//    switch( nControlID )
//    {
//        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
//        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
//        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;
//    }
}


StandardHUD::StandardHUD()
{
    // Initialize dialogs
	m_HUD.Init( &g_TeapotWarsApp.g_DialogResourceManager );
    m_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    //m_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    //m_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    //m_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );
}


HRESULT StandardHUD::VOnRestore()
{
    m_HUD.SetLocation( DXUTGetD3D9BackBufferSurfaceDesc()->Width-170, 0 );
    m_HUD.SetSize( 170, 170 );
	return S_OK;
}



HRESULT StandardHUD::VOnRender(double fTime, float fElapsedTime)
{
	HRESULT hr;
	DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"StandardUI" ); // These events are to help PIX identify what the code is doing
	V( m_HUD.OnRender( fElapsedTime ) );
	DXUT_EndPerfEvent();
	return S_OK;
};


LRESULT CALLBACK StandardHUD::VOnMsgProc( AppMsg msg )
{
    return m_HUD.MsgProc( msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam );
}



//========================================================================
//
// TeapotWarsGameView Implementation
//
//========================================================================

//
// ListenForTeapotViewEvents				- Chapter 19, page 725
//
void ListenForTeapotViewEvents(EventListenerPtr listener)
{
	// hook in the physics event listener
	safeAddListener( listener, EvtData_PhysCollision::sk_EventType );
	safeAddListener( listener, EvtData_Destroy_Actor::sk_EventType );
	safeAddListener( listener, EvtData_Fire_Weapon::sk_EventType );
	safeAddListener( listener, EvtData_New_Game::sk_EventType );
	safeAddListener( listener, EvtData_New_Actor::sk_EventType );
	safeAddListener( listener, EvtData_Move_Actor::sk_EventType );
	safeAddListener( listener, EvtData_Game_State::sk_EventType );
	safeAddListener( listener, EvtData_Request_New_Actor::sk_EventType );
	safeAddListener( listener, EvtData_Debug_String::sk_EventType );
	safeAddListener( listener, EvtData_UpdateActorParams::sk_EventType );
//	safeAddListener( listener, EvtData_Decompression_Progress::sk_EventType );

}

//
// ListenForTeapotGameCommands				- Chapter 19
//
void ListenForTeapotGameCommands(EventListenerPtr listener)
{
	safeAddListener( listener, EvtData_Fire_Weapon::sk_EventType );
	safeAddListener( listener, EvtData_Thrust::sk_EventType );
	safeAddListener( listener, EvtData_Steer::sk_EventType );
	safeAddListener( listener, EvtData_AiSteer::sk_EventType );
}

//
// TeapotWarsGameView::TeapotWarsGameView	- Chapter 19, page 724
//
TeapotWarsGameView::TeapotWarsGameView() 
{ 
	m_BaseGameState = BGS_Initializing;
	m_bShowUI = true; 

	m_pScene.reset(GCC_NEW ScreenElementScene());

	Frustum frustum;
	frustum.Init(D3DX_PI/4.0f, 1.0f, 1.0f, 100.0f);
	m_pCamera.reset(GCC_NEW CameraNode(&Mat4x4::g_Identity, frustum));
	assert(m_pScene && m_pCamera && _T("Out of memory"));

	m_pScene->VAddChild(optional_empty(), m_pCamera);
	m_pScene->SetCamera(m_pCamera);


	EventListenerPtr listener ( GCC_NEW TeapotWarsGameViewListener( this ) );
	ListenForTeapotViewEvents(listener);
}


TeapotWarsGameView::~TeapotWarsGameView() 
{
	assert(1);
}

//
// TeapotWarsGameView::VOnMsgProc				- Chapter 19, page 727
//
LRESULT CALLBACK TeapotWarsGameView::VOnMsgProc( AppMsg msg )
{
	if (HumanView::VOnMsgProc(msg))
		return 1;

	if (msg.m_uMsg==WM_KEYDOWN)
	{
		if (msg.m_wParam==VK_F1)
		{
			m_bShowUI = !m_bShowUI;
			m_StandardHUD->VSetVisible(m_bShowUI);
			return 1;
		}
		else if (msg.m_wParam==VK_F2)
		{
			// test the picking API

			CPoint ptCursor;
			GetCursorPos( &ptCursor );
			ScreenToClient( g_pApp->GetHwnd(), &ptCursor );

			RayCast rayCast(ptCursor);
			m_pScene->Pick(&rayCast);
			rayCast.Sort();

			if (rayCast.m_NumIntersections)
			{
				// You can iterate through the intersections on the raycast.
				int a = 0;
			}
		}
		else if (msg.m_wParam==VK_F3)
		{
			//extern void CreateThreads();
			//CreateThreads();

			extern void testThreading(CProcessManager *procMgr);
			//testThreading(m_pProcessManager);

			extern void testStreaming(CProcessManager *procMgr);
			//testStreaming(m_pProcessManager);

			extern void testRealtimeEvents(CProcessManager *procMgr);
			//testRealtimeEvents(m_pProcessManager);

			extern void testRealtimeDecompression(CProcessManager *procMgr);
			testRealtimeDecompression(m_pProcessManager);
		}
		else if (msg.m_wParam==VK_F8)
		{
			TeapotWarsGame *twg = static_cast<TeapotWarsGame *>(g_pApp->m_pGame);
			twg->ToggleRenderDiagnostics();
		}
		else if (msg.m_wParam==VK_F9)
		{
			m_KeyboardHandler = m_pTeapotController;
			m_MouseHandler = m_pTeapotController;
			m_pCamera->SetTarget(m_pTeapot);
			m_pTeapot->SetAlpha(0.8f);
			ReleaseCapture();
			return 1;
		}
		else if (msg.m_wParam==VK_F11)
		{
			m_KeyboardHandler = m_pFreeCameraController;
			m_MouseHandler = m_pFreeCameraController;
			m_pCamera->ClearTarget();
			m_pTeapot->SetAlpha(fOPAQUE);
			SetCapture(g_pApp->GetHwnd());
			return 1;
		}
		else if (msg.m_wParam=='Q')
		{
			if (GameCodeApp::Ask(QUESTION_QUIT_GAME)==IDYES)
			{
				g_pApp->SetQuitting(true);
			}
			return 1;
		}
	}

	return 0;
}





//
// TeapotWarsGameView::VRenderText				- Chapter 19, page 727
//
void TeapotWarsGameView::VRenderText(CDXUTTextHelper &txtHelper)
{
	HumanView::VRenderText(txtHelper);

	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
	txtHelper.Begin();

	//Scoreboard (with shadow)...
	const ActorScoreMap& actorScores = g_TeapotWarsApp.GetGame()->GetData().GetActorScores();
	std::wstring scoreStr;
	TCHAR tempBuffer[256];
	int player = 1;
	for( ActorScoreMap::const_iterator i=actorScores.begin(); i!=actorScores.end(); ++i )
	{
		//It's good practice to use the string table instead of hardcoding strings into the code
		wsprintf( tempBuffer, _T("%s %d: %d\n"), g_pApp->GetString(IDS_PLAYER).c_str(), player, i->second );
		++player;
		scoreStr.append( tempBuffer );
	}
	txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width/2, 5 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	txtHelper.DrawTextLine(scoreStr.c_str());
	txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width/2-1, 5-1 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 0.25f, 1.0f, 0.25f, 1.0f ) );
	txtHelper.DrawTextLine(scoreStr.c_str());
	//...Scoreboard

	if( m_bShowUI )
	{
		// Output statistics...
		txtHelper.SetInsertionPos( 5, 5 );
		txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
		txtHelper.DrawTextLine( DXUTGetFrameStats() );
		txtHelper.DrawTextLine( DXUTGetDeviceStats() );
		//...output statistics
		
		//txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
		txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.5f ) );

		//Game State...
		switch (m_BaseGameState)
		{
			case BGS_Initializing:
				txtHelper.DrawTextLine(g_pApp->GetString(IDS_INITIALIZING).c_str());
				break;

			case BGS_MainMenu:
				txtHelper.DrawTextLine(L"Main Menu");
				break;

			case BGS_SpawnAI:
				txtHelper.DrawTextLine(L"Spawn AI");
				break;

			case BGS_WaitingForPlayers:
				txtHelper.DrawTextLine(g_pApp->GetString(IDS_WAITING).c_str());
				break;

			case BGS_LoadingGameEnvironment:
				txtHelper.DrawTextLine(g_pApp->GetString(IDS_LOADING).c_str());
				break;

			case BGS_Running:
#ifndef DISABLE_PHYSICS
				txtHelper.DrawTextLine(g_pApp->GetString(IDS_RUNNING).c_str());
#else
				txtHelper.DrawTextLine(g_pApp->GetString(IDS_NOPHYSICS).c_str());
#endif //!DISABLE_PHYSICS
				break;
		}
		//...Game State

		//Camera...
		TCHAR buffer[256];
		const TCHAR *s = NULL;
		Mat4x4 toWorld;
		Mat4x4 fromWorld;
		if (m_pCamera)
		{	
			m_pCamera->VGet()->Transform(&toWorld, &fromWorld);
		}
		swprintf(buffer, g_pApp->GetString(IDS_CAMERA_LOCATION).c_str(), toWorld.m[3][0], toWorld.m[3][1], toWorld.m[3][2]);
		txtHelper.DrawTextLine( buffer );
		//...Camera

		//Help text.  Right justified, lower right of screen.
		RECT helpRect;
		helpRect.left = 0;
		helpRect.right = pd3dsdBackBuffer->Width - 10;
		helpRect.top = pd3dsdBackBuffer->Height-15*8;
		helpRect.bottom = pd3dsdBackBuffer->Height;
		txtHelper.SetInsertionPos( helpRect.right, helpRect.top );
		txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
		txtHelper.DrawTextLine( helpRect, DT_RIGHT, g_pApp->GetString(IDS_CONTROLS_HEADER).c_str() );
		helpRect.top = pd3dsdBackBuffer->Height-15*7;
		txtHelper.DrawTextLine( helpRect, DT_RIGHT, g_pApp->GetString(IDS_CONTROLS).c_str() );
		//...Help
	}//end if (m_bShowUI)

	txtHelper.End();
}



//
// TeapotWarsGameView::VOnUpdate				- Chapter 19, page 730
//
void TeapotWarsGameView::VOnUpdate( int deltaMilliseconds )
{
	HumanView::VOnUpdate( deltaMilliseconds );

	if (m_pFreeCameraController)
	{
		m_pFreeCameraController->OnUpdate(deltaMilliseconds);
	}

	if (m_pTeapotController)
	{
		m_pTeapotController->OnUpdate(deltaMilliseconds);
	}

	//Send out a tick to script listeners.
	const EvtData_Update_Tick tickEvent( deltaMilliseconds );
	safeTriggerEvent( tickEvent );
}

//
// TeapotWarsGameView::VOnAttach				- Chapter 19, page 731
//
void TeapotWarsGameView::VOnAttach(GameViewId vid, optional<ActorId> aid)
{
	HumanView::VOnAttach(vid, aid);
	BuildInitialScene();
}

void TeapotWarsGameView::VSetCameraOffset(const Vec4 & camOffset )
{
	m_pCamera->SetCameraOffset( camOffset );
}


//
// TeapotWarsGameView::MoveActor				- Chapter 19, page 731
//
void TeapotWarsGameView::MoveActor(ActorId id, Mat4x4 const &mat)
{
    shared_ptr<ISceneNode> node = m_pScene->FindActor(id);
	if (node)
	{
		node->VSetTransform(&mat);
	}
}

//
// TeapotWarsGameView::HandleGameState			- Chapter 19, page 731
//
void TeapotWarsGameView::HandleGameState(BaseGameState newState)
{
	m_BaseGameState = newState;
}



//
// TeapotWarsGameView::BuildInitialScene		- Chapter 19, page 725
//
void TeapotWarsGameView::BuildInitialScene()
{
	SoundResource resource("SpaceGod7-Level2.ogg");
	shared_ptr<ResHandle> rh = g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<SoundResHandle> srh = boost::static_pointer_cast<SoundResHandle>(rh);
	shared_ptr<SoundProcess> music(GCC_NEW SoundProcess(srh, PROC_MUSIC, 0, true));
	m_pProcessManager->Attach(music);

	shared_ptr<CFadeProcess> fadeProc(new CFadeProcess(music, 10000, 100)); 
	m_pProcessManager->Attach(fadeProc);


	// Here's our sky node
	shared_ptr<SkyNode> sky(GCC_NEW SkyNode("Sky2", m_pCamera));
	m_pScene->VAddChild(optional_empty(), sky);

	m_StandardHUD.reset(GCC_NEW StandardHUD); 

	VPushElement(m_pScene);
	VPushElement(m_StandardHUD);

	// A movement controller is going to control the camera, 
	// but it could be constructed with any of the objects you see in this
	// function. You can have your very own remote controlled sphere. What fun...

	m_pFreeCameraController.reset(GCC_NEW MovementController(m_pCamera, 0, 0, false));

	VOnRestore();

}

//
// TeapotWarsGameViewListener::TeapotWarsGameViewListener	- Chapter 19, page 766
//
TeapotWarsGameViewListener::TeapotWarsGameViewListener( TeapotWarsGameView *view)
{
	m_pView = view;
}



//
// TeapotWarsGameViewListener::HandleEvent					- Chapter 19, page 731
//
bool TeapotWarsGameViewListener::HandleEvent( IEventData const & event )
{
	// [rez] removing event spew
	//if ( EvtData_Move_Actor::sk_EventType.getHashValue() != event.VGetEventType().getHashValue() )
	//{
	//	OutputDebugStringA( event.VGetEventType().getStr().c_str() );
	//	OutputDebugStringA("\n");
	//}

	if ( EvtData_PhysCollision::sk_EventType == event.VGetEventType() )
	{
		EvtData_PhysCollision const & ed = static_cast< const EvtData_PhysCollision & >( event );
		shared_ptr<IActor> pGameActorA = g_pApp->m_pGame->VGetActor(ed.m_ActorA);
		shared_ptr<IActor> pGameActorB = g_pApp->m_pGame->VGetActor(ed.m_ActorB);
		if (!pGameActorA || !pGameActorB)
			return false;
			
		int typeA = pGameActorA->VGetType();
		int typeB = pGameActorB->VGetType();

		if(	(AT_Teapot==typeA && AT_Sphere==typeB)
			|| (AT_Sphere==typeA && AT_Teapot==typeB) )
		{
			// play the sound a bullet makes when it hits a teapot

			SoundResource resource("computerbeep3.wav");
			shared_ptr<SoundResHandle> srh = boost::static_pointer_cast<SoundResHandle>(g_pApp->m_ResCache->GetHandle(&resource));
			shared_ptr<SoundProcess> sfx(GCC_NEW SoundProcess(srh, PROC_SOUNDFX, 100, false));
			m_pView->m_pProcessManager->Attach(sfx);
		}
	}
	else
	if ( EvtData_Destroy_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Destroy_Actor & castEvent = static_cast< const EvtData_Destroy_Actor & >( event );
		ActorId aid = castEvent.m_id;
		m_pView->m_pScene->RemoveChild(aid);
	}
	else if ( EvtData_Fire_Weapon::sk_EventType == event.VGetEventType() )
	{
		// play a weapon fire sound
		SoundResource resource("blip.wav");
		shared_ptr<SoundResHandle> srh = boost::static_pointer_cast<SoundResHandle>(g_pApp->m_ResCache->GetHandle(&resource));
		shared_ptr<SoundProcess> sfx1(GCC_NEW SoundProcess(srh, PROC_SOUNDFX, 100, false));
		shared_ptr<SoundProcess> sfx2(GCC_NEW SoundProcess(srh, PROC_SOUNDFX, 60, false));
		shared_ptr<SoundProcess> sfx3(GCC_NEW SoundProcess(srh, PROC_SOUNDFX, 40, false));
		m_pView->m_pProcessManager->Attach(sfx1);
		sfx1->SetNext(sfx2);
		sfx2->SetNext(sfx3);
	}
	else if ( EvtData_New_Game::sk_EventType == event.VGetEventType() )
	{
		// do nothing
	}
	else if ( EvtData_Move_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Move_Actor & ed = static_cast< const EvtData_Move_Actor & >( event );
		m_pView->MoveActor(ed.m_Id, ed.m_Mat);
	}
	else if ( EvtData_New_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_New_Actor & ed = static_cast< const EvtData_New_Actor & >( event );

		shared_ptr<SceneNode> node = ed.m_pActorParams->VCreateSceneNode(m_pView->m_pScene);
		m_pView->m_pScene->VAddChild(ed.m_pActorParams->m_Id, node);
		node->VOnRestore(&(*(m_pView->m_pScene)));

		if (ed.m_pActorParams->m_Type == AT_Teapot)
		{
			TeapotParams *p = static_cast<TeapotParams *>(ed.m_pActorParams);
			//shared_ptr<SceneNode> teapot = m_pView->CreateTeapot(*p);
			//assert(teapot);
			if (p->m_ViewId == m_pView->m_ViewId)
			{
				m_pView->m_pTeapot = node;
				m_pView->m_pTeapotController.reset(GCC_NEW TeapotController(m_pView->m_pTeapot, 0, 0));
				m_pView->m_KeyboardHandler = m_pView->m_pTeapotController;
				m_pView->m_MouseHandler = m_pView->m_pTeapotController;
				m_pView->m_pCamera->SetTarget(m_pView->m_pTeapot);
				m_pView->m_pTeapot->SetAlpha(0.8f);	
			}
		}
	}
	else if ( EvtData_Game_State::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Game_State & ed = static_cast< const EvtData_Game_State & >( event );
		BaseGameState gameState = ed.m_gameState;
		m_pView->HandleGameState(gameState);
	}
	else if ( EvtData_Debug_String::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Debug_String & castEvent = static_cast< const EvtData_Debug_String & >( event );
		switch( castEvent.m_Type )
		{
			case EvtData_Debug_String::kDST_ScriptMsg:
			{
				OutputDebugStringA( "LUA DEBUG: " );
				OutputDebugStringA( castEvent.m_DebugMessage.c_str() );
				OutputDebugStringA( "\n" );

				// If our parent's console is open, tell it as well.
				HumanView::Console & console = m_pView->GetConsole();
				if (console.IsActive())
					console.AddDisplayText(castEvent.m_DebugMessage);
				break;
			}
			
			default:
			{
				assert( 0 && "Unknown/unsupported debug string encountered!" );
				break;
			}
		}
	}
	else if ( EvtData_UpdateActorParams::sk_EventType == event.VGetEventType() )
	{
		// Commenting this is leaks HUGE memory!!!! Not sure why....
		const EvtData_UpdateActorParams & castEvent = static_cast< const EvtData_UpdateActorParams & >( event );

		//Update the params in the scene node.
		shared_ptr< ISceneNode > sceneNode = m_pView->m_pScene->FindActor( castEvent.m_ActorID );
		SceneNode * pSceneNode = static_cast< SceneNode * >( sceneNode.get() );
		ActorParams * pParams = pSceneNode->VGetActorParams();
		if ( NULL != pParams )
		{
			//Update the params, restore the actor.
			ActorParams::TErrorMessageList errorMessages;
			pParams->VInit( castEvent.VGetLuaEventData(), errorMessages );
			sceneNode->VOnRestore( m_pView->m_pScene.get() );
		}
	}
/**	else if ( EvtData_Decompression_Progress::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Decompression_Progress & castEvent = static_cast< const EvtData_Decompression_Progress & >( event );
		if (castEvent.m_buffer != NULL)
		{
			const void *buffer = castEvent.m_buffer;
			SAFE_DELETE_ARRAY(buffer);
		}
	}
**/
	return false;
}

///////////////////////////////////////////////////////////////////////////////
//
// AITeapotView::AITeapotView					- Chapter 19, page 737
//
AITeapotView::AITeapotView(shared_ptr<PathingGraph> pPathingGraph) : IGameView(), m_pPathingGraph(pPathingGraph)
{
	m_pStateMachine = NULL;
	EventListenerPtr listener ( GCC_NEW AITeapotViewListener( this ) );
	safeAddListener( listener, EvtData_New_Actor::sk_EventType );
}

//
// AITeapotView::~AITeapotView					- Chapter 19, page 737
//
AITeapotView::~AITeapotView(void)
{
	SAFE_DELETE(m_pStateMachine);
	OutputDebugString(_T("Destroying AITeapotView\n"));
}


//
// AITeapotViewListener::HandleEvent			- Chapter 19, page 738
//
bool AITeapotViewListener::HandleEvent( IEventData const & event )
{
	if ( EvtData_New_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_New_Actor & ed = static_cast< const EvtData_New_Actor & >( event );
		switch(ed.m_pActorParams->m_Type)
		{
			case AT_Sphere:
			{
				// somebody took a shot!
				break;
			}
			case AT_Teapot:
			{
				TeapotParams *p = static_cast<TeapotParams *>(ed.m_pActorParams);
				if (p->m_ViewId == m_pView->m_ViewId)
				{
					// we need a valid ID
					assert(p->m_Id.valid());
					m_pView->m_PlayerActorId = p->m_Id;
					
					// create the state machine and set the initial state
					//m_pView->m_pStateMachine.reset(GCC_NEW TeapotStateMachine(*(p->m_Id),m_pView->m_pPathingGraph));
					m_pView->m_pStateMachine = GCC_NEW TeapotStateMachine(*(p->m_Id),m_pView->m_pPathingGraph);
					m_pView->m_pStateMachine->SetState("wander");
				}
				break;
			}
			
			/**************
			case AT_AiTeapot:
			{
				AiTeapotParams *p = static_cast<AiTeapotParams *>(ed.m_pActorParams);
				if (p->m_ViewId == m_pView->m_ViewId)
				{
					// we need a valid ID
					assert(p->m_Id.valid());
					m_pView->m_PlayerActorId = p->m_Id;

					// create the state machine and set the initial state
					//m_pView->m_pStateMachine.reset(GCC_NEW TeapotStateMachine(*(p->m_Id),m_pView->m_pPathingGraph));
					m_pView->m_pStateMachine = GCC_NEW TeapotStateMachine(*(p->m_Id),m_pView->m_pPathingGraph);
					m_pView->m_pStateMachine->SetState("wander");
				}
				break;
			}

			case AT_TestObject:
			{
				// do nothing...
				break;
			}
			*****************/

			default:
				OutputDebugString(_T("AI Listener - Unsupported Actor Type\n"));
		}
	}

	return false;
}
