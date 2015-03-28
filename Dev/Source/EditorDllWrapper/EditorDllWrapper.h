#pragma once
//========================================================================
// EditorDllWrapper.h
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

#define DllExport _declspec(dllexport)

#include "..\Editor\EditorGlobalFunctions.h"

//===================================================================
//
// This file exposes the functions in the EditorGlobalFunctions file
// for use by the C# app.
//
//===================================================================

extern "C" DllExport int DllWrapperEditorMain(int instancePtrAddress, int hPrevInstancePtrAddress, int hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight);
extern "C" DllExport void DllWrapperRenderFrame();
extern "C" DllExport void DllWrapperDXShutdown();
extern "C" DllExport void DllWrapperWndProc(int hWndPtrAddress, int msg, int wParam, int lParam);

extern "C" DllExport bool DllWrapperIsGameRunning();

extern "C" DllExport int DllWrapperGetActorType( ActorId actorId );
extern "C" DllExport int DllWrapperGetNumActors();
extern "C" DllExport void DllWrapperGetActorList( int actorIdArrayPtrAddress, int size );
extern "C" DllExport DWORD DllWrapperGetActorColor( ActorId actorId );
extern "C" DllExport void DllWrapperGetActorPos( int actorPosPtrAddress, ActorId actorId );
extern "C" DllExport int DllWrapperPickActor(int hWndPtrAddress);

extern "C" DllExport void DllWrapperSetActorPos(ActorId actorId, const float x, const float y, const float z);
extern "C" DllExport void DllWrapperRotateActorXAxis( ActorId actorId, float radians );
extern "C" DllExport void DllWrapperRotateActorYAxis( ActorId actorId, float radians );
extern "C" DllExport void DllWrapperRemoveActor( ActorId actorId );
extern "C" DllExport void DllWrapperCreateActor( ActorType actorType, DWORD color, float length, float radius, LPCTSTR lEffectFileName, LPCTSTR lMeshFileName );

extern "C" DllExport void DllWrapperSaveLevel( LPCTSTR lFileName );
extern "C" DllExport void DllWrapperOpenLevel( LPCTSTR lFileName );