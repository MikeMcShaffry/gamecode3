require "wxLua"

-----------------------------------------------------------------------------
-- Name:        Sizer.wx.lua
-- Purpose:     Shows using sizers in wxLua
-- Author:      Francis Irving
-- Created:     23/01/2002
-- RCS-ID:      $Id: Sizer.wx.lua,v 1.1 2003/10/08 22:29:40 john Exp $
-- Copyright:   (c) 2002 Creature Labs. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------

frame = wx.wxFrame(wx.wxNull,  -1,  "wxLua sizer test frame")

-- Create two controls (note that their parents are the _frame_ (not the sizer))
textEntry = wx.wxTextCtrl(frame, -1, "Enter URL");
button = wx.wxButton(frame, -1, "test")

-- Put them in a vertical sizer, with ratio 3 units for the text entry, 5 for button
-- and padding of 6 pixels.
sizerTop = wx.wxBoxSizer(wx.wxVERTICAL)
sizerTop:AddWindow(textEntry, 3, wx.wxGROW + wx.wxALL, 6)
sizerTop:AddWindow(button, 5, wx.wxGROW + wx.wxALL, 6)

-- Set up the frame to use that sizer to move/resize its children controls
frame:SetAutoLayout(wx.TRUE)
frame:SetSizer(sizerTop)

-- Optional - these will set an initial minimal size, just enough to hold the
-- controls (more useful for dialogs than a frame)
--sizerTop.SetSizeHints(frame)
--sizerTop.Fit(frame)

-- Start the application
wx.wxGetBaseApp():SetTopWindow(frame)
frame:Show(wx.TRUE)
wx.wxGetBaseApp():MainLoop()
