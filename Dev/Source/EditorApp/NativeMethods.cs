//========================================================================
// NativeMethods.cs
//
// Part of the Editor Application
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

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

//========================================================================
//
// This file sets up some of the groundwork enabling us to call functions
// from our unmanaged dll within C# code
//
//========================================================================

namespace EditorApp
{
    //
    // class NativeMethods                  - Chapter 20, page 758
    //
    // We also need to import some functions that will allow us to load
    // the dll, and free it when we're done.
    static class NativeMethods
    {     
#if DEBUG
        const string editorDllName = "Editord.dll";
#else
        const string editorDllName = "Editor.dll";
#endif
        [DllImport(editorDllName)]
        public unsafe static extern int EditorMain(IntPtr instancePtrAddress, IntPtr hPrevInstancePtrAddress, IntPtr hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight);
        [DllImport(editorDllName)]
        public static extern void RenderFrame();
        [DllImport(editorDllName)]
        public static extern void DXShutdown();
        [DllImport(editorDllName)]
        public unsafe static extern void WndProc(IntPtr hWndPtrAddress, int msg, int wParam, int lParam);

        [DllImport(editorDllName)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool IsGameRunning();

        [DllImport(editorDllName)]
        public static extern int GetActorType(uint actorId);
        [DllImport(editorDllName)]
        public static extern int GetNumActors();
        [DllImport(editorDllName)]
        public unsafe static extern void GetActorList(IntPtr actorIdArrayPtrAddress, int size);
        [DllImport(editorDllName)]
        public static extern UInt32 GetActorColor(uint actorId);
        [DllImport(editorDllName)]
        public unsafe static extern void GetActorPos(IntPtr actorPosPtrAddress, uint actorId);
        [DllImport(editorDllName)]
        public unsafe static extern int PickActor(IntPtr hWndPtrAddress);

        [DllImport(editorDllName)]
        public static extern void SetActorPos(uint actorId, float x, float y, float z);
        [DllImport(editorDllName)]
        public static extern void RotateActorXAxis(uint actorId, float radians);
        [DllImport(editorDllName)]
        public static extern void RotateActorYAxis(uint actorId, float radians);
        [DllImport(editorDllName)]
        public static extern void RemoveActor(uint actorId);
        [DllImport(editorDllName)]
        public static extern void CreateActor(int type, UInt32 color, float length, float radius, string lEffectFileName, string lMeshFileName);

        [DllImport(editorDllName)]
        public static extern void SaveLevel(string lFileName);
        [DllImport(editorDllName)]
        public static extern void OpenLevel(string lFileName);
    }
}
