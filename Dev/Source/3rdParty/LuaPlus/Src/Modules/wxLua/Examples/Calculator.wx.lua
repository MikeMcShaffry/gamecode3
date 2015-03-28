-------------------------------------------------------------------------=---
-- Name:        Calculator.wx.lua
-- Purpose:     Calculator wxLua sample
-- Author:      J Winwood
--              Based on the wxWindows sample by Marco Ghislanzoni
-- Created:     March 2002
-- Updated      January 2003 to use XML resources
-- Copyright:   (c) 2002-2003 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---

require "wxLua"

ID_0        = 102
ID_1        = 103
ID_2        = 104
ID_3        = 105
ID_4        = 106
ID_5        = 107
ID_6        = 108
ID_7        = 109
ID_8        = 110
ID_9        = 111
ID_DECIMAL  = 112
ID_EQUALS   = 113
ID_PLUS     = 114
ID_MINUS    = 115
ID_MULTIPLY = 116
ID_DIVIDE   = 117
ID_CLEAR    = 118 
ID_OFF      = 119

local xpmdata = 
{
    "16 16 6 1",
    "   s None  c None",
    ".  c #000000",
    "+  c #c0c0c0",
    "@  c #808080",
    "#  c #ffff00",
    "$  c #ffffff",
    "                ",
    "   @@@@@        ",
    "  @#+#+#@       ",
    " @#+#+#+#@@@@@@ ",
    " @$$$$$$$$$$$$@.",
    " @$#+#+#+#+#+#@.",
    " @$+#+#+@+#+#+@.",
    " @$#+#+@+@+#+#@.",
    " @$+#+@+#+@+#+@.",
    " @$#+@+#+#+@+#@.",
    " @$+#+#+#+#+#+@.",
    " @$#+#+#+#+#+#@.",
    " @@@@@@@@@@@@@@.",
    "  ..............",
    "                ",
    "                "
}

bitmap = wx.wxBitmapFromXPMData(xpmdata)
icon = wx.wxDefaultIcon()
icon:CopyFromBitmap(bitmap)

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

dialog     = nil
res        = nil
txtDisplay = nil

-- old style resources (if present)
if wx.wxResourceParseFile then
    wx.wxResourceParseFile(getPath().."/Calculator.wxr")

    -- Create main dialog using default constructor
    dialog = wx.wxEmptyDialog()
    if dialog:LoadFromResource(wx.wxNull, "dialog1") == wx.FALSE then
        wx.wxMessageBox("Error loading wxr resources!", 
                        "Calculator",
                        wx.wxOK + wx.wxICON_EXCLAMATION, 
                        dialog)
        wx.wxExit(0)
    end                     
    txtDisplay  = wx.wxFindWindowByName("statictext1", dialog);
    if not txtDisplay then
        wx.wxError([[Unable to find window "statictext1" in the dialog]])
    end
else
-- xml style resources (if present)
    res = wx.wxXmlResourceGetDefault()
    res:InitAllHandlers()
    res:Load(getPath().."/Calculator.xrc")
    dialog = wx.wxEmptyDialog()
    if res:LoadDialogCreate(dialog, wx.wxNull, "dialog1") == wx.FALSE then
        wx.wxMessageBox("Error loading xrc resources!", 
                        "Calculator",
                        wx.wxOK + wx.wxICON_EXCLAMATION, 
                        wx.wxNull)
        wx.wxExit(0)
    end
    txtDisplay = dialog:FindWindow(res:GetXRCID("statictext1"))
    if not txtDisplay then
        wx.wxError([[Unable to find window "statictext1" in the dialog]])
    end    
    ID_0        = res:GetXRCID("button0")
    ID_1        = res:GetXRCID("button1")
    ID_2        = res:GetXRCID("button2")
    ID_3        = res:GetXRCID("button3")
    ID_4        = res:GetXRCID("button4")
    ID_5        = res:GetXRCID("button5")
    ID_6        = res:GetXRCID("button6")
    ID_7        = res:GetXRCID("button7")
    ID_8        = res:GetXRCID("button8")
    ID_9        = res:GetXRCID("button9")
    ID_DECIMAL  = res:GetXRCID("button_decimal")
    ID_EQUALS   = res:GetXRCID("button_equals")
    ID_PLUS     = res:GetXRCID("button_plus")
    ID_MINUS    = res:GetXRCID("button_minus")
    ID_MULTIPLY = res:GetXRCID("button_multiply")
    ID_DIVIDE   = res:GetXRCID("button_divide")
    ID_OFF      = res:GetXRCID("button_off")
    ID_CLEAR    = res:GetXRCID("button_clear")
end

dialog:SetIcon(icon)

if not txtDisplay:DynamicCast("wxStaticText") then
    wx.wxError([[window "statictext1" is not a "wxStaticText" or is not derived from it"]])
end

txtDisplay:SetLabel("0")

lastNumber       = 0
lastOperation    = ID_PLUS
clearDisplay     = nil

function OnClear(event)
	txtDisplay:SetLabel("0")
	lastNumber     = 0
	lastOperation  = ID_PLUS
end

function OnNumber(event)
	local number        = event:GetId()
	local displayString = txtDisplay:GetLabel()

	if displayString == "0" or clearDisplay then
		displayString = ""
    end
	clearDisplay = nil

	-- Limit string length to 12 chars
	if string.len(displayString) < 12 then
		if number == ID_DECIMAL then
			-- If the first pressed char is "." then we want "0."
			if string.len(displayString) == 0 then
				displayString = displayString.."0."
            else
				displayString = displayString.."."
            end
		else
            number = number - ID_0
			-- If first character entered is 0 we reject it
			if number == 0 and string.len(displayString) == 0 then
				displayString = "0"
			elseif displayString == "" then
                displayString = number
            else
				displayString = displayString..number
			end
		end

		txtDisplay:SetLabel(displayString)
	end
end

function DoOperation(a, b, operation)
    local result = a
    if operation == ID_PLUS then
        result =  b + a
    elseif operation == ID_MINUS then
        result =  b - a
    elseif operation == ID_MULTIPLY then
        result = b * a
    elseif operation == ID_DIVIDE then
        if a == 0 then
            result = "Error"
        else
            result = b / a
        end
    end
	return result
end

function OnOperator(event)	
	-- Get display content
	local displayString    = txtDisplay:GetLabel()
	local currentOperation = tonumber(displayString)

	-- Get the required lastOperation
	local operation = event:GetId()

    displayString = DoOperation(currentOperation, lastNumber, lastOperation)
    lastNumber    = tonumber(displayString)
    
    if lastOperation ~= ID_EQUALS or operation == ID_EQUALS then
        txtDisplay:SetLabel(displayString)
    end
    clearDisplay  = 1
    lastOperation = operation
end

function OnQuit(event)
	-- Hide main dialog
	dialog:Show(wx.FALSE)

    wx.wxMessageBox("Calculator sample based on the calc sample written by Marco Ghislanzoni", 
                       "Calculator", 
                       wx.wxOK + wx.wxICON_INFORMATION)

    dialog:Destroy()
end

dialog:ConnectEvent(ID_0,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_1,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_2,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_3,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_4,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_5,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_6,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_7,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_8,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_9,        wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_DECIMAL,  wx.wxEVT_COMMAND_BUTTON_CLICKED, OnNumber)
dialog:ConnectEvent(ID_EQUALS,   wx.wxEVT_COMMAND_BUTTON_CLICKED, OnOperator)
dialog:ConnectEvent(ID_PLUS,     wx.wxEVT_COMMAND_BUTTON_CLICKED, OnOperator)
dialog:ConnectEvent(ID_MINUS,    wx.wxEVT_COMMAND_BUTTON_CLICKED, OnOperator)
dialog:ConnectEvent(ID_MULTIPLY, wx.wxEVT_COMMAND_BUTTON_CLICKED, OnOperator)
dialog:ConnectEvent(ID_DIVIDE,   wx.wxEVT_COMMAND_BUTTON_CLICKED, OnOperator)
dialog:ConnectEvent(ID_OFF,      wx.wxEVT_COMMAND_BUTTON_CLICKED, OnQuit) 
dialog:ConnectEvent(ID_CLEAR,    wx.wxEVT_COMMAND_BUTTON_CLICKED, OnClear)

dialog:ConnectEvent(wx.wxEVT_CLOSE_WINDOW, function(event)
                                                 OnQuit(event)    
                                                 event:Skip()
                                              end)

dialog:Centre()
dialog:Show(wx.TRUE)

accelTable = wx.wxAcceleratorTable{
                                     { wx.wxACCEL_NORMAL, string.byte('0'),            ID_0        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD0,          ID_0        },
                                     { wx.wxACCEL_NORMAL, string.byte('1'),            ID_1        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD1,          ID_1        },
                                     { wx.wxACCEL_NORMAL, string.byte('2'),            ID_2        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD2,          ID_2        },
                                     { wx.wxACCEL_NORMAL, string.byte('3'),            ID_3        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD3,          ID_3        },
                                     { wx.wxACCEL_NORMAL, string.byte('4'),            ID_4        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD4,          ID_4        },
                                     { wx.wxACCEL_NORMAL, string.byte('5'),            ID_5        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD5,          ID_5        },
                                     { wx.wxACCEL_NORMAL, string.byte('6'),            ID_6        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD6,          ID_6        },
                                     { wx.wxACCEL_NORMAL, string.byte('7'),            ID_7        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD7,          ID_7        },
                                     { wx.wxACCEL_NORMAL, string.byte('8'),            ID_8        },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD8,          ID_8        },
                                     { wx.wxACCEL_NORMAL, string.byte('9'),            ID_9        },                                      
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD9,          ID_9        },
                                     { wx.wxACCEL_NORMAL, string.byte('.'),            ID_DECIMAL  },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD_DECIMAL,   ID_DECIMAL  },
                                     { wx.wxACCEL_NORMAL, string.byte('='),            ID_EQUALS   },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD_ENTER,     ID_EQUALS   },
                                     { wx.wxACCEL_NORMAL, 13,                      ID_EQUALS   },
                                     { wx.wxACCEL_NORMAL, string.byte('+'),            ID_PLUS     },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD_ADD,       ID_PLUS     },
                                     { wx.wxACCEL_NORMAL, string.byte('-'),            ID_MINUS    },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD_SUBTRACT,  ID_MINUS    },
                                     { wx.wxACCEL_NORMAL, string.byte('*'),            ID_MULTIPLY },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD_MULTIPLY,  ID_MULTIPLY },
                                     { wx.wxACCEL_NORMAL, string.byte('/'),            ID_DIVIDE   },
                                     { wx.wxACCEL_NORMAL, wx.VXK_NUMPAD_DIVIDE,    ID_DIVIDE   },
                                     { wx.wxACCEL_NORMAL, string.byte('C'),            ID_CLEAR    },
                                     { wx.wxACCEL_NORMAL, string.byte('c'),            ID_CLEAR    },
                                     { wx.wxACCEL_NORMAL, wx.WXK_ESCAPE,           ID_OFF      }
                               }

dialog:SetAcceleratorTable(accelTable)

wx.Run()
