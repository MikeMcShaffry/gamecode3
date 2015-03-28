#! /home/john/cvs/wxLua50/lua50/bin/lua

local tagsfilename = "tags"

-- Example of ctags file
-- name   file      def                   kind    class           access
-- Clear  string.h  /^  void Clear()$/;"  kind:f  class:wxString  access:public

local letSp  = string.byte(" ")
local letTab = 9
local letCR  = 13

-- Trim leading and trailing whitespace
function trim(arg)
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

-- Read a "col" in a line of text separated by sep_char
function ReadToNextCol(charNum, linetext, sep_char)
    local i = charNum
    local len = string.len(linetext)
    local outtext = ""

    while ( i <= len) do
        local c = string.byte(linetext, i)
        if (c == sep_char) then
            return i+1, outtext
        end
        outtext = outtext..string.char(c)
        i = i + 1
    end
    return i+1, outtext
end

-- create a table indexed by numbers of columns
function ColsToTable(linetext)
    local len = string.len(linetext)
    local col = 1
    local coltext
    local rowtable = {}
    local i = 1
    while (i <= len) do
        i, coltext = ReadToNextCol(i, linetext, letTab)
        rowtable[col] = coltext
        col = col + 1
    end
    return rowtable
end

function TrimTagDefinition(def_text)
    -- remove ctags extras
    local txt = trim(string.sub(def_text, 3, string.len(def_text)-4))
    -- remove {} if necessary
    local f0, f1 = string.find(txt, "{", 1, 1)
    if (f0) then txt = trim(string.sub(txt, 1, f0-1)) end
    -- remove // trailing comments
    f0, f1 = string.find(txt, "\\/\\/", 1, 1)
    if (f0) then txt = trim(string.sub(txt, 1, f0-1)) end

    string.gsub(txt, "WXDLLEXPORT ", "")

    return txt
end

function FindTagField(field_name, rowtable)
    for i=1,table.getn(rowtable) do
        if (string.find(rowtable[i], field_name)) then
            return string.sub(rowtable[i], string.len(field_name)+1)
        end
    end
    return nil
end

-- rework a row of cols since ctags doesn't stick in blank tabs to maintain formatting
function LineToTagsFields(rowtable)
    local newtable = {}

    newtable.name  = rowtable[1]
    newtable.file  = rowtable[2]
    newtable.def   = TrimTagDefinition(rowtable[3])
    newtable.kind  = rowtable[4]
    -- up to 4 seems to always exist

    if (not rowtable[5]) then return newtable end

    newtable.inherits = FindTagField("inherits:", rowtable)
    newtable.class = FindTagField("class:", rowtable)
    newtable.access = FindTagField("access:", rowtable)

    newtable.enum = FindTagField("enum:", rowtable)

    return newtable
end

function ReadTagsFile(filename)
    local linenumber = 0
    local tagstable = {}

    for linetext in io.lines(filename) do
        if ((string.len(linetext) > 2) and (string.sub(linetext, 1, 2) ~= "!_")) then
            linenumber = linenumber + 1
            local rowtable = LineToTagsFields(ColsToTable(linetext))
            tagstable[linenumber] = rowtable
        end
    end

    return tagstable
end

-- ctags kinds
--C
--    c  classes
--    d  macro definitions
--    e  enumerators (values inside an enumeration)
--    f  function definitions
--    g  enumeration names
--    m  class, struct, and union members
--    n  namespaces
--    p  function prototypes [off]
--    s  structure names
--    t  typedefs
--    u  union names
--    v  variable definitions
--    x  external variable declarations [off]
--C++
--    c  classes
--    d  macro definitions
--    e  enumerators (values inside an enumeration)
--    f  function definitions
--    g  enumeration names
--    m  class, struct, and union members
--    n  namespaces
--    p  function prototypes [off]
--    s  structure names
--    t  typedefs
--    u  union names
--    v  variable definitions
--    x  external variable declarations [off]

function CreateClassTable(tags_table)
    local class_table = {}
    local n = 0
    for i=1,table.getn(tags_table) do
        local row_table = tags_table[i]
        if (row_table.kind == "kind:c") then
            n = n + 1
            class_table[row_table.name] = {}
            class_table[row_table.name].members = {}
            class_table[row_table.name].name = row_table.name
            class_table[row_table.name].inherits = row_table.inherits
        end
    end

    -- now do functions
    for i=1,table.getn(tags_table) do
        local row_table = tags_table[i]

        if (((row_table.kind == "kind:f") or (row_table.kind == "kind:p")) and
            row_table.class and
            row_table.access == "public" and
            string.sub(row_table.name, 1, 1) ~= "~") then

            local class_name =  row_table.class
            if (class_table[class_name] == nil) then
                class_table[class_name] = {}
                class_table[class_name].members = {}
            end

            table.insert(class_table[class_name].members, table.getn(class_table)+1, row_table.def)
        end
    end
    return class_table
end

function CreateFunctionTable(tags_table)
    local func_table = {}
    for i=1,table.getn(tags_table) do
        local row_table = tags_table[i]
        if (row_table.kind == "kind:f" and (row_table.class == nil)) then
            local item = row_table.def
            table.insert(func_table, table.getn(func_table)+1, item)
        end
    end
    return func_table
end

function CreateDefsTable(tags_table)
    local defs_table = {}
    local item = 0
    for i=1,table.getn(tags_table) do
        local row_table = tags_table[i]
        if (row_table.kind == "kind:d") then
            -- don't want #ifndef __WX_SOMEFILE__ #defines
            if (string.sub(row_table.name, 1, 1) ~= "_") then
                item = item + 1
                defs_table[item] = row_table.name
            end
        end
    end
    return defs_table
end

function CreateEnumTable(tags_table)
    local enum_table = {}
    local n = 0
    for i=1,table.getn(tags_table) do
        local row_table = tags_table[i]
        if (row_table.kind == "kind:e") then
            n = n + 1

            local item = row_table.name
            if (row_table.class) then
                item = string.sub(row_table.class, 6).."::"..item
            end
            enum_table[n] = item
        end
    end
    return enum_table
end

function PrintTableHelper(index, item)  print(index, item) end
function PrintTable(a_table)            table.foreach(a_table, PrintTableHelper) end

function PrintTableTableHelper(index, item)  table.foreach(item, PrintTableHelper) end
function PrintTableTable(a_table)            table.foreach(a_table, PrintTableTableHelper) end


function PrintClassTable(class_table)
    local function PrintClassTableHelper(index, item)
        if (not index or not item or not item.name) then return end

        local a_class = item
        local inherits = ""
        if (a_class.inherits) then inherits = ", "..a_class.inherits end
        print("%class "..a_class.name..inherits)

        for j=1,table.getn(a_class.members) do
            print("\t"..a_class.members[j])
        end
    end

    table.foreach(class_table, PrintClassTableHelper)

end

function PrintDefsTable(defs_table)
    local function PrintDefsTableHelper(index, item)
        print("%define "..item)
    end

    table.foreach(defs_table, PrintDefsTableHelper)
end

function PrintEnumTable(enum_table)
    local function PrintEnumTableHelper(index, item)
        print("%enum "..item)
    end

    table.foreach(enum_table, PrintEnumTableHelper)
end

function PrintFuncTable(func_table)
    local function PrintFuncTableHelper(index, item)
        print("%builtin "..item)
    end

    table.foreach(func_table, PrintFuncTableHelper)
end


function main()
    local tags_table = ReadTagsFile(tagsfilename)

    local func_table = CreateFunctionTable(tags_table)
    local class_table = CreateClassTable(tags_table)
    local enum_table = CreateEnumTable(tags_table)
    local defs_table = CreateDefsTable(tags_table)

    print("//--------------------------------------------defs")
    PrintDefsTable(defs_table)
    print("//--------------------------------------------enums")
    PrintEnumTable(enum_table)
    print("//--------------------------------------------function")
    PrintFuncTable(func_table)
    print("//--------------------------------------------classes")
    PrintClassTable(class_table)

end


main()
