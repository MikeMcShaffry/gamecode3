//========================================================================
// File: TeapotController.cpp
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



#include "TeapotWarsStd.h"

#include "Actors.h"
#include "TeapotController.h"
#include "..\SceneGraph\SceneNodes.h"
#include "..\EventManager\EventManager.h"
#include "TeapotEvents.h"
#include "GameCode.h"
#include "TeapotWars.h"
#include "Physics\PhysicsEventListener.h"


////////////////////////////////////////////////////
// TeapotController Implementation
////////////////////////////////////////////////////


//
// TeapotController::TeapotController			- Chapter 19, page 735
//
TeapotController::TeapotController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch)
: m_object(object)
{
	memset(m_bKey, 0x00, sizeof(m_bKey));
}



//
// TeapotController::VOnLButtonDown				- Chapter 19, page 735
//
bool TeapotController::VOnLButtonDown(const CPoint &mousePos)
{
	optional<ActorId> aid = m_object->VGet()->ActorId();
	assert(aid.valid() && _T("The teapot controller isn't attached to a valid actor!"));
	safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Fire_Weapon( *aid ) ) );
	return true;
}



//
// TeapotController::OnUpdate				- Chapter 19, page 736
//
void TeapotController::OnUpdate(DWORD const deltaMilliseconds)
{
	if (m_bKey['W'] || m_bKey['S'])
	{
		const ActorId actorID = *m_object->VGet()->ActorId();
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Thrust( actorID, m_bKey['W']? 1.0f : -1.0 ) ) );
	}
	if (m_bKey['A'] || m_bKey['D'])
	{
		const ActorId actorID = *m_object->VGet()->ActorId();
		safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Steer( actorID, m_bKey['A']? -1.0 : 1.0 ) ) );
	}
}

