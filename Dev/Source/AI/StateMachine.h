#pragma once
//========================================================================
// StateMachine.h : Defines the core AI State Machine classes
// Author: David "Rez" Graham
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
//========================================================================

const int AISTATE_UPDATE_TIME = 500;

class PathPlan;
class StateMachine;
class PathingGraph;


//--------------------------------------------------------------------------------------------------------
// class AiState						- Chapter 17, page 642
// This is the base class for all AI states.  We inherit from CProcess, which allows us to use the 
// process manager to manage the updates automatically
//--------------------------------------------------------------------------------------------------------
class AiState : public CProcess
{
protected:
	int m_timer;
	PathPlan* m_pPlan;
	Vec3 m_target;
	ActorId m_actorId;
	StateMachine* m_pStateMachine;
	
public:
	AiState(const ActorId& actorId, StateMachine* pStateMachine, int processType);
	virtual ~AiState(void);
	virtual void VOnUpdate(const int deltaMilliseconds);
};


//--------------------------------------------------------------------------------------------------------
// class StateMachine							- Chapter 17, page 643
// Each AI actor has a StateMachine object that manages its internal AI state in the AITeapotView object.
// CreateState() is the factory method for spawning concrete state objects.  Since the state classes all 
// live in the game layer, it's up to that layer to inherit from this class and implement the function.
//--------------------------------------------------------------------------------------------------------
class StateMachine
{
protected:
	shared_ptr<PathingGraph> m_pPathingGraph;
	shared_ptr<AiState> m_pCurrState;
	ActorId m_actorId;
	
public:
	StateMachine(const ActorId& actorId, shared_ptr<PathingGraph> pPathingGraph);
	virtual ~StateMachine(void);
	
	void SetState(const std::string& stateName);
	
	shared_ptr<PathingGraph> GetPathingGraph(void) { return m_pPathingGraph; }
	
protected:
	// This is the factory method for spawning concrete state objects.  Since the state classes all live in 
	// the game layer, it's up to that layer to inherit from this class and implement this function.
	virtual void CreateState(const std::string& stateName) = 0;
};


