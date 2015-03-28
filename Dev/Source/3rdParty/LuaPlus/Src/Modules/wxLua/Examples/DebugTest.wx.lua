
-- return the path part of the currently executing file
local strAt = string.byte('@')
function getPath()

    function findLast(filePath)
        local lastOffset = nil
        local offset
        repeat
            offset = string.find(filePath, "\\")
            if offset == nil then
                offset = string.find(filePath, "/")
            end
            if offset then
                if not lastOffset then
                    lastOffset = offset
                else
                    lastOffset = lastOffset + offset
                end
                filePath = string.sub(filePath, offset + 1)
            end
        until not offset
        return lastOffset
    end

    local filePath = debug.getinfo(1, "S").source
    if string.byte(filePath) == strAt then
        local offset = findLast(filePath)
        if offset ~= nil then
            -- remove the @ at the front up to just before the path separator
            filePath = string.sub(filePath, 2, offset - 1)
        else
            filePath = "."
        end
    else
        filePath = wx.wxGetCwd().."/Examples"
    end
    return filePath
end

dofile (getPath().."/Minimal.wx.lua")