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

#include "PhysicsDebugDrawer.h"

//========================================================================
//
//   - Chapter 15, page 558
//
void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
	if ( IDirect3DDevice9 * d3ddevice = DXUTGetD3D9Device() )
	{
		DWORD oldLightingState;
		d3ddevice->GetRenderState( D3DRS_LIGHTING, &oldLightingState );
	
		// disable lighting for the lines
		d3ddevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
		COLORED_VERTEX verts[2];
	
		verts[0].position.x = from.x();
		verts[0].position.y = from.y();
		verts[0].position.z = from.z();
		verts[0].color = D3DCOLOR_XRGB( BYTE(255*lineColor.x()), BYTE(255*lineColor.y()), BYTE(255*lineColor.z()) );
		verts[1].position.x = to.x();
		verts[1].position.y = to.y();
		verts[1].position.z = to.z();
		verts[1].color = verts[0].color;
		
		d3ddevice->SetFVF( COLORED_VERTEX::FVF );
		d3ddevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, verts, sizeof(COLORED_VERTEX) );
		
		// restore original lighting state
		d3ddevice->SetRenderState( D3DRS_LIGHTING, oldLightingState );
	}
}

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	// draw a line to represent the normal.  This only lasts one frame, and is hard to see.
	//   it might help to linger this drawn object onscreen for a while to make it more noticeable
	
	btVector3 const startPoint = PointOnB;
	btVector3 const endPoint = PointOnB + normalOnB * distance;
	
	drawLine( startPoint, endPoint, color );
}

//========================================================================
//
//   - Chapter 15, page 559
//
void BulletDebugDrawer::reportErrorWarning(const char* warningString)
{
	OutputDebugStringA( warningString );
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	// not implemented
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
	// not implemented, this class just draws everything all the time
}

int BulletDebugDrawer::getDebugMode() const
{
	return -1; // draw everything
}