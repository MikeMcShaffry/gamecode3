//========================================================================
// MessageFilter.cs
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

namespace EditorApp
{
    //========================================================================
    // class MessageHandler                     - Chapter 20, page 762
    //
    // We need to hook up our own message handler, since it's difficult 
    // getting the proper C wndProc params from using the standard C#
    // event handlers.
    //
    //========================================================================

    public class MessageHandler : IMessageFilter 
    {
        const int WM_LBUTTONDOWN = 0x0201;
        const int WM_LBUTTONUP = 0x0202;
        const int WM_LBUTTONDBLCLK = 0x0203;
        const int WM_RBUTTONDOWN = 0x0204;
        const int WM_RBUTTONUP = 0x0205;
        const int WM_RBUTTONDBLCLK = 0x0206;
        const int WM_MBUTTONDOWN = 0x0207;
        const int WM_MBUTTONUP = 0x0208;
        const int WM_MBUTTONDBLCLK = 0x0209;

        const int WM_KEYDOWN = 0x0100;
        const int WM_KEYUP = 0x0101;
        const int WM_SYSKEYDOWN = 0x0104;
        const int WM_SYSKEYUP = 0x0105;
        const int WM_CLOSE = 0x0010;


        IntPtr m_formHandle;
        IntPtr m_displayPanelHandle;
        EngineDisplayForm m_parent;
        bool m_gameIsRunning;

        // We take both the EngineDisplayForm's handle and its displayPanel handle, since messages
        // will sometimes be for the form, or the display panel.
        public MessageHandler( IntPtr formHandle, IntPtr displayPanelHandle, EngineDisplayForm parent )
        {
            m_formHandle = formHandle;
            m_displayPanelHandle = displayPanelHandle;
            m_parent = parent;
            m_gameIsRunning = false;
        }

        public bool PreFilterMessage(ref Message m)
        {
            // Intercept messages only if they occur for the EngineDisplayForm
            // or its display panel.
            if (m.HWnd == m_displayPanelHandle || m.HWnd == m_formHandle)
            {
                switch (m.Msg)
                {
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONUP:
                    case WM_LBUTTONDBLCLK:
                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:
                    case WM_RBUTTONDBLCLK:
                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:
                    case WM_MBUTTONDBLCLK:
                    case WM_KEYDOWN:
                    case WM_KEYUP:
                    case WM_SYSKEYDOWN:
                    case WM_SYSKEYUP:
                    case WM_CLOSE:
                        {
                            NativeMethods.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                            // If the left mouse button is up, try doing a 
                            // ray cast to see if it intersects with an actor
                            if (m.Msg == WM_LBUTTONUP)
                            {
                                m_parent.SelectActor();
                            }
                            return true;
                        }
                }
            }
            return false;
        }

        //
        // Application_Idle                     - Chapter 20, page 764
        //
        public void Application_Idle(object sender, EventArgs e)
        {
            try
            {
                // Render the scene if we are idle
                NativeMethods.RenderFrame();
                if (!m_gameIsRunning)
                {
                    // In addition, test to see if the editor is
                    // fully initialized and running.
                    bool isRunning = false;
                    isRunning = NativeMethods.IsGameRunning();
                    if (isRunning)
                    {
                        // If the editor is running, then we should
                        // populate the editor form with information
                        // about the scene.
                        // We just need to do this once. 
                        m_parent.SetEditorReadyForUse();
                        m_gameIsRunning = true;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            m_parent.Invalidate();
        }

    }
}
