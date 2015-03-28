//========================================================================
// EditorDllWrapper.cpp
//
// Part of the EditorDllWrapper Application
//
// GameCode3 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 3rd Edition" by Mike McShaffry, published by
// Paraglyph Press. ISBN: 1-932111-91-3
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the author a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: http://www.amazon.com/exec/obidos/ASIN/1932111913/gamecodecompl-20/
//
// There's also a companion web site at http://www.mcshaffry.com/GameCode/portal.php
//
// (c) Copyright 2005 Michael L. McShaffry
//
// This work is licensed under the Creative Commons Attribution-ShareAlike License. 
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/1.0/ 
// or send a letter to:
//      Creative Commons
//      559 Nathan Abbott Way
//      Stanford, California 94305, USA.
//
//========================================================================
#include "EditorDllWrapper.h"

int DllWrapperEditorMain(int instancePtrAddress, int hPrevInstancePtrAddress, int hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight)
{
	// C# gets unhappy passing pointers to unmanaged dlls, so instead we pass
	// the actual address of the pointer itself as an int. Then we convert the int
	// into a pointer value.
	
	// In general, we do this for any pointers that we pass from C# to the unmanaged dll.
	HINSTANCE hInstance = (HINSTANCE)instancePtrAddress;
	HINSTANCE hPrevInstance = (HINSTANCE) hPrevInstancePtrAddress;
	HWND hWnd = (HWND)hWndPtrAddress;
	return EditorMain(hInstance, hPrevInstance, L"", hWnd, nCmdShow, screenWidth, screenHeight);
}

void DllWrapperRenderFrame()
{
	RenderFrame();
}

void DllWrapperDXShutdown()
{
	DXShutdown();
}

void DllWrapperWndProc(int hWndPtrAddress, int msg, int wParam, int lParam)
{
	HWND hWnd = (HWND)hWndPtrAddress;
	WndProc( hWnd, msg, WPARAM(wParam), LPARAM(lParam) );
}

bool DllWrapperIsGameRunning()
{
	bool isRunning = IsRunning();
	return isRunning;
}

int DllWrapperGetActorType( ActorId actorId )
{
	return GetActorType( actorId );
}

int DllWrapperGetNumActors()
{
	return GetNumActors();
}

void DllWrapperGetActorList( int actorIdArrayPtrAddress, int size )
{
	int* actorIdArrayPtr = (int*)actorIdArrayPtrAddress;
	GetActorList( actorIdArrayPtr, size );
}

DWORD DllWrapperGetActorColor( ActorId actorId )
{
	return GetActorColor(actorId);
}

void DllWrapperGetActorPos( int actorPosPtrAddress, ActorId actorId )
{
	float* actorPosPtr = (float*)actorPosPtrAddress;
	GetActorPos( actorPosPtr, actorId );
}

int DllWrapperPickActor(int hWndPtrAddress)
{
	HWND hWnd = (HWND)hWndPtrAddress;
	return PickActor(hWnd);
}

void DllWrapperSetActorPos(ActorId actorId, const float x, const float y, const float z)
{
	SetActorPos( actorId, x, y, z );
}

void DllWrapperRotateActorXAxis( ActorId actorId, float radians )
{
	RotateActorXAxis( actorId, radians );
}

void DllWrapperRotateActorYAxis( ActorId actorId, float radians )
{
	RotateActorYAxis( actorId, radians );
}

void DllWrapperRemoveActor( ActorId actorId )
{
	RemoveActor( actorId );
}

void DllWrapperCreateActor( ActorType actorType, DWORD color, float length, float radius, LPCTSTR lEffectFileName, LPCTSTR lMeshFileName )
{
	CreateActor( actorType, color, length, radius, (char*)lEffectFileName, (char*)lMeshFileName );
}

void DllWrapperSaveLevel( LPCTSTR lFileName )
{
	// When passing strings from C# to unmanaged dlls, you
	// have to pass as a LPCTSTR and cast to a char*
	SaveLevel( (char*)lFileName );
}

void DllWrapperOpenLevel( LPCTSTR lFileName )
{
	OpenLevel( (char*)lFileName );
}
