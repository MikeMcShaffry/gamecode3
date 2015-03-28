//========================================================================
// PhysicsEventListener.cpp : implements the events sent FROM the phsyics system
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


//
// Basic, simple, physics event listener construct
//

#include "GameCodeStd.h"

#include "PhysicsEventListener.h"

#include "Physics.h"
#include "..\EventManager\Events.h"

const EventType EvtData_PhysTrigger_Enter::sk_EventType( "phys_trigger_enter" );

const EventType EvtData_PhysTrigger_Leave::sk_EventType( "phys_trigger_leave" );

const EventType EvtData_PhysCollision::sk_EventType( "phys_obj_collision" );

const EventType EvtData_PhysSeparation::sk_EventType( "phys_obj_separate" );


PhysicsEventListener::PhysicsEventListener( IGamePhysics *pPhysics )
	: m_pPhysics(pPhysics)
{
}

PhysicsEventListener::~PhysicsEventListener()
{
}


bool PhysicsEventListener::HandleEvent( IEventDataPtr const & event )
{
	const EventType & eventType = event->VGetEventType();
	if ( EvtData_PhysTrigger_Enter::sk_EventType == eventType )
	{
		// handle phys obj entering trigger volume
	}
	else if ( EvtData_PhysTrigger_Leave::sk_EventType == eventType )
	{
		// handle phys obj leaving trigger volume
	}
	else if ( EvtData_PhysCollision::sk_EventType == eventType )
	{
	}
	else if ( EvtData_PhysSeparation::sk_EventType == eventType )
	{
		// handle pressure plates, etc.
	}

	return false;
}