
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


#include "GameCodeStd.h"
#include "GameCode.h"
//#include "resource.h"

#include "Audio\CAudio.h"
#include "Debugging\Minidump.h"


MiniDumper g_MiniDump(false);				// Chapter 21, page 811-817

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//
// See Game Coding Complete 3rd Edition - Chapter 5 - page 121
//--------------------------------------------------------------------------------------

INT WINAPI GameCode3(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
	// Set up checks for memory leaks.
	// Game Coding Complete reference - Chapter 21, page 834
	//
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// set this flag to keep memory blocks around
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;				// this flag will cause intermittent pauses in your game!

	// perform memory check for each alloc/dealloc
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;				// remember this is VERY VERY SLOW!

	//_CRTDBG_LEAK_CHECK_DF is used at program initialization to force a 
	//   leak check just before program exit. This is important because
	//   some classes may dynamically allocate memory in globally constructed
	//   objects.
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
	if (!g_pApp->InitInstance (hInstance, lpCmdLine)) 
	{
		return FALSE;
	}

    // Pass control to the sample framework for handling the message pump and 
    // dispatching render calls. The sample framework will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.

	DXUTMainLoop();
	DXUTShutdown();

    return g_pApp->GetExitCode();	
}


//This is code that tests shared pointers.


#include <boost\config.hpp>
#include <boost\shared_ptr.hpp>

using boost::shared_ptr;


class IPrintable
{
public:
	virtual void VPrint()=0;
};

class CPrintable : public IPrintable
{
	char *m_Name;
public:
	CPrintable(char *name)   { m_Name = name; printf("create %s\n",m_Name); }
	virtual ~CPrintable()     { printf("delete %s\n",m_Name); }
	void VPrint()             { printf("print %s\n",m_Name); }
};

shared_ptr<CPrintable> CreateAnObject(char *name)
{
	return shared_ptr<CPrintable>(new CPrintable(name));
}

void ProcessObject(shared_ptr<CPrintable> o)
{
	printf("(print from a function) ");
	o->VPrint();
}

void TestSharedPointers(void)
{
	shared_ptr<CPrintable> ptr1(new CPrintable("1"));	// create object 1
	shared_ptr<CPrintable> ptr2(new CPrintable("2"));	// create object 2

	ptr1 = ptr2;						// destroy object 1	
	ptr2 = CreateAnObject("3");			// used as a return value
	ProcessObject(ptr1);				// call a function

	// BAD USEAGE EXAMPLES....
	//
	CPrintable o1("bad");
	//ptr1 = &o1;	   // Syntax error! It's on the stack....
	//
	CPrintable *o2 = new CPrintable("bad2");
	//ptr1 = o2;		// Syntax error! Use the next line to do this...

	ptr1 = shared_ptr<CPrintable>(o2);

	// You can even use shared_ptr on ints!

	shared_ptr<int> a(new int);
	shared_ptr<int> b(new int);

	*a = 5;
	*b = 6;

	const int *q = a.get();				// use this for reading in MT code

	// this is especially cool - you can also use it in lists.
	std::list< shared_ptr<int> > intList;
	std::list< shared_ptr<IPrintable> > printableList;
	for (int i=0; i<100; ++i)
	{
		intList.push_back(shared_ptr<int>(new int(rand())));
		printableList.push_back(shared_ptr<IPrintable>(new CPrintable("list")));
	}

	int *z = new int;
	shared_ptr<int> bad1(z);
	shared_ptr<int> bad2(z);


	// No leaks!!!! Isn't that cool...
}

class CJelly;
class CPeanutButter
{
public:
	shared_ptr<CJelly> m_pJelly;
	CPeanutButter(CJelly *pJelly) { m_pJelly.reset(pJelly); }
};

class CJelly
{
public:
	shared_ptr<CPeanutButter> m_pPeanutButter;
	CJelly();
};

CJelly::CJelly()
{
	m_pPeanutButter.reset(new CPeanutButter(this));
}


void PleaseLeakMyMemory()
{
	shared_ptr<CJelly> pJelly(new CJelly);
}