#pragma once
//========================================================================
// TeapotWars.h : source file for the sample game
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

#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....

#include "..\Scripting\LuaStateManager.h"

//========================================================================
//  Content References in Game Coding Complete 3rd Edition
// 
//  class TeapotWarsGameApp				- Chapter 19, page 701
//  class TeapotWarsGameData			- Chapter 19, page 703
//  class TeapotWarsBaseGame			- Chapter 19, page 704
//  class TeapotWarsGame				- Chapter 19, page 706
//

class TeapotWarsBaseGame;
class PathingGraph;


// Note: Don't put anything in this class that needs to be destructed

class TeapotWarsGameApp : public GameCodeApp
{
private:
	void RegisterGameSpecificEvents( void );	//Registers game-specific events for later usage.

protected:
	virtual BaseGameLogic *VCreateGameAndView();
	virtual bool VLoadGame();

public:
	TeapotWarsGameApp() : GameCodeApp() { }
	inline TeapotWarsBaseGame const * const GetGame() const;

	virtual TCHAR *VGetGameTitle() { return _T("Teapot Wars"); }
	virtual TCHAR *VGetGameAppDirectory() { return _T("Game Coding Complete 3\\Teapot Wars\\3.0"); }
	virtual HICON VGetIcon();
};


class IEventManager;
typedef std::map<ActorId, int> ActorScoreMap;


// Presumably we'll put more in this data structure - 
// It is going to be mirrored in a game logic proxy if the server is remote.
class TeapotWarsGameData 
{
public:
	ActorScoreMap m_actorScores;

	const ActorScoreMap &GetActorScores() const { return m_actorScores; }
	void RemoveActor(ActorId id) { m_actorScores.erase(id); }
};


class TeapotWarsBaseGame : public BaseGameLogic
{
protected:
	// TODO: This should really be private
	TeapotWarsGameData m_data;


public: 
	TeapotWarsBaseGame(GameOptions const &options)
		: BaseGameLogic(options) 
	{ }

	bool VLoadGame(std::string gameName);
	void VRemoveActor(ActorId id);
	void VAddActor(shared_ptr<IActor> actor, ActorParams *p);
	void VSetProxy() { BaseGameLogic::VSetProxy(); }

	// overloadable
	virtual void VRegisterHit(const ActorId sphere, const ActorId teapot) { }

	const TeapotWarsGameData& GetData() const { return m_data; }
};



class TeapotWarsGame : public TeapotWarsBaseGame
{
	friend class TeapotWarsEventListener;

protected:
	float m_Lifetime;	//indicates how long this game has been in session
	Vec3 m_StartPosition;
	int m_HumanPlayersAttached;
	int m_AIPlayersAttached;
	EventListenerPtr m_teapotWarsEventListener;

public:
	TeapotWarsGame(struct GameOptions const &options);

	~TeapotWarsGame();

	// TeapotWars Methods

	// Update
	virtual void VOnUpdate(float time, float elapsedTime);
	virtual void VSetProxy();

	// Overloads
	virtual void VRegisterHit(const ActorId sphere, const ActorId teapot);
	virtual void VBuildInitialScene();
	virtual void VChangeState(BaseGameState newState);
	virtual void VAddView(shared_ptr<IGameView> pView, optional<ActorId> actor=optional_empty());
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) { return m_pPhysics; }
	
	// set/clear render diagnostics
	void ToggleRenderDiagnostics() { m_RenderDiagnostics = !m_RenderDiagnostics; }

private:
	// These functions actually CREATE the actor.  They should only be called as a result
	// of a Request_New_Actor.
//	optional<ActorId> CreateTestObjectActor(TestObjectParams const &p);
	//optional<ActorId> CreateSphereActor(SphereParams const &p);
//	optional<ActorId> CreateTeapotActor(TeapotParams const &p);
//	optional<ActorId> CreateAiTeapotActor(AiTeapotParams const &p);
//	optional<ActorId> CreateGridActor(GridParams const &p);

private:
	//Allows access for script calls.
	LuaObject m_MetaTable;

	//Script accessible functions.
	void SetCameraOffset( LuaObject gameViewIndex, LuaObject offsetTable );
};

inline TeapotWarsBaseGame const * const  TeapotWarsGameApp::GetGame() const
{ 
	return dynamic_cast<TeapotWarsBaseGame*>(m_pGame); 
}