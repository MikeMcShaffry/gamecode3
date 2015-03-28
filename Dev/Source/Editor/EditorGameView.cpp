//========================================================================
// EditorGameView.cpp
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


#include "GameCodeStd.h"

#include "GameCode.h"
#include "Actors.h"
#include "Audio\CAudio.h"
#include "Audio\CSoundProcess.h"
#include "SceneGraph\SceneNodes.h"
#include "SceneGraph\Geometry.h"
#include "Graphics2D\Sprite.h"
#include "Lang\Strings.h"

#include "EditorGameView.h"
//#include "EditorEvents.h"
//#include "EditorController.h"

#include "EventManager\Events.h"
#include "EventManager\EventManagerImpl.h"

#include "Physics\PhysicsEventListener.h"
#include "Editor.h"

//#define A2T(dest,src) \
//  #ifdef UNICODE  \
//    wcstombs (dest, src, 1 + wcslen (src)); \
//  #else  \
//    strcpy (dest, src);  \
//  #endif

extern Editor globalApp;


//
// ListenForEditorEvents				- Chapter 20, page 748
//
void ListenForEditorEvents(EventListenerPtr listener)
{
	// Note that the only events we really care about are related
	// to when the app is starting up, and events related to 
	// creating, destroying, or moving actors in the scene
	safeAddListener( listener, EvtData_Destroy_Actor::sk_EventType );
	safeAddListener( listener, EvtData_New_Game::sk_EventType );
	safeAddListener( listener, EvtData_New_Actor::sk_EventType );
	safeAddListener( listener, EvtData_Move_Actor::sk_EventType );
}

//========================================================================
//
// EditorGameView implementation
//
//========================================================================


//
// EditorGameView::EditorGameView				- Chapter 20, page 749
//
EditorGameView::EditorGameView() 
{
	// The EditorGameView differs from its counterpart in TeapotWars primarily
	// in the fact that we don't have any UI classes. Any level information
	// should be displayed in the C# app.

	m_pScene.reset(GCC_NEW ScreenElementScene());

	Frustum frustum;
	frustum.Init(D3DX_PI/4.0f, 1.0f, 1.0f, 100.0f);
	m_pCamera.reset(GCC_NEW CameraNode(&Mat4x4::g_Identity, frustum));
	assert(m_pScene && m_pCamera && _T("Out of memory"));

	m_pScene->VAddChild(optional_empty(), m_pCamera);
	m_pScene->SetCamera(m_pCamera);

	EventListenerPtr listener ( GCC_NEW EditorGameViewListener( this ) );
	ListenForEditorEvents(listener);
}

void 
EditorGameView::VOnUpdate( int deltaMilliseconds )
{
	// Much like TeapotWarsView::VOnUpdate, except
	// we only have one controller in the editor

	HumanView::VOnUpdate( deltaMilliseconds );

	if (m_pFreeCameraController)
	{
		m_pFreeCameraController->OnUpdate(deltaMilliseconds);
	}
}

/********
shared_ptr<SceneNode> 
EditorGameView::CreateGrid(const GridParams &p)
{
	shared_ptr<SceneNode> grid(GCC_NEW Grid(p.m_Id, "Grid", p));
	m_pScene->VAddChild(p.m_Id, grid);
	grid->VOnRestore(&*m_pScene);
	return grid;
}

shared_ptr<SceneNode> 
EditorGameView::CreateGenericMeshObject(const GenericMeshObjectParams &p)
{
	size_t origsize = strlen(p.m_XFileName) + 1;
	const size_t newsize = GenericMeshObjectParams::sk_MaxFileNameLen ;
    size_t convertedChars = 0;
    wchar_t meshFileName[newsize];
    mbstowcs_s(&convertedChars, meshFileName, origsize, p.m_XFileName, _TRUNCATE);
	
	origsize = strlen(p.m_FXFileName) + 1;
	wchar_t effectFileName[newsize];
	mbstowcs_s(&convertedChars, effectFileName, origsize, p.m_FXFileName, _TRUNCATE);

	shared_ptr<SceneNode> meshFile(GCC_NEW ShaderMeshNode(p.m_Id, "MeshFile", meshFileName, effectFileName, RenderPass_Actor, &p.m_Mat, p.m_Color ));
	m_pScene->VAddChild(p.m_Id, meshFile);
	meshFile->VOnRestore(&*m_pScene);
	return meshFile;

	shared_ptr<SceneNode> nul;
	return nul;
}

shared_ptr<SceneNode> 
EditorGameView::CreateSphere(const SphereParams &p)
{
	ID3DXMesh *sphereMesh;
	if ( SUCCEEDED( 
		D3DXCreateSphere( 
		DXUTGetD3D9Device(), p.m_Radius, p.m_Segments, p.m_Segments, &sphereMesh, NULL) ) )
	{
		Mat4x4 trans;
		trans.BuildTranslation(p.m_Pos);
		shared_ptr<SceneNode> sphere(GCC_NEW ShaderMeshNode(p.m_Id, "Sphere", sphereMesh, m_pScene->GetEffect(L"GameCode3.fx"), RenderPass_Actor, &trans, p.m_Color));
		m_pScene->VAddChild(p.m_Id, sphere);
		SAFE_RELEASE(sphereMesh);
		return sphere;
	}

	shared_ptr<SceneNode> nul;
	return nul;
}

shared_ptr<SceneNode> 
EditorGameView::CreateTeapot(const TeapotParams &p)
{
	ID3DXMesh *teapotMesh;
    if( SUCCEEDED( D3DXCreateTeapot( DXUTGetD3D9Device(), &teapotMesh, NULL ) ) )
	{
		shared_ptr<SceneNode> teapot(GCC_NEW ShaderMeshNode(p.m_Id, "Teapot", teapotMesh, m_pScene->GetEffect(L"GameCode3.fx"), RenderPass_Actor, &p.m_Mat, p.m_Color));
		m_pScene->VAddChild(p.m_Id, teapot);
		SAFE_RELEASE(teapotMesh);
		return teapot;
	}

	shared_ptr<SceneNode> nul;
	return nul;
}

shared_ptr<SceneNode> 
EditorGameView::CreateAiTeapot(const AiTeapotParams &p)
{
	ID3DXMesh *teapotMesh;
	if( SUCCEEDED( D3DXCreateTeapot( DXUTGetD3D9Device(), &teapotMesh, NULL ) ) )
	{
		shared_ptr<SceneNode> teapot(GCC_NEW ShaderMeshNode(p.m_Id, "AiTeapot", teapotMesh, m_pScene->GetEffect(L"GameCode3.fx"), RenderPass_Actor, &p.m_Mat, p.m_Color));
		m_pScene->VAddChild(p.m_Id, teapot);
		SAFE_RELEASE(teapotMesh);
		return teapot;
	}

	shared_ptr<SceneNode> nul;
	return nul;
}

shared_ptr<SceneNode> 
EditorGameView::CreateTestObject(TestObjectParams const &p)
{
	shared_ptr<SceneNode> testObject(GCC_NEW TestObject(p, "TestObject"));
	m_pScene->VAddChild(p.m_Id, testObject);
	testObject->VOnRestore(&*m_pScene);
	return testObject;
}

**********/


void EditorGameView::MoveActor(ActorId id, Mat4x4 const &mat)
{
    shared_ptr<ISceneNode> node = m_pScene->FindActor(id);
	if (node)
	{
		node->VSetTransform(&mat);
	}
}


//
// EditorGameView::BuildInitialScene				- Chapter 20, page 749
//
void EditorGameView::BuildInitialScene()
{
	// EditorGameView::BuildInitialScene differs from the TeapotWarsView
	// version of this function in that we don't have a sound manager
	// or HUD.

	// Here's our sky node
	// This would be good to put into a file loaded into the editor,
	// rather than hardcoded. Another homework assignment!
	shared_ptr<SkyNode> sky(GCC_NEW SkyNode("Sky2", m_pCamera));
	m_pScene->VAddChild(optional_empty(), sky);

	VPushElement(m_pScene);

	// We also make sure that the MovementController is hooked up
	// to the keyboard and mouse handlers, since this is our primary method
	// for moving the camera around.
	m_pFreeCameraController.reset(GCC_NEW MovementController(m_pCamera, 90, 0, true));
	m_KeyboardHandler = m_pFreeCameraController;
	m_MouseHandler = m_pFreeCameraController;
	m_pCamera->ClearTarget();

	VOnRestore();
}

//========================================================================
//
// EditorGameView implementation
//
//========================================================================

EditorGameViewListener::EditorGameViewListener( EditorGameView *view)
{
	m_pView = view;
}

//
// EditorGameViewListener::HandleEvent				- Chapter 20, page 750
//
bool EditorGameViewListener::HandleEvent( IEventData const & event )
{
	// As mentioned earlier, the only events we really care about here
	// are changes in game state, and events related to changes in the actors
	// in the scene

	if ( EvtData_Destroy_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Destroy_Actor & castEvent = static_cast< const EvtData_Destroy_Actor & >( event );
		ActorId aid = castEvent.m_id;
		m_pView->m_pScene->RemoveChild(aid);
	}
	else if ( EvtData_New_Game::sk_EventType == event.VGetEventType() )
	{
		m_pView->BuildInitialScene();
	}
	else if ( EvtData_Move_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_Move_Actor & ed = static_cast< const EvtData_Move_Actor & >( event );
		m_pView->MoveActor(ed.m_Id, ed.m_Mat);
	}
	else if ( EvtData_New_Actor::sk_EventType == event.VGetEventType() )
	{
		const EvtData_New_Actor & ed = static_cast< const EvtData_New_Actor & >( event );
		shared_ptr<SceneNode> node = ed.m_pActorParams->VCreateSceneNode(m_pView->m_pScene);
		m_pView->m_pScene->VAddChild(ed.m_pActorParams->m_Id, node);
		node->VOnRestore(&(*(m_pView->m_pScene)));
	}
	return false;
}
