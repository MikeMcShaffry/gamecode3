
---------------------------------------------------------------------------
--  Options
---------------------------------------------------------------------------

Options = {
  routeSize = 25,      -- number of places in a route
  poolSize = 3,        -- number of routes in pool before growth
  growthFactor = 30,   -- multiply poolSize by this number to grow
  percentMutate = 100, -- percentage of mutations to apply (wrt pool size)
  finishCount = 20,    -- end if no better plan for this no. of generations
  -- limits
  minRouteSize = 3, maxRouteSize = 500,
  minPoolSize = 1, maxPoolSize = 100,
  minGrowthFactor = 1, maxGrowthFactor = 1000,
  minPctMutate = 5, maxPctMutate = 1000,
  minFinishCount = 1, maxFinishCount = 200
}


--
-- Create a set of options with defaults
--
-- return: new options
--

function Options.Create()
  return Utils.ShallowCopy(Options)
end


--
-- Edit (in-place) a set of options
--
-- in:
--   parent = parent window (frame)
-- return: wx.wxID_OK or wx.wxID_CANCEL
--
function Options:Edit(parent)
  local dialog = wx.wxDialog(parent, -1, "Options")
  -- sizer for labels and input fields
  local gSizer = wx.wxFlexGridSizer(0, 2, 10, 10)
  -- create interface elements
  local routeSizeLabel, routeSizeField = WxUtils.NumericInput(dialog, gSizer,
    "Number of places", self.minRouteSize, self.maxRouteSize, self.routeSize)
  local poolSizeLabel, poolSizeField = WxUtils.NumericInput(dialog, gSizer, 
    "Best routes pool size", self.minPoolSize, self.maxPoolSize, self.poolSize)
  local growthLabel, growthField = WxUtils.NumericInput(dialog, gSizer, 
    "Pool growth by", self.minGrowthFactor, self.maxGrowthFactor, self.growthFactor)
  local percentMutateLabel, percentMutateField = WxUtils.NumericInput(dialog, gSizer, 
    "Mutations as % of pool", self.minPctMutate, self.maxPctMutate, 
    self.percentMutate)
  local finishCountLabel, finishCountField = WxUtils.NumericInput(dialog, gSizer, 
    "Steady count to finish", self.minFinishCount, self.maxFinishCount, 
    self.finishCount)
  -- add OK, Cancel buttons (in a sizer) and the function to call on "OK"
  local bSizer = WxUtils.OkCancelSizer(dialog, wx.wxID_OK,
    -- call this when OK is pressed to check that all values are valid
    function(unused)
      local ck = WxUtils.CheckFieldValue
      local valid = ck(routeSizeField, self.minRouteSize, self.maxRouteSize)
        and ck(poolSizeField, self.minPoolSize, self.maxPoolSize)
        and ck(growthField, self.minGrowthFactor, self.maxGrowthFactor)
        and ck(percentMutateField, self.minPctMutate, self.maxPctMutate)
        and ck(finishCountField, self.minFinishCount, self.maxFinishCount)
      return valid
    end)
  -- sizer to hold it all
  local vSizer = wx.wxBoxSizer(wx.wxVERTICAL)
  vSizer:AddSizer(gSizer, 0, wx.wxALL, 10)
  vSizer:AddSizer(bSizer, 0, wx.wxALL + wx.wxGROW, 10)
  -- apply layout
  dialog:SetSizer(vSizer)
  vSizer:Fit(dialog)
  -- show dialog, if OK assign values (validity already checked)
  local result = dialog:ShowModal()
  if result == wx.wxID_OK then
    self.routeSize = tonumber(routeSizeField:GetValue())
    self.poolSize = tonumber(poolSizeField:GetValue())
    self.growthFactor = tonumber(growthField:GetValue())
    self.percentMutate = tonumber(percentMutateField:GetValue())
    self.finishCount = tonumber(finishCountField:GetValue())
  end
  dialog:Destroy()
  return result
end



