-------------------------------------------------------------------------=---
-- Name:        BmpBtn.wx.lua
-- Purpose:     Bitmapbutton wxLua sample
-- Author:      J Winwood
-- Created:     February 2002
-- Copyright:   (c) 2002 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---
require "wxLua"


-- You can initialize all the handlers with the following:

if not wx.wxNullImage then
    wx.wxInitAllImageHandlers()
else
-- or specific ones with the following
-- note the use of a predefined object for self.
    wx.wxNullImage:AddHandler(wx.wxJPEGHandler())
    wx.wxNullImage:AddHandler(wx.wxGIFHandler())
end    

frame = wx.wxFrame(wx.wxNull, 
                   -1, 
                   "Testing on "..wx.wxGetOsDescription(), 
                   wx.wxPoint(100, 100), 
                   wx.wxSize(420, 150), 
                   wx.wxDEFAULT_FRAME_STYLE )
      
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

local filePath = getPath()

bitmap1 = wx.wxBitmapFromFile(filePath.."/bitmaps/claw_small.jpg", 
                              wx.wxBITMAP_TYPE_JPEG ) 
button1 = wx.wxBitmapButton(frame, 
                            -1, 
                            bitmap1, 
                            wx.wxPoint(5, 5), 
                            wx.wxSize(100, 100))

bitmap2 = wx.wxBitmapFromFile(filePath.."/bitmaps/layout.gif", 
                                 wx.wxBITMAP_TYPE_GIF ) 
button2 = wx.wxBitmapButton(frame, 
                            -1, 
                            bitmap2, 
                            wx.wxPoint(110, 5), 
                            wx.wxSize(100, 100))
                         
bitmap3 = wx.wxBitmapFromFile(filePath.."/bitmaps/copy.bmp", 
                              wx.wxBITMAP_TYPE_BMP ) 
button3 = wx.wxBitmapButton(frame, 
                            -1, 
                            bitmap3, 
                            wx.wxPoint(215, 5), 
                            wx.wxSize(100, 100))

frame:Show(wx.TRUE)

-- a few other tests output to the debug window or stdout.

rect = wx.wxRect(10,20,60,80)
width = rect.Width
height = rect.Height
rect.Width = 45
rect.Height = 55
print("Rect ", width, height, rect.Width, rect.Height, rect:GetWidth(), rect:GetHeight())

print("OS description", wx.wxGetOsDescription())

version, major, minor = wx.wxGetOsVersion()
print("OS Version", version, major, minor)

point = wx.wxPoint(10, 20)
x, y = point:GetXY()
print("Point x, y", x, y)

x, y = wx.wxDisplaySize()
print("Display Size x, y", x, y)

if wx.wxDisplaySizeMM then
    x, y = wx.wxDisplaySizeMM()
    print("Display Size x, y", x, y, "mm")
end    

now = wx.wxNow()
print("Now ", now)

dt = wx.wxDateTime()
dt:ParseDate("20 Nov 1999")
print(dt:Format("wxDateTime test : %d:%m:%y"))

if wx.wxClientDisplayRect then
    x, y, width, height = wx.wxClientDisplayRect()
    print("Client Display Rect", x, y, width, height)
end    

x, y, descent, externalLeading = frame:GetTextExtent("Text Extent")
print("Text Extent", x, y, descent, externalLeading)

wx.Run()

