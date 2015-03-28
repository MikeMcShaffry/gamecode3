//=======================================================================// EditorGlobalFunctions.cpp
//
// Part of the Editor Application
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

#include "EditorGlobalFunctions.h"

#define INVALID_ID -1;





//
// EditorMain								- Chapter 20, page 752
//
// This function is similar to the Main in GameCode3, but with a few
// key differences. We don't want to use GameCode's main loop since
// that would lock out the C# app.
//

int EditorMain(int *instancePtrAddress, int *hPrevInstancePtrAddress, int *hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight)
{
	// C# gets unhappy passing pointers to unmanaged dlls, so instead we pass
	// the actual address of the pointer itself as an int. Then we convert the int
	// into a pointer value.
	
	// In general, we do this for any pointers that we pass from C# to the unmanaged dll.
	HINSTANCE hInstance = (HINSTANCE)instancePtrAddress;
	HINSTANCE hPrevInstance = (HINSTANCE) hPrevInstancePtrAddress;
	HWND hWnd = (HWND)hWndPtrAddress;
	WCHAR *lpCmdLine = L"";

	// Set up checks for memory leaks.
	// Game Coding Complete reference - Chapter 12, page 446
	//
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// set this flag to keep memory blocks around
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;				// this flag will cause intermittent pauses in your game!

	// perform memory check for each alloc/dealloc
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;				// remember this is VERY VERY SLOW!

	// always perform a leak check just before app exits.
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;					

	_CrtSetDbgFlag(tmpDbgFlag);


    // Set the callback functions. These functions allow the sample framework to notify
    // the application about device changes, user input, and windows messages.  The 
    // callbacks are optional so you need only set callbacks for events you're interested 
    // in. However, if you don't handle the device reset/lost callbacks then the sample 
    // framework won't be able to reset your device since the application must first 
    // release all device resources before resetting.  Likewise, if you don't handle the 
    // device created/destroyed callbacks then the sample framework won't be able to 
    // recreate your device resources.

	DXUTSetCallbackD3D9DeviceCreated( GameCodeApp::OnCreateDevice );
	DXUTSetCallbackD3D9DeviceDestroyed( GameCodeApp::OnDestroyDevice );
	DXUTSetCallbackMsgProc( GameCodeApp::MsgProc );
    DXUTSetCallbackD3D9DeviceReset( GameCodeApp::OnResetDevice );
    DXUTSetCallbackD3D9DeviceLost( GameCodeApp::OnLostDevice );
	DXUTSetCallbackD3D9FrameRender( GameCodeApp::OnRender );
	DXUTSetCallbackFrameMove( GameCodeApp::OnUpdateGame );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

	// Perform application initialization
	if (!g_pApp->InitInstance (hInstance, lpCmdLine, hWnd, screenWidth, screenHeight)) 
	{
		return FALSE;
	}

	// Instead of calling Main Loop, we set up everything for the main loop
	DXUTSetUpMainLoop();
	return true;
}


//
// RenderFrame								- Chapter 20, page 754
//
void RenderFrame()
{
	// In TeapotWars, this would be called by GameCode's main loop
	// Since the C# app has its own main loop, we expose this
	// function so that C# app can call from its main loop
	DXUTRender3DEnvironment(); 
}

//
// DXShutdown								- Chapter 20, page 755
//
int DXShutdown()
{
	// Normally this is called after the GameCode main loop ends. We
	// expose this here so that the C# app can shutdown after it finishes
	// its main loop
	DXUTShutdown();

    return g_pApp->GetExitCode();	
}

//
// WndProc									- Chapter 20, page 755
//
void WndProc(int *hWndPtrAddress, int msg, int wParam, int lParam)
{
	HWND hWnd = (HWND)hWndPtrAddress;
	DXUTStaticWndProc( hWnd, msg, WPARAM(wParam), LPARAM(lParam) );
}



//
// IsGameRunning							- Chapter 20, page 770
//    Note: The book incorrectly names this IsRunning    
//
bool IsGameRunning()
{
	EditorGame* game = (EditorGame*)g_pApp->m_pGame;
	if (game)
	{
		bool isRunning = game->IsRunning();
		return isRunning;
	}
	return false;
}


//
// GetActorPos								- Chapter 20, page 768
//
void GetActorPos( int *actorPosPtrAddress, ActorId actorId )
{
	float* ptr = (float*)actorPosPtrAddress;

	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor( actorId  );
	if ( !pActor )
	{
		return;
	}

	// Just to keep things simple, the C# app allocates memory,
	// and we populate that memory with position information.
	// This way, the C# app can free this memory when it's
	// finised.
	Vec3 position = pActor->VGetMat().GetPosition();
	ptr[0]=position.x; 
	ptr[1]=position.y; 
	ptr[2]=position.z; 
}

//
// GetActorType							- Chapter 20, page 768
//
int GetActorType( ActorId actorId )
{
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor( actorId );
	if ( !pActor )
	{
		return 0;
	}

	return pActor->VGetType();
}

//
// GetNumActors							- Chapter 20, page 767
//
int GetNumActors()
{
	EditorGame* pGame = (EditorGame*)g_pApp->m_pGame;
	if ( pGame )
	{
		return pGame->GetNumActors();
	}
	else
	{
		return 0;
	}
}


//
// GetActorList							- Chapter 20, page 767
//
void GetActorList( int *ptr, int numActors )
{
	// To keep things simple, we pass the actor ids to the C# app
	// the C# app iterates through the actor ids, and calls back into
	// the unmanaged dll to get the appropriate information about each
	// actor
	EditorGame* pGame = (EditorGame*)g_pApp->m_pGame;
	if ( pGame )
	{
		ActorMap::iterator itr;
		int actorArrayIndex;
		for ( itr = pGame->GetActorMapBegin(), actorArrayIndex = 0; 
			itr != pGame->GetActorMapEnd() && actorArrayIndex < numActors; itr++, actorArrayIndex++ )
		{
			ActorId actorId = itr->first;
			ptr[actorArrayIndex] = actorId;
		}
	}
}

//
// GetActorColor							- Chapter 20, page 768
//
DWORD GetActorColor( ActorId actorId )
{
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor( actorId );
	if (!pActor)
	{
		return 0;
	}

	shared_ptr<ActorParams> pActorParams = pActor->VGetParams();
	if (!pActorParams)
	{
		return 0;
	}

	return pActorParams->m_Color;

}

//
// PickActor									- Chapter 20, page 769
//
int PickActor(int *hWndPtrAddress)
{
	HWND hWnd = (HWND)hWndPtrAddress;

	CPoint ptCursor;
	GetCursorPos( &ptCursor );
	
	// Convert the screen coordinates of the mouse cursor into
	// coordinates relative to the client window
	ScreenToClient( hWnd, &ptCursor );
	RayCast rayCast(ptCursor);
	EditorGame* pGame = (EditorGame*)g_pApp->m_pGame;

	if (!pGame)
	{
		return INVALID_ID;
	}

	shared_ptr<IGameView> pView = pGame->GetView();
	if (!pView)
	{
		return INVALID_ID;
	}

	shared_ptr<EditorGameView> gameView = boost::static_pointer_cast<EditorGameView>( pView );

	// Cast a ray through the scene. The RayCast object contains an array of Intersection
	// objects.
	gameView->GetScene()->Pick(&rayCast);
	rayCast.Sort();
	
	// If there are any intersections, get information from the first intersection.
	if (rayCast.m_NumIntersections)
	{
		Intersection firstIntersection = rayCast.m_IntersectionArray[0];
		optional<ActorId> maybeID = firstIntersection.m_actorId;
		if (maybeID.valid())
		{
			ActorId id = *maybeID;
			return id;
		}
	}
	return INVALID_ID;
}


//
// SetActorPos									- Chapter 20, page 780
//
void SetActorPos( ActorId actorId, const float x, const float y, const float z )
{
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor( actorId );
	if ( !pActor )
	{
		return;
	}
	Mat4x4 position = pActor->VGetMat();
	position.SetPosition( Vec3(x, y, z) );
	safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Move_Actor( actorId, position) ) );
}


//
// RotateActorXAxis								- Chapter 20, page 780
//
void RotateActorXAxis( ActorId actorId, float radians )
{
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor( actorId );
	if ( !pActor )
	{
		return;
	}

	Mat4x4 initialRotationMatrix = pActor->VGetMat();
	Vec3 position = initialRotationMatrix.GetPosition();
	initialRotationMatrix.SetPosition( Vec3(0.0f, 0.0f, 0.0f) );

	Mat4x4 rotation = Mat4x4::g_Identity;;
	rotation.BuildRotationX( radians );

	initialRotationMatrix *= rotation;
	initialRotationMatrix.SetPosition( position );
	safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Move_Actor( actorId, initialRotationMatrix) ) );
}

void RotateActorYAxis( ActorId actorId, float radians )
{
	shared_ptr<IActor> pActor = g_pApp->m_pGame->VGetActor( actorId );
	if ( !pActor )
	{
		return;
	}
	Mat4x4 initialRotationMatrix = pActor->VGetMat();
	Vec3 position = initialRotationMatrix.GetPosition();
	initialRotationMatrix.SetPosition( Vec3(0.0f, 0.0f, 0.0f) );

	Mat4x4 rotation = Mat4x4::g_Identity;;
	rotation.BuildRotationY( radians );

	initialRotationMatrix *= rotation;
	initialRotationMatrix.SetPosition( position );
	safeQueEvent( IEventDataPtr( GCC_NEW EvtData_Move_Actor( actorId, initialRotationMatrix) ) );
}

//
// RemoveActor								- Chapter 20, page 779
//
void RemoveActor( ActorId actorId )
{
	EditorGame* pGame = (EditorGame*)g_pApp->m_pGame;
	if (pGame)
	{
		const EvtData_Destroy_Actor destroyActor( actorId );
		safeTriggerEvent( destroyActor );
	}
}

//
// CreateActor								- Chapter 20, page 778
//
void CreateActor( ActorType type, DWORD color, float length, float radius, LPCTSTR effectFileName, LPCTSTR meshFileName )
{
	Vec3 startPosition(0.0f, 1.5f, 0.0f);
	switch (type)
	{
	case AT_Teapot:
		{
			TeapotParams tp;
			tp.m_Mat.SetPosition(startPosition);
			tp.m_Length = 2.5;
			tp.m_Color = color;
			const EvtData_Request_New_Actor requestActor( &tp );
			safeTriggerEvent( requestActor );
		}
		break;
	case AT_Grid:
		{
			GridParams grid;
			grid.m_Color = color;
			strcpy_s( grid.m_Texture, GridParams::sk_MaxTextureNameLen, "grid.dds" );
			grid.m_Mat = Mat4x4::g_Identity;
			grid.m_Squares = length;
			grid.m_Mat.SetPosition(startPosition);
			const EvtData_Request_New_Actor gridEvt( &grid );
			safeTriggerEvent( gridEvt );
		}
		break;
	case AT_TestObject:
		{
			TestObjectParams top;
			top.m_Type = AT_TestObject;
			top.m_Color = color;
			top.m_Mat.SetPosition(startPosition);
			const EvtData_Request_New_Actor testObjEvt( &top );
			safeTriggerEvent( testObjEvt );
		}
		break;
	case AT_Sphere:
		{
			SphereParams sp;
			sp.m_Pos = startPosition;
			sp.m_Radius = radius;
			sp.m_Segments = 16;
			sp.m_Color = color;
			const EvtData_Request_New_Actor requestActor( &sp );
			safeTriggerEvent( requestActor );
		}
		break;
	case AT_GenericMeshObject:
		{
			GenericMeshObjectParams gmp;
			gmp.m_Color = color;
			GenericToAnsiCch( gmp.m_FXFileName, effectFileName , GenericMeshObjectParams::sk_MaxFileNameLen);
			GenericToAnsiCch( gmp.m_XFileName, meshFileName , GenericMeshObjectParams::sk_MaxFileNameLen);
			gmp.m_Mat = Mat4x4::g_Identity;
			gmp.m_Mat.SetPosition(startPosition);
			const EvtData_Request_New_Actor gmpEvt( &gmp );
			safeTriggerEvent( gmpEvt );
		}
		break;
	default:
		break;
	}
}

//
// SaveLevel							- Chapter 20, page 790
//
//   TODO: This should return a bool specifying if the level was successfully saved
//
void SaveLevel( char* fileName )
{
	FILE * pFile;
	pFile = fopen (fileName,"w");

	if (!pFile)
	{
		return;
	}

	EditorGame* pGame = (EditorGame*)g_pApp->m_pGame;
	if (!pGame)
	{
		return;
	}
	ActorMap::iterator itr;
	int actorArrayIndex;

	// To save out our level, we want to iterate through all the actors
	// and then write out information that will enable the editor to
	// recreate and reposition each actor when we want to reload this level
	for ( itr = pGame->GetActorMapBegin(), actorArrayIndex = 0; 
		itr != pGame->GetActorMapEnd() && actorArrayIndex < pGame->GetNumActors(); itr++, actorArrayIndex++ )
	{
		shared_ptr<IActor> pActor = itr->second;
		if (!pActor)
		{
			continue;
		}
		shared_ptr<ActorParams> pActorParams = pActor->VGetParams();
		if (!pActorParams)
		{
			continue;
		}

		ActorId actorId;
		optional<ActorId> maybeID = pActor->VGetID();
		if (maybeID.valid())
		{
			actorId = *maybeID;
		}
		else
		{
			continue;
		}

		switch (pActor->VGetType())
		{
		case AT_Sphere:
			{
				// For the sphere, the relevant params we want to save are its
				// radius and segments. We don't care about its transform matrix
				// since, as a sphere, the only thing that really matters is its
				// position.
				WriteBeginningParamsBlock(pFile, "sphere", actorId);
				WritePosParams(pFile, pActor->VGetMat().GetPosition());
				shared_ptr<SphereParams> sphereParams = boost::static_pointer_cast<SphereParams>( pActorParams );
				fprintf(pFile, "   Radius = %f,\n", sphereParams->m_Radius);
				fprintf(pFile, "   Segments = %i,\n", sphereParams->m_Segments);
				WriteColorParams(pFile, sphereParams->m_Color);
				WriteEndParamsBlock(pFile, "sphere", actorId);
			}
			break;
		case AT_Teapot:
			{
				// We want to write out the matrix params for the teapot, since we're
				// interested in saving its position and rotation.
				WriteBeginningParamsBlock(pFile, "teapot", actorId);
				shared_ptr<TeapotParams> teapotParams = boost::static_pointer_cast<TeapotParams>( pActorParams );
				WriteColorParams(pFile, teapotParams->m_Color);
				WriteMatrixParams(pFile, pActor->VGetMat());
				WriteEndParamsBlock(pFile, "teapot", actorId);
			}
			break;

		case AT_TestObject:
			{
				// We want to write out the matrix params for the Test Object, since we're
				// interested in saving its position and rotation.
				WriteBeginningParamsBlock(pFile, "testObject", actorId);
				shared_ptr<TestObjectParams> testObjectParams = boost::static_pointer_cast<TestObjectParams>( pActorParams );
				WriteColorParams(pFile, testObjectParams->m_Color);
				WriteMatrixParams(pFile, pActor->VGetMat());
				WriteEndParamsBlock(pFile, "testObject", actorId);
			}
			break;
		case AT_Grid:
			{
				// The grid contains some extra information, like its texture and
				// number of squares.
				WriteBeginningParamsBlock(pFile, "grid", actorId);
				shared_ptr<GridParams> gridParams = boost::static_pointer_cast<GridParams>( pActorParams );
				WriteColorParams(pFile, gridParams->m_Color);
				WriteMatrixParams(pFile, pActor->VGetMat());
				fprintf(pFile, "   Texture = \"%s\",\n", gridParams->m_Texture);
				fprintf(pFile, "   Squares = %i,\n", gridParams->m_Squares);
				WriteEndParamsBlock(pFile, "grid", actorId);
			}
			break;
		case AT_GenericMeshObject:
			{
				WriteBeginningParamsBlock(pFile, "genericMeshObject", actorId);
				shared_ptr<GenericMeshObjectParams> genericMeshObjectParams = boost::static_pointer_cast<GenericMeshObjectParams>( pActorParams );
				WriteColorParams(pFile, genericMeshObjectParams->m_Color);
				WriteMatrixParams(pFile, pActor->VGetMat());
				std::string meshFileName(genericMeshObjectParams->m_XFileName);
				std::string effectFileName(genericMeshObjectParams->m_FXFileName);

				ReplaceAllCharacters(meshFileName, "\\", "\\\\");
				ReplaceAllCharacters(effectFileName, "\\", "\\\\");

				fprintf(pFile, "   XFile = \"%s\",\n", meshFileName.c_str());
				fprintf(pFile, "   FXFile = \"%s\",\n", effectFileName.c_str());
				WriteEndParamsBlock(pFile, "genericMeshObject", actorId);
			}
			break;
		default:
			break;
		};
	}
   fclose (pFile);
}

void WriteBeginningParamsBlock(FILE* pFile, std::string paramType, ActorId actorId)
{
	// Each param block in a lua file starts the same way. The only difference
	// is in the type of actor, and their id - which we add to make sure each
	// param block is unique.
	fprintf(pFile, "local %sParams%i=\n", paramType.c_str(), actorId);
	fprintf(pFile, "{\n");
	fprintf(pFile, "   ActorType = \"%s\",\n", paramType.c_str());
}

void WriteEndParamsBlock(FILE* pFile, std::string paramType, ActorId actorId)
{
	// Each param block in a lua file ends the same way. We make sure to
	// get the actor id and the actor type, so we can add a final line at the
	// end of the param block that will cause the Lua Manager to trigger
	// a new event when this file is reloaded.
	fprintf(pFile, "}\n");
	fprintf(pFile, "EventManager:TriggerEvent( \"request_new_actor\", %sParams%i )\n", paramType.c_str(), actorId);
}

void WriteMatrixParams( FILE* pFile, const Mat4x4 &mat )
{
	fprintf(pFile, "   Mat =\n");
	fprintf(pFile, "   {\n");
	for( int i = 0; i < 4; i++ )
	{
		for ( int j = 0; j < 4; j++ )
		{
			fprintf(pFile, "      _%i%i = %f,\n", i+1, j+1, mat.m[i][j]);
		}
	}
	fprintf(pFile, "   },\n");
}

void WriteColorParams( FILE* pFile, const Color &color )
{
	fprintf(pFile, "   Color =\n");
	fprintf(pFile, "   {\n");
	fprintf(pFile, "      R = %f,\n", color.r);
	fprintf(pFile, "      G = %f,\n", color.g);
	fprintf(pFile, "      B = %f,\n", color.b);
	fprintf(pFile, "      A = %f,\n", color.a);
	fprintf(pFile, "   },\n");
}

void WritePosParams( FILE* pFile, const Vec3 &pos )
{
	fprintf(pFile, "   Pos =\n");
	fprintf(pFile, "   {\n");
	fprintf(pFile, "      %f,\n", pos.x);
	fprintf(pFile, "      %f,\n", pos.y);
	fprintf(pFile, "      %f,\n", pos.z);
	fprintf(pFile, "   },\n");
}

//
// OpenLevel							- Chapter 20, page 787
//
//   TODO: This should return a bool specifying if the level was successfully opened.
//
void OpenLevel( char* fileName )
{
	EditorGame* pGame = (EditorGame*)g_pApp->m_pGame;
	if (pGame)
	{
		pGame->OpenLevelFile(fileName);
	}
}

void ReplaceAllCharacters( std::string& origString, std::string subString, std::string newSubString )
{
	size_t subStringPos = origString.find(subString, 0);
	while (subStringPos != std::string.npos)
	{
		origString.replace( subStringPos, subString.length(), newSubString );
		subStringPos += newSubString.length();
		subStringPos = origString.find(subString, subStringPos);
	}
}