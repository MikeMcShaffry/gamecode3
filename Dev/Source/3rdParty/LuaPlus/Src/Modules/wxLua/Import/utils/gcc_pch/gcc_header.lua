--  #! /home/john/cvs/wxLua50/lua50/bin/lua

local gccfilename = "wxWinInc.pch"
local includefilename = "wx_incl.h"
local includepaths = "/home/john/wx/wxCVS_25/wxWindows/include"

include_table = {}

local letSp  = string.byte(" ")
local letTab = 9
local letCR  = 13

-- Trim leading and trailing whitespace
function TrimWhitespace(arg)
    local len   = string.len(arg)
    local idxStart = 1
    local idxEnd   = len

    for idx = 1, len do
        local val = string.byte(arg, idx)
        if val ~= letCR and val ~= letSp and val ~= letTab then
            idxStart = idx
            break
        end
    end
    for idx = len, 1, -1 do
        local val = string.byte(arg, idx)
        if val ~= letCR and val ~= letSp and val ~= letTab then
            idxEnd = idx
            break
        end
    end
    return string.sub(arg, idxStart, idxEnd)
end

function StringRemove(str, substr)
    local result = str
    local f0, f1 = string.find(str, substr)
    if (f0) then
        result = string.sub(str, 1 , a-1)..string.sub(str, a+string.len(substr))
    end
    return result
end

-- load a file (a c header) that contains a list of
-- #include "wantedheader.h"
-- store wantedheader.h in the global include_table
function LoadIncludes(filename)
    for linetext in io.lines(filename) do
        local f0, f1 = string.find(linetext, "#include", 1, 1)
        if (f0) then
            f0, f1 = string.find(linetext, "\"", f0+9, 1)
            linetext = string.sub(linetext, f0+1)
            f0, f1 = string.find(linetext, "\"", 1, 1)
            linetext = string.sub(linetext, 1, f0-1)
            table.insert(include_table, linetext)
        end
    end
end

function KeepInclude(includename)

    return string.find(includename, includepaths, 1, 1)

--[[    for n=1, table.getn(include_table) do
        if (string.find(includename, include_table[n], 1, 1)) then
            return true
        end
    end

    return nil ]]
end

function main(filename)
    local wantheader = nil
    local currentheader = ""
    local printed_header = false

    for linetext in io.lines(filename) do
        linetext = TrimWhitespace(linetext)
        if (string.sub(linetext, 1, 1) == "#") then
            wantheader = KeepInclude(linetext)
            if (wantheader) then
                currentheader = string.sub(linetext, string.find(linetext, "include/wx",1,1)+11, (string.find(linetext, "\" ",1,1) or string.len(linetext))-1)
                printed_header = false
            end
        elseif wantheader then
            if (not printed_header) then
                print("// "..currentheader)
                printed_header = true
            end

            if (string.len(linetext) > 0) then
                print(linetext)
            end
        end
    end
end

LoadIncludes(includefilename)
main(gccfilename)

