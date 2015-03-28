/////////////////////////////////////////////////////////////////////////////
// Purpose:     Implements the client end of wxLua debugging session
// Author:      J. Winwood
// Created:     May 2002
// RCS-ID:      $Id: wxLuaDebugTarget.cpp,v 1.3 2004/03/31 00:41:21 john Exp $
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaDebugTarget.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxLuaDebugTarget.h"

static const char *debugName = "__wxLuaDebug__";
static const int  connectTimeout = 200;            // 20 seconds

//----------------------------------------------------------------------------

LuaDebugTarget::LuaDebugTarget(lua_State      *luaState,
                               const wxString &serverName,
                               int             portNumber) :
    m_luaState(luaState),
    m_portNumber(portNumber),
    m_serverName(serverName),
#if wxCHECK_VERSION(2, 3, 3)
    m_debugCondition(m_debugMutex),
#endif
    m_forceBreak(false),
    m_resetRequested(false),
    m_fConnected(false),
    m_fRunning(false),
    m_fStopped(false),
    m_fErrorsSeen(false),
    m_nFramesUntilBreak(0),
#if wxCHECK_VERSION(2, 3, 3)
    m_runCondition(m_runMutex),
#endif
    m_pThread(NULL)
{
    lua_pushlightuserdata(luaState, this);
    lua_setglobal(luaState, debugName);

    lua_sethook(luaState, LuaDebugHook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);

    lua_pushcfunction(m_luaState, LuaPrint);
    lua_setglobal(m_luaState, "print");

    wxGetLuaHandler().EnterLuaCriticalSection();
}

LuaDebugTarget::~LuaDebugTarget()
{
    if (m_pThread != NULL)
        delete m_pThread;

    wxGetLuaHandler().LeaveLuaCriticalSection();
}

void LuaDebugTarget::DisplayError(const wxString &strError)
{
    NotifyError(strError);
}

bool LuaDebugTarget::WaitForConnect() const
{
    for (int idx = 0; idx < connectTimeout; ++idx)
	{
		if (m_fConnected)
			break;

		wxUsleep(100);
	}
	return m_fConnected;
}

bool LuaDebugTarget::Run()
{
	// Assume something is going to go wrong
	m_fErrorsSeen = true;

	m_pThread = new LuaThread(this);
	// Start the thread
    if (m_pThread != NULL						 &&
		m_pThread->Create() == wxTHREAD_NO_ERROR &&
        m_pThread->Run()    == wxTHREAD_NO_ERROR)
    {
		// Wait for the connection to the server to complete
		if (!WaitForConnect())
		{
			wxMessageBox(wxT("Unable to connect to server"), wxT("wxLua client"), wxOK | wxCENTRE, NULL);
		}
		else
		{
			// OK, now we can start running.
			m_runCondition.Wait();

			m_fRunning    = true;
			m_fErrorsSeen = false;

			size_t idx;
			for (idx = 0; idx < m_bufferList.Count(); ++idx)
			{
				int rc = LUA_ERRERR;

				const wxLuaBuffer *item = m_bufferList.Item(idx);

                rc = LuaDoBuffer(m_luaState,
                                 wx2lua(item->GetBuffer()),
                                 item->GetBufferLength(),
                                 wx2lua(item->GetFileName()));

				delete item;

                wxString errorMsg;
                m_fErrorsSeen = !wxLuaCheckRunError(rc, &errorMsg);
                if (m_fErrorsSeen)
                {
                    NotifyError(errorMsg);
                    break;
                }
			}
		}
	}
    return !m_fErrorsSeen;
}

void LuaDebugTarget::Stop()
{
    NotifyExit();
    if (m_fConnected)
    {
        m_clientSocket.Shutdown(SD_BOTH);
        wxUsleep(100);
        m_clientSocket.Close();
    }
	m_pThread->Wait();
}

void *LuaDebugTarget::LuaThread::Entry()
{
	m_pTarget->ThreadFunction();
	return 0;
}

void LuaDebugTarget::LuaThread::OnExit()
{
}

bool LuaDebugTarget::LuaThread::TestDestroy()
{
	return wxThread::TestDestroy();
}

void LuaDebugTarget::ThreadFunction()
{
    bool fThreadRunning = false;
    try
    {
        m_clientSocket.Connect(m_serverName, m_portNumber);

        m_fConnected    = true;
        fThreadRunning = true;
    }
    catch(...)
    {
    }

    while (fThreadRunning && !m_pThread->TestDestroy())
    {
        try
        {
            debugCommands debugCommand = (debugCommands) ReadByte(&m_clientSocket);
            switch(debugCommand)
            {
                case CMD_ADD_BREAKPOINT:
                    {
                        wxString fileName   = ReadString(&m_clientSocket);
                        int      lineNumber = ReadInt(&m_clientSocket);

                        AddBreakPoint(fileName, lineNumber);
                    }
                    break;

                case CMD_REMOVE_BREAKPOINT:
                    {
                        wxString fileName   = ReadString(&m_clientSocket);
                        int      lineNumber = ReadInt(&m_clientSocket);

                        RemoveBreakPoint(fileName, lineNumber);
                    }
                    break;

                case CMD_CLEAR_ALL_BREAKPOINTS:
                    ClearAllBreakPoints();
                    break;

                case CMD_RUN_BUFFER:
                    {
                        wxString fileName = ReadString(&m_clientSocket);
                        wxString buffer   = ReadString(&m_clientSocket);

                        Run(fileName, buffer);
                    }
                    break;

                case CMD_DEBUG_STEP:
					Step();
                    break;

                case CMD_DEBUG_STEPOVER:
                    StepOver();
                    break;

                case CMD_DEBUG_STEPOUT:
                    StepOut();
                    break;

                case CMD_DEBUG_CONTINUE:
                    Continue();
                    break;

                case CMD_DEBUG_BREAK:
                    Break();
                    break;

                case CMD_ENUMERATE_STACK:
                    EnumerateStack();
                    break;

                case CMD_ENUMERATE_STACK_ENTRY:
                    {
                        int stackRef = ReadInt(&m_clientSocket);
                        EnumerateStackEntry(stackRef);
                    }
                    break;

                case CMD_ENUMERATE_TABLE_REF:
                    {
                        int tableRef  = ReadInt(&m_clientSocket);
                        int index     = ReadInt(&m_clientSocket);
						int itemNode  = ReadInt(&m_clientSocket);
                        EnumerateTable(tableRef, index, itemNode);
                    }
                    break;

                case CMD_RESET:
                    Reset();
                    break;

				case CMD_EVALUATE_EXPR:
					{
						int		 exprRef  = ReadInt(&m_clientSocket);
						wxString buffer   = ReadString(&m_clientSocket);
						EvaluateExpr(exprRef, buffer);
					}
					break;

                case CMD_CLEAR_DEBUG_REFERENCES:
                    {
                        size_t idx, idxMax = m_references.GetCount();
                        for (idx = 0; idx < idxMax; ++idx)
                        {
                            int iItem = m_references.Item(idx);
                            tremove(m_luaState, iItem);
                        }
                    }
                    break;
            }
        }
        catch(...)
        {
            fThreadRunning = false;
        }
    }
}

bool LuaDebugTarget::AddBreakPoint(const wxString &fileName, int lineNumber)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPoint;
    breakPoint = breakPoint.Format(wxT("%u:"), lineNumber) + fileName;

    m_breakPointList.Add(breakPoint);

    return false;
}

bool LuaDebugTarget::RemoveBreakPoint(const wxString &fileName, int lineNumber)
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPoint;
    breakPoint = breakPoint.Format(wxT("%u:"), lineNumber) + fileName;

    m_breakPointList.Remove(breakPoint);

    return true;
}

bool LuaDebugTarget::ClearAllBreakPoints()
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);
    m_breakPointList.Clear();
    return true;
}

bool LuaDebugTarget::Run(const wxString &fileName, const wxString &buffer)
{
    wxLuaBuffer *pBuffer = new wxLuaBuffer(fileName,
                                           buffer);
    if (pBuffer != NULL)
    {
        m_bufferList.Add(pBuffer);
        return true;
    }
    return false;
}

bool LuaDebugTarget::Step()
{
    m_nextOperation = DEBUG_STEP;
    if (!m_fRunning)
        m_runCondition.Signal();
    else
    if (m_fStopped)
        m_debugCondition.Signal();
    return true;
}

bool LuaDebugTarget::StepOver()
{
    m_nFramesUntilBreak = 0;
    m_nextOperation = DEBUG_STEPOVER;
    if (!m_fRunning)
        m_runCondition.Signal();
    else
    if (m_fStopped)
        m_debugCondition.Signal();
    return true;
}

bool LuaDebugTarget::StepOut()
{
    m_nFramesUntilBreak = 1;
    m_nextOperation = DEBUG_STEPOVER;
    if (!m_fRunning)
        m_runCondition.Signal();
    else
    if (m_fStopped)
        m_debugCondition.Signal();
    return true;
}

bool LuaDebugTarget::Continue()
{
    m_nextOperation = DEBUG_GO;
    if (!m_fRunning)
        m_runCondition.Signal();
    else
    if (m_fStopped)
        m_debugCondition.Signal();
    return true;
}

bool LuaDebugTarget::Break()
{
    m_forceBreak = TRUE;
    return true;
}

bool LuaDebugTarget::Reset()
{
    m_forceBreak     = TRUE;
    m_resetRequested = TRUE;
    if (!m_fRunning)
        m_runCondition.Signal();
    else
    if (m_fStopped)
        m_debugCondition.Signal();
    return true;
}

bool LuaDebugTarget::EnumerateStack()
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL)
    {
        int       nIndex = 0;
        bool      fFirst = true;
        lua_Debug luaDebug;

		wxGetLuaHandler().EnterLuaCriticalSection();

        while (lua_getstack(m_luaState, nIndex, &luaDebug) != 0)
        {
            if (lua_getinfo(m_luaState, "Sln", &luaDebug))
            {
                wxString stackItem;

                // skip stack frames that do not have line number
                int  currentLine = luaDebug.currentline;
                if (!fFirst || currentLine != -1)
                {
                    if (currentLine == -1)
                        currentLine = 0;

                    if (luaDebug.name != NULL)
                        stackItem.Printf(wxT("function %s line %u"), luaDebug.name, currentLine);
                    else
                        stackItem.Printf(wxT("line %u"), currentLine);

                    wxLuaDebugDataItem *item = new wxLuaDebugDataItem(stackItem, wxT(""), wxT(""), wxT(""), nIndex, 0);
                    if (item != NULL)
                        pSortedList->Add(item);

                    fFirst = false;
                }
            }

            ++nIndex;
        }
		wxGetLuaHandler().LeaveLuaCriticalSection();
    }
    return NotifyStackEnumeration(pSortedList);
}

bool LuaDebugTarget::EnumerateStackEntry(int stackRef)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData;
    if (pSortedList != NULL)
    {
		wxGetLuaHandler().EnterLuaCriticalSection();
		{
			lua_Debug luaDebug;
			if (lua_getstack(m_luaState, stackRef, &luaDebug) != 0)
			{
				int idx  = 1;
				wxString name = lua2wx(lua_getlocal(m_luaState, &luaDebug, idx));
				while (!name.IsEmpty())
				{
					bool fIsTable = false;
					wxString type;
					wxString value;

					switch (lua_type(m_luaState, -1))
					{
						case LUA_TNIL:
							type  = wxT("Nil");
							value = wxT("nil");
							break;

						case LUA_TBOOLEAN:
							type = wxT("Boolean");
							value = MakeBoolean(lua_toboolean(m_luaState, -1));
							break;

						case LUA_TNUMBER:
							type = wxT("Number");
							value = MakeNumber(lua_tonumber(m_luaState, -1));
							break;

						case LUA_TSTRING:
							type  = wxT("String");
							value = lua2wx(lua_tostring(m_luaState, -1));
							break;

						case LUA_TTABLE:
							type  = wxT("Table");
							value = GetTableInfo(-1);
							fIsTable = true;
							break;

						case LUA_TFUNCTION:
							if (lua_iscfunction(m_luaState, -1))
							{
								type  = wxT("C Function");
								value.Printf(wxT("%p"), lua_tocfunction(m_luaState, -1));
							}
							else
							{
								type = wxT("Lua Function");
								value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
							}
							break;

						case LUA_TUSERDATA:
							type  = wxT("User Data");
							value = GetUserDataInfo(-1);
							break;

						case LUA_TLIGHTUSERDATA:
							type  = wxT("Light User Data");
							value = GetUserDataInfo(-1, false);
							break;

						case LUA_TTHREAD:
							type = wxT("Thread");
							value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
							break;
					}

					int nRef = LUA_NOREF;

					if (lua_istable(m_luaState, -1))
					{
						lua_pushvalue(m_luaState, -1);
						nRef = tinsert(m_luaState, -1);
						m_references.Add(nRef);
					}

					wxLuaDebugDataItem *item = new wxLuaDebugDataItem(name, type, value, wxT(""), nRef, 0);
					if (item != NULL)
						pSortedList->Add(item);

					name = lua2wx(lua_getlocal(m_luaState, &luaDebug, ++idx));
				}
			}
		}
		wxGetLuaHandler().LeaveLuaCriticalSection();
    }
    return NotifyStackEntryEnumeration(stackRef, pSortedList);
}

bool LuaDebugTarget::EnumerateTable(int tableRef, int nIndex, int nItemNode)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL)
    {
		wxGetLuaHandler().EnterLuaCriticalSection();
		{
			if (tableRef == -1)
			{
				lua_pushvalue(m_luaState, LUA_GLOBALSINDEX);
				int nRef = tinsert(m_luaState, -1);
				pSortedList->Add(new wxLuaDebugDataItem(wxT("Globals"),
											   wxT(""),
                                               wxT(""),
                                               wxT(""),
											   nRef,
											   0));
				m_references.Add(nRef);
			}
			else
			{
				if (tget(m_luaState, tableRef) != 0)
				{
					int nTop = lua_gettop(m_luaState);

					// start iterating
					lua_pushnil(m_luaState);
					while (lua_next(m_luaState, nTop) != 0)
					{
						bool fIsTable = false;
						wxString type;
						wxString value;
						wxString name;

						// get the index
						switch (lua_type(m_luaState, -2))
						{
							case LUA_TNIL:
								name = wxT("Nil");
								break;

							case LUA_TBOOLEAN:
								name = MakeBoolean(lua_toboolean(m_luaState, -2));
								break;

							case LUA_TNUMBER:
								name = MakeNumber(lua_tonumber(m_luaState, -2));
								break;

							case LUA_TSTRING:
								name = lua2wx(lua_tostring(m_luaState, -2));
								break;

							case LUA_TTABLE:
								name = GetTableInfo(-2);
								break;

							case LUA_TFUNCTION:
								if (lua_iscfunction(m_luaState, -2))
									name.Printf(wxT("%p"), lua_tocfunction(m_luaState, -2));
								else
									name.Printf(wxT("%p"), lua_topointer(m_luaState, -2));
								break;

							case LUA_TUSERDATA:
								name = GetUserDataInfo(-2);
								break;

							case LUA_TLIGHTUSERDATA:
								name = GetUserDataInfo(-2, false);
								break;

							case LUA_TTHREAD:
								name.Printf(wxT("%p"), lua_topointer(m_luaState, -2));
								break;
						}

						// get the value
						switch (lua_type(m_luaState, -1))
						{
							case LUA_TNIL:
								type  = wxT("Nil");
								value = wxT("nil");
								break;

							case LUA_TBOOLEAN:
								value = MakeBoolean(lua_toboolean(m_luaState, -1));
								type = wxT("Boolean");
								break;

							case LUA_TNUMBER:
								value = MakeNumber(lua_tonumber(m_luaState, -1));
								type = wxT("Number");
								break;

							case LUA_TSTRING:
								type  = wxT("String");
								value = lua2wx(lua_tostring(m_luaState, -1));
								break;

							case LUA_TTABLE:
								type  = wxT("Table");
								value = GetTableInfo(-1);
								fIsTable = true;
								break;

							case LUA_TFUNCTION:
								if (lua_iscfunction(m_luaState, -1))
								{
									type  = wxT("C Function");
									value.Printf(wxT("%p"), lua_tocfunction(m_luaState, -1));
								}
								else
								{
									type = wxT("Lua Function");
									value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
								}
								break;

							case LUA_TUSERDATA:
								type  = wxT("User Data");
								value = GetUserDataInfo(-1);
								break;

							case LUA_TLIGHTUSERDATA:
								type  = wxT("Light User Data");
								value = GetUserDataInfo(-1, false);
								break;

							case LUA_TTHREAD:
								type = wxT("Thread");
								value.Printf(wxT("%p"), lua_topointer(m_luaState, -1));
								break;
						}

						int nRef = LUA_NOREF;
						if (fIsTable)
						{
							nRef = tinsert(m_luaState, -1);
							m_references.Add(nRef);
						}
						else
							lua_pop(m_luaState, 1);

						wxLuaDebugDataItem *item = new wxLuaDebugDataItem(name, type, value, wxT(""), nRef, nIndex);
						if (item != NULL)
							pSortedList->Add(item);
					}

					// remove reference
					lua_pop(m_luaState, 1);
				}
			}
		}
		wxGetLuaHandler().LeaveLuaCriticalSection();
	}
    return NotifyTableEnumeration(nItemNode, pSortedList);
}

bool LuaDebugTarget::EvaluateExpr(int exprRef, const wxString &strExpr)
{
    wxString strResult(wxT("Error"));
    int      nReference = LUA_NOREF;

    wxGetLuaHandler().EnterLuaCriticalSection();
	{
		if (wxStrpbrk(strExpr.c_str(), wxT(" ~=<>+-*/%(){}[]:;,.\"'")) != NULL)
		{
			// an expression
			int nOldTop = lua_gettop(m_luaState);

            LuaCharBuffer charbuf(strExpr);
            int nResult = luaL_loadbuffer(m_luaState, charbuf.GetData(), charbuf.Length(), "debug");

            if (nResult == 0)
                nResult = lua_pcall(m_luaState, 0, LUA_MULTRET, 0);  // call main

            wxString errorMsg;
            if (!wxLuaCheckRunError(nResult, &errorMsg))
                lua_pushlstring(m_luaState, wx2lua(errorMsg), errorMsg.Len());
            else if (lua_gettop(m_luaState) == nOldTop)
                lua_pushliteral(m_luaState, "OK");

			nReference = tinsert(m_luaState, -1);
			lua_settop(m_luaState, nOldTop);
		}
		else
		{
			 lua_Debug ar;
			 int	   iLevel = 0;
			 bool      fFound = false;

			 while (lua_getstack(m_luaState, iLevel++, &ar) != 0)
			 {
				int	   iIndex = 0;
				wxString name = lua2wx(lua_getlocal(m_luaState, &ar, ++iIndex));
				if (!name.IsEmpty())
				{
					if (strExpr == name)
					{
						nReference = tinsert(m_luaState, -1);
						fFound = true;
						break;
					}
                    else
                        lua_pop(m_luaState, 1);
				}

				if (fFound)
                    break;

                name = lua2wx(lua_getlocal(m_luaState, &ar, ++iIndex));
			 }

			 if (!fFound)
			 {
				  int nOldTop = lua_gettop(m_luaState);
				  lua_pushvalue(m_luaState, LUA_GLOBALSINDEX);
				  lua_pushnil(m_luaState);
				  while (lua_next(m_luaState, -2) != 0)
				  {
					  if (lua_type(m_luaState, -2) == LUA_TSTRING)
					  {
						  wxString name = lua2wx(lua_tostring(m_luaState, -2));
						  if (strExpr == name)
						  {
							  nReference = tinsert(m_luaState, -1); // reference value and pop it.
							  lua_pop(m_luaState, 1);	// pop index
							  fFound = true;
							  break;
						  }
						  else
							  lua_pop(m_luaState, 1);  // removes `value';
					  }
					  else
						  lua_pop(m_luaState, 1);  // removes `value';
				  }
				  lua_settop(m_luaState, nOldTop); // the table of globals.
			 }
		}

		if (nReference != LUA_NOREF && tget(m_luaState, nReference) != 0)
		{
			tremove(m_luaState, nReference);
			switch (lua_type(m_luaState, -1))
			{
				case LUA_TNIL:
					strResult = wxT("Nil");
					break;

				case LUA_TBOOLEAN:
					strResult = MakeBoolean(lua_toboolean(m_luaState, -1));
					break;

				case LUA_TNUMBER:
					strResult = MakeNumber(lua_tonumber(m_luaState, -1));
					break;

				case LUA_TSTRING:
					strResult = wxString::Format(wxT("\"%s\""), lua_tostring(m_luaState, -1));
					break;

				case LUA_TTABLE:
					strResult = wxT("table ") + GetTableInfo(-1);
					break;

				case LUA_TFUNCTION:
					if (lua_iscfunction(m_luaState, -1))
						strResult = wxString::Format(wxT("C-function %p"), lua_tocfunction(m_luaState, -1));
					else
						strResult = wxString::Format(wxT("lua function %p"), lua_topointer(m_luaState, -1));
					break;

				case LUA_TUSERDATA:
					strResult = wxT("user data ") + GetUserDataInfo(-1);
					break;

				case LUA_TLIGHTUSERDATA:
					strResult = wxT("light user data ") + GetUserDataInfo(-1, false);
					break;

				case LUA_TTHREAD:
					strResult = wxString::Format(wxT("thread %p"), lua_topointer(m_luaState, -1));
					break;

				default:
					break;
			}

			lua_pop(m_luaState, 1);
		}
    }
    wxGetLuaHandler().LeaveLuaCriticalSection();

    return NotifyEvaluateExpr(exprRef, strResult);
}

wxString LuaDebugTarget::GetTableInfo(int index)
{
    int         nItems = luaL_getn(m_luaState, index);
    const void *pItem  = lua_topointer(m_luaState, index);

    if (nItems == 0)
        return wxString::Format(wxT("%p"), pItem);

    return wxString::Format(wxT("%p (approx %u items)"), pItem, nItems);
}

wxString LuaDebugTarget::GetUserDataInfo(int index, bool full)
{
    if (full)
    {
        int nTag = ttag(m_luaState, index);

        const char *pTagName = NULL;
        if (nTag != TLUA_NOTAG)
            pTagName = GetLuaTagName(m_luaState, nTag);

        if (pTagName != NULL)
            return wxString::Format(wxT("%p (%s)"), lua_touserdata(m_luaState, index), pTagName);

        return wxString::Format(wxT("%p (%u)"), lua_touserdata(m_luaState, index), nTag);
    }

    return wxString::Format(wxT("%p"), lua_touserdata(m_luaState, index));
}

wxString LuaDebugTarget::MakeNumber(double dnum)
{
    long num = (long) dnum;
    if ((double) num == dnum)
    {
        if (num >= 0)
            return wxString::Format(wxT("%lu (0x%lx)"), num, num);

        return wxString::Format(wxT("%ld (0x%lx)"), num, num);
    }

    return wxString::Format(wxT("%g"), dnum);
}

wxString LuaDebugTarget::MakeBoolean(int num)
{
    if (num)
        return wxT("True");

    return wxT("False");
}

bool LuaDebugTarget::NotifyBreak(const wxString &fileName, int lineNumber)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_BREAK);
        WriteString(&m_clientSocket, fileName);
        WriteInt(&m_clientSocket, lineNumber);
        return true;
    }
    return false;
}

bool LuaDebugTarget::NotifyPrint(const wxString &errorMsg)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_ERROR);
        WriteString(&m_clientSocket, errorMsg);
        return true;
    }
    return false;
}

bool LuaDebugTarget::NotifyError(const wxString &errorMsg)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_ERROR);
        WriteString(&m_clientSocket, errorMsg);
        return true;
    }
    else
        wxMessageBox(errorMsg, wxT("wxLua client error"), wxOK | wxCENTRE, NULL);

    m_fErrorsSeen = true;
    return false;
}

bool LuaDebugTarget::NotifyExit()
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_EXIT);
        return true;
    }
    return false;
}

bool LuaDebugTarget::NotifyStackEnumeration(wxLuaDebugData *pDebugData)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_STACK_ENUM);
        WriteDebugData(&m_clientSocket, pDebugData);
        return true;
    }
	delete pDebugData;
    return false;
}

bool LuaDebugTarget::NotifyStackEntryEnumeration(int          entryRef,
                                                 wxLuaDebugData *pDebugData)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_STACK_ENTRY_ENUM);
        WriteInt(&m_clientSocket, entryRef);
        WriteDebugData(&m_clientSocket, pDebugData);
        return true;
    }
	delete pDebugData;
    return false;
}

bool LuaDebugTarget::NotifyTableEnumeration(int          itemNode,
                                            wxLuaDebugData *pDebugData)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_TABLE_ENUM);
		WriteInt(&m_clientSocket, itemNode);
        WriteDebugData(&m_clientSocket, pDebugData);
        return true;
    }
	delete pDebugData;
    return false;
}

bool LuaDebugTarget::NotifyEvaluateExpr(int				exprRef,
                                        const wxString &strResult)
{
    if (WaitForConnect())
    {
        WriteByte(&m_clientSocket, EVENT_DEBUG_EVALUATE_EXPR);
        WriteInt(&m_clientSocket, exprRef);
        WriteString(&m_clientSocket, strResult);
        return true;
    }
    return false;
}

wxString LuaDebugTarget::GetDebugInfo(int &lineNumber) const
{
	lua_Debug luaDebug;
	lua_getstack(m_luaState, 0, &luaDebug);
	lua_getinfo(m_luaState, "Sln", &luaDebug);
	lineNumber = luaDebug.currentline - 1;
	return lua2wx(luaDebug.source);
}

bool LuaDebugTarget::DebugHook(int event)
{
    bool fWait = false;
    m_fStopped = true;
    if (m_forceBreak)
    {
        if (m_resetRequested)
		{
		    NotifyExit();
            wxExit();
		}

		int      lineNumber = 0;
		wxString fileName   = GetDebugInfo(lineNumber);

        if (NotifyBreak(fileName, lineNumber))
            fWait = true;
    }
    else
    {
        if (event == LUA_HOOKCALL) // call
            m_nFramesUntilBreak++;
        else
        if (event == LUA_HOOKRET || event == LUA_HOOKTAILRET) // return
        {
            if (m_nFramesUntilBreak > 0)
                m_nFramesUntilBreak--;
        }
        else
        if (event == LUA_HOOKLINE) // line
        {
            switch (m_nextOperation)
            {
                case DEBUG_STEP:
                    {
						int      lineNumber = 0;
						wxString fileName   = GetDebugInfo(lineNumber);

                        if (NotifyBreak(fileName, lineNumber))
                            fWait = true;
                    }
                    break;

                case DEBUG_STEPOVER:
                    if (m_nFramesUntilBreak == 0)
                    {
						int      lineNumber = 0;
						wxString fileName   = GetDebugInfo(lineNumber);

                        if (NotifyBreak(fileName, lineNumber))
                            fWait = true;
                    }
                    break;

                case DEBUG_GO:
                default:
                    {
						int      lineNumber = 0;
						wxString fileName   = GetDebugInfo(lineNumber);

                        if (AtBreakPoint(fileName, lineNumber) &&
                            NotifyBreak(fileName, lineNumber))
                            fWait = true;
                    }
                    break;
            }
        }
    }

    if (fWait)
    {
		// release the critical section so
		// the other thread can access LUA
		wxGetLuaHandler().LeaveLuaCriticalSection();
		// Wait for a command
        m_debugCondition.Wait();
		// acquire the critical section again
		wxGetLuaHandler().EnterLuaCriticalSection();
    }
    m_fStopped = false;
    return fWait;
}

void LUACALL LuaDebugTarget::LuaDebugHook(lua_State *luaState, lua_Debug *pLuaDebug)
{
    lua_getglobal(luaState, debugName);
    LuaDebugTarget *pTarget = (LuaDebugTarget *) lua_touserdata(luaState, -1);
    lua_pop(luaState, 1);

    if (pTarget != NULL)
        pTarget->DebugHook(pLuaDebug->event);
}

bool LuaDebugTarget::AtBreakPoint(const wxString &fileName,
                                  int             lineNumber) const
{
    wxCriticalSectionLocker locker(m_breakPointListCriticalSection);

    wxString breakPoint;
    breakPoint = breakPoint.Format(wxT("%u:"), lineNumber) + fileName;

    return (m_breakPointList.Index(breakPoint) != wxNOT_FOUND);
}

int LUACALL LuaDebugTarget::LuaPrint (lua_State *L)
{
	int		 idx;
	wxString stream;
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    for (idx = 1; idx <= n;  idx++)
    {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, idx);   /* value to print */
        lua_call(L, 1, 1);
        wxString s = lua2wx(lua_tostring(L, -1));  /* get result */
        if (s.IsEmpty())
            return luaL_error(L, "`tostring' must return a string to `print'");
		if (idx > 1)
			stream.Append(wxT("\t"));
		stream.Append(s);
        lua_pop(L, 1);  /* pop result */
    }

    lua_getglobal(L, debugName);
    LuaDebugTarget *pTarget = (LuaDebugTarget *) lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (pTarget != NULL)
        pTarget->NotifyPrint(stream);

	return 0;
}
