-------------------------------------------------------------------------=---
-- Name:        XrcDlg.wx.lua
-- Purpose:     Xml Dialog wxLua sample
-- Author:      J Winwood
-- Created:     January 2003
-- Copyright:   (c) 2003 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---

require "wxLua"

res   = nil
frame = nil

function derivedDialog()
   local dlg = res:CreateDialog(frame, "derived_dialog")
   
   dlg:ConnectEvent(res:GetXRCID("my_button"), 
                    wx.wxEVT_COMMAND_BUTTON_CLICKED, 
                    function (event)
                        local msgDlg = wx.wxMessageDialog(dlg, "You clicked on My Button")    
                        msgDlg:ShowModal()                    
                    end )
                    
   dlg:ConnectEvent(res:GetXRCID("my_checkbox"), 
                    wx.wxEVT_UPDATE_UI, 
                    function (event)
                        local isChecked = dlg:FindWindow(res:GetXRCID("my_checkbox")):DynamicCast("wxCheckBox"):IsChecked()
                        dlg:FindWindow(res:GetXRCID("my_textctrl")):DynamicCast("wxTextCtrl"):Enable(isChecked)                        
                    end )
                    
   dlg:ConnectEvent(wx.wxID_OK,
                    wx.wxEVT_COMMAND_BUTTON_CLICKED, 
                    function (event)
                        local msgDlg = wx.wxMessageDialog(dlg, 
                                                          "Press OK to close Derived dialog, or Cancel to abort",
                                                          "Overriding base class OK button handler",
                                                          wx.wxOK + wx.wxCANCEL + wx.wxCENTER)
    
                        if msgDlg:ShowModal() == wx.wxID_OK then
                            dlg:EndModal( wx.wxID_OK )
                        end
                    end )
   dlg:ShowModal()
end

-- return the path part of the currently executing file
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

    res = wx.wxXmlResourceGetDefault()
    res:InitAllHandlers()
      
    local path = getPath().."/"
    res:Load(path.."rc/menu.xrc")
    res:Load(path.."rc/toolbar.xrc")
    res:Load(path.."rc/basicdlg.xrc")
    res:Load(path.."rc/derivdlg.xrc")
    res:Load(path.."rc/controls.xrc")
    res:Load(path.."rc/frame.xrc")
    res:Load(path.."rc/uncenter.xrc")
    res:Load(path.."rc/artprov.xrc")
    res:Load(path.."rc/platform.xrc")
    res:Load(path.."rc/variable.xrc")
    res:Load(path.."rc/custclas.xrc")
        
--  res:SetVariable( "version", "2.4.0")        
    
-- use this form     
    frame = wx.wxFrameDefault()
    res:LoadFrame(frame, wx.wxNull, "main_frame")

-- alternatively use this
--  frame = res:CreateFrame(wx.wxNull, "main_frame")
    
    frame:SetMenuBar(res:LoadMenuBar("main_menu"))
    frame:SetToolBar(res:LoadToolBar(frame, "main_toolbar"))
    
    frame:CreateStatusBar( 1 )
    frame:SetStatusText("Welcome to wxLua.")
    
-- connect the selection event of the exit menu item to an
-- event handler that closes the window
    frame:ConnectEvent(res:GetXRCID("exit_tool_or_menuitem"), 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                          frame:Close(TRUE)
                       end )
    
    frame:ConnectEvent(res:GetXRCID("non_derived_dialog_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                           local dlg = res:CreateDialog(frame, "non_derived_dialog")
                           dlg:ShowModal()
                       end )
                       
    frame:ConnectEvent(res:GetXRCID("derived_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       derivedDialog)
                       
    frame:ConnectEvent(res:GetXRCID("controls_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED,                        
                       function (event)
                            local dlg = res:CreateDialog(frame, "controls_dialog")
                            
                            local listctrl = dlg:FindWindow(res:GetXRCID("controls_listctrl")):DynamicCast("wxListCtrl")
                            listctrl:InsertColumnInfo(0, "Name", wx.wxLIST_FORMAT_LEFT, 200)
                            listctrl:InsertStringItem(0, "Todd Hope")
                            listctrl:InsertStringItem(1, "Kim Wynd")
                            listctrl:InsertStringItem(2, "Leon Li")

                            local treectrl = dlg:FindWindow(res:GetXRCID("controls_treectrl")):DynamicCast("wxTreeCtrl")                            
                            treectrl:AddRoot("Godfather")
                            treectrl:AppendItem(treectrl:GetRootItem(), "Evil henchman 1")
                            treectrl:AppendItem(treectrl:GetRootItem(), "Evil henchman 2")
                            treectrl:AppendItem(treectrl:GetRootItem(), "Evil accountant")
                            dlg:ShowModal()
                       end )
                       
    frame:ConnectEvent(res:GetXRCID("uncentered_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                            local dlg = res:CreateDialog(frame, "uncentered_dialog")
                            dlg:ShowModal()                          
                       end )
                       
    frame:ConnectEvent(res:GetXRCID("custom_class_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                            local dlg = res:CreateDialog(frame, "custom_class_dialog")
                            local listctrl = wx.wxListCtrl(dlg, 
                                                           -1, 
                                                           wx.wxDefaultPosition, 
                                                           wx.wxDefaultSize, 
                                                           wx.wxLC_REPORT, 
                                                           wx.wxDefaultValidator)
                            res:AttachUnknownControl("custom_control_placeholder", 
                                                     listctrl);
                            listctrl:InsertColumnInfo(0, "Name", wx.wxLIST_FORMAT_LEFT, 200)
                            listctrl:InsertColumnInfo(1, "Address", wx.wxLIST_FORMAT_LEFT, 200)
                            listctrl:InsertStringItem(0, "Todd Hope")
                            listctrl:InsertStringItem(1, "Kim Wynd")
                            listctrl:InsertStringItem(2, "Leon Li")
                            dlg:ShowModal()                          
                       end )
                       
    frame:ConnectEvent(res:GetXRCID("platform_property_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                            local dlg = res:CreateDialog(frame, "platform_property_dialog")
                            dlg:ShowModal()                          
                       end )
                       
    frame:ConnectEvent(res:GetXRCID("art_provider_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                            local dlg = res:CreateDialog(frame, "art_provider_dialog")
                            dlg:ShowModal()                                                    
                       end )
                       
    frame:ConnectEvent(res:GetXRCID("variable_expansion_tool_or_menuitem"),
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                            local dlg = res:CreateDialog(frame, "variable_expansion_dialog")
                            dlg:ShowModal()                                                                              
                       end )
    
-- connect the selection event of the about menu item
    frame:ConnectEvent(res:GetXRCID("about_tool_or_menuitem"), 
                       wx.wxEVT_COMMAND_MENU_SELECTED, 
                       function (event)
                          wx.wxMessageBox( 'This is the "About" dialog of the wxLua XML resource sample.', 
                                           "About wxLua", 
                                           wx.wxOK + wx.wxICON_INFORMATION, 
                                           frame )
                       end )
        
-- show the frame window
    frame:Show(wx.TRUE)
end

main()

wx.Run()
