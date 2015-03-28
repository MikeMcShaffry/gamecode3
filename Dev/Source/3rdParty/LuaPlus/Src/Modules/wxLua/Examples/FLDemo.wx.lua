require "wxLua"

-----------------------------------------------------------------------------
-- Name:        FLDemo.wx.lua
-- Purpose:     'FL' wxLua sample
-- Author:      J Winwood
-- Modified by:
-- Created:
-- RCS-ID:
-- Copyright:   (c) 2002 J Winwood. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------

frame = nil

function CreateTextCtrl(name)
    local textctrl = wx.wxTextCtrl(frame,
                                   -1,
                                   name,
                                   wx.wxDefaultPosition,
                                   wx.wxSize(0,0),
                                   wx.wxTE_MULTILINE)
    textctrl:SetBackgroundColour(wx.wxColour(192,192,192))
    return textctrl
end

function main()

    -- return the path part of the currently executing file
    local strAt = string.byte('@')
    function getPath()

        function findLast(filePath)
            local lastOffset = nil
            local offset
            repeat
                offset = string.find(filePath, "\\")
                if offset == nil then
                    offset = string.find(filePath, "/")
                end
                if offset then
                    if not lastOffset then
                        lastOffset = offset
                    else
                        lastOffset = lastOffset + offset
                    end
                    filePath = string.sub(filePath, offset + 1)
                end
            until not offset
            return lastOffset
        end

        local filePath = debug.getinfo(1, "S").source
        if string.byte(filePath) == strAt then
            local offset = findLast(filePath)
            if offset ~= nil then
                -- remove the @ at the front up to just before the path separator
                filePath = string.sub(filePath, 2, offset - 1)
            else
                filePath = "."
            end
        else
            filePath = wx.wxGetCwd().."/Examples"
        end
        return filePath
    end

    local bitmapPath = getPath().."/bitmaps/"
    newBitmap     = wx.wxBitmapFromFile(bitmapPath.."new.bmp",     wx.wxBITMAP_TYPE_BMP)
    openBitmap    = wx.wxBitmapFromFile(bitmapPath.."open.bmp",    wx.wxBITMAP_TYPE_BMP)
    saveBitmap    = wx.wxBitmapFromFile(bitmapPath.."save.bmp",    wx.wxBITMAP_TYPE_BMP)
    saveAllBitmap = wx.wxBitmapFromFile(bitmapPath.."saveall.bmp", wx.wxBITMAP_TYPE_BMP)
    cutBitmap     = wx.wxBitmapFromFile(bitmapPath.."cut.bmp",     wx.wxBITMAP_TYPE_BMP)
    copyBitmap    = wx.wxBitmapFromFile(bitmapPath.."copy.bmp",    wx.wxBITMAP_TYPE_BMP)
    pasteBitmap   = wx.wxBitmapFromFile(bitmapPath.."paste.bmp",   wx.wxBITMAP_TYPE_BMP)
    helpBitmap    = wx.wxBitmapFromFile(bitmapPath.."help.bmp",    wx.wxBITMAP_TYPE_BMP)

-- create the frame window
    frame = wx.wxFrame( wx.wxNull,
                        -1,
                        "FL Demo",
                        wx.wxPoint(-1, -1),
                        wx.wxSize(450, 450),
                        wx.wxDEFAULT_FRAME_STYLE )

    client = CreateTextCtrl("Client")

    layout = wx.wxFrameLayout(frame, client)

    if wx.wxPlatformGTK then
        local props = layout:GetPaneProperties()
        props:SetRealTimeUpdates(wx.FALSE)
        layout:SetPaneProperties(props, wx.wxALL_PANES)
    end

    layout:SetUpdatesManager(wx.cbGCUpdatesMgrDefault())

    local fileMenu = wx.wxMenu()
    fileMenu:Append(wx.wxID_EXIT,
                    "&Exit",
                    "Quit the program")

    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT,
                    "&About",
                    "About the wxLua FL Demo Application")

    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")

    frame:SetMenuBar(menuBar)

	frame:CreateStatusBar(1)
	frame:SetStatusText("Welcome to wxLua.")

    frame:ConnectEvent(wx.wxID_EXIT,
				       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                          frame:Close(TRUE)
                       end )

    frame:ConnectEvent( wx.wxEVT_CLOSE_WINDOW,
                    function ( event )
                        event:Skip()
                        layout:Delete()
                        layout = nil
                    end)

    frame:ConnectEvent(wx.wxID_ABOUT,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                          wx.wxMessageBox( 'This is the "About" dialog of the FL Demo wxLua sample.',
                                           "About wxLua",
                                           wx.wxOK + wx.wxICON_INFORMATION,
                                           frame )
                       end )

    layout:PushDefaultPlugins()
    layout:AddPlugin(wx.wxClassInfo("cbBarHintsPlugin"))
    layout:AddPlugin(wx.wxClassInfo("cbHintAnimationPlugin"))
--  layout:AddPlugin(wx.wxClassInfo("cbRowDragPlugin"))
    layout:AddPlugin(wx.wxClassInfo("cbAntiflickerPlugin"))
    layout:AddPlugin(wx.wxClassInfo("cbSimpleCustomizationPlugin"))

    local sizes0 = wx.cbDimInfo(200, 45, 200, 85, 175, 35, wx.FALSE, 4, 4)
    local sizes1 = wx.cbDimInfo(150, 35, 150, 85, 175, 35, wx.TRUE,  4, 4)
    local sizes2 = wx.cbDimInfo(195, 32, 185, 32, 195, 35, wx.TRUE,  4, 4, wx.cbDynToolBarDimHandler())

    toolBar = wx.wxDynamicToolBar(frame, -1)

    toolBar:AddTool(wx.wxID_NEW,   newBitmap)
    toolBar:AddTool(wx.wxID_OPEN,  openBitmap)
    toolBar:AddTool(wx.wxID_SAVE,  saveBitmap)
    toolBar:AddSeparator()
    toolBar:AddTool(wx.wxID_CUT,   cutBitmap)
    toolBar:AddTool(wx.wxID_COPY,  copyBitmap)
    toolBar:AddTool(wx.wxID_PASTE, pasteBitmap)
    toolBar:AddSeparator()
    toolBar:AddTool(wx.wxID_ABOUT, helpBitmap)

    layout:AddBar(toolBar,                  sizes2, wx.FL_ALIGN_TOP, 0, 0, "First Toolbar",  wx.FALSE)
    layout:AddBar(CreateTextCtrl("Third"),  sizes1, wx.FL_ALIGN_TOP, 1, 0, "Second Toolbar", wx.TRUE)
    layout:AddBar(CreateTextCtrl("Second"), sizes0, wx.FL_ALIGN_TOP, 1, 0, "Second View",    wx.TRUE)
    layout:AddBar(CreateTextCtrl("First"),  sizes0, wx.FL_ALIGN_TOP, 1, 0, "First View",     wx.TRUE)

    layout:EnableFloating(wx.TRUE)

    frame:Show(wx.TRUE)
end

main()
wx.wxGetBaseApp():MainLoop()
