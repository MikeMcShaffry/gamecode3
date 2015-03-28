#pragma once
//========================================================================
// File: SceneNodes.h
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
//  class Grid					- Chapter 13, page 448
//  class SceneNodeProperties	- Chapter 14, page 471
//  enum RenderPass				- Chapter 14, page 473
//  class Material				- Chapter 13, page 453
//  class SceneNode				- Chapter 14, page 473
//  struct AlphaSceneNode		- Chapter 14, page 479
//  class Scene					- Chapter 14, page 481
//  class RootNode				- Chapter 14, page 489
//  class CameraNode			- Chapter 14, page 491
//  class TestObject			- Chapter 14, page 494
//  class MeshNode				- Chapter 14, page 504
//  class SkyNode				- Chapter 14, page 498
//  class ShaderMeshNode		- Chapter 14, page 516

#include "Geometry.h"


// Forward declarations
class SceneNode;
class Scene;
class RayCast;
class MovementController;



//////////////////////////////////////////////////////////////
//
// Material Description
//		See Chapter 13, page 471 for a discussion on Materials
//////////////////////////////////////////////////////////////

class Material
{
	D3DMATERIAL9 m_D3DMaterial;
public:
	Material();
	void SetAmbient(const Color &color);
	const Color GetAmbient() { return m_D3DMaterial.Ambient; }

	void SetDiffuse(const Color &color);
	const Color GetDiffuse() { return m_D3DMaterial.Diffuse; }

	void SetSpecular(const Color &color, const float power);
	void GetSpecular(Color &_color, float &_power) 
		{ _color = m_D3DMaterial.Specular; _power = m_D3DMaterial.Power; }

	void SetEmissive(const Color &color);
	const Color GetEmissive() { return m_D3DMaterial.Emissive; }

	void Set(const Color &color);
	void SetAlpha(const float alpha);
	bool HasAlpha() const { return GetAlpha() != fOPAQUE; }
	float GetAlpha() const { return m_D3DMaterial.Diffuse.a; }
	void Use();
};


class Light
{
    D3DCOLORVALUE   m_diffuse;         /* Diffuse color of light */
    D3DCOLORVALUE   m_specular;        /* Specular color of light */
    D3DCOLORVALUE   m_ambient;         /* Ambient color of light */
    D3DVECTOR       m_position;         /* Position in world space */
    D3DVECTOR       m_direction;        /* Direction in world space */
public:
	Light();	

	void SetAmbient(const Color &color) { m_ambient = color; }
	const Color GetAmbient() { return m_ambient; }

	void SetDiffuse(const Color &color) { m_diffuse = color; }
	const Color GetDiffuse() { return m_diffuse; }

	void SetSpecular(const Color &color) { m_specular = color; }
	const Color GetSpecular() { return m_specular; }
};


////////////////////////////////////////////////////
//
// AlphaType					- Chapter 13, page 455
//
//   This enum defines the different types of alpha blending
//   types that can be set on a scene node.
//
//	
////////////////////////////////////////////////////

enum AlphaType
{
	AlphaOpaque,
	AlphaTexture,
	AlphaMaterial,
	AlphaVertex
};


//////////////////////////////////////////////////////////////////////
// SceneNodeProperties Description
//
//   This class is contained in the SceneNode class, and gains
//   easy access to common scene node properties such as its ActorId
//   or render pass, with a single ISceneNode::VGet() method.
//
//////////////////////////////////////////////////////////////////////

class SceneNodeProperties
{
	friend class SceneNode;

protected:
	optional<ActorId>		m_ActorId;
	std::string				m_Name;
	Mat4x4					m_ToWorld, m_FromWorld;
	float					m_Radius;
	RenderPass				m_RenderPass;
	Material				m_Material;
	AlphaType				m_AlphaType;

	void SetAlpha(const float alpha) 
		{ m_AlphaType=AlphaMaterial; m_Material.SetAlpha(alpha); } 

public:
	optional<ActorId> const &ActorId() const { return m_ActorId; }
	Mat4x4 const &ToWorld() const { return m_ToWorld; }
	Mat4x4 const &FromWorld() const { return m_FromWorld; }
	void Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const;

	const char * Name() const { return m_Name.c_str(); }		

	bool HasAlpha() const { return m_Material.HasAlpha(); }						
	float Alpha() const { return m_Material.GetAlpha(); }
	AlphaType AlphaType() const { return m_AlphaType; }

	RenderPass RenderPass() const { return m_RenderPass; }
	float Radius() const { return m_Radius; }

	Material GetMaterial() const { return m_Material; }
};

//////////////////////////////////////////////////////////////
//
// SceneNodeList Description
//
//   Every scene node has a list of its children - this 
//   is implemented as a vector since it is expected that
//   we won't add/delete nodes very often, and we'll want 
//   fast random access to each child.
//
//////////////////////////////////////////////////////////////

typedef std::vector<shared_ptr<ISceneNode> > SceneNodeList;


//////////////////////////////////////////////////////////////
//
// SceneNode Description
//
//   Implements ISceneNode. Forms the base class for any node
//   that can exist in the 3D scene graph managed by class Scene.
//
//////////////////////////////////////////////////////////////

class SceneNode : public ISceneNode
{
	friend class Scene;

protected:
	SceneNodeList			m_Children;
	SceneNode				*m_pParent;
	SceneNodeProperties		m_Props;

public:
	SceneNode(optional<ActorId> actorId, std::string name, RenderPass renderPass, const Mat4x4 *to, const Mat4x4 *from=NULL) 
	{ 
		m_pParent= NULL;
		m_Props.m_ActorId = actorId;
		m_Props.m_Name = name;
		m_Props.m_RenderPass = renderPass;
		m_Props.m_AlphaType = AlphaOpaque;
		VSetTransform(to, from); 
		SetRadius(0);
	}

	virtual ~SceneNode();

	virtual const SceneNodeProperties * const VGet() const { return &m_Props; }

	virtual HRESULT VPreRender(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPostRender(Scene *pScene);

	virtual void VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld=NULL);

	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual bool VRemoveChild(ActorId id);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const;
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	virtual HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);

	virtual void SetAlpha(float alpha) { return m_Props.SetAlpha(alpha); }

	Vec3 GetPosition() const { return m_Props.m_ToWorld.GetPosition(); }
	void SetPosition(const Vec3 &pos) { m_Props.m_ToWorld.SetPosition(pos); }

	void SetRadius(const float radius) { m_Props.m_Radius = radius; }
	void SetMaterial(const Material &mat) { m_Props.m_Material = mat; }

	// Most scene nodes will not have actor params, so they return a NULL pointer.
	virtual ActorParams * VGetActorParams( void )
	{
		return NULL;
	}
};


////////////////////////////////////////////////////
//
// EffectManager Description
//
//			- Chapter 14, page 515
////////////////////////////////////////////////////


typedef std::map<std::wstring, ID3DXEffect *> EffectNameMap;
class EffectManager 
{
protected:
	EffectNameMap	m_EffectMap;
public:
	~EffectManager();
	ID3DXEffect *Get(std::wstring);
	void OnLostDevice();
};



////////////////////////////////////////////////////
//
// AlphaSceneNode Description
// AlphaSceneNodes Description
//
// A list of scene nodes that need to be drawn in the alpha pass;
// the list is defined as an STL list
////////////////////////////////////////////////////

struct AlphaSceneNode
{
	shared_ptr<ISceneNode> m_pNode;
	Mat4x4 m_Concat;
	float m_ScreenZ;

	// For the STL sort...
	bool const operator <(AlphaSceneNode const &other) { return m_ScreenZ < other.m_ScreenZ; }
};

typedef std::list<AlphaSceneNode *> AlphaSceneNodes;


////////////////////////////////////////////////////
//
// SceneActorMap Description
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////

typedef std::map<ActorId, shared_ptr<ISceneNode> > SceneActorMap;


////////////////////////////////////////////////////
//
// Scene Description
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////

class CameraNode;
class SkyNode;

class Scene
{
protected:
	shared_ptr<SceneNode>  	m_Root;
	shared_ptr<CameraNode> 	m_Camera;
	ID3DXMatrixStack 		*m_MatrixStack;
	AlphaSceneNodes 		m_AlphaSceneNodes;
	SceneActorMap 			m_ActorMap;
	EffectManager			m_Effects;
	D3DLIGHT9				m_Light;

	void RenderAlphaPass();

public:

	Scene();
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnUpdate(const int deltaMilliseconds);
	shared_ptr<ISceneNode> FindActor(ActorId id);
	bool AddChild(optional<ActorId> id, shared_ptr<ISceneNode> kid)
	{ 
		if (id.valid())
		{
			// This allows us to search for this later based on actor id
			m_ActorMap[*id] = kid;	
		}
		return m_Root->VAddChild(kid); 
	}

	bool RemoveChild(ActorId id)
	{ 
		m_ActorMap.erase(id);
		return m_Root->VRemoveChild(id); 
	}

	void SetCamera(shared_ptr<CameraNode> camera) { m_Camera = camera; }
	const shared_ptr<CameraNode> GetCamera() const { return m_Camera; }

	void PushAndSetMatrix(const Mat4x4 &toWorld)
	{
		// Note this code carefully!!!!! It is COMPLETELY different
		// from some DirectX 9 documentation out there....
		// Scene::PushAndSetMatrix - Chapter 14, page 486

		m_MatrixStack->Push();
		m_MatrixStack->MultMatrixLocal(&toWorld);
		DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, m_MatrixStack->GetTop());
	}

	void PopMatrix() 
	{
		// Scene::PopMatrix - Chapter 14, page 486
		m_MatrixStack->Pop(); 
		DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, m_MatrixStack->GetTop());	
	}

	const Mat4x4 *GetTopMatrix() 
	{ 
		// Scene::GetTopMatrix - Chapter 14, page 486
		return static_cast<const Mat4x4 *>(m_MatrixStack->GetTop()); 
	}

	void AddAlphaSceneNode(AlphaSceneNode *asn) { m_AlphaSceneNodes.push_back(asn); }

	ID3DXEffect *GetEffect(std::wstring name) { return m_Effects.Get(name); }

	HRESULT Pick(RayCast *pRayCast) { return m_Root->VPick(this, pRayCast); }

};

////////////////////////////////////////////////////
//
// RootNode Description
//
//    This is the root node of the scene graph.
//
////////////////////////////////////////////////////

class RootNode : public SceneNode
{
public:
	RootNode();
	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual bool VRemoveChild(ActorId id);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const { return true; }
};




////////////////////////////////////////////////////
//
// CameraNode Description
//
//    A camera node controls the D3D view transform and holds the view
//    frustum definition
//
////////////////////////////////////////////////////

class CameraNode : public SceneNode
{
public:
	CameraNode(Mat4x4 const *t, Frustum const &frustum) 
	 : SceneNode(optional_empty(), "Camera", RenderPass_0, t),
	   m_Frustum(frustum),
	   m_bActive(true),
	   m_DebugCamera(false),
	   m_pTarget(shared_ptr<SceneNode>()),
	   m_CamOffsetVector( 0.0f, 0.0f, -10.0f, 0.0f )
	{ 
	}

	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const { return m_bActive; }

	virtual HRESULT SetViewTransform(Scene *pScene);

	const Frustum &GetFrustum() { return m_Frustum; }
	void SetTarget(shared_ptr<SceneNode> pTarget)
		{ m_pTarget = pTarget; }

	void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }
	shared_ptr<SceneNode> GetTarget() { return m_pTarget; }

	Mat4x4 GetWorldViewProjection(Scene *pScene);
	Mat4x4 GetProjection() { return m_Projection; }
	Mat4x4 GetView() { return m_View; }

	void SetCameraOffset( const Vec4 & cameraOffset )
	{
		m_CamOffsetVector = cameraOffset;
	}

protected:

	Frustum			m_Frustum;
    Mat4x4			m_Projection;
	Mat4x4			m_View;
	bool			m_bActive;
	bool			m_DebugCamera;
	shared_ptr<SceneNode> m_pTarget;
	Vec4			m_CamOffsetVector;	//Direction of camera relative to target.
};

////////////////////////////////////////////////////
//
// MeshNode Description
//
//    Attaches a D3D mesh object to the scene graph
//
////////////////////////////////////////////////////

class MeshNode : public SceneNode
{
protected:
	ID3DXMesh *m_pMesh;
	std::wstring m_XFileName;


public:
	MeshNode(const optional<ActorId> actorId, 
		std::string name, 
		ID3DXMesh *mesh, 
		RenderPass renderPass, 
		const Mat4x4 *t, 
		const Color &color); 

	MeshNode(const optional<ActorId> actorId, 
		std::string name, 
		std::wstring xFileName, 
		RenderPass renderPass, 
		const Mat4x4 *t, 
		const Color &color); 

	virtual ~MeshNode() { SAFE_RELEASE(m_pMesh); }
	HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
	
	float CalcBoundingSphere();
};


class ShaderMeshNode : public MeshNode
{
protected:
	ID3DXEffect *m_pEffect;
	std::wstring m_fxFileName;

public:
	ShaderMeshNode(const optional<ActorId> actorId, 
		std::string name, 
		ID3DXMesh *mesh, 
		std::wstring fxFileName,
		RenderPass renderPass, 
		const Mat4x4 *t, 
		const Color &color); 

	ShaderMeshNode(const optional<ActorId> actorId, 
		std::string name, 
		std::wstring xFileName, 
		std::wstring fxFileName,
		RenderPass renderPass, 
		const Mat4x4 *t, 
		const Color &color); 

	virtual ~ShaderMeshNode() { SAFE_RELEASE(m_pEffect); }
	virtual HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);
};

////////////////////////////////////////////////////
//
// class TeapotMeshNode				- Chapter 19, page 694
//
////////////////////////////////////////////////////

class TeapotMeshNode : public ShaderMeshNode
{
public:
	TeapotMeshNode(const optional<ActorId> actorId, std::string name, std::wstring fxFileName, RenderPass renderPass, const Mat4x4 *t, const Color &color);
	virtual HRESULT VOnRestore(Scene *pScene);
};


////////////////////////////////////////////////////
//
// Grid Description
//
//    A slightly modified version of Grid from 
//       the beginning 3D chapter
//
////////////////////////////////////////////////////

class Grid : public SceneNode
{
protected:
	GridParams				m_params;			// Grid creation parameters
	LPDIRECT3DTEXTURE9		m_pTexture;			// the grid texture for the axes planes
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			// the grid verts
	LPDIRECT3DINDEXBUFFER9	m_pIndices;			// the grid index
	DWORD					m_numVerts;
	DWORD					m_numPolys;

public:
	bool					m_bTextureHasAlpha;

	Grid(optional<ActorId> actorId, std::string name, const GridParams &params);
	virtual ~Grid();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	bool VHasAlpha() const { return m_bTextureHasAlpha; }
	virtual ActorParams * VGetActorParams( void )
	{
		return &m_params;
	}
};



////////////////////////////////////////////////////
//
// ArrowNode Description
//
//   Graphical object that looks like an arrow - they
//     aren't discussed in the book per se but you see them
//     in a lot of the figures!
//
////////////////////////////////////////////////////

class ArrowNode : public SceneNode
{
protected:
	ID3DXMesh *m_shaft;
	ID3DXMesh *m_cone;
	Mat4x4 m_coneTrans;
	Mat4x4 m_shaftTrans;
	
public:
	ArrowNode(std::string name, const float length, const Mat4x4 *t, const Color &color);

	virtual ~ArrowNode() { SAFE_RELEASE(m_shaft); SAFE_RELEASE(m_cone); }
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
};



////////////////////////////////////////////////////
//
// SkyNode Description
//
//    Implements a believable sky that follows
//	  the camera around
//
////////////////////////////////////////////////////

class SkyNode : public SceneNode
{
protected:
	LPDIRECT3DTEXTURE9		m_pTexture[5];		// the sky textures
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			// the sky verts
	DWORD					m_numVerts;
	DWORD					m_sides;
	const char *			m_textureBaseName;
	shared_ptr<CameraNode>	m_camera;
	bool					m_bActive;

public:
	SkyNode(const char *textureFile, shared_ptr<CameraNode> camera);
	virtual ~SkyNode();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);
	HRESULT VPreRender(Scene *pScene);
	bool VIsVisible(Scene *pScene) const { return m_bActive; }

};


////////////////////////////////////////////////////
//
// TestObject Description
//
//    Implements a test object that has no symmetry on X, Y, and Z axes.
//    Really useful to make sure you haven't flipped left-handed versus
//    right handed coordinate systems or something like that
//
////////////////////////////////////////////////////

//class TriangleIterator;
struct TestObjectParams;


class TestObject : public SceneNode
{
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			
	DWORD					m_numVerts;
	DWORD					m_numPolys;

	DWORD					m_color;
	bool					m_squashed;

public:
	TestObject(TestObjectParams const &p, std::string name);
	virtual ~TestObject();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);

//	TriangleIterator *CreateTriangleIterator();

	static WORD g_TestObjectIndices[][3];
	static Vec3 g_CubeVerts[];
	static Vec3 g_SquashedCubeVerts[];
};



////////////////////////////////////////////////////
//
// MovementController Description
//
//    Implements a WASD style movement controller
//
//  class MovementController		- Chapter 9, page 244
//
////////////////////////////////////////////////////

class MovementController : public IMouseHandler, public IKeyboardHandler
{
protected:
	Mat4x4  m_matFromWorld;
	Mat4x4	m_matToWorld;
    Mat4x4  m_matPosition;

	CPoint					m_lastMousePos;
	BYTE					m_bKey[256];			// Which keys are up and down

	// Orientation Controls
	float		m_fTargetYaw;
	float		m_fTargetPitch;
	float		m_fYaw;
	float		m_fPitch;
	float		m_fPitchOnDown;
	float		m_fYawOnDown;
	float		m_maxSpeed;
	float		m_currentSpeed;

	// Added for Ch19/20 refactor
	bool		m_mouseLButtonDown;
	bool		m_bRotateWhenLButtonDown;

	shared_ptr<SceneNode> m_object;

public:
	MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown);
	void SetObject(shared_ptr<SceneNode> newObject);

	void OnUpdate(DWORD const elapsedMs);

public:
	bool VOnMouseMove(const CPoint &mousePos);
	bool VOnLButtonDown(const CPoint &mousePos);
	bool VOnLButtonUp(const CPoint &mousePos);
	bool VOnRButtonDown(const CPoint &) { return false; }
	bool VOnRButtonUp(const CPoint &) { return false; }

	bool VOnKeyDown(const BYTE c) { m_bKey[c] = true; return true; }
	bool VOnKeyUp(const BYTE c) { m_bKey[c] = false; return true; }

	const Mat4x4 *GetToWorld() { return &m_matToWorld; }
	const Mat4x4 *GetFromWorld() { return &m_matFromWorld; }	
};

class Intersection
{
public:
    FLOAT m_fDist;                  // distance from ray origin to intersection
	DWORD m_dwFace;					// the face index of the intersection
	FLOAT m_fBary1, m_fBary2;		// Barycentric coordinates of the intersection
    FLOAT m_tu, m_tv;               // texture coords of intersection
	optional<ActorId> m_actorId;	// Which actor was intersected if there was one
	Vec3 m_worldLoc;				// world location of intersection
	Vec3 m_actorLoc;				// actor local coordinates of intersection
	Vec3 m_normal;					// normal of intersection

	bool const operator <(Intersection const &other) { return m_fDist < other.m_fDist; }
};

template <class T>
void InitIntersection(Intersection &intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v, optional<ActorId> actorId, WORD* pIndices, T* pVertices, const Mat4x4 &matWorld);


typedef std::vector<Intersection> IntersectionArray;

class RayCast
{
protected:
	LPDIRECT3DVERTEXBUFFER9     m_pVB; 

public:
	RayCast(CPoint point, DWORD maxIntersections = 16);

	DWORD m_MaxIntersections;
	DWORD m_NumIntersections;
	bool m_bUseD3DXIntersect;      // Whether to use D3DXIntersect
	bool m_bAllHits;			// Whether to just get the first "hit" or all "hits"
	CPoint m_Point;

	D3DXVECTOR3 m_vPickRayDir;
    D3DXVECTOR3 m_vPickRayOrig;

	IntersectionArray m_IntersectionArray;

	HRESULT Pick(Scene *pScene, optional<ActorId> actorId, ID3DXMesh *pMesh);
	HRESULT Pick(Scene *pScene, optional<ActorId> actorId, LPDIRECT3DVERTEXBUFFER9 pVerts, LPDIRECT3DINDEXBUFFER9 pIndices, DWORD numPolys);
	HRESULT Pick(Scene *pScene, optional<ActorId> actorId, LPDIRECT3DVERTEXBUFFER9 pVerts, DWORD numPolys);

	void Sort();
};


