#pragma once
//========================================================================
// TeapotStates.h : Defines some simple AI states
// Author: David "Rez" Graham
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

#include "ai/StateMachine.h"

class AITeapotView;


//--------------------------------------------------------------------------------------------------------
// class MoveState								- Chapter 17, page 646
// This is the base class for states that require movement using pathing.  Its purpose is to define a few
// functions common to pathing.  This class should never be instantiated by itself; always use one of the 
// base classes.
//--------------------------------------------------------------------------------------------------------
class MoveState : public AiState
{
protected:
	bool m_hasStartedRotation;
	float m_targetOrientation;
public:
	MoveState(const ActorId& actorId, StateMachine* pStateMachine, int processType) : AiState(actorId,pStateMachine,processType) {m_hasStartedRotation = false; m_targetOrientation = FLT_MAX;}
protected:
	void RotateAndMove(const int deltaMilliseconds);
};


//--------------------------------------------------------------------------------------------------------
// class AttackState
// This state causes the actor to face the victim and launch spheres at him.  If the victim moves out of
// range, the actor switches to the chase state.
//--------------------------------------------------------------------------------------------------------
class AttackState : public AiState
{
	bool m_hasStartedRotation;
	ActorId m_victim;
	int m_fireTimer;  // times how often we can fire
	int m_boredomTimer;  // times how long we stay in this state before changing
public:
	AttackState(const ActorId& actorId, StateMachine* pStateMachine);
	virtual void VOnUpdate(const int deltaMilliseconds);
	virtual void VOnInitialize(void);
private:
	void AttemptToFireWeapon(void);
};


//--------------------------------------------------------------------------------------------------------
// class ChaseTargetState						- Chapter 17, page 646
// This state chooses a random actor on the field and starts chasing him, launching into an attack (via
// the Attack state) when he gets within range.
//--------------------------------------------------------------------------------------------------------
class ChaseTargetState : public MoveState
{
	ActorId m_victim;
	int m_pathingTimer;
public:
	ChaseTargetState(const ActorId& actorId, StateMachine* pStateMachine) : MoveState(actorId,pStateMachine,PROC_AISTATE_CHASETARGET) { m_hasStartedRotation = false; m_pathingTimer = 0; }
	virtual void VOnUpdate(const int deltaMilliseconds);
	virtual void VOnInitialize(void);
};


//--------------------------------------------------------------------------------------------------------
// class WanderState
// The wandering behavior chooses a random node on the pathing graph and heads there.
//--------------------------------------------------------------------------------------------------------
class WanderState : public MoveState
{
public:
	WanderState(const ActorId& actorId, StateMachine* pStateMachine) : MoveState(actorId,pStateMachine,PROC_AISTATE_WANDER) {m_hasStartedRotation = false;}
	virtual void VOnUpdate(const int deltaMilliseconds);
	virtual void VOnInitialize(void);
};


//--------------------------------------------------------------------------------------------------------
// class SpinState
// The spin state just causes the teapot to spin.  Wheee!
//--------------------------------------------------------------------------------------------------------
class SpinState : public AiState
{
	int m_turnTimer;
public:
	SpinState(const ActorId& actorId, StateMachine* pStateMachine) : AiState(actorId,pStateMachine,PROC_AISTATE_SPIN) {}
	virtual void VOnUpdate(const int deltaMilliseconds);
private:
	void DumpOrientation(void);
};


//--------------------------------------------------------------------------------------------------------
// class WaitState
// Just wait here.
//--------------------------------------------------------------------------------------------------------
class WaitState : public AiState
{
public:
	WaitState(const ActorId& actorId, StateMachine* pStateMachine) : AiState(actorId,pStateMachine,PROC_AISTATE_WAIT) {}
};


//--------------------------------------------------------------------------------------------------------
// class TeapotStateMachine					- Chapter 17, page 644
// This is the implementation class for StateMachine that's specific to Teapot Wars.  It implements the 
// CreateState() function which is something you'll rewrite for every game.  Another solution would be
// to have a set of generic states that are created from the base class version of this function.  That 
// way you only inherit from StateMachine if you need some game-specific states.
//--------------------------------------------------------------------------------------------------------
class TeapotStateMachine : public StateMachine
{
public:
	TeapotStateMachine(const ActorId& actorId, shared_ptr<PathingGraph> pPathingGraph) : StateMachine(actorId,pPathingGraph) {}
	virtual void CreateState(const std::string& stateName);
};

