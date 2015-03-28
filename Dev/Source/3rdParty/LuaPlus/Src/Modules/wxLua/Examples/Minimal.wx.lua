require "wxLua"

-----------------------------------------------------------------------------
-- Name:        minimal.wx.lua
-- Purpose:     'Minimal' wxLua sample
-- Author:      J Winwood
-- Modified by:
-- Created:     16/11/2001
-- RCS-ID:      $Id: Minimal.wx.lua,v 1.1 2003/10/08 22:29:40 john Exp $
-- Copyright:   (c) 2001 J Winwood. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------

frame = nil

function main()

-- create the frame window
    frame = wx.wxFrame( wx.wxNull,
                        -1,
                        "Minimal Demo",
                        wx.wxPoint(-1, -1),
                        wx.wxSize(450, 450),
                        wx.wxDEFAULT_FRAME_STYLE )

-- [[
-- paint event handler
    function Paint(event)
-- create the paint DC
        local dc = wx.wxPaintDC(frame)
-- call some drawing functions
        dc:DrawRectangle(10, 10, 300, 300);
        dc:DrawRoundedRectangle(20, 20, 280, 280, 20);
        dc:DrawEllipse(30, 30, 260, 260);
        dc:DrawText("A test string", 50, 150);
-- the paint DC will be destroyed by the garbage collector,
-- however on Windows 9x/Me this may be too late (DC's are precious resource)
-- so delete it here
        dc:Delete()
    end

-- connect the paint event handler with the paint event
    frame:ConnectEvent(wx.wxEVT_PAINT, Paint)

-- create a simple file menu
    local fileMenu = wx.wxMenu()
    fileMenu:Append(wx.wxID_EXIT,
                    "&Exit",
                    "Quit the program")

-- create a simple help menu
    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT,
                    "&About",
                    "About the wxLua Minimal Application")

-- create a menu bar and append the file and help menus
    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")

-- insert the menu bar into the frame
    frame:SetMenuBar(menuBar)

-- create a simple status bar
	frame:CreateStatusBar(1)
	frame:SetStatusText("Welcome to wxLua.")

-- connect the selection event of the exit menu item to an
-- event handler that closes the window
    frame:ConnectEvent(wx.wxID_EXIT,
				       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                          frame:Close(TRUE)
                       end )

-- connect the selection event of the about menu item
    frame:ConnectEvent(wx.wxID_ABOUT,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                          wx.wxMessageBox( 'This is the "About" dialog of the Minimal wxLua sample.',
                                           "About wxLua",
                                           wx.wxOK + wx.wxICON_INFORMATION,
                                           frame )
                       end )

-- show the frame window
    frame:Show(wx.TRUE)
end

main()
wx.wxGetBaseApp():MainLoop()
