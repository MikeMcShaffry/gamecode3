#pragma once
//========================================================================
// AiEvents.h : Events for the AI system
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


struct EvtData_AiSteer : public BaseEventData
{
private:
	LuaObject m_LuaEventData;

public:
	static const EventType sk_EventType;
	ActorId m_id;
	float m_angleRadians;  // from -PI to PI
	float m_timeMs;

	explicit EvtData_AiSteer(ActorId id, float angleRadians, float timeMs)
		: m_id(id), 
		m_angleRadians(angleRadians),
		m_timeMs(timeMs)
	{}

	explicit EvtData_AiSteer(std::istrstream& in)
	{
		in >> m_id;
		in >> m_angleRadians;
		in >> m_timeMs;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( GCC_NEW EvtData_AiSteer (m_id, m_angleRadians, m_timeMs) );
	}

	virtual const EventType& VGetEventType(void) const { return sk_EventType; }

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	virtual void VBuildLuaEventData(void)
	{
		assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

		//Get the global state.
		LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
		m_LuaEventData.AssignNewTable( pState );

		//Set appropriate data.
		m_LuaEventData.SetInteger( "ActorId", m_id );
		m_LuaEventData.SetNumber( "AngleRadians", m_angleRadians );  // maybe add on for degrees since it's more human readable?
		m_LuaEventData.SetNumber( "TimeMs", m_timeMs);

		m_bHasLuaEventData = true;
	}
};


class AiEventListener : public IEventListener
{
public:
	explicit AiEventListener(void) : IEventListener() {}
	void ListenForAiEvents(void);
	char const* GetName(void) { return "AiEventListener"; }
	virtual bool HandleEvent( IEventData const & event );
};


