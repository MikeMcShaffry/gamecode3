-----------------------------------------------------------------------------
-- Name:        ParseTheTags.lua, based on ParseTags.lua
-- Purpose:     Lua routine to process the output from 'ctags' and the
--              original header files to extract classes, definitions, etc.
-- Author:      J Winwood
-- Modified by:
-- Created:     17/11/2001
-- Copyright:   (c) 2001 J Winwood.
-- RCS-ID:      $Id: ParseTheTags.lua,v 1.1 2003/08/27 01:22:48 john Exp $
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------

-- use the program "Exuberant CTags" >= v5.5 to generate the input file for this program
-- for example to generate the all the the wxWindows header files use
--
-- ctags -u --excmd=number --fields=afikms --c-types=cdefgmnpstuvx --c++-types=cdefgmnpstuvx -R -a *.h

-- print the private and protected members?
local printPrivate   = false
local printProtected = false

-- This is the base path of the files in the "tags" file it will be removed
--   see the function stripInclude
local basePath = "c:\\wxWidgets-2.5.4\\include\\wx\\"

-- These are files that you want to ignore from the ctags "tags" file
local excludeList = {}
excludeList["include/wx/chkconf.h"] = true
excludeList["include/wx/dbgrid.h"] = true
excludeList["include/wx/db.h"] = true
excludeList["include/wx/dbkeyg.h"] = true
excludeList["include/wx/dbtable.h"] = true
excludeList["include/wx/gsocket.h"] = true
excludeList["include/wx/imagbmp.h"] = true
excludeList["include/wx/imaggif.h"] = true
excludeList["include/wx/imagiff.h"] = true
excludeList["include/wx/imagjpeg.h"] = true
excludeList["include/wx/imagpcx.h"] = true
excludeList["include/wx/imagpng.h"] = true
excludeList["include/wx/imagpnm.h"] = true
excludeList["include/wx/imagtiff.h"] = true
excludeList["include/wx/imagxpm.h"] = true
excludeList["include/wx/isqlext.h"] = true
excludeList["include/wx/isql.h"] = true
excludeList["include/wx/layout.h"] = true
excludeList["include/wx/laywin.h"] = true
excludeList["include/wx/matrix.h"] = true
excludeList["include/wx/odbc.h"] = true
excludeList["include/wx/ownerdrw.h"] = true
excludeList["include/wx/ptr_scpd.h"] = true
excludeList["include/wx/resource.h"] = true
excludeList["include/wx/string.h"] = true
excludeList["include/wx/tokenzr.h"] = true
excludeList["include/wx/types.h"] = true
excludeList["include/wx/vector.h"] = true
excludeList["include/wx/vms_x_fix.h"] = true
excludeList["include/wx/wx.h"] = true
excludeList["include/wx/wx_cwc_d.h"] = true
excludeList["include/wx/wx_cwc.h"] = true
excludeList["include/wx/wx_cw_cm.h"] = true
excludeList["include/wx/wx_cw_d.h"] = true
excludeList["include/wx/wx_cw.h"] = true
excludeList["include/wx/wx_cwu_d.h"] = true
excludeList["include/wx/wxexpr.h"] = true
excludeList["include/wx/wxprec.h"] = true
excludeList["include/wx/wxshlba_cw.h"] = true
excludeList["include/wx/wxshlba_cwc_d.h"] = true
excludeList["include/wx/wxshlba_cwc.h"] = true
excludeList["include/wx/wxshlba_cw_d.h"] = true
excludeList["include/wx/wxshlba_cw_d.h"] = true
excludeList["include/wx/wxshlb_cwc_d.h"] = true
excludeList["include/wx/wxshlb_cwc.h"] = true
excludeList["include/wx/wxshlb_cw_d.h"] = true
excludeList["include/wx/wxshlb_cw.h"] = true
excludeList["include/wx/xpmdecod.h"] = true
excludeList["include/wx/xpmhand.h"] = true

-- globals
fileCache      = {}   -- cache of recently loaded files

fileList       = {}
    --includeList    = {}
    --classList      = {}
    --definitionList = {}
    --typesList      = {}
    --enumList       = {}


local resultTable = {}
local letSpace = string.byte(" ")
local letTab   = string.byte("\t") -- letTab = 9

------------------------------------------------------------------------------
-- Basic string/data processing functions
------------------------------------------------------------------------------

-- simple table dump function for an array type table
function dumpTable(atable)
    if table.getn(atable) then
        local entry = nil
        for idx = 1, table.getn(atable) do
            if entry == nil then
                entry = idx..": "..atable[idx]
            else
                entry = entry..", "..idx..": "..atable[idx]
            end
        end
        print(entry)
    end
end

-- read a file into a table in the file cache
function readFile(fileName)
    if fileCache[fileName] == nil then
        local lineTable  = {}

        for line in io.lines(fileName) do
            table.insert(lineTable, line)
        end

        fileCache[fileName] = lineTable
    end
    return fileCache[fileName]
end

-- trim the leading blanks
function trim(str)
    local breakIdx = 1
    for idx = 1, string.len(str) do
        if string.byte(str, idx) ~= letSpace and string.byte(str, idx) ~= letTab then
            breakIdx = idx
            break
        end
    end
    return string.sub(str, breakIdx)
end

-- trim trailing blanks
function rtrim(str)
    for idx = string.len(str), 1, -1 do
        if string.byte(str, idx) ~= letSpace and string.byte(str, idx) ~= letTab then
            breakIdx = idx
            break
        end
    end
    return string.sub(str, 1, breakIdx)
end

-- trim out c style comments /* ... */
function trimCcomments(str)
    local f0 = string.find(str, "/*", 1, 1)

    if (f0) then
        local f1 = string.find(str, "*/", 1, 1)
        if (f1) then
            str = string.sub(str, 1, f0-1)..string.sub(str, f1+2)
        else
            str = rtrim(string.sub(str, 1, f0-1))
        end
    end

    return str
end

-- trim off c and c++ style comments //
function trimComments(str)
    local last_str = ""
    repeat
        last_str = str
        str = trimCcomments(str)
    until (last_str == str)

    f0 = string.find(str, "//", 1, 1)
    if (f0) then
        return rtrim(string.sub(str, 1, f0-1))
    end

    return str
end

-- does the definition, or whatever, end in \
function multiline(str)
    local a = rtrim(str)
    if (string.sub(a, -1, -1) == "\\") then
        return true
    end

    return false
end

------------------------------------------------------------------------------
-- Tag specific processing functions
------------------------------------------------------------------------------

-- remove the "basePath" from the includename string returning right hand side
function stripInclude(includename)
    local f0, f1 = string.find(includename, basePath, 1, 1)
    if (f1) then return string.sub(includename, f1+1) end
    return includename
end

-- parse a line number,  90;" -> 90
function getLineNumber(lineString)
    --return tonumber(string.sub(lineString, 1, string.len(lineString)-2))

    local lineNumber = {}
    string.gsub(lineString, "([%d]+)", function (lineNo) table.insert(lineNumber, lineNo) end )
    return tonumber(lineNumber[1])
end

-- parse a classname, class:wxSomeClass -> wxSomeClass
function getClassName(classString)
    local classTable = {}
    string.gsub(classString, "([%w_]+)", function (matched) table.insert(classTable, matched) end )
    return classTable[2]
end

-- parse a enumname, enum:wxSomeClass::<anonymous>::sometype -> wxSomeClass::sometype
function getEnumName(enumString)
    local enumName = string.gsub(enumString, "enum:", "")
    enumName = string.gsub(enumName, "::<anonymous>", "")
    --enumName = string.gsub(enumName, "::::", "::")
    return enumName
end

-- parse a structname
function getStructName(structString)
    local structName = string.gsub(structString, "struct:", "")
    structName = string.gsub(structName, "::<anonymous>", "")
    return structName
end

-- parse a unionname
function getUnionName(unionString)
    local unionName = string.gsub(unionString, "union:", "")
    unionName = string.gsub(unionName, "::<anonymous>", "")
    return unionName
end

-- extract a line from a file, read from the .h file to read the whole method
function getName(methodName, fileData, lineNumber)
    local nextLine = fileData[lineNumber]
    nextLine = trim(nextLine)

    if methodName == nil then
        methodName = rtrim(nextLine)
    else
        methodName = methodName..rtrim(nextLine)
    end

    -- this is a bit of a hack - I should really do bracket matching.
    -- and scan forwards and backward to ensure an event number of parentheses
    -- and other brackets, still this seems to work for wxWindows.

    methodName = trimComments(methodName)

    -- if the line ends in a comma append the next line
    if string.sub(methodName, -1, -1) == "," then
        methodName = getName(methodName, fileData, lineNumber + 1)
    end

    methodName = string.gsub(methodName, "inline ", "")
    methodName = string.gsub(methodName, "extern ", "")

    -- This deletes { ... } stuff
    -- It's useful to know if a function is only {} though

    local f0 = string.find(methodName, "{", 1, 1)
    if (f0) then
        methodName = string.sub(methodName, 1, f0-1)
    end

    methodName = trim(rtrim(string.gsub(methodName, ";", "")))

    return methodName
end

-- build a list of included files **** NOT USED ****
function processInclude(item, fileTable)
    local includeFile = item.name
    if not fileTable.includes[includeFile] then
        fileTable.includes[includeFile] = 1
    end
end

-- processing of definitions
function processDefinition(item, fileTable)
    local fileData   = readFile(item.filename)
    local lineNumber = item.linenumber
    local definitionName = getName(nil, fileData, lineNumber)

    -- don't care about #undef
    if (string.find(definitionName, "undef") or multiline(definitionName)) then return nil end

    -- don't bother with #ifdef _WX_INCLUDE_FILE_ #defines
    if (string.byte(definitionName, 1) == string.byte("_") or
        string.byte(definitionName, string.len(definitionName)) == string.byte("_")) then return nil end

    -- only want first part of this
    local f0 = string.find(definitionName, "DECLARE_EVENT_TABLE_ENTRY", 1, 1)
    if (f0) then definitionName = string.sub(definitionName, 1, f0-1) end

    -- for some reason string.gsub(str, "#define", "%%define") doesn't work
    f0 = string.find(definitionName, "#define", 1, 1)
    if (f0) then
        definitionName = "%"..string.sub(definitionName, f0+1)
    end

    table.insert(fileTable.defs, definitionName)
    return definitionName
end

-- processing of typedefs
function processTypedef(item, fileTable)
    local fileData   = readFile(item.filename)
    local lineNumber = item.linenumber
    local typeName = getName(nil, fileData, lineNumber)

    table.insert(fileTable.typedefs, typeName)
    return typeName
end

-- processing of classes
function processClass(item, fileTable)
    local className = item.name
    if fileTable.classes[className] == nil then
        fileTable.classes[className] = {}
    end
    table.insert(fileTable.classes[className], item)

    return className
end

-- extract build a function prototype and all parameters
function processPrototype(item, fileTable)
    local fileData   = readFile(item.filename)
    local lineNumber = item.linenumber
    local methodName = getName(nil, fileData, lineNumber)

    local className = nil
    if item.base then
        className  = getClassName(item.base)
    end
    if (not className) then className = "builtin" end

    if fileTable.classes[className] == nil then
        fileTable.classes[className] = {}
    end
    methodName = string.gsub(methodName, "WXDLLEXPORT ", "")
    methodName = string.gsub(methodName, "WXCRITICAL_INLINE ", "")
    table.insert(fileTable.classes[className], item)

    if (string.find(item.name, "DECLARE_CLASS", 1, 1) or
        string.find(item.name, "DECLARE_ABSTRACT_CLASS", 1, 1) or
        string.find(item.name, "DECLARE_DYNAMIC_CLASS", 1, 1)) then
        fileTable.classes[className][1].classinfo = true
    end
    return methodName
end

-- processing of members
function processMember(item, fileTable)
    local fileData   = readFile(item.filename)
    local lineNumber = item.linenumber
    local memberName = getName(nil, fileData, lineNumber)

    -- this could be more clever
    -- if you want member "a", but it's declared as int a, b, c you get them all

    local className = ""
    if item.base then
        className  = getClassName(item.base)
    end
    if (not className) then className = "builtin" end

    if fileTable.classes[className] == nil then
        fileTable.classes[className] = {}
    end
    table.insert(fileTable.classes[className], item)

    return memberName
end

-- processing of enums
function processEnum(item, fileTable, type)
    local enumName = getEnumName(item.base)
    if fileTable.enums[enumName] == nil then
        fileTable.enums[enumName] = {}
    end
    table.insert(fileTable.enums[enumName], item)
    return item.name
end

-- processing of structs
function processStruct(item, fileTable, type)
    local structName = getStructName(item.base)
    if (item.base == "") then structName = item.name end
    if fileTable.structs[structName] == nil then
        fileTable.structs[structName] = {}
    end

    table.insert(fileTable.structs[structName], item)

    return item.name
end

-- processing of unions
function processUnion(item, fileTable, type)
    local unionName = getUnionName(item.base)
    if (item.base == "") then unionName = item.name end
    if fileTable.unions[unionName] == nil then
        fileTable.unions[unionName] = {}
    end

    table.insert(fileTable.unions[unionName], item)

    return item.name
end

function tableTagLine(line)
    local item = {}

     -- I can't seem to get a tab delimited separator to work
--   string.gsub(line, "(\t)", function(word)
--   string.gsub(line, "([%S]+)", function(word) table.insert(item, word) end )

    -- break up line by tabs
    local last_tab = 1
    for i=1, string.len(line) do
        if (string.byte(line, i) == letTab) then
            table.insert(item, string.sub(line, last_tab, i-1))
            --print(string.sub(line, last_tab, i-1))
            last_tab = i+1
        end
    end
    if (last_tab ~= 1) then table.insert(item, string.sub(line, last_tab)) end

    return item;
end


-- if includename is part of "excludeInclude" then return true, else false
function excludeInclude(includename)
    if (excludeList[includename]) then return true end
    return false;
end

function processTagLine(item, fileTable)

-- included header files (this tag field doesn't exist?)
    if item.kind == "z" then
        item.method = processInclude(item, fileTable)
        if (item.method) then item.wrapper = "%include \""..item.method.."\"" end
-- function prototypes
     elseif item.kind == "p" or item.kind == "f" then
        item.method = processPrototype(item, fileTable)
        item.wrapper = item.method
-- definitions
    elseif item.kind == "d" then
        item.method = processDefinition(item, fileTable)
        item.wrapper = item.method
-- typedefs
    elseif item.kind == "t" then
        item.method = processTypedef(item, fileTable)
        item.wrapper = item.method
-- classes
    elseif item.kind == "c" then
        item.method = processClass(item, fileTable, "class")
        item.wrapper = item.method
-- enum names
     --elseif item.kind == "g" then
     --    item.method = processClass(item, fileTable, "enum")
     --    item.wrapper = item.method
     --    if (a) then a = "%enum "..a end
-- enums
    elseif item.kind == "e" then
        item.method = processEnum(item, fileTable)
        item.wrapper = item.method
-- structs
     elseif item.kind == "s" then
         item.method = processStruct(item, fileTable, "struct")
         if (item.method) then item.wrapper = "%struct "..item.method end
-- unions
    elseif item.kind == "u" then
        item.method = processUnion(item, fileTable, "union")
        if (item.method) then item.wrapper = "%union "..item.method end
-- members
    elseif item.kind == "m" then
        if ((item.access == nil) or string.find(item.access, "access:public")) then
            item.method = processMember(item, fileTable)
            if (item.method) then item.wrapper = "%member "..item.method end
        end
    else
        --print("Unexpected data")
        --       dumpTable(item)
    end

    return item
end


function PrintFileTable(fileTable)
    print()
    print("// *****************************************************************")
    print("// "..fileTable.file)
    print("// *****************************************************************")

    if (table.getn(fileTable.defs) > 0) then print() end
    for defIndex, defValue in fileTable.defs do
        print(defValue)
    end

    if (table.getn(fileTable.typedefs) > 0) then print() end
    for typedefIndex, typedefValue in fileTable.typedefs do
        print(typedefValue)
    end

    for enumIndex, enumValue in fileTable.enums do
        print()
        print("%enum ".. enumIndex)
        for itemIndex, itemValue in enumValue do
            print("    "..itemValue.wrapper)
        end
        print("%end")
    end

    for structIndex, structValue in fileTable.structs do
        print()
        print("%struct ".. structIndex)
        for itemIndex, itemValue in structValue do
            print("    "..itemValue.wrapper)
        end
        print("%end")
    end

    for unionIndex, unionValue in fileTable.unions do
        print()
        print("%union ".. unionIndex)
        for itemIndex, itemValue in unionValue do
            print("    "..itemValue.wrapper)
        end
        print("%end")
    end

    for classIndex, classValue in fileTable.classes do
        print()
        if (classIndex ~= "builtin") then
            print("///////////////////////////////////////////////////////////////////////////////")
            print("// "..classIndex)
            print()

            --print("wxLUA_USE_"..classIndex)
            print("%include \""..fileTable.file.."\"")
        end
        for itemIndex, itemValue in classValue do
            if (classIndex == "builtin") then
                print("%builtin "..itemValue.wrapper)
            elseif (itemValue.kind == "c") then
                local s = "%class "
                if (not classValue[1].classinfo) then
                    s = s.."%noclassinfo "
                end
                s = s..itemValue.wrapper
                if (itemValue.base and (string.find(itemValue.base, "inherits"))) then
                    s = s..", "..string.gsub(itemValue.base, "inherits:", "")
                end
                print(s)
            elseif (not itemValue.access or string.find(itemValue.access, "access:public")) then
                print("    "..itemValue.wrapper)
            elseif (string.find(itemValue.access, "access:protected")) then
                if (printProtected) then print("        %protected "..itemValue.wrapper) end
            elseif (string.find(itemValue.access, "access:private")) then
                if (printPrivate) then   print("        %private "..itemValue.wrapper) end
            end
        end
        if (classIndex ~= "builtin") then
            print("%endclass")
            --print("%endif wxLUA_USE_"..classIndex)
        end
    end
end

function main()
    local lastItem = {}
    lastItem.base = " this is not a valid base"
    lastItem.filename = "this is not a filename"
    local currentName = ""

-- read the ctags generated file
    for line in io.lines("tags") do
        while true do -- a fake way to implement continue, call break to continue

        if ((string.len(line) < 2 or string.sub(line, 1, 3) == "!_")) then
            break
        end

        local tagLineTable = tableTagLine(line)

        if (not tagLineTable[4]) then break end

        local item = {}
        item.name = tagLineTable[1]
        item.filename = tagLineTable[2]
        item.file = stripInclude(tagLineTable[2])
        item.linenumber = getLineNumber(tagLineTable[3])
        item.kind = tagLineTable[4]
        item.base = tagLineTable[5] or ""
        item.access = tagLineTable[6]

        -- skip excluded includes
        if (excludeInclude(item.file)) then break end

        if (item.filename ~= lastItem.filename) then
            if (not fileList[item.file]) then
                local fileTable = {}
                fileTable.file = item.file
                fileTable.includes = {}
                fileTable.defs = {}
                fileTable.classes = {}
                fileTable.enums = {}
                fileTable.structs = {}
                fileTable.unions = {}
                fileTable.typedefs = {}
                fileList[item.file] = fileTable
            end
            if (fileList[lastItem.file]) then
                PrintFileTable(fileList[lastItem.file])
            end
        end

        item = processTagLine(item, fileList[item.file])

        lastItem = item

        break end
     end
end

main()
