require "wxLua"

function main()
    local frame = wx.wxFrame(wx.wxNull,
                                -1, 
                                "Choices",
                                wx.wxPoint(20, 20), 
                                wx.wxSize(450, 350))
    panel = wx.wxPanel(frame, -1)

    local n = wx.wxNotebook(panel, 
                            -1, 
                            wx.wxPoint(10,10), 
                            wx.wxSize(410, 300),
                            wx.wxNB_BOTTOM )

    local choices = {"one", "two", "three", "four"}

    local p1 = wx.wxPanel(n, -1)
    n:AddPage(p1, "wxRadioBox and wxListBox")    
    local b1 = wx.wxRadioBox(p1, -1, "Choice 1", wx.wxPoint(10,10), wx.wxSize(370, 100), choices, 1, wx.wxRA_SPECIFY_ROWS)
    local l1 = wx.wxListBox(p1, -1, wx.wxPoint(10,120), wx.wxSize(370, 100), choices)

    local p2 = wx.wxPanel(n, -1)
    n:AddPage(p2, "wxRadioBox and wxComboBox")    
    local b2 = wx.wxRadioBox(p2, -1, "Choice 2", wx.wxPoint(10,10), wx.wxSize(370, 100), choices, 1, wx.wxRA_SPECIFY_COLS)
    local l2 = wx.wxComboBox(p2, -1, "three", wx.wxPoint(10,120), wx.wxSize(370, 100), choices)

    local p3 = wx.wxPanel(n, -1)
    n:AddPage(p3, "extra")    
    local b3 = wx.wxRadioBox(p3, -1, "Choice 1", wx.wxPoint(10,10), wx.wxSize(370, 100), choices, 1, wx.wxRA_SPECIFY_ROWS)
    print("b3 - metatable before destruction", getmetatable(b3))
    b3:Destroy()
    print("b3 - metatable after destruction", getmetatable(b3))
    
    wx.wxGetBaseApp():SetTopWindow(frame)

    frame:Show(wx.TRUE)
end

main()
wx.Run()