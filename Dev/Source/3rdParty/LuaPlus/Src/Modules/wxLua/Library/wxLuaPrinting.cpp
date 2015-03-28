/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaPrinting.cpp
// Purpose:     Provide an interface to wxPrintout for wxLua.
// Author:      J. Winwood.
// Created:     July 2002.
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaPrinting.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/datetime.h"

#include "wxLuaCallback.h"
#include "wxLuaInternals.h"
#include "wxLuaPrinting.h"

// forward reference
class wxLuaObject;

// This lua tag is defined in wxLuaWrap.cpp
extern int s_wxLuaPrintout;

// ----------------------------------------------------------------------------
// wxLuaPrintout
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxLuaPrintout, wxPrintout)

wxLuaPrintout::wxLuaPrintout(const wxString& title, wxLuaObject *pObject) :
                 wxPrintout(title),
                 m_pObject(pObject),
                 m_minPage(0),
                 m_maxPage(0),
                 m_pageFrom(0),
                 m_pageTo(0)
{
}

void wxLuaPrintout::SetPageInfo(int minPage, int maxPage, int pageFrom, int pageTo)
{
    m_minPage  = minPage;
    m_maxPage  = maxPage;
    m_pageFrom = pageFrom;
    m_pageTo   = pageTo;
}

void wxLuaPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{    
    lua_State *luaState = getDerivedMethod(this, "GetPageInfo");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);
        
        LuaCall(luaState, 1, false);
        
        *minPage  = (int) lua_tonumber(luaState, -4);
        *maxPage  = (int) lua_tonumber(luaState, -3);
        *pageFrom = (int) lua_tonumber(luaState, -2);
        *pageTo   = (int) lua_tonumber(luaState, -1);
                
        lua_settop(luaState, nOldTop);
    }
    else
    {
        *minPage  = m_minPage;
        *maxPage  = m_maxPage;
        *pageFrom = m_pageFrom;
        *pageTo   = m_pageTo;
    }
}

bool wxLuaPrintout::HasPage(int pageNum)
{
    bool fResult = false;
    lua_State *luaState = getDerivedMethod(this, "HasPage");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);
        lua_pushnumber(luaState, pageNum);
        
        LuaCall(luaState, 2, false);
        
        fResult = (lua_toboolean(luaState, -1) != false);
        lua_settop(luaState, nOldTop);
    }
    else
        fResult = wxPrintout::HasPage(pageNum);
    return fResult;
}

bool wxLuaPrintout::OnBeginDocument(int startPage, int endPage)
{
    bool fResult = true;
    
    lua_State *luaState = getDerivedMethod(this, "OnBeginDocument");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);
        lua_pushnumber(luaState, startPage);
        lua_pushnumber(luaState, endPage);
        
        LuaCall(luaState, 3, false);
        
        fResult = (lua_toboolean(luaState, -1) != false);
        lua_pop(luaState, 1);
        lua_settop(luaState, nOldTop);
    }
    else
        fResult = wxPrintout::OnBeginDocument(startPage, endPage);            

    return fResult;
}

void wxLuaPrintout::OnEndDocument()
{
    lua_State *luaState = getDerivedMethod(this, "OnEndDocument");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);       
        LuaCall(luaState, 1, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxPrintout::OnEndDocument();
}

void wxLuaPrintout::OnBeginPrinting()
{
    lua_State *luaState = getDerivedMethod(this, "OnBeginPrinting");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);       
        LuaCall(luaState, 1, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxPrintout::OnBeginPrinting();
}

void wxLuaPrintout::OnEndPrinting()
{
    lua_State *luaState = getDerivedMethod(this, "OnEndPrinting");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);       
        LuaCall(luaState, 1, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxPrintout::OnEndPrinting();
}

void wxLuaPrintout::OnPreparePrinting()
{
    lua_State *luaState = getDerivedMethod(this, "OnPreparePrinting");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);       
        LuaCall(luaState, 1, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxPrintout::OnPreparePrinting();
}

bool wxLuaPrintout::OnPrintPage(int pageNum)
{
    bool fResult = false;
    lua_State *luaState = getDerivedMethod(this, "OnPrintPage");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaPrintout);
        lua_pushnumber(luaState, pageNum);
        
        LuaCall(luaState, 2, false);
        
        fResult = (lua_toboolean(luaState, -1) != false);
        lua_settop(luaState, nOldTop);
    }
    return fResult;
}
