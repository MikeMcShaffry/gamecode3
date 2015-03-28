#pragma once
//========================================================================
// EditorGlobalFunctions.h
//
// Part of the Editor Application
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

#define DllExport _declspec(dllexport)

#include "Editor.h"

//========================================================================
//
// This file contains functions that will be exposed via the dll wrapper
// to the C# app. The functions will be used by the C# app to query information
// about the editor, and to change the editor's scene.
//
//		- Chapter 20, page 771
//
//========================================================================


extern "C" DllExport int EditorMain(int *instancePtrAddress, int *hPrevInstancePtrAddress, int *hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight);
extern "C" DllExport void RenderFrame();
extern "C" DllExport int DXShutdown();
extern "C" DllExport void WndProc(int *hWndPtrAddress, int msg, int wParam, int lParam);

extern "C" DllExport bool IsGameRunning();

extern "C" DllExport int GetActorType( ActorId actorId );
extern "C" DllExport int GetNumActors();
extern "C" DllExport void GetActorList( int *actorIdArrayPtrAddress, int size );
extern "C" DllExport DWORD GetActorColor( ActorId actorId );
extern "C" DllExport void GetActorPos( int *actorPosPtrAddress, ActorId actorId );
extern "C" DllExport int PickActor(int *hWndPtrAddress);

extern "C" DllExport void SetActorPos(ActorId actorId, const float x, const float y, const float z);
extern "C" DllExport void RotateActorXAxis( ActorId actorId, float radians );
extern "C" DllExport void RotateActorYAxis( ActorId actorId, float radians );
extern "C" DllExport void RemoveActor( ActorId actorId );
extern "C" DllExport void CreateActor( ActorType actorType, DWORD color, float length, float radius, LPCTSTR lEffectFileName, LPCTSTR lMeshFileName );

extern "C" DllExport void SaveLevel( LPCTSTR lFileName );
extern "C" DllExport void OpenLevel( LPCTSTR lFileName );


void WriteBeginningParamsBlock(FILE* pFile, std::string paramType, ActorId actorId);
void WriteEndParamsBlock(FILE* pFile, std::string paramType, ActorId actorId);
void WriteMatrixParams( FILE* pFile, const Mat4x4 &mat );
void WriteColorParams( FILE* pFile, const Color &color );
void WritePosParams( FILE* pFile, const Vec3 &pos );
void ReplaceAllCharacters( std::string& origString, std::string subString, std::string newSubString );