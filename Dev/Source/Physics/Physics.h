#pragma once
//========================================================================
// Physics.h : Implements the IGamePhysics interface
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

extern IGamePhysics *CreateGamePhysics();
extern IGamePhysics *CreateNullPhysics();

extern float SpecificGravity(enum PhysicsDensity material);

enum PhysicsMaterial
{
	PhysMat_Playdough,
	PhysMat_Normal,
	PhysMat_Bouncy,
	PhysMat_Slippery,
	PhysMat_MaxMaterials
};


enum PhysicsDensity
{
	// Elemental
	PhysDens_Air,
	PhysDens_Water,

	// Synthetics
	PhysDens_Styrofoam,

	// Woods
	PhysDens_Balsa,
	PhysDens_Bamboo,
	PhysDens_Pine,
	PhysDens_Oak,
	PhysDens_Ebony,

	// Biologic
	PhysDens_Blood,
	PhysDens_Bone,

	// Metals and Stone
	PhysDens_Silicon,
	PhysDens_Aluminum,
	PhysDens_Asbestos,
	PhysDens_Barite,
	PhysDens_Basalt,
	PhysDens_Bismuth,
	PhysDens_Borax,
	PhysDens_Boron,
	PhysDens_Brass,
	PhysDens_Brick,
	PhysDens_Bronze,
	PhysDens_Calcium,
	PhysDens_Carbon,
	PhysDens_Chromium,
	PhysDens_Clay,
	PhysDens_Coal,
	PhysDens_Cobalt,
	PhysDens_Copper,
	PhysDens_Diamond, 
	PhysDens_Dolomite,
	PhysDens_Epoxy,
	PhysDens_Glass,
	PhysDens_Grystal,
	PhysDens_Granite,
	PhysDens_Gold,
	PhysDens_Hematite,
	PhysDens_Iridium,
	PhysDens_Iron_Cast,
	PhysDens_Iron_Wrought,
	PhysDens_Limestone,
	PhysDens_Lead,
	PhysDens_Magnetite,
	PhysDens_Manganese,
	PhysDens_Magnesium,
	PhysDens_Marble,
	PhysDens_Mercury,
	PhysDens_Molybdenum,
	PhysDens_Nickel,
	PhysDens_Platinum,
	PhysDens_Potassium,
	PhysDens_Quartz,
	PhysDens_Sandstone,
	PhysDens_Serpentine,
	PhysDens_Silver,
	PhysDens_Sodium,
	PhysDens_Steel,
	PhysDens_Talc,
	PhysDens_Tar,
	PhysDens_Tellurium,
	PhysDens_Tin,
	PhysDens_Titanium,
	PhysDens_Tungsten,
	PhysDens_Uranium,
	PhysDens_Vanadium,
	PhysDens_Vinyl,
	PhysDens_Wool,
	PhysDens_Zinc,

	PhysDens_Infinite,

	PhysDens_MaxDensities
};