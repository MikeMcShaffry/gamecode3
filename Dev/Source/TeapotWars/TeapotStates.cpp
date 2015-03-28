//========================================================================
// TeapotStates.cpp : Defines some simple AI states
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

#include "TeapotWarsStd.h"
#include "../MainLoop/CProcess.h"
#include "TeapotStates.h"
#include "../ai/Pathing.h"
#include "../EventManager/EventManager.h"
#include "../TeapotWars/TeapotEvents.h"

const float AI_TEAPOT_SPEED = 2.0f;

// AttackState const's
const int AI_TEAPOT_FIRE_TIMER = 1500;  // milliseconds before the teapot can fire another shot
const int AI_TEAPOT_BOREDOM_TIMER = 10000;  // milliseconds before the teapot gets bored and wanders off
const int AI_TEAPOT_PATHING_TIMER = 1000;  // milliseconds before teapot is allowed to replan
const float AI_TEAPOT_ATTACK_MIN = 15.0f;
const float AI_TEAPOT_ATTACK_MAX = 30.0f;


//------------------------------------------------------------------------------------------------
// MoveState::RotateAndMove						- Chapter 17, page 649
//------------------------------------------------------------------------------------------------
void MoveState::RotateAndMove(const int deltaMilliseconds)
{
	// grab the necessary interfaces
	shared_ptr<IGamePhysics> pPhysics = g_pApp->m_pGame->VGetGamePhysics();
	assert(pPhysics);
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor(m_actorId);
	assert(pActor);

	// calculate orientation
	float orientation = pPhysics->VGetOrientationY(m_actorId);
	Vec3 diff = m_target - pActor->VGetMat().GetPosition();
	if (m_targetOrientation == FLT_MAX)
		m_targetOrientation = GetYRotationFromVector(diff);

	// if we're not facing the right direction, start rotating if we haven't already
	if (fabs(m_targetOrientation - orientation) > 0.001f)
	{
		if (!m_hasStartedRotation)
		{
			pActor->VRotateY(m_targetOrientation);
			m_hasStartedRotation = true;
		}
	}

	// if we're already facing the right direction, pretend we rotated there
	else if (!m_hasStartedRotation)
		m_hasStartedRotation = true;


	// if we get here, we're done rotating so start moving
	else
	{
		m_targetOrientation = FLT_MAX;
		m_hasStartedRotation = false;
		diff.Normalize();
		float speed = AI_TEAPOT_SPEED * ((float)deltaMilliseconds / 1000.0f);
		diff *= speed;
		pPhysics->VTranslate(m_actorId,diff);
	}
}


//------------------------------------------------------------------------------------------------
// AttackState
//------------------------------------------------------------------------------------------------
AttackState::AttackState(const ActorId& actorId, StateMachine* pStateMachine) : AiState(actorId,pStateMachine,PROC_AISTATE_ATTACK)
{
	m_hasStartedRotation = false;
	m_fireTimer = 0;
	m_boredomTimer = 0;
}


void AttackState::VOnInitialize(void)
{
	AiState::VOnInitialize();
	m_victim = g_pApp->m_pGame->GetRandomActor(m_actorId);
	m_timer = AI_TEAPOT_FIRE_TIMER;
}

void AttackState::VOnUpdate(const int deltaMilliseconds)
{
	AiState::VOnUpdate(deltaMilliseconds);
	
	// update our timers
	m_fireTimer += deltaMilliseconds;
	m_boredomTimer += deltaMilliseconds;
	
	// check for boredom
	if (m_boredomTimer >= AI_TEAPOT_BOREDOM_TIMER)
		m_pStateMachine->SetState("wander");
	
	// Grab the actors.  If we can't find the victim actor, it means he was destroyed so we 
	// go about our business.
	shared_ptr<IGamePhysics> pPhysics = g_pApp->m_pGame->VGetGamePhysics();
	assert(pPhysics);
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor(m_actorId);
	assert(pActor);
	shared_ptr<IActor> pVictim = g_pApp->m_pGame->VGetActor(m_victim);
	if (!pVictim)
	{
		m_pStateMachine->SetState("wander");
		return;
	}
	
	// check to see how close we are and our orientation
	Vec3 actorPos(pActor->VGetMat().GetPosition());
	Vec3 victimPos(pVictim->VGetMat().GetPosition());
	Vec3 diff = victimPos - actorPos;
	float dist = diff.Length();
	float orientation = pPhysics->VGetOrientationY(m_actorId);

	// if we're not within firing range, chase the target
	if (dist < AI_TEAPOT_ATTACK_MIN || dist > AI_TEAPOT_ATTACK_MAX)
	{
		m_pStateMachine->SetState("chase");
		return;
	}
	
	float targetOrientation = GetYRotationFromVector(diff);

	// if we're not facing the right direction, start rotating if we haven't already
	if (fabs(targetOrientation - orientation) > 0.001f)
	{
		if (!m_hasStartedRotation)
		{
			pActor->VRotateY(targetOrientation);
			m_hasStartedRotation = true;
		}
	}

	// if we're already facing the right direction, pretend we rotated there
	else if (!m_hasStartedRotation)
		m_hasStartedRotation = true;
		
	// we're within range and facing the right way so try and fire a shot
	else
	{
		AttemptToFireWeapon();
		return;
	}
}

void AttackState::AttemptToFireWeapon(void)
{
	if (m_fireTimer >= AI_TEAPOT_FIRE_TIMER)
	{
		safeQueEvent(IEventDataPtr(GCC_NEW EvtData_Fire_Weapon(m_actorId)));
		m_fireTimer = 0;
	}
}


//------------------------------------------------------------------------------------------------
// ChaseTargetState
//------------------------------------------------------------------------------------------------
void ChaseTargetState::VOnInitialize(void)
{
	AiState::VOnInitialize();
	m_victim = g_pApp->m_pGame->GetRandomActor(m_actorId);
}

//------------------------------------------------------------------------------------------------
// ChaseTargetState::VOnUpdate					- Chapter 17, page 646
//------------------------------------------------------------------------------------------------
void ChaseTargetState::VOnUpdate(const int deltaMilliseconds)
{
	AiState::VOnUpdate(deltaMilliseconds);
	
	m_pathingTimer += deltaMilliseconds;

	// grab the necessary interfaces
	shared_ptr<IGamePhysics> pPhysics = g_pApp->m_pGame->VGetGamePhysics();
	assert(pPhysics);
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor(m_actorId);
	assert(pActor);
	
	// Try to grab the victim.  If we can't, he's probably dead already so switch to wander.
	shared_ptr<IActor> pVictim = g_pApp->m_pGame->VGetActor(m_victim);
	if (!pVictim)
	{
		m_pStateMachine->SetState("wander");
		return;
	}

	// grab the actors' positions
	Vec3 actorPos(pActor->VGetMat().GetPosition());
	Vec3 victimPos(pVictim->VGetMat().GetPosition());
	Vec3 diff = victimPos - actorPos;
	float dist = diff.Length();
	
	// check to see if we're close enough to start attacking
	if (dist >= AI_TEAPOT_ATTACK_MIN && dist <= AI_TEAPOT_ATTACK_MAX)
	{
		m_pStateMachine->SetState("attack");
		return;
	}
	
	// If we're not within range, path to where we need to be.  We'll never reach the end since the above condition
	// will be met long before we reach the destination.
	else
	{
		// if we have an invalid pathing plan or the plan we do have is stale, replan
		if (!m_pPlan || m_pathingTimer >= AI_TEAPOT_PATHING_TIMER || m_pPlan->CheckForEnd())
		{
			// path to victim
			SAFE_DELETE(m_pPlan);
			if (dist > AI_TEAPOT_ATTACK_MAX)  // too far away, try and path to his location
				m_pPlan = m_pStateMachine->GetPathingGraph()->FindPath(actorPos,victimPos);
			else  // too close, try and path to furthest node away from him
			{
				PathingNode* pFurthestNode = m_pStateMachine->GetPathingGraph()->FindFurthestNode(victimPos);
				m_pPlan = m_pStateMachine->GetPathingGraph()->FindPath(actorPos,pFurthestNode);
			}
			
			// if we can't find a path to the victim, give up
			if (!m_pPlan)
			{
				m_pStateMachine->SetState("wander");
				return;
			}

			// set our target pos
			m_pPlan->ResetPath();
			m_target = m_pPlan->GetCurrentNodePosition();
			m_pathingTimer = 0;
		}
		
		// If we get here, we have a valid plan that we're executing.  Calculate our target orientation.
		
		// check to see if we're close enough to the next node
		if (m_pPlan->CheckForNextNode(actorPos))
		{
			m_target = m_pPlan->GetCurrentNodePosition();
			m_hasStartedRotation = false;
			return;
		}
		
		// still too far so check to see that we're facing the right way
		else
			RotateAndMove(deltaMilliseconds);
	}
}


//------------------------------------------------------------------------------------------------
// WanderState
//------------------------------------------------------------------------------------------------
void WanderState::VOnInitialize(void)
{
	AiState::VOnInitialize();

	// grab the actor
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor(m_actorId);
	assert(pActor);

	// build a path to a random node
	do
	{
		PathingNode* pGoalNode = m_pStateMachine->GetPathingGraph()->FindRandomNode();
		m_pPlan = m_pStateMachine->GetPathingGraph()->FindPath(pActor->VGetMat().GetPosition(),pGoalNode);
	}
	while (!m_pPlan);
	
	
	m_pPlan->ResetPath();

	// set our target, and away we go
	m_target = m_pPlan->GetCurrentNodePosition();
}

void WanderState::VOnUpdate(const int deltaMilliseconds)
{
	AiState::VOnUpdate(deltaMilliseconds);

	// grab the necessary interfaces
	shared_ptr<IGamePhysics> pPhysics = g_pApp->m_pGame->VGetGamePhysics();
	assert(pPhysics);
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor(m_actorId);
	assert(pActor);

	// grab the actor's position
	Vec3 pos(pActor->VGetMat().GetPosition());
	
	// if we're close enough, set the next target and bail until next frame
	if (m_pPlan->CheckForNextNode(pos))
	{
		// check to see if we're at the end
		if (m_pPlan->CheckForEnd())
		{
			OutputDebugString(_T(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>End of path!!\n"));
			m_pStateMachine->SetState("chase");
			return;
		}

		OutputDebugString(_T("Found Node\n"));
	
		m_target = m_pPlan->GetCurrentNodePosition();
		m_hasStartedRotation = false;
		return;
	}
	
	// if we get here, we have a valid plan and just need to rotate & move into position
	RotateAndMove(deltaMilliseconds);
}



//------------------------------------------------------------------------------------------------
// SpinState
//------------------------------------------------------------------------------------------------
#define TURN_TIMER (2000)

void SpinState::VOnUpdate(const int deltaMilliseconds)
{
	m_turnTimer -= deltaMilliseconds;

	if (m_turnTimer <= 0)
	{
		shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor(m_actorId);
		assert(pActor);

		static bool flip = true;

		if (flip)
			pActor->VRotateY(D3DX_PI/2);
		else
			pActor->VRotateY(D3DX_PI);
		flip = !flip;
		m_turnTimer = TURN_TIMER;
		
		DumpOrientation();
	}
}

void SpinState::DumpOrientation(void)
{
	wchar_t str[64];  // I'm sure this is overkill
	memset(str,0,sizeof(wchar_t));
	shared_ptr<IGamePhysics> pPhysics = g_pApp->m_pGame->VGetGamePhysics();
	float deg = pPhysics->VGetOrientationY(m_actorId) * (180.0f / D3DX_PI);
	swprintf_s(str,64,_T("Turn: %f\n"),deg);
	OutputDebugString(str);
}


//------------------------------------------------------------------------------------------------
// TeapotStateMachine::CreateState					- Chapter 17, page 644
//------------------------------------------------------------------------------------------------
void TeapotStateMachine::CreateState(const std::string& stateName)
{
	// convert to lower case
	std::string lowerStateName(stateName);
	std::transform(lowerStateName.begin(),lowerStateName.end(),lowerStateName.begin(),tolower);

	if (lowerStateName == "attack")
		m_pCurrState.reset(GCC_NEW AttackState(m_actorId,this));
	else if (lowerStateName == "chase")
		m_pCurrState.reset(GCC_NEW ChaseTargetState(m_actorId,this));
	else if (lowerStateName == "wander")
		m_pCurrState.reset(GCC_NEW WanderState(m_actorId,this));
	else if (lowerStateName == "spin")
		m_pCurrState.reset(GCC_NEW SpinState(m_actorId,this));
	else if (lowerStateName == "wait")
		m_pCurrState.reset(GCC_NEW WaitState(m_actorId,this));
	else
		OutputDebugString(_T("Couldn't find state"));
}

