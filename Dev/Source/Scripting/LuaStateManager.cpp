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



#include "GameCodeStd.h"

#include "LuaStateManager.h"
#include "../EventManager/Events.h"

// LuaStateManager::LuaStateManager				- Chapter 11, page 317

LuaStateManager::LuaStateManager( void )
: m_GlobalState( true )	// 'true' Indicates to init the standard Lua library 
						// for dofile, etc.
{
	//Create our metatable...
	m_MetaTable = m_GlobalState->GetGlobals().CreateTable( "LuaStateManagerMetaTable" );
	m_MetaTable.SetObject( "__index", m_MetaTable );

	// Here we register two functions to make them accessible to script.
	m_MetaTable.RegisterObjectDirect( "DoFile", (LuaStateManager *)0, &LuaStateManager::DoFile );
	m_MetaTable.RegisterObjectDirect( "PrintDebugMessage", (LuaStateManager *)0, &LuaStateManager::PrintDebugMessage );
	
	LuaObject luaStateManObj = m_GlobalState->BoxPointer( this );
	luaStateManObj.SetMetaTable( m_MetaTable );

	// And here we expose the metatable as a named entity.
	m_GlobalState->GetGlobals().SetObject( "LuaStateManager", luaStateManObj );
}

LuaStateManager::~LuaStateManager()
{
}


// LuaStateManager::Init				- Chapter 11, page 318

bool LuaStateManager::Init( char const * const pInitFileName )
{
	// Create our global actor table.
	// This table will hold context for all actors created in the game world.
	LuaObject globals = m_GlobalState->GetGlobals();
	LuaObject actorTable = globals.CreateTable( "ActorList" );

	return DoFile( pInitFileName );
}

bool LuaStateManager::DoFile(char const * const pFileName)
{
	return ExecuteFile(m_GlobalState, pFileName);
}

bool LuaStateManager::ExecuteFile(LuaStateOwner & luaState, char const * const pFileName)
{
	const int retVal = luaState->DoFile(pFileName);
	const bool bSucceeded = ( 0 == retVal );
	assert( bSucceeded && "Unable to execute Lua script file!" );
	return bSucceeded;
}

int LuaStateManager::ExecuteString( char const * const pStringToExecute )
{
	const int retVal = m_GlobalState->DoString( pStringToExecute );

	return retVal;
}

LuaObject LuaStateManager::GetGlobalActorTable( void )
{
	return m_GlobalState->GetGlobal( "ActorList" );
}

void LuaStateManager::IdentifyLuaObjectType( LuaObject & objToTest )
{
	assert( !objToTest.IsNil() && "Nil!" );
	assert( !objToTest.IsBoolean() && "Boolean!" );
	assert( !objToTest.IsCFunction() && "C-Function!" );
	assert( !objToTest.IsFunction() && "Function!" );
	assert( !objToTest.IsInteger() && "Integer!" );
	assert( !objToTest.IsLightUserData() && "Light User Data!" );
	assert( !objToTest.IsNone() && "None!" );
	assert( !objToTest.IsNumber() && "Number!" );
	assert( !objToTest.IsString() && "String!" );
	assert( !objToTest.IsTable() && "Table!" );
	assert( !objToTest.IsUserData() && "User Data!" );
	assert( !objToTest.IsWString() && "Wide String!" );
	assert( 0 && "UNKNOWN!" );
}

void LuaStateManager::PrintDebugMessage( LuaObject debugObject )
{
	LuaObject stringObj;
	const char * pFinalStr = debugObject.ToString();

	//Generate an event.
	const EvtData_Debug_String debugEvent( ( NULL == pFinalStr ) ? "INVALID!" : pFinalStr, EvtData_Debug_String::kDST_ScriptMsg );
	safeTriggerEvent( debugEvent );
}