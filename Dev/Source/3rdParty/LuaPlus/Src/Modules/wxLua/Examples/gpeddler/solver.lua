
---------------------------------------------------------------------------
--  Solver
---------------------------------------------------------------------------

Solver = {
  window = nil,        -- window to display the route graph on
  info = nil,          -- StaticText for statistics
  opt = nil,           -- solving options
  font = nil,          -- font used for id labels in graph
  bitmap = nil,        -- buffer bitmap for flicker-free drawing
  bestRoute = nil,     -- best route found so far, or initial route
  solverCo = nil,      -- solving coroutine
  -- Solve() defined in subclasses
}


--
-- Create a solver
--
-- in:
--   window = where to show the best route
--   infoText = StaticText to show statistics
-- return: new solver
--
function Solver.Create(window, infoText)
  local self = Utils.ShallowCopy(Solver)
  self.window = window
  self.window:ConnectEvent(wx.wxEVT_PAINT, function(ev) self:Repaint() end)
  self.info = infoText
  self.font = wx.wxFont(10, wx.wxSWISS, wx.wxNORMAL, wx.wxNORMAL, wx.wxFALSE)
  return self
end


--
-- Repaint solver window when requested by system, show best route
-- (use self.bitmap as buffer to avoid flickering)
--
function Solver:Repaint()
  local w, h = self.window:GetSize()  
  -- if window size has changed, discard the current buffer bitmap
  if self.bitmap 
  and (self.bitmap:GetWidth() ~= w or self.bitmap:GetHeight() ~= h) then
    self.bitmap:Delete()
    self.bitmap = nil
  end 
  -- if no buffer bitmap, create one
  if not self.bitmap then 
    self.bitmap = wx.wxEmptyBitmap(w, h)
  end
  -- if there is something to draw, draw it
  if self.bestRoute then
    -- first draw into the buffer bitmap
    local dc = wx.wxMemoryDC()
    dc:SelectObject(self.bitmap)
    self.bestRoute:Draw(dc, w, h, self.font)
    dc:Delete()
    -- then draw the bitmap into the window
    dc = wx.wxPaintDC(self.window)
    dc:DrawBitmap(self.bitmap, 0, 0, 0)
    dc:Delete()
  end
end


--
-- Set initial (and therefore best) route, 
-- as a copy of the given route (it could be modified while solving)
--
-- in:
--   route = route to set
--
function Solver:SetBaseRoute(route)
  self.bestRoute = Utils.ShallowCopy(route)
  -- request window redrawing with no background drawing
  self.window:Refresh(wx.FALSE)
end


--
-- Evaluate a route: 
-- if shorter than bestRoute, copy it to bestRoute and draw it
-- note: route can change after the call, so a ShallowCopy of it is made
--
-- in:
--   route = route to be evaluated
-- return: true if bestRoute changed
--
function Solver:EvaluateRoute(route)
  local change = (route.length < self.bestRoute.length)
  if change then
    self.bestRoute = Utils.ShallowCopy(route)
    -- request window redrawing with no background drawing
    self.window:Refresh(wx.FALSE)
  end
  return change
end


--
-- Start solving: create a coroutine, save it in self.solverCo, start it
--
-- in:
--   opt = solving options
--
function Solver:StartSolving(opt)
  if not self:IsSolving() then
    self.opt = opt
    self.solverCo = coroutine.create(self.Solve)
    local r, err = coroutine.resume(self.solverCo, self)
    if not r then print(err) end
    -- force first redrawing
    self.window:Refresh(wx.FALSE)
    self.info:SetLabel("Solving...")
  end
end


--
-- Continue solving: resume coroutine, unless already finished
--
function Solver:ContinueSolving()
  if self:IsSolving() then 
    local r, err = coroutine.resume(self.solverCo) -- 2nd arg = nil: continue 
    if not r then print(err) end
  end
end


--
-- Stop solving: wait for coroutine to terminate, unless already finished
--
function Solver:StopSolving()
  if self:IsSolving() then 
    -- ask coroutine to stop, wait for it to terminate
    while coroutine.status(self.solverCo) ~= "dead" do 
      local r, err = coroutine.resume(self.solverCo, true) -- 2nd arg passed to yield
      if not r then print(err) end
    end
  end
end


--
-- Check if solving in progress
--
-- return: true if solving coroutine is active
--
function Solver:IsSolving()
  return self.solverCo and coroutine.status(self.solverCo) ~= "dead"
end


--
-- Append termination cause to self.info static text
--
-- in: stopped = true if stopped, false if algorithm terminated
--
function Solver:AddTermInfo(stopped)
  local status
  if stopped then status = "(stopped)" else status = "(done)" end
  self.info:SetLabel(self.info:GetLabel() .. "  " .. status)
end


---------------------------------------------------------------------------
--  ExhaustiveSolver
---------------------------------------------------------------------------

ExhaustiveSolver = {
  nTried = 0   -- number of routes tried
}


--
-- Create an exhaustive solver, inheriting from Solver
--
-- in:
--   window = where to show the best route
--   infoText = StaticText to show statistics
-- return: new exhaustive solver
--
function ExhaustiveSolver.Create(window, infoText)
  local self = Solver.Create(window, infoText) -- inherit from Solver
  self = Utils.ShallowCopy(ExhaustiveSolver, self)
  return self
end


--
-- Solve by enumerating all possible solutions,
-- yield periodically to other coroutines, request exit if yield returns true
--
function ExhaustiveSolver:Solve()
  -- reset route counter and stop request flag
  self.nTried = 0
  local stopRequest = false
  --
  -- define a function to evaluate every enumerated route and update info,
  -- return true to stop enumeration,
  -- yield once to other coroutines
  local function tryRoute(route)
    route:UpdateLength()
    self:EvaluateRoute(route)
    -- update text information
    self.nTried = self.nTried + 1
    local n = table.getn(self.bestRoute)
    self.info:SetLabel("Route " .. self.nTried .. " of " .. n - 1 .. "!"
      .. "  -  Best: " .. string.format("%.2f", self.bestRoute.length))
    -- give other coroutines time to run
    stopRequest = coroutine.yield() -- true = termination request
    return stopRequest
  end
  --
  -- start permutating from element 2 because the route is a closed path
  -- (all permutations can be reached without changing the starting point)
  local t = Utils.ShallowCopy(self.bestRoute) -- Permutate changes the table
  Utils.Permutate(t, tryRoute, 2)
  -- show termination cause
  self:AddTermInfo(stopRequest)
end


---------------------------------------------------------------------------
--  EvolutionarySolver
---------------------------------------------------------------------------

EvolutionarySolver = {
  pool = {},         -- pool of routes
  generation = 0,    -- generation number
  stableGens = 0     -- number of generations with no improvements
}


--
-- Create an evolutionary solver, inheriting from Solver
--
-- in:
--   window = where to show the best route
--   infoText = StaticText to show statistics
-- return: new evolutionary solver
--
function EvolutionarySolver.Create(window, infoText)
  local self = Solver.Create(window, infoText) -- inherit from Solver
  self = Utils.ShallowCopy(EvolutionarySolver, self)
  return self
end


--
-- Solve by evolution: init, grow, mutate, select best routes,
-- yield periodically to other coroutines, request exit if yield returns true
--
function EvolutionarySolver:Solve()
  -- reset generation, termination counters and stop request flag
  self.generation = 0
  self.stableGens = 0
  local stopRequest = false
  -- init pool with a number of copies of the initial route
  self.pool = {}
  for i = 1, self.opt.poolSize do
    table.insert(self.pool, Utils.ShallowCopy(self.bestRoute))
  end
  -- give other coroutines time to run
  stopRequest = stopRequest or coroutine.yield()
  --
  -- evolutionary solving loop, exit if stable for finishCount generations
  while not (stopRequest or self.stableGens >= self.opt.finishCount) do
    -- preserve original pool contents (current best routes)
    local originalPool = Utils.ShallowCopy(self.pool)
    -- expand pool by multiplying best routes, apply mutation
    self:Grow()
    stopRequest = stopRequest or self:Mutate()
    -- add back the original pool (non-mutated routes)
    for i = 1, table.getn(originalPool) do
      table.insert(self.pool, originalPool[i])
    end
    -- select the best (shortest) routes, discard the rest
    self:KeepBestRoutes()
    -- check the best route of the new pool, update generation counts
    local changed = self:EvaluateRoute(self.pool[1])
    if changed then self.stableGens = 0 else self.stableGens = self.stableGens + 1 end
    self.generation = self.generation + 1
    -- update text information
    self.info:SetLabel("Generation " .. self.generation
      .. "  -  Best: " .. string.format("%.2f", self.bestRoute.length))
    -- give other coroutines time to run
    stopRequest = stopRequest or coroutine.yield() -- true = termination request
  end
  -- show termination cause
  self:AddTermInfo(stopRequest)
end


--
-- Multiply pool by .opt.growFactor, by making copies of existing routes
-- (routes will be changed during mutation, places are immutable)
--
function EvolutionarySolver:Grow()
  local n = table.getn(self.pool) -- current size
  for i = 1, n do 
    local r = self.pool[n] -- route to be duplicated
    for j = 1, self.opt.growthFactor - 1 do
      table.insert(self.pool, Utils.ShallowCopy(r))
    end
  end
end


--
-- Apply percentMutate (referred to pool size) mutations to random routes
--
-- return: true if stop requested
--
function EvolutionarySolver:Mutate()
  local n = table.getn(self.pool)
  local stopReq = false
  local m = 1
  while (not stopReq) and m <= self.opt.percentMutate * n / 100.0 do
    -- mutate a route taken at random
    local route = self.pool[math.random(n)]
    -- could add selection of mutation tipe here
    route:Mutate()
    route:UpdateLength()
    m = m + 1
    -- give other coroutines time to run
    stopReq = coroutine.yield() -- true = termination request
  end
  return stopReq
end


--
-- Reduce pool to the best opt.poolSize routes
--
function EvolutionarySolver:KeepBestRoutes()
  -- sort routes by increasing length
  local function rcmp(r1, r2) return r1.length < r2.length end
  table.sort(self.pool, rcmp)
  -- keep in the pool only the first opt.poolSize routes
  local newPool = {}
  for i = 1, self.opt.poolSize do
    table.insert(newPool, self.pool[i])
  end
  self.pool = newPool
end
