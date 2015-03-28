/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaHtmlWindow.cpp
// Purpose:     Provide an interface to wxHtmlWindow for wxLua.
// Author:      J. Winwood.
// Created:     June 2002.
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaHtmlWindow.h"
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
#include "wxLuaHtmlWindow.h"
#include "wxLuaInternals.h"

// These are the wxLua tags defined in wxLuaWrap.cpp
extern int s_wxLuaHtmlWindow;
extern int s_wxHtmlCell;
extern int s_wxMouseEvent;
extern int s_wxHtmlLinkInfo;

DEFINE_EVENT_TYPE(wxEVT_HTML_TAG_HANDLER)

// ----------------------------------------------------------------------------
// wxLuaHtmlWindow
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxLuaHtmlWindow, wxHtmlWindow)

BEGIN_EVENT_TABLE(wxLuaHtmlWindow, wxHtmlWindow)
END_EVENT_TABLE()

wxLuaHtmlWindow::wxLuaHtmlWindow() : wxHtmlWindow()
{
}

wxLuaHtmlWindow::~wxLuaHtmlWindow()
{
}

wxLuaHtmlWindow::wxLuaHtmlWindow(wxWindow *parent, 
                                 wxWindowID id, 
                                 const wxPoint& pos, 
                                 const wxSize& size, 
                                 long  style, 
                                 const wxString& name) :
                 wxHtmlWindow(parent, id, pos, size, style, name)
{
}

void wxLuaHtmlWindow::OnCellClicked(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& event)
{
    bool fResult = false;
    lua_State *luaState = getDerivedMethod(this, "OnCellClicked");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaHtmlWindow);
        tpushusertag(luaState, cell, s_wxHtmlCell);
        lua_pushnumber(luaState, x);
        lua_pushnumber(luaState, y);
        tpushusertag(luaState, (void *) &event, s_wxMouseEvent);
        
        LuaCall(luaState, 5, false);
        
        fResult = (lua_tonumber(luaState, -1) != 0);
        lua_settop(luaState, nOldTop);
        
        if (fResult)
            wxHtmlWindow::OnCellClicked(cell, x, y, event);
    }
    else
        wxHtmlWindow::OnCellClicked(cell, x, y, event);
}

void wxLuaHtmlWindow::OnCellMouseHover(wxHtmlCell *cell, wxCoord x, wxCoord y)
{
    lua_State *luaState = getDerivedMethod(this, "OnCellMouseHover");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaHtmlWindow);
        tpushusertag(luaState, cell, s_wxHtmlCell);
        lua_pushnumber(luaState, x);
        lua_pushnumber(luaState, y);
        
        LuaCall(luaState, 4, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxHtmlWindow::OnCellMouseHover(cell, x, y);
}

void wxLuaHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    lua_State *luaState = getDerivedMethod(this, "OnLinkClicked");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaHtmlWindow);
        tpushusertag(luaState, (void *) &link, s_wxHtmlLinkInfo);
        
        LuaCall(luaState, 2, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxHtmlWindow::OnLinkClicked(link);
}

void wxLuaHtmlWindow::OnSetTitle(const wxString& title)
{
    lua_State *luaState = getDerivedMethod(this, "OnSetTitle");
    if (luaState != NULL)
    {
        int nOldTop = lua_gettop(luaState);
        tpushusertag(luaState, this, s_wxLuaHtmlWindow);
        lua_pushstring(luaState, wx2lua(title));
    
        LuaCall(luaState, 2, true);
        lua_settop(luaState, nOldTop);
    }
    else
        wxHtmlWindow::OnSetTitle(title);
}

// ----------------------------------------------------------------------------
// wxLuaHtmlWinTagHandler
// ----------------------------------------------------------------------------

class wxLuaHtmlWinTagHandler : public wxHtmlWinTagHandler
{
public:
    wxLuaHtmlWinTagHandler()
    {
    }
    
    wxString GetSupportedTags()
    {
        return wxString(wxT("LUA"));
    }

    bool HandleTag(const wxHtmlTag& tag)
    {
        wxLuaHtmlWinTagEvent htmlEvent(wxEVT_HTML_TAG_HANDLER);
        htmlEvent.SetTagInfo(&tag, m_WParser);
    
        if (wxTheApp->ProcessEvent(htmlEvent))
            return htmlEvent.GetParseInnerCalled();
        
        return FALSE;
    }
};

class wxLuaHtmlTagsModule : public wxHtmlTagsModule
{
    DECLARE_DYNAMIC_CLASS(wxLuaHtmlTagsModule)
public:
    void FillHandlersTable(wxHtmlWinParser *parser)
    {
        parser->AddTagHandler(new wxLuaHtmlWinTagHandler);
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxLuaHtmlTagsModule, wxHtmlTagsModule)

// ----------------------------------------------------------------------------
// wxLuaHtmlWinTagEvent
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxLuaHtmlWinTagEvent, wxEvent)

wxLuaHtmlWinTagEvent::wxLuaHtmlWinTagEvent() :
    m_pHtmlTag(NULL),
    m_pHtmlParser(NULL),
    m_fParseInnerCalled(false)
{
}

wxLuaHtmlWinTagEvent::wxLuaHtmlWinTagEvent(int eventType) :
    m_pHtmlTag(NULL),
    m_pHtmlParser(NULL),
    m_fParseInnerCalled(false)
{
    SetEventType(eventType);    
}

wxLuaHtmlWinTagEvent::~wxLuaHtmlWinTagEvent()
{
}

void wxLuaHtmlWinTagEvent::SetTagInfo(const wxHtmlTag *pHtmlTag, 
                                      wxHtmlWinParser *pParser)
{
    m_pHtmlTag    = pHtmlTag;
    m_pHtmlParser = pParser;
}

wxEvent* wxLuaHtmlWinTagEvent::Clone() const
{
    wxLuaHtmlWinTagEvent *pEvent = new wxLuaHtmlWinTagEvent(GetEventType());
    if (pEvent != NULL)
    {
        pEvent->m_pHtmlTag          = m_pHtmlTag;
        pEvent->m_pHtmlParser       = m_pHtmlParser;
        pEvent->m_fParseInnerCalled = m_fParseInnerCalled;
    }
    return pEvent;
}

const wxHtmlTag *wxLuaHtmlWinTagEvent::GetHtmlTag() const
{
    return m_pHtmlTag;
}

wxHtmlWinParser *wxLuaHtmlWinTagEvent::GetHtmlParser() const
{
    return m_pHtmlParser;
}

bool wxLuaHtmlWinTagEvent::GetParseInnerCalled() const
{
    return m_fParseInnerCalled;
}

void wxLuaHtmlWinTagEvent::SetParseInnerCalled(bool fParseInnerCalled)
{
    m_fParseInnerCalled = fParseInnerCalled;
}
