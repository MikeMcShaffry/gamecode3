-- parses a file from grep -r "DECLARE_X_CLASS" include/wx/*
-- then reads though the wxLuaWrapGtk[Msw].h file to
-- create a list of classes that DON'T have classinfo
-- it uses the s_wxXXX tags to find a list of all classes
-- that wxLua currently uses

local has_classinfo = {}
local no_classinfo = {}

local letSp  = string.byte(" ")
local letTab = 9
local letCR  = 13

function trim(arg)
    local idxMax   = string.len(arg)
    local idxStart = 1
    local idxEnd   = idxMax
    local empty = true

    if idxMax < 1 then return arg end

    for idx = 1, idxMax do
        local val = string.byte(arg, idx)
        if val ~= letCR and val ~= letSp and val ~= letTab then
            idxStart = idx
            empty = false
            break
        end
    end

    if empty then return "" end

    for idx = idxMax, 1, -1 do
        local val = string.byte(arg, idx)
        if val ~= letCR and val ~= letSp and val ~= letTab then
            idxEnd = idx
            break
        end
    end
    return string.sub(arg, idxStart, idxEnd)
end

for line in io.lines("classinfo") do
    local f0 = string.find(line, "(", 1, 1)
    local f1 = string.find(line, ")", 1, 1)
    if (f0 and f1) then
        line = trim(string.sub(line, f0+1, f1-1))
        has_classinfo[line] = true
        --print("Has classinfo - ["..line.."]")
    end
end

for line in io.lines("../../wxLuaWrapGTK.hpp") do
    local f0, f1 = string.find(line, "int s_", 1, 1)
    if (f0 == 1 and f1) then
        line = trim(string.sub(line, f1+1))
        line = trim(string.sub(line, 1, string.len(line)-1))
        if (not has_classinfo[line]) then
            table.insert(no_classinfo, line)
        end
    end
end

table.sort(no_classinfo)

for i=1, table.getn(no_classinfo) do
    print(no_classinfo[i])
end








