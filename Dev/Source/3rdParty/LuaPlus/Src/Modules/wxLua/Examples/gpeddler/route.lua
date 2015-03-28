
---------------------------------------------------------------------------
--  Place
---------------------------------------------------------------------------

-- note: a place is not treated as a 'class' to save space by avoiding
-- function references in every object (actually not a problem in this case),
-- places are treated as immutable in this program

Place = { 
  coordMax = 100.0, -- max value for coordinates
}

--
-- Create a place with random coordinates x,y in the range [0..coordMax)
--
-- in:
--   id = numeric id
-- return: new place with fields:
--   id   = numeric ID
--   x,y  = coordinates
--
function Place.Create(id)
  local self = {}
  self.id = id
  self.x = math.random() * Place.coordMax
  self.y = math.random() * Place.coordMax
  return self
end

--
-- Compute the distance between two places
--
-- in: 
--   place1 = first place
--   place2 = second place
-- return: distance
--
function Place.Distance(place1, place2)
  local dx = place2.x - place1.x
  local dy = place2.y - place1.y
  return math.sqrt(dx * dx + dy * dy)
end


---------------------------------------------------------------------------
--  Route
---------------------------------------------------------------------------

Route = {
  -- 1..getn() = places in visiting order
  length = 0  -- total length of current route (sum of distances)
}

--
-- Create a route with a sequence of randomly-placed places
--
-- in:
--   size = number of places
-- return: new route 
-- 
function Route.Create(size)
  local self = Utils.ShallowCopy(Route)
  for n = 1, size do
    table.insert(self, Place.Create(n))
  end
  -- compute initial length
  self:UpdateLength()
  return self
end


--
-- Compute closed-path length based on current route
--
-- return: nothing, but stores result in self.length
-- 
function Route:UpdateLength()
  -- start with last-first distance to close the path
  local rlen = Place.Distance(self[1], self[table.getn(self)])
  for n = 2, table.getn(self) do -- start from second place
    rlen = rlen + Place.Distance(self[n], self[n - 1])
  end  
  self.length = rlen
end


--
-- Print place names (id) in route order, useful for debugging
--
function Route:PrintNames()
  for i = 1, table.getn(self) do
    io.write(string.format("%s.", self[i].id))
  end
  print()
end


--
-- Draw route on a device context
--
-- in: 
--   dc = device context to draw on (do not delete)
--   width = available width, in pixels
--   height = available height, in pixels
--   font = font to use for id labels
--
function Route:Draw(dc, width, height, font)
  local margin = 5 -- pixels
  local xscale = (width - (2 * margin)) / Place.coordMax 
  local yscale = (height - (2 * margin)) / Place.coordMax 
  local fw, fh, fdescent = dc:GetTextExtent("Dummy", font)
  local txtOffset = (fh - fdescent) / 2
  -- pen, brush, should be deleted when finished
  local pen = wx.wxPen()
  local brush = wx.wxBrush(wx.wxRED, wx.wxSOLID)
  -- ready to go
  dc:SetFont(font)
  dc:BeginDrawing()
  dc:Clear(); -- (white)
  -- draw place squares and labels
  pen:SetColour(wx.wxRED) -- (failed assertion if skipped)
  pen:SetStyle(wx.wxTRANSPARENT)
  dc:SetPen(pen)
  dc:SetBrush(brush)
  table.foreachi(self, function(i, p)
    -- small square
    local x = p.x * xscale + margin
    local y = p.y * yscale + margin
    dc:DrawRectangle(x - 2, y - 2, 5, 5)
    -- id label (on the right if space enough, else on the left)
    local t = "" .. p.id
    local fw = dc:GetTextExtent(t, font) -- (ignore the rest)
    if fw < (width - x - 5) then x = x + 5 else x = x - fw - 5 end
    dc:DrawText(t, x, y - txtOffset); -- (black)
  end)
  -- draw connecting lines
  pen:SetStyle(wx.wxSOLID)
  pen:SetColour(wx.wxBLUE)
  dc:SetPen(pen)
  local prevx = self[1].x * xscale + margin
  local prevy = self[1].y * yscale + margin
  -- add first place after last to also draw last-first connection
  table.insert(self, self[1])
  table.foreachi(self, function(i, p)
    local x = p.x * xscale + margin
    local y = p.y * yscale + margin
    dc:DrawLine(x, y, prevx, prevy)
    prevx = x
    prevy = y
  end)
  -- finished, remove the added place and clean up
  table.remove(self)
  dc:EndDrawing()
  brush:Delete()
  pen:Delete()
end


--
-- Mutate (in-place) a route by reversing a random route segment
--
function Route:Mutate()
  local n = table.getn(self)
  -- determine start, end of segment
  local segfrom = math.random(n)
  local segto = math.random(n)
  if segfrom > segto then -- keep in ascending order
    segfrom, segto = segto, segfrom 
  end
  -- reverse places in chosen segment
  local i = segfrom
  local j = segto
  while j > i do
    self[i], self[j] = self[j], self[i]
    i = i + 1
    j = j - 1
  end
end

