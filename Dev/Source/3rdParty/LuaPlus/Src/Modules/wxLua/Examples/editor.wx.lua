require "wxLua"

-------------------------------------------------------------------------=---
-- Name:        Editor.wx.lua
-- Purpose:     wxLua IDE
-- Author:      J Winwood
-- Created:     March 2002
-- Copyright:   (c) 2002 Lomtick Software. All rights reserved.
-- Licence:     wxWindows licence
-------------------------------------------------------------------------=---

local wxID_REDO             = wx.wxID_HIGHEST + 1
local wxID_SELECTALL        = wx.wxID_HIGHEST + 2
local wxID_FINDNEXT         = wx.wxID_HIGHEST + 3
local wxID_FINDPREV         = wx.wxID_HIGHEST + 4
local wxID_REPLACE          = wx.wxID_HIGHEST + 5
local wxID_GOTOLINE         = wx.wxID_HIGHEST + 6
local wxID_TOGGLEBREAKPOINT = wx.wxID_HIGHEST + 7
local wxID_COMPILE          = wx.wxID_HIGHEST + 8
local wxID_RUN              = wx.wxID_HIGHEST + 9
local wxID_ATTACH_DEBUG     = wx.wxID_HIGHEST + 10
local wxID_START_DEBUG      = wx.wxID_HIGHEST + 11
local wxID_STEP             = wx.wxID_HIGHEST + 12
local wxID_STEP_OVER        = wx.wxID_HIGHEST + 13
local wxID_STEP_OUT         = wx.wxID_HIGHEST + 14
local wxID_CONTINUE         = wx.wxID_HIGHEST + 15
local wxID_BREAK            = wx.wxID_HIGHEST + 16
local wxID_RESET            = wx.wxID_HIGHEST + 17
local wxID_VIEWCALLSTACK    = wx.wxID_HIGHEST + 18
local wxID_VIEWWATCHWINDOW  = wx.wxID_HIGHEST + 19
local wxID_ADDWATCH         = wx.wxID_HIGHEST + 20
local wxID_EDITWATCH        = wx.wxID_HIGHEST + 21
local wxID_REMOVEWATCH      = wx.wxID_HIGHEST + 22
local wxID_EVALUATEWATCH    = wx.wxID_HIGHEST + 23
local wxID_SAVEALL          = wx.wxID_HIGHEST + 24
local wxID_CLOSE            = wx.wxID_HIGHEST + 25
local wxID_CLEAROUTPUT      = wx.wxID_HIGHEST + 26
local wxID_AUTOCOMPLETE     = wx.wxID_HIGHEST + 27

local wxBREAKPOINT_MARKER         = 1
local wxBREAKPOINT_MARKER_VALUE   = 2

local wxCURRENT_LINE_MARKER       = 2
local wxCURRENT_LINE_MARKER_VALUE = 4

local wxIDWatchWindow             = 100

programName      = nil
findData         = nil    -- wxFindData object
fHaveFound       = nil    -- true when string has been found
editorApp        = wx.wxGetBaseApp()
debugServer      = nil    -- wxLuaDebugServer object when debugging
fIsRunning       = nil    -- true when the debuggee is running
fIsConnected     = nil    -- true when a client has connected
portNumber       = 1551
watchWindow      = nil
watchListCtrl    = nil
notebook         = nil
errorLog         = nil
openDocuments    = {}     -- table of open documents
ignoredFilesList = {}
editorID         = 100

frame = wx.wxFrame( wx.wxNull, -1, "wxLua")

editorApp:SetVendorName("Lomtick Software")
editorApp:SetAppName("wxLua IDE")

config = wx.wxConfigGet()
config:SetRecordDefaults()

toolBar   = wx.wxToolBar(frame,
                         -1,
                         wx.wxDefaultPosition,
                         wx.wxDefaultSize,
                         wx.wxNO_BORDER + wx.wxTB_FLAT + wx.wxTB_DOCKABLE)

if not standaloneLoadBitmaps then
    -- return the path part of the currently executing file
    function getPath()
        local strAt = string.byte('@')

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

    local bitmapPath = getPath().."/bitmaps/"
    newBitmap     = wx.wxBitmapFromFile(bitmapPath.."new.bmp",     wx.wxBITMAP_TYPE_BMP)
    openBitmap    = wx.wxBitmapFromFile(bitmapPath.."open.bmp",    wx.wxBITMAP_TYPE_BMP)
    saveBitmap    = wx.wxBitmapFromFile(bitmapPath.."save.bmp",    wx.wxBITMAP_TYPE_BMP)
    saveAllBitmap = wx.wxBitmapFromFile(bitmapPath.."saveall.bmp", wx.wxBITMAP_TYPE_BMP)
    cutBitmap     = wx.wxBitmapFromFile(bitmapPath.."cut.bmp",     wx.wxBITMAP_TYPE_BMP)
    copyBitmap    = wx.wxBitmapFromFile(bitmapPath.."copy.bmp",    wx.wxBITMAP_TYPE_BMP)
    pasteBitmap   = wx.wxBitmapFromFile(bitmapPath.."paste.bmp",   wx.wxBITMAP_TYPE_BMP)
    helpBitmap    = wx.wxBitmapFromFile(bitmapPath.."help.bmp",    wx.wxBITMAP_TYPE_BMP)
else
    standaloneLoadBitmaps()
end

toolBar:AddTool(wx.wxID_NEW,   newBitmap,     wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Creates an empty document")
toolBar:AddTool(wx.wxID_OPEN,  openBitmap,    wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Opens an existing document")
toolBar:AddTool(wx.wxID_SAVE,  saveBitmap,    wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Saves the document")
toolBar:AddTool(wxID_SAVEALL,  saveAllBitmap, wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Saves the document")
toolBar:AddSeparator()
toolBar:AddTool(wx.wxID_CUT,   cutBitmap,     wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Cut the selection")
toolBar:AddTool(wx.wxID_COPY,  copyBitmap,    wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Copy the selection")
toolBar:AddTool(wx.wxID_PASTE, pasteBitmap,   wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Paste the clipboard")
toolBar:AddSeparator()
toolBar:AddTool(wx.wxID_HELP,  helpBitmap,    wx.wxNullBitmap, wx.FALSE, wx.wxNull, "Display the About box")
toolBar:Realize()

statusBar = frame:CreateStatusBar( 4 )

function restoreFramePosition(window, windowName)
    local s, x, y, w, h

    config:SetPath("/"..windowName)

    s = config:ReadInt("s", -1)
    x = config:ReadInt("x", 0)
    y = config:ReadInt("y", 0)
    w = config:ReadInt("w", 0)
    h = config:ReadInt("h", 0)

    if s ~= -1 and s ~= 2 then

        local clientX, clientY, clientWidth, clientHeight
        clientX, clientY, clientWidth, clientHeight = wx.wxClientDisplayRect()

        if x < clientX - 5 then
            x = 0
        end
        if y < clientY - 5 then
            y = 0
        end

        if w > clientWidth then
            w = clientWidth
        end

        if h > clientHeight then
            h = clientHeight
        end

        window:Move(x, y)
        window:SetClientSizeWH(w, h)
    end

    if s == 1 then
        window:Maximize(wx.TRUE)
    end

    config:SetPath("/")
end

function saveFramePosition(window, windowName)
    local s, x, y, w, h

    config:SetPath("/"..windowName)

    s    = 0
    w, h = window:GetClientSize()
    x, y = window:GetPosition()

    if window:IsMaximized() == wx.TRUE then
        s = 1
    elseif window:IsIconized() == wx.TRUE then
        s = 2
    end

    config:WriteInt("s", s)

    if s == 0 then
        config:WriteInt("x", x)
        config:WriteInt("y", y)
        config:WriteInt("w", w)
        config:WriteInt("h", h)
    end

    config:SetPath("/")
end

function getFonts()
    local font, fontItalic
    if wx.wxPlatformWindows then
        font       = wx.wxFont(10, wx.wxNORMAL, wx.wxNORMAL, wx.wxNORMAL, wx.FALSE, "Andale Mono")
        fontItalic = wx.wxFont(10, wx.wxNORMAL, wx.wxFONTSTYLE_ITALIC, wx.wxNORMAL, wx.FALSE, "Andale Mono")
    else
        font       = wx.wxFont(12, wx.wxNORMAL, wx.wxNORMAL, wx.wxNORMAL, wx.FALSE, "lucidatypewriter")
        fontItalic = wx.wxFont(12, wx.wxNORMAL, wx.wxFONTSTYLE_ITALIC, wx.wxNORMAL, wx.FALSE, "lucidatypewriter")
    end
    return font, fontItalic
end

splitter = wx.wxSplitterWindow(frame,
                               -1,
                               wx.wxDefaultPosition,
                               wx.wxDefaultSize,
                               wx.wxSP_3DSASH)

notebook = wx.wxNotebook(splitter, -1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxCLIP_CHILDREN)

function getEditor(selection)
    local editor    = nil
    if not selection then
        selection = notebook:GetSelection()
    end
    if notebook:GetPageCount() > 0 then
        editor = notebook:GetPage(selection):DynamicCast("wxStyledTextCtrl")
    end
    return editor
end

function updateStatusText(editor)
    if editor then
        local pos = editor:GetCurrentPos()
        local row = editor:LineFromPosition(pos)
        local col = 1 + pos - editor:PositionFromLine(row)

        frame:SetStatusText("Ln "..tostring(row + 1).." Col "..tostring(col), 3)

        if editor:GetOvertype() == wx.TRUE then
            frame:SetStatusText("OVR", 1)
        else
            frame:SetStatusText("INS", 1)
        end

        if editor:GetReadOnly() == wx.TRUE then
            frame:SetStatusText("R/O", 2)
        else
            frame:SetStatusText("R/W", 2)
        end
    else
        frame:SetStatusText("", 1)
        frame:SetStatusText("", 2)
        frame:SetStatusText("", 3)
    end
end

function getModTime(filePath)
        local fileName = wx.wxFileName(filePath)
        return fileName:GetModificationTime();
end

function isAlteredOnDisk(editor)
    local id = editor:GetId()
    if openDocuments[id] then
        local filePath = openDocuments[id].filePath
        local fileName = openDocuments[id].fileName
        local oldModTime = openDocuments[id].modTime
        if filePath and (string.len(filePath) > 0) then
            if oldModTime:IsValid() then
                local modTime = getModTime(filePath)
                if oldModTime ~= nil and modTime:IsValid() and oldModTime:IsEarlierThan(modTime) then
                    if wx.wxMessageBox(fileName.." has been modified. Do you want to reload it?",
                                                "wxLua Message",
                                                wx.wxYES_NO + wx.wxCENTRE,
                                                frame) == wx.wxYES then
                        local handle = io.open(filePath, "rb")
                        if handle then
                            local st = handle:read("*a")
                            handle:close()
                            editor:Clear()
                            editor:ClearAll()
                            editor:MarkerDeleteAll(wxBREAKPOINT_MARKER)
                            editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                            editor:InsertText(0, st)
                            setupKeywords(editor, isLuaFile(filePath))
                            editor:EmptyUndoBuffer()
                            modTime = getModTime(filePath)
                            setDocumentModified(id, nil, false)
                        end
                    end
                else
                    openDocuments[id].modTime = modTime
                end
            end
        end
    end
end

function setEditorSelection(selection)
    local editor = getEditor(selection)
    if editor then
        editor:SetFocus()
        isAlteredOnDisk(editor)
    end
    updateStatusText(editor)
end

notebook:ConnectEvent(wx.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                      function ( event )
                          local selection = event:GetSelection()
                          event:SetSelection(selection)
                          setEditorSelection(selection)
                          event:Skip() -- do not understand why this is necessary
                      end)

function CreateOutput()
    local errorLog = wx.wxStyledTextCtrl(splitter, -1)
    errorLog:SetFont(font)
    errorLog:StyleSetFont(wx.wxSTC_STYLE_DEFAULT, font)
    errorLog:StyleClearAll()
    errorLog:SetReadOnly(wx.TRUE)
    return errorLog
end

splitter:Initialize(notebook)

font, fontItalic = getFonts()

function createUserList(key, letter)
    local prefix
    local userList = {}
    if letter < "a" then
        prefix = "WX"..string.char(key)
    else
        prefix = "wx"..string.char(key)
    end
    for index, value in wx do
        if string.sub(index, 1, 3) == prefix then
            table.insert(userList, index.." ")
        end
    end
    table.sort(userList)
    return table.concat(userList)
end

function setDocumentModified(id, modified, resetModTime)
    local fileName = openDocuments[id].fileName
    if not fileName then
        fileName = "Untitled"
    end
    if modified then
        openDocuments[id].isModified = 1
        notebook:SetPageText(openDocuments[id].index, "* "..fileName)
    else
        local filePath = openDocuments[id].filePath
        openDocuments[id].isModified = nil
        if resetModTime and filePath and string.len(filePath) > 0 then
            openDocuments[id].modTime = getModTime(filePath)
        end
        notebook:SetPageText(openDocuments[id].index, fileName)
    end
end

function createEditor(name)
    local editor = wx.wxStyledTextCtrl(notebook, editorID, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxSUNKEN_BORDER)
    editorID = editorID + 1

    editor:SetBufferedDraw(wx.TRUE)
    editor:SetFont(font)
    editor:StyleSetFont(wx.wxSTC_STYLE_DEFAULT, font)
    editor:StyleClearAll()
    editor:StyleSetForeground(0,  wx.wxColour(128, 128, 128))          -- White space
    editor:StyleSetForeground(1,  wx.wxColour(0, 127, 0))              -- Comment
    editor:StyleSetFont(1, fontItalic)
	editor:StyleSetUnderline(1, wx.FALSE)
    editor:StyleSetForeground(2,  wx.wxColour(0, 127, 0))              -- Line Comment
    editor:StyleSetFont(2, fontItalic)                                 -- Doc. Comment
	editor:StyleSetUnderline(2, wx.FALSE)
    editor:StyleSetForeground(3,  wx.wxColour(127, 127, 127))          -- Number
    editor:StyleSetForeground(4,  wx.wxColour(0, 127, 127))            -- Keyword
    editor:StyleSetForeground(5,  wx.wxColour(0, 0, 127))              -- Double quoted string
    editor:StyleSetBold(5,  wx.TRUE)
	editor:StyleSetUnderline(5, wx.FALSE)
    editor:StyleSetForeground(6,  wx.wxColour(127, 0, 127))            -- Single quoted string
    editor:StyleSetForeground(7,  wx.wxColour(127, 0, 127))            -- not used
    editor:StyleSetForeground(8,  wx.wxColour(0, 127, 127))            -- Literal strings
    editor:StyleSetForeground(9,  wx.wxColour(127, 127, 0))            -- Preprocessor
    editor:StyleSetForeground(10, wx.wxColour(0, 0, 0))                -- Operators
    editor:StyleSetBold(10, wx.TRUE)
    editor:StyleSetForeground(11, wx.wxColour(0, 0, 0))                -- Identifiers
    editor:StyleSetForeground(12, wx.wxColour(0, 0, 0))                -- Unterminated strings
    editor:StyleSetBackground(12, wx.wxColour(224, 192, 224))
    editor:StyleSetBold(12, wx.TRUE)
    editor:StyleSetEOLFilled(12, wx.TRUE)

    editor:StyleSetForeground(13, wx.wxColour(0, 0, 95))               -- Keyword highlighting styles
    editor:StyleSetForeground(14, wx.wxColour(0, 95, 0))
    editor:StyleSetForeground(15, wx.wxColour(127, 0, 0))
    editor:StyleSetForeground(16, wx.wxColour(127, 0, 95))
    editor:StyleSetForeground(17, wx.wxColour(63, 95, 95))

    editor:StyleSetForeground(18, wx.wxColour(0, 127, 127))            -- Nested literal strings ???
    editor:StyleSetBackground(18, wx.wxColour(240,255,255))
    editor:StyleSetForeground(19, wx.wxColour(0, 127, 127))
    editor:StyleSetBackground(19, wx.wxColour(224,255,255))
    editor:StyleSetForeground(20, wx.wxColour(0, 127, 127))
    editor:StyleSetBackground(20, wx.wxColour(192,255,255))
    editor:StyleSetForeground(21, wx.wxColour(0, 127, 127))
    editor:StyleSetBackground(21, wx.wxColour(176,255,255))
    editor:StyleSetForeground(22, wx.wxColour(0, 127, 127))
    editor:StyleSetBackground(22, wx.wxColour(160,255,255))
    editor:StyleSetForeground(23, wx.wxColour(0, 127, 127))
    editor:StyleSetBackground(23, wx.wxColour(144,255,255))
    editor:StyleSetForeground(24, wx.wxColour(0, 127, 127))
    editor:StyleSetBackground(24, wx.wxColour(128,155,255))

    editor:StyleSetForeground(32, wx.wxColour(224, 192, 224))          -- Line number
    editor:StyleSetBackground(33, wx.wxColour(192, 192, 192))          -- Brace highlight
    editor:StyleSetForeground(34, wx.wxColour(0, 0, 255))
    editor:StyleSetBold(34, wx.TRUE)                                   -- Brace incomplete highlight
    editor:StyleSetForeground(35, wx.wxColour(255, 0, 0))
    editor:StyleSetBold(35, wx.TRUE)                                   -- Indentation guides
    editor:StyleSetForeground(37, wx.wxColour(192, 192, 192))
    editor:StyleSetBackground(37, wx.wxColour(255, 255, 255))
    editor:SetUseTabs(wx.FALSE)
    editor:SetTabWidth(4)
    editor:SetIndent(4)
    editor:SetIndentationGuides(wx.TRUE)
    editor:SetMarginSensitive(1, wx.TRUE)
    editor:MarkerDefine(wxBREAKPOINT_MARKER,   wx.wxSTC_MARK_CIRCLE, wx.wxGREEN, wx.wxGREEN)
    editor:MarkerDefine(wxCURRENT_LINE_MARKER, wx.wxSTC_MARK_ARROW,  wx.wxRED,   wx.wxRED)
    editor:SetVisiblePolicy(wx.wxSTC_VISIBLE_SLOP, 3)
    editor:SetXCaretPolicy(wx.wxSTC_CARET_SLOP, 10)
    editor:SetYCaretPolicy(wx.wxSTC_CARET_SLOP, 3)

    editor:ConnectEvent(wx.wxEVT_STC_MARGINCLICK,
                        function ( event )
                            local row = editor:LineFromPosition(event:GetPosition())
                            toggleDebugMarker(editor, row)
                        end)

    editor:ConnectEvent(wx.wxEVT_STC_CHARADDED,
                        function ( event )
                            -- auto-indent
                            local ch = event:GetKey()
                            if ch == 13 or ch == 10 then
                                local lineNum = editor:LineFromPosition(editor:GetCurrentPos())
                                local lineLength = editor:LineLength(lineNum)
                                if lineNum > 0 and lineLength <= 2 then
                                    local lineOffset
                                    lineNum = lineNum - 1
                                    lineLength = editor:LineLength(lineNum)
                                    prevLine   = editor:GetLine(lineNum)
                                    for lineOffset = 1, lineLength do
                                        ch = string.byte(string.sub(prevLine, lineOffset))
                                        if ch ~= 32 and ch ~= 9 then
                                            editor:ReplaceSelection(string.sub(prevLine, 1, lineOffset - 1))
                                            break
                                        end
                                    end
                                end
                            else
                            -- code completion prompt
                                local pos = editor:GetSelection()
                                local lineStart = editor:LineFromPosition(pos)
                                if editor:PositionFromLine(lineStart) <= (pos - 6) then
                                    local range = editor:GetTextRange(pos - 6, pos - 1)
                                    if ((range == "wx.WX") or (range == "wx.wx")) then
                                        local commandEvent = wx.wxCommandEvent(wx.wxEVT_COMMAND_MENU_SELECTED,
                                                                               wxID_AUTOCOMPLETE)
                                        wx.wxPostEvent(frame, commandEvent)
                                    end
                                end
                            end
                        end)

    editor:ConnectEvent(wx.wxEVT_STC_USERLISTSELECTION,
                        function ( event )
                            local pos = editor:GetSelection()
                            editor:SetSelection(pos - 3, pos)
                            editor:ReplaceSelection(event:GetText())
                        end)

    editor:ConnectEvent(wx.wxEVT_STC_MODIFIED,
                        function ( event )
                        end)

    editor:ConnectEvent(wx.wxEVT_STC_SAVEPOINTREACHED,
                        function ( event )
                            setDocumentModified(editor:GetId(), nil, false)
                        end)

    editor:ConnectEvent(wx.wxEVT_STC_SAVEPOINTLEFT,
                        function ( event )
                            setDocumentModified(editor:GetId(), 1, false)
                        end)

    editor:ConnectEvent(wx.wxEVT_STC_UPDATEUI,
                        function ( event )
                            updateStatusText(editor)
                        end)

    if notebook:AddPage(editor, name, wx.TRUE) ~= 0 then
        local id = editor:GetId()
        local document = {}
        document.isModified = nil
        document.fileName   = nil
        document.filePath   = nil
        document.editor     = editor
        document.index      = notebook:GetSelection()
        document.modTime = nil
        openDocuments[id] = document
    end
    return editor
end

function isLuaFile(filePath)
    return string.lower(string.sub(filePath, -4)) == ".lua"
end

function setupKeywords(editor, useLuaParser)
    if useLuaParser then
        editor:SetLexer(wx.wxSTC_LEX_LUA)
        editor:SetKeyWords(0,
                           [[and break do else elseif end false for function if in
                             local nil not or repeat return then true until while]])
        editor:SetKeyWords(1,
                           [[_PROMPT _VERSION
                            _G _VERSION assert collectgarbage dofile error gcinfo getfenv getmetatable
                            ipairs loadfile loadlib loadstring next pairs pcall print rawequal
                            rawget rawset require setfenv setmetatable tonumber tostring type unpack xpcall]])
        editor:SetKeyWords(2,
                           [[abs acos asin atan atan2 ceil cos deg exp
                            floor frexp ldexp log log10
                            max min mod pi pow
                            rad random randomseed sin sort sqrt tan]])
        editor:SetKeyWords(3,
                           [[coroutine debug io os string table wx]])
        editor:SetKeyWords(4,
                           [[byte
                             char clock close concat create
                             date debug difftime dump
                             execute exit
                             find flush foreach foreachi format
                             getenv gethook getinfo getlocal getn getupvalue gfind gsub
                             input insert
                             len lines lower
                             open output
                             popen
                             read remove rename rep resume
                             setn status seek sethook setlocal
                             setlocale setupvalue sort
                             stderr stdin stdout sub
                             time type tmpfile tmpname traceback
                             upper
                             wrap write
                             yield]])

        local wxkeywords = {}
        for index, value in wx do
            table.insert(wxkeywords, index.." ")
        end

        editor:SetKeyWords(5, table.concat(wxkeywords))

    else
        editor:SetLexer(wx.wxSTC_LEX_NULL)
        editor:SetKeyWords(0, "")
    end
end

if not args then
    local editor = createEditor("Untitled")
    setupKeywords(editor, 1)
end

menuBar = wx.wxMenuBar()
fileMenu = wx.wxCreateMenu{
                        { wx.wxID_NEW,    "&New\tCtrl-N",        "Creates an empty document" },
                        { wx.wxID_OPEN,   "&Open...\tCtrl-O",    "Opens an existing document" },
                        { wx.wxID_SAVE,   "&Save\tCtrl-S",       "Saves the current document" },
                        { wx.wxID_SAVEAS, "Save &As...\tCtrl-A", "Saves the current document to a file with a new name" },
                        { wxID_SAVEALL,   "Save &All...",        "Saves all open documents" },
                        { wxID_CLOSE,     "&Close\tCtrl+W",      "Close the current window" },
                        { },
                        { wx.wxID_EXIT,   "E&xit\tAlt-X",        "Exit Application" }
                       }
menuBar:Append(fileMenu, "&File")

function saveFile(editor, filePath)
    local saved = nil
    local st    = editor:GetText()
    local backPath = filePath..".bak"
    os.remove(backPath)
    os.rename(filePath, backPath)
    local handle = io.open(filePath, "wb")
    if handle then
        handle:write(st)
        handle:close()
        editor:EmptyUndoBuffer()
        setDocumentModified(editor:GetId(), nil, true)
        saved = 1
    end
    return saved
end

function saveAs(editor)
    local saved      = nil
    local fileDialog = wx.wxFileDialog(frame,
                                       "Save file as",
                                       "",
                                       "",
                                       "Lua files(*.lua)|*.lua|Text files(*.txt)|*.txt|All files(*.*)|*.*",
                                       wx.wxSAVE)
    if fileName then
        fileDialog:SetFilename( fileName )
    end
    if fileDialog:ShowModal() == wx.wxID_OK then
        local filePath = fileDialog:GetPath()
        local fileName = fileDialog:GetFilename()
        local id       = editor:GetId()
        if saveFile(editor, filePath) then
            setupKeywords(editor, isLuaFile(filePath))
            openDocuments[id].filePath = filePath
            openDocuments[id].fileName = fileName
            openDocuments[id].modTime = getModTime(filePath)
            notebook:SetPageText(openDocuments[id].index, fileName)
            saved = 1
        end
    end
    fileDialog:Destroy()
    return saved
end

function fileNew( event )
    local editor = createEditor("Untitled")
    setupKeywords(editor, 1)
end
frame:ConnectEvent(wx.wxID_NEW, wx.wxEVT_COMMAND_MENU_SELECTED, fileNew)

function findDocumentToReuse()
    local editor = nil
    for id, document in openDocuments do
        local filePath = document.filePath
        if not document.isModified and
           not filePath            and
           not (document.editor:GetReadOnly() == wx.TRUE) then
            editor = document.editor
            break
        end
    end
    return editor
end

function loadFile(filePath, fileName)
    local result = nil
    local editor = findDocumentToReuse()
    if not editor then
        editor = createEditor(fileName)
    end
    local handle = io.open(filePath, "rb")
    if handle then
        local st = handle:read("*a")
        handle:close()
        editor:Clear()
        editor:ClearAll()
        editor:MarkerDeleteAll(wxBREAKPOINT_MARKER)
        editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
        editor:InsertText(0, st)
        setupKeywords(editor, isLuaFile(filePath))
        editor:EmptyUndoBuffer()
        local id = editor:GetId()
        openDocuments[id].filePath = filePath
        openDocuments[id].fileName = fileName
        setDocumentModified(id, nil, true)
        result = editor
    end
    return result
end

function fileOpen(event)
    local fileDialog = wx.wxFileDialog(frame,
                                       "Open file",
                                       "",
                                       "",
                                       "Lua files(*.lua)|*.lua|Text files(*.txt)|*.txt|All files(*.*)|*.*",
                                       wx.wxOPEN)
    if fileDialog:ShowModal() == wx.wxID_OK then
        loadFile(fileDialog:GetPath(), fileDialog:GetFilename())
    end
    fileDialog:Destroy()
end
frame:ConnectEvent(wx.wxID_OPEN, wx.wxEVT_COMMAND_MENU_SELECTED, fileOpen )

frame:ConnectEvent(wx.wxID_SAVEAS,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        saveAs(editor)
                   end)

frame:ConnectEvent(wx.wxID_SAVEAS,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        if editor then
                            local id = editor:GetId()
                            if openDocuments[id] then
                                event:Enable(openDocuments[id].isModified ~= nil)
                            end
                        else
                            event:Enable(wx.FALSE)
                        end
                   end)

frame:ConnectEvent(wx.wxID_SAVE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor   = getEditor()
                        local id       = editor:GetId()
                        local filePath = openDocuments[id].filePath
                        if not filePath then
                            saveAs(editor)
                        else
                            saveFile(editor, filePath)
                        end
                    end)

frame:ConnectEvent(wx.wxID_SAVE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       local editor = getEditor()
                       if editor then
                           local id = editor:GetId()
                           if openDocuments[id] then
                               event:Enable(openDocuments[id].isModified ~= nil)
                           end
                       end
                   end)

function removePage(index)
    local  prevIndex = nil
    local  nextIndex = nil
    local newOpenDocuments = {}
    for id, document in openDocuments do
        if document.index < index then
            newOpenDocuments[id] = document
            prevIndex = document.index
        elseif document.index == index then
            document.editor:Destroy()
        elseif document.index > index then
            document.index = document.index - 1
            if nextIndex == nil then
                nextIndex = document.index
            end
            newOpenDocuments[id] = document
        end
    end
    notebook:RemovePage(index)
    openDocuments = newOpenDocuments
    if nextIndex then
        notebook:SetSelection(nextIndex)
    elseif prevIndex then
        notebook:SetSelection(prevIndex)
    end
    setEditorSelection(nil)
end

frame:ConnectEvent(wxID_CLOSE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local saved    = 1
                        local editor   = getEditor()
                        local id       = editor:GetId()
                        local document = openDocuments[id]
                        local filePath = document.filePath
                        local fileName = document.fileName
                        if document.isModified then
                            local message
                            if fileName then
                                message = "Save changes to '"..fileName.."' before exiting?"
                            else
                                message = "Save changes to 'Untitled' before exiting?"
                            end
                            local dialog = wx.wxMessageDialog( frame,
                                                               message,
                                                               "Save Changes?",
                                                               wx.wxYES_NO + wx.wxCENTRE + wx.wxICON_QUESTION )
                            local result = dialog:ShowModal()
                            dialog:Destroy()
                            if result == wx.wxID_YES then
                                if not filePath then
                                    saved = saveAs(editor)
                                else
                                    saved = saveFile(editor, filePath)
                                end
                            end
                        end
                        if saved then
                            removePage(openDocuments[id].index)
                        end
                    end)

frame:ConnectEvent(wxID_CLOSE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       local editor = getEditor()
                       event:Enable(editor ~= nil and debugServer == nil)
                   end)

function saveAll()
    for id, document in openDocuments do
        local editor   = document.editor
        local filePath = document.filePath
        if document.isModified then
            if not filePath then
                saveAs(editor)
            elseif saveFile(editor, filePath) then
                setDocumentModified(editor:GetId(), nil, false)
            end
        end
    end
end

frame:ConnectEvent(wxID_SAVEALL,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       local atLeastOneModifiedDocument = wx.FALSE
                       for id, document in openDocuments do
                           if document.isModified then
                               atLeastOneModifiedDocument = wx.TRUE
                               break
                           end
                       end
                       event:Enable(atLeastOneModifiedDocument)
                   end)

frame:ConnectEvent(wxID_SAVEALL,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local canRun = true
                        local filePath = {}
                        saveAll()
                        for id, document in openDocuments do
                            if document.isModified then
                                canRun = false
                            else
                                local filePath = document.filePath
                                table.insert(fileList, filePath.." ")
                            end
                        end
                        if canRun then
                            local files = table.concat(fileList)
                            wx.wxExecute("wxLua "..files)
                        end
                   end)

function saveOnExit()
    for id, document in openDocuments do
        local editor   = document.editor
        local fileName = document.fileName
        if document.isModified then
            local message
            if fileName then
                message = "Save changes to '"..fileName.."' before exiting?"
            else
                message = "Save changes to 'Untitled' before exiting?"
            end
            local dialog = wx.wxMessageDialog( frame,
                                               message,
                                               "Save Changes?",
                                               wx.wxYES_NO + wx.wxCENTRE + wx.wxICON_QUESTION )
            local result = dialog:ShowModal()
            dialog:Destroy()
            if result == wx.wxID_YES then
                local filePath = document.filePath
                if not filePath then
                    saveAs(editor)
                else
                    saveFile(editor, filePath)
                end
            end
        end
        document.isModified = nil
    end
end

function enableMenuItem( event )
    local editor = getEditor()
    event:Enable(editor ~= nil)
end

function findSelectedItem()
    local count = watchListCtrl:GetSelectedItemCount()
    if count > 0 then
        for idx = 0, watchListCtrl:GetItemCount() - 1 do
            if watchListCtrl:GetItemState(idx, wx.wxLIST_STATE_FOCUSED) ~= 0 then
                return idx
            end
        end
    end
    return -1
end

function processWatches()
    if watchListCtrl and debugServer then
        local count = watchListCtrl:GetItemCount()
        if count > 0 then
            for idx = 0, count - 1 do
                local expression = watchListCtrl:GetItemText(idx)
                debugServer:EvaluateExpr(idx, expression)
            end
        end
    end
end

function createWatchWindow()
    watchWindow = wx.wxFrame( frame, -1, "Watch Window", wx.wxDefaultPosition, wx.wxSize(190, 160))

    local watchMenu = wx.wxCreateMenu{
                                        { wxID_ADDWATCH,      "&Add Watch"        },
                                        { wxID_EDITWATCH,     "&Edit Watch\tF2"   },
                                        { wxID_REMOVEWATCH,   "&Remove Watch"     },
                                        { wxID_EVALUATEWATCH, "Evaluate &Watches" }
                                     }
    local watchMenuBar = wx.wxMenuBar()

    watchMenuBar:Append(watchMenu, "&Watches")
    watchWindow:SetMenuBar(watchMenuBar)

    local x, y = watchWindow:GetClientSize()
    watchListCtrl = wx.wxListCtrl(watchWindow, wxIDWatchWindow, wx.wxPoint(0, 0), wx.wxSize(x, y), wx.wxLC_REPORT + wx.wxLC_EDIT_LABELS)

    x, y = watchListCtrl:GetClientSize()

    local info = wx.wxListItem()
    info:SetMask(wx.wxLIST_MASK_TEXT + wx.wxLIST_MASK_WIDTH)
    info:SetText("Expression")
    info:SetWidth(x / 2)
    watchListCtrl:InsertColumn(0, info)

    info:SetText("Value")
    info:SetWidth(x / 2)
    watchListCtrl:InsertColumn(1, info)

    watchWindow:CentreOnParent()
    restoreFramePosition(watchWindow, "WatchWindow")
    watchWindow:Show(wx.TRUE)

    watchWindow:ConnectEvent( wx.wxEVT_CLOSE_WINDOW,
                                function ( event )
                                    saveFramePosition(watchWindow, "WatchWindow")
                                    event:Skip()
                                    watchWindow = nil
                                    watchListCtrl = nil
                                end)

    watchWindow:ConnectEvent( wx.wxEVT_SIZE,
                              function ( event )
                                 local x, y = watchWindow:GetClientSize()
                                 watchListCtrl:SetSize(x, y)
                                 event:Skip()
                              end)

    watchWindow:ConnectEvent(wxID_ADDWATCH,
                             wx.wxEVT_COMMAND_MENU_SELECTED,
                             function ( event )
                                local row = watchListCtrl:InsertStringItem(watchListCtrl:GetItemCount(), "Expr")
                                watchListCtrl:SetStringItem(row, 0, "Expr")
                                watchListCtrl:SetStringItem(row, 1, "Value")
                                watchListCtrl:EditLabel(row)
                             end)
    watchWindow:ConnectEvent(wxID_ADDWATCH,
                             wx.wxEVT_UPDATE_UI,
                             function ( event )
                               event:Enable(wx.TRUE)
                             end)

    watchWindow:ConnectEvent(wxID_EDITWATCH,
                             wx.wxEVT_COMMAND_MENU_SELECTED,
                             function ( event )
                                local row = findSelectedItem()
                                if row >= 0 then
                                    watchListCtrl:EditLabel(row)
                                end
                             end)
    watchWindow:ConnectEvent(wxID_EDITWATCH,
                             wx.wxEVT_UPDATE_UI,
                             function ( event )
                               event:Enable(watchListCtrl:GetSelectedItemCount() > 0)
                             end)

    watchWindow:ConnectEvent(wxID_REMOVEWATCH,
                             wx.wxEVT_COMMAND_MENU_SELECTED,
                             function ( event )
                                local row = findSelectedItem()
                                if row >= 0 then
                                    watchListCtrl:DeleteItem(row)
                                end
                            end)
    watchWindow:ConnectEvent(wxID_REMOVEWATCH,
                             wx.wxEVT_UPDATE_UI,
                             function ( event )
                               event:Enable(watchListCtrl:GetSelectedItemCount() > 0)
                             end)

    watchWindow:ConnectEvent(wxID_EVALUATEWATCH,
                             wx.wxEVT_UPDATE_UI,
                             function ( event )
                               event:Enable(watchListCtrl:GetItemCount() > 0)
                             end)
    watchWindow:ConnectEvent(wxID_EVALUATEWATCH,
                             wx.wxEVT_COMMAND_MENU_SELECTED,
                             function ( event )
                                 processWatches()
                             end)

    watchListCtrl:ConnectEvent(wx.wxEVT_COMMAND_LIST_END_LABEL_EDIT,
                               function ( event )
                                    watchListCtrl:SetStringItem(event:GetIndex(),
                                                                          0,
                                                                          event:GetText())
                                    processWatches()
                                    event:Skip()
                               end)

end

function closeWatchWindow()
    if watchWindow then
        watchListCtrl = nil
        watchWindow:Destroy()
        watchWindow = nil
    end
end

frame:ConnectEvent( wx.wxID_EXIT,
                    wx.wxEVT_COMMAND_MENU_SELECTED,
                    function ( event )
                        saveOnExit()
                        frame:Close()
                        closeWatchWindow()
                    end)

editMenu = wx.wxCreateMenu{
                               { wx.wxID_UNDO,      "&Undo\tCtrl-Z",       "Undo the last action" },
                               { wx.wxID_REDO,      "&Redo\tCtrl-Y",       "Redo the last action undone" },
                               { },
                               { wx.wxID_CUT,       "Cu&t\tCtrl-X",        "Deletes selected text and copies it to clipboard" },
                               { wx.wxID_COPY,      "&Copy\tCtrl-C",       "Copies selected text to the clipboard" },
                               { wx.wxID_PASTE,     "&Paste\tCtrl-V",      "Inserts clipboard contents at the current location" },
                               { wx.wxID_CLEAR,     "D&elete\tDel",        "Deletes selected text without copying to the clipboard" },
                               { wx.wxID_SELECTALL, "Select &All\tCtrl-A", "Select all text in the editor" },
                               { },
                               { wxID_AUTOCOMPLETE, "Complete Identifier\tCtrl+K", "Complete the current identifier" },
                               { },
                               { wxID_CLEAROUTPUT,  "Clear output",        "Clear the output window" },
                             }
menuBar:Append(editMenu, "&Edit")

frame:ConnectEvent(wx.wxID_UNDO,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:Undo()
                   end)
frame:ConnectEvent(wx.wxID_UNDO,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       local editor = getEditor()
                       if editor then
                            event:Enable(editor:CanUndo())
                       end
                   end)

frame:ConnectEvent(wx.wxID_REDO,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:Redo()
                   end)
frame:ConnectEvent(wx.wxID_REDO,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        if editor then
                            event:Enable(editor:CanRedo())
                        end
                   end)

frame:ConnectEvent(wx.wxID_CUT,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:Cut()
                   end)
frame:ConnectEvent(wx.wxID_CUT,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wx.wxID_COPY,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:Copy()
                   end)
frame:ConnectEvent(wx.wxID_COPY,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wx.wxID_PASTE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:Paste()
                   end)
frame:ConnectEvent(wx.wxID_PASTE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        if editor then
                            event:Enable(editor:CanPaste())
                        else
                            event:Enable(wx.FALSE)
                        end
                   end)

frame:ConnectEvent(wx.wxID_CLEAR,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:Clear()
                   end)
frame:ConnectEvent(wx.wxID_CLEAR,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wx.wxID_SELECTALL,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        editor:SelectAll()
                   end)
frame:ConnectEvent(wx.wxID_SELECTALL,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wxID_AUTOCOMPLETE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        local pos = editor:GetSelection()
                        local lineStart = editor:LineFromPosition(pos)
                        if editor:PositionFromLine(lineStart) <= (pos - 6) then
                            local range = editor:GetTextRange(pos - 6, pos - 1)
                            if ((range == "wx.WX") or (range == "wx.wx")) then
                                local poschar = editor:GetTextRange(pos - 1, pos)
                                local userList = createUserList(string.byte(poschar), string.sub(range, -1))
                                if userList then
                                    editor:UserListShow(1, userList)
                                end
                            end
                        end
                   end)
frame:ConnectEvent(wxID_AUTOCOMPLETE,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wxID_CLEAROUTPUT,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        if errorLog ~= nil then
                            errorLog:SetReadOnly(wx.FALSE)
                            errorLog:ClearAll()
                            errorLog:SetReadOnly(wx.TRUE)
                        end
                   end)

frame:ConnectEvent(wxID_CLEAROUTPUT,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                       enableMenuItem(event)
                   end)

function makeDebugFileName(editor, filePath)
    if not filePath then
        filePath = "file"..tostring(editor)
    end
    return filePath
end

function toggleDebugMarker(editor, row)
    local markers = editor:MarkerGet(row)
    if markers >= wxCURRENT_LINE_MARKER_VALUE then
        markers = markers - wxCURRENT_LINE_MARKER_VALUE
    end
    local id       = editor:GetId()
    local filePath = makeDebugFileName(editor, openDocuments[id].filePath)
    if markers >= wxBREAKPOINT_MARKER_VALUE then
        editor:MarkerDelete(row, wxBREAKPOINT_MARKER)
        if debugServer then
            debugServer:RemoveBreakPoint(filePath, row)
        end
    else
        editor:MarkerAdd(row, wxBREAKPOINT_MARKER)
        if debugServer then
            debugServer:AddBreakPoint(filePath, row)
        end
    end
end

findMenu = wx.wxCreateMenu{
                            { wx.wxID_FIND,    "&Find\tCtrl-F",            "Find the specified text" },
                            { wxID_FINDNEXT,   "Find &Next\tF3",           "Find the next occurrence of the specified text" },
                            { wxID_FINDPREV,   "Find &Previous\tShift-F3", "Repeat the search backwards in the file" },
                            { wxID_REPLACE,    "&Replace\tCtrl-H",         "Replaces the specified text with different text" },
                            { },
                            { wxID_GOTOLINE,   "&Goto line\tCtrl-G",       "Go to a selected line" }
                          }
menuBar:Append(findMenu, "&Search")

function ensureRangeVisible(posStart, posEnd)
    local editor = getEditor()
    if posStart > posEnd then
        posStart, posEnd = posEnd, posStart
    end

    local lineStart = editor:LineFromPosition(posStart)
    local lineEnd   = editor:LineFromPosition(posEnd)
    for line = lineStart, lineEnd do
        editor:EnsureVisibleEnforcePolicy(line)
    end
end

function setSearchFlags(editor, fWholeWord, fMatchCase)
    local flags = 0
    if fWholeWord then
        flags = wx.wxSTC_FIND_WHOLEWORD
    end
    if fMatchCase then
        flags = flags + wx.wxSTC_FIND_MATCHCASE
    end
    editor:SetSearchFlags(flags)
end

function findstring(strFind, fDown, fWholeWord, fMatchCase)
    local editor = getEditor()
    local lenFind = string.len(strFind)
    if lenFind == 0 then
        return
    end
    local startSel      = editor:GetSelectionStart()
    local endSel        = editor:GetSelectionEnd()
    local startPosition = endSel
    local endPosition   = editor:GetLength()
    if not fDown then
        startPosition = startSel - 1
        endPosition   = 0
    end
    editor:SetTargetStart(startPosition)
    editor:SetTargetEnd(endPosition)
    setSearchFlags(editor, fWholeWord, fMatchCase)
    local posFind = editor:SearchInTarget(strFind)
    if posFind == -1 then
        if not fDown then
            startPosition = editor:GetLength()
            endPosition   = 0
        else
            startPosition = 0
            endPosition   = editor:GetLength()
        end
        editor:SetTargetStart(startPosition)
        editor:SetTargetEnd(endPosition)
        posFind = editor:SearchInTarget(strFind)
    end
    if posFind == -1 then
        fHaveFound = nil

       frame:SetStatusText("Not found.")
    else
        fHaveFound = 1
        local start  = editor:GetTargetStart()
        local finish = editor:GetTargetEnd()
        ensureRangeVisible(start, finish)
        editor:SetSelection(start, finish)
    end
end

function replace(strFind, strReplace, fDown, fWholeWord, fMatchCase, fReplaceAll)
    local editor = getEditor()
    local findLen = string.len(strFind)
    if findLen == 0 then
        return
    end

    if fReplaceAll then
        local startPosition = editor:GetSelectionStart()
        local endPosition   = editor:GetLength()
        local replaceLen    = string.len(strReplace)

        editor:SetTargetStart(startPosition)
        editor:SetTargetEnd(endPosition)
        setSearchFlags(editor, fWholeWord, fMatchCase)
        local posFind = editor:SearchInTarget(strFind)
        if (posFind ~= -1) and (posFind <= endPosition) then
            local lastMatch = posFind
            editor:BeginUndoAction()
            while posFind ~= -1 do
                local lenTarget = editor:GetTargetEnd() - editor:GetTargetStart()
                editor:ReplaceTarget(strReplace)

                endPosition = endPosition + replaceLen - lenTarget
                lastMatch = posFind + replaceLen

                if lenTarget <= 0 then
                    lastMatch = lastMatch + 1
                end

                editor:SetTargetStart(lastMatch)
                editor:SetTargetEnd(endPosition)
                posFind = editor:SearchInTarget(strFind)
            end
            editor:SetSelection(lastMatch, lastMatch)
            editor:EndUndoAction()
        end
    else
        if fHaveFound then
            local replaceLen = string.len(strReplace)
            local start  = editor:GetSelectionStart()
            local finish = editor:GetSelectionEnd()
            editor:SetTargetStart(start)
            editor:SetTargetEnd(finish)
            editor:ReplaceTarget(strReplace)
            editor:SetSelection(start + replaceLen, finish)
            fHaveFound = nil
        end

        findstring(strFind, fDown, fWholeWord, fMatchCase)
    end
end

function findText(where, strFind, findFlags)
    -- where == 2                  - Find previous
    -- where == 1                  - Find next
    -- where == 0 or where == nil  - Find

    local fDown, fWholeWord, fMatchCase

    if findFlags >= wx.wxFR_MATCHCASE then
        findFlags = findFlags - wx.wxFR_MATCHCASE
        fMatchCase = 1
    end
    if findFlags >= wx.wxFR_WHOLEWORD then
        findFlags = findFlags - wx.wxFR_WHOLEWORD
        fWholeWord = 1
    end
    if findFlags >= wx.wxFR_DOWN and where ~= 2 then
        fDown = 1
    end

    findstring(strFind, fDown, fWholeWord, fMatchCase)
end

function replaceText(where, strFind, strReplace, findFlags)
    -- where == 1                  - Replace all
    -- where == 0                  - Replace

    local fDown, fWholeWord, fMatchCase

    if findFlags >= wx.wxFR_MATCHCASE then
        findFlags = findFlags - wx.wxFR_MATCHCASE
        fMatchCase = 1
    end
    if findFlags >= wx.wxFR_WHOLEWORD then
        findFlags = findFlags - wx.wxFR_WHOLEWORD
        fWholeWord = 1
    end
    if findFlags >= wx.wxFR_DOWN and where ~= 2 then
        fDown = 1
    end

    replace(strFind, strReplace, fDown, fWholeWord, fMatchCase, where == 1)
end

function findReplace(replace)
    if not findData then
        findData = wx.wxFindReplaceData(wx.wxFR_DOWN)
    end

    local flag
    local title

    if replace then
        flag  = wx.wxFR_REPLACEDIALOG
        title = "Replace"
    else
        flag  = 0
        title = "Find"
    end

    local finddlg = wx.wxFindReplaceDialog(frame,
                                           findData,
                                           title,
                                           flag)

    finddlg:ConnectEvent(wx.wxEVT_COMMAND_FIND,
                         function ( event )
                           findText(0,
                                    event:GetFindString(),
                                    event:GetFlags())
                         end)

    finddlg:ConnectEvent(wx.wxEVT_COMMAND_FIND_NEXT,
                         function ( event )
                           findText(1,
                                    event:GetFindString(),
                                    event:GetFlags())
                         end)

    finddlg:ConnectEvent(wx.wxEVT_COMMAND_FIND_REPLACE,
                         function ( event )
                            replaceText(0,
                                        event:GetFindString(),
                                        event:GetReplaceString(),
                                        event:GetFlags())
                         end)

    finddlg:ConnectEvent(wx.wxEVT_COMMAND_FIND_REPLACE_ALL,
                         function ( event )
                            replaceText(1,
                                        event:GetFindString(),
                                        event:GetReplaceString(),
                                        event:GetFlags())
                         end)

    finddlg:ConnectEvent(wx.wxEVT_COMMAND_FIND_CLOSE,
                         function ( event )
                           finddlg:Destroy()
                         end)

    finddlg:ShowModal()
end

frame:ConnectEvent(wx.wxID_FIND,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        findReplace()
                   end)
frame:ConnectEvent(wx.wxID_FIND,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wxID_FINDNEXT,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        if findData then
                            findText(1,
                                     findData:GetFindString(),
                                     findData:GetFlags())
                        end
                   end)
frame:ConnectEvent(wxID_FINDNEXT,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        event:Enable(findData ~= nil)
                   end)

frame:ConnectEvent(wxID_FINDPREV,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        if findData then
                           findText(2,
                                    findData:GetFindString(),
                                    findData:GetFlags())
                        end
                   end)
frame:ConnectEvent(wxID_FINDPREV,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        event:Enable(findData ~= nil)
                   end)

frame:ConnectEvent(wxID_REPLACE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        findReplace(1)
                   end)
frame:ConnectEvent(wxID_REPLACE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       enableMenuItem(event)
                   end)

frame:ConnectEvent(wxID_GOTOLINE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        local linemax = editor:LineFromPosition(editor:GetLength()) + 1
                        local linenum = wx.wxGetNumberFromUser( "Enter line number",
                                                                "1 .. "..tostring(linemax),
                                                                "Goto Line",
                                                                1,
                                                                1,
                                                                linemax,
                                                                frame)
                        if linenum > 0 then
                            editor:GotoLine(linenum-1)
                        end
                   end)
frame:ConnectEvent(wxID_GOTOLINE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                       enableMenuItem(event)
                   end)

function layoutChildren(event)
    if toolBar then
        local x, y = frame:GetClientSize()

        if wx.wxPlatformWindows then
            toolBar:SetSize(x, -1)
        else
            toolBar:SetSize(x, 34)
        end
        toolBar:Move(0, 0)

        local xoffset, yoffset = toolBar:GetSize()

        splitter:SetDimensions(0, yoffset, x, y - yoffset)
    else
        event:Skip()
    end
end
frame:ConnectEvent(wx.wxEVT_SIZE, layoutChildren)

function closeWindow(event)
    fIsConnected = nil
    if debugServer then
        debugServer:Reset()
        debugServer:Delete()
        debugServer = nil
    end
    fIsRunning = nil
    saveOnExit()
    saveFramePosition(frame, "MainFrame")
    config:Set():Destroy()
    event:Skip()
    closeWatchWindow()
end
frame:ConnectEvent(wx.wxEVT_CLOSE_WINDOW, closeWindow)

function downloadBreakpoints(editor, fileName)
    local nextLine = editor:MarkerNext(0, wxBREAKPOINT_MARKER_VALUE)
    while nextLine ~= -1 do
        debugServer:AddBreakPoint(fileName, nextLine)
        nextLine = editor:MarkerNext(nextLine + 1, wxBREAKPOINT_MARKER_VALUE)
    end
end

debugMenu = wx.wxCreateMenu{
                             { wxID_TOGGLEBREAKPOINT, "Toggle Breakpoint\tF9", "Toggle Breakpoint" },
                             { },
                             { wxID_COMPILE,          "Compile\tF7",           "Test compile the wxLua program" },
                             { wxID_RUN,              "Run\tF6",               "Execute the current file" },
                             { wxID_ATTACH_DEBUG,     "Attach\tShift-F6",      "Allow a client to start a debugging session" },
                             { wxID_START_DEBUG,      "Start Debug\tShift-F5", "Start a debugging session" },
                             { },
                             { wxID_RESET,            "Reset",                 "Reset the debugging session" },
                             { wxID_STEP,             "Step\tF11",             "Step into the next line" },
                             { wxID_STEP_OVER,        "Step Over\tShift-F11",  "Step over the next line" },
                             { wxID_STEP_OUT,         "Step Out\tF8",          "Step out of the current function" },
                             { wxID_CONTINUE,         "Continue\tF5",          "Run the program at full speed" },
                             { wxID_BREAK,            "Break\tF12",            "Stop execution of the program at the next executed line of code" },
                             { },
                             { wxID_VIEWCALLSTACK,    "View Call Stack",       "View the LUA call stack" },
                             { wxID_VIEWWATCHWINDOW,  "View Watches",          "View the Watch window" }
                            }
menuBar:Append(debugMenu, "&Debug")

frame:ConnectEvent(wxID_TOGGLEBREAKPOINT,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor = getEditor()
                        local pos = editor:GetCurrentPos()
                        local row = editor:LineFromPosition(pos)
                        toggleDebugMarker(editor, row)
                   end)
frame:ConnectEvent(wxID_TOGGLEBREAKPOINT,
                   wx.wxEVT_UPDATE_UI,
                   function(event)
                        enableMenuItem(event)
                   end)

frame:ConnectEvent(wxID_ATTACH_DEBUG,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                       debugServer = wx.wxLuaDebugServer(portNumber)
                       if debugServer then
                           debugServer:StartServerThread()
                       end
                       displayOutput("Waiting for client connect. Start client with wxLua -d"..wx.wxGetHostName()..":"..portNumber.."\n")
                       portNumber = portNumber + 1
                   end)
frame:ConnectEvent(wxID_ATTACH_DEBUG,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        event:Enable(debugServer == nil and editor ~= nil)
                   end)

frame:ConnectEvent(wxID_RUN,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local fileList = {}
                        saveAll()
                        for id, document in openDocuments do
                            local filePath = document.filePath
							if filePath == nil then
								return
							end
							table.insert(fileList, ' "'..filePath..'"')
                        end
                        wx.wxExecute('"'..programName..'" '..table.concat(fileList))
                   end)
frame:ConnectEvent(wxID_RUN,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        event:Enable(debugServer == nil and editor ~= nil)
                   end)

frame:ConnectEvent(wxID_START_DEBUG,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                       debugServer = wx.wxLuaDebugServer(portNumber)
                       if debugServer then
                           debugServer:StartClient()
                           setAllEditorsReadOnly(1)
                       end
                       displayOutput("Waiting for client connection.\n")
                       portNumber = portNumber + 1
                   end)
frame:ConnectEvent(wxID_START_DEBUG,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        event:Enable(debugServer == nil and editor ~= nil)
                   end)

frame:ConnectEvent(wxID_COMPILE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        local editor     = getEditor()
                        local editorText = editor:GetText()
                        local id         = editor:GetId()
                        local filePath   = makeDebugFileName(editor, openDocuments[id].filePath)
                        wx.wxLuaDebugServerCompile(filePath, editorText)
                   end)
frame:ConnectEvent(wxID_COMPILE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        enableMenuItem(event)
                   end)

function setAllEditorsReadOnly(flag)
    for id, document in openDocuments do
        local editor = document.editor
        if flag then
            editor:SetReadOnly(wx.TRUE)
        else
            editor:SetReadOnly(wx.FALSE)
        end
    end
end

frame:ConnectEvent(wxID_RESET,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                       for id, document in openDocuments do
                           local editor = document.editor
                           editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                       end
                       fIsConnected = nil
                       if debugServer then
                           debugServer:Reset()
                           debugServer:Delete()
                           debugServer = nil
                       end
                       setAllEditorsReadOnly(nil)
                       ignoredFilesList = {}
                       fIsRunning = nil
                       displayOutput("\nClient reset\n")
                   end)
frame:ConnectEvent(wxID_RESET,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        event:Enable(debugServer ~= nil and editor ~= nil)
                   end)

frame:ConnectEvent(wxID_STEP,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        for id, document in openDocuments do
                            local editor = document.editor
                            editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                        end
                        if debugServer then
                            debugServer:Step()
                            fIsRunning = 1
                        end
                   end)
frame:ConnectEvent(wxID_STEP,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        event:Enable(debugServer ~= nil and not fIsRunning and editor ~= nil)
                   end)

frame:ConnectEvent(wxID_STEP_OVER,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        for id, document in openDocuments do
                           local editor = document.editor
                           editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                        end
                        if debugServer then
                            debugServer:StepOver()
                            fIsRunning = 1
                        end
                   end)
frame:ConnectEvent(wxID_STEP_OVER,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        local editor = getEditor()
                        event:Enable(debugServer ~= nil and not fIsRunning and editor ~= nil)
                   end)

frame:ConnectEvent(wxID_STEP_OUT,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        for id, document in openDocuments do
                           local editor = document.editor
                           editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                        end
                        if debugServer then
                            debugServer:StepOut()
                            fIsRunning = 1
                        end
                   end)
frame:ConnectEvent(wxID_STEP_OUT,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        event:Enable(debugServer ~= nil and not fIsRunning)
                   end)

frame:ConnectEvent(wxID_CONTINUE,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        for id, document in openDocuments do
                            local editor = document.editor
                            editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                        end
                        if debugServer then
                            debugServer:Continue()
                            fIsRunning = 1
                        end
                   end)
frame:ConnectEvent(wxID_CONTINUE,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        event:Enable(debugServer ~= nil and not fIsRunning)
                   end)

frame:ConnectEvent(wxID_BREAK,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        if debugServer then
                            debugServer:Break()
                        end
                   end)
frame:ConnectEvent(wxID_BREAK,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        event:Enable(debugServer ~= nil and fIsRunning)
                   end)

frame:ConnectEvent(wxID_VIEWCALLSTACK,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        if debugServer then
                            debugServer:DisplayStackDialog(frame)
                        end
                   end)
frame:ConnectEvent(wxID_VIEWCALLSTACK,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                        event:Enable(debugServer ~= nil and not fIsRunning)
                   end)

frame:ConnectEvent(wxID_VIEWWATCHWINDOW,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   function ( event )
                        if not watchWindow then
                            createWatchWindow()
                        end
                   end)
frame:ConnectEvent(wxID_VIEWWATCHWINDOW,
                   wx.wxEVT_UPDATE_UI,
                   function ( event )
                         event:Enable(debugServer ~= nil and not fIsRunning)
                   end)

editorApp:ConnectEvent(-1,
                       wx.wxEVT_DEBUG_CLIENT_CONNECTED,
                       function (event)
                            if debugServer then
                                for id, document in openDocuments do
                                    local editor     = document.editor
                                    local editorText = editor:GetText()
                                    local filePath   = makeDebugFileName(editor, document.filePath)
                                    debugServer:Run(filePath, editorText)
                                    downloadBreakpoints(editor, filePath)
                                end
                                fIsConnected = 1
                                debugServer:Step()
                                fIsRunning = 1
                            end
                            displayOutput("Client connected\n")
                       end)

function debuggerIgnoreFile(fileName)
    ignoreFlag = nil
    for idx, ignoreFile in ignoredFilesList do
        if string.upper(ignoreFile) == string.upper(fileName) then
            ignoreFlag = 1
        end
    end
    return ignoreFlag
end

editorApp:ConnectEvent(-1,
                       wx.wxEVT_DEBUG_BREAK,
                       function (event)
                           local row = event:GetLineNumber()
                           local eventFileName = event:GetFileName()
                           if string.sub(eventFileName, 1, 1) == '@' then
                               eventFileName = string.sub(eventFileName, 2, -1)
                               if wx.wxIsAbsolutePath(eventFileName) == wx.FALSE then
                                   eventFileName = wx.wxGetCwd().."/"..eventFileName
                               end
                           end
                           if wx.wxPlatformWindows then
                               eventFileName = wx.wxUnix2DosFilename(eventFileName)
                           end
                           local fileFound = false
                           displayOutput("At Breakpoint line: "..tostring(row).." file: "..eventFileName.."\n")
                           for id, document in openDocuments do
                               local editor   = document.editor
                               local filePath = makeDebugFileName(editor, document.filePath)
                               if string.upper(filePath) == string.upper(eventFileName) then
                                   local selection = document.index
                                   notebook:SetSelection(selection)
                                   setEditorSelection(selection)
                                   editor:MarkerAdd(row, wxCURRENT_LINE_MARKER)
                                   editor:EnsureVisibleEnforcePolicy(row)
                                   fileFound = true
                                   break
                               end
                           end
                           if not debuggerIgnoreFile(eventFileName) then
                               if not fileFound then
                                    local fileDialog = wx.wxFileDialog(frame,
                                                                       "Select file for debugging",
                                                                       "",
                                                                       eventFileName,
                                                                       "Lua files(*.lua)|*.lua|Text files(*.txt)|*.txt|All files(*.*)|*.*",
                                                                       wx.wxOPEN)
                                    if fileDialog:ShowModal() == wx.wxID_OK then
                                        local editor = loadFile(fileDialog:GetPath(), fileDialog:GetFilename())
                                        if editor then
                                           editor:MarkerAdd(row, wxCURRENT_LINE_MARKER)
                                           editor:EnsureVisibleEnforcePolicy(row)
                                           editor:SetReadOnly(wx.TRUE)
                                           fileFound = true
                                        end
                                    end
                                    fileDialog:Destroy()
                               end
                               if not fileFound then
                                    table.insert(ignoredFilesList, eventFileName)
                               end
                           end

                           if fileFound then
                               fIsRunning = nil
                               processWatches()
                           elseif debugServer then
                               debugServer:Continue()
                               fIsRunning = 1
                           end
                       end)

function displayOutput(message)
    if errorLog == nil then
        errorLog = CreateOutput()
    end
    if splitter:IsSplit() == wx.FALSE then
        local x, y = frame:GetClientSize()
        splitter:SplitHorizontally(notebook, errorLog, (2 * y) / 3)
    end
    errorLog:SetReadOnly(wx.FALSE)
    errorLog:AddText(message)
    errorLog:SetReadOnly(wx.TRUE)
end

editorApp:ConnectEvent(-1,
                       wx.wxEVT_DEBUG_PRINT,
                       function (event)
                           displayOutput(event:GetMessage())
                       end)

editorApp:ConnectEvent(-1,
                       wx.wxEVT_DEBUG_ERROR,
                       function (event)
                           displayOutput(event:GetMessage())
                       end)

editorApp:ConnectEvent(-1,
                       wx.wxEVT_DEBUG_EXIT,
                       function (event)
                           for id, document in openDocuments do
                               local editor = document.editor
                               editor:MarkerDeleteAll(wxCURRENT_LINE_MARKER)
                           end
                           fIsConnected = nil
                           if debugServer then
                               debugServer:Delete()
                               fIsRunning = nil
                               debugServer = nil
                           end
                           setAllEditorsReadOnly(nil)
                           ignoredFilesList = {}
                           displayOutput("\nClient finished\n")
                       end)

editorApp:ConnectEvent(-1,
                       wx.wxEVT_DEBUG_EVALUATE_EXPR,
                       function (event)
                           if watchListCtrl then
                               watchListCtrl:SetStringItem(event:GetReference(),
                                                           1,
                                                           event:GetMessage())
                           end
                       end)

function displayAbout(event)
    local page = [[
        <html>
        <body bgcolor = "#FFFFFF">
        <table cellspacing=4 cellpadding=4 width="100%">
          <tr>
            <td bgcolor = "#202020">
            <center>
                <font size = +2 color = "#FFFFFF">
                    <b><br>wxLua Version 2.6<br></b>
                </font>
            </center>
            </td>
          </tr>
          <tr>
            <td bgcolor = "#DCDCDC">
            <b>Copyright (C) 2002-2004 Lomtick Software</b>
            <p>
            <font size=-1>
              <table cellpadding = 0 cellspacing = 0 width = "100%">
                <tr>
                  <td width = "65%">
                    J. Winwood (luascript@thersgb.net)<p>
                  </td>
                  <td valign = top>
                    <img src = "memory:wxLua">
                  </td>
                </tr>
              </table>
            <font size = 1>
                Licenced under wxWindows Library Licence, Version 3.
            </font>
            </font>
            </td>
          </tr>
        </table>
        </body>
        </html>
    ]]

    local dlg = wx.wxDialog(frame, -1, "About")
    local topsizer = wx.wxBoxSizer(wx.wxVERTICAL)

    local html = wx.wxLuaHtmlWindow(dlg,
                                    -1,
                                    wx.wxDefaultPosition,
                                    wx.wxSize(360, 150),
                                    wx.wxHW_SCROLLBAR_NEVER)
    local line = wx.wxStaticLine(dlg, -1)
    local button = wx.wxButton(dlg, wx.wxID_OK, "OK")

    button:SetDefault()

    html:SetBorders(0)
    html:SetPage(page)
    html:SetSize(html:GetInternalRepresentation():GetWidth(),
                 html:GetInternalRepresentation():GetHeight())

    topsizer:AddWindow(html, 1, wx.wxALL, 10)
    topsizer:AddWindow(line, 0, wx.wxEXPAND + wx.wxLEFT + wx.wxRIGHT, 10)

    topsizer:AddWindow(button, 0, wx.wxALL + wx.wxALIGN_RIGHT, 15)

    dlg:SetAutoLayout(wx.TRUE)
    dlg:SetSizer(topsizer)
    topsizer:Fit(dlg)

    dlg:ShowModal()
    dlg:Destroy()
end
frame:ConnectEvent(wx.wxID_HELP,
                   wx.wxEVT_COMMAND_MENU_SELECTED,
                   displayAbout)

frame:SetStatusWidths{-1, 29, 29, 150}
frame:SetStatusText("Ready")
frame:SetMenuBar(menuBar)

restoreFramePosition(frame, "MainFrame")

frame:Show(wx.TRUE)

if args then
    local fileLoaded
    programName = args[0]
    for index, fileName in args do
        if index ~= "n" and index ~= 0 then
            loadFile(fileName, fileName)
            fileLoaded = 1
        end
        if notebook:GetPageCount() > 0 then
            notebook:SetSelection(0)
        end
    end
    if not fileLoaded then
       local editor = createEditor("Untitled")
       setupKeywords(editor, 1)
    end
end

frame:SetIcon(wxLuaEditorIcon)

wx.Run()
