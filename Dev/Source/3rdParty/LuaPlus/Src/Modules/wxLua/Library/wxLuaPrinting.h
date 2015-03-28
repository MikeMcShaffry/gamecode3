/////////////////////////////////////////////////////////////////////////////
// Purpose:     A wrapper around wxPrintout for wxLua
// Author:      J. Winwood
// Created:     July 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_PRINTING_H
#define WX_LUA_PRINTING_H

#ifdef __GNUG__
    #pragma interface "wxLuaPrinting.h"
#endif

#include "wx/print.h"

// ----------------------------------------------------------------------------
// wxLuaPrintout
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLuaPrintout : public wxPrintout
{
public:
    wxLuaPrintout(const wxString& title = _("Printout"),
                  wxLuaObject *pObject = NULL);

    void SetPageInfo(int minPage, int maxPage, int pageFrom, int pageTo);
    // overrides
    virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);
    virtual bool HasPage(int pageNum);
    virtual bool OnBeginDocument(int startPage, int endPage);
    virtual void OnEndDocument();
    virtual void OnBeginPrinting();
    virtual void OnEndPrinting();
    virtual void OnPreparePrinting();
    virtual bool OnPrintPage(int pageNumber);

    wxLuaObject *GetID() const { return m_pObject; }

private:
    wxLuaObject *m_pObject;
    int          m_minPage;
    int          m_maxPage;
    int          m_pageFrom;
    int          m_pageTo;
    DECLARE_CLASS(wxLuaPrintout)
};

#endif //WX_LUA_PRINTING_H
