-------------------------------------------------------------------------=---
-- Name:        Dialog.wx.lua
-- Purpose:     Dialog wxLua sample
--              Based on the C++ version by Marco Ghislanzoni
-- Author:      J Winwood
-- Created:     March 2002
-- Copyright:   (c) 2001 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---

-- IDs of the controls in the dialog
local Dialog_CelsiusToFahrenheit  = 1
local Dialog_FahrenheitToCelsius  = 2
local Dialog_About                = 3
local Dialog_Close                = 4  
local Dialog_CelsiusDegrees       = 5
local Dialog_FahrenheitDegrees    = 6
local Dialog_CelsiusText          = 7
local Dialog_FahrenheitText       = 8
local Dialog_StaticBox            = 9

-- Create the dialog
local dialog = wx.wxDialog(wx.wxNull, 
                           -1, 
                           "Temperature Converter", 
                           wx.wxPoint(0, 0), 
                           wx.wxSize(147, 162))

-- Create the controls
local statBox                = wx.wxStaticBox (dialog,  Dialog_StaticBox,            "Convert",     wx.wxPoint(5,   0),   wx.wxSize(130, 100))
local celsiusText            = wx.wxStaticText(dialog,  Dialog_CelsiusText,          "Celsius:",    wx.wxPoint(10,  15))
local celsiusDegree          = wx.wxTextCtrl  (dialog,  Dialog_CelsiusDegree,        nil,           wx.wxPoint(10,  30),  wx.wxSize(50,   20))
local fahrenheitText         = wx.wxStaticText(dialog,  Dialog_FahrenheitText,       "Fahrenheit:", wx.wxPoint(10,  55))
local fahrenheitDegree       = wx.wxTextCtrl  (dialog,  Dialog_FahrenheitDegree,     nil,           wx.wxPoint(10,  70),  wx.wxSize(50,   20))
local btnCelsiusToFahrenheit = wx.wxButton    (dialog,  Dialog_CelsiusToFahrenheit,  "C -> F",      wx.wxPoint(80,  30),  wx.wxSize(50,   20))
local btnFahrenheitToCelsius = wx.wxButton    (dialog,  Dialog_FahrenheitToCelsius,  "F -> C",      wx.wxPoint(80,  70),  wx.wxSize(50,   20))
local btnAbout               = wx.wxButton    (dialog,  Dialog_About,                "About",       wx.wxPoint(10, 110),  wx.wxSize(50,   20))
local btnClose               = wx.wxButton    (dialog,  Dialog_Close,                "Close",       wx.wxPoint(80, 110),  wx.wxSize(50,   20))

-- Attach an event handler to the CelsiusToFahrenheit button
dialog:ConnectEvent(Dialog_CelsiusToFahrenheit, 
                    wx.wxEVT_COMMAND_BUTTON_CLICKED, 
                    function(event)
                        local celsiusString = celsiusDegree:GetValue()
                        if celsiusString == "" then
                            wx.wxMessageBox("The Celsius entry is invalid", 
                                            "Error!", 
                                            wx.wxOK + wx.wxICON_EXCLAMATION + wx.wxCENTRE, 
                                            dialog)
                        else
                            local fahrenheitValue = (tonumber(celsiusString) * 9 / 5) + 32                                
                            fahrenheitDegree:SetValue(fahrenheitValue)
                        end
                    end)

-- Attach an event handler to the FahrenheitToCelsius button
dialog:ConnectEvent(Dialog_FahrenheitToCelsius, 
                    wx.wxEVT_COMMAND_BUTTON_CLICKED, 
                    function(event)   
                        local fahrenheitString = fahrenheitDegree:GetValue()
                        if fahrenheitString == "" then
                            wx.wxMessageBox("The fahrenheit entry is invalid!", 
                                            "Error!", 
                                            wx.wxOK + wx.wxICON_EXCLAMATION + wx.wxCENTRE,
                                            dialog)
                        else
                            local celsiusValue = (tonumber(fahrenheitString) - 32) * 5 / 9
                            celsiusDegree:SetValue(celsiusValue)
                        end
                    end)

-- Attach an event handler to the Close button
dialog:ConnectEvent(Dialog_Close, 
                    wx.wxEVT_COMMAND_BUTTON_CLICKED, 
                    function(event)
                        dialog:Destroy()
                    end)

dialog:ConnectEvent(wx.wxEVT_CLOSE_WINDOW,
                   function (event)
                        dialog:Destroy()
                        event:Skip()
                   end)
                    
-- Attach an event handler to the About button
dialog:ConnectEvent(Dialog_About, 
                    wx.wxEVT_COMMAND_BUTTON_CLICKED, 
                    function(event)
                        wx.wxMessageBox("Based on the C++ version by Marco Ghislanzoni", 
                                        "Temperature converter",     
                                        wx.wxOK + wx.wxICON_INFORMATION, 
                                        dialog)
                    end)

-- Centre the dialog on the screen                    
dialog:Centre()
-- Show the dialog
dialog:Show(wx.TRUE)
