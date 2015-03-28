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

#include "Physics\Physics.h"
#include "Physics\PhysicsEventListener.h"
#include "MainLoop\Initialization.h"

#include "ai/Pathing.h"

#include "TeapotWars.h"
#include "TeapotWarsView.h"
#include "TeapotWarsNetwork.h"
#include "TeapotEvents.h"

#include "TeapotResources.h"

#include "EventManager\Events.h"
#include "EventManager\EventManagerImpl.h"

TeapotWarsGameApp g_TeapotWarsApp;

//========================================================================
// wWinMain - Defines the entry point to your application (Chapter 5 - page 121)
//
// In the book _tWinMain goes straight into the initialization sequence. Here,
// the GameCode3() free function handles it. This allows the GameCode3 function
// to live in a library, separating the game engine from game specific code,
// in this case TeapotWars.
//
//========================================================================


INT WINAPI wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
	return GameCode3(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

//========================================================================
//
// TeapotWarsGameApp Implementation
//
//========================================================================

// This function registers game-specific events 
// (cf. the GameCodeApp::RegisterBaseGameEvents() function)
void TeapotWarsGameApp::RegisterGameSpecificEvents( void )
{
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Fire_Weapon::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Thrust::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Steer::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_New_Game::sk_EventType );
	m_pEventManager->RegisterCodeOnlyEvent( EvtData_Request_Start_Game::sk_EventType );
	m_pEventManager->RegisterEvent< EvtData_Request_New_Actor >( EvtData_Request_New_Actor::sk_EventType );
	m_pEventManager->RegisterEvent< EvtData_UpdateActorParams >( EvtData_UpdateActorParams::sk_EventType );
}

//
// TeapotWarsGameApp::VCreateGameAndView - Chapter 19, page 702
//
BaseGameLogic *TeapotWarsGameApp::VCreateGameAndView()
{
	BaseGameLogic *game = NULL;
	assert(m_pOptions && _T("The game options object is uninitialized."));

	// Register any game-specific events here.
	RegisterGameSpecificEvents();

	if (m_pOptions->m_gameHost.empty())
	{
		game = GCC_NEW TeapotWarsGame(*m_pOptions);
	}
	else
	{
		game = GCC_NEW TeapotWarsGameProxy(*m_pOptions);

		EventListenerPtr listener ( GCC_NEW NetworkEventForwarder( 0 ) );
		extern void ListenForTeapotGameCommands(EventListenerPtr listener);
		ListenForTeapotGameCommands(listener);

	}

	shared_ptr<IGameView> menuView(GCC_NEW MainMenuView());
	game->VAddView(menuView);

	return game;
}




//
// TeapotWarsGameApp::VLoadGame				- Chapter 19, page 703
//
bool TeapotWarsGameApp::VLoadGame()
{
	// Ordinarilly you'd read the game options and see what the current game
	// needs to be - or perhaps pop up a dialog box and ask which game
	// needed loading. All of the game graphics are initialized by now, too...
	return m_pGame->VLoadGame("NewGame");
}

HICON TeapotWarsGameApp::VGetIcon()
{
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON1));
}


//
// class TeapotWarsEventListener			- Chapter 19, page 752
//
class TeapotWarsEventListener : public IEventListener
{
public:
	
	explicit TeapotWarsEventListener( TeapotWarsGame *teapotWars ) : m_TeapotWars(teapotWars) { };
	virtual ~TeapotWarsEventListener() { };
	char const * GetName(void) { return "TeapotWarsEventListener"; }
	bool HandleEvent( IEventData const & event );

private:
	TeapotWarsGame *m_TeapotWars;
};

//
// TeapotWarsEventListener::HandleEvent		- Chapter 19, page 715
//
const float g_WeaponForce = .3f;
bool TeapotWarsEventListener::HandleEvent( IEventData const & event )
{
	if ( EvtData_Request_Start_Game::sk_EventType == event.VGetEventType() )
	{
		m_TeapotWars->VChangeState(BGS_WaitingForPlayers);
	}
	else if ( EvtData_Game_State::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Game_State & castEvent = static_cast< const EvtData_Game_State & >( event );
		m_TeapotWars->VChangeState(castEvent.m_gameState);
	}
	else if ( EvtData_Remote_Client::sk_EventType == event.VGetEventType() )
	{
		// This event is always sent from clients to the game server.

		const EvtData_Remote_Client & castEvent = static_cast< const EvtData_Remote_Client & >( event );
		const int sockID = castEvent.m_socketId;
		const int ipAddress = castEvent.m_ipAddress;

		// The teapot has already been created - we need to go find it.
		ActorMap::iterator i = m_TeapotWars->m_ActorList.begin();
		ActorMap::iterator end = m_TeapotWars->m_ActorList.end();
		shared_ptr<IActor> actor = shared_ptr<BaseActor>(); 
		while (i != end)
		{
			actor = (*i).second;
			if (actor->VGetType() == AT_Teapot)
			{
				shared_ptr<ActorParams> params = actor->VGetParams();
				shared_ptr<TeapotParams> teapotParams = boost::static_pointer_cast<TeapotParams>(params);
				if (teapotParams->m_ViewId == VIEWID_NO_VIEW_ATTACHED)
				{
					break;
				}
			}
			++i;
		}

		if (actor != shared_ptr<BaseActor>())
		{
			NetworkGameView *netGameView = GCC_NEW NetworkGameView( sockID );

			shared_ptr<IGameView> gameView(netGameView);
			m_TeapotWars->VAddView(gameView, actor->VGetID());

			extern void ListenForTeapotViewEvents(EventListenerPtr listener);

			EventListenerPtr listener ( GCC_NEW NetworkEventForwarder( sockID ) );
			ListenForTeapotViewEvents( listener );
		}
	}

	else if ( EvtData_Network_Player_Actor_Assignment::sk_EventType == event.VGetEventType() )
	{
		// we're a remote client getting an actor assignment.
		// the server assigned us a playerId when we first attached (the server's socketId, actually)
		const EvtData_Network_Player_Actor_Assignment & castEvent =
			static_cast< const EvtData_Network_Player_Actor_Assignment & >( event );
		
		shared_ptr<IGameView> playersView(GCC_NEW TeapotWarsGameView());
		m_TeapotWars->VAddView(playersView, castEvent.m_actorId);	
	}

	else if ( EvtData_PhysCollision::sk_EventType == event.VGetEventType() )
	{
		const EvtData_PhysCollision & castEvent = static_cast< const EvtData_PhysCollision & >( event );
		shared_ptr<IActor> pGameActorA = m_TeapotWars->VGetActor(castEvent.m_ActorA);
		shared_ptr<IActor> pGameActorB = m_TeapotWars->VGetActor(castEvent.m_ActorB);
		if (!pGameActorA || !pGameActorB)
			return false;
			
		int typeA = pGameActorA->VGetType();
		int typeB = pGameActorB->VGetType();

		if (AT_Sphere==typeA && AT_Teapot==typeB)
		{
			m_TeapotWars->VRegisterHit(pGameActorA->VGetID(), pGameActorB->VGetID());
			m_TeapotWars->VRemoveActor(pGameActorA->VGetID());//get rid of the sphere
		}

		if (AT_Teapot==typeA && AT_Sphere==typeB)
		{
			m_TeapotWars->VRegisterHit(pGameActorB->VGetID(), pGameActorA->VGetID());
			m_TeapotWars->VRemoveActor(pGameActorB->VGetID());//get rid of the sphere
		}
	}
	else if ( EvtData_Thrust::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Thrust & castEvent = static_cast< const EvtData_Thrust & >( event );
		shared_ptr<IActor> pActor = m_TeapotWars->VGetActor(castEvent.m_id);
		if( pActor )
		{
			static const float newtonForce = 1.f;
			float thrustForce = castEvent.m_throttle * newtonForce;

			Mat4x4 rotation = pActor->VGetMat();
			rotation.SetPosition(Vec3(0,0,0));
			Vec3 dir = rotation.Xform(g_Forward);
			dir.Normalize();
			m_TeapotWars->m_pPhysics->VApplyForce(dir, thrustForce, castEvent.m_id);
		}
	}
	else if ( EvtData_Steer::sk_EventType == event.VGetEventType() )
	{
		static const float newtonForce = -.25 * 1.8f;

		const EvtData_Steer & castEvent = static_cast< const EvtData_Steer & >( event );
		float steerForce = -castEvent.m_dir * newtonForce;
		m_TeapotWars->m_pPhysics->VApplyTorque(Vec3(0,1,0), steerForce, castEvent.m_id);
	}
	else if ( EvtData_Fire_Weapon::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Fire_Weapon & castEvent = static_cast< const EvtData_Fire_Weapon & >( event );
		ActorId gunnerId = castEvent.m_id;

		shared_ptr<IActor> pGunner = m_TeapotWars->VGetActor(gunnerId);
		if (pGunner)
		{
			// TODO: You should check his stores of ammo right here!!!

			//Calculate depth offset from the controller
			Vec4 at = g_Forward4 * 2.0f;
			Vec4 atWorld = pGunner->VGetMat().Xform(at);

			//Calculate up offset from the controller
			Vec4 up = g_Up4 * 2.f;
			Vec4 upWorld = pGunner->VGetMat().Xform(up);

			Vec4 direction = atWorld + upWorld;
			Vec3 normalDir(direction);
			normalDir.Normalize();

			SphereParams sp;
			sp.m_Pos = pGunner->VGetMat().GetPosition() + Vec3(direction);
			sp.m_Radius = 0.25;
			sp.m_Segments = 16;
			sp.m_Color = g_Cyan;
			sp.m_NormalDir = normalDir;
			sp.m_Force = g_WeaponForce;

			//Request creation of this actor.
			const EvtData_Request_New_Actor cannonBallEvt( &sp );
			safeTriggerEvent( cannonBallEvt );
		}
	}
	else if ( EvtData_Move_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Move_Actor & castEvent = static_cast< const EvtData_Move_Actor & >( event );
		m_TeapotWars->VMoveActor(castEvent.m_Id, castEvent.m_Mat);
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
		const ActorId actorID = m_TeapotWars->GetNewActorID();
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

		pActorParams->VCreate(m_TeapotWars);


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
	else if ( EvtData_UpdateActorParams::sk_EventType == event.VGetEventType() )
	{
		//Update the parameters for the specified actor.
		const EvtData_UpdateActorParams & castEvent = static_cast< const EvtData_UpdateActorParams & >( event );
		shared_ptr< IActor > destActor = g_pApp->m_pGame->VGetActor( castEvent.m_ActorID );

		//Re-jigger the actor params.
		BaseActor * pBaseActor = static_cast< BaseActor * >( destActor.get() );
		shared_ptr<ActorParams> params = pBaseActor->VGetParams();

		ActorParams::TErrorMessageList errorMessages;
		params->VInit( castEvent.VGetLuaEventData(), errorMessages );
	}

	return false;
}

//
// TeapotWarsBaseGame::VLoadGame			- Chapter 19, page 704
//
bool TeapotWarsBaseGame::VLoadGame(std::string gameName)
{
	if (gameName=="NewGame")
	{
		VBuildInitialScene();
		safeTriggerEvent( EvtData_New_Game() );
	}

	return true;
}




//
// TeapotWarsBaseGame::VRemoveActor			- TODO
//
void TeapotWarsBaseGame::VRemoveActor(ActorId aid) 
{ 
	shared_ptr<IActor> actor = VGetActor( aid );
	assert(actor && "No such actor!");
	if (!actor)
	{
		return;
	}

	//Call any script-related destructor.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	LuaObject globalActorTable = g_pApp->m_pLuaStateManager->GetGlobalActorTable();
	assert( globalActorTable.IsTable() && "Global actor table is NOT a table!" );
	LuaObject actorData = globalActorTable[ aid ];

	shared_ptr<ActorParams> actorParams = actor->VGetParams();
	if ( 0 != strlen( actorParams->m_OnDestroyLuaFunctionName ) )
	{
		//First attempt to FIND the function specified.
		LuaObject foundObj = g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobal( actorParams->m_OnDestroyLuaFunctionName );
		if ( foundObj.IsNil() )
		{
			assert( 0 && "Unable to find specified OnDestroyFunc function!" );
		}
		else
		{
			//Make sure it actually *IS* a function.
			if ( false == foundObj.IsFunction() )
			{
				assert( 0 && "Specified OnDestroyFunc doesn't exist!" );
			}
			else
			{
				//Attempt to call the function.
				LuaFunction< void > onDestroyFunc( foundObj );
				onDestroyFunc( aid, actorData );	//Pass in the actor ID and this actor's user-owned data table.
			}
		}
	}

	//Remove the actor from script's grubby little hands.
	actorData.AssignNil( pState );

	BaseGameLogic::VRemoveActor(aid); 
	m_data.RemoveActor(aid);

	//Now remove the actor from the system.
	safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Destroy_Actor( aid ) ) );
}

//
// TeapotWarsBaseGame::VAddActor			- Chapter 19, page 705
//
void TeapotWarsBaseGame::VAddActor(shared_ptr<IActor> actor, ActorParams *p)
{
	BaseGameLogic::VAddActor(actor, p);
	if (p->m_Type==AT_Teapot && p->m_Id.valid())
	{
		m_data.m_actorScores[*p->m_Id] = 0;
	}

	//Ensure script knows about this actor, too.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	LuaObject globalActorTable = g_pApp->m_pLuaStateManager->GetGlobalActorTable();
	assert( globalActorTable.IsTable() && "Global actor table is NOT a table!" );
	LuaObject addedActorData = globalActorTable.CreateTable( *p->m_Id );	//The actor ID is the key.
	addedActorData.SetInteger( "ActorID", *p->m_Id );

	if ( 0 != p->m_OnCreateLuaFunctionName[0] ) 
	{
		addedActorData.SetString( "OnCreateFunc", p->m_OnCreateLuaFunctionName );
	}

	if ( 0 != p->m_OnDestroyLuaFunctionName[0] ) 
	{
		addedActorData.SetString( "OnDestroyFunc", p->m_OnDestroyLuaFunctionName );
	}

	//If this actor has any script-specific functions to call, do so now.
	if ( 0 != strlen( p->m_OnCreateLuaFunctionName ) )
	{
		//First attempt to FIND the function specified.
		LuaObject foundObj = g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobal( p->m_OnCreateLuaFunctionName );
		if ( foundObj.IsNil() )
		{
			assert( 0 && "Unable to find specified OnCreateFunc function!" );
		}
		else
		{
			//Make sure it actually *IS* a function.
			if ( false == foundObj.IsFunction() )
			{
				assert( 0 && "Specified OnCreateFunc doesn't exist!" );
			}
			else
			{
				//Attempt to call the function.
				LuaFunction< void > onCreateFunc( foundObj );
				onCreateFunc( *p->m_Id, addedActorData );	//Pass in the actor ID and this actor's user-owned data table.
			}
		}
	}
}





//========================================================================
//
// TeapotWarsGame Implementation
//
//========================================================================

//
// TeapotWarsGame::TeapotWarsGame			- Chapter 19, page 707
//
TeapotWarsGame::TeapotWarsGame(GameOptions const &options)
: TeapotWarsBaseGame(options)
, m_Lifetime(0)
, m_StartPosition(6.0f, 1.5f, 3.0f)
, m_HumanPlayersAttached(0)
, m_AIPlayersAttached(0)
{
	m_pPhysics.reset(CreateGamePhysics());

	m_teapotWarsEventListener = shared_ptr<TeapotWarsEventListener> (GCC_NEW TeapotWarsEventListener ( this ) );
	safeAddListener( m_teapotWarsEventListener, EvtData_Remote_Client::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_PhysCollision::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_New_Actor::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_Destroy_Actor::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_Move_Actor::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_Request_New_Actor::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_Request_Start_Game::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_Network_Player_Actor_Assignment::sk_EventType );
	safeAddListener( m_teapotWarsEventListener, EvtData_UpdateActorParams::sk_EventType );

	extern void ListenForTeapotGameCommands(EventListenerPtr listener);
	ListenForTeapotGameCommands(m_teapotWarsEventListener);

	//Open up access to script.
	{
		//Create our metatable...
		m_MetaTable = g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobals().CreateTable("TeapotWarsGame");
		m_MetaTable.SetObject("__index", m_MetaTable);
		
//		m_MetaTable.RegisterObjectDirect( "SetCameraOffset", (TeapotWarsGame *)0, &TeapotWarsGame::SetCameraOffset);
		m_MetaTable.RegisterObjectDirect< TeapotWarsGame > ( "SetCameraOffset", NULL, &TeapotWarsGame::SetCameraOffset); 

		LuaObject luaStateManObj = g_pApp->m_pLuaStateManager->GetGlobalState()->BoxPointer(this);
		luaStateManObj.SetMetaTable(m_MetaTable);
		g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobals().SetObject("TeapotWarsGame", luaStateManObj);
	}
}





//
// TeapotWarsGame::~TeapotWarsGame			- added post press
//
// This destructor must be called to remove all the actors from the actor list.
// If we rely on the BaseGame class to do that, the virtual chain is broken and the
// VRemoveActor method that gets called is BaseGame::VRemoveActor, which doesn't 
// remove the actor from the game data or physics, causing memory leaks.
//
TeapotWarsGame::~TeapotWarsGame()
{
	while (!m_ActorList.empty())
	{
		ActorMap::iterator i = m_ActorList.begin();
		ActorId aid = (*i).first;
		VRemoveActor(aid);
	}
}


//
// TeapotWarsGame::VOnUpdate			- Chapter 19, page 709
//
void TeapotWarsGame::VOnUpdate(float time, float elapsedTime)
{
	int deltaMilliseconds = int(elapsedTime * 1000.0f);
	m_Lifetime += elapsedTime;

	BaseGameLogic::VOnUpdate(time, elapsedTime);

	if (m_bProxy)
		return;

	switch(m_State)
	{
	case BGS_LoadingGameEnvironment:
		break;
	
	case BGS_MainMenu:
		break;

	case BGS_WaitingForPlayers:
		if (m_ExpectedPlayers + m_ExpectedRemotePlayers == m_HumanPlayersAttached ) 
		{
			VChangeState(BGS_LoadingGameEnvironment);
		}
		break;

	case BGS_SpawnAI:
		if(g_pApp->m_pOptions->m_numAIs && m_Lifetime>5)//five seconds to wait for a human opponent
		{
			for (int i=0; i<g_pApp->m_pOptions->m_numAIs; ++i)
			{
				shared_ptr<IGameView> gameView(GCC_NEW AITeapotView(m_pPathingGraph));
				VAddView(gameView);
				m_Lifetime = 0;
			}
			VChangeState(BGS_Running);
		}
		break;

	case BGS_Running:
		break;

	default:
		assert(0 && _T("Unrecognized state."));
	}

	// look in Chapter 15, page 563 for more on this bit of code
	if(m_pPhysics)
	{
		m_pPhysics->VOnUpdate(elapsedTime);
		m_pPhysics->VSyncVisibleScene();
	}
}



void TeapotWarsGame::VSetProxy()
{
	TeapotWarsBaseGame::VSetProxy();
	safeAddListener( m_teapotWarsEventListener, EvtData_Game_State::sk_EventType );
}

//
// TeapotWarsGame::VChangeState					- Chapter 19, page 712
//
void TeapotWarsGame::VChangeState(BaseGameState newState)
{
	TeapotWarsBaseGame::VChangeState(newState);

	switch(newState)
	{
	case BGS_WaitingForPlayers:
		if (m_bProxy)
			break;

		for (int i=0; i<m_ExpectedPlayers; i++)
		{
			shared_ptr<IGameView> playersView(GCC_NEW TeapotWarsGameView());
			VAddView(playersView);			

			TeapotParams tp;
			tp.m_Mat = Mat4x4::g_Identity;
			tp.m_Mat.BuildRotationY(-D3DX_PI / 2.0f);
			tp.m_Mat.SetPosition(m_StartPosition);
			//tp.m_Pos = m_StartPosition;
			tp.m_Length = 2.5;
			tp.m_ViewId = playersView->VGetId();
			tp.m_Color = g_Green;
			const EvtData_Request_New_Actor requestActor( &tp );
			safeTriggerEvent( requestActor );
			m_StartPosition += Vec3(8, 0, 8);
		}

		for (int i=0; i<m_ExpectedRemotePlayers; i++)
		{
			TeapotParams tp;
			tp.m_Mat = Mat4x4::g_Identity;
			tp.m_Mat.BuildRotationY(-D3DX_PI / 2.0f);
			tp.m_Mat.SetPosition(m_StartPosition);
			tp.m_Length = 2.5;
			tp.m_ViewId = VIEWID_NO_VIEW_ATTACHED;
			tp.m_Color = g_Red;
			const EvtData_Request_New_Actor requestActor( &tp );
			safeTriggerEvent( requestActor );
			m_StartPosition += Vec3(8, 0, 8);
		}

		for (int i=0; i<m_ExpectedAI; i++)
		{
			shared_ptr<IGameView> aiView(GCC_NEW AITeapotView(m_pPathingGraph));
			VAddView(aiView);

			TeapotParams tp;
			tp.m_Mat = Mat4x4::g_Identity;
			tp.m_Mat.BuildRotationY(-D3DX_PI / 2.0f);
			tp.m_Mat.SetPosition(m_StartPosition);
			tp.m_Length = 2.5;
			tp.m_ViewId = aiView->VGetId();
			tp.m_Color = g_Yellow;
			const EvtData_Request_New_Actor requestActor( &tp );
			safeTriggerEvent( requestActor );
			m_StartPosition += Vec3(8, 0, 8);
		}

		break;
	}
}

//
// TeapotWarsGame::VAddView					- Chapter 19, page 713
//
void TeapotWarsGame::VAddView(shared_ptr<IGameView> pView, optional<ActorId> actor)
{
	TeapotWarsBaseGame::VAddView(pView, actor);
	if (boost::dynamic_pointer_cast<NetworkGameView>(pView))
	{
		m_HumanPlayersAttached++;
	}
	else if (boost::dynamic_pointer_cast<TeapotWarsGameView>(pView))
	{
		m_HumanPlayersAttached++;
	}
	else if (boost::dynamic_pointer_cast<AITeapotView>(pView))
	{
		m_AIPlayersAttached++;
	}
}








//
// TeapotWarsGame::VRegisterHit				- Chapter 19, page 714
//
void TeapotWarsGame::VRegisterHit(const ActorId sphere, const ActorId teapot)
{
	if (m_bProxy)
		return;

	//Make the teapot dizzy if it hits

	const float hitForce = 80.f;

	m_pPhysics->VApplyTorque(Vec3(0,1,0), hitForce, teapot);

	//everyone else gets a point, hahaha
	for( ActorScoreMap::iterator i=m_data.m_actorScores.begin()
		;i!=GetData().GetActorScores().end(); ++i )
	{
		if( i->first!=teapot )
		{
			++(i->second);
		}
	}

	VRemoveActor(sphere);
}


// VBuildInitialScene was added post-press.
void TeapotWarsGame::VBuildInitialScene()
{
	TeapotWarsBaseGame::VBuildInitialScene();

	if (m_bProxy)
		return;

	GridParams grid;
	grid.m_Color = g_Gray40;
	strcpy_s( grid.m_Texture, GridParams::sk_MaxTextureNameLen, "grid.dds" );
	grid.m_Mat = Mat4x4::g_Identity;
	grid.m_Squares = 100;

	/*
	const EvtData_Request_New_Actor grid1Evt( &grid );
	safeTriggerEvent( grid1Evt );
	*/

	float dim = 5.0f;
	float angle = D3DX_PI / 16.0f;
	float height = sin(angle) * (dim / 2.0f);
	grid.m_Color = g_Red;
	grid.m_Squares = dim;
	grid.m_Mat.BuildRotationX(-angle);
	grid.m_Mat.SetPosition(Vec3(0.0f, height, -dim));
	const EvtData_Request_New_Actor grid2Evt( &grid );
	safeTriggerEvent( grid2Evt );

	grid.m_Color = g_Blue;
	grid.m_Mat.BuildRotationX(angle);
	grid.m_Mat.SetPosition(Vec3(0.0f, height, dim));
	const EvtData_Request_New_Actor grid3Evt( &grid );
	safeTriggerEvent( grid3Evt );

	// super simple test object
	TestObjectParams top;
	top.m_Type = AT_TestObject;
	top.m_Color = g_Green;
	top.m_Mat = Mat4x4::g_Identity;
	top.m_Mat.SetPosition(Vec3(6.0f,0,18.0f));
	const EvtData_Request_New_Actor testObjEvt( &top );
	safeTriggerEvent( testObjEvt );
	
	//for (int i=0; i<5; i++)
	//{
	//	top.m_Pos = Vec3(-5.0f, 3.0f+i*2, -2.0f-(float(i)/2));
	//	aid = CreateTestObject(top);
	//	assert(aid.valid() && _T("Test object creation failed."));
	//}
}







// Script functions.
void TeapotWarsGame::SetCameraOffset( LuaObject gameViewIndex, LuaObject offsetTable )
{
	//Ensure we have some non-garbage data.
	if ( false == gameViewIndex.IsNumber() )
	{
		assert( 0 && "Called SetCameraOffset() and first param wasn't a number for gameview index!" );
		return;
	}
	if ( false == offsetTable.IsTable() )
	{
		assert( 0 && "Called SetCameraOffset() and second param wasn't a table for camera offset!" );
		return;
	}

	//Make sure we've got a good camera offset table.
	Vec4 camOffset( 0.0f, 0.0f, 0.0f, 0.0f );
	const int tableCount = offsetTable.GetTableCount();
	if ( 3 != tableCount )
	{
		assert( 0 && "Called SetCameraOffset() and second param didn't have exactly 3 entries!" );
		return;
	}
	camOffset.x = offsetTable[ 1 ].GetFloat();
	camOffset.y = offsetTable[ 2 ].GetFloat();
	camOffset.z = offsetTable[ 3 ].GetFloat();

	//Now ensure we have a valid value for game view.
	bool bFound = false;
	const GameViewId gameViewID = gameViewIndex.GetInteger();
	for ( GameViewList::iterator iter = m_gameViews.begin(), end = m_gameViews.end(); iter != end; ++iter )
	{
		shared_ptr<IGameView> pGameView = *iter;
		const GameViewId testID = pGameView->VGetId();
		if ( testID == gameViewID )
		{
			//Got our man, now ensure it's of the proper type.
			const GameViewType testType = pGameView->VGetType();
			if ( GameView_Human == testType )
			{
				bFound = true;
				//Cast it, then give it the message.
				HumanView * pHumanView = static_cast< HumanView * >( &*pGameView );
				pHumanView->VSetCameraOffset( camOffset );
			}
			break;
		}
	}

	assert( bFound && "Called SetCameraOffset() for an invalid game view ID." );
}


