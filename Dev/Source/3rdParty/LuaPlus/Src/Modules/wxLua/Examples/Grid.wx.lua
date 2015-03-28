-----------------------------------------------------------------------------
-- Name:        Grid.wx.lua
-- Purpose:     'Grid' wxLua sample
-- Author:      J Winwood
-- Created:     January 2002
-- Copyright:   (c) 2002 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------
require "wxLua"

function main()

    local frame = wx.wxFrame(wx.wxNull,
                             -1, 
                             "Testing wxGrid",
                             wx.wxPoint(25, 25), 
                             wx.wxSize(350, 250))

    local fileMenu = wx.wxMenu("", wxMENU_TEAROFF)
    fileMenu:Append(wx.wxID_EXIT, "&Exit\tCtrl-X", "Quit the program")
    
    local helpMenu = wx.wxMenu("", wxMENU_TEAROFF)
    helpMenu:Append(wx.wxID_ABOUT, "&About\tCtrl-A", "About the Grid wxLua Application")
    
    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")
    
    frame:SetMenuBar(menuBar)
    
	frame:CreateStatusBar(1)
	frame:SetStatusText("Welcome to wxLua.")
        
    frame:ConnectEvent(wx.wxID_EXIT, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           frame:Close()
                       end )

    frame:ConnectEvent(wx.wxID_ABOUT, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           wx.wxMessageBox( 'This is the "About" dialog of the Grid wxLua sample.', 
                                               "About wxLua", 
                                               wx.wxOK + wx.wxICON_INFORMATION,
                                               frame )
                        end )
    
    local grid = wx.wxGrid(frame, -1)

    grid:CreateGrid(10, 8)
    grid:SetColSize(3, 200)
    grid:SetRowSize(4, 45)
    grid:SetCellValue(0, 0, "First cell")
    grid:SetCellValue(1, 1, "Another cell")
    grid:SetCellValue(2, 2, "Yet another cell")
    grid:SetCellFont(0, 0, wx.wxFont(10, wx.wxROMAN, wx.wxITALIC, wx.wxNORMAL))
    grid:SetCellTextColour(1, 1, wx.wxRED)
    grid:SetCellBackgroundColour(2, 2, wx.wxCYAN)
    
    frame:Show(wx.TRUE)
end

main()

wx.Run()

