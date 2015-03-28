/////////////////////////////////////////////////////////////////////////////
// Purpose:       Stuff to setup wxLua in a lua_State
// Author:        J Winwood
// Created:       14/11/2001
// Modifications: Type checking added when accessing user data items
//                Support for wxWidgets 2.3
//                Thanks to Francis Irving for separating out into
//                Library and Standalone modules.
//                The number of non-wxObject class encapsulations was getting
//                out of hand! Reimplemented using preprocessor macros.
//                Added code to reset the metatable of objects when they are
//                deleted (in the C++ sense) so that method calls can not be
//                made on deleted objects. Applies to wxWindow-derived objects
//                only.
// Copyright:     (c) 2001-2003 Lomtick Software. All rights reserved.
// Licence:       wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaInternals.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/app.h"
#endif

#include "wx/datetime.h"

#include "../Import/luasetup.h.in"   // get the base library setup parameters
#include "wxLuaInternals.h"
#include "wxLuaCallback.h"

class wxStyledTextCtrlDocument;   // for void* wxStyledTextCtrl::GetDocPointer()

#ifdef FindText
# undef FindText
#endif

// always defined in wxLuaWrap.cpp
extern int s_wxIcon;
extern int s_wxWindow;
extern int s_wxEvtHandler;
extern int s_wxEvent;
extern int s_wxBitmap;

#if defined(__WXGTK__) || defined(__WXMAC__) || defined(__WXMOTIF__) 
    #include "../wxLua.xpm"
#endif

#include "wxLuaInterpreter.h"
#include "wxLuaHtmlWindow.h"
#include "wxLuaLibrary.h"

static int LUACALL LuaTableErrorHandler(lua_State *L)
{
    terror(L, "Cannot modify read-only wxLua table");
    return 0;
}

static const char wxLuaReferences[] = "wxLuaReferences";
static const char wxNull[]          = "wxNull";

// ----------------------------------------------------------------------------
// wxLuaStateVariables
// ----------------------------------------------------------------------------

wxLuaStateVariables::wxLuaStateVariables()
{
    m_typesRegistered = FALSE;
    m_startTag        = 0;
    m_lastTag         = 0;
    m_functionTag     = 0;
    m_wxLuaTable      = 0;
    m_wxNull          = 0;
    m_wxDeleteTable   = 0;
    m_inEventType     = -1;
    m_pDerivedList    = new wxHashTable(wxKEY_INTEGER);
    m_pTrackedList    = new wxHashTable(wxKEY_INTEGER);
    m_pAppHandlerList = new wxList;
    m_pDestroyHandlerList = new wxList;
    m_pWindowList     = new wxList;
}

wxLuaStateVariables::~wxLuaStateVariables()
{
    ClearCallbacks();
    delete m_pDerivedList;
    delete m_pTrackedList;
    delete m_pAppHandlerList;
    delete m_pDestroyHandlerList;
    delete m_pWindowList;
}

void wxLuaStateVariables::ClearCallbacks()
{
    // remove any and all callbacks that use this event handler since its gone
    wxNode* node = m_pAppHandlerList->GetFirst();
    while (node)
    {
        wxLuaCallback *pCallback = (wxLuaCallback *) node->GetData();

        if (pCallback)
        {
            pCallback->ClearState();
            pCallback->ClearStateVars();
        }

        node = node->GetNext();
    }
    node = m_pDestroyHandlerList->GetFirst();
    while (node)
    {
        wxLuaDestroyCallback *pCallback = (wxLuaDestroyCallback *) node->GetData();

        if (pCallback)
        {
            pCallback->ClearState();
            pCallback->ClearStateVars();
        }

        node = node->GetNext();
    }
    
    // don't "own" pointers to wxLuaCallbacks, let wxEventHandler do it
    m_pAppHandlerList->DeleteContents(false);
    // don't delete wxLuaDestroyCallbacks, let wxEventHandler do it
    m_pDestroyHandlerList->DeleteContents(false);
}


static int lua_wxRun(lua_State* L)
{
    int returns;
    returns = wxTheApp->MainLoop();
    lua_pushnumber(L, returns);
    return 1;
}


// Register (and re-register if required) all the types, classes and objects
// contained within the generated binding
int LUACALL RegisterWxLuaFunctions(lua_State *L, bool registerTypes)
{
    GET_LUASTATEVARS_MSG(L, 0)

    if (!registerTypes && !stateVars->m_typesRegistered)
        terror(L, "wxLua: First time registration must register types");

    // create references table in registry
    tpushliteralstring(L, wxLuaReferences);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);

    // create a tag for the wxLua table
    if (registerTypes)
        stateVars->m_wxLuaTable = tnewtag(L);

    // create the wxLua table
    lua_newtable(L);
    int tableOffset = lua_gettop(L);

    // set the table tag
    tsettag (L, stateVars->m_wxLuaTable);
    tsettagmethod (L, stateVars->m_wxLuaTable, "__newindex", LuaTableErrorHandler);

    // register all out classes etc. in the wxLua table
    registerGeneratedClasses(L, tableOffset, registerTypes);

    // register wxNull
    if (registerTypes)
        stateVars->m_wxNull = tnewtag(L);

    tpushliteralstring(L, wxNull);
    tpushusertag(L, NULL, stateVars->m_wxNull);
    lua_rawset(L, tableOffset);

    registerOthers(L, registerTypes);

    lua_pushstring(L, "Run");
    lua_pushcclosure(L, lua_wxRun, 0);
    lua_rawset(L, tableOffset);

    stateVars->m_typesRegistered = true;

    return tableOffset;
}

void LUACALL RegisterWxLua(lua_State *L, bool registerTypes)
{
    int tableOffset = RegisterWxLuaFunctions(L, registerTypes);

    // create a global called 'wx'
    lua_pushliteral(L, "wx");
    lua_pushvalue(L, tableOffset);
    lua_rawset(L, LUA_GLOBALSINDEX);
    lua_pop(L, 1);

    wxIcon *pIcon = new wxIcon(wxICON(LUA));
    addToTrackedMemoryList(L, pIcon);
    pushuserdatatype(L, s_wxIcon, pIcon);
    lua_setglobal(L, "wxLuaEditorIcon");
}


// Free up the tracked resources list.
void LUACALL CleanupWxLua(lua_State *L, bool closeLua /* = true*/)
{
    GET_LUASTATEVARS_RET(L)

    stateVars->m_pDerivedList->DeleteContents(true);
    stateVars->m_pTrackedList->DeleteContents(true);

    stateVars->ClearCallbacks();

    if (closeLua)
    {
        tpushliteralstring(L, wxLuaReferences);
        lua_pushnil(L);
        lua_rawset(L, LUA_REGISTRYINDEX);

//        lua_close(L);
    }

    // don't "own" pointers to wxLuaCallbacks, let wxEventHandler do it
    stateVars->m_pAppHandlerList->DeleteContents(false);
    // don't delete wxLuaDestroyCallbacks, let wxEventHandler do it
    stateVars->m_pDestroyHandlerList->DeleteContents(false);
    // don't delete wxWindow derived classes, let wxWidgets do it
    stateVars->m_pWindowList->DeleteContents(false);
}

// ----------------------------------------------------------------------------
// txxx functions to make lua calls easier
// ----------------------------------------------------------------------------

int LUACALL tinsert(lua_State *L, int iParam)
{
    if (lua_isnil(L, iParam))
    {
        lua_pop(L, 1);
        return LUA_REFNIL;
    }

    int iCount;

    lua_pushvalue(L, iParam);                // get value to store
    int nTop = lua_gettop(L);

    tpushliteralstring(L, wxLuaReferences);
    lua_rawget(L, LUA_REGISTRYINDEX);        // pop key, push result

    lua_pushliteral(L, "n");                 // push 'n' as key
    lua_rawget(L, -2);                       // pop key, push result
    if (lua_isnil(L, -1))                    // not used before?
        iCount = 0;                          //
    else
        iCount = (int) lua_tonumber(L, -1);  // get result value
    lua_pop(L, 1);                           // pop result

    iCount++;                                // next unallocated index

    lua_pushvalue(L, nTop);                  // push value to store
    lua_rawseti(L, -2, iCount);              // store value, pop value

    lua_pushliteral(L, "n");                 // push key
    lua_pushnumber(L, iCount);               // push value
    lua_rawset(L, -3);                       // pop key, value

    lua_pop(L, 3);                           // restore stack

    return iCount;
}

bool LUACALL tget(lua_State *L, int iIndex)
{
    if (iIndex == LUA_REFNIL)
    {
        lua_pushnil(L);
        return true;
    }

    int  iCount;
    bool ret = false;                        // false indicates index out of range

    tpushliteralstring(L, wxLuaReferences);
    lua_rawget(L, LUA_REGISTRYINDEX);        // pop key, push result (refs table)

    lua_pushliteral(L, "n");                 // push 'n' as key
    lua_rawget(L, -2);                       // pop key, push result
    iCount = (int) lua_tonumber(L, -1);      // get result value
    lua_pop(L, 1);                           // pop result

    if (iIndex > 0 && iIndex <= iCount)      // ensure in range
    {
        lua_rawgeti(L, -1, iIndex);          // push result
        ret = true;
    }
    else
        lua_pushnil(L);                      // push result

    lua_remove(L, -2);                       // balance stack

    return ret;
}

bool LUACALL tremove(lua_State *L, int iIndex)
{
    if (iIndex == LUA_REFNIL)
        return true;

    int  iCount;
    bool ret = false;                        // false indicates index out of range

    tpushliteralstring(L, wxLuaReferences);
    lua_rawget(L, LUA_REGISTRYINDEX);        // pop key, push result (refs table)

    lua_pushliteral(L, "n");                 // push 'n' as key
    lua_rawget(L, -2);                       // pop key, push result
    iCount = (int) lua_tonumber(L, -1);      // get result value
    lua_pop(L, 1);                           // pop result

    if (iIndex > 0 && iIndex <= iCount)      // ensure in range
    {
        lua_pushnil(L);                      // push nil as value
        lua_rawseti(L, -2, iIndex);          // set table, pop value
        ret = true;
    }
    lua_pop(L, 1);                           // clean up stack
    return ret;
}

int LUACALL tgetn (lua_State *L)
{
    tpushliteralstring(L, wxLuaReferences);
    lua_rawget(L, LUA_REGISTRYINDEX);        // pop key, push result (refs table)

    lua_pushliteral(L, "n");
    lua_rawget(L, -2);

    int n;
    if (lua_isnumber(L, -1))
    {
        n = (int) lua_tonumber(L, -1);
    }
    else
    {
        /* must count elements */
        n = 0;
        for (;;)
        {
            lua_rawgeti(L, -2, ++n);
            bool isNil = lua_isnil(L, -1);
            lua_pop(L, 1);
            if (isNil)
                break;
        }
        --n;
    }
    lua_pop(L, 2);
    return n;
}

void LUACALL terror (lua_State *L, const char *errorMsg)
{
    lua_pushstring(L, errorMsg);
    lua_error(L);
}

void LUACALL tpushusertag(lua_State *L, const void *u, int tag)
{
    const void **ptr = (const void **) lua_newuserdata(L, sizeof(void *));
    if (ptr != NULL)
    {
        *ptr = u;
        if (tag != TLUA_NOTAG && tget(L, tag))
        {
            if (lua_setmetatable(L, -2) == 0)
                terror(L, "wxLua: Unable to set metatable");
        }
    }
    else
        terror(L, "wxLua: Out of memory");
}

void LUACALL tsettag(lua_State *L, int tag)
{
    if (tag != TLUA_NOTAG && tget(L, tag))
    {
        if (!lua_setmetatable(L, -2))
            terror(L, "wxLua: Unable to set metatable");
    }
}

int LUACALL ttag (lua_State *L, int index)
{
    int tag = TLUA_NOTAG;
    if (lua_getmetatable(L, index) != 0)
    {
        lua_pushliteral(L, "tag");
        lua_rawget(L, -2);
        if (lua_isnumber(L, -1))
        {
            tag = (int) lua_tonumber(L, -1);
        }
        lua_pop(L, 2);
    }
    return tag;
}

void * LUACALL ttouserdata (lua_State *L, int index, bool reset /* = false*/)
{
    void *pdata = NULL;
    void **ptr = (void **) lua_touserdata(L, index);
    if (ptr != NULL)
    {
        pdata = *ptr;
        if (reset)
            *ptr = NULL;
    }
    return pdata;
}

int LUACALL tnewtag(lua_State *L)
{
    lua_newtable(L);
    lua_pushvalue(L, -1);
    int tag = tinsert(L, -1);
    lua_pushliteral(L, "tag");
    lua_pushnumber(L, tag);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    return tag;
}

int LUACALL tnewweaktag(lua_State *L, bool weakKey, bool weakData)
{
    lua_newtable(L);    // main table
    lua_newtable(L);    // metatable
    if (weakKey)
    {
        if (weakData)
            lua_pushliteral(L, "kv");
        else
            lua_pushliteral(L, "k");
    }
    else
    {
        if (weakData)
            lua_pushliteral(L, "v");
        else
            lua_pushnil(L);
    }
    lua_pushliteral(L, "__mode");
    lua_rawset(L, -3);  // set mode of main table
    lua_setmetatable(L, -2); // via the metatable
    lua_pushvalue(L, -1);
    int tag = tinsert(L, -1);
    lua_pushliteral(L, "tag");
    lua_pushnumber(L, tag);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    return tag;
}

bool LUACALL tsettagmethod(lua_State *L, int tag, const char *method, lua_CFunction func, void *pClass /* = NULL*/)
{
    if (tag != TLUA_NOTAG && tget(L, tag))
    {
        if (pClass != NULL)
        {
            lua_pushstring(L, method);
            lua_pushlightuserdata(L,  pClass);
            lua_pushcclosure(L, func, 1);
        }
        else
        {
            lua_pushstring(L, method);
            lua_pushcclosure(L, func, 0);
        }
        lua_rawset(L, -3);
        lua_pop(L, 1);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// wxLuaObject
//-----------------------------------------------------------------------------

// The wxLuaObject class wraps a Lua object reference in
// a wxWidgets object derived class so that a Lua object
// can be used for user data, and also with a simple
// extension by the input/output mechanism of the
// wxValidator classes.

IMPLEMENT_DYNAMIC_CLASS(wxLuaObject, wxObject)

// Default constructor
wxLuaObject::wxLuaObject() : m_luaState(NULL), m_iReference(LUA_NOREF),
                             m_allocatedBool(FALSE), m_bool(FALSE),
                             m_allocatedInt(FALSE), m_int(0),
                             m_allocatedString(FALSE), m_allocatedArray(FALSE)
{
}

// Constructor that is passed a lua state and a parameter index.
wxLuaObject::wxLuaObject(lua_State *L, int iParam, int WXUNUSED(iRef)) 
            : m_luaState(L), m_allocatedBool(FALSE), m_bool(FALSE),
                             m_allocatedInt(FALSE), m_int(0),
                             m_allocatedString(FALSE), m_allocatedArray(FALSE)
{
    // set up the reference
    m_iReference = tinsert(L, iParam);
}

wxLuaObject::~wxLuaObject()
{
    // If a refererence exists, remove it
    if ((m_iReference != LUA_NOREF) && (m_luaState != NULL))
        tremove(m_luaState, m_iReference);
}

// Return a flag value that indicated whether the
// object is being used by a wxValidator class
// (else 0).
int  wxLuaObject::GetAllocationFlags() const
{
    int flags = 0;
    if (m_allocatedBool)
        flags = 1;
    if (m_allocatedInt)
        flags += 2;
    if (m_allocatedString)
        flags += 4;
    if (m_allocatedArray)
        flags += 8;
    return flags;
}

// Allow the flag value to be manipulated.
void wxLuaObject::ModifyAllocationFlags(int iValue)
{
    if (iValue & 1)
        m_allocatedBool = false;
    if (iValue & 2)
        m_allocatedInt  = false;
    if (iValue & 4)
        m_allocatedString = false;
    if (iValue & 8)
        m_allocatedArray = false;

    if (iValue & 16)
        m_allocatedBool = true;
    if (iValue & 32)
        m_allocatedInt = true;
    if (iValue & 64)
        m_allocatedString = true;
    if (iValue & 128)
        m_allocatedArray = true;
}

// Get the value of the reference object (or
// a proxy if the object has been aliased for a
// wxValidator class.
bool wxLuaObject::GetObject()
{
    if (m_allocatedBool)
    {
        lua_pushnumber(m_luaState, m_bool);
        return true;
    }
    else
    if (m_allocatedInt)
    {
        lua_pushnumber(m_luaState, m_int);
        return true;
    }
    else
    if (m_allocatedString)
    {
        lua_pushstring(m_luaState, wx2lua(m_string));
        return true;
    }
    else
    if (m_allocatedArray)
    {
        lua_newtable(m_luaState);

        size_t idx;
        for (idx = 0; idx < m_arrayInt.Count(); ++idx)
        {
            lua_pushnumber(m_luaState, m_arrayInt.Item(idx));
            lua_rawseti(m_luaState, -2, idx);
        }
        return true;
    }
    return (m_iReference != LUA_NOREF &&
            tget(m_luaState, m_iReference) != 0);
}

// Set the reference and lua state up from a supplied parameter.
void wxLuaObject::SetObject(lua_State *L, int iParam)
{
    m_luaState = L;
    SetObject(iParam);
}

// Remove any existing reference and allocated another
void wxLuaObject::SetObject(int iParam)
{
    if (m_iReference != LUA_NOREF)
        tremove(m_luaState, m_iReference);

    m_iReference = LUA_NOREF;

    if (m_luaState != NULL)
    {
        m_iReference = tinsert(m_luaState, iParam);
    }
}

// The following methods are used by the wxValidator interface
bool *wxLuaObject::GetBoolPtr()
{
    if ((m_luaState != NULL) && (m_iReference != LUA_NOREF) && GetObject())
    {
        m_bool          = (lua_tonumber(m_luaState, -1) != 0);
        m_allocatedBool = true;
    }
    return &m_bool;
}

int *wxLuaObject::GetIntPtr()
{
    if ((m_luaState != NULL) && (m_iReference != LUA_NOREF) && GetObject())
    {
        m_int          = (int) lua_tonumber(m_luaState, -1);
        m_allocatedInt = true;
    }
    return &m_int;
}

wxString *wxLuaObject::GetStringPtr()
{
    if ((m_luaState != NULL) && (m_iReference != LUA_NOREF) && GetObject())
    {
        m_string          = lua2wx(lua_tostring(m_luaState, -1));
        m_allocatedString = true;
    }
    return &m_string;
}

wxArrayInt *wxLuaObject::GetArrayPtr()
{
    if ((m_luaState != NULL) && (m_iReference != LUA_NOREF) && GetObject())
    {
        if (lua_istable(m_luaState, -1))
        {
            int idx, nItems = luaL_getn(m_luaState, -1);
            if (nItems > 0)
                for (idx = 1; idx <= nItems; ++idx)
                {
                    lua_rawgeti(m_luaState, -1, idx);
                    int iValue = (int) getnumbertype(m_luaState, -1);
                    m_arrayInt.Add(iValue);
                    lua_pop(m_luaState, 1);
                }
        }
        lua_pop(m_luaState, 1);
        m_allocatedArray = true;
    }
    return &m_arrayInt;
}

wxLuaCheckStack::wxLuaCheckStack(lua_State *lua_state, const wxString &msg)
{
    m_luastate = lua_state;
    m_msg      = msg;
    m_top      = lua_gettop(m_luastate);
}

wxLuaCheckStack::~wxLuaCheckStack()
{
    TestStack(m_msg);
}

void wxLuaCheckStack::TestStack(const wxString &msg)
{
    wxString message = msg;

    if (message.IsEmpty())
        message = m_msg;

#if  defined(__WXMSW__)
    wxString buf = wxString::Format(_("Stack state in %s: in %u out %u\r\n"), message.c_str(), m_top, lua_gettop(m_luastate));
    OutputDebugString(buf);
#elif defined(__WXGTK__) || defined(__WXMAC__)
    fprintf(stderr, "Stack state in %s: in %u out %u\r\n", (const char *)wx2lua(msg), m_top, lua_gettop(m_luastate));
#endif
}

void wxLuaCheckStack::DumpGlobals(lua_State *lua_state)
{
    lua_pushvalue(lua_state, LUA_GLOBALSINDEX);
    DumpTable(lua_state, lua_gettop(lua_state));
}

void wxLuaCheckStack::DumpTable(lua_State *lua_state, const wxString &name)
{
    lua_pushstring(lua_state, wx2lua(name));
    lua_gettable(lua_state, LUA_GLOBALSINDEX);
    DumpTable(lua_state, lua_gettop(lua_state));
}

wxString wxLuaCheckStack::GetTableInfo(lua_State *lua_state, int index)
{
    int         nTag   = ttag(lua_state, index);
    int         nItems = luaL_getn(lua_state, index);
    const void *pItem  = lua_topointer(lua_state, index);

    wxString tableInfo;

    if (nTag != TLUA_NOTAG)
    {
        if (nItems == 0)
            tableInfo = wxString::Format(_("%p (tag %u)"), pItem, nTag);
        else
            tableInfo = wxString::Format(_("%p (tag %u, approx. %u items)"), pItem, nTag, nItems);
    }
    else
    {
        if (nItems == 0)
            tableInfo = wxString::Format(_("%p (no tag)"), pItem);
        else
            tableInfo = wxString::Format(_("%p (no tag, approx %u items)"), pItem, nItems);
    }

    return tableInfo;
}

void wxLuaCheckStack::DumpType(lua_State *lua_state, const wxString &name, int index, wxString &outputbuffer)
{
    wxString type;
    wxString value;

    switch (lua_type(lua_state, index))
    {
        default:
            type = wxString::Format(wxT("%u"), lua_type(lua_state, index));
            value = wxT("?");
            break;

        case LUA_TNIL:
            type  = wxT("Nil");
            value = wxT("nil");
            break;

        case LUA_TNUMBER:
            type = wxT("Number");
            value = MakeNumber(lua_tonumber(lua_state, index));
            break;

        case LUA_TSTRING:
            type  = wxT("String");
            value = lua2wx(lua_tostring(lua_state, index));
            break;

        case LUA_TTABLE:
            type  = wxT("Table");
            value = GetTableInfo(lua_state, index);
            break;

        case LUA_TFUNCTION:
            if (lua_iscfunction(lua_state, index))
            {
                type  = wxT("C Function");
                value = wxString::Format(wxT("%p"), lua_tocfunction(lua_state, index));
            }
            else
            {
                type = wxT("Lua Function");
                value = wxString::Format(wxT("%p"), lua_topointer(lua_state, index));
            }

            break;

        case LUA_TUSERDATA:
            type  = wxT("User Data");
            value = GetUserDataInfo(lua_state, index);
            break;
    }

    outputbuffer = wxString::Format(_("%s - type: %s, value: %s\n"), name.c_str(), type.c_str(), value.c_str());
}

wxString wxLuaCheckStack::GetUserDataInfo(lua_State *lua_state, int index)
{
    wxString userdataInfo;
    wxString tagName;

    int nTag = ttag(lua_state, index);

    if (nTag != TLUA_NOTAG)
        tagName = lua2wx(GetLuaTagName(lua_state, nTag));

    if (!tagName.IsEmpty())
        userdataInfo = wxString::Format(wxT("%p (%s)"), lua_touserdata(lua_state, index), tagName.c_str());
    else
        userdataInfo = wxString::Format(wxT("%p (%u)"), lua_touserdata(lua_state, index), nTag);

    return userdataInfo;
}

wxString wxLuaCheckStack::MakeNumber(double dnum)
{
    wxString number;

    long num = (long) dnum;
    if ((double) num == dnum)
    {
        if (num >= 0)
            number = wxString::Format(wxT("%lu (0x%lx)"), num, num);
        else
            number = wxString::Format(wxT("%ld (0x%lx)"), num, num);
    }
    else
        number = wxString::Format(wxT("%g"), dnum);

    return number;
}

void wxLuaCheckStack::DumpTable(lua_State *lua_state, int index)
{
    lua_pushnil(lua_state);
    while(lua_next(lua_state, index) != 0)
    {
        wxString outputIndex;
        wxString outputValue;

        DumpType(lua_state, wxT("index"), -2, outputIndex);
        DumpType(lua_state, wxT("value"), -1, outputValue);

#if  defined(__WXMSW__)
        OutputDebugString(outputIndex + outputValue);
#elif defined(__WXGTK__) || defined(__WXMAC__)
        fprintf(stderr, wx2lua(outputIndex + outputValue));
#endif
        lua_pop(lua_state, 1);
    }
    lua_pop(lua_state, 1);
}

// The following functions support memory allocated for objects in
// the Lua system.

bool LUACALL removeTrackedMemory(lua_State *L, void *pObject, bool fDelete)
{
    bool result = false;
    GET_LUASTATEVARS_MSG(L, FALSE)

    wxObject *pDeleteObject = stateVars->m_pTrackedList->Delete((int) pObject);
    if (fDelete && pDeleteObject != NULL)
    {
        delete pDeleteObject;
        result = true;
    }
    return result;
}

void LUACALL addToTrackedMemoryList(lua_State *L, wxObject *pObject)
{
    wxCHECK_RET(L && pObject, wxT("Invalid lua state and/or wxObject"));
    GET_LUASTATEVARS_RET(L)
    stateVars->m_pTrackedList->Put((int)pObject, pObject);
}

bool IsParentWindowTracked(wxList *list, wxWindow* win)
{
    wxCHECK_MSG(list && win, FALSE, wxT("Invalid list or parent"));
    wxWindow *parent = win;
    
    while (parent)
    {
        if (list->Find(parent))
            return TRUE;
        
        parent = parent->GetParent();
    }
    
    return FALSE;
}

void LUACALL addToTrackedWindowList(lua_State *L, wxWindow *pWindow)
{
    wxCHECK_RET(L && pWindow, wxT("Invalid lua state and/or wxWindow"));

    // don't track these windows since they're supposed to be attached
    if (wxDynamicCast(pWindow, wxMenuBar) != NULL) return;
    if (wxDynamicCast(pWindow, wxToolBar) != NULL) return;

    GET_LUASTATEVARS_RET(L)
    
    // only need to track parent window, it deletes children for us
    if (!IsParentWindowTracked(stateVars->m_pWindowList, pWindow))
    {
        stateVars->m_pWindowList->Append(pWindow);
        new wxLuaCallback(L, 3, pWindow->GetId(), wxEVT_DESTROY, pWindow->GetEventHandler());
    }
}

// Pointers and counts to the information generated by the binder
// These are initialized in registerGeneratedClasses()
static size_t       classCount      = 0;
static WXLUACLASS  *pClassList      = NULL;
static size_t       defineCount     = 0;
static WXLUADEFINE *pDefineList     = NULL;
static size_t       stringCount     = 0;
static WXLUASTRING *pStringList     = NULL;
static size_t       eventCount      = 0;
static WXLUAEVENT  *pEventList      = NULL;
static size_t       objectCount     = 0;
static WXLUAOBJECT *pObjectList     = NULL;

// Function to compare the class tag of two classes
static int classListCompareFn(const void *p1, const void *p2)
{
    return (*((const WXLUACLASS *) p1)->class_tag) - (*((const WXLUACLASS *) p2)->class_tag);
}

// Find the WXLUACLASS that corresponds to a given tag.
// If found see if the required tag is derived from the
// tag of the class found.
static bool LUACALL IsDerivedTag(int iTag, int iParamTag)
{
    WXLUACLASS classItem;
    classItem.class_tag = &iTag;
    // this relies on LUA allocating tags in ascending order of definition
    // if LUA stops doing this, then the search may break.
    const WXLUACLASS *pLuaClass = (WXLUACLASS *) bsearch(&classItem,
                                                         pClassList,
                                                         classCount,
                                                         sizeof(WXLUACLASS),
                                                         classListCompareFn);

    if (pLuaClass != NULL)
    {
        while (pLuaClass->baseclassindex != -1)
        {
            pLuaClass = pClassList + pLuaClass->baseclassindex;
            if (*pLuaClass->class_tag == iParamTag)
                return true;
        }
    }
    return false;
}

// Return a pointer to the object that corresponds to the specified stack
// index and tag
void * LUACALL getuserdatatype(lua_State *L, int iParam, int iParamTag)
{
    GET_LUASTATEVARS_MSG(L, NULL)

    int   iTag;
    void *pReturn = NULL;
    if (!lua_isuserdata(L, iParam))
        goto error;

    if (lua_islightuserdata(L, iParam))
        goto error;

    iTag = ttag(L, iParam);
    if (stateVars->m_functionTag == iTag)
    {
        wxLuaFunction *pFunction = (wxLuaFunction *) ttouserdata(L, iParam);
        pReturn = pFunction->GetObject();
    }
    else
    if ((stateVars->m_startTag <= iTag) && (stateVars->m_lastTag  >= iTag) &&
        ((iTag == iParamTag) || IsDerivedTag(iTag, iParamTag)))
            pReturn = ttouserdata(L, iParam);
    else
    if (stateVars->m_wxNull != iTag)
        goto error;

    return pReturn;

error:
    terror(L, wx2lua(wxString::Format(_("wxLua: Unexpected user defined data type for parameter %d."), iParam)));
    return NULL;
}

// ----------------------------------------------------------------------------
// wxLuaDestroyCallback
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxLuaDestroyCallback, wxObject);

wxLuaDestroyCallback::wxLuaDestroyCallback(lua_State *L, int id, wxEvtHandler *pHandler, int iTag) 
                     : m_luaState(L), m_luaStateVars(NULL), m_pHandler(pHandler), m_id(id)
{
    GET_LUASTATEVARS_RET(L)
    m_luaStateVars = stateVars;

    // allocate a LUA proxy for the object
    const void **ptr = (const void **) lua_newuserdata(L, sizeof(void *));
    if (ptr != NULL)
    {
        // save the address of the object in the proxy
        *ptr = pHandler;

        // and set the metatable of the proxy
        if (iTag != TLUA_NOTAG && tget(L, iTag))
        {
            if (lua_setmetatable(L, -2) == 0)
            {
                terror(L, "wxLua: Unable to set metatable");
            }
        }

        // get a reference to the destroy handler table
        if (tget(L, stateVars->m_wxDeleteTable))
        {
            // create a reference to object
            lua_pushlightuserdata(L, pHandler); // key
            lua_pushvalue(L, -3);               // value
            // save it in the destroy handler table
            lua_rawset(L, -3);
            lua_pop(L, 1);
        }

        stateVars->m_pDestroyHandlerList->Append(this);

        // connect the event handler
        pHandler->Connect(id,
                          wxEVT_DESTROY,
                          (wxObjectEventFunction) &wxLuaDestroyCallback::EventHandler,
                          this);
    }
    else
    {
        terror(L, "wxLua: Out of memory");
    }
}

void wxLuaDestroyCallback::EventHandler(wxEvent& event)
{
    wxLuaDestroyCallback *theCallback = (wxLuaDestroyCallback *) event.m_callbackUserData;
    if (theCallback && (((wxWindow*)event.GetEventObject())->GetEventHandler() == m_pHandler))
    {
        theCallback->OnDestroy();
    }

    event.Skip();
}

void wxLuaDestroyCallback::OnDestroy()
{
    // FIXME - Is it an error to receive an event after you've deleted lua?
    //  probably not if lua is getting shutdown
    if (!m_luaState || !m_luaStateVars)
        return;
    
    if (tget(m_luaState, m_luaStateVars->m_wxDeleteTable))
    {
        // clear the metatable reference in the lua proxy.
        lua_pushlightuserdata(m_luaState, m_pHandler);
        lua_rawget(m_luaState, -2);
        lua_pushnil(m_luaState);
        lua_setmetatable(m_luaState, -2);
        lua_pop(m_luaState, 2);
    }
}

wxLuaDestroyCallback::~wxLuaDestroyCallback()
{
    if (m_luaStateVars)
        m_luaStateVars->m_pDestroyHandlerList->DeleteObject(this);
}

// Push a data type onto the stack and set its tag
void LUACALL pushuserdatatype(lua_State *L, int iTag, const void *data)
{
    if (data != NULL)
    {
        // if the object we are referencing is derived from wxWindow
        if (IsDerivedTag(iTag, s_wxWindow))
        {
            wxWindow *win = wxDynamicCast(data, wxWindow);
            // Connect an object to the destroy event.
            wxLuaDestroyCallback *pCallback = new wxLuaDestroyCallback(L,
                                                                       win->GetId(),
                                                                       (wxEvtHandler *) win,
                                                                       iTag);
            if (pCallback == NULL)
            {
                terror(L, "wxLua: Out of memory");
            }
        }
        else
        {
            // Otherwise handle normally
            tpushusertag(L, data, iTag);
        }
    }
    else
    {
        lua_pushnil(L);
    }
}

// Validate that the object at the stack index specified is a string object
// or that the object can be converted to a string.
// Return the string.
const char * LUACALL getstringtype(lua_State *L, int iParam)
{
    switch(lua_type(L, iParam))
    {
        case LUA_TNIL:
        case LUA_TSTRING:
        case LUA_TNUMBER:
            break;
        default:
            terror(L, wx2lua(wxString::Format(_("wxLua: Expected string for parameter %d."), iParam)));
            break;
    }

    return lua_tostring(L, iParam);
}

// Validate that the object at the stack index specified is a boolean object
// or that the object can be converted to a boolean.
// Return the boolean.
bool LUACALL getbooleantype(lua_State *L, int iParam)
{
    switch(lua_type(L, iParam))
    {
        case LUA_TNIL:
        case LUA_TNUMBER:
        case LUA_TBOOLEAN:
            /*OK*/
            break;
        default:
            terror(L, wx2lua(wxString::Format(_("wxLua: Expected boolean for parameter %d."), iParam)));
            break;
    }

    int num = (int) lua_toboolean(L, iParam);

//  This test is not reliable FALSE == 0 but TRUE !=0 not TRUE == 1
//  The paramter can be the result of evaluating an expression as well as one of the two
//  boolean constants.
//  if (!(num == TRUE) || !(num == FALSE))
//      terror(L, wxString::Format("wxLua: Expected 'TRUE' or 'FALSE' for parameter %d.", iParam));

    return num ? true : false;
}

// Validate that the object at the stack index specified is a enumeration object
// or that the object can be converted to a enumeration.
// Return the enumeration value.
long LUACALL getenumerationtype(lua_State *L, int iParam)
{
    switch(lua_type(L, iParam))
    {
        case LUA_TNIL:
        case LUA_TSTRING:
        case LUA_TNUMBER:
            /*OK*/
            break;
        default:
            terror(L, wx2lua(wxString::Format(_("wxLua: Expected enum number for parameter %d."), iParam)));
            break;
    }

    return (long) lua_tonumber(L, iParam);
}

// Validate that the object at the stack index specified is a enumeration object
// or that the object can be converted to a enumeration.
// Return the enumeration value.
double LUACALL getnumbertype(lua_State *L, int iParam)
{
    switch(lua_type(L, iParam))
    {
        case LUA_TNIL:
        case LUA_TSTRING:
        case LUA_TNUMBER:
        case LUA_TBOOLEAN:
            /*OK*/
            break;
        default:
            terror(L, wx2lua(wxString::Format(_("wxLua: Expected number for parameter %d."), iParam)));
            break;
    }

    return lua_tonumber(L, iParam);
}

// Validate that the object at the stack index specified is a table object
// Convert the table to an array of wxStrings. This assumes that each
// numeric entry in the table is a string object or can be converted to a string
// Return a pointer to the array of wxStrings.
wxString * LUACALL tostringarray(lua_State *L, int iParam, int &count)
{
    int       idx;
    wxString *pItems = NULL;

    count = 0;

    if (lua_istable(L, iParam))
    {
        int nItems = luaL_getn(L, iParam);
        if (nItems > 0)
            pItems = new wxString[nItems];
        if (pItems != NULL)
        {
            for (idx = 1; idx <= nItems; ++idx)
            {
                lua_rawgeti(L, iParam, idx);
                const char *pString = getstringtype(L, -1);
                if (pString != NULL)
                    pItems[count++] = lua2wx(pString);
                lua_pop(L, 1);
            }
        }
    }
    return pItems;
}

// Convert a wxArrayString object to an table of strings
int LUACALL tostringtable(lua_State *L, wxArrayString &files)
{
    lua_newtable(L);

    size_t idx;
    for (idx = 0; idx < files.Count(); ++idx)
    {
        const wxString &item = files.Item(idx);
        lua_pushstring(L, wx2lua(item));
        lua_rawseti(L, -2, idx + 1);
    }
    return idx;
}

int LUACALL tointtable(lua_State *L, const wxArrayInt &table)
{
    lua_newtable(L);

    size_t idx;
    for (idx = 0; idx < table.Count(); ++idx)
    {
        int item = table.Item(idx);
        lua_pushnumber(L, item);
        lua_rawseti(L, -2, idx + 1);
    }
    return idx;
}

// Validate that the object at the stack index specified is a table object
// Convert the table to an array of ints. This assumes that each
// numeric entry in the table is a numeric object or can be converted to a number
// Return a pointer to the array of ints.
int * LUACALL tointarray(lua_State *L, int iParam, int &count)
{
    int  idx;
    int *pItems = NULL;

    count = 0;

    if (lua_istable(L, iParam))
    {
        int nItems = luaL_getn(L, iParam);
        if (nItems > 0)
            pItems = new int[nItems];
        if (pItems != NULL)
        {
            for (idx = 1; idx <= nItems; ++idx)
            {
                lua_rawgeti(L, iParam, idx);
                int iValue = (int) getnumbertype(L, -1);
                pItems[count++] = iValue;
                lua_pop(L, 1);
            }
        }
    }
    return pItems;
}

int LUACALL toarrayint(lua_State *L, int iParam, wxArrayInt &intArray)
{
    int  idx, count = 0;
    intArray.Clear();

    if (lua_istable(L, iParam))
    {
        int nItems = luaL_getn(L, iParam);
        if (nItems > 0)
        {
            for (idx = 1; idx <= nItems; ++idx)
            {
                lua_rawgeti(L, iParam, idx);
                if (lua_isnumber(L, -1))
                {
                    int iValue = (int) getnumbertype(L, -1);
                    intArray.Add(iValue);
                    ++count;
                }
                lua_pop(L, 1);
            }
        }
    }
    return count;
}

const char ** LUACALL tochararray(lua_State *L, int iParam, int &count)
{
    int          idx;
    const char **pItems = NULL;

    count = 0;

    if (lua_istable(L, iParam))
    {
        int nItems = luaL_getn(L, iParam);
        if (nItems > 0)
            pItems = new const char *[nItems];
        if (pItems != NULL)
        {
            for (idx = 1; idx <= nItems; ++idx)
            {
                lua_rawgeti(L, iParam, idx);
                const char *pValue = getstringtype(L, -1);
                pItems[count++] = pValue;
                lua_pop(L, 1);
            }
        }
    }
    return pItems;
}

// Lookup a lua method or get property called cpIndex in the WXLUACLASS array pClass
// Ensure that the lookup is of the required type. If the lua method
// can not be found on the current class recurse through base classes
// Return a pointer to the WXLUAMETHOD that corresponds to the method name,
// else return NULL;
// Set isProperty to true if a property is found else it is a method.
WXLUAMETHOD * LUACALL getLuaMethod(WXLUACLASS *pClass, const char *cpIndex, bool &isProperty)
{
    int iMethod;
    // find a method in the class, recurse through classes from which this class is derived.
    for (iMethod = 0; iMethod < pClass->num_methods; ++iMethod)
    {
        WXLUAMETHOD *pMethod = pClass->methods + iMethod;
        if (((pMethod->type == LuaMethod) || (pMethod->type == LuaGetProp))
                && (strcmp(pMethod->name, cpIndex) == 0))
        {
            isProperty = (pMethod->type == LuaGetProp);
            return pMethod;
        }
    }

    if (pClass->baseclassindex != -1)
        return getLuaMethod(pClassList + pClass->baseclassindex, cpIndex, isProperty);

    return NULL;
}

// Lookup a lua property called cpIndex in the WXLUACLASS array pClass
// Ensure that the lookup is of the required type. If the lua property
// can not be found on the current class recurse through base classes
// Return a pointer to the WXLUAMETHOD that corresponds to the property name,
// else return NULL;
// Find 'Set' properties if isSet is true else 'Get' properties
WXLUAMETHOD * LUACALL getLuaProperty(WXLUACLASS *pClass, const char *cpIndex, bool isSet)
{
    int iMethod;
    // find a method in the class, recurse through classes from which this class is derived.
    for (iMethod = 0; iMethod < pClass->num_methods; ++iMethod)
    {
        WXLUAMETHOD *pMethod = pClass->methods + iMethod;
        if (isSet)
        {
            if ((pMethod->type == LuaSetProp) && (strcmp(pMethod->name, cpIndex) == 0))
                return pMethod;
        }
        else
        {
            if ((pMethod->type == LuaGetProp) && (strcmp(pMethod->name, cpIndex) == 0))
                return pMethod;
        }
    }

    if (pClass->baseclassindex != -1)
        return getLuaProperty(pClassList + pClass->baseclassindex, cpIndex, isSet);

    return NULL;
}

// Function to compare two events by eventType
static int eventListCompareFn(const void *p1, const void *p2)
{
    return (*((const WXLUAEVENT *) p1)->eventType) - (*((const WXLUAEVENT *) p2)->eventType);
}

// Register the classes, definitions, objects and pointers generated by the binding
// Sort the event list into order for faster event handler processing.
void LUACALL registerGeneratedClasses(lua_State *L, int tableOffset, bool registerTypes)
{
    GET_LUASTATEVARS_RET(L)

    static const luaL_reg funcTable[] =
    {
        {"__gc",       garbageCollect },
        {"__index",    getTableFunc   },
        {"__newindex", setTableFunc   }
    };
    const unsigned funcCount = sizeof(funcTable)/sizeof(funcTable[0]);

    unsigned iClass;
    if (registerTypes)
        stateVars->m_startTag = tnewtag(L);

    int iTag = stateVars->m_startTag;
    // install the classes, functions and methods
    pClassList = GetClassList(classCount);
    for (iClass = 0; iClass < classCount; ++iClass,
        iTag = registerTypes ? tnewtag(L) : iTag + 1)
    {
        WXLUACLASS *pClass = pClassList + iClass;

        *pClass->class_tag = iTag;

        for (unsigned iFunction = 0; iFunction < funcCount; iFunction++)
        {
            tsettagmethod (L, iTag, funcTable[iFunction].name, funcTable[iFunction].func, (void *) pClass);
        }

        for (int iMethod = 0; iMethod < pClass->num_methods; ++iMethod)
        {
            WXLUAMETHOD *pMethod = pClass->methods + iMethod;
            if ((pMethod->type == LuaConstructor) || (pMethod->type == LuaGlobal))
            {
                lua_pushstring(L, pMethod->name);
                lua_pushcfunction(L, pMethod->func);
                lua_rawset(L, tableOffset);
            }
        }
    }
    stateVars->m_lastTag = iTag;

    // for backward compatibility
    lua_pushliteral(L, "FALSE");
    lua_pushboolean(L, false);
    lua_rawset(L, tableOffset);

    lua_pushliteral(L, "TRUE");
    lua_pushboolean(L, true);
    lua_rawset(L, tableOffset);

    // install the number definitions
    pDefineList = GetDefineList(defineCount);
    for (unsigned iDefine = 0; iDefine < defineCount; ++iDefine)
    {
        WXLUADEFINE *pDefine = pDefineList + iDefine;
        lua_pushstring(L, pDefine->name);
        lua_pushnumber(L, pDefine->value);
        lua_rawset(L, tableOffset);
    }

    // install the string definitions
    pStringList = GetStringList(stringCount);
    for (unsigned iString = 0; iString < stringCount; ++iString)
    {
        WXLUASTRING *pString = pStringList + iString;
        lua_pushstring(L, pString->name);
        lua_pushstring(L, wx2lua((wxChar*)pString->strValue));
        lua_rawset(L, tableOffset);
    }

    // install the objects and pointers
    pObjectList = GetObjectList(objectCount);
    for (unsigned iObject = 0; iObject < objectCount; ++iObject)
    {
        WXLUAOBJECT *pObject = pObjectList + iObject;
        lua_pushstring(L, pObject->objName);
        if (pObject->objPtr != 0)
            tpushusertag(L, pObject->objPtr, *pObject->objClassTag);
        else
            tpushusertag(L, *pObject->pObjPtr, *pObject->objClassTag);
        lua_rawset(L, tableOffset);
    }

    // register all the wxWidgets event types
    pEventList = GetEventList(eventCount);
    for (unsigned iEvent = 0; iEvent < eventCount; ++iEvent)
    {
        WXLUAEVENT *pEvent = pEventList + iEvent;
        lua_pushstring(L, pEvent->eventName);
        lua_pushnumber(L, *pEvent->eventType);
        lua_rawset(L, tableOffset);
    }

    // sort the event list into order for faster lookup.
    qsort(pEventList,
          eventCount,
          sizeof(WXLUAEVENT),
          eventListCompareFn);
}

// Register any other objects that are not generated by the binding.
void LUACALL registerOthers(lua_State *L, bool registerTypes)
{
    GET_LUASTATEVARS_RET(L)

    // register our 'function' object handlers
    if (registerTypes)
    {
        stateVars->m_functionTag = tnewtag(L);
        stateVars->m_wxDeleteTable = tnewweaktag(L, false, true);
    }

    tsettagmethod (L, stateVars->m_functionTag, "__gc",    functionGarbageCollect);
    tsettagmethod (L, stateVars->m_functionTag, "__call",   callFunction);
}

// A function object needs to be deleted because the object is being
// garbage collected.
int LUACALL functionGarbageCollect(lua_State *L)
{
    GET_LUASTATEVARS_MSG(L, 0)

    if (lua_isuserdata(L, 1) && (lua_islightuserdata(L, 1) == 0) && (ttag(L, 1) == stateVars->m_functionTag))
    {
        wxLuaFunction *pFunction = (wxLuaFunction *)ttouserdata(L, 1, true);
        if (pFunction != NULL)
            delete pFunction;
    }
    return 0;
}

// if the class defines a gc function, then call it.
int LUACALL garbageCollect(lua_State *L)
{
    GET_LUASTATEVARS_MSG(L, 0)

    int retVal = 0;
    WXLUACLASS *pClass = (WXLUACLASS *)lua_touserdata(L, lua_upvalueindex(1));

    if ((pClass != NULL) && lua_isuserdata(L, 1) && (lua_islightuserdata(L, 1) == 0) && (ttag(L, 1) == *pClass->class_tag))
    {
        wxHashTable *pHashTable = (wxHashTable *) stateVars->m_pDerivedList->Delete((int) ttouserdata(L, 1, true));
        if (pHashTable != NULL)
            delete pHashTable;

        int iMethod;
        for (iMethod = 0; iMethod < pClass->num_methods; ++iMethod)
        {
            WXLUAMETHOD *pMethod = pClass->methods + iMethod;
            if (pMethod->type == LuaDelete)
            {
                retVal = (*pMethod->func)(L);
                break;
            }
        }
    }
    return retVal;
}

// Find a derived method given an object and and a method name.
// If the method can be found, push it on to the Lua stack and return 1
// otherwise return 0 and nothing is pushed.
lua_State * LUACALL getDerivedMethod(void *pObject, const char *method, lua_State *L)
{
    if (L)
    {
        wxLuaStateVariables *stateVars = wxFindLuaStateVariables(L);
        if (stateVars)
        {
            wxHashTable *pHashTable = (wxHashTable *) stateVars->m_pDerivedList->Get((int) pObject);
            if (pHashTable != NULL)
            {
                wxLuaObject *pLuaObject = (wxLuaObject *) pHashTable->Get(lua2wx(method));
                if ((pLuaObject != NULL) && pLuaObject->GetObject())
                    return pLuaObject->GetLuaState();
            }
        }
    }
    else
    {
        extern wxHashMapLuaInterpreter s_wxHashMapLuaInterpreter;
        wxHashMapLuaInterpreter::iterator it;
        for (it = s_wxHashMapLuaInterpreter.begin(); it != s_wxHashMapLuaInterpreter.end(); ++it)
        {
            wxLuaStateVariables *stateVars = it->second->GetStateVariables();
            if (stateVars)
            {
                wxHashTable *pHashTable = (wxHashTable *) stateVars->m_pDerivedList->Get((int) pObject);
                if (pHashTable != NULL)
                {
                    wxLuaObject *pLuaObject = (wxLuaObject *) pHashTable->Get(lua2wx(method));
                    if ((pLuaObject != NULL) && pLuaObject->GetObject())
                        return pLuaObject->GetLuaState();
                }
            }
        }
    }

    return NULL;
}

static const char baseString[] = "base_";

// Called by LUA to find the method that corresponds to
// a given method name. The class to lookup is in an
// upvalue. (gettable tag method).
// If the method is a function method push the create a
// wxLua function object onto the lua stack, setting its tag
// so that the function tag of the object will be called to
// invoke the function.
// todo: Handling of properties
int LUACALL getTableFunc(lua_State *L)
{
    GET_LUASTATEVARS_MSG(L, 0)

    bool        fFound  = false;
    int         result  = 0;
    WXLUACLASS *pClass  = (WXLUACLASS *)lua_touserdata(L, lua_upvalueindex(1));
    const char *cpIndex = "{unknown}";

    if ((pClass != NULL) && lua_isuserdata(L, 1) && (lua_islightuserdata(L, 1) == 0) && (ttag(L, 1) == *pClass->class_tag))
    {
        void *pObject = ttouserdata(L, 1);
        cpIndex = lua_tostring(L, 2);

        bool fCallbase = (memcmp(cpIndex, baseString, sizeof(baseString)-1) == 0);
        if (fCallbase)
            cpIndex += sizeof(baseString)-1;
        else
        {
            if (getDerivedMethod(pObject, cpIndex, L) != NULL)
            {
                fFound = true;
                result = 1;
            }
        }

        if (!fFound)
        {
            bool propertyFlag;
            WXLUAMETHOD *pMethod = getLuaMethod(pClass, cpIndex, propertyFlag);
            if (pMethod != NULL)
            {
                if (propertyFlag)
                {
                    lua_remove(L, 2);
                    result = (*pMethod->func)(L);
                    fFound = true;
                }
                else
                {
                    wxLuaFunction *pFunction = new wxLuaFunction(pMethod, pClass, pObject);
                    if (pFunction != NULL)
                    {
                        tpushusertag(L, pFunction, stateVars->m_functionTag);
                        result = 1;
                        fFound = true;
                    }
                }
            }
        }
    }

    if (!fFound)
        terror(L, wx2lua(wxString::Format(_("wxLua: Attempt to call an invalid method '%s'."), lua2wx(cpIndex).c_str())));

    return result;
}

// Called by LUA to find the method that corresponds to
// a given method name. The class to lookup is in an
// upvalue. (settable tag method).
// todo: Handling of properties
int LUACALL setTableFunc(lua_State *L)
{
    GET_LUASTATEVARS_MSG(L, 0)

    WXLUACLASS *pClass = (WXLUACLASS *)lua_touserdata(L, lua_upvalueindex(1));
    const char *cpIndex = lua_tostring(L, 2);

    if ((pClass != NULL) &&
        lua_isuserdata(L, 1) &&
        (lua_islightuserdata(L, 1) == 0) &&
        (ttag(L, 1) == *pClass->class_tag))
    {
        WXLUAMETHOD *pMethod = getLuaProperty(pClass, cpIndex, true);
        if (pMethod != NULL)
        {
            lua_remove(L, 2);
            (*pMethod->func)(L);
        }
        else
        {
            void *pObject = ttouserdata(L, 1);

            bool         fCreated   = false;
            wxHashTable *pHashTable = (wxHashTable *) stateVars->m_pDerivedList->Get((int) pObject);
            if (pHashTable == NULL)
            {
                pHashTable = new wxHashTable(wxKEY_STRING);
                if (pHashTable != NULL)
                {
                    pHashTable->DeleteContents(true);

                    stateVars->m_pDerivedList->Put((int) pObject, pHashTable);
                    fCreated = true;
                }
            }

            if (pHashTable != NULL)
            {
                wxString index = lua2wx(cpIndex);
                if (!fCreated)
                {
                    wxLuaObject *pObject = (wxLuaObject *)pHashTable->Delete(index);
                    if (pObject != NULL)
                        delete pObject;
                }
                pHashTable->Put(index, new wxLuaObject(L, 3));
            }
        }
    }

    return 0;
}

// Handler for the 'function' tag method.
int LUACALL callFunction(lua_State *L)
{
    GET_LUASTATEVARS_MSG(L, 0)

    if (lua_isuserdata(L, 1) && (lua_islightuserdata(L, 1) == 0) && (ttag(L, 1) == stateVars->m_functionTag))
    {
        wxLuaFunction *pFunction = (wxLuaFunction *)ttouserdata(L, 1);
        return pFunction->CallMethod(L);
    }
    return 0;
}

// Lookup and return the 'name' of a Lua object from its tag.
const char * LUACALL GetLuaTagName(lua_State *L, int nTag)
{
    GET_LUASTATEVARS_MSG(L, NULL)

    if (nTag == stateVars->m_wxNull)
        return wxNull;
    if (nTag == stateVars->m_functionTag)
        return "wxLuaFunction";

    if ((pClassList != NULL) && (nTag >= stateVars->m_startTag) && (nTag <= stateVars->m_lastTag))
    {
        size_t iClass = 0;
        for (iClass = 0; iClass < classCount; ++iClass)
        {
            WXLUACLASS *pClass = pClassList + iClass;

            if (*pClass->class_tag == nTag)
                return pClass->name;
        }
    }
    return NULL;
}

// Connect an event to a handler. This Lua 'C' function supports
// function calls with either three or four parameters. These parameters
// are:         The class (which must be derived from wxEvtHandler),
//              The event type
// (Optional)   The ID of the object the event is for
//              A Lua function to call to handle the event.
//              The Lua function gets called with a single parameter
//              which is a reference to the event object
//              associated with the event.
int LUACALL wxEvtHandler_ConnectEvent(lua_State *L)
{
    int          id;
    double       eventType = 0;
    wxLuaCallback *pCallback = NULL;

    int nParams = lua_gettop(L);

    wxEvtHandler *evtHandler = (wxEvtHandler *)getuserdatatype(L, 1, s_wxEvtHandler);

    switch (nParams)
    {
        case 4:
        {
            if (!lua_isfunction(L, 4))
            {
                terror(L, "wxLua: ConnectEvent: Callback function is not a valid Lua function.");
                return 0;
            }

            if (lua_isnil(L, 3))
            {
                terror(L, "wxLua: ConnectEvent: Invalid event type.");
                return 0;
            }
            else
                eventType = lua_tonumber(L, 3);

            id  = (int) lua_tonumber(L, 2);
            if (lua_isnil(L, 2))
            {
                terror(L, "wxLua: ConnectEvent: Invalid identifier.");
                return 0;
            }

            // Create and connect the callback
            pCallback = new wxLuaCallback(L, 4, id, (int) eventType, evtHandler);
            break;
        }
        case 3:
        {
            if (!lua_isfunction(L, 3))
            {
                terror(L, "wxLua: ConnectEvent: Callback function is not a valid Lua function.");
                return 0;
            }

            if (evtHandler->IsKindOf(CLASSINFO(wxWindow)))
            {
                //id = ((wxWindow *)evtHandler)->GetId(); // FIXME! bug in Mac this is the fix
                id = -1;
            }
            else
            {
                terror(L, "wxLua: ConnectEvent: Unexpected event object type.");
                return 0;
            }

            if (lua_isnil(L, 2))
            {
                terror(L, "wxLua: ConnectEvent: Invalid event type.");
                return 0;
            }
            else
            {
                eventType = lua_tonumber(L, 2);
                // Create and connect the callback
                pCallback = new wxLuaCallback(L, 3, id, (int) eventType, evtHandler);
            }
            
            break;
        }
        default:
        {
            terror(L, "wxLua: ConnectEvent: Incorrect number of parameters.");
            break;
        }
    }
    return 0;
}

// Attempt to cast an object reference (the first parameter) to another type.
// The type requested is specified by the second parameter. Presumably the
// type requested will be derived from the supplied object, otherwise
// bad things will happen.
int LUACALL wxObject_DynamicCast(lua_State *L)
{
    int         iResult   = 0;
    const char *className = lua_tostring(L, 2);
    lua_pushvalue(L, 1);
    if (className != NULL)
    {
        wxObject *pObject = ( wxObject * ) ttouserdata(L, 1 );
        unsigned iClass;
        for (iClass = 0; iClass < classCount; ++iClass)
        {
            WXLUACLASS *pClass = pClassList + iClass;
            if (strcmp(pClass->name, className) == 0)
            {
                if (pClass->pClassInfo != NULL)
                {
                    if (pObject->IsKindOf(pClass->pClassInfo))
                    {
                        tsettag(L, *pClass->class_tag);
                        iResult = 1;
                    }
                    else
                        terror(L, "wxLua: DynamicCast: Unable to cast to a class of this type.");
                }
                break;
            }
        }

        if (iResult == 0)
            terror(L, "wxLua: DynamicCast: Class not found.");
    }
    else
        terror(L, "wxLua: DynamicCast: Incorrect number of parameters.");

    return iResult;
}

//-----------------------------------------------------------------------------
// wxLuaCallback
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxLuaCallback, wxObject);

// Encapsulate a lua function reference for use by a event callback
wxLuaCallback::wxLuaCallback( lua_State    *L,
                              int           theRoutine,
                              int           id,
                              wxEventType   eventType,
                              wxEvtHandler *pEvtHandler )
                              : m_luaState(L), m_luaStateVars(NULL),
                                m_pHandler(pEvtHandler),
                                m_id(id), m_eventType(eventType)
{
    GET_LUASTATEVARS_RET(L)
    m_luaStateVars = stateVars;
    m_routine = tinsert(L, theRoutine);

    pEvtHandler->Connect( id,
                          eventType,
                          (wxObjectEventFunction) &wxLuaCallback::EventHandler,
                          this);

    // FIXME - Need to track events attached to an application object
    // labenski - track all of them? useful to run ClearState when window is destroyed
    //if (pEvtHandler->IsKindOf(CLASSINFO(wxApp)))
    {
        m_luaStateVars->m_pAppHandlerList->Append(this);
    }
}

wxLuaCallback::~wxLuaCallback()
{
    // Remove the reference to the Lua function that we are going to call
    if (m_luaState)
        tremove(m_luaState, m_routine);
    // delete the reference to this handler
    if (m_luaStateVars)
        m_luaStateVars->m_pAppHandlerList->DeleteObject(this);
}

// This function is called with "this" being of the type which the event is arriving
// at.  The user data is used to route it to the correct place.
void wxLuaCallback::EventHandler(wxEvent& event)
{        
    wxLuaCallback *theCallback = (wxLuaCallback *) event.m_callbackUserData;

    if (!theCallback || !theCallback->GetLuaState() || !theCallback->m_luaStateVars)
        return;

    theCallback->m_luaStateVars->m_inEventType = event.GetEventType();

    if (event.GetEventType() == wxEVT_DESTROY)
    {
        event.Skip();
        theCallback->m_luaStateVars->m_pWindowList->DeleteObject((wxWindow*)event.GetEventObject());

        // Disconnect all callbacks associated with this window's evthandler
        theCallback->ClearState();

        wxEvtHandler *evtHandler = ((wxWindow*)event.GetEventObject())->GetEventHandler();

        wxNode* node = theCallback->m_luaStateVars->m_pAppHandlerList->GetFirst();
        while (node)
        {
            wxLuaCallback *pCallback = (wxLuaCallback *) node->GetData();

            if ((pCallback != NULL) && (pCallback->GetEvtHandler() == evtHandler))
            {
                pCallback->ClearState();
            }

            node = node->GetNext();
        }
    }
    else //if (theCallback->m_luaState)
        theCallback->CallFunction(&event);
        
    theCallback->m_luaStateVars->m_inEventType = -1;
}

// Call a lua function to handle an event. The lua function will receive
// a single parameter, the type of event.
void wxLuaCallback::CallFunction(wxEvent *pEvent)
{
    // Cannot call it if Lua is gone or the interpreter has been destroyed
    // This can happen when the program exists since windows may be destroyed
    // after lua has been deleted 
    if (!m_luaState || !wxFindLuaStateVariables(m_luaState))
        return;

    int eventClassTag = s_wxEvent;

    WXLUAEVENT eventItem;
    int        eventType = pEvent->GetEventType();
    eventItem.eventType = &eventType;

    WXLUAEVENT *pLuaEvent = (WXLUAEVENT *) bsearch(&eventItem,
                                                    pEventList,
                                                    eventCount,
                                                    sizeof(WXLUAEVENT),
                                                    eventListCompareFn);

    if (pLuaEvent != NULL)
        eventClassTag = *pLuaEvent->eventClassTag;

    lua_checkstack(m_luaState,LUA_MINSTACK);
    int oldTop = lua_gettop(m_luaState);
    if (tget(m_luaState, m_routine))
    {
        lua_pushvalue(m_luaState, LUA_GLOBALSINDEX);
        if (lua_setfenv(m_luaState, -2) != 0)
        {
            tpushusertag(m_luaState, pEvent, eventClassTag);
            LuaCall(m_luaState, 1, true);
        }
        else
            terror(m_luaState, "wxLua: CallFunction: function is not a Lua function.");
    }
    else
        terror(m_luaState, "wxLua: CallFunction: function has been garbage collected.");

    lua_settop(m_luaState, oldTop);
}

int LUACALL CreateStandaloneBitmaps(lua_State *L)
{
    wxCHECK_MSG(s_wxBitmap != -1, 1, wxT("wxBitmap is not wrapped by wxLua"));

#include "../Examples/bitmaps/new.xpm"
#include "../Examples/bitmaps/open.xpm"
#include "../Examples/bitmaps/save.xpm"
#include "../Examples/bitmaps/saveall.xpm"
#include "../Examples/bitmaps/cut.xpm"
#include "../Examples/bitmaps/copy.xpm"
#include "../Examples/bitmaps/paste.xpm"
#include "../Examples/bitmaps/help.xpm"

    wxBitmap *returns;

    returns = new wxBitmap(new_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "newBitmap");

    returns = new wxBitmap(open_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "openBitmap");

    returns = new wxBitmap(save_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "saveBitmap");

    returns = new wxBitmap(saveall_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "saveAllBitmap");

    returns = new wxBitmap(cut_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "cutBitmap");

    returns = new wxBitmap(copy_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "copyBitmap");

    returns = new wxBitmap(paste_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "pasteBitmap");

    returns = new wxBitmap(help_xpm);
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_wxBitmap, returns );
    lua_setglobal(L, "helpBitmap");

    return 0;
}

#if 1 // defined(LUA_EMBEDDED)  // FIXME - who gets the error message?
    #define LUAERROR_OUTPUT(msg) \
        wxLuaInterpreter *luaInterpreter = wxFindLuaInterpreter(L); \
        luaInterpreter->DisplayError(msg);
#else
    #define LUAERROR_OUTPUT(msg) \
        wxGetLuaHandler().DisplayError(msg);
#endif

int LUACALL LuaError(lua_State *L, int status, int top)
{
    int newtop = lua_gettop(L);

    wxString errorMsg;
    if (!wxLuaCheckRunError(status, &errorMsg))
    {
        switch(status)
        {
            case LUA_ERRMEM:
            case LUA_ERRERR:
                if (newtop > top)
                    errorMsg += wxT("\n");
                break;
            case LUA_ERRRUN:
            case LUA_ERRFILE:
            case LUA_ERRSYNTAX:
            default:
                if (newtop > top)
                    errorMsg += lua2wx(lua_tostring(L, -1));
                break;
        }

        errorMsg += wxT("\n");
        LUAERROR_OUTPUT(errorMsg)
        lua_settop(L, top);
        status = 0;
    }

    return status;
}

int LUACALL LuaCall(lua_State *L, int narg, int clear)
{
    int status;
    int top  = lua_gettop(L);
    int base = top - narg;  // function index
    lua_pushliteral(L, "_TRACEBACK");
    lua_rawget(L, LUA_GLOBALSINDEX);  // get traceback function
    lua_insert(L, base);  // put it under chunk and args
    status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
    lua_remove(L, base);  // remove traceback function
    status = LuaError(L, status, top - (narg + 1));
    return status;
}

int LUACALL LuaDoFile(lua_State *L, const wxString &file)
{
    int top = lua_gettop(L);
    int status = luaL_loadfile(L, wx2lua(file));
    if (status == 0)
        status = lua_pcall(L, 0, LUA_MULTRET, 0);  // call main
    status = LuaError(L, status, top);
    return status;
}

int LUACALL LuaDoBuffer(lua_State *L, const char *buffer, size_t len, const char *name)
{
    int top = lua_gettop(L);
    int status = luaL_loadbuffer(L, buffer, len, name);
    if (status == 0)
        status = lua_pcall(L, 0, LUA_MULTRET, 0);  // call main
    status = LuaError(L, status, top);
    return status;
}

bool wxLuaCheckRunError(int ret_val, wxString *msg)
{
    switch (ret_val)
    {
        case 0 :
            return TRUE;
        case LUA_ERRRUN:
            if (msg) *msg = wxT("wxLua: Error while running chunk");
            return FALSE;
        case LUA_ERRSYNTAX:
            if (msg) *msg = wxT("wxLua: Syntax error during pre-compilation");
            return FALSE;
        case LUA_ERRMEM:
            if (msg) *msg = wxT("wxLua: Memory allocation error");
            return FALSE;
        case LUA_ERRERR:
            if (msg) *msg = wxT("wxLua: Generic error or an error occurred while running the error handler");
            return FALSE;
        case LUA_ERRFILE:
            if (msg) *msg = wxT("wxLua: Error occurred while opening file");
            return FALSE;
        default :
            if (msg) *msg = wxT("wxLua: Unknown error");
            break;
    }

    return FALSE;
}
