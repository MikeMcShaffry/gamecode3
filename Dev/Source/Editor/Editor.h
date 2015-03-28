#pragma once
//========================================================================
// Editor.h : source file for the sample game
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
//  class Editor						- Chapter 20, page 743
//  class EditorGame					- Chapter 20, page 745
//

#include "GameCodeStd.h"

#include "GameCode.h"
#include "Actors.h"

#include "Physics\Physics.h"
#include "Physics\PhysicsEventListener.h"
#include "MainLoop\Initialization.h"

#include "EventManager\Events.h"
#include "EventManager\EventManagerImpl.h"
//#include "EditorEvents.h"
#include "EditorGameView.h"
#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....


using namespace std;

class EditorGame;

class Editor : public GameCodeApp
{
private:
	void RegisterGameSpecificEvents( void );	//Registers game-specific events for later usage.
protected:
	BaseGameLogic *VCreateGameAndView();
	bool VLoadGame();
public:
	Editor() : GameCodeApp() { }
	inline EditorGame const * const GetGame() const;
//	bool InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight);

	TCHAR *VGetGameTitle() { return _T("Editor App"); }
	TCHAR *VGetGameAppDirectory() { return _T("Game Coding Complete 3\\Editor App\\1.0"); }
	HICON VGetIcon();
};


class EditorGame : public BaseGameLogic
{
	friend class EditorEventListener;

protected:
	float m_Lifetime;	//indicates how long this game has been in session
	EventListenerPtr m_editorEventListener;

public:
	EditorGame(GameOptions const &options);
	~EditorGame();
	
	bool VLoadGame(string gameName);
	virtual void VOnUpdate(float time, float elapsedTime);

	// We need to expose this information so that the C# app can
	// know how big of an array to allocate to hold the list of
	// actors
	int GetNumActors() { return (int)m_ActorList.size(); }

	// Exposes the actor map so that the global functions
	// can retrieve actor information
	ActorMap::iterator GetActorMapBegin() { return m_ActorList.begin(); }
	ActorMap::iterator GetActorMapEnd() { return m_ActorList.end(); }

	bool IsRunning() { return (BGS_Running == m_State); }
	shared_ptr<IGameView> GetView();
	
	void OpenLevelFile( char* fileName );
};


//
// Editor::GetGame							- Chapter 20, page 744
//
inline EditorGame const * const  Editor::GetGame() const
{ 
	return dynamic_cast<EditorGame*>(m_pGame); 
}