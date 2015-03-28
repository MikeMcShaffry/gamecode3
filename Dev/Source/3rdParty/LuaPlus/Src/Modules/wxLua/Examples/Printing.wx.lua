-----------------------------------------------------------------------------
-- Name:        printing.wx.lua
-- Purpose:     'Printing' wxLua sample
-- Author:      J Winwood
-- Modified by:
-- Created:     4/7/2002
-- Modified     
-- RCS-ID:      $Id: Printing.wx.lua,v 1.1 2003/10/08 22:29:40 john Exp $
-- Copyright:   (c) 2002 J Winwood. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------

require "wxLua"

local wxID_PRINT        = wx.wxID_HIGHEST + 1
local wxID_PRINTPREVIEW = wx.wxID_HIGHEST + 2
local wxID_PRINTSETUP   = wx.wxID_HIGHEST + 3
local wxID_PAGESETUP    = wx.wxID_HIGHEST + 4

frame         = nil
printData     = wx.wxPrintData()
pageSetupData = wx.wxPageSetupDialogData()

function DisplayFigure(dc, pageNumber)
	-- call some drawing functions
    dc:DrawRectangle(10, 10, 300, 300)
    dc:DrawRoundedRectangle(20, 20, 280, 280, 20)
    dc:DrawEllipse(30, 30, 260, 260)
	if pageNumber then
		dc:DrawText("Test page "..pageNumber, 50, 150)
	else
		dc:DrawText("A test string", 50, 150)
	end
end

function ConnectPrintEvents(printOut)

    printOut:SetPageInfo(1, 16, 2, 15)

    printOut.HasPage = function(self, pageNum)
                            return wx.TRUE
                       end
                       
    printOut.GetPageInfo = function(self)
                               return 1, 6, 2, 5
                           end

    printOut.OnBeginDocument = function(self, startPage, endPage)
                                   return self:base_OnBeginDocument(startPage, endPage)
                               end
                           
    printOut.OnEndDocument = function(self)
                             end
    
    printOut.OnBeginPrinting = function(self)
                               end
    
    printOut.OnEndPrinting = function(self)
                             end
    
    printOut.OnPreparePrinting = function(self)
                                 end
                               
    printOut.OnPrintPage = function(self, pageNum)
                                local dc = self:GetDC()
                                
                                -- Marcos modified init
                                
                                local ppiScreenX,  ppiScreenY  = self:GetPPIScreen()
                                local ppiPrinterX, ppiPrinterY = self:GetPPIPrinter()
                                local scale = ppiPrinterX/ppiScreenX
                                local w, h = dc:GetSize()
                                local a ,b = self:GetPageSizeMM()
                                local pageWidth, pageHeight = self:GetPageSizePixels()
                                local overallScale = (scale * ( w / pageWidth))
                                  
--                              print( 'dc:GetUserScale-->'..dc:GetUserScale())
--                              print(string.format('ppiScreenX %s,  ppiScreenY %s', ppiScreenX, ppiScreenY))
--                              print(string.format("ppiPrinterX %s, ppiPrinterY %s", ppiPrinterX, ppiPrinterY))
--                              print(string.format("printOut:GetPageSizeMM--> a %s, b %s dc:GetSize-->w %s, h %s, printOut:GetPageSizePixels--> pageWidth %s, pageHeight %s", a, b, w, h, pageWidth, pageHeight ))
                                  
                                dc:SetUserScale(overallScale, overallScale)
                                  
--                              print('overallScale '..overallScale, 'GetUserScale '..dc:GetUserScale())
                               
                                -- Marcos modified end

                                DisplayFigure(dc, pageNum)
                                
                                return wx.TRUE
                           end
end

function Print()
    local printDialogData = wx.wxPrintDialogDataFromPrintData(printData)
    local printer  = wx.wxPrinter(printDialogData)
    local printout = wx.wxLuaPrintout("Test Print")
    ConnectPrintEvents(printout)
    
    if printer:Print(frame, 
                     printout, 
                     wx.TRUE) == wx.FALSE then
        if printer:GetLastError() == wx.wxPRINTER_ERROR then
            wx.wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", 
                            "Printing.wx.lua", 
                            wx.wxOK)
        else
            wx.wxMessageBox("You cancelled printing", 
                            "Printing.wx.lua", 
                            wx.wxOK)
        end
    else
        printData = printer:GetPrintDialogData():GetPrintData():Copy()
    end
end

function PrintPreview()
    local printerPrintout = wx.wxLuaPrintout("Test Print")   
    ConnectPrintEvents(printerPrintout)
    
    local previewPrintout = wx.wxLuaPrintout("Test Print Preview")   
    ConnectPrintEvents(previewPrintout)
    
    local printDialogData = wx.wxPrintDialogDataFromPrintData(printData):GetPrintData()
    local preview         = wx.wxPrintPreview(printerPrintout, 
                                              previewPrintout, 
                                              printDialogData)
    local result = preview:Ok()
    if result == wx.FALSE then
        wx.wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", 
                        "Printing.wx.lua", 
                        wx.wxOK)
    else
        local previewFrame = wx.wxPreviewFrame(preview, 
                                               frame, 
                                               "Test Print Preview", 
                                               wx.wxPoint(100, 100), 
                                               wx.wxSize(600, 650))

        previewFrame:ConnectEvent(wx.wxEVT_CLOSE_WINDOW,
                                  function (event)
                                  previewFrame:Destroy()
                                  event:Skip()
                                  end )

        previewFrame:Centre(wx.wxBOTH)
        previewFrame:Initialize()
        previewFrame:Show(wx.TRUE)
    end
end

function PrintSetup()
    local printDialogData = wx.wxPrintDialogDataFromPrintData(printData)
    local printerDialog   = wx.wxPrintDialog(frame, printDialogData)
    printerDialog:GetPrintDialogData():SetSetupDialog(wx.TRUE)
    printerDialog:ShowModal()
    printData = printerDialog:GetPrintDialogData():GetPrintData():Copy()
	printerDialog:Destroy()
end

function PageSetup()
    printData = pageSetupData:GetPrintData():Copy()
    local pageSetupDialog = wx.wxPageSetupDialog(frame, pageSetupData)
    pageSetupDialog:ShowModal()    
    printData     = pageSetupDialog:GetPageSetupData():GetPrintData():Copy()
    pageSetupData = pageSetupDialog:GetPageSetupData():Copy()
	pageSetupDialog:Destroy()
end

function main()
-- create the frame window
    frame = wx.wxFrame( wx.wxNull, 
                        -1, 
                        "Printing Demo", 
                        wx.wxPoint(-1, -1), 
                        wx.wxSize(450, 450), 
                        wx.wxDEFAULT_FRAME_STYLE )

-- paint event handler
    function Paint(event)
-- create the paint DC
        local dc = wx.wxPaintDC(frame)			
		DisplayFigure(dc)
-- the paint DC will be destroyed by the garbage collector,
-- however on Windows 9x/Me this may be too late (DC's are precious resource)
-- so delete it here
        dc:Delete()
    end

-- connect the paint event handler with the paint event
    frame:ConnectEvent(wx.wxEVT_PAINT, Paint)        
                                      
-- create a simple file menu
    local fileMenu = wx.wxMenu()
    fileMenu:Append(wxID_PAGESETUP, 
                    "Page S&etup", 
                    "Set up the page")
    fileMenu:Append(wxID_PRINTSETUP, 
                    "Print &Setup", 
                    "Set up the printer")
    fileMenu:Append(wxID_PRINTPREVIEW, 
                    "Print Pre&view", 
                    "Preview the test print")
    fileMenu:Append(wxID_PRINT, 
                    "&Print", 
                    "Print the test print")
    fileMenu:Append(wx.wxID_EXIT, 
                    "&Exit", 
                    "Quit the program")

-- create a simple help menu
    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT, 
                    "&About", 
                    "About the wxLua Printing Application")
    
-- create a menu bar and append the file and help menus
    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")
 
-- insert the menu bar into the frame
    frame:SetMenuBar(menuBar)

-- create a simple status bar
	frame:CreateStatusBar(1)
	frame:SetStatusText("Welcome to wxLua.")
    
    frame:ConnectEvent(wxID_PAGESETUP,
				       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           PageSetup()
                       end )

    frame:ConnectEvent(wxID_PRINTSETUP,
				       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           PrintSetup()
                       end )


    frame:ConnectEvent(wxID_PRINTPREVIEW,
				       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                          PrintPreview()
                       end )


    frame:ConnectEvent(wxID_PRINT, 
				       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                          Print()
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
                          wx.wxMessageBox( 'This is the "About" dialog of the Printing wxLua sample.', 
                                           "About wxLua", 
                                           wx.wxOK + wx.wxICON_INFORMATION, 
                                           frame )
                       end )
    
-- show the frame window
    frame:Show(wx.TRUE)
end

main()

wx.Run()
