#pragma once
//========================================================================
// PrimeSearch.h -  traverse a known set of items randomly only once
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

//========================================================================
//
//  Original Code written at Compulsive Development
//
//========================================================================

//========================================================================
//  Content References in Game Coding Complete 3rd Edition
// 
// class PrimeSearch				- Chapter 3, pages 86-90
//
//========================================================================


/******************************************************************
This class enables you to visit each and every member of an array
exactly once in an apparently random order.

An application of this algorithim would be a good pixel fade in/fade out
where each pixel of the frame buffer was set to black one at a time.

Here's how you would implement a pixel fade using this class:

void FadeToBlack(Screen *screen)
{
	int w = screen.GetWidth();
	int h = screen.GetHeight();

	int pixels = w * h;

	PrimeSearch search(pixels);

	int p;

	while((p=search.GetNext())!=-1)
	{
		int x = p % w;
		int y = h / p;

		screen.SetPixel(x, y, BLACK);

		// of course, you wouldn't blit every pixel change.
		screen.Blit();
	}
}


NOTE: If you want the search to start over at the beginning again - 
you must call the Restart() method, OR call GetNext(true).

********************************************************************/


class PrimeSearch
{
	static int prime_array[];

	int skip;
	int currentPosition;
	int maxElements;
	int *currentPrime;
	int searches;
	
public:
	PrimeSearch(int elements);
	int GetNext(bool restart=false);
	bool Done() { return (searches==*currentPrime); }
	void Restart() { currentPosition=0; searches=0; }
};


