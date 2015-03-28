
-----------------------------------------------------------------------------
-- gpeddler 2
--
-- (C) 2003 by Enrico Colombini <erix@erix.it>
-- this program is distributed under the wxWindows license
-----------------------------------------------------------------------------

require "wxLua"

gpVersion = "gpeddler 2.0"
gpDate = "September 2003"


-- include other modules

dofile("utils.lua")
dofile("wxutils.lua")
dofile("route.lua")
dofile("solver.lua")
dofile("options.lua")
dofile("help.lua")


-----------------------------------------------------------------------------
-- MainFrame
-----------------------------------------------------------------------------

MainFrame = {
  -- interface
  window = nil,      -- main window
  area1 = nil,       -- drawing area for solvers
  area2 = nil,
  box1 = nil,        -- static boxes
  box2 = nil,
  stat1 = nil,       -- info text for solvers
  stat2 = nil,
  startButton = nil, -- buttons
  stopButton = nil,
  newRouteButton = nil,
  optionsButton = nil,
  -- IDs for menu/button commands
  startID = WxUtils.wxUserID(),
  stopID = WxUtils.wxUserID(),
  newRouteID = WxUtils.wxUserID(),
  optionsID = WxUtils.wxUserID(),
  -- application data
  solver1 = nil,   -- the exhaustive solver
  solver2 = nil,   -- the evolutionary solver
  options = nil,   -- solving options
  baseRoute = nil, -- initial route for both solvers
}


--
-- Create a MainFrame
--
-- return: new MainFrame
--
function MainFrame.Create()
  local self = Utils.ShallowCopy(MainFrame)
  local defaultPos = wx.wxPoint(-1, -1)
  local minAreaSize = wx.wxSize(300, 300)
  -- create the frame window
  self.window = wx.wxFrame(wx.wxNull, -1, gpVersion, defaultPos)
  -- create interface elements
  local win = self.window -- for convenience
  self.box1 = wx.wxStaticBox(win, -1, "Exhaustive solver - best route")
  self.box2 = wx.wxStaticBox(win, -1, "Evolutionary solver - best route")
  self.area1 = wx.wxWindow(win, -1, defaultPos, minAreaSize)
  self.area2 = wx.wxWindow(win, -1, defaultPos, minAreaSize)
  self.stat1 = wx.wxStaticText(win, -1, "Stat: ", defaultPos)
  self.stat2 = wx.wxStaticText(win, -1, "Stat: ", defaultPos)
  -- create buttons, menus
  self:SetupButtons()
  self:SetupMenus()
  -- build layout, show the frame window
  self:SetupLayout()
  win:CenterOnScreen() -- seems to be needed to keep window within screen
  win:Show(wx.TRUE)
  return self
end


-----------------------------------------------------------------------------
-- MainFrame (interface definition)
-----------------------------------------------------------------------------

--
-- Create buttons, connect them to handler functions
--
function MainFrame:SetupButtons()
  local win = self.window
  self.startButton = wx.wxButton(win, self.startID, "Start")
  self.stopButton = wx.wxButton(win, self.stopID, "Stop")
  self.newRouteButton = wx.wxButton(win, self.newRouteID, "New route")
  self.optionsButton = wx.wxButton(win, self.optionsID, "Options")
  -- connect button events
  local evType = wx.wxEVT_COMMAND_BUTTON_CLICKED
  win:ConnectEvent(self.startID, evType, function(ev) self:Start() end)
  win:ConnectEvent(self.stopID, evType, function(ev) self:Stop() end)
  win:ConnectEvent(self.newRouteID, evType, function(ev) self:NewRoute() end)
  win:ConnectEvent(self.optionsID, evType, function(ev) self:Options() end)
end


--
-- Create menus, connect them to handler functions
--
function MainFrame:SetupMenus()
  local fileMenu = wx.wxMenu()
  fileMenu:Append(wx.wxID_EXIT, "E&xit")
  local helpMenu = wx.wxMenu()
  helpMenu:Append(wx.wxID_ABOUT, "&About gpeddler2")
  local menuBar = wx.wxMenuBar()
  menuBar:Append(fileMenu, "&File")
  menuBar:Append(helpMenu, "&Help")
  self.window:SetMenuBar(menuBar)
  -- connect menu events
  local evType = wx.wxEVT_COMMAND_MENU_SELECTED
  self.window:ConnectEvent(wx.wxID_EXIT, evType, function(ev) self:Exit() end)
  self.window:ConnectEvent(wx.wxID_ABOUT, evType, function(ev) self:About() end)
end


--
-- Build frame layout using sizers
--
function MainFrame:SetupLayout()
  -- sizers for the two solvers
  local vSizer1 = wx.wxStaticBoxSizer(self.box1, wx.wxVERTICAL)
  vSizer1:AddWindow(self.area1, 1, wx.wxALL + wx.wxGROW, 10)
  vSizer1:AddWindow(self.stat1, 0, wx.wxALL, 10)
  local vSizer2 = wx.wxStaticBoxSizer(self.box2, wx.wxVERTICAL)
  vSizer2:AddWindow(self.area2, 1, wx.wxALL + wx.wxGROW, 10)
  vSizer2:AddWindow(self.stat2, 0, wx.wxALL, 10)
  -- sizer to keep both solvers side-to-side
  local hSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
  hSizer:AddSizer(vSizer1, 1, wx.wxGROW);
  hSizer:Add(10, 0, 0) -- spacing
  hSizer:AddSizer(vSizer2, 1, wx.wxGROW, 10);
  -- sizer for the buttons
  local bSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
  bSizer:Add(1, 1, 1) -- center buttons using stretchable space
  bSizer:AddWindow(self.startButton, 0, wx.wxLEFT + wx.wxRIGHT, 10)
  bSizer:AddWindow(self.stopButton, 0, wx.wxLEFT + wx.wxRIGHT, 10)
  bSizer:AddWindow(self.newRouteButton, 0, wx.wxLEFT + wx.wxRIGHT, 10)
  bSizer:AddWindow(self.optionsButton, 0, wx.wxLEFT + wx.wxRIGHT, 10)
  bSizer:Add(1, 1, 1)
	-- sizer to hold it all
  local mainSizer = wx.wxBoxSizer(wx.wxVERTICAL)
  mainSizer:AddSizer(hSizer, 1, wx.wxALL + wx.wxGROW, 10);
  mainSizer:AddSizer(bSizer, 0, wx.wxALL - wx.wxTOP + wx.wxGROW, 10);
  -- make layout work
  self.window:SetSizer(mainSizer)
  mainSizer:SetSizeHints(self.window)
  mainSizer:Fit(self.window)
end


-----------------------------------------------------------------------------
-- MainFrame (user actions)
-----------------------------------------------------------------------------

--
-- "Exit"
--
function MainFrame:Exit()
  -- Cleanup will be called automatically
  self.window:Close(wx.TRUE)
end


--
-- "Start"
--
function MainFrame:Start()
  local options = Options.Create()
  -- enable/disable interface elements
  self:UpdateInterface()
  -- re-assign the current route to both solvers
  self:SetBaseRoute(false)
  -- start solving coroutines usign the current set of options
  self.solver1:StartSolving(self.options)
  self.solver2:StartSolving(self.options)
end


--
-- "Stop"
--
function MainFrame:Stop()
  self.solver1:StopSolving()
  self.solver2:StopSolving()
  -- enable/disable interface elements
  self:UpdateInterface()
end


--
-- "New route"
--
function MainFrame:NewRoute()
  -- assign a new route to both solvers
  self:SetBaseRoute(true)
end


--
-- "Options"
--
function MainFrame:Options()
  -- remember current number of places
  local rsize = self.options.routeSize
  -- let user edit options
  local r = self.options:Edit(self.window)
  -- if number of places changed, re-generate a random route
  if r == wx.wxID_OK and self.options.routeSize ~= rsize then
    self:SetBaseRoute(true)
  end
end


--
-- "About" menu chosen
--
function MainFrame:About()
  -- create help window if not already visible, else move to front
  HelpFrame.ShowHtml(self.window, "help/index.html", wx.wxSize(700, 500))
end


-----------------------------------------------------------------------------
-- MainFrame (application code)
-----------------------------------------------------------------------------

--
-- Initialize the application
--
function MainFrame:Init()
  -- randomize
  math.randomseed(os.time())
  -- init image handlers (for use in html help window)
  wx.wxInitAllImageHandlers()
  -- create the two solvers
  self.solver1 = ExhaustiveSolver.Create(self.area1, self.stat1)
  self.solver2 = EvolutionarySolver.Create(self.area2, self.stat2)
  -- use a default set of options
  self.options = Options.Create()
  -- create a default baseRoute and assign it to solvers
  self:SetBaseRoute(true)
  -- enable/disable interface elements
  self:UpdateInterface()
  -- setup close handler (called before closing window)
  self.window:ConnectEvent(wx.wxEVT_CLOSE_WINDOW,
    function(ev)
      self:Cleanup()
      ev:Skip()
    end)
  -- setup idle handler
  self.window:ConnectEvent(-1, wx.wxEVT_IDLE, function(ev) self:OnIdle(ev) end)
end


--
-- Clean up before the main frame is closed
-- (called automatically via handler)
--
function MainFrame:Cleanup()
  -- be sure coroutines are finished
  self.solver1:StopSolving()
  self.solver2:StopSolving()
  -- help window will be closed automatically, if open
end


--
-- OnIdle: called while there are no waiting messages,
-- continue solving in progress (if any) by resuming coroutines
--
-- in:
--   ev = wx.wxIdleEvent
--
function MainFrame:OnIdle(ev)
  self.solver1:ContinueSolving() -- can be called even if not active
  self.solver2:ContinueSolving()
  -- enable/disable interface elements
  self:UpdateInterface()
  ev:RequestMore() -- call me again if still idle
end


--
-- Enable/disable interface elements according to status,
--
function MainFrame:UpdateInterface()
  local solving = self.solver1:IsSolving() or self.solver2:IsSolving()
  self.startButton:Enable(not solving)
  self.stopButton:Enable(solving)
  self.newRouteButton:Enable(not solving)
  self.optionsButton:Enable(not solving)
end


--
-- Set a new random baseRoute if requested,
-- assign baseRoute to both solvers,
-- use length given in options
--
-- in:
--   new = create a new random baseRoute if true (else reuse existing one)
--
function MainFrame:SetBaseRoute(new)
  if new then
    self.baseRoute = Route.Create(self.options.routeSize)
  end
  self.solver1:SetBaseRoute(self.baseRoute)
  self.solver2:SetBaseRoute(self.baseRoute)
end


-----------------------------------------------------------------------------
-- Program starts here
-----------------------------------------------------------------------------

-- create the main window
local main = MainFrame.Create()
-- initialize the application
main:Init()

wx.Run()
