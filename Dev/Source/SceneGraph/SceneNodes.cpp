//========================================================================
// File: SceneNodes.cpp
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

#include "..\GameCode.h"
#include "..\Actors.h"

#include "SceneNodes.h"
#include "tchar.h"
#include "..\ResourceCache\ResCache2.h"
#include "Geometry.h"


////////////////////////////////////////////////////
// Material Implementation
////////////////////////////////////////////////////

//
// Material::Material				- Chapter 14, page 496
//
Material::Material()
{
	ZeroMemory( &m_D3DMaterial, sizeof( D3DMATERIAL9 ) );
	Set(g_White);
}

//
// Material::SetAmbient				- Chapter 14, page 496
//
void Material::SetAmbient(const Color &color)
{
	m_D3DMaterial.Ambient = color;
}

//
// Material::SetDiffuse				- Chapter 14, page 496
//
void Material::SetDiffuse(const Color &color)
{
	m_D3DMaterial.Diffuse = color;
}

//
// Material::SetSpecular			- Chapter 14, page 497
//
void Material::SetSpecular(const Color &color, const float power)
{
	m_D3DMaterial.Specular = color;
	m_D3DMaterial.Power = power;
}

//
// Material::SetEmissive			- Chapter 14, page 497
//
void Material::SetEmissive(const Color &color)
{
	m_D3DMaterial.Emissive = color;
}


//
// Material::Set					- Chapter 14, page 497
//
void Material::Set(const Color &color)
{
	m_D3DMaterial.Diffuse = color;
	m_D3DMaterial.Ambient = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
	m_D3DMaterial.Specular = g_White;
	m_D3DMaterial.Emissive = g_Black;
}


//
// Material::SetAlpha				- Chapter 14, page 497
//
void Material::SetAlpha(float alpha)
{
	m_D3DMaterial.Diffuse.a = 
		m_D3DMaterial.Ambient.a = 
		m_D3DMaterial.Specular.a = alpha;
}


//
// Material::Use					- Chapter 14, page 497
//
void Material::Use()
{
	DXUTGetD3D9Device()->SetMaterial( &m_D3DMaterial );	
}




////////////////////////////////////////////////////
// SceneNodeProperties Implementation
////////////////////////////////////////////////////

//
// SceneNodeProperties::Transform			- Chapter 14, page 472
//
void SceneNodeProperties::Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const
{
	if (toWorld)
		*toWorld = m_ToWorld;
	
	if (fromWorld)
		*fromWorld = m_FromWorld;
}


////////////////////////////////////////////////////
// SceneNode Implementation
////////////////////////////////////////////////////



SceneNode::~SceneNode()
{
	// Get rid of all those pesky kids...
	//
	// NOTE: Because the kids are all managed by smart pointers, we don't need
	//       to iterate the STL list and get rid of them. It just 'works'.
	//
	//	while (!m_Children.empty())
	//	{
	//		m_Children.pop_front();
	//	}
}

//
// SceneNode::VOnRestore					- Chapter 14, page 475
//
HRESULT SceneNode::VOnRestore(Scene *pScene)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnRestore()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();
	while (i != end)
	{
		(*i)->VOnRestore(pScene);
		i++;
	}
	return S_OK;
}


//
// SceneNode::VSetTransform					- Chapter 14, page 575
//
void SceneNode::VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld)
{
	m_Props.m_ToWorld = *toWorld;
	if (!fromWorld)
	{
		// Good god this is expensive...why bother???? Look in Chapter 14.
		m_Props.m_FromWorld = m_Props.m_ToWorld.Inverse();
	}
	else
	{
		m_Props.m_FromWorld = *fromWorld;
	}
}

//
// SceneNode::VPreRender					- Chapter 14, page 476
//
HRESULT SceneNode::VPreRender(Scene *pScene) 
{
	pScene->PushAndSetMatrix(m_Props.m_ToWorld);
	return S_OK;
}

//
// SceneNode::VPostRender					- Chapter 14, page 476
//
HRESULT SceneNode::VPostRender(Scene *pScene) 
{
	pScene->PopMatrix();
	return S_OK;
}



//
// SceneNode::VIsVisible				- Chapter 14, page 477
//	
bool SceneNode::VIsVisible(Scene *pScene) const
{
	// transform the location of this node into the camera space 
	// of the camera attached to the scene

	Mat4x4 toWorld, fromWorld;
	pScene->GetCamera()->VGet()->Transform(&toWorld, &fromWorld);
	Vec3 pos = VGet()->ToWorld().GetPosition();

	pos = fromWorld.Xform(pos);

	Frustum const &frustum = pScene->GetCamera()->GetFrustum();
	return frustum.Inside(pos, VGet()->Radius());
}

//
// SceneNode::VOnUpdate					- Chapter 14, page 476
//
HRESULT SceneNode::VOnUpdate(Scene *pScene, DWORD const elapsedMs)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnUpdate()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	while (i != end)
	{
		(*i)->VOnUpdate(pScene, elapsedMs);
		i++;
	}
	return S_OK;
}


//
// SceneNode::VRender					- Chapter 14, page 477
//
HRESULT SceneNode::VRender(Scene *pScene)
{
	m_Props.m_Material.Use();

	switch (m_Props.m_AlphaType)
	{
		case AlphaTexture:
			// Nothing to do here....
			break;

		case AlphaMaterial:
			DXUTGetD3D9Device()->SetRenderState( D3DRS_COLORVERTEX, true);
			DXUTGetD3D9Device()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
			break;

		case AlphaVertex:
			assert(0 && _T("Not implemented!"));
			break;
	}

	return S_OK;
}

//
// SceneNode::VRenderChildren					- Chapter 14, page 478
//
HRESULT SceneNode::VRenderChildren(Scene *pScene)
{
	// Iterate through the children....
	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	while (i != end)
	{
		if ((*i)->VPreRender(pScene)==S_OK)
		{
			// You could short-circuit rendering
			// if an object returns E_FAIL from
			// VPreRender()

			// Don't render this node if you can't see it
			if ((*i)->VIsVisible(pScene))
			{
				float alpha = (*i)->VGet()->m_Material.GetAlpha();
				if (alpha==fOPAQUE)
				{
					(*i)->VRender(pScene);
				}
				else if (alpha!=fTRANSPARENT)
				{
					// The object isn't totally transparent...
					AlphaSceneNode *asn = GCC_NEW AlphaSceneNode;
					assert(asn);
					asn->m_pNode = *i;
					asn->m_Concat = *pScene->GetTopMatrix();
					
					Vec4 worldPos(asn->m_Concat.GetPosition());
					
					Mat4x4 fromWorld = pScene->GetCamera()->VGet()->FromWorld();

					Vec4 screenPos = fromWorld.Xform(worldPos);

					asn->m_ScreenZ = screenPos.z;

					pScene->AddAlphaSceneNode(asn);
				}
			}

			(*i)->VRenderChildren(pScene);
		}
		(*i)->VPostRender(pScene);
		i++;
	}

	return S_OK;
}


//
// SceneNode::VAddChild					- Chapter 14, page 480
//
bool SceneNode::VAddChild(shared_ptr<ISceneNode> kid)
{
	m_Children.push_back(kid); 

	// The radius of the sphere should be fixed right here
	Vec3 kidPos = kid->VGet()->ToWorld().GetPosition();
	Vec3 dir = kidPos - m_Props.ToWorld().GetPosition();
	float newRadius = dir.Length() + kid->VGet()->Radius();
	if (newRadius > m_Props.m_Radius)
		m_Props.m_Radius = newRadius;
	return true; 
}

//
// SceneNode::VRemoveChild					- not in the book
//
//   If an actor is destroyed it should be removed from the scene graph.
//   Generally the HumanView will recieve a message saying the actor has been
//   destroyed, and it will then call Scene::RemoveChild which will traverse 
//   the scene graph to find the child that needs removing.
//
bool SceneNode::VRemoveChild(ActorId id)
{
	for(SceneNodeList::iterator i=m_Children.begin(); i!=m_Children.end(); ++i)
	{
		const SceneNodeProperties* pProps = (*i)->VGet();
		if(pProps->ActorId().valid() && id==*pProps->ActorId())
		{
			i = m_Children.erase(i);	//this can be expensive for vectors
			return true;
		}
	}
	return false;
}



HRESULT SceneNode::VPick(Scene *pScene, RayCast *raycast) 
{
	for(SceneNodeList::const_iterator i=m_Children.begin(); i!=m_Children.end(); ++i)
	{
		HRESULT hr = (*i)->VPick(pScene, raycast);

		if (hr==E_FAIL)
			return E_FAIL;
	}

	return S_OK;
}



////////////////////////////////////////////////////
// Scene Implementation
////////////////////////////////////////////////////


//
// Scene::Scene						- Chapter 14, page 483
//
Scene::Scene()
{
	m_Root.reset(GCC_NEW RootNode());
	D3DXCreateMatrixStack(0, &m_MatrixStack);
}

//
// Scene::~Scene					- Chapter 14, page 483
//
Scene::~Scene()
{
	SAFE_RELEASE(m_MatrixStack);
}

//
// Scene::OnRender					- Chapter 14, page 484
//

HRESULT Scene::OnRender()
{
    // Set up lighting states
	// TODO: This should be a Light, that gets added to a list of lights....
	// Some of which would cast shadows...

	DXUTGetD3D9Device()->SetLight( 1, &m_Light );
    DXUTGetD3D9Device()->LightEnable( 1, TRUE );
    DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, TRUE );

	// The render passes usually go like this 
	// 1. Static objects & terrain
	// 2. Actors (dynamic objects that can move)
	// 3. The Sky
	// 4. Anything with Alpha

	if (m_Root && m_Camera)
	{
		// The scene root could be anything, but it
		// is usually a SceneNode with the identity
		// matrix

		m_Camera->SetViewTransform(this);

		if (m_Root->VPreRender(this)==S_OK)
		{
			m_Root->VRender(this);
			m_Root->VRenderChildren(this);
			m_Root->VPostRender(this);
		}
	}

	RenderAlphaPass();

	return S_OK;
}	


//
// Scene::OnRestore					- Chapter 14, page 484
//
HRESULT Scene::OnRestore()
{
	if (!m_Root)
		return S_OK;

    D3DXVECTOR3 vecLightDirUnnormalized(1.0f, -6.0f, 1.0f);
	ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
    m_Light.Type        = D3DLIGHT_POINT;
    m_Light.Diffuse = g_White / 5.0f;
    m_Light.Specular = g_Black;
    m_Light.Ambient = g_White / 50.0f;

	D3DXVec3Normalize( (D3DXVECTOR3*)&m_Light.Direction, &vecLightDirUnnormalized );
    m_Light.Position.x   = 5.0f;
    m_Light.Position.y   = 5.0f;
    m_Light.Position.z   = -2.0f;
    m_Light.Range        = 100.0f;
    m_Light.Falloff		= 1.0f;
    m_Light.Attenuation0	= 0.1f;
    m_Light.Attenuation1	= 0.0f;
    m_Light.Attenuation2	= 0.0f;
    m_Light.Theta			= 0.0f;
    m_Light.Phi			= 0.0f;
 
	return m_Root->VOnRestore(this);
}

//
// Scene::OnUpdate					- Chapter 14, page 485
//
HRESULT Scene::OnUpdate(const int deltaMilliseconds)
{
	static DWORD lastTime = 0;
	DWORD elapsedTime = 0;
	DWORD now = timeGetTime();

	if (!m_Root)
		return S_OK;

	if (lastTime == 0)
	{
		lastTime = now;
	}

	elapsedTime = now - lastTime;
	lastTime = now;

	return m_Root->VOnUpdate(this, elapsedTime);
}

//
// Scene::FindActor					- Chapter 14, page 487
//
shared_ptr<ISceneNode> Scene::FindActor(ActorId id)
{
	SceneActorMap::iterator i = m_ActorMap.find(id);
	if (i==m_ActorMap.end())
	{
		shared_ptr<ISceneNode> null;
		return null;
	}

	return (*i).second;
}


//
// Scene::RenderAlphaPass					- Chapter 14, page 487
//
void Scene::RenderAlphaPass()
{
	Mat4x4 oldWorld;
	DXUTGetD3D9Device()->GetTransform(D3DTS_WORLD, &oldWorld);

	DWORD oldZWriteEnable;
	DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);

	DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    DXUTGetD3D9Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

	// Texture stage states

	m_AlphaSceneNodes.sort();
	while (!m_AlphaSceneNodes.empty())
	{
		AlphaSceneNodes::reverse_iterator i = m_AlphaSceneNodes.rbegin();
		PushAndSetMatrix((*i)->m_Concat);
		(*i)->m_pNode->VRender(this);
		delete (*i);
		PopMatrix();
		m_AlphaSceneNodes.pop_back();
	}

	DXUTGetD3D9Device()->SetRenderState(D3DRS_COLORVERTEX, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);

	DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, &oldWorld);

}


// EffectManager::~EffectManager				- Chapter 14, page 515
//
EffectManager::~EffectManager()
{
	OnLostDevice();
}

// EffectManager::Get							- Chapter 14, page 515
//
ID3DXEffect *EffectManager::Get(std::wstring name)
{
	ID3DXEffect *pEffect = m_EffectMap[name];

	if (pEffect == NULL)
	{	
		// Load the effect file
		DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG | D3DXSHADER_NO_PRESHADER;
		WCHAR effectFile[MAX_PATH];
		DXUTFindDXSDKMediaFileCch( effectFile, MAX_PATH, name.c_str() );	
		D3DXCreateEffectFromFile( DXUTGetD3D9Device(), effectFile, NULL, NULL, dwShaderFlags, NULL, &pEffect, NULL );
		m_EffectMap[name] = pEffect;
	}

	return pEffect;
}

// EffectManager::OnLostDevice					- Chapter 14, page 515
//
void EffectManager::OnLostDevice()
{
	while (! m_EffectMap.empty() )
	{
		EffectNameMap::iterator i = m_EffectMap.begin();
		ID3DXEffect *effect = (*i).second;
		SAFE_RELEASE(effect);
		m_EffectMap.erase ( i );
	}
}

////////////////////////////////////////////////////
// RootNode Implementation
////////////////////////////////////////////////////

//
// RootNode::RootNode					- Chapter 14, page 489
//
RootNode::RootNode()
: SceneNode(optional_empty(), "Root", RenderPass_0, &Mat4x4::g_Identity)
{
	m_Children.reserve(RenderPass_Last);

	shared_ptr<SceneNode> staticGroup(GCC_NEW SceneNode(optional_empty(), "StaticGroup", RenderPass_Static, &Mat4x4::g_Identity));
	m_Children.push_back(staticGroup);	// RenderPass_Static = 0

	shared_ptr<SceneNode> actorGroup(GCC_NEW SceneNode(optional_empty(), "ActorGroup", RenderPass_Actor, &Mat4x4::g_Identity));
	m_Children.push_back(actorGroup);	// RenderPass_Actor = 1

	shared_ptr<SceneNode> skyGroup(GCC_NEW SceneNode(optional_empty(), "SkyGroup", RenderPass_Sky, &Mat4x4::g_Identity));
	m_Children.push_back(skyGroup);	// RenderPass_Sky = 2
}

//
// RootNode::VAddChild					- Chapter 14, page 489
//
bool RootNode::VAddChild(shared_ptr<ISceneNode> kid)
{
	// The Root node has children that divide the scene graph into render passes.
	// Scene nodes will get added to these children based on the value of the
	// render pass member variable.

	if (!m_Children[kid->VGet()->RenderPass()])
	{
		assert(0 && _T("There is no such render pass"));
		return false;
	}

	return m_Children[kid->VGet()->RenderPass()]->VAddChild(kid);
}

//
// RootNode::VRemoveChild					- added post-press
//
//   Returns false if nothing was removed
//
bool RootNode::VRemoveChild(ActorId id)
{
	bool anythingRemoved = false;
	for(int i=RenderPass_0; i<RenderPass_Last; ++i)
	{
		if(m_Children[i]->VRemoveChild(id))
		{
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

//
// RootNode::VRenderChildren					- Chapter 14, page 514
//
HRESULT RootNode::VRenderChildren(Scene *pScene)
{
	// This code creates fine control of the render passes.

	for (int pass = RenderPass_0; pass < RenderPass_Last; ++pass)
	{
		switch(pass)
		{
			case RenderPass_Static:
			case RenderPass_Actor:
				m_Children[pass]->VRenderChildren(pScene);
				break;

			case RenderPass_Sky:
			{
				DWORD oldZWriteEnable;
				DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
				DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);

				DWORD oldLightMode;
				DXUTGetD3D9Device()->GetRenderState( D3DRS_LIGHTING, &oldLightMode );
				DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, FALSE );

				DWORD oldCullMode;
				DXUTGetD3D9Device()->GetRenderState( D3DRS_CULLMODE, &oldCullMode );
				DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

				m_Children[pass]->VRenderChildren(pScene);

				// Notice that the render states are returned to 
				// their original settings.....
				// Could there be a better way???

				DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, oldLightMode );
				DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, oldCullMode );

				DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
				break;
			}
		}
	}

	return S_OK;
}


////////////////////////////////////////////////////
// CameraNode Implementation
////////////////////////////////////////////////////


//
// CameraNode::VRender					- Chapter 14, page 492
//
HRESULT CameraNode::VRender(Scene *pScene)
{
	if (m_DebugCamera)
	{
		pScene->PopMatrix();

		m_Frustum.Render();	

		pScene->PushAndSetMatrix(m_Props.ToWorld());
	}

	return S_OK;
} 

//
// CameraNode::VOnRestore					- Chapter 14, page 493
//
HRESULT CameraNode::VOnRestore(Scene *pScene)
{
	m_Frustum.SetAspect(DXUTGetWindowWidth() / (FLOAT) DXUTGetWindowHeight());
	D3DXMatrixPerspectiveFovLH( &m_Projection, m_Frustum.m_Fov, m_Frustum.m_Aspect, m_Frustum.m_Near, m_Frustum.m_Far );		
	DXUTGetD3D9Device()->SetTransform( D3DTS_PROJECTION, &m_Projection );
	return S_OK;
}

//
// CameraNode::SetView					- Chapter 14, page 493
//
HRESULT CameraNode::SetViewTransform(Scene *pScene)
{
	//If there is a target, make sure the camera is
	//rigidly attached right behind the target
	if(m_pTarget)
	{
		Mat4x4 mat = m_pTarget->VGet()->ToWorld();
		Vec4 at = m_CamOffsetVector;
		Vec4 atWorld = mat.Xform(at);
		Vec3 pos = mat.GetPosition() + Vec3(atWorld);
		mat.SetPosition(pos);
		VSetTransform(&mat);
	}

	m_View = VGet()->FromWorld();
	DXUTGetD3D9Device()->SetTransform( D3DTS_VIEW, &m_View );
	return S_OK;
}


Mat4x4 CameraNode::GetWorldViewProjection(Scene *pScene)
{ 
	Mat4x4 world = *pScene->GetTopMatrix();
	Mat4x4 view = VGet()->FromWorld();
	Mat4x4 worldView = world * view;
	return worldView * m_Projection;
}
////////////////////////////////////////////////////
// MeshNode Implementation
////////////////////////////////////////////////////

//
// MeshNode::MeshNode				- Chapter 14, page 504
//
MeshNode::MeshNode(const optional<ActorId> actorId, 
	std::string name, 
	ID3DXMesh *mesh, 
	RenderPass renderPass, 
	const Mat4x4 *t, 
	const Color &color)
 : SceneNode(actorId, name, renderPass, t) 
{ 
	m_pMesh = mesh; 
	if (m_pMesh)
	{
		// Added post press - not all mesh modes have meshes. Some are just effects!
		m_pMesh->AddRef(); 
	}

	Material mat;
	mat.Set(color);
	SetMaterial(mat);
}

//
// MeshNode::MeshNode				- Chapter 14, page 505
//
MeshNode::MeshNode(const optional<ActorId> actorId, 
	std::string name, 
	std::wstring xFileName, 
	RenderPass renderPass, 
	const Mat4x4 *t, 
	const Color &color)
 : SceneNode(actorId, name, renderPass, t) 
{ 
	m_pMesh = NULL;
	m_XFileName = xFileName;

	Material mat;
	mat.Set(color);
	SetMaterial(mat);
}

//
// MeshNode::CalcBoundingSphere				- Chapter 14, page 507
//
float MeshNode::CalcBoundingSphere()
{
    D3DXVECTOR3* pData;
    D3DXVECTOR3 vCenter;
    FLOAT fObjectRadius;
	HRESULT hr;
	V( m_pMesh->LockVertexBuffer( 0, ( LPVOID* )&pData ) );
    V( D3DXComputeBoundingSphere( pData, m_pMesh->GetNumVertices(),
                                  D3DXGetFVFVertexSize( m_pMesh->GetFVF() ), &vCenter, &fObjectRadius ) );
    V( m_pMesh->UnlockVertexBuffer() );

	return fObjectRadius;
}


//
// MeshNode::VRender				- Chapter 14, page 505
//
HRESULT MeshNode::VRender(Scene *pScene)
{
	if (S_OK != SceneNode::VRender(pScene) )
		return E_FAIL;


	return m_pMesh->DrawSubset(0); 
} 

//
// MeshNode::VOnRestore				- Chapter 14, page 506
//
// This function loads the mesh and ensures the mesh has normals; it also optimizes the 
// mesh for the graphics card's vertex cache, which improves performance by organizing 
// the internal triangle list for less cache misses.
//
HRESULT MeshNode::VOnRestore(Scene *pScene)
{
	if (m_XFileName.empty())
	{
		SetRadius(CalcBoundingSphere());
		return S_OK;
	}

	// Change post press - release the mesh only if we have a valid mesh file name to load.
	// Otherwise we likely created it on our own, and needs to be kept.
	SAFE_RELEASE(m_pMesh);

    WCHAR str[MAX_PATH];
    HRESULT hr;

    // Load the mesh with D3DX and get back a ID3DXMesh*.  For this
    // sample we'll ignore the X file's embedded materials since we know 
    // exactly the model we're loading.  See the mesh samples such as
    // "OptimizedMesh" for a more generic mesh loading example.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, m_XFileName.c_str() ) );

    V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, DXUTGetD3D9Device(), NULL, NULL, NULL, NULL, &m_pMesh) );

    DWORD *rgdwAdjacency = NULL;

    // Make sure there are normals which are required for lighting
    if( !(m_pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        ID3DXMesh* pTempMesh;
        V( m_pMesh->CloneMeshFVF( m_pMesh->GetOptions(), 
                                  m_pMesh->GetFVF() | D3DFVF_NORMAL, 
                                  DXUTGetD3D9Device(), &pTempMesh ) );
        V( D3DXComputeNormals( pTempMesh, NULL ) );

        SAFE_RELEASE( m_pMesh );
        m_pMesh = pTempMesh;
    }

    // Optimize the mesh for this graphics card's vertex cache 
    // so when rendering the mesh's triangle list the vertices will 
    // cache hit more often so it won't have to re-execute the vertex shader 
    // on those vertices so it will improve perf.     

    rgdwAdjacency = GCC_NEW DWORD[m_pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    V( m_pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
    V( m_pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
    
	SAFE_DELETE_ARRAY(rgdwAdjacency);

	SetRadius(CalcBoundingSphere());

    return S_OK;
}

HRESULT MeshNode::VPick(Scene *pScene, RayCast *pRayCast)
{
	if (SceneNode::VPick(pScene, pRayCast)==E_FAIL)
		return E_FAIL;

	pScene->PushAndSetMatrix(m_Props.ToWorld());
	HRESULT hr = pRayCast->Pick(pScene, m_Props.ActorId(), m_pMesh);
	pScene->PopMatrix();

	return hr;
}


////////////////////////////////////////////////////
// MeshNode Implementation
////////////////////////////////////////////////////

//
// ShaderMeshNode::ShaderMeshNode				- Chapter 14, page 517
//
ShaderMeshNode::ShaderMeshNode(const optional<ActorId> actorId, 
	std::string name, 
	ID3DXMesh *mesh, 
	std::wstring fxFileName,			// used to be effect pointer - changed to fix a VOnRestore crash.
	RenderPass renderPass, 
	const Mat4x4 *t, 
	const Color &color)
 : MeshNode(actorId, name, mesh, renderPass, t, color) 
{ 
	m_fxFileName = fxFileName;
}

//
// ShaderMeshNode::ShaderMeshNode				- Chapter 14, page 517
//
ShaderMeshNode::ShaderMeshNode(const optional<ActorId> actorId, 
	std::string name, 
	std::wstring xFileName, 
	std::wstring fxFileName,
	RenderPass renderPass, 
	const Mat4x4 *t, 
	const Color &color)
 : MeshNode(actorId, name, xFileName, renderPass, t, color) 
{ 
	m_fxFileName = fxFileName;
}

//
// ShaderMeshNode::VOnRestore					- very similar to MeshNode::VOnRestore
//
HRESULT ShaderMeshNode::VOnRestore(Scene *pScene)
{
	m_pEffect = pScene->GetEffect(m_fxFileName);
	m_pEffect->AddRef();
	return MeshNode::VOnRestore(pScene);
}

//
// ShaderMeshNode::VRender						- Chapter 14, page 517				
//
HRESULT ShaderMeshNode::VRender(Scene *pScene)
{
	if (S_OK != SceneNode::VRender(pScene) )
		return E_FAIL;

	HRESULT hr;

    // Update the effect's variables.  Instead of using strings, it would 
    // be more efficient to cache a handle to the parameter by calling 
    // ID3DXEffect::GetParameterByName

	Mat4x4 worldViewProj = pScene->GetCamera()->GetWorldViewProjection(pScene);

	D3DXCOLOR ambient =  m_Props.GetMaterial().GetAmbient();
    V_RETURN( m_pEffect->SetValue( "g_MaterialAmbientColor", &ambient, sizeof( D3DXCOLOR ) ) );
	D3DXCOLOR diffuse =  m_Props.GetMaterial().GetDiffuse();
    V_RETURN( m_pEffect->SetValue( "g_MaterialDiffuseColor", &diffuse, sizeof( D3DXCOLOR ) ) );


	V( m_pEffect->SetMatrix( "g_mWorldViewProjection", &worldViewProj ) );
    V( m_pEffect->SetMatrix( "g_mWorld", pScene->GetTopMatrix() ) );
    V( m_pEffect->SetFloat( "g_fTime", ( float )1.0f ) );

	D3DXVECTOR3 vLightDir[3];
    D3DXCOLOR vLightDiffuse[3];
	
	D3DXCOLOR vLightAmbient(0.35f, 0.35f, 0.35f, 1.0f);
    D3DXVECTOR3 vecLightDirUnnormalized(1.0f, 6.0f, 1.0f);
    D3DXVECTOR3 vecLightDirNormalized;

    D3DXVec3Normalize( &vecLightDirNormalized, &vecLightDirUnnormalized );

    // Render the light arrow so the user can visually see the light dir
    for( int i = 0; i < 3; i++ )
    {
        vLightDir[i] = vecLightDirNormalized;
        vLightDiffuse[i] = D3DXCOLOR( 1, 1, 1, 1 );
    }

    V( m_pEffect->SetValue( "g_LightDir", vLightDir, sizeof( D3DXVECTOR3 ) * 3 ) );
    V( m_pEffect->SetValue( "g_LightDiffuse", vLightDiffuse, sizeof( D3DXVECTOR4 ) * 3 ) );
	V( m_pEffect->SetInt( "g_nNumLights", 1 ) );
    V( m_pEffect->SetValue( "g_LightAmbient", &vLightAmbient, sizeof( D3DXVECTOR4 ) * 1 ) );
	V( m_pEffect->SetFloat( "g_fAlpha",  m_Props.GetMaterial().GetAlpha() ) );

	if (m_Props.GetMaterial().GetAlpha()!=1.0f)
	{
		V( m_pEffect->SetTechnique( "RenderSceneWith1Light" ) );
	}
	else
	{
		V( m_pEffect->SetTechnique( "RenderSceneWith1Light" ) );
	}

    // Apply the technique contained in the effect 
	UINT iPass, cPasses;

    V( m_pEffect->Begin( &cPasses, 0 ) );

    for( iPass = 0; iPass < cPasses; iPass++ )
    {
        V( m_pEffect->BeginPass( iPass ) );

        // The effect interface queues up the changes and performs them 
        // with the CommitChanges call. You do not need to call CommitChanges if 
        // you are not setting any parameters between the BeginPass and EndPass.
        // V( g_pEffect->CommitChanges() );

        // Render the mesh with the applied technique
        V( m_pMesh->DrawSubset( 0 ) );

        V( m_pEffect->EndPass() );
    }
    V( m_pEffect->End() );

	return S_OK;
} 


//
// TeapotMeshNode::TeapotMeshNode					- Chapter 19, page 694 
//
TeapotMeshNode::TeapotMeshNode(const optional<ActorId> actorId, std::string name, std::wstring fxFileName, RenderPass renderPass, const Mat4x4 *t, const Color &color)
: ShaderMeshNode(actorId, name, NULL, fxFileName, renderPass, t, color) 
{ 
	// there's nothing else needed here...
}

//
// TeapotMeshNode::VOnRestore						- Chapter 19, page 695
//
HRESULT TeapotMeshNode::VOnRestore(Scene *pScene)
{
	HRESULT hr;

	IDirect3DDevice9 * pDevice = DXUTGetD3D9Device();
	V( D3DXCreateTeapot( pDevice, &m_pMesh, NULL ) );

	//Rotate the teapot 90 degrees from default so that the spout faces forward
	Mat4x4 rotateY90 = m_Props.ToWorld();
	rotateY90.SetPosition(Vec3(0.0f, 0.0f, 0.0f));
	IDirect3DVertexBuffer9* pVB = NULL;
	m_pMesh->GetVertexBuffer(&pVB);
	Vec3* pVertices = NULL;
	pVB->Lock( 0, 0, (void**)&pVertices, 0 );
	for (unsigned int i=0; i<m_pMesh->GetNumVertices(); ++i)
	{
		*pVertices = rotateY90.Xform(*pVertices);
		++pVertices;
		//The structs depicted in this vertex buffer actually store
		//information for normals in addition to xyz, thereby
		//making the vertices in pVB twice the size of the one described
		//by *pVertices.  So we address that here.
		*pVertices = rotateY90.Xform(*pVertices);	//rotate the normals, too
		++pVertices;
	}
	pVB->Unlock();
	SAFE_RELEASE( pVB );
	//...end rotation

	// Note - the mesh is needed BEFORE calling the base class VOnRestore.
	V ( ShaderMeshNode::VOnRestore ( pScene ) );

	return S_OK;
}




////////////////////////////////////////////////////
// Grid Implementation
////////////////////////////////////////////////////

//
// Grid::Grid						- Chapter 13, page 448
//
Grid::Grid(optional<ActorId> actorId,
		   std::string name,
		   const GridParams &params)
	: SceneNode(actorId, name, RenderPass_0, &params.m_Mat)
{
	m_params = params;

	m_bTextureHasAlpha = false;

	m_pTexture = NULL;
	m_pVerts = NULL;
	m_pIndices = NULL;
	m_numVerts = m_numPolys = 0;
}

//
// Grid::~Grid						- Chapter 13, page 449
//
Grid::~Grid()
{
	SAFE_RELEASE(m_pTexture);	
	SAFE_RELEASE(m_pVerts);
	SAFE_RELEASE(m_pIndices);
}

//
// Grid::VOnRestore								- Chapter 13, page 449
//   Note: the book incorrectly names this method Grid::Create
//
//	 Here's a list of changes that were made from the book's code:
//      1. The resource cache is used to hold the texture file.
//		2. The texture file isn't hard coded - it is stored in a member variable
//		3. SetRadius() is called with the spherical dimensions of this grid to aid culling
//		4. The input parameter was changed to a platform independant Scene object
//
HRESULT Grid::VOnRestore(Scene *pScene)
{
	// Call the base class's restore
	SceneNode::VOnRestore(pScene);

	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pVerts);
	SAFE_RELEASE(m_pIndices);

	Resource resource(m_params.m_Texture);
	shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);

	if ( FAILED ( D3DXCreateTextureFromFileInMemory( DXUTGetD3D9Device(), texture->Buffer(), texture->Size(), &m_pTexture ) ) )
		return E_FAIL;

	SetRadius( sqrt(m_params.m_Squares * m_params.m_Squares / 2.0f) );

    // Create the vertex buffer - we'll need enough verts
	// to populate the grid. If we want a 2x2 grid, we'll
	// need 3x3 set of verts.
	m_numVerts = (m_params.m_Squares+1)*(m_params.m_Squares+1);

    if( FAILED( DXUTGetD3D9Device()->CreateVertexBuffer( 
		m_numVerts*sizeof(COLORED_TEXTURED_VERTEX),
		D3DUSAGE_WRITEONLY, COLORED_TEXTURED_VERTEX::FVF,
        D3DPOOL_MANAGED, &m_pVerts, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    COLORED_TEXTURED_VERTEX* pVertices;
    if( FAILED( m_pVerts->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
        return E_FAIL;

	for( DWORD j=0; j<(m_params.m_Squares+1); j++ )
    {
		for (DWORD i=0; i<(m_params.m_Squares+1); i++)
		{
			// Which vertex are we setting?
			int index = i + (j * (m_params.m_Squares+1) );
			COLORED_TEXTURED_VERTEX *vert = &pVertices[index];

			// Default position of the grid is centered on the origin, flat on
			// the XZ plane.
			float x = (float)i - (m_params.m_Squares/2.0f);
			float y = (float)j - (m_params.m_Squares/2.0f);
			vert->position = ( x * Vec3(1,0,0) ) + ( y * Vec3(0,0,1) );
			vert->color    = m_params.m_Color;

			// The texture coordinates are set to x,y to make the
			// texture tile along with units - 1.0, 2.0, 3.0, etc.
			vert->tu       = x;
			vert->tv       = y;
		}
	}
    m_pVerts->Unlock();

	// The number of indicies equals the number of polygons times 3
	// since there are 3 indicies per polygon. Each grid square contains
	// two polygons. The indicies are 16 bit, since our grids won't
	// be that big!
	m_numPolys = m_params.m_Squares*m_params.m_Squares*2;
	if( FAILED( DXUTGetD3D9Device()->CreateIndexBuffer(sizeof(WORD) * m_numPolys * 3,
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndices, NULL ) ) )
	{
		return E_FAIL;
	}

    WORD *pIndices;
    if( FAILED( m_pIndices->Lock( 0, 0, (void**)&pIndices, 0 ) ) )
        return E_FAIL;
	
	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	for( DWORD j=0; j<m_params.m_Squares; j++ )
    {
		for (DWORD i=0; i<m_params.m_Squares; i++)
		{
			// Triangle #1  ACB
			*(pIndices) = WORD(i + (j*(m_params.m_Squares+1)));
			*(pIndices+1) = WORD(i + ((j+1)*(m_params.m_Squares+1)));
			*(pIndices+2) = WORD((i+1) + (j*(m_params.m_Squares+1)));

			// Triangle #2  BCD
			*(pIndices+3) = WORD((i+1) + (j*(m_params.m_Squares+1)));
			*(pIndices+4) = WORD(i + ((j+1)*(m_params.m_Squares+1)));
			*(pIndices+5) = WORD((i+1) + ((j+1)*(m_params.m_Squares+1)));
			pIndices+=6;
		}
	}
	
    m_pIndices->Unlock();
	return S_OK;
}

//
// Grid::VRender					- Chapter 13, page 451
//   Note: the book incorrectly names this method Grid::Render
// 
//   This function also handles alpha textures on the Grid, where the book's
//   code does not. 
//
HRESULT Grid::VRender(Scene *pScene)
{
	// This is slightly different from the Chapter 7 implementation...

	DWORD oldLightMode;
	DXUTGetD3D9Device()->GetRenderState( D3DRS_LIGHTING, &oldLightMode );
	DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, FALSE );

	DWORD oldCullMode;
	DXUTGetD3D9Device()->GetRenderState( D3DRS_CULLMODE, &oldCullMode );
	DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.

	DXUTGetD3D9Device()->SetTexture( 0, m_pTexture );

	if (m_bTextureHasAlpha)
	{
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);  
			
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}
	else
	{
		DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}

	DXUTGetD3D9Device()->SetStreamSource( 0, m_pVerts, 0, sizeof(COLORED_TEXTURED_VERTEX) );
	DXUTGetD3D9Device()->SetIndices(m_pIndices);
	DXUTGetD3D9Device()->SetFVF( COLORED_TEXTURED_VERTEX::FVF );
	DXUTGetD3D9Device()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0, 0, m_numVerts, 0, m_numPolys );

	// Notice that the render states are returned to 
	// their original settings.....
	// Could there be a better way???

	DXUTGetD3D9Device()->SetTexture (0, NULL);
	DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, oldLightMode );
	DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, oldCullMode );

	return S_OK;
}


HRESULT Grid::VPick(Scene *pScene, RayCast *pRayCast)
{
	if (SceneNode::VPick(pScene, pRayCast)==E_FAIL)
		return E_FAIL;

	pScene->PushAndSetMatrix(m_Props.ToWorld());

	HRESULT hr = pRayCast->Pick(pScene, m_Props.ActorId(), m_pVerts, m_pIndices, m_numPolys);

	pScene->PopMatrix();

	return hr;
}




////////////////////////////////////////////////////
// SkyNode Implementation
////////////////////////////////////////////////////

//
// SkyNode::SkyNode				- Chapter 14, page 499
//
SkyNode::SkyNode(const char *pTextureBaseName, shared_ptr<CameraNode> camera)
: SceneNode(optional_empty(), "Sky", RenderPass_Sky, &Mat4x4::g_Identity)
, m_camera(camera)
, m_bActive(true)
{
	m_textureBaseName = pTextureBaseName;
	for (int i=0; i<5; ++i)
	{
		m_pTexture[i] = NULL;
	}
}


//
// SkyNode::~SkyNode				- Chapter 14, page 499
//
SkyNode::~SkyNode()
{
	for (int i=0; i<5; ++i)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}
	SAFE_RELEASE(m_pVerts);
}

//
// SkyNode::VOnRestore				- Chapter 14, page 500
//
HRESULT SkyNode::VOnRestore(Scene *pScene)
{
	// Call the base class's restore
	SceneNode::VOnRestore(pScene);

	const char *suffix[] = { "_n.jpg", "_e.jpg",  "_s.jpg",  "_w.jpg",  "_u.jpg" };
	for (int i=0; i<5; ++i)
	{
		char name[256];
		strcpy(name, m_textureBaseName);
		strcat(name, suffix[i]);

		Resource resource(name);
		shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
		if ( FAILED ( D3DXCreateTextureFromFileInMemory( DXUTGetD3D9Device(), texture->Buffer(), texture->Size(), &m_pTexture[i] ) ) )
			return E_FAIL;
	}

	m_numVerts = 20;

    if( FAILED( DXUTGetD3D9Device()->CreateVertexBuffer( 
		m_numVerts*sizeof(COLORED_TEXTURED_VERTEX),
		D3DUSAGE_WRITEONLY, COLORED_TEXTURED_VERTEX::FVF,
        D3DPOOL_MANAGED, &m_pVerts, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    COLORED_TEXTURED_VERTEX* pVertices;
    if( FAILED( m_pVerts->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
        return E_FAIL;

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	COLORED_TEXTURED_VERTEX skyVerts[4];
	D3DCOLOR skyVertColor = 0xffffffff;
	float dim = 50.0f;

	skyVerts[0].position = Vec3( dim, dim, dim ); skyVerts[0].color=skyVertColor; skyVerts[0].tu=1; skyVerts[0].tv=0; 
	skyVerts[1].position = Vec3(-dim, dim, dim ); skyVerts[1].color=skyVertColor; skyVerts[1].tu=0; skyVerts[1].tv=0; 
	skyVerts[2].position = Vec3( dim,-dim, dim ); skyVerts[2].color=skyVertColor; skyVerts[2].tu=1; skyVerts[2].tv=1; 
	skyVerts[3].position = Vec3(-dim,-dim, dim ); skyVerts[3].color=skyVertColor; skyVerts[3].tu=0; skyVerts[3].tv=1; 

	Vec3 triangle[3];
	triangle[0] = Vec3(0,0,0);
	triangle[1] = Vec3(5,0,0);
	triangle[2] = Vec3(5,5,0);

	Vec3 edge1 = triangle[1]-triangle[0];
	Vec3 edge2 = triangle[2]-triangle[0];

	Vec3 normal;
	normal = edge1.Cross(edge2);
	normal.Normalize();

	Mat4x4 rotY;
	rotY.BuildRotationY(D3DX_PI/2.0f);
	Mat4x4 rotX;
	rotX.BuildRotationX(-D3DX_PI/2.0f);
	
	m_sides = 5;

	for (DWORD side = 0; side < m_sides; side++)
	{
		for (DWORD v = 0; v < 4; v++)
		{
			Vec4 temp;
			if (side < m_sides-1)
			{
				temp = rotY.Xform(Vec3(skyVerts[v].position));
			}
			else
			{
				skyVerts[0].tu=1; skyVerts[0].tv=1; 
				skyVerts[1].tu=1; skyVerts[1].tv=0; 
				skyVerts[2].tu=0; skyVerts[2].tv=1; 
				skyVerts[3].tu=0; skyVerts[3].tv=0; 

				temp = rotX.Xform(Vec3(skyVerts[v].position));
			}
			skyVerts[v].position = Vec3(temp.x, temp.y, temp.z);
		}
		memcpy(&pVertices[side*4], skyVerts, sizeof(skyVerts));
	}

	m_pVerts->Unlock();
	return S_OK;
}

//
// SkyNode::VPreRender				- Chapter 14, page 502
//
HRESULT SkyNode::VPreRender(Scene *pScene)
{
	Vec3 cameraPos = m_camera->VGet()->ToWorld().GetPosition();
	Mat4x4 mat = m_Props.ToWorld();
	mat.SetPosition(cameraPos);
	VSetTransform(&mat);

	return SceneNode::VPreRender(pScene);
}


//
// SkyNode::VRender				- Chapter 14, page 502
//
HRESULT SkyNode::VRender(Scene *pScene)
{

	// Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.

    DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	DXUTGetD3D9Device()->SetStreamSource( 0, m_pVerts, 0, sizeof(COLORED_TEXTURED_VERTEX) );
	DXUTGetD3D9Device()->SetFVF( COLORED_TEXTURED_VERTEX::FVF );

	for (DWORD side = 0; side < m_sides; side++)
	{
		// TODO: A good optimization would be to transform the camera's
		// world look vector into local space, and do a dot product. If the
		// result is positive, we shouldn't draw the side since it has to be
		// behind the camera!

		// Sky boxes aren't culled by the normal mechanism

		DXUTGetD3D9Device()->SetTexture( 0, m_pTexture[side] );
		DXUTGetD3D9Device()->DrawPrimitive( D3DPT_TRIANGLESTRIP , 4 * side, 2);
	}

	DXUTGetD3D9Device()->SetTexture (0, NULL);
	return S_OK;
}

////////////////////////////////////////////////////
// ArrowNode Implementation - added post press
////////////////////////////////////////////////////

ArrowNode::ArrowNode(std::string name, const float length, const Mat4x4 *t, const Color &color) 
	: SceneNode(optional_empty(), name, RenderPass_0, t) 
{ 
    D3DXCreateCylinder( DXUTGetD3D9Device(), 0.1f * length, 0.0f, 0.3f * length, 16, 1, &m_cone, NULL);
	D3DXCreateCylinder( DXUTGetD3D9Device(), 0.05f * length, 0.05f * length, 0.7f * length, 16, 2, &m_shaft, NULL);

	m_coneTrans.BuildTranslation( 0, 0, (0.7f + 0.15f) * length);
	m_shaftTrans.BuildTranslation( 0, 0, (0.35f * length));

	Material mat;
	mat.Set(color);
	SetMaterial(mat);

	// The anchor point is at 0,0,0 - so the radius must incorporate the whole length.
	SetRadius(length);
}

HRESULT ArrowNode::VRender(Scene *pScene)
{
	if (S_OK != SceneNode::VRender(pScene) )
		return E_FAIL;

	pScene->PushAndSetMatrix(m_shaftTrans);

	m_shaft->DrawSubset(0); 
	pScene->PopMatrix();

	pScene->PushAndSetMatrix(m_coneTrans);
	m_cone->DrawSubset(0);

	pScene->PopMatrix();
	return S_OK;
}


HRESULT ArrowNode::VPick(Scene *pScene, RayCast *pRayCast)
{
	if (SceneNode::VPick(pScene, pRayCast)==E_FAIL)
		return E_FAIL;

	pScene->PushAndSetMatrix(m_shaftTrans);
	HRESULT hr = pRayCast->Pick(pScene, m_Props.ActorId(), m_shaft);
	pScene->PopMatrix();

	if (hr == E_FAIL)
		return hr;

	pScene->PushAndSetMatrix(m_coneTrans);
	hr = pRayCast->Pick(pScene, m_Props.ActorId(), m_cone);
	pScene->PopMatrix();

	return hr;
}





////////////////////////////////////////////////////
// TestObject Implementation
//
//        - Chapter 14, page 494
////////////////////////////////////////////////////

TestObject::TestObject(TestObjectParams const &p, std::string name)
	: SceneNode(p.m_Id, name, RenderPass_0, &p.m_Mat)
{
	m_color = p.m_Color;

	m_pVerts = NULL;
	m_numVerts = m_numPolys = 0;

	Material mat;
	mat.Set(g_Green);
	SetMaterial(mat);
}


TestObject::~TestObject()
{
	SAFE_RELEASE(m_pVerts);
}


// TestObject::g_CubeVerts
//        - Chapter 14, page 495

Vec3 TestObject::g_CubeVerts[] = 
{
	Vec3( 0.5,0.5,-0.5),		// Vertex 0.
	Vec3(-0.5,0.5,-0.5),		// Vertex 1.
	Vec3(-0.5,0.5,0.5),  		// And so on.
	Vec3(0.5,0.5,0.5),
	Vec3(0.5,-0.5,-0.5),
	Vec3(-0.5,-0.5,-0.5),
	Vec3(-0.5,-0.5,0.5),
	Vec3(0.5,-0.5,0.5)
};


// TestObject::g_SquashedCubeVerts
//        - Chapter 14, page 495

Vec3 TestObject::g_SquashedCubeVerts[] = 
{
	Vec3( 0.5,0.5,-0.25),		// Vertex 0.
	Vec3(-0.5,0.5,-0.25),		// Vertex 1.
	Vec3(-0.5,0.5,0.5),			// And so on.
	Vec3(0.75,0.5,0.5),
	Vec3(0.75,-0.5,-0.5),
	Vec3(-0.5,-0.5,-0.5),
	Vec3(-0.5,-0.3f,0.5),
	Vec3(0.5,-0.3f,0.5)
};


// TestObject::g_TestObjectIndices
//        - Chapter 14, page 495

WORD TestObject::g_TestObjectIndices[][3] = 
{
	{ 0,1,2 },                // Face 0 has three vertices.
	{ 0,2,3 },                // And so on.
	{ 0,4,5 },
	{ 0,5,1 },
	{ 1,5,6 },
	{ 1,6,2 },
	{ 2,6,7 },
	{ 2,7,3 },
	{ 3,7,4 },
	{ 3,4,0 },
	{ 4,7,6 },
	{ 4,6,5 }
};

// TestObject::VOnRestore
//        - Chapter 14, page 496

HRESULT TestObject::VOnRestore(Scene *pScene)
{
	// Call the base class's restore
	SceneNode::VOnRestore(pScene);

	Vec3 center;
	Vec3 *verts = m_squashed ? g_SquashedCubeVerts : g_CubeVerts;
	float radius;
	HRESULT hr = D3DXComputeBoundingSphere( 
		static_cast<D3DXVECTOR3*>(verts), 8,
        D3DXGetFVFVertexSize(D3DFVF_XYZ), 
		&center, &radius );

	SetRadius(radius);

    // Create the vertex buffer - this object is essentailly 
	// a squashed cube, but since we want each face to be flat shaded
	// each face needs its own set of verts - because each vert has a normal
	// and thus can't have any vert shared by adjacent faces.
	m_numPolys = 12;
	m_numVerts = m_numPolys * 3;

    if( FAILED( DXUTGetD3D9Device()->CreateVertexBuffer( 
		m_numVerts*sizeof(UNTRANSFORMED_UNLIT_VERTEX),
		D3DUSAGE_WRITEONLY, UNTRANSFORMED_UNLIT_VERTEX::FVF,
        D3DPOOL_MANAGED, &m_pVerts, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    UNTRANSFORMED_UNLIT_VERTEX* pVertices;
    if( FAILED( m_pVerts->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
        return E_FAIL;

	static Color colors[6] = { g_White, g_Gray65, g_Cyan, g_Red, g_Green, g_Blue };

	for (DWORD face=0; face<m_numPolys; ++face )
	{
		UNTRANSFORMED_UNLIT_VERTEX* v = &pVertices[face * 3];
		v->position = verts[g_TestObjectIndices[face][0]];
		v->diffuse = colors[face/2];
		v->specular    = colors[face/2];
		(v+1)->position = verts[g_TestObjectIndices[face][1]];
		(v+1)->diffuse = colors[face/2];
		(v+1)->specular    = colors[face/2];
		(v+2)->position = verts[g_TestObjectIndices[face][2]];	
		(v+2)->diffuse = colors[face/2];
		(v+2)->specular    = colors[face/2];

		Vec3 a = v->position - (v+1)->position;
		Vec3 b = (v+2)->position - (v+1)->position;

		Vec3 cross = a.Cross(b);
		cross /= cross.Length();
		v->normal = cross;
		(v+1)->normal = cross;
		(v+2)->normal = cross;
	}

	m_pVerts->Unlock();

	return S_OK;
}


// TestObject::VOnRestore
//        - Chapter 14, page 497

HRESULT TestObject::VRender(Scene *pScene)
{
	if (S_OK != SceneNode::VRender(pScene) )
		return E_FAIL;

	DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, TRUE );
	DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	DXUTGetD3D9Device()->SetTexture (0, NULL);
	DXUTGetD3D9Device()->SetRenderState( D3DRS_COLORVERTEX , TRUE );
	DXUTGetD3D9Device()->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	// Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.

	DXUTGetD3D9Device()->SetStreamSource( 0, m_pVerts, 0, sizeof(UNTRANSFORMED_UNLIT_VERTEX) );
	DXUTGetD3D9Device()->SetFVF( UNTRANSFORMED_UNLIT_VERTEX::FVF );

	DXUTGetD3D9Device()->DrawPrimitive( D3DPT_TRIANGLELIST , 0, 12);

	return S_OK;
}

/*
#define CUBE_SIDES (6)

TriangleIterator *TestObject::CreateTriangleIterator()
{
	TriangleIterator *ti = GCC_NEW TriangleIterator;
	int triCountList[CUBE_SIDES];
	for (int i=0; i<CUBE_SIDES; ++i)
		triCountList[i] = 2;

	ti->InitializeStrippedMesh(m_pVerts, sizeof(UNTRANSFORMED_UNLIT_VERTEX), CUBE_SIDES, triCountList );

	return ti;
}
*/





////////////////////////////////////////////////////
// MovementController Implementation - added post press
////////////////////////////////////////////////////

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define RADIANS_TO_DEGREES(x) ((x) * 180.0f / D3DX_PI)
#define DEGREES_TO_RADIANS(x) ((x) * D3DX_PI / 180.0f)

MovementController::MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown)
: m_object(object)
{
	m_object->VGet()->Transform(&m_matToWorld, &m_matFromWorld);

	m_fTargetYaw = m_fYaw = RADIANS_TO_DEGREES(-initialYaw);
	m_fTargetPitch = m_fPitch = RADIANS_TO_DEGREES(initialPitch);

	m_maxSpeed = 30.0f;			// 30 meters per second
	m_currentSpeed = 0.0f;

	Vec3 pos = m_matToWorld.GetPosition();

	m_matPosition.BuildTranslation(pos);

	//m_bLeftMouseDown = false;
    POINT ptCursor;
    GetCursorPos( &ptCursor );
	m_lastMousePos = ptCursor;

	memset(m_bKey, 0x00, sizeof(m_bKey));

	m_mouseLButtonDown = false;
	m_bRotateWhenLButtonDown = rotateWhenLButtonDown;
}


bool MovementController::VOnLButtonDown(const CPoint &mousePos)
{
	m_mouseLButtonDown = true;

	// We want mouise movement to be relative to the position
	// the cursor was at when the user first presses down on
	// the left button
	m_lastMousePos = mousePos;
	return true;
}

bool MovementController::VOnLButtonUp(const CPoint &mousePos)
{
	m_mouseLButtonDown = false;
	return true;
}


//  class MovementController::VOnMouseMove		- Chapter 9, page 246

bool MovementController::VOnMouseMove(const CPoint &mousePos)
{
	// There are two modes supported by this controller.

	if (m_bRotateWhenLButtonDown)
	{
		// Mode 1 - rotate the view only when the left mouse button is down
		// Only look around if the left button is down
		if(m_lastMousePos!=mousePos && m_mouseLButtonDown)
		{
			m_fTargetYaw = m_fTargetYaw + (m_lastMousePos.x - mousePos.x);
			m_fTargetPitch = m_fTargetPitch + (mousePos.y - m_lastMousePos.y);
			m_lastMousePos = mousePos;
		}
	}
	else if(m_lastMousePos!=mousePos)
	{
		// Mode 2 - rotate the controller when the mouse buttons are up
		m_fTargetYaw = m_fTargetYaw + (m_lastMousePos.x - mousePos.x);
		m_fTargetPitch = m_fTargetPitch + (mousePos.y - m_lastMousePos.y);
		m_lastMousePos = mousePos;
	}


	return true;
	return true;
}


//  class MovementController::OnUpdate		- Chapter 9, page 246

void MovementController::OnUpdate(DWORD const deltaMilliseconds)
{
	//if (m_bKey['Q'])
	//{
	//	// This code is a cheat to position the camera exactly in a given
	//	// spot so I can take screen shots!

	//	Mat4x4 camTranslate;
	//	D3DXMatrixTranslation(&m_matPosition, 8.847f, 7.055f, 11.618f);

	//	m_fTargetYaw = m_fYaw += -64.35f;
	//	m_fTargetPitch = m_fPitch = 28.57f;

	//	// Calculate the new rotation matrix from the camera
	//	// yaw and pitch.
	//	Mat4x4 matRot;
	//	D3DXMatrixRotationYawPitchRoll(&matRot, DEGREES_TO_RADIANS(m_fYaw), DEGREES_TO_RADIANS(m_fPitch), 0);

	//	// Create the new object-to-world matrix, and the
	//	// new world-to-object matrix. 
	//	D3DXMatrixMultiply(&m_matToWorld, &matRot, &m_matPosition);
	//	D3DXMatrixInverse(&m_matFromWorld, NULL, &m_matToWorld);

	//	m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	//	return;
	//}

	bool bTranslating = false;
	Vec4 atWorld(0,0,0,0);
	Vec4 rightWorld(0,0,0,0);
	Vec4 upWorld(0,0,0,0);

	if (m_bKey['W'] || m_bKey['S'])
	{
		// In D3D, the "look at" default is always
		// the positive Z axis.
		Vec4 at = g_Forward4; 
		if (m_bKey['S'])
			at *= -1;

		// This will give us the "look at" vector 
		// in world space - we'll use that to move
		// the camera.
		atWorld = m_matToWorld.Xform(at);
		bTranslating = true;
	}

	if (m_bKey['A'] || m_bKey['D'])
	{
		// In D3D, the "right" default is always
		// the positive X axis.
		Vec4 right = g_Right4; 
		if (m_bKey['A'])
			right *= -1;

		// This will give us the "right" vector 
		// in world space - we'll use that to move
		// the camera
		rightWorld = m_matToWorld.Xform(right);
		bTranslating = true;
	}

	if (m_bKey[' '] || m_bKey['C'] || m_bKey['X'])
	{
		// In D3D, the "up" default is always
		// the positive Y axis.
		Vec4 up = g_Right4; 
		if (!m_bKey[' '])
			up *= -1;

		//Unlike strafing, Up is always up no matter
		//which way you are looking
		upWorld = up;
		bTranslating = true;
	}

	//Handling rotation as a result of mouse position
	{
		// The secret formula!!! Don't give it away!
		//If you are seeing this now, then you must be some kind of elite hacker!
		m_fYaw += (m_fTargetYaw - m_fYaw) * ( .35f );
		m_fTargetPitch = MAX(-90, MIN(90, m_fTargetPitch));
		m_fPitch += (m_fTargetPitch - m_fPitch) * ( .35f );

		// Calculate the new rotation matrix from the camera
		// yaw and pitch.
		Mat4x4 matRot;
		matRot.BuildYawPitchRoll(DEGREES_TO_RADIANS(-m_fYaw), DEGREES_TO_RADIANS(m_fPitch), 0);

		// Create the new object-to-world matrix, and the
		// new world-to-object matrix. 

		m_matToWorld = matRot * m_matPosition;
		m_matFromWorld = m_matToWorld.Inverse(); 
		m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}

	if (bTranslating)
	{
		float elapsedTime = (float)deltaMilliseconds / 1000.0f;

		Vec3 direction = atWorld + rightWorld + upWorld;
		direction.Normalize(); 

		// Ramp the acceleration by the elapsed time.
		float numberOfSeconds = 5.f;
		m_currentSpeed += m_maxSpeed * ( (elapsedTime*elapsedTime) / numberOfSeconds);
		if (m_currentSpeed > m_maxSpeed)
			m_currentSpeed = m_maxSpeed;

		direction *= m_currentSpeed;

		Vec3 pos = m_matPosition.GetPosition() + direction;
		m_matPosition.SetPosition(pos);
		m_matToWorld.SetPosition(pos);

		m_matFromWorld = m_matToWorld.Inverse();
		m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}
	else
	{
		m_currentSpeed = 0.0f;
	}
}


template <class T>
void InitIntersection(Intersection &intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v, optional<ActorId> actorId, WORD* pIndices, T* pVertices, const Mat4x4 &matWorld)
{
	intersection.m_dwFace = faceIndex;
	intersection.m_fDist = dist;
	intersection.m_fBary1 = u;
    intersection.m_fBary2 = v;

    // iThisTri = &pIndices[3 * pIntersection->m_dwFace];
	T *v0 = &pVertices[pIndices[3 * faceIndex + 0]];
	T *v1 = &pVertices[pIndices[3 * faceIndex + 1]];
	T *v2 = &pVertices[pIndices[3 * faceIndex + 2]];

    // If all you want is the vertices hit, then you are done.  In this sample, we
    // want to show how to infer texture coordinates as well, using the BaryCentric
    // coordinates supplied by D3DXIntersect
    FLOAT dtu1 = v1->tu - v0->tu;
    FLOAT dtu2 = v2->tu - v0->tu;
    FLOAT dtv1 = v1->tv - v0->tv;
    FLOAT dtv2 = v2->tv - v0->tv;
    intersection.m_tu = v0->tu + intersection.m_fBary1 * dtu1 + intersection.m_fBary2 * dtu2;
    intersection.m_tv = v0->tv + intersection.m_fBary1 * dtv1 + intersection.m_fBary2 * dtv2;

	Vec3 a = v0->position - v1->position;
	Vec3 b = v2->position - v1->position;

	Vec3 cross = a.Cross(b);
	cross /= cross.Length();

	Vec3 actorLoc = BarycentricToVec3(v0->position, v1->position, v2->position, intersection.m_fBary1, intersection.m_fBary2);
	intersection.m_actorLoc = actorLoc;
	intersection.m_worldLoc = matWorld.Xform(actorLoc);
	intersection.m_actorId = actorId;
	intersection.m_normal = cross;
}



RayCast::RayCast(CPoint point, DWORD maxIntersections)
{
	m_MaxIntersections = maxIntersections;
	m_IntersectionArray.reserve(m_MaxIntersections);
	m_bUseD3DXIntersect = true;
	m_bAllHits = true;
	m_NumIntersections = 0;
	m_Point = point;
}

HRESULT RayCast::Pick(Scene *pScene, optional<ActorId> actorId, ID3DXMesh *pMesh)
{
	if (!m_bAllHits && m_NumIntersections > 0)
		return S_OK;
	
	HRESULT hr;

    IDirect3DDevice9* pD3Device = DXUTGetD3D9Device();
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

	// Get the inverse view matrix
    const Mat4x4 matView = pScene->GetCamera()->GetView();
    const Mat4x4 matWorld = *pScene->GetTopMatrix();
	const Mat4x4 proj = pScene->GetCamera()->GetProjection();

    // Compute the vector of the Pick ray in screen space
    D3DXVECTOR3 v;
    v.x = ( ( ( 2.0f * m_Point.x ) / pd3dsdBackBuffer->Width ) - 1 ) / proj._11;
    v.y = -( ( ( 2.0f * m_Point.y ) / pd3dsdBackBuffer->Height ) - 1 ) / proj._22;
    v.z = 1.0f;

	
    D3DXMATRIX mWorldView = matWorld * matView;
    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &mWorldView );

    // Transform the screen space Pick ray into 3D space
    m_vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
    m_vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
    m_vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
    m_vPickRayOrig.x = m._41;
    m_vPickRayOrig.y = m._42;
    m_vPickRayOrig.z = m._43;

	ID3DXMesh* pTempMesh;
    V( pMesh->CloneMeshFVF( pMesh->GetOptions(), TEXTURED_VERTEX::FVF,
                                  DXUTGetD3D9Device(), &pTempMesh ) );

    LPDIRECT3DVERTEXBUFFER9 pVB;
    LPDIRECT3DINDEXBUFFER9 pIB;

    pTempMesh->GetVertexBuffer( &pVB );
    pTempMesh->GetIndexBuffer( &pIB );

	WORD* pIndices;
	TEXTURED_VERTEX* pVertices;

	pIB->Lock( 0, 0, ( void** )&pIndices, 0 );
	pVB->Lock( 0, 0, ( void** )&pVertices, 0 );
	
	DWORD intersections = 0;

    // When calling D3DXIntersect, one can get just the closest intersection and not
    // need to work with a D3DXBUFFER.  Or, to get all intersections between the ray and 
    // the mesh, one can use a D3DXBUFFER to receive all intersections.  We show both
    // methods.
    if( !m_bAllHits )
    {
        // Collect only the closest intersection
        BOOL bHit;
        DWORD dwFace;
        FLOAT fBary1, fBary2, fDist;
        D3DXIntersect( pTempMesh, &m_vPickRayOrig, &m_vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist,
                       NULL, NULL );
        if( bHit )
        {
            m_NumIntersections = 1;
			m_IntersectionArray.resize(1);
			InitIntersection(m_IntersectionArray[0], dwFace, fDist, fBary1, fBary2, actorId, pIndices, pVertices, matWorld);
		}
        else
        {
            m_NumIntersections = 0;
        }
    }
    else
    {
        // Collect all intersections
        BOOL bHit;
        LPD3DXBUFFER pBuffer = NULL;
        D3DXINTERSECTINFO* pIntersectInfoArray;
        if( FAILED( hr = D3DXIntersect( pTempMesh, &m_vPickRayOrig, &m_vPickRayDir, &bHit, NULL, NULL, NULL, NULL,
                                        &pBuffer, &intersections ) ) )
        {
            SAFE_RELEASE( pTempMesh );
            SAFE_RELEASE( pVB );
            SAFE_RELEASE( pIB );

            return hr;
        }
        if( intersections > 0 )
        {
			m_IntersectionArray.resize(m_NumIntersections + intersections);

            pIntersectInfoArray = ( D3DXINTERSECTINFO* )pBuffer->GetBufferPointer();
            if( m_NumIntersections > m_MaxIntersections )
                m_NumIntersections = m_MaxIntersections;

            for( DWORD i = 0; i < intersections; i++ )
            {
				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[i + m_NumIntersections];

				InitIntersection(*pIntersection, pIntersectInfoArray[i].FaceIndex,
					pIntersectInfoArray[i].Dist,
					pIntersectInfoArray[i].U,
					pIntersectInfoArray[i].V,
					actorId, pIndices, pVertices, matWorld);
            }
        }

        SAFE_RELEASE( pBuffer );
    }

	m_NumIntersections += intersections;

    pVB->Unlock();
    pIB->Unlock();

    SAFE_RELEASE( pTempMesh );
    SAFE_RELEASE( pVB );
    SAFE_RELEASE( pIB );

	return S_OK;
}


HRESULT RayCast::Pick(Scene *pScene, optional<ActorId> actorId, LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DINDEXBUFFER9 pIB, DWORD numPolys)
{
	if (!m_bAllHits && m_NumIntersections > 0)
		return S_OK;

	WORD* pIndices;
	COLORED_TEXTURED_VERTEX* pVertices;

	pIB->Lock( 0, 0, ( void** )&pIndices, 0 );
	pVB->Lock( 0, 0, ( void** )&pVertices, 0 );

    IDirect3DDevice9* pD3Device = DXUTGetD3D9Device();
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

	// Get the inverse view matrix
    const Mat4x4 matView = pScene->GetCamera()->GetView();
    const Mat4x4 matWorld = *pScene->GetTopMatrix();
	const Mat4x4 proj = pScene->GetCamera()->GetProjection();

    // Compute the vector of the Pick ray in screen space
    D3DXVECTOR3 v;
    v.x = ( ( ( 2.0f * m_Point.x ) / pd3dsdBackBuffer->Width ) - 1 ) / proj._11;
    v.y = -( ( ( 2.0f * m_Point.y ) / pd3dsdBackBuffer->Height ) - 1 ) / proj._22;
    v.z = 1.0f;

	
    D3DXMATRIX mWorldView = matWorld * matView;
    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &mWorldView );

    // Transform the screen space Pick ray into 3D space
    m_vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
    m_vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
    m_vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
    m_vPickRayOrig.x = m._41;
    m_vPickRayOrig.y = m._42;
    m_vPickRayOrig.z = m._43;

    FLOAT fBary1, fBary2;
    FLOAT fDist;
    for( DWORD i = 0; i < numPolys; i++ )
    {
        Vec3 v0 = pVertices[pIndices[3 * i + 0]].position;
        Vec3 v1 = pVertices[pIndices[3 * i + 1]].position;
        Vec3 v2 = pVertices[pIndices[3 * i + 2]].position;

        // Check if the Pick ray passes through this point
        if( IntersectTriangle( m_vPickRayOrig, m_vPickRayDir, v0, v1, v2,
                               &fDist, &fBary1, &fBary2 ) )
        {
            if( m_bAllHits || m_NumIntersections == 0 || fDist < m_IntersectionArray[0].m_fDist )
            {

                if( !m_bAllHits )
                    m_NumIntersections = 0;

				++m_NumIntersections;

				m_IntersectionArray.resize(m_NumIntersections);

				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[m_NumIntersections-1];

				InitIntersection(*pIntersection, i, fDist, fBary1, fBary2, actorId, pIndices, pVertices, matWorld);

                if( m_NumIntersections == m_MaxIntersections )
                    break;
            }
        }
    }

	pVB->Unlock();
    pIB->Unlock();

	return S_OK;
}




void RayCast::Sort() 
{ 
	std::sort(m_IntersectionArray.begin(), m_IntersectionArray.end()) ; 
}



