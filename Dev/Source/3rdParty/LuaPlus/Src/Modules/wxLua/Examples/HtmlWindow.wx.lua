require "wxLua"

-------------------------------------------------------------------------=---
-- Name:        HtmlWindow.wx.lua
-- Purpose:     HtmlWindow wxLua sample
-- Author:      J Winwood
-- Created:     May 2002
-- Copyright:   (c) 2002 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---

frame = nil
html  = nil

htmlTextPage = [[<html>
                   <head>
                   <title>Bound Widget demonstration</title>
                   </head>
                   <body>
                        <h3>wxHtmlWidgetCell demonstration</h3>
                        There are three bound widgets below.
                        <hr>
                        <center>
                            <lua text="first widget" 
                                x=100 y=70>
                        </center>
                        <hr>
                        <lua text="small widget" 
                                x=60 y=50>
                        <hr>
                        <lua text="widget with floating width" 
                            float=y x=70 y=40>
                    </body>
                 </html>]]

function main()

-- create the frame window
    frame = wx.wxFrame( wx.wxNull, 
                        -1, 
                        "HtmlWindow Demo", 
                        wx.wxDefaultPosition, 
                        wx.wxSize(450, 450), 
                        wx.wxDEFAULT_FRAME_STYLE )

                                      
-- create a simple file menu
    local fileMenu = wx.wxMenu()
    fileMenu:Append(wx.wxID_PREVIEW, 
                    "Print Pre&view", 
                    "Preview the HTML document")
    fileMenu:Append(wx.wxID_PRINT, 
                    "&Print", 
                    "Print the HTML document")
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
    frame.MenuBar = menuBar

-- create a simple status bar
	frame:CreateStatusBar(2)
	frame:SetStatusText("Welcome to wxLua.")

    frame:ConnectEvent(wx.wxID_PREVIEW, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           local printing = wx.wxHtmlEasyPrinting("HtmlWindow.wx.lua", frame)
                           printing:PreviewText(htmlTextPage)
                       end )

    frame:ConnectEvent(wx.wxID_PRINT, 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           local printing = wx.wxHtmlEasyPrinting("HtmlWindow.wx.lua", frame)
                           printing:PrintText(htmlTextPage)
                       end )

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
                          wx.wxMessageBox( 'This is the "About" dialog of the HtmlWindow wxLua sample.', 
                                           "About wxLua", 
                                           wx.wxOK + wx.wxICON_INFORMATION, 
                                           frame )
                       end)

-- create the html window
    html = wx.wxLuaHtmlWindow(frame)
                       
    html.OnSetTitle = function(self, title)
                           frame.Title = frame.Title.." - "..title
                      end

-- when a lua custom tag is parsed in the html, this event handler
-- will be invoked
    wx.wxGetBaseApp():ConnectEvent(-1,
                                   wx.wxEVT_HTML_TAG_HANDLER,
                                   function (event)
                                       CreateBoundWindow(event)
                                   end)
                                   
-- set the frame window and status bar                       
    html:SetRelatedFrame(frame, "HtmlWindow wxLua Sample : %s")
    html:SetRelatedStatusBar(1)
-- load the document
    html:SetPage(htmlTextPage)
--  html:LoadPage("testpage.html")

-- show the frame window
    wx.wxGetBaseApp().TopWindow = frame
    frame:Show(wx.TRUE)
end

function CreateBoundWindow(event)
    local ax, ay
    local rc, fl = 0

    -- parse the X parameter in the custom lua tag
    rc, ax = event.HtmlTag:GetParamAsInt("X")
    -- parse the Y parameter
    ay = event.HtmlTag:GetParam("Y")
    -- if there is a float tag set the float
    if event.HtmlTag:HasParam("FLOAT") == wx.TRUE then
        fl = ax
    end

    -- create the control to embed
    local parent = event.HtmlParser.Window
    if parent then
        local wnd = wx.wxTextCtrl( parent, 
                                   -1, 
                                   event.HtmlTag:GetParam("TEXT"),
                                   wx.wxPoint(0, 0), 
                                   wx.wxSize(ax, ay), 
                                   wx.wxTE_MULTILINE )
        -- show the control
        wnd:Show(wx.TRUE)
    
        -- create the container widget cell
        local widget = wx.wxHtmlWidgetCell(wnd, fl)
        
        -- insert the cell into the document
        event.HtmlParser:OpenContainer():InsertCell(widget)
        event:SetParseInnerCalled(wx.FALSE)
    end
end

main()
wx.wxGetBaseApp():MainLoop()
