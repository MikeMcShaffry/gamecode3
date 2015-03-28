//========================================================================
// Editor.cpp : source file for the sample game
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

#include <atlconv.h>
#include "resource.h"
#include "Editor.h"

#include "Dbt.h"

#include "DumbStuff\GameCodeError.h"
#include "DumbStuff\CMath.h"
#include "DumbStuff\String.h"
#include "ResourceCache\ResCache2.h"
#include "Debugging\MiniDump.h"
#include "Graphics2D\Font.h"
#include "Lang\Strings.h"
#include "Audio\DirectSoundAudio.h"
#include "Network\Network.h"
#include "Scripting\LuaStateManager.h"

Editor globalApp;

//========================================================================
//
// Editor Implementation
//
//========================================================================

//===================================================================
// Win32 Specific Stuff
//
// InitInstance - this checks system resources, creates your window, and launches the game
//
// preprocessor macro setting in the C++ options of the project provides the base macro
// C preprocessor string concatenation takes care of the rest.
//
// This is identical to the InitInstance in GameCode.cpp, but with
// key differences - instead of a creating a window, we take the window
// pointer from the C# app and set the window instead. In the call to
// Create Device, we pass in parameters of the C# app, instead of the default
// screen size parameters.
//
//===================================================================


//
// Editor::RegisterGameSpecificEvents				- Chapter 20, page 744
//
void Editor::RegisterGameSpecificEvents( void )
{
	// We only care about two events - when the game is starting, and when 
	// a new actor is being requested. All other relevant events are registered
	// by GameCode.
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_New_Game::sk_EventType );
	m_pEventManager->RegisterEvent< EvtData_Request_New_Actor >( EvtData_Request_New_Actor::sk_EventType );
}


//
// Editor::VCreateGameAndView						- Chapter 20, page 743
//
BaseGameLogic* Editor::VCreateGameAndView()
{
	BaseGameLogic *game = NULL;
	assert(m_pOptions && _T("The game options object is uninitialized."));

	// Register any game-specific events here.
	RegisterGameSpecificEvents();

	game = GCC_NEW EditorGame(*m_pOptions);

	shared_ptr<IGameView> gameView(GCC_NEW EditorGameView());
	game->VAddView(gameView);

	return game;
}

bool Editor::VLoadGame()
{
	return m_pGame->VLoadGame("NewGame");
}

HICON Editor::VGetIcon()
{
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON1));
}

//========================================================================
//
// EditorEventListener
//
//========================================================================

class EditorEventListener : public IEventListener
{
public:
	
	explicit EditorEventListener( EditorGame *editorGame ) : m_editorGame(editorGame) { };
	virtual ~EditorEventListener() { };
	char const * GetName(void) { return "EditorEventListener"; }
	bool HandleEvent( IEventData const & event );

private:
	EditorGame *m_editorGame;
};

bool EditorEventListener::HandleEvent( IEventData const & event )
{
	// Note that the only events listed below are for actor requests.
	// Since we don't have a main menu, we can skip directly to setting
	// up the initial scene, which is taken care of by the EditorGame
	// itself.

	if ( EvtData_Move_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Move_Actor & castEvent = static_cast< const EvtData_Move_Actor & >( event );
		m_editorGame->VMoveActor(castEvent.m_Id, castEvent.m_Mat);
	}
	else if ( EvtData_Request_New_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Request_New_Actor & castEvent = static_cast< const EvtData_Request_New_Actor & >( event );

		ActorParams * pActorParams = NULL;
		const bool bCreateEventData = castEvent.VHasLuaEventData();
		if ( false == bCreateEventData )
		{
			//Actor params were created FOR us, so we won't need to allocate it.
			pActorParams = castEvent.m_pActorParams;
		}
		else
		{
			//Create actor params from the Lua object.
			LuaObject actorDef = castEvent.VGetLuaEventData();

			pActorParams = ActorParams::CreateFromLuaObj( actorDef );
		}

		//Did we get valid actor params?
		if ( NULL == pActorParams )
		{
			assert( 0 && "Invalid parameters specified for actor!" );
			return false;
		}

		//Valid params.
		const ActorId actorID = m_editorGame->GetNewActorID();
		pActorParams->m_Id = actorID;
		//Package as a new actor event.
		/*
		IEventDataPtr actorEvent( IEventDataPtr( GCC_NEW EvtData_New_Actor( actorID, pActorParams ) ) );
		const bool bSuccess = safeQueEvent( actorEvent );
		*/
		const EvtData_New_Actor actorEvent( actorID, pActorParams );
		const bool bSuccess = safeTriggerEvent( actorEvent );
		if ( bCreateEventData )
		{
			SAFE_DELETE( pActorParams );
		}
		return bSuccess;
	}
	else if ( EvtData_New_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_New_Actor & castEvent = static_cast< const EvtData_New_Actor & >( event );
		ActorParams * pActorParams = castEvent.m_pActorParams;

		if ( NULL == pActorParams )
		{
			assert( 0 && "Received a new actor event with NULL actor parameters!" );
			return false;
		}

		pActorParams->VCreate(m_editorGame);

		if ( false == castEvent.m_id )
		{
			assert( 0 && "Unable to construct desired actor type!" );
			return false;
		}
		else
		{
			return true;
		}
	}
	else if ( EvtData_Destroy_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Destroy_Actor & castEvent = static_cast< const EvtData_Destroy_Actor & >( event );
		ActorId aid = castEvent.m_id;
		m_editorGame->VRemoveActor(aid);
	}

	return false;
}

//========================================================================
//
// EditorGame Implementation
//
//========================================================================

//
// EditorGame::EditorGame					- Chapter 20, page 745
//
EditorGame::EditorGame(GameOptions const &options)
: BaseGameLogic(options)
, m_Lifetime(0)
{
	// We're not using physics for the editor, but as an optional feature,
	// you can hook up a control that will allow you to toggle the physics
	// on and off. I leave this as a future assignment!
	m_pPhysics.reset(CreateNullPhysics());

	m_editorEventListener = shared_ptr<EditorEventListener> (GCC_NEW EditorEventListener ( this ) );
	safeAddListener( m_editorEventListener, EvtData_New_Actor::sk_EventType );
	safeAddListener( m_editorEventListener, EvtData_Destroy_Actor::sk_EventType );
	safeAddListener( m_editorEventListener, EvtData_Move_Actor::sk_EventType );
	safeAddListener( m_editorEventListener, EvtData_Request_New_Actor::sk_EventType );
}


//
// EditorGame::~EditorGame					- Chapter 20, not in the book
//
EditorGame::~EditorGame()
{
	while (!m_ActorList.empty())
	{
		ActorMap::iterator i = m_ActorList.begin();
		ActorId aid = (*i).first;
		VRemoveActor(aid);
	}
}

//
// EditorGame::EditorGame					- Chapter 20, page 747
//
bool EditorGame::VLoadGame(string gameName)
{
	if (gameName=="NewGame")
	{
		VBuildInitialScene();
		// After we build the initial scene, change the game
		// to New Game. The Editor Game View will do what it 
		// needs to do, and then GameCode will transition
		// the game state into Running.
		safeTriggerEvent( EvtData_New_Game() );
	}

	return true;
}

//
// EditorGame::VOnUpdate					- Chapter 20, page 745
//
void EditorGame::VOnUpdate(float time, float elapsedTime)
{
	int deltaMilliseconds = int(elapsedTime * 1000.0f);
	m_Lifetime += elapsedTime;

	BaseGameLogic::VOnUpdate(time, elapsedTime);

	switch(m_State)
	{
	case BGS_MainMenu:
		// If we are at the main menu, go ahead and
		// start loading the game environment since the
		// editor doesn't have a main menu.
		VChangeState(BGS_LoadingGameEnvironment );
		break;
	case BGS_LoadingGameEnvironment:
		break;
	case BGS_WaitingForPlayers:
		break;
	case BGS_Running:
		break;
	case BGS_SpawnAI:
		break;
	default:
		assert(0 && _T("Unrecognized state."));
	}

	// See prior note about physics. If we wanted to toggle
	// physics on and off, we'd toggle the following bit of code.

	// Note from Mr.Mike - I created a special class called NullPhysics, which can
	// stand in the place of a real phsyics class, it just doesn't do anything. :)
	if(m_pPhysics)
	{
		m_pPhysics->VOnUpdate(elapsedTime);
		m_pPhysics->VSyncVisibleScene();
	}
}

//
// EditorGame::OpenLevelFile						- Chapter 20, page 747
//
void EditorGame::OpenLevelFile( char* fileName )
{
	while (m_ActorList.size() > 0)
	{
		ActorId id = m_ActorList.begin()->first;
		const EvtData_Destroy_Actor destroyActor( id );
		safeTriggerEvent( destroyActor );
	}
	const bool bStartupScriptSuccess = g_pApp->m_pLuaStateManager->DoFile( fileName );
	if ( false == bStartupScriptSuccess )
	{
		assert( 0 && "Unable to execute level file!" );
	}
}

//
// EditorGame::GetView								- Chapter 20, page 747
//
shared_ptr<IGameView> EditorGame::GetView() 
{
	shared_ptr<IGameView> pGameView = *m_gameViews.begin();
	return pGameView; 
}
