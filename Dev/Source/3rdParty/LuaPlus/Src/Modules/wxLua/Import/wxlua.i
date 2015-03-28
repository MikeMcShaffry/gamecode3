/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxLua specific wrappers
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// wxLuaInterpreter

%class %delete wxLuaInterpreter, wxObject
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxLuaObject
// Allows Lua data items to be used for client data.
%typedef luaObject lua_State*

%class %delete wxLuaObject, wxObject
    wxLuaObject(luaObject object)
    void SetObject(luaObject object)
    luaObject GetObject() const
    %property=Object, read, write
    int GetAllocationFlags() const
    %property=AllocationFlags, read
    void ModifyAllocationFlags(int iValue)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxLuaDebugServer

%if wxLUA_USE_wxLuaDebugServer

%include "../Library/wxLuaDebugServer.h"

%class %noclassinfo %delete wxLuaDebugServer
    wxLuaDebugServer(int portNumber)
    %constructor wxLuaDebugServerCompile(const wxString &buffer, const wxString &fileName)
    bool StartClient()
    bool StartServerThread()
    bool AddBreakPoint(const wxString &fileName, int lineNumber)
    bool RemoveBreakPoint(const wxString &fileName, int lineNumber)
    bool ClearAllBreakPoints()
    bool Run(const wxString &file, const wxString &fileName)
    bool Step()
    bool StepOver()
    bool StepOut()
    bool Continue()
    bool Break()
    bool Reset()
    void DisplayStackDialog(wxWindow *pParent)
    bool EvaluateExpr(int exprRef, const wxString &expr)
%endclass

%class %noclassinfo wxLuaDebugData
%endclass

%class wxLuaDebugEvent, wxEvent
    %define %event wxEVT_DEBUG_CLIENT_CONNECTED
    %define %event wxEVT_DEBUG_BREAK
    %define %event wxEVT_DEBUG_PRINT
    %define %event wxEVT_DEBUG_ERROR
    %define %event wxEVT_DEBUG_EXIT
    %define %event wxEVT_DEBUG_STACK_ENUM
    %define %event wxEVT_DEBUG_STACK_ENTRY_ENUM
    %define %event wxEVT_DEBUG_TABLE_ENUM
    %define %event wxEVT_DEBUG_EVALUATE_EXPR

    int GetLineNumber() const
    int GetReference() const
    wxString GetFileName() const
    wxString GetMessage() const
    const wxLuaDebugData *GetDebugData() const
%endclass

%endif wxLUA_USE_wxLuaDebugServer

///////////////////////////////////////////////////////////////////////////////
// wxLuaPrintout

%if wxLUA_USE_wxLuaPrintout

%include "../Library/wxLuaPrinting.h"

%class %delete wxLuaPrintout, wxPrintout
    wxLuaPrintout(const wxString& title = "Printout", wxLuaObject *pObject = NULL)
    wxLuaObject *GetID()
    void SetPageInfo(int minPage, int maxPage, int pageFrom = 0, int pageTo = 0)
%endclass

%endif wxLUA_USE_wxLuaPrintout

///////////////////////////////////////////////////////////////////////////////
// wxLuaHtmlWindow

%if wxLUA_USE_wxLuaHtmlWindow

%include "../Library/wxLuaHtmlWindow.h"

%class wxLuaHtmlWindow, wxHtmlWindow
    wxLuaHtmlWindow(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHW_SCROLLBAR_AUTO, const wxString& name = "htmlWindow")
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxLuaHtmlWinTagEvent

%class wxLuaHtmlWinTagEvent, wxEvent
    %define %event wxEVT_HTML_TAG_HANDLER

    const wxHtmlTag      *GetHtmlTag() const
    %property=HtmlTag, read
    wxHtmlWinParser      *GetHtmlParser() const
    %property=HtmlParser, read
    void                  SetParseInnerCalled(bool fParseInnerCalled = true)
    bool                  GetParseInnerCalled() const
    %property=ParseInnerCalled, read, write
%endclass

%endif wxLUA_USE_wxLuaHtmlWindow

%if wxLUA_USE_wxLuaApp

%include "../Library/wxLuaLibrary.h"

%endif wxLUA_USE_wxLuaApp
