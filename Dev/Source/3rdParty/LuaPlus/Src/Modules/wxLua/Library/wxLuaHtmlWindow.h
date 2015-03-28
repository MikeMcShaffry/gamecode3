/////////////////////////////////////////////////////////////////////////////
// Purpose:     A wrapper around wxHtmlWindow for wxLua
// Author:      J. Winwood
// Created:     June 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_HTML_WINDOW_H
#define WX_LUA_HTML_WINDOW_H

#ifdef __GNUG__
    #pragma interface "wxLuaHtmlWindow.h"
#endif

#include "wx/print.h"
#include "wx/html/htmlwin.h"
#include "wx/html/htmlcell.h"

// ----------------------------------------------------------------------------
// wxLuaHtmlWindow
// ----------------------------------------------------------------------------

class wxLuaHtmlWindow : public wxHtmlWindow
{
public:

    wxLuaHtmlWindow();
    virtual ~wxLuaHtmlWindow();

    wxLuaHtmlWindow(wxWindow *parent,
                    wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHW_SCROLLBAR_AUTO,
                    const wxString& name = wxT("wxLuaHtmlWindow"));

    virtual void OnCellClicked(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& event);
    virtual void OnCellMouseHover(wxHtmlCell *cell, wxCoord x, wxCoord y);
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link);
    virtual void OnSetTitle(const wxString& title);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS (wxLuaHtmlWindow)
};

// ----------------------------------------------------------------------------
// wxLuaHtmlWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaHtmlWinTagEvent: public wxEvent
{
public:
    wxLuaHtmlWinTagEvent();
    wxLuaHtmlWinTagEvent(int eventType);
    virtual ~wxLuaHtmlWinTagEvent();

    void SetTagInfo(const wxHtmlTag *pHtmlTag, wxHtmlWinParser *pParser);

    const wxHtmlTag *GetHtmlTag() const;
    wxHtmlWinParser *GetHtmlParser() const;
    void             SetParseInnerCalled(bool fParseInnerCalled);
    bool             GetParseInnerCalled() const;

protected:
    virtual wxEvent* Clone() const;

private:
    const wxHtmlTag *m_pHtmlTag;
    wxHtmlWinParser *m_pHtmlParser;
    bool             m_fParseInnerCalled;
    DECLARE_DYNAMIC_CLASS(wxLuaHtmlWinTagEvent)
};

typedef void (wxEvtHandler::*wxLuaHtmlWinTagEventFunction)(wxLuaHtmlWinTagEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_HTML_TAG_HANDLER, 2520)
END_DECLARE_EVENT_TYPES()

#define EVT_HTML_TAG_HANDLER(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_HTML_TAG_HANDLER, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxLuaHtmlWinTagEventFunction) & fn, (wxObject *) NULL),

#endif //WX_LUA_HTML_WINDOW_H
