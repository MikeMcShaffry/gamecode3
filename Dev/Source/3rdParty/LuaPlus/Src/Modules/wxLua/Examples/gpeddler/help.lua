
---------------------------------------------------------------------------
--  HelpFrame
---------------------------------------------------------------------------

-- this is not a 'class', it handles a single HTML help frame

HelpFrame = {
  frameWin = nil,   -- frame to show help page into
  defaultSize = wx.wxSize(540, 380)
}


--
-- Show HTML help in a help frame, create frame if not existing,
-- move frame to the front if already existing
--
-- in:
--   parent = parent window
--   pagefile = file containing page to be shown
--   minSize (opt) = minimum HTML window size as wx.wxSize, defaultSize if nil
--
function HelpFrame.ShowHtml(parent, pagefile, minSize)
  if HelpFrame.frameWin then
    -- already existing, move to front
    HelpFrame.frameWin:Raise()
  else
    HelpFrame.CreateNew(parent, pagefile, minSize)
  end
end


--
-- Create a help frame with an HTML window, show page in it,
-- the frame will be closed by the user (frameWin will be set to nil)
--
-- in:
--   parent = parent window
--   pagefile = file containing page to be shown
--   minSize (opt) = minimum HTML window size as wx.wxSize, defaultSize if nil
--
function HelpFrame.CreateNew(parent, pagefile, minSize)
  local defaultPos = wx.wxPoint(-1, -1)
  local minHtmlSize = minSize or HelpFrame.defaultSize
  -- create the frame window, will be closed when the parent closes
  local frameWin = wx.wxFrame(parent, -1, "About gpeddler 2", defaultPos)
  -- create the html window inside it
  local htmlWin = wx.wxHtmlWindow(frameWin, -1, defaultPos, minHtmlSize)
  -- add sizer with centered Close button
  local closeButton = wx.wxButton(frameWin, wx.wxID_CLOSE, "Close")
  local bSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
  bSizer:Add(1, 1, 1); bSizer:AddWindow(closeButton); bSizer:Add(1, 1, 1)
  -- button handler: close the help frame
  frameWin:ConnectEvent(wx.wxID_CLOSE, wx.wxEVT_COMMAND_BUTTON_CLICKED, 
    function(ev) 
      frameWin:Close(wx.TRUE) 
    end)
  -- sizer to hold it all
  local vSizer = wx.wxBoxSizer(wx.wxVERTICAL)
  vSizer:AddWindow(htmlWin, 1, wx.wxGROW)
  vSizer:AddSizer(bSizer, 0, wx.wxALL + wx.wxGROW, 10)
  -- apply layout
  frameWin:SetSizer(vSizer)
  vSizer:Fit(frameWin)
  -- load the requested HTML page
  htmlWin:LoadPage(pagefile)
  -- show the frame window, set focus to html page
  frameWin:Show(wx.TRUE)
  htmlWin:SetFocus()
  -- remember that frame is active now, will be inactive when closed
  HelpFrame.frameWin = frameWin
  frameWin:ConnectEvent(wx.wxEVT_CLOSE_WINDOW, 
    function(ev) 
      HelpFrame.frameWin = nil
      ev:Skip() 
    end)
end

