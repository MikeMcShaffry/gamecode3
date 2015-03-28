---------------------------------------------------------------------------
--  wxWindows-related utilities
---------------------------------------------------------------------------

-- First of all, issue an error if wxLua not present ("wx" not defined)

if not wx then
  print("# This program requires wxLua and must be launched as:\n"
     .. "#   wxLua <program_name>\n"
     .. "# wxLua can be downloaded (at the time of this writing) from:\n"
     .. "#   http://www.luascript.thersgb.net\n")
  os.exit(1)
end


-- Local data

WxUtils = {
  wxLastID = wx.wxID_HIGHEST  -- used by wxUserID
}


--
-- Generate an unused wxWindows user ID
--
-- return: new ID
--
function WxUtils.wxUserID()
  WxUtils.wxLastID = WxUtils.wxLastID + 1
  return WxUtils.wxLastID
end


--
-- Show an error message in a dialog window
--
-- in:
--   text = message to show
--
function WxUtils.ShowError(text)
  local dialog = wx.wxMessageDialog(wx.wxNull, text, "Error", 
    wx.wxOK + wx.wxICON_EXCLAMATION)
  dialog:ShowModal()
  dialog:Destroy() 
end


--
-- Create a numeric input field (wxTextCtrl) and its text label (wxStaticText),
-- show min, max limits in label, 
-- add label and field to a sizer
--
-- in:
--   dialog = dialog window
--   sizer = sizer to add label and field to, ignore if nil
--   desc = descriptive text for the label
--   vmin (opt) = minimum value, as integer or formatted text, ignore if nil
--   vmax (opt) = maximum value, as integer or formatted text, ignore if nil
--   default (opt) = default value, as integer or formatted text, empty if nil
-- return: label, field
--  
function WxUtils.NumericInput(dialog, sizer, desc, vmin, vmax, default)
  vmin = vmin or "" -- null string if not given
  vmax = vmax or ""
  default = default or ""
  -- show range if at least ne limit given
  if vmin or vmax then
    desc = desc .. " (" .. vmin .. "..." .. vmax .. ")"
  end
  -- create label and text input field
  local dp = wx.wxPoint(-1, -1) -- default position  
  local label = wx.wxStaticText(dialog, -1, desc, dp)
  local field = wx.wxTextCtrl(dialog, -1, default, dp)
  -- add both to sizer, if any
  if sizer then 
    sizer:AddWindow(label, 0, wx.wxALIGN_CENTER)
    sizer:AddWindow(field)
  end
  return label, field
end


--
-- Check if a field value is between given limits
--
-- in:
--   field = value field (wxStaticText)
--   vmin (opt) = minimum allowed value, ignore if nil
--   vmax (opt) = maximum allowed value, ignore if nil
--
function WxUtils.CheckFieldValue(field, vmin, vmax)
  local errMsg = nil -- suppose all OK
  -- read the value from the field
  local value = tonumber(field:GetValue())
  if not value then
    errMsg = "Non-numeric value in selected field"
  elseif (vmin and value < vmin) or (vmax and value > vmax) then
    errMsg = "Selected value is out of range"
  end
  -- in case of error, show message and select offending field
  if errMsg then 
    WxUtils.ShowError(errMsg)
    field:SetFocus()
    field:SetSelection(-1, -1)
    field:SetInsertionPointEnd()
  end
  return (not errMsg) -- (true = OK)
end


--
-- Create an horizontal sizer with OK, Cancel buttons,
-- call a function when OK is pressed
--
-- in:
--   dialog = dialog window
--   focus = button to set focus on (wx.wxID_OK or wx.wxID_CANCEL), if any
--   okFunc = function called when OK is pressed,
--     in: dialog
--     return: true to close dialog
-- return: sizer
--
function WxUtils.OkCancelSizer(dialog, focus, okFunc)
  -- create buttons
  local cancelButton = wx.wxButton(dialog, wx.wxID_CANCEL, "Cancel")
  local okButton = wx.wxButton(dialog, wx.wxID_OK, "OK")
  -- create sizer, align to the right
  local sizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
  sizer:Add(1, 1, 1) -- set buttons to the right using stretchable space
  sizer:AddWindow(cancelButton)
  sizer:AddWindow(okButton, 0, wx.wxLEFT, 10)
  -- button handlers
  local evType = wx.wxEVT_COMMAND_BUTTON_CLICKED
  dialog:ConnectEvent(wx.wxID_CANCEL, evType, 
    function(ev) dialog:EndModal(wx.wxID_CANCEL) end)
  dialog:ConnectEvent(wx.wxID_OK, evType, 
    function(ev) 
      if okFunc(dialog) then dialog:EndModal(wx.wxID_OK) end 
    end)
  -- set focus
  if focus == wx.wxID_OK then 
    okButton:SetFocus()
  elseif focus == wx.wxID_CANCEL then 
    cancelButton:SetFocus()
  end
  return sizer
end

