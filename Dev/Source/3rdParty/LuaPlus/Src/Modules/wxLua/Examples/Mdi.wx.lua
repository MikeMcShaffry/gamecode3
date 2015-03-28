-----------------------------------------------------------------------------
-- Name:        mdi.wx.lua
-- Purpose:     'Mdi' wxLua sample
-- Author:      J Winwood
-- Modified by:
-- Created:     16/11/2001
-- RCS-ID:      $Id: Mdi.wx.lua,v 1.1 2003/10/08 22:29:40 john Exp $
-- Copyright:   (c) 2001 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------
require "wxLua"

childList   = {}	
numChildren = 0

function main()

    local frame = wx.wxMDIParentFrame( wx.wxNull, 
                                       -1, 
                                       "MDI Demo", 
                                       wx.wxPoint(-1, -1), 
                                       wx.wxSize(450, 450), 
                                       wx.wxDEFAULT_FRAME_STYLE )

    function CreateChild()
        
        local child = wx.wxMDIChildFrame( frame, 
                                          -1, 
                                          "" )
        child:SetSize(330,340)
        childList[child:GetId()] = child    
        numChildren = numChildren + 1
        child:SetTitle("Child "..numChildren)
    
        function Paint(event)
            local dc = wx.wxPaintDC(childList[event:GetId()])			
            dc:DrawRectangle(10, 10, 300, 300);
            dc:DrawRoundedRectangle(20, 20, 280, 280, 20);
            dc:DrawEllipse(30, 30, 260, 260);
            dc:DrawText("A test string", 50, 150);
            dc:Delete()
        end
        child:ConnectEvent(wx.wxEVT_PAINT, Paint)        
    end
                                      
    local fileMenu = wx.wxMenu()
    fileMenu:Append(wx.wxID_NEW,  "&New",  "Create a new child window")
    fileMenu:Append(wx.wxID_EXIT, "&Exit", "Quit the program")
    
    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT, "&About", "About the wxLua MDI Application")
    
    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")
    
    frame:SetMenuBar(menuBar)
    
    frame:CreateStatusBar(1)
    frame:SetStatusText("Welcome to wxLua.")
    
    frame:ConnectEvent(wx.wxID_NEW, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           CreateChild()
                       end )
    
    frame:ConnectEvent(wx.wxID_EXIT, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           frame:Close()
                       end )

    frame:ConnectEvent(wx.wxID_ABOUT, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           wx.wxMessageBox( 'This is the "About" dialog of the MDI wxLua sample.', 
                                               "About wxLua", 
                                               wx.wxOK + wx.wxICON_INFORMATION, 
                                               frame )
                        end )
    
    frame:Show(wx.TRUE)
end

main()

wx.wxGetBaseApp():MainLoop()
