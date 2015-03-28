/////////////////////////////////////////////////////////////////////////////
// Purpose:     Implements the debugger end of wxLua debugging session
// Author:      J. Winwood
// Created:     June 2003
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
// Ray Gilbert moved class definitions out of wxLuaDebugServer, wxLuaDebugIO
// to allow alternate debugger model
//
// Original Debugging Model:
// wxLuaDebugServer, LuaDebugTarget
// The current process acts as a server - debugger (wxLuaDebugServer)
// starts a new process which starts up as a client - debuggee (LuaDebugTarget)
// that connects back to server process.
//
// Alternative Debugging Model:
// wxLuaDebugService, wxLuaDebuggee, wxLuaDebugger
// The current process acts as a server - debuggee (wxLuaDebugService; wxLuaDebuggee).
// This service waits for client - debugger to connect.

#ifndef WX_LUA_DEBUG_H
#define WX_LUA_DEBUG_H

#ifdef __GNUG__
    #pragma interface "wxLuaDebug.h"
#endif

#include "wx/dynarray.h"
#include "wx/treectrl.h"

class wxLuaDebugData;

extern "C"
{
    typedef struct lua_State lua_State;
    typedef struct lua_Debug lua_Debug;
}

// ----------------------------------------------------------------------------
// wxLuaBuffer
// ----------------------------------------------------------------------------

class wxLuaBuffer : public wxObject
{
public:
    wxLuaBuffer(const wxString &fileName, const wxString &buffer);

    wxString GetBuffer() const { return m_buffer; }
    wxString GetFileName() const { return m_fileName; }
    int      GetBufferLength() const { return m_buffer.Length(); }

private:
    wxString m_fileName;
    wxString m_buffer;
    DECLARE_CLASS(wxLuaBuffer)
};


#if wxCHECK_VERSION(2, 5, 0)
WX_DEFINE_ARRAY_PTR(wxLuaBuffer *, wxLuaBufferArray);
#else
WX_DEFINE_ARRAY(wxLuaBuffer *, wxLuaBufferArray);
#endif

// ----------------------------------------------------------------------------
// wxLuaDebugDataItem
// ----------------------------------------------------------------------------
class wxLuaDebugDataItem : public wxTreeItemData
{
public:
    wxLuaDebugDataItem(const wxLuaDebugDataItem &debugDataItem);
    wxLuaDebugDataItem(const wxString &itemName,
                       const wxString &itemType,
                       const wxString &itemValue,
                       const wxString &itemSource,
                       int             nReference,
                       int             nIndex    = 0,
                       bool            fExpanded = false);

    wxString GetName() const      { return m_itemName; }
    wxString GetType() const      { return m_itemType; }
    wxString GetValue() const     { return m_itemValue; }
    wxString GetSource() const    { return m_itemSource; }
    int      GetReference() const { return m_nReference; }
    int      GetIndex() const     { return m_nIndex; }
    bool     IsExpanded() const   { return m_fExpanded; }

    void     SetExpanded(bool fExpanded) { m_fExpanded = fExpanded; }

private:
    wxString m_itemName;
    wxString m_itemType;
    wxString m_itemValue;
    wxString m_itemSource;
    int      m_nReference;
    int      m_nIndex;
    bool     m_fExpanded;
};

WX_DEFINE_SORTED_ARRAY(wxLuaDebugDataItem *, wxLuaDebugDataItemArray);

// ----------------------------------------------------------------------------
// wxLuaDebugData
// ----------------------------------------------------------------------------
class wxLuaDebugData : public wxLuaDebugDataItemArray
{
public:
    wxLuaDebugData();
    ~wxLuaDebugData();

    wxLuaDebugData *Clone();
    static int SortFunction(wxLuaDebugDataItem *elem1, wxLuaDebugDataItem *elem2 );
};

// ----------------------------------------------------------------------------
// wxLuaInterface
// ----------------------------------------------------------------------------
class wxLuaInterface
{
public:
    wxLuaInterface(lua_State *luaState);
    virtual ~wxLuaInterface();

    wxLuaDebugData *BuildStack();
    wxLuaDebugData *BuildStackList(int nEntry);
    wxLuaDebugData *BuildTableList(int nRef, int nEntry);
    wxLuaDebugData *GetGlobalData();

    lua_State *GetLuaState() const { return m_luaState; }

private:
    int         GetStack(int nIndex, lua_Debug *luaDebug);
    bool        GetInfo(const wxString &opts, lua_Debug *luaDebug);
    wxString    GetLocal(lua_Debug *luaDebug, int index);
    void        GetGlobals();
    int         GetTop();
    int         Type(int index);
    double      ToNumber(int index);
    int         ToBoolean(int index);
    wxString    ToString(int index);
    const void *ToPointer(int index);
    bool        IsCFunction(int index);
    const void *ToCFunction(int index);
    const void *ToUserData(int index);
    int         GetTag(int index);
    int         GetN(int index);
    int         Ref();
    int         GetRef(int ref);
    void        Pop(int count);
    void        PushNil();
    void        PushValue(int index);
    int         Next(int index);
    bool        IsLuaTable(int index) const;

    wxString GetTableInfo(int index);
    wxString GetUserDataInfo(int index, bool full = true);

    static wxString MakeNumber(double dnum);
    static wxString MakeBoolean(int num);

    lua_State *m_luaState;
    wxArrayInt m_references;
};


enum debugEvents
{
    EVENT_DEBUG_BREAK,
    EVENT_DEBUG_PRINT,
    EVENT_DEBUG_ERROR,
    EVENT_DEBUG_EXIT,
    EVENT_DEBUG_STACK_ENUM,
    EVENT_DEBUG_STACK_ENTRY_ENUM,
    EVENT_DEBUG_TABLE_ENUM,
    EVENT_DEBUG_EVALUATE_EXPR,
    EVENT_DEBUG_BREAKPOINT_ADDED,
    EVENT_DEBUG_BREAKPOINT_REMOVED
};

enum debugCommands
{
    CMD_ADD_BREAKPOINT,
    CMD_REMOVE_BREAKPOINT,
    CMD_CLEAR_ALL_BREAKPOINTS,
    CMD_RUN_BUFFER,
    CMD_DEBUG_STEP,
    CMD_DEBUG_STEPOVER,
    CMD_DEBUG_STEPOUT,
    CMD_DEBUG_CONTINUE,
    CMD_DEBUG_BREAK,
    CMD_ENUMERATE_STACK,
    CMD_ENUMERATE_STACK_ENTRY,
    CMD_ENUMERATE_TABLE_REF,
    CMD_EVALUATE_EXPR,
    CMD_RESET,
    CMD_CLEAR_DEBUG_REFERENCES,
    CMD_DISABLE_BREAKPOINT,
    CMD_ENABLE_BREAKPOINT
};

////////////////////////////////////////////////////////////////
// wxLuaDebuggerBase
//
// Debugger Interface
class wxLuaDebuggerBase : public wxEvtHandler
{
public:
    wxLuaDebuggerBase() {}
    ~wxLuaDebuggerBase() {}

    virtual bool AddBreakPoint(const wxString &fileName, int lineNumber) = 0;
    virtual bool RemoveBreakPoint(const wxString &fileName, int lineNumber) = 0;
    virtual bool ClearAllBreakPoints() = 0;
    virtual bool Run(const wxString &fileName, const wxString &buffer) = 0;
    virtual bool Step() = 0;
    virtual bool StepOver() = 0;
    virtual bool StepOut() = 0;
    virtual bool Continue() = 0;
    virtual bool Break() = 0;
    virtual bool Reset() = 0;
    virtual bool EnumerateStack() = 0;
    virtual bool EnumerateStackEntry(int stackEntry) = 0;
    virtual bool EnumerateTable(int tableRef, int nIndex, int nItemNode) = 0;
    virtual bool CleanupDebugReferences() = 0;

private:
    DECLARE_CLASS(wxLuaDebuggerBase)
};

// ----------------------------------------------------------------------------
// wxLuaDebugEvent
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaDebugEvent : public wxEvent
{
public:
    wxLuaDebugEvent();
    wxLuaDebugEvent(int         eventType);
    wxLuaDebugEvent(int         eventType,
                    int         lineNumber,
                    const wxString &fileName,
                    bool        enabledFlag = false);
    virtual ~wxLuaDebugEvent();

    void               SetMessage(const wxString &message);
    void               SetDebugData(int             nReference,
                                    wxLuaDebugData *pDebugData = NULL);

    int                   GetLineNumber() const;
    wxString              GetFileName() const;
    wxString              GetMessage() const;
    int                   GetReference() const;
    const wxLuaDebugData *GetDebugData() const;
    bool                  GetEnabledFlag() const;
protected:
    virtual wxEvent* Clone() const;

private:
    bool         m_fHasMessage;
    bool         m_fEnabledFlag;
    int          m_lineNumber;
    wxString     m_fileName;
    wxString     m_strMessage;
    int          m_nReference;
    wxLuaDebugData *m_pDebugData;
    DECLARE_DYNAMIC_CLASS(wxLuaDebugEvent)
};

typedef void (wxEvtHandler::*wxLuaDebugEventFunction)(wxLuaDebugEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_CLIENT_CONNECTED,    2510)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_BREAK,               2511)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_PRINT,               2512)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_ERROR,               2513)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_EXIT,                2514)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_STACK_ENUM,          2515)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_STACK_ENTRY_ENUM,    2516)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_TABLE_ENUM,          2517)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_EVALUATE_EXPR,       2518)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_STARTDEBUGGER,       2519)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_STOPDEBUGGER,        2520)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_BREAKPOINT_ADDED,    2521)
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUG_BREAKPOINT_REMOVED,  2522)
END_DECLARE_EVENT_TYPES()

#define EVT_DEBUG_CLIENT_CONNECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_BREAK,               id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_BREAK(id, fn)            DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_BREAK,               id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_PRINT(id, fn)            DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_PRINT,               id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_ERROR(id, fn)            DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_ERROR,               id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_EXIT(id, fn)             DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_EXIT,                id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_STACK_ENUM(fn)           DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_STACK_ENUM,          -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_STACK_ENTRY_ENUM(fn)     DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_STACK_ENTRY_ENUM,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_TABLE_ENUM(fn)           DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_TABLE_ENUM,          -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_EVALUATE_EXPR(id, fn)    DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_EVALUATE_EXPR,       id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_STARTDEBUGGER(fn)        DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_STARTDEBUGGER,       -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_STOPDEBUGGER(fn)         DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_STOPDEBUGGER,        -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_BREAKPOINT_ADDED(fn)     DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_BREAKPOINT_ADDED,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),
#define EVT_DEBUG_BREAKPOINT_REMOVED(fn)   DECLARE_EVENT_TABLE_ENTRY(wxEVT_DEBUG_BREAKPOINT_REMOVED,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaDebugEventFunction) & fn, (wxObject *) NULL),

#endif // WX_LUA_DEBUG_H
