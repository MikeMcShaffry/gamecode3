#pragma once
//========================================================================
// File: TeapotController.h
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
//  class TeapotController				- Chapter 19, page 735
//



#include "Interfaces.h"

class SceneNode;

class TeapotController : public IMouseHandler, public IKeyboardHandler
{
protected:
	BYTE					m_bKey[256];			// Which keys are up and down
	shared_ptr<SceneNode>	m_object;

public:
	TeapotController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch);
//	void SetObject(shared_ptr<SceneNode> newObject);

	void OnUpdate(DWORD const elapsedMs);

public:
	bool VOnMouseMove(const CPoint &mousePos) { return true; }
	bool VOnLButtonDown(const CPoint &mousePos);
	bool VOnLButtonUp(const CPoint &mousePos) { return true; }
	bool VOnRButtonDown(const CPoint &) { return false; }
	bool VOnRButtonUp(const CPoint &) { return false; }

	bool VOnKeyDown(const BYTE c) { m_bKey[c] = true; return true; }
	bool VOnKeyUp(const BYTE c) { m_bKey[c] = false; return true; }
};

