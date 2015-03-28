//========================================================================
// EngineDisplayForm.cs
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace EditorApp
{
    //
    // enum ActorType                           - Chapter 20, page 773
    //
    public enum ActorType
    {
        AT_Unknown,
        AT_Sphere,
        AT_Teapot,
        AT_TestObject,
        AT_Grid,
        AT_GenericMeshObject
    };

    //========================================================================
    // struct EditorActorParams                 - Chapter 20, page 773
    //
    // We'll use this to pass values to different files
    //
    //========================================================================

    public struct EditorActorParams
    {
        public const uint INVALID_ID = 0;

        public uint m_actorId;
        public ActorType m_actorType;
        public float[] m_pos;
        public Color m_color;
        public float m_length;
        public float m_radius;
        public string m_effectFile;
        public string m_meshFile;

        public EditorActorParams( uint actorId )
        {
            m_actorId = actorId;
            m_actorType = ActorType.AT_Unknown;
            m_pos = new float[0];
            m_color = Color.White;
            m_length = 0;
            m_radius = 0;
            m_effectFile = String.Empty;
            m_meshFile = String.Empty;
        }

        public EditorActorParams(uint actorId, ActorType actorType, float[] pos, Color color, float length, float radius)
        {
            m_actorId = actorId;
            m_actorType = actorType;
            m_pos = pos;
            m_color = color;
            m_length = length;
            m_radius = radius;
            m_effectFile = String.Empty;
            m_meshFile = String.Empty;
        }
    }

    //========================================================================
    //
    // class EngineDisplayForm                      - Chapter 20, page 759
    //
    // The EngineDisplayForm handles the rendering for the editor, and as such,
    // is the main class in the EditorApp that interacts with the unmanaged dll.
    //
    //========================================================================

    public partial class EngineDisplayForm : Form
    {
        const int INVALID_ID = -1;

        private MessageHandler m_messageFilter;

        private ActorPropertiesForm m_actorPropertiesForm;

        public unsafe EngineDisplayForm()
        {
            InitializeComponent();
            try
            {
                // This is how we get the instance handle for our C# app.
                System.IntPtr hInstance = System.Runtime.InteropServices.Marshal.GetHINSTANCE(this.GetType().Module);
                
                // This is how we get the window handle for the panel we'll be rendering into.
                IntPtr hwnd = this.DisplayPanel.Handle;

                // Call into our Dll main function, which will set up an instance of the EditorApp project.
                // Remember that we can't pass the pointer itself, so we'll pass the pointer value.
                // The int will be converted into a pointer value later.
                NativeMethods.EditorMain(hInstance, IntPtr.Zero, hwnd, 1, this.DisplayPanel.Width, this.DisplayPanel.Height);

                m_messageFilter = new MessageHandler(this.Handle, this.DisplayPanel.Handle, this);

                m_actorPropertiesForm = new ActorPropertiesForm(this);
                m_actorPropertiesForm.Show();
            }
            catch (Exception)
            {
                // If something goes wrong with the initialization, free the library
                // TODO QT: Why is this commented out???? Maybe Quoc knows...
                // NativeMethods.FreeLibrary(m_pDll);
            }
        }

        public EditorApp.MessageHandler GetMessageHandler()
        {
            return m_messageFilter;
        }

        //
        // SetEditorReadyForUse             - Chapter 20, page 760
        //
        public void SetEditorReadyForUse()
        {
            m_actorPropertiesForm.InitScene();
        }

        public void Shutdown()
        {
            try
            {
                NativeMethods.DXShutdown();
            }
            catch
            {
                // dll was not initialized so do nothing
            }
        }

        //
        // GetActorInformation                  - Chapter 20, page 760
        //
        public EditorActorParams GetActorInformation(uint actorId)
        {
            EditorActorParams actorParams;

            try
            {
                // We're getting a position array from the unmanaged dll, so
                // allocate space in memory that can hold 3 floats.
                IntPtr tempArray = Marshal.AllocCoTaskMem(3 * sizeof(float));

                NativeMethods.GetActorPos(tempArray, actorId);

                // Copy the memory into a float array and dispose of our memory.
                float[] actorPos = new float[3];
                Marshal.Copy(tempArray, actorPos, 0, 3);
                Marshal.FreeCoTaskMem(tempArray);

                int actorType = NativeMethods.GetActorType(actorId);
                UInt32 actorColor = NativeMethods.GetActorColor(actorId);
                actorParams = new EditorActorParams(actorId, (ActorType)actorType, actorPos, Color.FromArgb((int)actorColor), 0.0f, 0.0f);

                return actorParams;
            }
            catch
            {
                return actorParams = new EditorActorParams(EditorActorParams.INVALID_ID);
            }
        }

        //
        // GetActorList                         - Chapter 20, page 761
        //
        public unsafe int[] GetActorList()
        {
            // We need to know how many actors there are,
            // in order to find out how much memory to allocate
            int numActors = NativeMethods.GetNumActors();

            IntPtr tempArray = Marshal.AllocCoTaskMem(numActors * sizeof(int));
            NativeMethods.GetActorList(tempArray, numActors);

            // Copy the memory into an array of ints, and dispose of our
            // our memory.
            int[] actorList = new int[numActors];
            Marshal.Copy(tempArray, actorList, 0, numActors);
            Marshal.FreeCoTaskMem(tempArray);

            return actorList;
        }

        //
        // SelectActor                              - Chapter 20, page 761
        //
        public unsafe void SelectActor()
        {
            IntPtr hWnd = this.DisplayPanel.Handle;
            int actorId = NativeMethods.PickActor(hWnd);
            if (actorId != INVALID_ID)
            {
                m_actorPropertiesForm.SelectTreeNode(actorId);
            }
        }

   }
}