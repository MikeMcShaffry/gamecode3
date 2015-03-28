-----------------------------------------------------------------------------
-- Name:        minimal.wx.lua
-- Purpose:     'Minimal' wxLua sample
-- Author:      J Winwood
-- Modified by:
-- Created:     16/11/2001
-- RCS-ID:      $Id: VeryMinimal.wx.lua,v 1.1 2003/10/08 22:29:40 john Exp $
-- Copyright:   (c) 2001 J Winwood. All rights reserved.
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------

require "wxLua"

frame = nil

function main()

-- create the frame window
    frame = wx.wxFrame( wx.wxNull, 
                        -1, 
                        "Minimal Demo", 
                        wx.wxPoint(-1, -1), 
                        wx.wxSize(450, 450), 
                        wx.wxDEFAULT_FRAME_STYLE )

-- show the frame window
    frame:Show(wx.TRUE)
end

main()

wx.wxGetBaseApp():MainLoop()

