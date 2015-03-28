//========================================================================
// StateMachine.cpp : Defines the core AI State Machine classes
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

#include "GameCodeStd.h"
#include "../MainLoop/CProcess.h"
#include "Pathing.h"
#include "StateMachine.h"
#include "../GameCode.h"


//------------------------------------------------------------------------------------------------
// AiState
//------------------------------------------------------------------------------------------------
AiState::AiState(const ActorId& actorId, StateMachine* pStateMachine, int processType) : CProcess(processType)
{
	m_timer = 0;
	m_actorId = actorId;
	m_pStateMachine = pStateMachine;
	m_pPlan = NULL;
}

AiState::~AiState(void)
{
	if (m_pPlan)
		delete m_pPlan;
}

void AiState::VOnUpdate(const int deltaMilliseconds)
{
	CProcess::VOnUpdate(deltaMilliseconds);
}


//------------------------------------------------------------------------------------------------
// StateMachine
//------------------------------------------------------------------------------------------------
StateMachine::StateMachine(const ActorId& actorId, shared_ptr<PathingGraph> pPathingGraph)
{
	m_pPathingGraph = pPathingGraph;
	m_actorId = actorId;
}

StateMachine::~StateMachine(void)
{
	OutputDebugString(_T("Destroying state machine\n"));
}

//------------------------------------------------------------------------------------------------
// StateMachine::SetState				- Chapter 17, 645
//------------------------------------------------------------------------------------------------
void StateMachine::SetState(const std::string& stateName)
{
	// kill the old state if there is one
	if (m_pCurrState)
	{
		m_pCurrState->VKill();
		m_pCurrState.reset();
	}

	// instantiate the concrete state object
	CreateState(stateName);

	// attach the state process
	if (m_pCurrState)
		g_pApp->m_pGame->AttachProcess(m_pCurrState);
}
