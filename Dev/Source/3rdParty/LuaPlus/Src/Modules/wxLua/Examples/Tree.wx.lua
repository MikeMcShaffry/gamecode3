require "wxLua"

function main()
    frame = wx.wxFrame( wx.wxNull, 
                        -1, 
                        "Tree Demo", 
                        wx.wxDefaultPosition, 
                        wx.wxSize(450, 400), 
                        wx.wxDEFAULT_FRAME_STYLE )
    
    tree = wx.wxTreeCtrl( frame,
                          -1,
                          wx.wxDefaultPosition,
                          wx.wxDefaultSize,
                          wx.wxTR_LINES_AT_ROOT + wx.wxTR_HAS_BUTTONS )
    
    local ifr = tree:AddRoot( "Root" )
    print("root value", ifr:GetValue(), "\n")
    local ifw = tree:GetRootItem();
    print("root value", ifw:GetValue(), "\n")
    
    for idx = 0, 10 do
        ir = tree:AppendItem( ifr, "Parent ("..idx..")" )
        for jdx = 0, 5 do
            tree:AppendItem( ir, "Child ("..idx..", "..jdx..")" )
        end
        if (idx == 2) or (idx == 5) then 
            tree:Expand(ir)
        end
    end

    local ifw = tree:GetRootItem();
    print("root value", ifw:GetValue(), "\n")
    
    tree:ConnectEvent( wx.wxEVT_COMMAND_TREE_ITEM_EXPANDING,
                       function( event )
                           local selNode = event:GetItem()
                           print("Item:", selNode:GetValue(), "\n")
                       end )
    
    tree:Expand(ifr)

    wx.wxGetBaseApp():SetTopWindow(frame)

    frame:Show(wx.TRUE)
end

main()
                   
wx.Run()

