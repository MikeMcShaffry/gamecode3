#pragma once
//========================================================================
// EditorGameView.h : source file for the sample game
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

#include "string.h"
#include "SceneGraph\SceneNodes.h"
#include "EventManager\EventManager.h"

//========================================================================
//
// This will be similar to TeapotWarsView, but will be much more 
// streamlined since we don't a HUD, or a main menu screen, or sound 
// controllers.
//
//========================================================================

void ListenForEditorEvents(EventListenerPtr listener);

class EditorController;


class EditorGameView : public HumanView
{
	friend class EditorGameViewListener;

protected:
	BaseGameState m_BaseGameState;	// what is the current game state

	shared_ptr<ScreenElementScene> m_pScene;
	shared_ptr<MovementController> m_pFreeCameraController;
	shared_ptr<CameraNode> m_pCamera;
	void BuildInitialScene();

	void MoveActor(ActorId id, Mat4x4 const &mat);
public:

	EditorGameView(); 
	virtual void VOnUpdate(int deltaMilliseconds);
	shared_ptr<ScreenElementScene> GetScene()	{ return m_pScene; }
};


class EditorGameViewListener : public IEventListener
{
public:
	explicit EditorGameViewListener( EditorGameView *view);
	virtual char const * GetName(void) { return "EditorGameView"; }
	virtual bool HandleEvent( IEventData const & event );
private:
	EditorGameView *m_pView;
};