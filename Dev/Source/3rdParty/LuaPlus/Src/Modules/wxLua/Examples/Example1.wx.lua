require "wxLua"

frame = nil
gauge = nil

frame = wx.wxFrame( wx.wxNull,
	-1,
	"hp",
	wx.wxPoint(-1, -1),
	wx.wxSize(200, 40),
	wx.wxFRAME_TOOL_WINDOW + wx.wxCAPTION + wx.wxSTAY_ON_TOP + wx.wxRESIZE_BORDER)

frame:Show(wx.TRUE)
gauge = wx.wxGauge( frame, -1, 20, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxGA_SMOOTH)

frame:SetTitle("Hp!")
gauge:SetRange(100)
gauge:SetValue(75)

wx.Run()

