-------------------------------------------------------------------------=---
-- Name:        scribble.wx.lua
-- Purpose:     'Scribble' wxLua sample
-- Author:      J Winwood
-- Modified by: Thanks to Peter Prade and Nick Trout for fixing
--              the bug in the for loop in DrawPoints()
-- Created:     16/11/2001
-- RCS-ID:      $Id: Scribble.wx.lua,v 1.3 2004/03/31 07:06:03 john Exp $
-- Copyright:   (c) 2001 J Winwood. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---
require "wxLua"

frame          = nil
mouseDown      = nil
pointsList     = {}
itemColour     = 1
running        = 1
isModified     = nil
redrawRequired = 1
fileName       = ""

function main()
    local penColours = { "Red", "Orange", "Yellow", "Green", "Blue", "Purple", "Black", "Grey" }
    local pens = {}
    local screenWidth, screenHeight = wx.wxDisplaySize()
    local bitmap = wx.wxEmptyBitmap(screenWidth, screenHeight)
    
    for idx = 1, 8 do
        pens[idx] = wx.wxPenFromColourName(penColours[idx], 3, wx.wxSOLID)
    end

    frame = wx.wxFrame( wx.wxNull,
                        -1,
                        "Scribble Demo",
                        wx.wxPoint(-1, -1),
                        wx.wxSize(450, 450),
                        wx.wxDEFAULT_FRAME_STYLE )

    function DrawPoints(drawDC)
        for listIndex = 1, table.getn(pointsList) do
            local listValue = pointsList[listIndex]
            local lastX, lastY
            drawDC:SetPen(pens[listValue.penColour])
            for pointsIndex = 1, table.getn(listValue) do
                local pointsValue = listValue[pointsIndex]
                if pointsIndex > 1 then
                    drawDC:DrawLine(lastX, lastY, pointsValue.x, pointsValue.y)
                end
                lastX, lastY = pointsValue.x, pointsValue.y
            end
            drawDC:SetPen(wx.wxNullPen)
        end
    end

    function Paint(event)
        local dc = wx.wxPaintDC(frame)
        dc:BeginDrawing()
    
        if redrawRequired then
            if running then
                local bitmapDC = wx.wxMemoryDC()    
                bitmapDC:SelectObject( bitmap )
                bitmapDC:Clear()           
                DrawPoints(bitmapDC)
                bitmapDC:SelectObject(wx.wxNullBitmap)
                bitmapDC:Delete()
            end
            redrawRequired = nil
        end
        
        if running then
            dc:DrawBitmap(bitmap, 0, 0, 0)
        end
        
        dc:EndDrawing()
        dc:Delete()
    end

    frame:ConnectEvent(wx.wxEVT_PAINT, Paint)
    frame:ConnectEvent(wx.wxEVT_ERASE_BACKGROUND, function(event) end)

    function OnLeftDown(event)
        local pointItem = {}
        pointItem.penColour = itemColour
        frame:SetStatusText(penColours[itemColour], 1)

        itemColour = itemColour + 1
        if penColours[itemColour] == nil then
            itemColour = 1
        end

        local points = {}
        points.x = event:GetX()
        points.y = event:GetY()

        pointItem.n = 1
        pointItem[1] = points
        
        table.insert(pointsList, pointItem)

        frame:CaptureMouse()
        mouseDown = 1
        isModified = 1
        redrawRequired = 1
        frame:Refresh()
    end

    function OnLeftUp(event)
        if mouseDown then
            local points    = {}
            points.x = event:GetX()
            points.y = event:GetY()

            table.insert(pointsList[table.getn(pointsList)], points)

            frame:ReleaseMouse()
            mouseDown = nil
            redrawRequired = 1
            frame:Refresh()
        end
    end

    function OnMotion(event)
        if mouseDown then
            local points    = {}
            points.x = event:GetX()
            points.y = event:GetY()

            table.insert(pointsList[table.getn(pointsList)], points)

            redrawRequired = 1
            frame:Refresh()
        end
    end

    frame:ConnectEvent(-1, wx.wxEVT_LEFT_DOWN, OnLeftDown )
    frame:ConnectEvent(-1, wx.wxEVT_LEFT_UP,   OnLeftUp )
    frame:ConnectEvent(-1, wx.wxEVT_MOTION,    OnMotion )

    function QuerySaveChanges()
        local dialog = wx.wxMessageDialog( frame, 
                                           "Document has changed. Do you wish to save the changes?",
                                           "Save Changes?", 
                                           wx.wxYES_NO + wx.wxCANCEL + wx.wxCENTRE + wx.wxICON_QUESTION )
        local result = dialog:ShowModal()
        dialog:Destroy()
        return result
    end

    function LoadScribbles()
        pointsList = {}        
        return ((pcall(dofile, fileName)) ~= nil)
    end

    -- modified from the lua sample save.lua
    function savevar (n, v)
        if v ~= nil then 
            io.write(n, "=")
            if type(v) == "string" then 
                io.write(format("%q", v))
            elseif type(v) == "table" then
                io.write("{}\n")
                for r,f in v do
                    if type(r) == 'string' then
                        savevar(n.."."..r, f)
                    else
                        savevar(n.."["..r.."]", f)
                    end
                end
            else 
                io.write(tostring(v)) 
            end
            io.write("\n")
        end
    end    

    function SaveScribbles()
        io.output(fileName)
        savevar("pointsList", pointsList)
        io.output()
        return 1
    end
    
    function Open()
        local result     = nil
        local fileDialog = wx.wxFileDialog(frame, 
                                           "Open file", 
                                           "", 
                                           "", 
                                           "Scribble files(*.scribble)|*.scribble", 
                                           wx.wxOPEN)
        if fileDialog:ShowModal() == wx.wxID_OK then
            fileName = fileDialog:GetPath()
            result = LoadScribbles()
            if result then
                frame:SetTitle("Scribble Demo - " .. fileName)
            end
        end
        fileDialog:Destroy()
        return result
    end
    
    function SaveAs()
        local fileDialog = wx.   wxFileDialog(frame)
        fileDialog:SetMessage("Save file as")
        fileDialog:SetStyle(wx.wxSAVE)
        fileDialog:SetWildcard("Scribble files(*.scribble)|*.scribble")
        if fileDialog:ShowModal() == wx.wxID_OK then
            fileName = fileDialog:GetPath()
            result = SaveScribbles()
            if result then
                frame:SetTitle("Scribble Demo - " .. fileName)
            end
        end
        fileDialog:Destroy()
        return result
    end

    function SaveChanges()
       local isOKToContinue
       if fileName == "" then
           isOKToContinue = SaveAs()
       else
           isOKToContinue = SaveScribbles()
       end
       return isOKToContinue
    end

    local fileMenu = wx.wxMenu()
    fileMenu:Append(wx.wxID_NEW,
                    "&New",
                    "Begin a new drawing")
    fileMenu:Append(wx.wxID_OPEN,
                    "&Open",
                    "Open an existing drawing")
    fileMenu:Append(wx.wxID_SAVE,
                    "&Save",
                    "Save the drawing")
    fileMenu:Append(wx.wxID_SAVEAS,
                    "Save &As",
                    "Save the drawing to a new file")
    fileMenu:AppendSeparator()
    fileMenu:Append(wx.wxID_EXIT,
                    "&Exit",
                    "Quit the program")

    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT,
                    "&About",
                    "About the wxLua Minimal Application")

    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")

    frame:SetMenuBar(menuBar)

    frame:CreateStatusBar(2)
    frame:SetStatusText("Welcome to wxLua.")

    frame:ConnectEvent(wx.wxID_EXIT,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                          frame:Close(wx.TRUE)
                       end )

    frame:ConnectEvent(wx.wxID_NEW,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                         local isOKToContinue = 1
                         if isModified then
                             local response = QuerySaveChanges()
                             if response == wx.wxID_YES then
                                 isOKToContinue = SaveChanges()
                             elseif response == wx.wxID_CANCEL then
                                 isOKToContinue = nil
                             end
                         end
                         if isOKToContinue then
                             fileName = ""
                             frame:SetTitle( "Scribble Demo")
                             pointsList = {}
                             redrawRequired = 1
                             frame:Refresh()
                             isModified = nil
                         end
                       end )

    frame:ConnectEvent(wx.wxID_OPEN,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                         local isOKToContinue = 1
                         if isModified then
                             local response = QuerySaveChanges()
                             if response == wx.wxID_YES then
                                 isOKToContinue = SaveChanges()
                             elseif response == wx.wxID_CANCEL then
                                 isOKToContinue = nil
                             end
                         end
                         if isOKToContinue then
                             Open()
                             redrawRequired = 1
                             frame:Refresh()
                             isModified = nil
                         end
                       end )

    frame:ConnectEvent(wx.wxID_SAVE,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                         local isSaved
                         if fileName == "" then
                             isSaved = SaveAs()
                         else
                             isSaved = SaveScribbles()
                         end
                         if isSaved then
                             isModified = nil
                         end
                       end )

    frame:ConnectEvent(wx.wxID_SAVEAS,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                         local isSaved = SaveAs()
                         if isSaved then
                             isModified = nil
                         end
                       end )

    frame:ConnectEvent(wx.wxID_ABOUT,
                       wx.wxEVT_COMMAND_MENU_SELECTED,
                       function (event)
                          wx.wxMessageBox( 'This is the "About" dialog of the Scribble wxLua Sample.',
                                           "About wxLua",
                                           wx.wxOK + wx.wxICON_INFORMATION,
                                           frame )
                       end )

    frame:ConnectEvent(wx.wxEVT_CLOSE_WINDOW,
                       function (event)
                          local isOkToClose = 1
                          if isModified then
                            local dialog = wx.wxMessageDialog( frame, 
                                                                  "Save changes before exiting?",
                                                                  "Save Changes?", 
                                                                  wx.wxYES_NO + wx.wxCENTRE + wx.wxICON_QUESTION )
                            local result = dialog:ShowModal()
                            --dialog:Destroy()
                            if result == wx.wxID_YES then
                                isOkToClose  = SaveChanges()
                            end
                          end
                          if isOkToClose then
                              -- prevent paint events using the bitmapDC after we
                              -- have deleted them
                              running = nil
                              bitmap:Delete()
                              -- ensure the event is passed on so 
                              -- that it is handled
                              event:Skip()
                          end
                       end )
    
    frame:Show(wx.TRUE)
end

main()

wx.wxGetBaseApp():MainLoop()
