/////////////////////////////////////////////////////////////////////////////
// Purpose:     Implements the client end of wxLua debugging session
// Author:      J. Winwood
// Created:     May 2002
// RCS-ID:      $Id: wxLuaDebugTarget.h,v 1.2 2003/10/18 03:20:54 john Exp $
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef LUA_DEBUG_TARGET_H
#define LUA_DEBUG_TARGET_H

#ifdef __GNUG__
    #pragma interface "wxLuaDebugTarget.h"
#endif

#include "wx/dynarray.h"
#include "../Library/wxLua.h"
#include "../Library/wxLuaInternals.h"
#include "../Library/wxLuaLibrary.h"
#include "../Library/wxLuaDebug.h"
#include "../Library/wxLuaDebugIO.h"
#include "../Library/wxLuaSocket.h"
#include "../Library/wxLuaStackTree.h"

class LuaDebugTarget : public wxObject, public wxLuaDebugIO
{
private:
    class LuaThread : public wxThread
    {
    public:
        LuaThread(LuaDebugTarget *pTarget) :
            wxThread(wxTHREAD_JOINABLE)
        {
            m_pTarget = pTarget;
        }

        bool TestDestroy();

    protected:
        // thread execution starts here
        virtual void *Entry();

        // called when the thread exits
        virtual void OnExit();

    private:
        LuaDebugTarget *m_pTarget;
    };

    class BreakPointObject : public wxObject
    {
        public:
            BreakPointObject(const wxString &fileName,
                             int             lineNumber)
                :   m_fileName(fileName),
                    m_lineNumber(lineNumber)
            {
            }

            const wxString &GetFileName() const { return m_fileName; }
            int  GetLineNumber() const { return m_lineNumber; }

        private:
            wxString m_fileName;
            int      m_lineNumber;
    };

public:

    LuaDebugTarget(lua_State      *state,
                   const wxString &serverName,
                   int             portNumber);
    ~LuaDebugTarget();

    bool Run();
    void Stop();

    void ThreadFunction();
    void DisplayError(const wxString &strError);

protected:

    enum debugOperations
    {
        DEBUG_STEP,
        DEBUG_STEPOVER,
        DEBUG_STEPOUT,
        DEBUG_GO
    };

    lua_State          *m_luaState;
    int                 m_portNumber;
    wxString            m_serverName;
    wxSortedArrayString m_breakPointList;
#if wxCHECK_VERSION(2, 3, 3)
    wxMutex             m_debugMutex;
#endif
    wxCondition         m_debugCondition;
    wxLuaSocket         m_clientSocket;
    debugOperations     m_nextOperation;
    bool                m_forceBreak;
    bool                m_resetRequested;
    bool                m_fConnected;
    bool                m_fRunning;
    bool                m_fStopped;
    bool                m_fErrorsSeen;
    int                 m_nFramesUntilBreak;
#if wxCHECK_VERSION(2, 3, 3)
    wxMutex             m_runMutex;
#endif
    wxCondition         m_runCondition;
    wxLuaBufferArray    m_bufferList;
    LuaThread          *m_pThread;
    wxArrayInt          m_references;

    void ExitThread();

    mutable wxCriticalSection m_breakPointListCriticalSection;

    bool AddBreakPoint(const wxString &fileName, int lineNumber);
    bool RemoveBreakPoint(const wxString &fileName, int lineNumber);
    bool ClearAllBreakPoints();
    bool Run(const wxString &fileName, const wxString &buffer);
    bool Step();
    bool StepOver();
    bool StepOut();
    bool Continue();
    bool Break();
    bool Reset();
    bool EnumerateStack();
    bool EnumerateStackEntry(int stackRef);
    bool EnumerateTable(int tableRef, int nIndex, int itemNode);
    bool EvaluateExpr(int exprRef, const wxString &strExpr);

    bool NotifyBreak(const wxString &fileName, int lineNumber);
    bool NotifyPrint(const wxString &errorMsg);
    bool NotifyError(const wxString &errorMsg);
    bool NotifyExit();
    bool NotifyStackEnumeration(wxLuaDebugData *pDebugData);
    bool NotifyStackEntryEnumeration(int stackRef, wxLuaDebugData *pDebugData);
    bool NotifyTableEnumeration(int itemNode, wxLuaDebugData *pDebugData);
    bool NotifyEvaluateExpr(int exprRef, const wxString &strResult);

    bool DebugHook(int event);

private:
    bool WaitForConnect() const;
    bool AtBreakPoint(const wxString &fileName, int lineNumber) const;
    wxString GetTableInfo(int index);
    wxString GetUserDataInfo(int index, bool full = true);
    wxString MakeBoolean(int num);
    wxString MakeNumber(double dnum);
    wxString GetDebugInfo(int &lineNumber) const;

    static void LUACALL LuaDebugHook(lua_State *state, lua_Debug *debug);
    static int  LUACALL LuaPrint (lua_State *L);
};

#endif // LUA_DEBUG_TARGET_H
