#pragma once
#ifndef __INCLUDED_SOURCE_SCRIPTLISTENER_H
#define __INCLUDED_SOURCE_SCRIPTLISTENER_H
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


#include "LuaStateManager.h"
#include "../EventManager/EventManager.h"

//--
// class ScriptEventListener				- Chapter 11, page 334
//
// The ScriptEventListener holds a script callback function that responds
// to a particular event.
class ScriptEventListener : public IEventListener
{
public:
	ScriptEventListener( LuaObject explicitHandlerFunction );

	virtual ~ScriptEventListener()
	{
	}

	virtual char const * GetName( void )
	{
		return "Script Listener";
	}

	virtual bool HandleEvent( IEventData const & event );

	const LuaObject & GetHandlerFunction( void ) const
	{
		return m_HandlerFunction;
	}

protected:
	// This function is virtual as sub-classes may pass additional
	// parameters.
	virtual bool VCallLuaFunction( LuaObject & eventData );

	LuaObject m_HandlerFunction;
};

//--
// class ScriptActorEventListener					- Chapter 11, page 342
//
// The ScriptActorEventListener holds a script callback function tied
// to a specific actor, and when called, passes in the actor's script
// data.
class ScriptActorEventListener : public ScriptEventListener
{
public:
	ScriptActorEventListener( LuaObject explicitHandlerFunction, const ActorId actorID );

	virtual ~ScriptActorEventListener()
	{
	}

	virtual char const * GetName( void )
	{
		return "Script Actor Listener";
	}

	ActorId GetActorID( void ) const
	{
		return m_SrcActorID;
	}

private:

	// This will pass the event data object as well as
	// look up our actor's specific script data to pass.
	virtual bool VCallLuaFunction( LuaObject & eventData );

	// Our source actor.
	const ActorId m_SrcActorID;
};

#endif	//__INCLUDED_SOURCE_SCRIPTLISTENER_H