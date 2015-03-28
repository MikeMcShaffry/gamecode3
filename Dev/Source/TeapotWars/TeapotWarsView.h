#pragma once
//========================================================================
// TeapotWarsView.h : source file for the sample game
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
//  class TeapotWarsScene					- not in the book, line for line anyway
//  class TeapotWarsGameView				- Chapter 19, page 724
//  class TeapotWarsGameViewListener		- Chapter 19, page 731
//  class AITeapotView						- Chapter 19, page 737
//  class AITeapotViewListener				- Chapter 19, page 738
//


#include "SceneGraph\SceneNodes.h"
#include "EventManager\EventManager.h"




class MainMenuUI : public CBaseUI
{
protected:
	CDXUTDialog m_SampleUI;					// dialog for sample specific controls
	void Set();
	bool m_bCreatingGame;
	int m_NumAIs;
	int m_NumPlayers;
	std::string m_HostName;
	std::string m_HostListenPort;
	std::string m_ClientAttachPort;

public:
	MainMenuUI();
	virtual ~MainMenuUI();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 1; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
	void CALLBACK _OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
};


class StandardHUD : public CBaseUI
{
protected:
	CDXUTDialog m_HUD;                  // dialog for standard controls

public:
	StandardHUD();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 1; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
};

class IEventManager;
class SoundProcess;
class TeapotController;

class MainMenuView : public HumanView
{
protected:
	shared_ptr<MainMenuUI> m_MainMenuUI; 
public:

	MainMenuView(); 
	~MainMenuView(); 
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	virtual void VRenderText(CDXUTTextHelper &txtHelper);	
	virtual void VOnUpdate(int deltaMilliseconds);
};





class TeapotWarsGameView : public HumanView
{
	friend class TeapotWarsGameViewListener;

protected:
	bool  m_bShowUI;					// If true, it renders the UI control text
	BaseGameState m_BaseGameState;	// what is the current game state

	shared_ptr<ScreenElementScene> m_pScene;
	shared_ptr<SoundProcess> m_music;
	shared_ptr<TeapotController> m_pTeapotController;
	shared_ptr<MovementController> m_pFreeCameraController;
	shared_ptr<CameraNode> m_pCamera;
	shared_ptr<SceneNode> m_pTeapot;
	shared_ptr<StandardHUD> m_StandardHUD; 

	void BuildInitialScene();

	void MoveActor(ActorId id, Mat4x4 const &mat);
	void HandleGameState(BaseGameState newState);
public:

	TeapotWarsGameView(); 
	~TeapotWarsGameView();

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );	
	virtual void VRenderText(CDXUTTextHelper &txtHelper);	
	virtual void VOnUpdate(int deltaMilliseconds);
	virtual void VOnAttach(GameViewId vid, optional<ActorId> aid);

	virtual void VSetCameraOffset(const Vec4 & camOffset );
};


class TeapotWarsGameViewListener : public IEventListener
{
	TeapotWarsGameView *m_pView;
public:
	explicit TeapotWarsGameViewListener( TeapotWarsGameView *view);
	virtual char const * GetName(void) { return "TeapotWarsGameViewListener"; }
	virtual bool HandleEvent( IEventData const & event );
};


class AITeapotView : public IGameView 
{
	friend class AITeapotViewListener;

private:
	shared_ptr<PathingGraph> m_pPathingGraph;
	StateMachine* m_pStateMachine;

protected:
	GameViewId	m_ViewId;
	optional<ActorId> m_PlayerActorId;

public:
	AITeapotView(shared_ptr<PathingGraph> pPathingGraph);
	virtual ~AITeapotView();

	virtual HRESULT VOnRestore() { return S_OK; }
	virtual void VOnRender(double fTime, float fElapsedTime) {}
	virtual void VOnLostDevice() {}
	virtual GameViewType VGetType() { return GameView_AI; }
	virtual GameViewId VGetId() const { return m_ViewId; }
	virtual void VOnAttach(GameViewId vid, optional<ActorId> aid) { m_ViewId = vid; m_PlayerActorId = aid; }
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg ) {	return 0; }
	virtual void VOnUpdate( int deltaMilliseconds ) {}
	
	shared_ptr<PathingGraph> GetPathingGraph(void) const { return m_pPathingGraph; }
	void RotateActorY(float angleRadians);
};


class AITeapotViewListener : public IEventListener
{
	AITeapotView *const m_pView;
public:
	explicit AITeapotViewListener(AITeapotView *pView) : IEventListener(), m_pView(pView) {}
	virtual char const* GetName(void) { return "AITeapotViewListener"; }
	virtual bool HandleEvent(IEventData const& event);
};

