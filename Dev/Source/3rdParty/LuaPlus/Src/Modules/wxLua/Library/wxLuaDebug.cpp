/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaDebug.cpp
// Purpose:     Debugging I/O functions for wxLua
// Author:      J. Winwood
// Created:     May 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
// Ray Gilbert 

#ifdef __GNUG__
    #pragma implementation "wxLuaDebug.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxLuaDebug.h"
#include "wxLuaInternals.h"

DEFINE_EVENT_TYPE(wxEVT_DEBUG_CLIENT_CONNECTED)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_BREAK)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_PRINT)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_ERROR)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_EXIT)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_STACK_ENUM)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_STACK_ENTRY_ENUM)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_TABLE_ENUM)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_EVALUATE_EXPR)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_STARTDEBUGGER)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_STOPDEBUGGER)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_BREAKPOINT_ADDED)
DEFINE_EVENT_TYPE(wxEVT_DEBUG_BREAKPOINT_REMOVED)

// ----------------------------------------------------------------------------
// wxLuaBuffer
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxLuaBuffer, wxObject)

wxLuaBuffer::wxLuaBuffer(const wxString &fileName, const wxString &buffer) :
             m_fileName(fileName),
             m_buffer(buffer)
{
}

// ----------------------------------------------------------------------------
// wxLuaBuffer
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxLuaDebuggerBase, wxEvtHandler)

// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxLuaDebugEvent, wxEvent)

wxLuaDebugEvent::wxLuaDebugEvent()
{
    m_fHasMessage = false;
    m_pDebugData  = NULL;
    m_nReference  = -1;
    m_fEnabledFlag = false;
}

wxLuaDebugEvent::wxLuaDebugEvent(int eventType)
{
    SetEventType(eventType);
    
    m_fHasMessage = false;
    m_pDebugData  = NULL;
    m_nReference  = -1;
    m_fEnabledFlag = false;
}

wxLuaDebugEvent::wxLuaDebugEvent(int         eventType,
                                 int         lineNumber,    
                                 const wxString &file,
                                 bool        enabledFlag)
{
    SetEventType(eventType);
    
    m_lineNumber  = lineNumber;
    m_fileName    = file;
    m_fHasMessage = false;
    m_nReference  = -1;
    m_pDebugData  = NULL;  
    m_fEnabledFlag = enabledFlag;
}

wxLuaDebugEvent::~wxLuaDebugEvent()
{
    if (m_pDebugData != NULL)
        delete m_pDebugData;
}

void wxLuaDebugEvent::SetMessage(const wxString &pMessage)
{
    m_strMessage  = pMessage;
    m_fHasMessage = true;
}

void wxLuaDebugEvent::SetDebugData(int nReference, wxLuaDebugData *pData)
{
    m_nReference = nReference;
    m_pDebugData = pData;
}

int wxLuaDebugEvent::GetLineNumber() const
{
    return m_lineNumber;
}

wxString wxLuaDebugEvent::GetFileName() const
{
    return m_fileName;
}

wxString wxLuaDebugEvent::GetMessage() const
{
    return m_strMessage;
}

bool wxLuaDebugEvent::GetEnabledFlag() const
{
    return m_fEnabledFlag;
}

const wxLuaDebugData *wxLuaDebugEvent::GetDebugData() const
{
    return m_pDebugData;
}

int wxLuaDebugEvent::GetReference() const
{
    return m_nReference;
}

wxEvent *wxLuaDebugEvent::Clone() const
{
    wxLuaDebugEvent *pEvent = new wxLuaDebugEvent(GetEventType(),
                                                  m_lineNumber,
                                                  m_fileName);
    if (pEvent != NULL)
    {
        if (m_fHasMessage)
            pEvent->SetMessage(m_strMessage);

        if (m_pDebugData != NULL)
            pEvent->SetDebugData(m_nReference, m_pDebugData->Clone());
        else
            pEvent->SetDebugData(m_nReference);
    }
    return pEvent;
}

// ----------------------------------------------------------------------------
// Debug Data Item 
// ----------------------------------------------------------------------------
wxLuaDebugDataItem::wxLuaDebugDataItem(const wxString &itemName,
                                       const wxString &itemType,
                                       const wxString &itemValue,
                                       const wxString &itemSource,
                                       int         nReference,
                                       int         nIndex,
                                       bool        fExpanded)
{
    m_itemName    = itemName;
    m_itemType    = itemType;
    m_itemValue   = itemValue;
    m_itemSource  = itemSource;
    m_nReference  = nReference;
    m_nIndex      = nIndex;
    m_fExpanded   = fExpanded;
}

wxLuaDebugDataItem::wxLuaDebugDataItem(const wxLuaDebugDataItem &treeItem)
{
    m_itemName    = treeItem.m_itemName;
    m_itemType    = treeItem.m_itemType;
    m_itemValue   = treeItem.m_itemValue;
    m_itemSource  = treeItem.m_itemSource;
    m_nReference  = treeItem.m_nReference;
    m_nIndex      = treeItem.m_nIndex;
    m_fExpanded   = treeItem.m_fExpanded;
}

//////////////////////////////////////////////////////////////////////////
// Debug Data
//
// Debug Info sent via socket to debugger client
wxLuaDebugData::wxLuaDebugData() :
    wxLuaDebugDataItemArray(SortFunction)
{
}

wxLuaDebugData::~wxLuaDebugData()
{
    size_t idx;
    for (idx = 0; idx < Count(); ++idx)
    {
        const wxLuaDebugDataItem *pData = Item(idx);
        delete pData;
    }
}

wxLuaDebugData *wxLuaDebugData::Clone()
{
    wxLuaDebugData *pCloneData = new wxLuaDebugData;
    if (pCloneData != NULL)
    {
        size_t idx;
        for (idx = 0; idx < Count(); ++idx)
        {
            const wxLuaDebugDataItem *pOldData = Item(idx);
            if (pOldData != NULL)
                pCloneData->Add(new wxLuaDebugDataItem(*pOldData));
        }
    }
    return pCloneData;
}

int wxLuaDebugData::SortFunction(wxLuaDebugDataItem *elem1, wxLuaDebugDataItem *elem2 )
{
    return wxStrcmp(elem1->GetName(), elem2->GetName());
}

// ----------------------------------------------------------------------------
// wxLuaInterface (local)
// ----------------------------------------------------------------------------

wxLuaInterface::wxLuaInterface(lua_State *luaState) : m_luaState(luaState)
{
}

wxLuaInterface::~wxLuaInterface()
{
    size_t idx, idxMax = m_references.GetCount();
    for (idx = 0; idx < idxMax; ++idx)
    {
        int iItem = m_references.Item(idx);
        tremove(m_luaState, iItem);
    }
}

int wxLuaInterface::GetStack(int nIndex, lua_Debug *luaDebug)
{
    return lua_getstack(m_luaState, nIndex, luaDebug);
}

bool wxLuaInterface::GetInfo(const wxString &what, lua_Debug *luaDebug)
{
    return (lua_getinfo(m_luaState, wx2lua(what), luaDebug) != 0);
}

wxString wxLuaInterface::GetLocal(lua_Debug *luaDebug, int index)
{
    return lua2wx(lua_getlocal(m_luaState, luaDebug, index));
}

void wxLuaInterface::GetGlobals()
{
    lua_pushvalue(m_luaState, LUA_GLOBALSINDEX);
}

int wxLuaInterface::GetTop()
{
    return lua_gettop(m_luaState);
}

int wxLuaInterface::Type(int index)
{
    return lua_type(m_luaState, index);
}

double wxLuaInterface::ToNumber(int index)
{
    return lua_tonumber(m_luaState, index);
}

int wxLuaInterface::ToBoolean(int index)
{
    return lua_toboolean(m_luaState, index);
}

wxString wxLuaInterface::ToString(int index)
{
    return lua2wx(lua_tostring(m_luaState, index));
}

const void *wxLuaInterface::ToPointer(int index)
{
    return lua_topointer(m_luaState, index);
}

bool wxLuaInterface::IsCFunction(int index)
{
    return lua_iscfunction(m_luaState, index) != 0;
}

const void *wxLuaInterface::ToCFunction(int index)
{
    return (const void *) lua_tocfunction(m_luaState, index);
}

const void *wxLuaInterface::ToUserData(int index)
{
    return lua_touserdata(m_luaState, index);
}

int wxLuaInterface::GetTag(int index)
{
    return ttag(m_luaState, index);
}

int wxLuaInterface::GetN(int index)
{
    return luaL_getn(m_luaState, index);
}

int wxLuaInterface::Ref()
{
    int nReference = tinsert(m_luaState, -1);
    m_references.Add(nReference);
    return nReference;
}

int wxLuaInterface::GetRef(int ref)
{
    return tget(m_luaState, ref);
}

void wxLuaInterface::Pop(int count)
{
    lua_pop(m_luaState, count);
}

void wxLuaInterface::PushNil()
{
    lua_pushnil(m_luaState);
}

void wxLuaInterface::PushValue(int index)
{
    lua_pushvalue(m_luaState, index);
}

int wxLuaInterface::Next(int index)
{
    return lua_next(m_luaState, index);
}

wxLuaDebugData *wxLuaInterface::BuildStack()
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();

    int       nIndex    = 0;
    lua_Debug luaDebug;

    bool fFirst = true;

    while (GetStack(nIndex, &luaDebug) != 0)
    {
        if (GetInfo(wxT("Sln"), &luaDebug))
        {
            wxString itemName;
            wxString source = lua2wx(luaDebug.source);

            // skip stack frames that do not have line number
            int  currentLine = luaDebug.currentline;
            if (!fFirst || (currentLine != -1))
            {
                if (currentLine == -1)
                    currentLine = 0;

                if (luaDebug.name != NULL)
                    itemName.Printf(_("function %s line %u"), luaDebug.name, currentLine);
                else
                    itemName.Printf(_("line %u"), currentLine);

                wxLuaDebugDataItem *item = new wxLuaDebugDataItem(itemName, wxT(""), wxT(""), source, nIndex, 0);
                pSortedList->Add(item);

                fFirst = false;
            }
        }

        ++nIndex;
    }
    return pSortedList;
}

wxLuaDebugData *wxLuaInterface::BuildStackList(int nEntry)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL)
    {
        lua_Debug luaDebug;
        if (GetStack(nEntry, &luaDebug) != 0)
        {
            int         idx  = 1;
            wxString name = GetLocal(&luaDebug, idx);
            while (!name.IsEmpty())
            {
                bool fIsTable = false;
                wxString type;
                wxString value;
                wxString source = lua2wx(luaDebug.source);

                switch (Type(-1))
                {
                    case LUA_TNIL:
                        type  = wxT("Nil");
                        value = wxT("nil");
                        break;

                    case LUA_TBOOLEAN:
                        type = wxT("Boolean");
                        value = MakeBoolean(ToBoolean(-1));
                        break;

                    case LUA_TNUMBER:
                        type = wxT("Number");
                        value = MakeNumber(ToNumber(-1));
                        break;

                    case LUA_TSTRING:
                        type  = wxT("String");
                        value = ToString(-1);
                        break;

                    case LUA_TTABLE:
                        type  = wxT("Table");
                        value = GetTableInfo(-1);
                        fIsTable = true;
                        break;

                    case LUA_TFUNCTION:
                        if (IsCFunction(-1))
                        {
                            type  = wxT("C Function");
                            value.Printf(wxT("%p"), ToCFunction(-1));
                        }
                        else
                        {
                            type = wxT("Lua Function");
                            value.Printf(wxT("%p"), ToPointer(-1));
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
                        type  = wxT("Thread");
                        value.Printf(wxT("%p"), ToPointer(-1));
                        break;
                }

                int nRef = LUA_NOREF;

                if (fIsTable)
                {
                    nRef = Ref();
                }
                else
                    Pop(1);

                wxLuaDebugDataItem *item = new wxLuaDebugDataItem(name, type, value, source, nRef, 0);
                pSortedList->Add(item);

                name = GetLocal(&luaDebug, ++idx);
            }
        }
    }

    return pSortedList;
}

wxLuaDebugData *wxLuaInterface::BuildTableList(int nRef, int nIndex)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL && GetRef(nRef) != 0)
    {
        int nTop = GetTop();

        // start iterating
        PushNil();
        while (Next(nTop) != 0)
        {
            bool        fIsTable = false;
            wxString type;
            wxString value;
            wxString name;
            wxString source;

            // get the index
            switch (Type(-2))
            {
                case LUA_TNIL:
                    name = wxT("Nil");
                    break;

                case LUA_TBOOLEAN:
                    name = MakeBoolean(ToBoolean(-2));
                    break;

                case LUA_TNUMBER:
                    name = MakeNumber(ToNumber(-2));
                    break;

                case LUA_TSTRING:
                    name = ToString(-2);
                    break;

                case LUA_TTABLE:
                    name = GetTableInfo(-2);
                    break;

                case LUA_TFUNCTION:
                    if (IsCFunction(-2))
                        name.Printf(wxT("%p"), ToCFunction(-2));
                    else
                        name.Printf(wxT("%p"), ToPointer(-2));
                    break;

                case LUA_TUSERDATA:
                    name = GetUserDataInfo(-2);
                    break;

                case LUA_TLIGHTUSERDATA:
                    name = GetUserDataInfo(-2, false);
                    break;
            }

            // get the value
            switch (Type(-1))
            {
                case LUA_TNIL:
                    type  = wxT("Nil");
                    value = wxT("nil");
                    break;

                case LUA_TBOOLEAN:
                    value = MakeBoolean(ToBoolean(-1));
                    type = wxT("Boolean");
                    break;

                case LUA_TNUMBER:
                    value = MakeNumber(ToNumber(-1));
                    type = wxT("Number");
                    break;

                case LUA_TSTRING:
                    type  = wxT("String");
                    value = ToString(-1);
                    break;

                case LUA_TTABLE:
                    type  = wxT("Table");
                    value = GetTableInfo(-1);
                    fIsTable = true;
                    break;

                case LUA_TFUNCTION:
                    if (IsCFunction(-1))
                    {
                        type  = wxT("C Function");
                        value.Printf(wxT("%p"), ToCFunction(-1));
                    }
                    else
                    {
                        type = wxT("Lua Function");
                        value.Printf(wxT("%p"), ToPointer(-1));
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
                    value.Printf(wxT("%p"), ToPointer(-1));
                    break;
            }

            int nRef = LUA_NOREF;
            if (fIsTable)
                nRef = Ref();
            else
                Pop(1);

            wxLuaDebugDataItem *item = new wxLuaDebugDataItem(name, type, value, source, nRef, nIndex);
            pSortedList->Add(item);
        }

        // remove reference
        Pop(1);
    }
    return pSortedList;
}

wxString wxLuaInterface::GetTableInfo(int index)
{
    wxString tableInfo;

    int         nItems = GetN(index);
    const void *pItem  = ToPointer(index);

    if (nItems == 0)
        tableInfo.Printf(wxT("%p"), pItem);
    else
        tableInfo.Printf(wxT("%p approx %u items)"), pItem, nItems);

    return tableInfo;
}

wxString wxLuaInterface::GetUserDataInfo(int index, bool full)
{
    wxString userdatainfo;

    if (full)
    {
        wxString pTagName;
        int nTag = GetTag(index);

        if (nTag != TLUA_NOTAG)
            pTagName = lua2wx(GetLuaTagName(m_luaState, nTag));

        if (pTagName != wxEmptyString)
            userdatainfo.Printf(wxT("%p (%s)"), ToUserData(index), pTagName.c_str());
        else
            userdatainfo.Printf(wxT("%p (%u)"), ToUserData(index), nTag);
    }
    else
        userdatainfo.Printf(wxT("%p"), ToUserData(index));

    return userdatainfo;
}

wxString wxLuaInterface::MakeNumber(double dnum)
{
    wxString number;

    long num = (long) dnum;
    if ((double) num == dnum)
    {
        if (num >= 0)
            number.Printf(wxT("%lu (0x%lx)"), num, num);
        else
            number.Printf(wxT("%ld (0x%lx)"), num, num);
    }
    else
        number.Printf(wxT("%g"), dnum);

    return number;
}

wxString wxLuaInterface::MakeBoolean(int num)
{
    if (num)
        return wxT("True");
    return wxT("False");
}

wxLuaDebugData *wxLuaInterface::GetGlobalData()
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();
    if (pSortedList != NULL)
    {
        GetGlobals();
        int nRef = Ref();
        pSortedList->Add(new wxLuaDebugDataItem(wxT("Globals"), wxT(""), wxT(""), wxT(""), nRef, 0));
    }
    return pSortedList;
}
