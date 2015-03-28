----------------------------------------------------------------------------------
-- Name:        wxLuaWrap.i
-- Purpose:     definition of interface data, functions and classes
-- Author:      J Winwood
-- Created:     3/11/2001
-- Modified by: J Winwood. 21/1/2002
--              Automatic generation of event handler table.
--              Minor modifications for code generation.
-- RCS-ID:      $Id: wxLuaWrap.lua,v 1.20 2004/05/20 21:45:00 john Exp $
-- Copyright:
-- Original files for wxBasic (Wrap.bas, others)
--      (c) 2001 David Cuny
-- Conversion to Lua, modification to generate Lua wrappers
-- Modified interface for %override, %define and %includefile etc.
--      (c) 2001-2002 J Winwood
-- Modified interface for lua 5.
--      (c) 2003 J Winwood
-- Licence:     wxWindows licence
----------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- wxLuaWrap.lua takes these parameters
--
--    comment  = 1/0 for creating a commented file or not (default = 0)
--    fVerbose = 2/1/0 for printing debugging info to the console (default = 1)
--
--*   target = "msw"/"gtk" [experimental mac/mgl/motif/os2/univ/x11/coca]
--
--    wxWinDir = the location of the wxWindows root dir
--               the output of `wx-config --prefix` under unix
--               if not defined the os environment variable defined by
--               wxWinEnv is used to find it
--
--    wxWinEnv = the os environment variable that has been set
--               to specify the path to the root dir of wxWindows,
--               default is WXWIN
--
--    wxSetup_h = location of the setup.h file wxWindows uses to determine
--                what features have been compiled in
--                if not defined, this tries to guess it using wxWinDir
--                it may or may not be very sucessful (see wxCxxFlags for unix)
--                ex. "/home/john/wx/wx/lib/wx/include/gtkd-2.5/setup.h"
--
--    wxVersion_h = the location of the version.h file in
--                  don't usually need to set this since if wxWinDir is found
--                  this is always wxWinDir/"include/wx/version.h"
--                  ex. "/home/john/wx/wx/include/version.h"
--
--    wxCxxFlags = output of `wx-config --cxxflags` (for unix systems)
--                 if defined then this is parsed to figure out the location
--                 of wxSetup_h is wxSetup_h is not already defined
--
--*   luasetupFile    = "path/and/filename/of/input/luasetup.h.in"
--*   luasetupFileOut = "path/and/filename/of/output/luasetup.h"
--                The luasetup files are used to trim down the size of the
--                wrapped classes. They contain wxLUA_USE_* C++ #defines that
--                you can use to adjust the functionality of your C++  program.
--                The input file luasetup.h.in is read in, compared with
--                wxWidgets setup.h file and parts that you excluded from
--                wxWidgets will be excluded from wxLua. If you want more
--                methods to exclude parts make SURE you start it with
--                wxLUA_USE_[yourname] since that part is expected.
--
--    inputFile  = "filename_of_wrapper_file.i"
--                 defaults to wxLuaWrap.i
--    inputPaths = {"../relative/path/to/wrapper/files/", "../more/wrappers/"}
--                 note trailing / and always use / NEVER \ (even in DOS)
--                 default is the dir of this program which is always checked last
--    outputFile = "../relative/path/to/write/filename_of_wrapped_file.cpp"
--                 default filename is wxLuaWrap.cpp
--
--
--(*) these are necessary
--
--    EXAMPLE USAGE :
--    lua wxLuaWrap.lua -e"target=\"gtk\"; luasetupFile=\"../Standalone/luasetup.h.in\"; luasetupFileOut=\"../Standalone/luasetup.h\"; inputPaths={ \"../Import/\" }; outputFile=\"../Standalone/wxLuaWrap.cpp\""
------------------------------------------------------------------------------

-- Change Log

-- 16 Aug 2003
-- Skip Token Directive (conditional directives) can now be added to skipToken table
-- Skip Directives can now be conjugated, anded and ored with '~', '&' and '|'
-- Added file parsing for wxWindows version.h, and setup.h
-- Changed from isWindows flag to target
-- Current Skip Directives
--
-- 16 Oct 2003
-- Use a fake continue statement using extra while loop, use break not a="" to exit see main()
-- Record all the files read in to a table so you read them once
-- Added methods for finding version.h and setup.h easier in unix
-- Added %member for Get/Set of C++ member variables
-- Made it so you can override all version/dir settings
-- Added a method to search for .i files using search paths
-- Added unix method to find setup.h using wx-config --cxxflags output

-- NOTE! if (0) is TRUE in lua, grrr...
if (comment == nil) or (comment == 0) then
    comment = false
else
    comment = true
end
if fVerbose == nil then fVerbose = 0 end

local filein  = inputFile or "wxLuaWrap.i"
local fileout = outputFile or nil

-- wx versions
if wxVer22 == nil then wxVer22 = 0 end
if wxVer23 == nil then wxVer23 = 0 end
if wxVer24 == nil then wxVer24 = 0 end
if wxVer25 == nil then wxVer25 = 0 end

-- wx Compat versions
if wxCompat20 == nil then wxCompat20 = 0 end
if wxCompat22 == nil then wxCompat22 = 0 end
if wxCompat24 == nil then wxCompat24 = 0 end

if wxUnicode == nil then wxUnicode = 0 end

if luasetupFile == nil    then luasetupFile = "luasetup.h.in" end
if luasetupFileOut == nil then luasetupFileOut = "../Import/luasetup.h" end
if target == nil          then target = "msw" end
if wxWinEnv == nil        then wxWinEnv = "WXWIN" end
if isEmbedded == nil      then isEmbedded = 0 end

if isStandalone == nil then
    if isEmbedded == 0 then
        isStandalone = 1
    else
        isStandalone = 0
    end
end

if wxWinDir == nil then wxWinDir = os.getenv(wxWinEnv) end
-- wxSetup_h stays nil if not defined yet

-- parse the output of wx-config --cxxflags for unix to find wxSetup_h
if (wxSetup_h == nil) and (wxCxxFlags ~= nil) then
    local wxcxxflags = wxCxxFlags
    while 1 do
        local a; local b
        a, b = string.find(wxcxxflags, "-I", 1, true)
        if b == nil then
            break
        else
            local c; local d
            c, d = string.find(wxcxxflags, " ", b, true)
            local sub = string.sub(wxcxxflags, b+1, c-1)
            if string.find(sub, "lib/wx/include", 1, true) then
                wxSetup_h = sub.."/wx/setup.h"
                break
            else
                wxcxxflags = string.sub(wxcxxflags, d)
            end
        end
    end
end

-- search default paths for the wxWindows setup.h file
if (wxSetup_h == nil) and (wxWinDir ~= nil) then
    setupPathList =
    {
        wxWinDir.."/lib/vc_lib/"..target.."/wx/setup.h",
        wxWinDir.."/lib/"..target.."/wx/setup.h",
        wxWinDir.."/include/wx/"..target.."/setup.h",
        wxWinDir.."/include/wx/setup.h",
        n = 4
    }

    for i=1,setupPathList.n do
        local InFilehandle = io.open(setupPathList[i])
        if InFilehandle then
            io.close(InFilehandle)
            wxSetup_h = setupPathList[i]
            break
        end
    end
end

if fVerbose >= 1 then
    print("\nFinding wxWindows Path Using Environment Variable : "..wxWinEnv.."="..(wxWinDir or "nil"))
    print("Target is : "..target)
end

-- if wxWinDir is nil then assume you want the current ver of wxWindows
if wxWinDir == nil then
    wxWinDir = ""

    print("Warning: Cannot Find wxWindows Base Path Environment Variable. Please Set "..wxWinEnv)

    if (wxVer22 == 0) and (wxVer23 == 0) and (wxVer24 == 0) and (wxVer25 == 0) then
        print("Error: no version of wxWindows set. Assuming you have 2.4")
        wxVer24 = 1
        wxCompat24 = 1
    end
end

local wxVersion_h = wxWinDir.."/include/wx/version.h"

-------------------------------------------------------------------------------
-- Globals
-------------------------------------------------------------------------------

infile            = nil
infileList        = {}
codeList          = {}
classList         = {}
classIndex        = {}
tagList           = {}
missingTagList    = {}
wrapperList       = {}
controlList       = {}
includeList       = {}
class             = {}
enum              = {}
enumValue         = {}
typedef           = {}
include           = {}
defineList        = {}
stringList        = {}
eventList         = {}
objectList        = {}
pointerList       = {}
encapsulationList = {}
skip              = 0
args              = 0
opts              = 0
inFileName        = filein
lineNumber        = 0

wrapperData       = {}

setupTable       = { n = 0 }        -- list of setup values
skipToken        = { n = 0 }        -- list of conditional skip directives
dataType         = { n = 0 }        -- store of all datatypes
keywords         = {}               -- list of wrapper interface keywords

local letSp  = string.byte(" ")
local letTab = string.byte("\t") -- 9
local letLF  = string.byte("\n") --10
local letCR  = string.byte("\r") --13
local letOr  = string.byte("|")
local letAnd  = string.byte("&")
local letA   = string.byte("A")
local letZ   = string.byte("Z")
local leta   = string.byte("a")
local letz   = string.byte("z")
local num0   = string.byte("0")
local num9   = string.byte("9")
local uscore = string.byte("_")
local colon  = string.byte(":")

-- Classes that are not derived from a wxWindow
local NonwxWindows = { wxRegion=1, wxMask=1 }

-- Set Setup Value
function setSetupValue(def, value)
    if def and def ~= "" then
        if not setupTable[def] then
            setupTable[def] = value
            setupTable.n = setupTable.n + 1  -- FIXME ? what is this doing?
            setupTable[setupTable.n] = def
        else
            setupTable[def] = value
        end
    end
end

-- when wxWindows setup overrides lua wrapper define
function setupOverride(luadef, wxdef)
    if setupTable[luadef] == 1 then
        if not wxdef then
            if fVerbose >= 1 then
                print("Downgrading Lua Setup "..luadef.." = 0, reason: not implemented by version")
            end
            setupTable[luadef] = 0
--        elseif not setupTable[wxdef] then
--            if fVerbose >= 1 then
--                print("Downgrading Lua Setup "..luadef.." = 0, reason: "..wxdef.." not defined")
--            end
--            setupTable[luadef] = 0
        elseif setupTable[wxdef] == 0 then
            if fVerbose >= 1 then
                print("Downgrading Lua Setup "..luadef.." = 0, reason: "..wxdef.." = 0")
            end
            setupTable[luadef] = 0
        end
    end
end

-- Find is parent class of datatype is corrent
function isKindOf(className, parent)
    if className == parent then return true end

    local baseName = dataType[className].parent

    while baseName do
        if baseName == parent then return true end
        baseName = dataType[baseName].parent
    end

    return false
end

-- Add dataType to Table (enum, typedef, classes, structs, and global functions
function addDataType(datatype, ctype, intrinsic, parent)
    if datatype and datatype ~= "" then
        if not dataType[datatype] then
            dataType[datatype] = { fSkip = false, ctype = ctype,  n = 0, intrinsic = intrinsic, abstract = false, parent = parent }
            dataType.n = dataType.n + 1
            dataType[dataType.n] = datatype
        end
    end
end

-- Add member functions to Data Type
function addDataTypeMember(datatype, ctype, member)
    if datatype and datatype ~= "" then
        addDataType(datatype, ctype)

        if member and member ~= "" then
            if not dataType[datatype][member] then
                dataType[datatype][member] = { fSkip = false, n = 0 }
                dataType[datatype].n = dataType[datatype].n + 1
                dataType[datatype][dataType[datatype].n] = member
            end
        end
    end
end

-- Add DataType Member Dependency
function addDataTypeMemberDependency(datatype, ctype, member, dependency)
    if datatype and datatype ~= "" and member and member ~= "" then
        addDataTypeMember(datatype, ctype, member)

        if dependency and dependency ~= "" then
            if not dataType[datatype][member][dependency] then
                dataType[datatype][member][dependency] = true
                dataType[datatype][member].n = dataType[datatype][member].n + 1
                dataType[datatype][member][dataType[datatype][member].n] = dependency
            end
        end
    end
end

-- have we mapped data types
function GetDeterminancy(datatype, func)
    -- not in a class, then global (builtin)
    if datatype == "" then
        datatype = "builtin"
    end

    local type = dataType[datatype]

    -- class/enum/typedefed?
    if not type then
        return -3, datatype
    end

    -- member function
    if func then
        local member = type[func]
        if not member then
            return -2, datatype.."::"..func
        end

        -- Has member function got any dependencies
        for d = 1, member.n do
            local dependency = member[d]

            if not dataType[dependency] then
                return -1, datatype.."::"..func.." dependency "..dependency
            end
        end
    end

    return 1, nil
end

function isTarget(skipflag)
    if (skipflag == "%"..target) then
        return 1
    end

    return 0
end

function isBlank(arg)
    if string.len(arg) == 0 then
        return 1
    end
    return nil
end

function hasText(arg)
    if string.len(arg) > 0 then
        return 1
    end
    return nil
end

-- fileExists - return true if file exists and is readable
function fileExists(path)
  local InFilehandle = io.open(path)
  if not InFilehandle then return false end
  io.close(InFilehandle)
  return true
end

-- find %includefile using inputPaths
function findIncludefile(filename)
   if inputPaths ~= nil then
       for n = 1, table.getn(inputPaths) do
           if fileExists(inputPaths[n]..filename) then
                return inputPaths[n]..filename
           end
        end
    end

   if fileExists(filename) then
       return filename
   end

    print("Error can't find '"..filename.."' in any search paths")
end

function trim(arg)
    local idxMax   = string.len(arg)
    local idxStart = 1
    local idxEnd   = idxMax
    local empty = true

    if idxMax < 1 then return arg end

    for idx = 1, idxMax do
        local val = string.byte(arg, idx)
        if val ~= letCR and val ~= letLF and val ~= letSp and val ~= letTab then
            idxStart = idx
            empty = false
            break
        end
    end

    if empty then return "" end

    for idx = idxMax, 1, -1 do
        local val = string.byte(arg, idx)
        if val ~= letCR and val ~= letLF and val ~= letSp and val ~= letTab then
            idxEnd = idx
            break
        end
    end
    return string.sub(arg, idxStart, idxEnd)
end

-- Split string
function splitLineByChar(line,delimiter,keep)
    local item = {}
    item.n = 0

    local wordStart = -1
    local wordEnd = -1
    for i=1, string.len(line) do
        local val = string.byte(line, i)
        local delim = nil

        -- delimiter can be an array of char
        for d=1, string.len(delimiter) do
            if (val == string.byte(delimiter, d)) then
                delim = val
                break
            end
        end

        -- keep delimiter in list
        local keepDelim = 0
        if delim and keep and keep ~= "" then
            for d=1, string.len(keep) do
                if (delim == string.byte(keep, d)) then
                    keepDelim = 1
                    break
                end
            end
        end

        if delim == nil then
            if wordStart == -1 then
                wordStart = i
            end
            wordEnd = i
        else
            if wordStart ~= -1 then
                item.n = item.n + 1
                item[item.n] = string.sub(line, wordStart, wordEnd)
            end
            if keepDelim == 1 then
                item.n = item.n + 1
                item[item.n] = string.char(delim)
            end
            wordStart = -1
        end
    end

    if (wordStart ~= -1) then
        item.n = item.n + 1
        item[item.n] = string.sub(line, wordStart)
    end

    return item;
end

-- returns 1 if line has skip token directive
function hasSkipTokenDirective(line)
    local limit = skipToken.n
    for tokenIdx = 1, limit do
        local f0 = string.find(line, skipToken[tokenIdx].token, 1, true)
        if (f0) then
            return 1
        end
    end

    return 0
end

-- returns skip token directive value (1, 0), arg2 isToken flag
function getSkipTokenValue(tokenStr)
    local limit = skipToken.n
    for tokenIdx = 1, limit do
        if (skipToken[tokenIdx].token == tokenStr) then
            return skipToken[tokenIdx].value, 1
        end
    end

    return 0, 0
end

-- returns 1 if all anded skip token directives return 1
-- (e.g %wxver24&%wxver25)
function getSkipTokenAndValue(tokenStr)
    local item = splitLineByChar(tokenStr, "&")
    if (item.n == 0) then
        return 0, 0, ""
    end

    for i = 1, item.n do
        local val, tok = getSkipTokenValue(item[i])
        if (tok == 0) then
            return 0, 0, ""
        end

        if val == 0 then
            return 0, tok, item[i]
        end
    end

    return 1, 1, tokenStr
end

-- returns 1 if an ored skip token directive returns 1
-- (e.g %wxver24|%wxver25)
function getSkipTokenOrValue(tokenStr)
    local item = splitLineByChar(tokenStr, "|")
    local istok = 0
    local rettoken = ""

    for i = 1, item.n do
        local val, tok, token = getSkipTokenAndValue(item[i])
        if (val == 1) then
            return 1, 1, token
        end

        if (tok == 1) then
            istok = 1
            rettoken = tokenStr
        end
    end

    return 0, istok, rettoken
end

-- Split line into words, check if ord matches token
function processSkipTokenDirectives(line)
    local keep = 1              -- keep line until we process a skip directive to reject it
    local processedLine = ""    -- line with skip directives removed
    local rettoken = ""

    if hasSkipTokenDirective(line) == 1 then
        -- split line into words
        local item = splitLineByChar(line, "\r\n\t ")

        for i = 1, item.n do
            local val, tok, token = getSkipTokenOrValue(item[i])

            -- is skip token directive
            if (tok == 1) then
                -- keep
                if (val == 0) then
                    keep = 0
                    rettoken = token
                end
            else
                -- reconstructing line
                processedLine = processedLine.." "..item[i]
            end
        end

        -- strip off leading space
        processedLine = string.sub(processedLine, 2)
    else
        -- simple, no skip token directives
        processedLine = line
    end

    return keep, processedLine, rettoken
end

-- Check to see if the tag has been stored yet, if not it's missing
function checkTag(argType, theClass)
-- we can now automatically do dependencies
--    if not tagList[argType] and not dataType[argType] then
--        if not missingTagList[argType] then
--            missingTagList[argType] = {}
--        end
--        table.insert(missingTagList[argType], "Error: missing class \""..argType.."\" in \""..theClass.."\" file : "..inFileName.." line : "..lineNumber)
--        return false
--    elseif missingTagList[argType] then   -- found it, not missing anymore
--        missingTagList[argType] = nil
--    end

    return true
end

function decodeArg(argList, arg, opt, argCode, isCtor, theClass)
    local orig        = arg
    local isRef       = nil
    local isArray     = nil
    local argType     = nil
    local argItem     = nil
    local declare     = nil
    local parm        = nil
    local optFlag     = nil
    local isConst     = nil
    local argnum

    if isCtor then
        argnum = tostring(args + opts + 1)
    else
        argnum = tostring(args + opts + 2)
    end

    if string.find(arg, "=", 1, true) then
        arg, tmp = remove(arg, "=")
        opt = arg
        arg = tmp
        optFlag = 1
    else
        if optFlag then
            print("Error: at line "..lineNumber..": "..argName.." should be optional")
            exit(1)
        end
        opt = ""
    end
-- const?
    if string.find(arg, "const", 1, true) == 1 then
        isConst = 1
        arg, tmp = remove(arg, "const")
    end
-- get type
    argType, arg = readToken(arg)
    argType = trim(argType)
    argType = fixType(argType)

    local isWx = nil
    if string.len(argType) > 2 and not isIntrinsicDataType(argType) then
       isWx = 1
    end
-- reference?
    if string.find(arg, "&", 1, true) then
        isRef = 1
        arg, tmp = remove(arg, "&")
    else
        isRef = nil
    end

    ptrs = 0
-- pointers
    while string.find(arg, "*", 1, true) do
        ptrs = ptrs + 1
        arg, tmp = remove(arg, "*")
    end

    argName, junk = readToken(arg)
    argName = trim(arg)

    if string.find(arg, "[]", 1, true) then
        isArray = 1
        arg, tmp = remove(arg, "[]")
        local off = string.find(argName, "[]", 1, true)
        if off then
            argName = string.sub(argName, 1, off - 1)
            argName = trim(argName)
        end
    else
        isArray = nil
    end

-- handle it
    if isArray then
        if argType == "wxString" then
            argItem = "tostringarray(L, "..argnum..", count)"
            declare = "int count = 0; SmartStringArray ptr; "..argType.." *"..argName.."; ptr = "..argName
        else
            declare = argType.." *"..argName
            if isWx then
                checkTag(argType, theClass)
                argItem = "("..argType.."*)getuserdatatype(L, "..argnum..", s_"..argType..")"
            else
                argItem = "("..argType.."*)ttouserdata(L, "..argnum..")"
            end
        end
        parm = argName
    elseif ptrs == 1 then
        if argType == "wxString" then
            argItem = "lua2wx(getstringtype(L, "..argnum.."))"
            declare = argType.." "..argName
        else
            declare = argType.." *"..argName
            if isWx then
                checkTag(argType, theClass)
                argItem = "("..argType.."*)getuserdatatype(L, "..argnum..", s_"..argType..")"
            else
                argItem = "("..argType.."*)ttouserdata(L, "..argnum..")"
            end
        end
        parm = argName
    elseif ptrs == 2 then
        if isWx then
            checkTag(argType, theClass)
            argItem = "("..argType.."**)getuserdatatype(L, "..argnum..", s_"..argType..")"
        else
            argItem = "("..argType.."*)ttouserdata(L, "..argnum..")"
        end
        declare = argType.." **"..argName
        parm = argName
    elseif isRef then
        if argType == "wxString" then
            argItem = "lua2wx(getstringtype(L, "..argnum.."))"
            parm = argName
            declare = argType.." "..argName
        else
            declare = argType.." *"..argName
            parm = "*"..argName
            if isWx then
                checkTag(argType, theClass)
                argItem = "("..argType.."*)getuserdatatype(L, "..argnum..", s_"..argType..")"
            else
                argItem = "("..argType.."*)ttouserdata(L, "..argnum..")"
            end
        end
    else
        if argType == "wxString" then
            argItem = "lua2wx(getstringtype(L, "..argnum.."))"
        elseif argType == "bool" then
            argItem = "getbooleantype(L, "..argnum..")"
        elseif isEnumerated(argType) then
            argItem = "("..argType..") getenumerationtype(L, "..argnum..")"
        else
            argItem = "("..argType..") getnumbertype(L, "..argnum..")"
        end
        declare = argType.." "..argName
        parm = argName
    end

    if isBlank(opt) then
        declare = declare.." = "..argItem..";"
    else
        if isRef and argType ~= "wxString" then
            opt = "&"..opt
		elseif argType == "wxString" then
     		if opt == "\"\"" then
				opt = "wxT(\"\")"
			elseif string.sub(opt, 0, 1) == "\"" then
				opt = "wxT("..opt..")"
            elseif opt ~= "NULL" then
                opt = "wxString("..opt..")"   -- gcc complains about ? wxString : wxChar*
            elseif isArray == nil and opt == "NULL" then
				opt = "wxT(\"\")"
			end
        end
        declare = declare.." = (argCount >= "..argnum.." ? "..argItem.." : "..opt..");"
    end

    if isConst then
        if argType ~= "wxString" and not isArray then
            declare = "const "..declare
        end
    end

    argComment = ""
    if (comment) then argComment = "\n    // "..orig end
    argCode = argComment.."\n    "..declare..argCode

    if hasText(argList) then
        argList = argList..", "..parm
    else
        argList = parm
    end
    return argList, arg, opt, optFlag, argCode, declare, argType
end

function buildConstructor(a, theClass)
    local word = {}

    local a, b = remove(a, "%constructor")
    local name

-- get function name
    a, name = remove(a, "(")

-- build the arglist...
    local accum = ""
    local at    = 0

    a = a.." "
    for i = 1, string.len(a) do
        char = string.sub(a, i, i)
        if char == " " or char == "," or char == ")" then
-- add word?
        if string.len(accum) > 0 then
                at = at + 1
                word[at] = accum
                accum = ""
            end
-- add separator?
            if char ~= " " then
                at = at + 1
                word[at] = accum
            end
        else
            accum = accum..char
        end
    end

    local assignFlag = nil
    local optParam   = 0
    for i = 1, at do
        if word[i + 1] == "," or word[i + 1] == ")" or word[i + 1] == "=" then
            if assignFlag then
                assignFlag = nil
            else
                if word[i + 1] == "=" then
                    assignFlag = 1
                    optParam = optParam + 1
                else
                    args = args + 1
                end
            end
        end
    end
end

function makeClassParam(class)
    local methods = class.."_methods"
    return methods..", sizeof("..methods..")/sizeof("..methods.."[0])"
end

function fixType(type)
-- if typedef is found, replace type with replacement typedef
    local result = typedef[type]
    if not result then
        result = type
    end
    return result
end

function isIntrinsicDataType(name)
    if not dataType[name] then
        if (fVerbose >= 1) then
            print("isIntrinsicDataType Warning: Cannot Find DataType "..name)
        end

        return nil
    end

    return dataType[name].intrinsic
end

-- local strNative  = {["wx"] = 1, ["cb"] = 1}
-- function isNative(name)
-- return true if starts with strNative and is not enumerated
--    local result = 1
--    if strNative[string.sub(name, 1, 2)] == nil then
--        result = nil
--    end
-- false if enumerated value or if it is name == "wxCoord"
--    if result and (enum[name] or name == "wxCoord") then
--        result = nil
--    end
--    return result
-- end

function isEnumerated(name)
-- return true if is enumerated
    local result = nil
    if enum[name] then
        result = 1
    end
    return result
end

function readLine(inputfile, trimFlag)
    local text = inputfile:read( "*l")
    if text then
        if trimFlag == nil then
            text = trim(text)
        end
    elseif table.getn(infileList) > 0 then
        local fileRec = table.remove(infileList)
        lineNumber = fileRec.lineNumber
        inFileName = fileRec.inFileName
        infile     = fileRec.infile
        return readLine(infile, trimFlag)
    end
    lineNumber = lineNumber + 1
    return text
end

-- could be implemented using string.find(.."[%w_]"..)
function readToken(arg)
    local idx    = 1
    arg = trim(arg)

    for i = 1, string.len(arg) do
        byte = string.byte(arg, i)
        if ((byte >= letA and byte <= letZ) or
            (byte >= leta and byte <= letz) or
            (byte >= num0 and byte <= num9) or
            (byte == uscore) or
            (byte == colon)) then
        else
            idx = i
            r = string.sub(arg, 1, i - 1)
            if r ~= "signed" and r ~= "unsigned" then
                break
            end
        end
    end
    r = string.sub(arg, 1, idx - 1)
    a = string.sub(arg, idx)
    return r, a
end

-- split string at position of substring returning before and after strings
function remove(srcstring, substring)
    local beforestring, afterstring
    local a = string.find(srcstring, substring, 1, true)
    if a then
        beforestring = trim(string.sub(srcstring, 1, a - 1))
        afterstring = trim(string.sub(srcstring, a + string.len(substring)))
    else
        beforestring = srcstring
        afterstring = ""
    end
    return afterstring, beforestring
end

function removeLast(srcstring, substring)
    local beforestring, afterstring
    local a = string.find(srcstring, substring, 1, true)
    local b = a

    while b do
        a = b
        b = string.find(srcstring, substring, a+string.len(substring), true)
    end

    if a then
        beforestring = trim(string.sub(srcstring, 1, a - 1))
        afterstring = trim(string.sub(srcstring, a + string.len(substring)))
    else
        beforestring = srcstring
        afterstring = ""
    end
    return afterstring, beforestring
end

function removeComma(text)
    local result
    if string.len(text) > 0 then
        result = string.sub(text, 1, string.len(text) - 2)
    else
        result = text
    end
    return result
end

function findBaseClass(baseName, className)
    local limit = classIndex.n
    for result = 1, limit do
        if classIndex[result] == baseName then
            return " "..tostring(result - 1).." /* "..baseName.." */"
        end
    end
    if fVerbose >= 1 then
        print("Warning: base class "..baseName.." has not been defined for class "..className..".")
    end
    return -1;
end

function insertClass(name)
    local limit = classIndex.n
    if limit ~= nil then
        for result = 1, limit do
            if classIndex[result] == name then
                if fVerbose >= 1 then
                    print("Warning: class "..name.." has been multiply defined.")
                end
                return result
            end
        end
        classIndex.n = classIndex.n + 1
        classIndex[classIndex.n] = name
    else
        classIndex[1] = name
        classIndex.n = 1
    end
    return classIndex.n - 1
end

-- remove comments
function removeComments(srcstring)
    local f0 = string.find(srcstring, "//", 1, true)
    if f0 then
        srcstring = string.sub(srcstring, 1, f0-1)
    end

    return trim(srcstring)
end

function writeSetupFile()
    if fVerbose >= 1 then
	    print("Configuring Available wxWindow Objects: "..luasetupFile.." -> "..luasetupFileOut)
    end

	os.remove(luasetupFileOut)

    local outfile = io.output(luasetupFileOut)
    if outfile == nil then
        print("Unable to open '"..luasetupFileOut.."' for writing.")
        return
    end

    outfile:write("// Automatically generated by wxLuaWrap.lua from "..luasetupFile.."\n\n")
    outfile:write("#ifndef __WXLUA_SETUP__\n")
    outfile:write("#define __WXLUA_SETUP__\n\n")

    local sortedSetup = {}
    for idx, val in pairs(setupTable) do
        if string.find(idx, "wxLUA_USE_") then
            local count = 40 - string.len(idx); if count < 1 then count = 1 end
            local space = string.rep(" ", count)
            table.insert(sortedSetup, "#define "..idx..space..val.."\n")
        end
    end

    table.sort(sortedSetup)

    for n=1, table.getn(sortedSetup) do
        outfile:write(sortedSetup[n])
    end

    outfile:write("\n#endif // __WXLUA_SETUP__\n")
    io.write()
end

function InitKeywords()
    -- we always have target in case we haven't added it to list
    skipToken[1] = {token = "%"..target, value = 1 }
    skipToken[2] = {token = "%wxver22", value = wxVer22 }
    skipToken[3] = {token = "%wxver23", value = wxVer23 }
    skipToken[4] = {token = "%wxver24", value = wxVer24 }
    skipToken[5] = {token = "%wxver25", value = wxVer25 }
    skipToken[6] = {token = "<wxver23", value = wxVer22 }

    if (wxVer23 == 1 or wxVer22 == 1) then
        skipToken[7] = {token = "<wxver24", value = 1 }
    else
        skipToken[7] = {token = "<wxver24", value = 0 }
    end

    if (wxVer24 == 1 or wxVer23 == 1 or wxVer22 == 1) then
        skipToken[8] = {token = "<wxver25", value = 1 }
    else
        skipToken[8] = {token = "<wxver25", value = 0 }
    end

    skipToken[9]  = {token = "%wxcompat20", value = wxCompat20 }
    skipToken[10] = {token = "%wxcompat22", value = wxCompat22 }
    skipToken[11] = {token = "%wxcompat24", value = wxCompat22 }
    skipToken[12] = {token = "%embedded",   value = isEmbedded }
    skipToken[13] = {token = "%standalone", value = isStandalone }
    skipToken[14] = {token = "%unicode",    value = wxUnicode }
    skipToken[15] = {token = "%win",        value = isTarget("%msw") }
    skipToken[16] = {token = "%msw",        value = isTarget("%msw") }
    skipToken[17] = {token = "%gtk",        value = isTarget("%gtk") }
    skipToken[18] = {token = "%mac",        value = isTarget("%mac") }
    skipToken[19] = {token = "%mgl",        value = isTarget("%mgl") }
    skipToken[20] = {token = "%motif",      value = isTarget("%motif") }
    skipToken[21] = {token = "%os2",        value = isTarget("%os2") }
    skipToken[22] = {token = "%univ",       value = isTarget("%univ") }
    skipToken[23] = {token = "%x11",        value = isTarget("%x11") }
    skipToken[24] = {token = "%cocoa",      value = isTarget("%cocoa") }

    -- Add New Target Here When They Are Available on wxWindows

    conjTokenMax = 24

    -- create conjugated tokens
    for conjToken = 1, conjTokenMax do
        if (skipToken[conjToken].value == 0) then
            skipToken[conjTokenMax+conjToken] = { token = "~"..skipToken[conjToken].token, value = 1 }
        else
            skipToken[conjTokenMax+conjToken] = { token = "~"..skipToken[conjToken].token, value = 0 }
        end
    end

    skipToken.n = conjTokenMax + conjTokenMax

    for i=1, skipToken.n do
        keywords[skipToken[i].token] = 1
    end

    local keys =
    {
        "%if",
        "%endif",
        "%override",
        "%skip",
        "%private",
        "%protected",
        "%stop",
        "%end",
        "%includefile",
        "%builtin",
        "%constructor",
        "%rename",
        "%alias",
        "%rename",
        "%property",
        "read",
        "write",
        "%enum",
        "%typedef",
        "%define",
        "%string",
        "%event",
        "%object",
        "%pointer",
        "%endclass",
        "%class",
        "%struct",
        "%alias",
        "%delete",
        "%noclassinfo",
        "%encapsulate",
        "%include",
        n = 30
    }

    for k=1, keys.n do
        keywords[keys[k]] = 1
    end
end

-- assume datatype if not keyword
function isDataType(arg)
    if (hasSkipTokenDirective(arg) == 1) then
        return nil
    end

    if (keywords[arg] == 1) then
        return nil
    end

    argAttrib =
    {
        "unsigned",
        "virtual",
        "static",
        "const",
        "return",
        "&", "*",
        "(", ")",
        "[", "]",
        "{", "}",
        ";", ",",
        n = 15 }

    for i = 1, argAttrib.n do
        if arg == argAttrib[i] then
            return nil
        end
    end

    return 1
end

function InitDataTypes()
    -- well known datatypes
    addDataType("void",             "builtin", true)
    addDataType("int",              "builtin", true)
    addDataType("char",             "builtin", true)
    addDataType("long",             "builtin", true)
    addDataType("bool",             "builtin", true)
    addDataType("short",            "builtin", true)
    addDataType("size_t",           "builtin", true)
    addDataType("off_t",            "builtin", true)
    addDataType("unsigned int",     "builtin", true)
    addDataType("unsigned char",    "builtin", true)
    addDataType("unsigned long",    "builtin", true)
    addDataType("unsigned short",   "builtin", true)
    addDataType("double",           "builtin", true)
    addDataType("float",            "builtin", true)
    addDataType("time_t",           "builtin", true)
    addDataType("lua_State*",       "builtin", true)
    addDataType("wxString",         "wx2lua",  true) -- converted to lua string
    addDataType("wxCoord",          "wxtypedef", true) -- either a long or int
    addDataType("WXTYPE",           "wxtypedef", true) -- short int
    addDataType("wxWindowID",       "wxtypedef", true) -- int
    addDataType("wxEventType",      "wxtypedef", true) -- int
    addDataType("wxSocketEventFlags", "wxtypedef", true) -- int
    addDataType("wxUint32",         "wxtypedef", true) -- unsigned long or int
end

function addWrapperLine(filename, line_number, line_text)
    if line_text ~= "" then
        local wrapLine = { filename=filename, linenumber=line_number, line=line_text }
        table.insert(wrapperData, wrapLine)
    end
end

function getWrapperLine(n)
    lineNumber = wrapperData[n].linenumber
    inFileName = wrapperData[n].filename
    return wrapperData[n].line
end

-- Build dataType table
function BuildDataTypeTable()
    local builtin      = ""
    local theClass     = ""
    local currentClass = ""
    local constructor  = ""
    local opt          = ""
    local argCode      = ""
    local replaceCode  = {}
    local eof          = nil
    local optFlag      = nil
    local stopFlag     = 0
    local skipFlag     = 0

    local declare, alias, rename, head, func, ptr, extra
    local retnum, constructorFlag, base, proxy, deleteReq
    local funcStatic, funcVirtual, argList, noClassInfo

    infile = io.input(findIncludefile(filein))

    while not eof do while not eof do   -- second for "continue" using break
        -- read a line
        local theLine = readLine(infile)
        if not theLine then
            eof = 1
            break
        end

        local a = theLine

-- strip line comments
        a = removeComments(a)

-- if empty line read another
        if a == "" then
            break
        end

-- replace tabs with spaces
        a = string.gsub(a, "\t", " ")

-- %if wxLUA_USE_xxx ... %endif
        if string.find(a, "%if", 1, true) then
            a = trim(remove(a, "%if"))
            if setupTable[a] ~= 1 then
                print("Skipping %if: "..a)

                local startFileName = inFileName
                local ifcount = 1
                while not eof do
                    -- read a line
                    local theLine = readLine(infile, 1)
                    if not theLine then
                        eof = 1
                        print("ERROR: Unterminated %if %endif started in "..startFileName)
                        break
                    end
                    theLine = removeComments(theLine)
                    if string.find(theLine, "%if", 1, true) then
                        ifcount = ifcount + 1
                    elseif string.find(theLine, "%endif", 1, true) then
                        ifcount = ifcount - 1
                        if (ifcount == 0) then break end
                    end
                end
            end
        end

-- skip %protected functions
        if string.find(a, "%protected", 1, true) then
            break
        end

-- skip %private functions
        if string.find(a, "%private", 1, true) then
            break
        end

-- just ignore c++ operator
        if string.find(a, "operator", 1, true) then
            break
        end

-- if we've made it this far then is is a valid wrapper - add it to the table
        if a ~= "" then
            addWrapperLine(inFileName, lineNumber, a)
        end

-- %endclass or %endstruct
        if string.find(a, "%endclass", 1, true) or string.find(a, "%endstruct", 1, true) then
            theClass = ""
            skipFlag = 0
            break
        end

-- conditional skip directive
        if a ~= "" and not string.find(a, "%override", 1, true) then
            local keep, processedLine, rettoken = processSkipTokenDirectives(a)
            if (keep == 1) then
                -- keeping line, use line with stripped skip directives
                a = processedLine
            else
                -- rejecting line, we need to remove all other directive types (esp. %skip)
                a = "/* Skipped "..rettoken.." */"
                break
            end
        end

-- %skip directive
        if string.find(a, "%skip", 1, true) then
            skipFlag = -1
            break
        end

-- %stop directive
        if string.find(a, "%stop", 1, true) then
            eof = 1
            break
        end

-- %override code override until %end
        if string.find(a, "%override", 1, true) then
            -- read past override definition
            local startFileName = inFileName
            while not eof do
                -- read a line
                local theLine = readLine(infile, 1)
                if not theLine then
                    eof = 1
                    print("ERROR: Unterminated %override started in "..startFileName)
                    break
                end

                addWrapperLine(inFileName, lineNumber, theLine)

                if theLine == "%end" then
                    break
                end
            end

            break
        end

-- %includefile - handle an include file
        if string.find(a, "%includefile", 1, true) then
            local fileRec = {}
            a, b = remove(a, "%includefile")
            fileRec.infile     = infile
            fileRec.lineNumber = lineNumber
            fileRec.inFileName = inFileName
            table.insert(infileList, fileRec)
            a = findIncludefile(a)
            infile = io.input(a)
            if infile == nil then
                print("Error: at line "..lineNumber.." in file "..inFileName..": Include file '"..a.."' can not be opened.")
                exit(1)
            else
                inFileName = a
                lineNumber = 0
            end

            break
        end

-- %enum <name> until %end
        if string.find(a, "%enum", 1, true) then
            a, b = remove(a, "%enum")

            -- add datatype
            addDataType(a, "enum", true)

            -- read past enum definition
            local startFileName = inFileName
            while not eof do
                local theLine = readLine(infile, 1)
                if not theLine then
                    eof = 1
                    print("ERROR: Unterminated %enum started in "..startFileName)
                    break
                end

                addWrapperLine(inFileName, lineNumber, theLine)

                theLine = removeComments(theLine)
                if string.find(theLine, "%end", 1, true) then
                    break
                end
            end

            break
        end
-- %typedef <name> <name>
        if string.find(a, "%typedef", 1, true) then
            a, b = remove(a, "%typedef ")
            a, b = remove(a, " ")

            -- add datatype
            addDataType(b, "typedef")

            break
        end
-- %class <name> or %struct <name>
        if string.find(a, "%class", 1, true) == 1 or string.find(a, "%struct", 1, true) == 1 then
            local isClass = string.find(a, "%class", 1, true) == 1

            if skipFlag == -1 then
                skipFlag = -2
            else
                skipFlag = 0
                if isClass then
                    a, b = remove(a, "%class")
                else
                    a, b = remove(a, "%struct")
                end

                if string.find(a, "%alias", 1, true) then
                    a, b = remove(a, "%alias")
                    a, alias = remove(a, " ")
                else
                    alias = ""
                end

                deleteReq = nil
                if (string.find(a, "%delete", 1, true)) then
                    a, b = remove(a, "%delete")
                    deleteReq = 1
                end

                noClassInfo = nil
                if (string.find(a, "%noclassinfo", 1, true)) then
                    a, b = remove(a, "%noclassinfo")
                    noClassInfo = 1
                end

                local encapsulate = nil
                if (string.find(a, "%encapsulate", 1, true)) then
                    a, b = remove(a, "%encapsulate")
                    encapsulate = true
                end

                if string.find(a, ",", 1, true) then
                    a, theClass = remove(a, ",")
                    base = a
                else
                    theClass = a
                    base = nil
                end

                -- add datatype
                addDataType(theClass, "class", nil, base)
            end
        else
            local method = ""
            local item = splitLineByChar(a, "\t\r\n *&()[],;={}", "(),{=")
            for i=1, item.n do
                if item[i] == "(" then
                    -- assume method() (WithNoCasts *)
                    method = item[i-1]
                    if method then
                        if theClass == "" then
                            addDataType("builtin", "function", method)
                        else
                            addDataTypeMember(theClass, "class", method)
                        end
                    end
                    break
                end
            end

            -- add datatype dependencies of method
            if method ~= "" then
                local hasType = nil
                local retType = true
                local castDepth = 0
                for i=1, item.n do
                    if castDepth == 0 then
                        local dependency = item[i]
                        if (item[i] ~= method and retType or not retType)  and isDataType(dependency) and not hasType and item[i-1] ~= "%rename" then
                            hasType = 1

                            -- unsigned datatype
                            if item[i-1] == "unsigned" then
                                dependency = "unsigned "..dependency
                            end

                            if theClass == "" then
                                addDataTypeMemberDependency("builtin", "function", method, dependency)
                            else
                                addDataTypeMemberDependency(theClass, "class", method, dependency)
                            end
                        end

                        if item[i] == "(" then
                            if retType then
                                retType = nil
                            else
                               castDepth = castDepth + 1
                            end

                            hasType = nil
                        end

                        if item[i] == "," then
                            hasType = nil
                        end


                        -- end-of-function parameters
                        if item[i] == "{" or item[i] == ")" and castDepth == 0 then
                            if theClass ~= "" then
                                if item[i+1] == "=" and item[i+2] == "0" then
                                    if fVerbose >= 1 then
                                        print("Class "..theClass.." Abstract")
                                    end

                                    dataType[theClass].abstract = true
                                end
                            end
                            break
                        end
                    else
                        if item[i] == "(" then
                            castDept = castDepth + 1
                        end

                        if item[i] == ")" then
                            castDept = castDepth - 1
                        end
                    end
                end
            end
        end
    end end  -- second for "continue"

    if fVerbose >= 2 then
        print("dataType Dump")
        print("=====================================================================")
        for i = 1, dataType.n do
            local typeName = dataType[i]
            print("dataType: "..typeName)

            local type = dataType[typeName]
            for m = 1, type.n do
                local memberName = type[m]
                print("\tmember: "..memberName)

                local member = type[memberName]
                for d = 1, member.n do
                    local dependency = member[d]
                    print("\t\tdependency: "..dependency)
                end
            end
        end
        print("=====================================================================")
    end
    print(" ")
end

function isSkip(line)
    local item = splitLineByChar(line, "\t\r\n *&()[],;={}", "(),{=")
    for i=1, item.n do
        if dataType[item[i]] then
            return dataType[item[i]].fSkip
        end
    end

    return nil
end

function isAbstract(class)
    if dataType[class] then
        return dataType[class].abstract
    end

    return nil
end

-- get wx version
function ReadSetupFiles(wxversionFile, setupFile)
    local major
    local minor
    local release

    if (wxversionFile and fileExists(wxversionFile)) then
        -- get wx version from version file (version.h)
        if fVerbose >= 1 then
            print("Parsing wxWindows Version File: "..wxversionFile)
        end
        for line in io.lines(wxversionFile) do
            local item = splitLineByChar(line, "\r\n\t ")

            if item[1] == "#define" and item[2] == "wxMAJOR_VERSION" then
                major = item[3]
            end

            if item[1] == "#define" and item[2] == "wxMINOR_VERSION" then
                minor = item[3]
            end

            if item[1] == "#define" and item[2] == "wxRELEASE_NUMBER" then
                release = item[3]
            end

            -- add defines to Lookup Table
            if item[1] == "#define" and item[2] then
                if not string.find(line, "\\", 1, true) then -- ignore defines with line continuation
                    if item[3] == "0" then
                        setSetupValue(item[2], 0)
                    elseif item[3] == "1" then
                        setSetupValue(item[2], 1)
                    else
                        local def = ""
                        for d=3, item.n do
                            if def ~= "" then def = def.." " end

                            def = def..item[d]
                        end

                        setSetupValue(item[2], def)
                    end
                end
            end
        end

        if major == "2" and minor == "2" then
            wxVer22 = 1
            outputDir = "wx22/"
            if fVerbose >= 1 then
                print("Version: wxWindows 2.2")
            end
        end

        if major == "2" and minor == "3" then
            wxVer23 = 1
            outputDir = "wx23/"
            if fVerbose >= 1 then
                print("Version: wxWindows 2.3")
            end
        end

        if major == "2" and minor == "4" then
            wxVer24 = 1
            outputDir = "wx24/"
            if fVerbose >= 1 then
                print("Version: wxWindows 2.4")
            end
        end

        if major == "2" and minor == "5" then
            wxVer25 = 1
            outputDir = "wx25/"
            if fVerbose >= 1 then
                print("Version: wxWindows 2.5")
            end
        end
    end

    if wxSetup_h and fileExists(wxSetup_h) then
        -- get compat from setup.h
        if fVerbose >= 1 then
            print("Parsing wxWindows Setup File: "..wxSetup_h)
        end
        for line in io.lines(wxSetup_h) do
            local item = splitLineByChar(line, "\r\n\t ")
            if item[1] == "#define" and item[2] == "WXWIN_COMPATIBILITY_2" and item[3] == "1" then
				wxCompat20 = 1
                if fVerbose >= 1 then
                    print("Setup: Compatibility For wxWindows Version 2")
                end
            end

            if item[1] == "#define" and item[2] == "WXWIN_COMPATIBILITY_2_2" and item[3] == "1" then
				wxCompat22 = 1
                if fVerbose >= 1 then
                    print("Setup: Compatibility For wxWindows Version 2.2")
                end
            end

            if item[1] == "#define" and item[2] == "WXWIN_COMPATIBILITY_2_4" and item[3] == "1" then
				wxCompat24 = 1
                if fVerbose >= 1 then
                    print("Setup: Compatibility For wxWindows Version 2.4")
                end
            end

            if item[1] == "#define" and item[2] == "wxUSE_UNICODE" and item[3] == "1" then
                wxUnicode = 1
                if fVerbose >= 1 then
                    print("Setup: wxWindows Unicode")
                end
            end

            -- add defines to Lookup Table
            if item[1] == "#define" and item[2] then
                if not string.find(line, "\\", 1, true) then -- ignore defines with line continuation
                    if item[3] == "0" then
                        setSetupValue(item[2], 0)
                    elseif item[3] == "1" then
                        setSetupValue(item[2], 1)
                    else
                        local def = ""
                        for d=3, item.n do
                            if def ~= "" then def = def.." " end

                            def = def..item[d]
                        end

                        setSetupValue(item[2], def)
                    end
                end
            end
        end
    else
        if fVerbose >= 1 then
            print("Cannot find wxWindow's setup.h file - relying on the luasetup.h file")
        end
    end

    -- cmdLine Flag
    if fUnicode then
        wxUnicode = fUnicode

        if fVerbose >= 1 then
            if (wxUnicode == 1) then
                print("CmdLine: wxWindows Unicode")
            else
                print("CmdLine: wxWindows Ansi")
            end
        end
    end

    if (fileout == nil) then
        if (wxUnicode == 1) then
            fileout = outputDir.."luawrap"..target.."u.hpp"
        else
            fileout = outputDir.."luawrap"..target..".hpp"
        end
    end

    if fVerbose >= 1 then
        print("Output File: "..fileout)
    end

    -- parse Lua Setup File (last so it takes precedence)
    if (setupFile) then
        if fVerbose >= 1 then
            print("Parsing Lua Setup File: "..setupFile)
        end
        for line in io.lines(setupFile) do
            local item = splitLineByChar(removeComments(line), "\r\n\t ")

            -- add defines to Lookup Table
            if item[1] == "#define" and item[2] then
                if not string.find(line, "\\", 1, true) then -- ignore defines with line continuation
                    if item[3] == "0" then
                        setSetupValue(item[2], 0)
                    elseif item[3] == "1" then
                        setSetupValue(item[2], 1)
                    else
                        local def = ""
                        for d=3, item.n do
                            if def ~= "" then def = def.." " end

                            def = def..item[d]
                        end

                        setSetupValue(item[2], def)
                    end
                end
            end
        end
    end

    -- Setup Override
    setupOverride("wxLUA_USE_wxDateTime",     "wxUSE_DATETIME")
    setupOverride("wxLUA_USE_wxTimeSpan",     "wxUSE_DATETIME")
    setupOverride("wxLUA_USE_wxDateSpan",     "wxUSE_DATETIME")
    setupOverride("wxLUA_USE_wxCalendarCtrl", "wxUSE_DATETIME")
    setupOverride("wxLUA_USE_wxTimer",        "wxUSE_TIMER")
    setupOverride("wxLUA_USE_wxSocket",       "wxUSE_SOCKETS")
    setupOverride("wxLUA_USE_wxRegEx",        "wxUSE_REGEX")
    setupOverride("wxLUA_USE_wxWave",         "wxUSE_WAVE")

    if wxVer22 == 1 then
        setupOverride("wxLUA_USE_FL",                  nil)
        setupOverride("wxLUA_USE_wxStyledTextCtrl",    nil)
        setupOverride("wxLUA_USE_wxFindReplaceDialog", nil)
        setupOverride("wxLUA_USE_wxFileName",          nil)
--        setupOverride("wxLUA_USE_wxCommandProcessor", nil)
        setupOverride("wxLUA_USE_wxXMLResource",       nil)
    end

    if wxVer23 == 1 then
        if wxUnicode == 1 then
            setupOverride("wxLUA_USE_FL",              nil)
            if fVerbose == 1 then
                print("\tFL in wxWindows 2.3.4 does not support Unicode")
            end
        end
    end
end

function main()
    local builtin      = ""
    local theClass     = ""
    local currentClass = ""
    local constructor  = ""
    local opt          = ""
    local argCode      = ""
    local replaceCode  = {}
    local eof          = nil
    local optFlag      = nil
    local stopFlag     = 0
    local skipFlag     = 0

    local declare, alias, rename, head, func, ptr, extra
    local retnum, constructorFlag, base, proxy, deleteReq
    local funcStatic, funcVirtual, argList, noClassInfo

    if not fileExists(wxVersion_h) then
        print("Error: Cannot Find wxWindows Version File "..wxVersion_h)
    end

    ReadSetupFiles(wxVersion_h, luasetupFile)
	-- Write setup file with one found on wxLuaWrap.lua commandline, modified
    --  by wxWidgets setup.h
	writeSetupFile()

    if (fVerbose >= 2) then
        print("Setup Definition Table")
        print("==========================")

        for def = 1, setupTable.n do
            if (setupTable[setupTable[def]]) then
                print(setupTable[def].."\t= "..setupTable[setupTable[def]])
            else
                print(setupTable[def].."\t= nil")
            end
        end
        print("==========================")
        print("")
    end

    -- setup Keywords List
    InitKeywords()

    if (fVerbose >= 2) then
        print("Skip Directive State Table")
        print("==========================")

        for tok = 1, skipToken.n do
            if (skipToken[tok].value) then
                print(skipToken[tok].token.."\t= "..skipToken[tok].value)
            else
                print(skipToken[tok].token.."\t= nil")
            end
        end
        print("==========================")
        print("")
    end


    -- load builtin datatypes
    InitDataTypes()

    -- parse interface file for datatypes
    BuildDataTypeTable()

    table.insert(codeList, "\n")

    local wrapperCount = table.getn(wrapperData)

	local n = 1
	while n <= wrapperCount do while 1 do -- second while for continue using break
--    for n=1, wrapperCount do while 1 do -- second while for continue using break
        -- read a line
        local theLine = getWrapperLine(n)
        local skipOverride = 0
        local skipClassInPrototype = 0

        local a = theLine

-- prevent code in comments from being interpreted
        a = removeComments(a)

-- replace tabs with spaces
        a = string.gsub(a, "\t", " ")

--[[
-- %if wxLUA_USE_xxx ... %endif
        if string.find(a, "%if", 1, true) then
            a = trim(remove(a, "%if"))
            if setupTable[a] ~= 1 then
                local ifcount = 1
                while n < wrapperCount do
                    n = n + 1
                    -- read a line
                    local theLine = getWrapperLine(n)
                    theLine = removeComments(theLine)
                    if string.find(theLine, "%if", 1, true) then
                        ifcount = ifcount + 1
                    elseif string.find(theLine, "%endif", 1, true) then
                        ifcount = ifcount - 1
                        if (ifcount == 0) then break end
                    end
                end
            end
        end

-- skip %protected functions
        if string.find(a, "%protected", 1, true) then
            break
        end

-- skip %private functions
        if string.find(a, "%private", 1, true) then
            break
        end

-- just ignore c++ operator
        if string.find(a, "operator", 1, true) then
            break
        end
]]

-- check for skipToken directive
-- (ignore %override as it will do its own skipToken processing)
        if a ~= "" and not string.find(a, "%override", 1, true) then
            local keep, processedLine, rettoken = processSkipTokenDirectives(a)
            if (keep == 1) then
                if fVerbose >= 2 then
                    if (theClass ~= nil and theClass ~= "") then
                       print(theClass.." Ok: "..a)
                    else
                       print(a)
                    end
                end

                -- keeping line, use line with stripped skip directives
                a = processedLine
            else
                if fVerbose >= 2 then
                    if (theClass ~= nil and theClass ~= "") then
                       print(theClass.." Skipping: "..a)
                    else
                       print("Skipping "..a)
                    end
                end

                -- rejecting line, we need to remove all other directive types (esp. %skip)
                a = "/* Skipped "..rettoken.." */"
                break
            end
        end

-- %skip
        if string.find(a, "%skip", 1, true) then
            skipFlag = -1
            break
        end

-- skip %class <name> or %struct <name>
        if string.find(a, "%class", 1, true) == 1 or string.find(a, "%struct", 1, true) == 1 then
            if isSkip(a) then
                if fVerbose >= 1 then
                    print("Skipping Class: "..a)
                end

                skipFlag = -1
                break
            end
        end

-- %stop
        if string.find(a, "%stop", 1, true) then
            n = wrapperCount + 1
            break
        end

-- %override code override until %end
        if string.find(a, "%override", 1, true) then
            if (fVerbose >= 2)then
                print("Reading Override: "..a)
            end

            local replacement = ""
            local source = a
            a, b = remove(a, "%override")

            skipOverride = 0

            -- check for skipToken
            local keep, processedLine, rettoken = processSkipTokenDirectives(a)
            if (keep == 0) then
                skipOverride = 1
                if fVerbose >= 2 then
                   print("Skipping "..a)
                end
            end
            a = processedLine

            -- read overide function
            while n < wrapperCount do
                n = n + 1
                -- read a line
                local theLine = getWrapperLine(n)
                if theLine == "%end" then
                    break
                else
                    -- check for skipToken
                    local keep, processedLine, rettoken = processSkipTokenDirectives(theLine)
                    if (keep == 1) then
                        replacement = replacement.."\n"..processedLine
                    end
                end
            end

            if (skipOverride == 0) then
                if replaceCode[a] ~= nil then
                    if (skipOverride == 0) then
                        if (replaceCode[a].skip == 0) then
                            if (fVerbose >= 1) then
                                print("Warning: at line "..lineNumber.." in file "..inFileName..": Duplicate override for function '"..a.."'.")
                            end
                        end

                        replaceCode[a].override = 0
                        replaceCode[a].skip = 0
                        replaceCode[a].source = source
                    end
                else
                    replaceCode[a] = { override = replacement.."\n", skip = skipOverride, source = source }
                end
            end
            break
        end

-- %includefile - handle an include file
        if string.find(a, "%includefile", 1, true) then
            break
--[[
            local fileRec = {}
            a, b = remove(a, "%includefile")
            fileRec.infile     = infile
            fileRec.lineNumber = lineNumber
            fileRec.inFileName = inFileName
            table.insert(infileList, fileRec)
            a = findIncludefile(a)
            infile = io.input(a)
            if infile == nil then
                print("Error: at line "..lineNumber.." in file "..inFileName..": Include file '"..a.."' can not be opened.")
                exit(1)
            else
                inFileName = a
                lineNumber = 0
            end
            a = ""
]]
        end

-- %builtin - for creating direct access to methods
        if string.find(a, "%builtin", 1, true) then
            a, b = remove(a, "%builtin")
            if isBlank(builtin) then
                builtin = theClass
                theClass = "builtin"
            end
        elseif hasText(builtin) then
            theClass = builtin
            builtin = ""
        end
-- clear prior constructor
        if hasText(constructor) then
            theClass = constructor
            constructor = ""
        end
-- %constructor - for alternate constructors
        if string.find(a, "%constructor", 1, true) then
            -- skip constructor in abstract classes
            if isAbstract(theClass) then
                a = ""
            else
                buildConstructor(a, theClass)
                a, b = remove(a, "%constructor")
                constructor = theClass
                c = a
                c, theClass = remove(c, "(")
            end
        end
-- %rename <new name>
        if string.find(a, "%rename", 1, true) then
            a, b = remove(a, "%rename")
            a, rename = remove(a, " ")
        else
            rename = ""
        end
-- %alias <alias>
        if string.find(a, "%alias", 1, true) == 1 then
            a, b = remove(a, "%alias")
            a, alias = remove(a, " ")
        else
            alias = ""
        end
-- %rename - in case order is %alias %rename
        if string.find(a, "%rename", 1, true) then
            a, b = remove(a, "%rename")
            a, rename = remove(a, " ")
        end
-- %property
        if skipFlag == 0 and string.find(a, "%property", 1, true) then
            a, b = remove(a, "%property")
            a, b = remove(a, "=")
            a, b = remove(a, ",")
            local propertyName = b
            local readStart = string.find(a, "read")
            local writeStart = string.find(a, "write")

            local skipProperty = 0

            -- Do we know all data types of Get Property Function
            local member = "Get"..propertyName
            local determancy, msg  = GetDeterminancy(theClass, member)
            if determancy < 1 then
                if fVerbose >= 1 then
                    print("Skipping Property: Cannot Find Property Function "..msg)
                end
                skipProperty = 1
            end

            if skipProperty == 0 then
                if readStart then
                    table.insert(wrapperList, "    { LuaGetProp,     \""..propertyName.."\", "..theClass.."_Get"..propertyName..", 0, 0 },\n")
                end
                if writeStart then
                    table.insert(wrapperList, "    { LuaSetProp,     \""..propertyName.."\", "..theClass.."_Set"..propertyName..", 1, 1 },\n")
                end
                if fVerbose >= 2 then
                    print("Property name=", propertyName, "read: ", readStart, "write: ", writeStart)
                end
            end
        end
-- %member type m_memberVariable
        if skipFlag == 0 and string.find(a, "%member", 1, true) then
            a, b = remove(a, "%member")
            a, b = remove(a, " ")
            local memberType = fixType(b)
            local memberName = a
            local memberGetFunc = "Get_"..memberName
            local memberSetFunc = "Set_"..memberName
            if hasText(rename) then
                memberGetFunc = "Get"..rename
                memberSetFunc = "Set"..rename
            end

            -- create the code for the function to get the member
            table.insert(codeList, "\n// "..theLine)
            table.insert(codeList, "\nstatic int LUACALL "..theClass.."_"..memberGetFunc.."(lua_State *L)\n{")
            if (comment) then table.insert(codeList, "\n    // get this") end
            table.insert(codeList, "\n    "..theClass.." *self = ("..theClass.."*)getuserdatatype(L, 1, s_"..theClass..");")
            if memberType == "wxString" then
                if (comment) then table.insert(codeList, "\n    // push the result string") end
                table.insert(codeList, "\n    lua_pushstring(L, wx2lua(self->"..memberName.."));")
            elseif not isIntrinsicDataType(memberType) then
                if (comment) then table.insert(codeList, "\n    // push the result datatype") end
                table.insert(codeList, "\n    pushuserdatatype(L, s_"..memberType..", self->"..memberName..");")
            elseif type == "BOOL" or type == "bool" then
                if (comment) then table.insert(codeList, "\n    // push the result flag") end
                table.insert(codeList, "\n    lua_pushboolean(L, self->"..memberName..");")
            else
                if (comment) then table.insert(codeList, "\n    // push the result number") end
                table.insert(codeList, "\n    lua_pushnumber(L, self->"..memberName..");")
            end
            if (comment) then table.insert(codeList, "\n    // return the number of parameters") end
            table.insert(codeList, "\n    return 1;\n}\n")

            -- create the code for the function to set the member
            table.insert(codeList, "\n// "..theLine)
            table.insert(codeList, "\nstatic int LUACALL "..theClass.."_"..memberSetFunc.."(lua_State *L)\n{")
            if memberType == "wxString" then
                if (comment) then table.insert(codeList, "\n    // get the string value") end
                table.insert(codeList, "\n    wxString val = lua2wx(getstringtype(L, 2);")
            elseif not isIntrinsicDataType(memberType) then
                if (comment) then table.insert(codeList, "\n    // get the data type value") end
                table.insert(codeList, "\n    "..memberType.." val = *(("..memberType.."*)getuserdatatype(L, 2, s_"..memberType.." ));")
            elseif type == "BOOL" or type == "bool" then
                if (comment) then table.insert(codeList, "\n    // get the boolean value") end
                table.insert(codeList, "\n    bool val = getbooleantype(L, 2);")
            else
                if (comment) then table.insert(codeList, "\n    // get the number value") end
                table.insert(codeList, "\n    "..memberType.." val = getnumbertype(L, 2);")
            end
            if (comment) then table.insert(codeList, "\n    // get this") end
            table.insert(codeList, "\n    "..theClass.." *self = ("..theClass.."*)getuserdatatype(L, 1, s_"..theClass..");")
            table.insert(codeList, "\n    self->"..memberName.." = val;")
            if (comment) then table.insert(codeList, "\n    // return the number of parameters") end
            table.insert(codeList, "\n    return 0;\n}\n")

            table.insert(wrapperList, "    { LuaMethod,      \""..memberGetFunc.."\", "..theClass.."_"..memberGetFunc..", 0, 0 },\n")
            table.insert(wrapperList, "    { LuaMethod,      \""..memberSetFunc.."\", "..theClass.."_"..memberSetFunc..", 1, 1 },\n")
            table.insert(wrapperList, "    { LuaGetProp,     \""..memberName.."\", "..theClass.."_"..memberGetFunc..", 0, 0 },\n")
            table.insert(wrapperList, "    { LuaSetProp,     \""..memberName.."\", "..theClass.."_"..memberSetFunc..", 1, 1 },\n")

            if isVerbose then
                print("Member type=", memberType, "name: ", memberName)
            end

            break
        end
-- %enum <name> until %end
        if string.find(a, "%enum", 1, true) then
            a, b = remove(a, "%enum")
            enum[a] = 1
            local member = string.find(a, "::")
            local namespace = ""
            if (member) then
                namespace = string.sub(a, 0, member - 1) .. "::"
            end

            while n < wrapperCount do
                n = n + 1
                local theLine = getWrapperLine(n)

                theLine = removeComments(theLine)
                if string.find(theLine, "%end", 1, true) then
                    break
                end
                local data = theLine
                if string.len(data) > 1 then
                    -- check for skipToken
                    local keep, processedLine, rettoken = processSkipTokenDirectives(data)
                    if (keep == 1) then
                        enumValue[processedLine] = namespace .. processedLine
                    end
                end
            end
            break
        end
-- %typedef <name> <name>
        if string.find(a, "%typedef", 1, true) then
            a, b = remove(a, "%typedef ")
            a, b = remove(a, " ")
            typedef[b] = a
            break
        end
-- %define [%string][%event][%object][%pointer] <symbol>
        if string.find(a, "%define", 1, true) then
            if skipFlag == 0 then
                a, b = remove(a, "%define ")
                a, b = remove(a, " ")
                if string.find(b, "%string", 1, true) then
                    b = trim(a)
                    stringList[b] = b
                -- %event description
                elseif string.find(b, "%event", 1, true) then
                    b = trim(a)
                    eventList[b] = "s_"..theClass
                elseif string.find(b, "%object", 1, true) then
                    b = trim(a)
                    objectList[b] = "s_"..theClass
                elseif string.find(b, "%pointer", 1, true) then
                    b = trim(a)
                    pointerList[b] = "s_"..theClass
                else
                    defineList[b] = b
                end
            end
            break
        end
-- %endclass or %endstruct
        if string.find(a, "%endclass", 1, true) or string.find(a, "%endstruct", 1, true) then
            theClass = ""
            skipFlag = 0
            break
        end
-- %class <name> or %struct <name>
        if string.find(a, "%class", 1, true) == 1 or string.find(a, "%struct", 1, true) == 1 then
            local isClass = string.find(a, "%class", 1, true) == 1

            if skipFlag == -1 then
                skipFlag = -2
            else
                skipFlag = 0
                if isClass then
                    a, b = remove(a, "%class")
                else
                    a, b = remove(a, "%struct")
                end

                if string.find(a, "%alias", 1, true) then
                    a, b = remove(a, "%alias")
                    a, alias = remove(a, " ")
                else
                    alias = ""
                end

                deleteReq = nil
                if (string.find(a, "%delete", 1, true)) then
                    a, b = remove(a, "%delete")
                    deleteReq = 1
                end

                noClassInfo = nil
                if (string.find(a, "%noclassinfo", 1, true)) then
                    a, b = remove(a, "%noclassinfo")
                    noClassInfo = 1
                end

                local encapsulate = nil
                if (string.find(a, "%encapsulate", 1, true)) then
                    a, b = remove(a, "%encapsulate")
                    encapsulate = true
                end

                if string.find(a, ",", 1, true) then
                    a, theClass = remove(a, ",")
                    base = a
                else
                    theClass = a
                    base = nil
                end

                if (encapsulate) then
                    encapsulationList[theClass] = "DECLARE_ENCAPSULATION  ("..theClass..")\nIMPLEMENT_ENCAPSULATION("..theClass..")\n"
                end

                if fVerbose >= 2 then
                    print("")
                    print("Parsing class", theClass)
                end
                table.insert(class, theClass)
                classList[theClass] = base
                a = ""
                if hasText(alias) then
                    classList[alias] = theClass
                    alias = ""
                end
-- skip if builtin
                if theClass ~= "builtin" and isBlank(constructor) then
-- the destructor and delete routine
                    local delName = theClass.."_Delete"
                    local gcName  = theClass.."_destructor"
                    if deleteReq then
                        table.insert(codeList, "\nstatic int LUACALL "..gcName.."(lua_State *L)\n{")
                        table.insert(codeList, "\n    "..theClass.." * self = ("..theClass.."*)getuserdatatype(L, 1, s_"..theClass..");")
                        if (comment) then table.insert(codeList, "\n    // remove from tracked memory list") end
                        table.insert(codeList, "\n    if (self != 0)")
                        table.insert(codeList, "\n        removeTrackedMemory(L, self);")
                        table.insert(codeList, "\n    return 0;")
                        table.insert(codeList, "\n}\n")

                        table.insert(codeList, "\nstatic int LUACALL "..delName.."(lua_State *L)\n{")
                        table.insert(codeList, "\n    "..theClass.." * self = ("..theClass.."*)getuserdatatype(L, 1, s_"..theClass..");")
                        if (comment) then table.insert(codeList, "\n    // remove from tracked memory list") end
                        table.insert(codeList, "\n    if (self != 0)")
                        table.insert(codeList, "\n        if (removeTrackedMemory(L, self))")
                        table.insert(codeList, "\n        {")
                        if (comment) then table.insert(codeList, "\n            // if removed, reset the tag so that gc() is not called on this object.") end
                        table.insert(codeList, "\n            lua_pushnil(L);")
                        table.insert(codeList, "\n            lua_setmetatable(L, -2);")
                        table.insert(codeList, "\n        }")
                    else  -- no warning for unused parameters
                        table.insert(codeList, "\nstatic int LUACALL "..gcName.."(lua_State *)\n{")
                    end
                    table.insert(codeList, "\n    return 0;")
                    table.insert(codeList, "\n}\n")

-- link delete routine
                     if currentClass ~= theClass and isBlank(constructor) then
                        if currentClass ~= "" then
                            table.insert(wrapperList, "};\n\n")
                        end
                        local classPos = insertClass(theClass)
                        if classList[theClass] then
                            tagList[theClass] = "int s_"..theClass.." = -1;\n"
                            table.insert(wrapperList, "static WXLUAMETHOD "..theClass.."_methods[] = {\n")
                            if noClassInfo then
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", "..findBaseClass(classList[theClass], theClass)..", NULL, &s_"..theClass.." /*"..classPos.."*/ }, \n")
                            else
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", "..findBaseClass(classList[theClass], theClass)..", CLASSINFO("..theClass.."), &s_"..theClass.." /*"..classPos.."*/ }, \n")
                            end
                        else
                            tagList[theClass] = "int s_"..theClass.." = -1;\n"
                            table.insert(wrapperList, "static WXLUAMETHOD "..theClass.."_methods[] = {\n")
                            if noClassInfo then
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", -1, NULL, &s_"..theClass.." /*"..classPos.."*/ }, \n")
                            else
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", -1, CLASSINFO("..theClass.."), &s_"..theClass.." /*"..classPos.."*/ }, \n")
                            end
                        end
                        currentClass = theClass
                    end
                    table.insert(wrapperList, "    { ")
                    table.insert(wrapperList, "LuaDelete,      \""..theClass.."\", ")
                    table.insert(wrapperList, gcName..", 0, 0 },\n")
                    if deleteReq then
                        table.insert(wrapperList, "    { ")
                        table.insert(wrapperList, "LuaMethod,      \"Delete\", ")
                        table.insert(wrapperList, delName..", 0, 0 },\n")
                    end
                end
            end
-- %include file
        elseif string.find(a, "%include", 1, true) == 1 and skipFlag == 0 then
            a, b = remove(a, "%include")
            -- don't include multiple times
            if not include[a] then
                table.insert(includeList, "#include "..a.."\n")
                include[a] = 1
            end
            break
        end
-- ignore, not a method
        if not string.find(a, "(", 1, true) then
            break
        end
-- virtual
        if string.find(a, "virtual", 1, true) == 1 then
            a, b = remove(a, "virtual")
            funcVirtual = 1
        else
            funcVirtual = nil
        end
-- static
        if string.find(a, "static", 1, true) == 1 then
            a, b = remove(a, "static")
            funcStatic = 1
        else
            funcStatic = nil
        end

        -- skip constructor in abstract classes
        if theClass ~= "" then
            if string.find(a, theClass.."(", 1, true) then
                if isAbstract(theClass) then
                    break
                end
            end
        end

-- function type
        if hasText(a) and skipFlag == 0 then
            local type
            local funcConst  = nil
            local deref      = nil

-- remove const
            if string.find(a, "const", 1, true) == 1 then
                a, ignore = remove(a, "const")
                funcConst = 1
            else
                funcConst = nil
            end
-- constructor?
            if string.find(a, theClass.."(", 1, true) then
                type = "void"
            else
-- type should follow...
                a, type = remove(a, " ")
-- signed/unsigned?
                if type == "unsigned" or type == "signed" then
                    a, b = remove(a, " ")
                    type = type.." "..b
                end
            end
            ptr = ""
            extra = 0

-- pointers
            while string.find(a, "*", 1, true) == 1 do
                a, b = remove(a, "*")
                ptr = ptr.."*"
                extra = extra + 1
            end
            at = string.find(type, "*", 1, true)
            if at then
                ptr = string.sub(type, at, string.len(type) - (at - 1))
                type = string.sub(type, 1, at - 1)
                if extra == 0 then
                    extra = extra + 1
                end
            end
-- reference
            if string.find(type, "&", 1, true) then
                local pos = string.find(type, "&")
                type = string.sub(type, 1, pos - 1)
                ptr = "*"
                extra = extra + 1
                deref = 1
            else
                deref = nil
            end

            type = fixType(type)

            defList         = ""
            argList         = ""
            argCode         = ""
            hasOptional     = nil
            moreArgs        = 1
            args            = 0
            opts            = 0
            optFlag         = nil
            constructorFlag = hasText(constructor)

            a, func = remove(a, "(")
            local fullArg = a
            local posArg = string.find(fullArg, ")")
            if posArg ~= -1 then
                if posArg == nil then
                    print("Error: Syntax error:"..fullArg.." at line "..lineNumber.." in file "..inFileName)
                    print("Line: "..theLine)
                    error(1)
                end
                fullArg = string.sub(fullArg, 1, posArg)
            end

-- process args
            while moreArgs do
                retnum = "1"
                if string.find(a, ",", 1, true) then
                    a, arg = remove(a, ",")
                elseif string.find(a, ")", 1, true) then
                    -- allow for casts in arg
                    local leftBr = string.byte("(")
                    local rightBr = string.byte(")")
                    local castDepth = 0
                    for idx=1, string.len(a) do
                        if string.byte(a, idx) == rightBr then
                            if castDepth == 0 then
                                arg = string.sub(a, 1, idx - 1)
                                a = string.sub(a, idx + 1)
                                break
                            else
                                castDepth = castDepth - 1
                            end
                        elseif string.byte(a, idx) == leftBr then
                            castDepth = castDepth + 1
                        end
                    end
                    moreArgs = nil
                else
                    print("Error: parsing args")
                    print(theLine)
                    os.exit(1)
                end

-- decode the args
                if hasText(arg) then
                    local ctorArg = nil

                    if theClass == func then
                        ctorArg = 1
                    end

                    argList, arg, opt, optFlag, argCode, declare, argType = decodeArg(argList, arg, opt, argCode, ctorArg or hasText(builtin), theClass)
                    if optFlag then
                        opts = opts + 1
                    else
                        args = args + 1
                    end
                end
            end

            -- Do we know all data types of Function
            local classDataType = theClass

            -- Builtin? (global)
            if not isBlank(builtin) then
                classDataType = builtin
            end

            -- Constructor?
            if not isBlank(constructor) then
                classDataType = constructor
            end

            local determancy, msg  = GetDeterminancy(classDataType, func)
            if determancy < 1 then
                if fVerbose >= 1 then
                    print("Skipping "..classDataType.."::"..func..": Cannot Find DataType "..msg)
                end
                skipClassInPrototype = 1
            end

            if skipClassInPrototype == 0 then
                if type ~= "void" then
                    if type == "wxString" then
                        if deref or extra > 0 then
                            if funcConst then
                                defList = defList.."\n    const wxString *returns;"
                            else
                                defList = defList.."\n    wxString *returns;"
                            end
                        else
                            defList = defList.."\n    wxString returns;"
                        end
                    elseif not isIntrinsicDataType(type) then
                        if funcConst and extra > 0 then
                            defList = defList.."\n    const "..type.." *returns;"
                        else
                            defList = defList.."\n    "..type.." *returns;"
                        end
                    else
                        if funcConst then
                            defList = defList.."\n    const "..type.." "..ptr.."returns;"
                        else
                            if extra > 0 then
                                defList = defList.."\n    "..type.." ".."*".." returns;"
                            else
                                defList = defList.."\n    "..type.." "..ptr.."returns;"
                            end
                        end
                    end
                elseif constructorFlag then
                    defList = defList.."\n    "..constructor.." *returns;"
                elseif func == theClass then
                    defList = defList.."\n    "..theClass.." *returns;"
                elseif type == "void" and ptr ~= "" then
                    if funcConst then
                        defList = defList.."\n    const "..type.." "..ptr.."returns;"
                    else
                        if extra > 0 then
                            defList = defList.."\n    "..type.." ".."*".." returns;"
                        else
                            defList = defList.."\n    "..type.." "..ptr.."returns;"
                        end
                    end
                end

    -- make the function name
                if theClass == func then
                    funcName = theClass.."_constructor"
                    shortFuncName = theClass
                elseif func == "~"..theClass then
                    funcName = theClass.."_destructor"
                    shortFuncName = nil
                elseif hasText(rename) then
                    funcName = theClass.."_"..rename
                    shortFuncName = rename
                else
                    funcName = theClass.."_"..func
                    shortFuncName = func
                end

                if not replaceCode[funcName] then
                    local callfunc = nil
                    if fVerbose >= 2 and shortFuncName then
                        print("Generating function ", type.." "..shortFuncName.." ("..fullArg)
                    end
    -- generate the prototype
                    table.insert(codeList, "\n// "..theLine.."\nstatic int LUACALL "..funcName.."(lua_State *L)\n{")
                    table.insert(codeList, "    "..defList)

                    if optFlag then
                        if (comment) then table.insert(codeList, "\n    // get number of arguments") end
                        table.insert(codeList, "\n    int argCount = lua_gettop(L);")
                    end
                    if hasText(argCode) then
                        table.insert(codeList, "    "..argCode)
                    end

                    if constructorFlag then
                        if (comment) then table.insert(codeList, "\n    // call constructor") end
                        table.insert(codeList, "\n    returns = new "..constructor.."("..argList..");")
                        if deleteReq then
                            if (comment) then table.insert(codeList, "\n    // add to tracked memory list") end
                            table.insert(codeList, "\n    addToTrackedMemoryList(L, returns);")
                        elseif not noClassInfo then
                            if (comment) then table.insert(codeList, "\n    // add to tracked window list") end
                            if (theClass == "wxToolBar") then -- wxToolBar is a wxObject not a wxWindow
                                table.insert(codeList, "\n    if (wxDynamicCast(returns, wxToolBar) != NULL)")  --No good for gcc 3.4
                                --table.insert(codeList, "\n    if ((wxToolBar*)returns != NULL)")
                                table.insert(codeList, "\n        addToTrackedWindowList(L, (wxToolBar*)returns);")
                            elseif (constructorFlag and isKindOf(constructor, "wxWindow")) or (not constructorFlag and isKindOf(theClass, "wxWindow")) then
                                table.insert(codeList, "\n    if (wxDynamicCast(returns, wxWindow) != NULL)")  --No good for gcc 3.4
                                --table.insert(codeList, "\n    if ((wxWindow*)returns != NULL)")
                                table.insert(codeList, "\n        addToTrackedWindowList(L, (wxWindow*)returns);")
                            end
                        end
                    elseif theClass == func then
                        if (comment) then table.insert(codeList, "\n    // call constructor") end
                        table.insert(codeList, "\n    returns = new "..theClass.."("..argList..");")
                        if deleteReq then
                            if (comment) then table.insert(codeList, "\n    // add to tracked memory list") end
                            table.insert(codeList, "\n    addToTrackedMemoryList(L, returns);")
                        elseif not noClassInfo then
                            if (comment) then table.insert(codeList, "\n    // add to tracked window list") end
                            if (theClass == "wxToolBar") then
                                table.insert(codeList, "\n    if (wxDynamicCast(returns, wxToolBar) != NULL)")  --No good for gcc 3.4
                                --table.insert(codeList, "\n    if ((wxToolBar*)returns != NULL)")
                                table.insert(codeList, "\n        addToTrackedWindowList(L, (wxToolBar*)returns);")
                            elseif (constructorFlag and isKindOf(constructor, "wxWindow")) or (not constructorFlag and isKindOf(theClass, "wxWindow")) then
                                table.insert(codeList, "\n    if (wxDynamicCast(returns, wxWindow) != NULL)")  --No good for gcc 3.4
                                --table.insert(codeList, "\n    if ((wxWindow*)returns != NULL)")
                                table.insert(codeList, "\n        addToTrackedWindowList(L, (wxWindow*)returns);")
                            end
                        end
                    else
                        if theClass == "builtin" then
    -- not really a class
                            head = func
                        elseif funcStatic == 1 then
                            head = theClass.."::"..func
                        else
                            if (comment) then table.insert(codeList, "\n    // get this") end
                            table.insert(codeList, "\n    "..theClass.." *self = ("..theClass.."*)getuserdatatype(L, 1, s_"..theClass..");")
                            head = "self->"..func
                        end
                        callfunc = 1
                    end
                    if callfunc then
                        if (comment) then table.insert(codeList, "\n    // call "..func) end
                        if type == "void" then
                            if (string.find(ptr, "*")) then
                                table.insert(codeList, "\n    returns = "..head.."("..argList..");")
                            else
                                table.insert(codeList, "\n    "..head.."("..argList..");")
                            end
                        elseif type == "wxString" and extra == 0 then
                            table.insert(codeList, "\n    returns = "..head.."("..argList..");")
                        elseif not isIntrinsicDataType(type) and extra == 0 then
                            if (comment) then table.insert(codeList, "\n    // allocate a new object using the copy constructor") end
                            table.insert(codeList, "\n    returns = new "..type.."("..head.."("..argList.."));")
                            if (comment) then table.insert(codeList, "\n    // add the new object to the tracked memory list") end
                            table.insert(codeList, "\n    addToTrackedMemoryList(L, returns);")
                        elseif deref then
                            table.insert(codeList, "\n    returns = &"..head.."("..argList..");")
                        else
                            table.insert(codeList, "\n    returns = "..head.."("..argList..");")
                        end
                    end
                    if type == "void" then
                        if func == theClass then
                            if (comment) then table.insert(codeList, "\n    // push the constructed class pointer") end
                            if constructorFlag then
                                table.insert(codeList, "\n    pushuserdatatype(L, s_"..constructor..", returns);")
                            else
                                table.insert(codeList, "\n    pushuserdatatype(L, s_"..theClass..", returns);")
                            end
                        elseif ptr == "*" then
                            if (comment) then table.insert(codeList, "\n    // push the result flag") end
                            table.insert(codeList, "\n    lua_pushlightuserdata(L, returns);")
                        else
                            retnum = "0"
                        end
                    elseif type == "wxString" then
                        if (comment) then table.insert(codeList, "\n    // push the result string") end
                        if extra == 0 then
                            table.insert(codeList, "\n    lua_pushstring(L, wx2lua(returns));")
                        elseif extra > 0 then
                            table.insert(codeList, "\n    lua_pushstring(L, wx2lua(*returns));")
                        end
                    elseif not isIntrinsicDataType(type) then
                        if (comment) then table.insert(codeList, "\n    // push the result datatype") end
                        if deref or extra > 0 then
                            table.insert(codeList, "\n    pushuserdatatype(L, s_"..type..", returns);")
                        else
                            table.insert(codeList, "\n    pushuserdatatype(L, s_"..type..", returns);")
                        end
                    elseif type == "BOOL" or type == "bool" and extra >= 0 then
                        if (comment) then table.insert(codeList, "\n    // push the result flag") end
                        table.insert(codeList, "\n    lua_pushboolean(L, returns);")
                    else
                        if not isIntrinsicDataType(type) or extra > 0 then
                            if ptr == "*" or extra > 0 then
                                if (comment) then table.insert(codeList, "\n    // push the result pointer") end
                                table.insert(codeList, "\n    lua_pushlightuserdata(L, returns);")
                            else
                                if (comment) then table.insert(codeList, "\n    // push the result number") end
                                table.insert(codeList, "\n    lua_pushnumber(L, returns);")
                            end
                        else
                            if ptr == "*" or extra > 0 then
                                if (comment) then table.insert(codeList, "\n    // push the result pointer") end
                                table.insert(codeList, "\n    lua_pushlightuserdata(L, returns);")
                            else
                                if (comment) then table.insert(codeList, "\n    // push the result number") end
                                table.insert(codeList, "\n    lua_pushnumber(L, returns);")
                            end
                        end
                    end

                    if (comment) then table.insert(codeList, "\n    // return the number of parameters") end
                    table.insert(codeList, "\n    return "..retnum..";")
                    table.insert(codeList, "\n}\n");
                else
    -- use replacement custom code generator for this method
                    if (replaceCode[funcName].skip == 0) then
                        if fVerbose >= 2 then
                            print("Inserting (override) function  ", type.." "..shortFuncName.." ("..fullArg)
                        end
                        table.insert(codeList, replaceCode[funcName].override)
                    end
                end

                local emitName = nil

    -- remove class from name
                shortName = funcName
                shortName, ignore = removeLast(shortName, "_")
                if theClass == "builtin" then
                    table.insert(wrapperList, "    { ")
                else
                    if currentClass ~= theClass and not constructorFlag then
                        if currentClass ~= "" then
                            table.insert(wrapperList, "};\n\n")
                        end
                        local classPos = insertClass(theClass)
                        if classList[theClass] then
                            tagList[theClass] = "int s_"..theClass.." = -1;\n"
                            table.insert(wrapperList, "static WXLUAMETHOD "..theClass.."_methods[] = {\n")
                            if noClassInfo then
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", "..findBaseClass(classList[theClass], theClass)..", NULL, &s_"..theClass.." /*"..classPos.."*/ }, \n")
                            else
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", "..findBaseClass(classList[theClass], theClass)..", CLASSINFO(".. theClass .."), &s_"..theClass.." /*"..classPos.."*/ }, \n")
                            end
                        else
                            tagList[theClass] = "int s_"..theClass.." = -1;\n"
                            table.insert(wrapperList, "static WXLUAMETHOD "..theClass.."_methods[] = {\n")
                            if noClassInfo then
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", -1, NULL, &s_"..theClass..", /*"..classPos.."*/ }, \n")
                            else
                                table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", -1, CLASSINFO("..theClass.."), &s_"..theClass..", /*"..classPos.."*/ }, \n")
                            end
                        end
                        currentClass = theClass
                    end
                    table.insert(wrapperList, "    { ")
                end
                if shortName == "constructor" then
                    table.insert(wrapperList, "LuaConstructor, ")
                    table.insert(wrapperList, "\""..theClass.."\", ")
                elseif shortName == "destructor" then
                    table.insert(wrapperList, "LuaDestructor, ")
                    table.insert(wrapperList, "\""..theClass.."\", ")
                elseif theClass == "builtin" then
                    table.insert(wrapperList, "LuaGlobal,      \""..shortName.."\", ")
                else
                    table.insert(wrapperList, "LuaMethod,      \""..shortName.."\", ")
                end
                table.insert(wrapperList, funcName..", ")
                table.insert(wrapperList, args..", "..tostring(args + opts).." },\n")
    -- alias?
                if hasText(alias) then
                    if theClass == "builtin" then
                        table.insert(wrapperList, "    { LuaGlobal,     ")
                    else
                        if currentClass ~= theClass and not constructorFlag then
                            if currentClass ~= "" then
                                table.insert(wrapperList, "};\n\n")
                            end
                            local classPos = insertClass(theClass)
                            if classList[theClass] then
                                tagList[theClass] = "int s_"..theClass.." = -1;\n"
                                table.insert(wrapperList, "static WXLUAMETHOD "..theClass.."_methods[] = {\n")
                                if noClassInfo then
                                    table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", "..findBaseClass(classList[theClass], theClass)..", NULL, &s_"..theClass.." /*"..classPos.."*/ }, \n")
                                else
                                    table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", "..findBaseClass(classList[theClass], theClass)..", CLASSINFO(".. theClass .."), &s_"..theClass.." /*"..classPos.."*/ }, \n")
                                end
                            else
                                tagList[theClass] = "int s_"..theClass.." = -1;\n"
                                table.insert(wrapperList, "static WXLUAMETHOD "..theClass.."_methods[] = {\n")
                                if noClassInfo then
                                    table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", -1, NULL, &s_"..theClass.." /*"..classPos.."*/ }, \n")
                                else
                                    table.insert(controlList, "    { \""..theClass.."\", "..makeClassParam(theClass)..", -1, CLASSINFO("..theClass.."), &s_"..theClass.." /*"..classPos.."*/ }, \n")
                                end
                            end
                            currentClass = theClass
                        end
                        table.insert(wrapperList, "    { LuaMethod,      ")
                    end
                    table.insert(wrapperList, "\""..alias.."\", ")
                    table.insert(wrapperList, funcName..", ")
                    table.insert(wrapperList, tostring(args)..", "..tostring(args + opts).." },\n")
                end
            end
        end
    break end n = n + 1; end

    --------------------------------------------------------------------------
    -- Warn for errors
    --------------------------------------------------------------------------

-- check the missingTagList against the tagList
    do
        local error_count = 0
        local function validateTag(idx, val)
            if not tagList[idx] then
                for i=1, table.getn(val) do
                    error_count = error_count + 1
                    print(val[i])
                end
            end
        end
        table.foreach(missingTagList, validateTag)

        if error_count > 0 then
            print("")
            print(error_count.." Errors found, please check your luasetup.h file and the .i wrappers.")
            print("\""..fileout.."\" is not useable in this state.")
        end
        missingTagList = nil
    end

    --------------------------------------------------------------------------
    -- Write the output file
    --------------------------------------------------------------------------
    io.input()
-- generate the output file
    local outfile = io.output(fileout)
    outfile:write("// File generated from '"..filein.."' by 'wxLuaWrap.lua'. Do not edit.\n")

    outfile:write("#include \"wx/wx.h\"\n\n")

-- lets be careful and have compat level, unicode checks
	if (wxCompat20 == 1) then
		outfile:write("#if !WXWIN_COMPATIBILITY_2\n")
		outfile:write("    #error Lua Interface was generated with WXWIN_COMPATIBILITY_2 = 1\n")
		outfile:write("#endif\n\n")
    else
		outfile:write("#if WXWIN_COMPATIBILITY_2\n")
		outfile:write("    #error Lua Interface was generated with WXWIN_COMPATIBILITY_2 = 0\n")
		outfile:write("#endif\n\n")
	end

	if (wxCompat22 == 1) then
		outfile:write("#if !WXWIN_COMPATIBILITY_2_2\n")
		outfile:write("    #error Lua Interface was generated with WXWIN_COMPATIBILITY_2_2 = 1\n")
		outfile:write("#endif\n\n")
    else
		outfile:write("#if WXWIN_COMPATIBILITY_2_2\n")
		outfile:write("    #error Lua Interface was generated with WXWIN_COMPATIBILITY_2_2 = 0\n")
		outfile:write("#endif\n\n")
	end

	if (wxCompat24 == 1) then
		outfile:write("#if !WXWIN_COMPATIBILITY_2_4\n")
		outfile:write("    #error Lua Interface was generated with WXWIN_COMPATIBILITY_2_4 = 1\n")
		outfile:write("#endif\n\n")
    else
		outfile:write("#if WXWIN_COMPATIBILITY_2_4\n")
		outfile:write("    #error Lua Interface was generated with WXWIN_COMPATIBILITY_2_4 = 0\n")
		outfile:write("#endif\n\n")
	end

	if (wxUnicode == 1) then
		outfile:write("#if !wxUSE_UNICODE\n")
		outfile:write("    #error Lua Interface was generated with wxUSE_UNICODE = 1\n")
		outfile:write("#endif\n\n")
    else
		outfile:write("#if wxUSE_UNICODE\n")
		outfile:write("    #error Lua Interface was generated with wxUSE_UNICODE = 0\n")
		outfile:write("#endif\n\n")
	end

    local idx, val

--  Write the #include "file" list
    for idx = 1, table.getn(includeList) do
        val = includeList[idx]
        if val then
            outfile:write(val)
        end
    end
    includeList = nil
    outfile:write("\n")

--  Write the DECLARE_ENCAPSULATION list
    do
        local function writeTag(idx, val) outfile:write(val) end
        table.foreach(encapsulationList, writeTag)
    end
    encapsulationList = nil
    outfile:write("\n")

-- Write the tag list, the int s_wxClassNames
    do
        local function writeTag(idx, val) outfile:write(val) end
        table.foreach(tagList, writeTag)
    end

-- Write the tag list for wxLua derived wxWindow classes if they're not used
-- the C++ code should check that these are not -1 before use
    -- These are tags used in wxLuaHtmlWindow
    if tagList["wxLuaHtmlWindow"] == nil then outfile:write("int s_wxLuaHtmlWindow = -1;\n") end
    if tagList["wxHtmlCell"]      == nil then outfile:write("int s_wxHtmlCell = -1;\n") end
    if tagList["wxHtmlLinkInfo"]  == nil then outfile:write("int s_wxHtmlLinkInfo = -1;\n") end
    if tagList["wxMouseEvent"]    == nil then outfile:write("int s_wxMouseEvent = -1;\n") end
    -- these are tags used in wxLuaInternals
    if tagList["wxIcon"]          == nil then outfile:write("int s_wxIcon = -1;\n") end
    if tagList["wxWindow"]        == nil then outfile:write("int s_wxWindow = -1;\n") end
    if tagList["wxEvtHandler"]    == nil then outfile:write("int s_wxEvtHandler = -1;\n") end
    if tagList["wxEvent"]         == nil then outfile:write("int s_wxEvent = -1;\n") end
    if tagList["wxBitmap"]        == nil then outfile:write("int s_wxBitmap = -1;\n") end
    -- these are tags used in wxLuaPrintout
    if tagList["wxLuaPrintout"]   == nil then outfile:write("int s_wxLuaPrintout = -1;\n") end

    tagList = nil
    outfile:write("\n")

-- Write the code list out, the function wxClass_memberFunctions
    for idx = 1, table.getn(codeList) do
        val = codeList[idx]
        if val then
            outfile:write(val)
        end
    end
    codeList = nil
    outfile:write("\n")

--  Write the wrapperList, structs for each class containing the members
    for idx = 1, table.getn(wrapperList) do
        val = wrapperList[idx]
        if val then
            outfile:write(val)
        end
    end
    wrapperList = nil
    outfile:write("};\n\n")

    local count=0;

-- Write out the controlList (ClassList), a struct containing the wrapperList structs
    count = 0
    outfile:write("WXLUACLASS *GetClassList(size_t &count)\n{\n")
    outfile:write("    static WXLUACLASS classList[] =\n    {\n")
    for idx = 1, table.getn(controlList) do
        val = controlList[idx]
        if val then
            outfile:write("    "..val)
            count = count + 1
        end
    end
    controlList = nil

    outfile:write("    };\n")
    outfile:write("    count = ",count,";\n")
    outfile:write("    return classList;\n")
    outfile:write("};\n\n")

-- Write out the defineList and enumList, all the %defined and %enum items
    count = 0
    outfile:write("WXLUADEFINE *GetDefineList(size_t &count)\n{\n")
    outfile:write("    static WXLUADEFINE defineList[] =\n    {\n")
    for idx, val in pairs(defineList) do
        if idx ~= nil and idx ~= "n" and idx ~= "" and val then
            outfile:write("        { \"",idx,"\", ",val," },\n")
            count = count + 1
        end
    end
    defineList = nil

    for idx, val in pairs(enumValue) do
        if idx ~= nil and idx ~= "n" and idx ~= "" and val then
            outfile:write("        { \"",idx,"\", ",val," },\n")
            count = count + 1
        end
    end
    enumValue = nil

    outfile:write("    };\n")
    outfile:write("    count = ",count,";\n")
    outfile:write("    return defineList;\n")
    outfile:write("};\n\n")

-- Write out the stringList, all the static or #defined strings
    count = 0
    outfile:write("WXLUASTRING *GetStringList(size_t &count)\n{\n")
    outfile:write("    static WXLUASTRING stringList[] =\n    {\n")
    for idx, val in pairs(stringList) do
        if idx ~= nil and idx ~= "n" and idx ~= "" and val then
            outfile:write("        { \"",idx,"\", ", val, " },\n")
            count = count + 1
        end
    end
    stringList = nil

    outfile:write("    };\n")
    outfile:write("    count = ",count,";\n")
    outfile:write("    return stringList;\n")
    outfile:write("};\n\n")

-- Write out the eventList
    count = 0
    outfile:write("WXLUAEVENT *GetEventList(size_t &count)\n{\n")
    outfile:write("    static WXLUAEVENT eventList[] =\n    {\n")
    for idx, val in pairs(eventList) do
        if idx ~= nil and idx ~= "n" and idx ~= "" and val then
  			outfile:write("        { &",idx,", \"",idx,"\", &",val," },\n")
            count = count + 1
        end
    end
    eventList = nil

    outfile:write("    };\n")
    outfile:write("    count = ",count,";\n")
    outfile:write("    return eventList;\n")
    outfile:write("};\n\n")

-- Write out the objectList and pointerList
    count = 0
    outfile:write("WXLUAOBJECT *GetObjectList(size_t &count)\n{\n")
    outfile:write("    static WXLUAOBJECT objectList[] =\n    {\n")
    for idx, val in pairs(objectList) do
        if idx ~= nil and idx ~= "n" and idx ~= "" and val then
            outfile:write("        { &",idx,", 0, \"",idx,"\", &",val," },\n")
            count = count + 1
        end
    end
    objectList = nil

    for idx, val in pairs(pointerList) do
        if idx ~= nil and idx ~= "n" and idx ~= "" and val then
            outfile:write("        { 0, (const void **) &",idx,", \"",idx,"\", &",val," },\n")
            count = count + 1
        end
    end
    pointerList = nil

    outfile:write("    };\n")
    outfile:write("    count = ",count,";\n")
    outfile:write("    return objectList;\n")
    outfile:write("};\n")
    io.write()

    print("Generated File: "..fileout)
end


main()
