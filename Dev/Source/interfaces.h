#pragma once
//========================================================================
// Interfaces.h : Defines interface classes defined throughout the book
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
// class IScreenElement			- Chapter 9, page 48
// class ISceneNode				- Chapter 14, page 469
// class IActor					- Chapter 19, page 682
// class IGame					- not in the book, line for line anyway
// class IGameView				- not in the book, line for line anyway
// class IKeyboardHandler		- Chapter 8, page 208
// class IMouseHandler			- Chapter 8, page 208
// class IJoystickHandler		- Chapter 8, page 208
// class IGamepadHandler		- Chapter 8, page 208
// class IResourceFile			- Chapter 7, page 192
// class IGamePhysics			- Chapter 15, page 543
//
//========================================================================


template<class T>
struct SortBy_SharedPtr_Content
{
bool operator()(const shared_ptr<T> &lhs, const shared_ptr<T> &rhs) const
{ return *lhs < *rhs; }
};


class IScreenElement
{
public:
	virtual HRESULT VOnRestore() = 0;
	virtual HRESULT VOnRender(double fTime, float fElapsedTime) = 0;
	virtual void VOnUpdate(int deltaMilliseconds) = 0;

	virtual int VGetZOrder() const = 0;
	virtual void VSetZOrder(int const zOrder) = 0;
	virtual bool VIsVisible() const = 0;
	virtual void VSetVisible(bool visible) = 0;

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg )=0;

	virtual ~IScreenElement() { };
	virtual bool const operator <(IScreenElement const &other) { return VGetZOrder() < other.VGetZOrder(); }
};

typedef unsigned int ActorId;
struct ActorParams;

class IActor
{
	friend class IGame;

public:
	// Yuk-these shouldn't be public
	virtual void VSetMat(const Mat4x4 &newMat)=0;
	virtual void VSetID(ActorId id)=0;
public:
	virtual ~IActor() { }
	virtual Mat4x4 const &VGetMat()=0;
	virtual const int VGetType()=0;
	virtual ActorId VGetID()=0; 
	virtual shared_ptr<ActorParams> VGetParams()=0;
	virtual bool VIsPhysical()=0;
	virtual bool VIsGeometrical()=0;
	virtual void VOnUpdate(int deltaMilliseconds)=0;
	virtual void VRotateY(float angleRadians) = 0;
};


class IGamePhysics;

class IGameLogic
{
public:
	virtual shared_ptr<IActor> VGetActor(const ActorId id)=0;
	virtual void VAddActor(shared_ptr<IActor> actor, struct ActorParams *p)=0;
	virtual void VRemoveActor(ActorId id)=0;
	virtual bool VLoadGame(std::string gameName)=0;
	virtual void VSetProxy()=0;				
	virtual void VOnUpdate(float time, float elapsedTime)=0;
	virtual void VChangeState(enum BaseGameState newState)=0;
	virtual void VMoveActor(const ActorId id, Mat4x4 const &mat)=0;
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) = 0;

	// 
	virtual void VBuildInitialScene()=0;
};


enum GameViewType
{
	GameView_Human,
	GameView_Remote,
	GameView_AI,
	GameView_Recorder,
	GameView_Other
};

typedef unsigned int GameViewId;

class IGameView 
{
public:
	virtual HRESULT VOnRestore()=0;
	virtual void VOnRender(double fTime, float fElapsedTime)=0;
	virtual void VOnLostDevice()=0;
	virtual GameViewType VGetType()=0;
	virtual GameViewId VGetId() const=0;
	virtual void VOnAttach(GameViewId vid, optional<ActorId> aid)=0;

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg )=0;
	virtual void VOnUpdate( int deltaMilliseconds )=0;

	virtual ~IGameView() { };
};



typedef std::list<shared_ptr<IScreenElement> > ScreenElementList;
typedef std::list<shared_ptr<IGameView> > GameViewList;


////////////////////////////////////////////////////
//
// IKeyboardHandler Description		- Chapter 8, page 208
// IMouseHandler Description		- Chapter 8, page 208
// IJoystickHandler Description		- Chapter 8, page 208
// IGamepadHandler Description		- Chapter 8, page 208
//
// These are the public APIs for any object that implements reactions
// to events sent by hardware user interface devices.
//
// Note: IJoystickHandler and IGamepadHandler are not currently
//       implemented anywhere in the codebase. Theu are here
//       as examples, and could require modification to actually work!
//
////////////////////////////////////////////////////

class IKeyboardHandler
{
public:
	virtual bool VOnKeyDown(const BYTE c)=0;
	virtual bool VOnKeyUp(const BYTE c)=0;
};

class IMouseHandler
{
public:
	virtual bool VOnMouseMove(const CPoint &mousePos) = 0;
	virtual bool VOnLButtonDown(const CPoint &mousePos) = 0;
	virtual bool VOnLButtonUp(const CPoint &mousePos) = 0;
	virtual bool VOnRButtonDown(const CPoint &mousePos) = 0;
	virtual bool VOnRButtonUp(const CPoint &mousePos) = 0;
};

class IJoystickHandler
{
	virtual bool VOnButtonDown(int const button, int const pressure)=0;
	virtual bool VOnButtonUp(int const button)=0;
	virtual bool VOnJoystick(float const x, float const y)=0;
};

class IGamepadHandler
{
	virtual bool VOnTrigger(bool const left, float const pressure)=0;
	virtual bool VOnButtonDown(int const button, int const pressure)=0;
	virtual bool VOnButtonUp(int const button)=0;
	virtual bool VOnDirectionalPad(int directionFlags)=0;
	virtual bool VOnThumbstick(int const stickNum, float const x, float const y)=0;
};

////////////////////////////////////////////////////
//
// IResourceFile Description
// The core of a resource cache system
// 
////////////////////////////////////////////////////

class Resource;
class IResourceFile
{
public:
	virtual bool VOpen()=0;
	virtual int VGetResourceSize(const Resource &r)=0;
	virtual int VGetResource(const Resource &r, char *buffer)=0;
	virtual ~IResourceFile() { }
};




/////////////////////////////////////////////////////////////////////////////
// RenderPass Description
//
//   3D scenes are drawn in passes - this enum defines the render passes
//   supported by the 3D scene graph created by class Scene.
/////////////////////////////////////////////////////////////////////////////

enum RenderPass
{
	RenderPass_0,
	RenderPass_Static = RenderPass_0,
	RenderPass_Actor,
	RenderPass_Sky,
	RenderPass_Last
};



/////////////////////////////////////////////////////////////////////////////
// ISceneNode Description
//
//   This is the public interface for nodes in a 3D scene graph.
/////////////////////////////////////////////////////////////////////////////

class Scene;
class SceneNodeProperties;
class RayCast;

class ISceneNode
{
protected:

public:
	virtual const SceneNodeProperties * const VGet() const=0;

	virtual void VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld=NULL)=0;

	virtual HRESULT VPreRender(Scene *pScene)=0;
	virtual HRESULT VRender(Scene *pScene)=0;
	virtual HRESULT VPostRender(Scene *pScene)=0;

	virtual bool VAddChild(shared_ptr<ISceneNode> kid)=0;
	virtual bool VRemoveChild(ActorId id)=0;
	virtual HRESULT VRenderChildren(Scene *pScene)=0;
	virtual HRESULT VOnRestore(Scene *pScene)=0;
	virtual bool VIsVisible(Scene *pScene) const=0;
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast)=0;

	virtual HRESULT VOnUpdate(Scene *, DWORD const elapsedMs)=0;

	virtual ~ISceneNode() { };
};



/////////////////////////////////////////////////////////////////////////////
// IGamePhysics Description
//
//   The interface defintion for a generic physics API.
/////////////////////////////////////////////////////////////////////////////

class IGamePhysics
{
public:

	// Initialiazation and Maintenance of the Physics World
	virtual bool VInitialize()=0;
	virtual void VSyncVisibleScene() = 0;
	virtual void VOnUpdate( float deltaSeconds ) = 0; 

	// Initialization of Physics Objects
	virtual void VAddSphere(float radius, IActor *actor, float specificGravity, enum PhysicsMaterial mat)=0;
	virtual void VAddBox(const Vec3& dimensions, IActor *gameActor, float specificGravity, enum PhysicsMaterial mat) = 0;
	virtual void VAddPointCloud(Vec3 *verts, int numPoints, IActor *gameActor, float specificGravity, enum PhysicsMaterial mat)=0;
	virtual void VRemoveActor(ActorId id)=0;

	// Debugging
	virtual void VRenderDiagnostics() = 0;

	// Physics world modifiers
	virtual void VCreateTrigger(const Vec3 &pos, const float dim, int triggerID)=0;
	virtual void VApplyForce(const Vec3 &dir, float newtons, ActorId aid)=0;
	virtual void VApplyTorque(const Vec3 &dir, float newtons, ActorId aid)=0;
	virtual bool VKinematicMove(const Mat4x4 &mat, ActorId aid)=0;
	
	// Physics actor states
	virtual void VRotateY(ActorId actorId, float angleRadians, float time) = 0;
	virtual float VGetOrientationY(ActorId actorId) = 0;
	virtual void VStopActor(ActorId actorId) = 0;
	virtual void VSetVelocity(ActorId actorId, const Vec3& vel) = 0;
	virtual void VTranslate(ActorId actorId, const Vec3& vec) = 0;

	virtual ~IGamePhysics() { };
};

