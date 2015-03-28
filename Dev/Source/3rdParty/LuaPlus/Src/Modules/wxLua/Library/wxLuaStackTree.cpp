/////////////////////////////////////////////////////////////////////////////
// Name:        StackTree.cpp
// Purpose:     Display the lua stack in a dialog.
// Author:      J. Winwood
// Created:     February 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "wxLuaStackTree.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/imaglist.h"
#include "wxLuaStackTree.h"
#include "wxLuaInternals.h"
#include "wxLua.h"

#include "wxLuaDebugIO.h"
#include "wxLuaDebugServer.h"
#include "wxLuaLibrary.h"

#if defined(__WXGTK__) || defined(__WXMAC__) || defined(__WXMOTIF__)
    #include "../wxLua.xpm"
#endif

#if !defined(__WXMSW__) || wxUSE_XPM_IN_MSW
/* Closed folder */
static char * icon1_xpm[] =
{
    /* width height ncolors chars_per_pixel */
    "16 16 6 1",
    /* colors */
    "   s None  c None",
    ".  c #000000",
    "+  c #c0c0c0",
    "@  c #808080",
    "#  c #ffff00",
    "$  c #ffffff",
    /* pixels */
    "                ",
    "   @@@@@        ",
    "  @#+#+#@       ",
    " @#+#+#+#@@@@@@ ",
    " @$$$$$$$$$$$$@.",
    " @$#+#+#+#+#+#@.",
    " @$+#+#+#+#+#+@.",
    " @$#+#+#+#+#+#@.",
    " @$+#+#+#+#+#+@.",
    " @$#+#+#+#+#+#@.",
    " @$+#+#+#+#+#+@.",
    " @$#+#+#+#+#+#@.",
    " @@@@@@@@@@@@@@.",
    "  ..............",
    "                ",
    "                "
};

/* File */
static char * icon2_xpm[] =
{
    /* width height ncolors chars_per_pixel */
    "16 16 3 1",
    /* colors */
    "     s None    c None",
    ".    c #000000",
    "+    c #ffffff",
    /* pixels */
    "                ",
    "  ........      ",
    "  .++++++..     ",
    "  .+.+.++.+.    ",
    "  .++++++....   ",
    "  .+.+.+++++.   ",
    "  .+++++++++.   ",
    "  .+.+.+.+.+.   ",
    "  .+++++++++.   ",
    "  .+.+.+.+.+.   ",
    "  .+++++++++.   ",
    "  .+.+.+.+.+.   ",
    "  .+++++++++.   ",
    "  ...........   ",
    "                ",
    "                "
};
#endif

// ----------------------------------------------------------------------------
// wxLuaStackFrame
// ----------------------------------------------------------------------------

wxLuaStackFrame::wxLuaStackFrame( wxWindow *parent,
                                  const wxString   &title,
                                  const wxPoint    &pos,
                                  const wxSize     &size,
                                  wxLuaDebugServer *pServer)
                : wxDialog(parent, idSTACK_FRAME, title, pos, size,
                  wxCAPTION | wxSYSTEM_MENU | wxDIALOG_MODAL | wxRESIZE_BORDER),
         m_luaInterface(NULL),
         m_pServer(pServer)
{
    Construct();
}

wxLuaStackFrame::wxLuaStackFrame( wxWindow         *parent,
                                  const wxString   &title,
                                  const wxPoint    &pos,
                                  const wxSize     &size,
                                  wxLuaInterface   *luaInterface)
                : wxDialog(parent, idSTACK_FRAME, title, pos, size,
                  wxCAPTION | wxSYSTEM_MENU | wxDIALOG_MODAL | wxRESIZE_BORDER),
                  m_luaInterface(luaInterface),
                  m_pServer(NULL)
{
    Construct();
}

wxLuaStackFrame::~wxLuaStackFrame()
{
    if (m_pServer != NULL)
        m_pServer->CleanupDebugReferences();

    if (m_pServer != NULL)
        wxGetLuaHandler().SetStackFrame(NULL);
}

void wxLuaStackFrame::Construct()
{
    m_treeSplitter      = NULL;
    m_valueSplitter     = NULL;
    m_scrolledWindow    = NULL;
    m_treeControl       = NULL;
    m_firstValueWindow  = NULL;
    m_secondValueWindow = NULL;

    if (m_pServer != NULL)
        wxGetLuaHandler().SetStackFrame(this);

    m_stackComboBox = new wxComboBox( this,
                                      idSTACK_COMBOBOX,
                                      wxEmptyString,
                                      wxPoint(0, 0),
                                      wxDefaultSize,
                                      0,
                                      NULL,
                                      wxCB_DROPDOWN | wxCB_READONLY);

    m_scrolledWindow = new wxSplitterScrolledWindow(this,
                                                    idSCROLLED_WINDOW,
                                                    wxDefaultPosition,
                                                    wxDefaultSize,
                                                    wxNO_BORDER | wxCLIP_CHILDREN | wxVSCROLL);


#if wxCHECK_VERSION(2, 3, 0)
    m_dismissButton = new wxButton(this,
                                   idDISMISS_BUTTON,
                                   _("Cancel"),
                                   wxDefaultPosition,
                                   wxDefaultSize,
                                   wxBU_EXACTFIT);
#else
    m_dismissButton = new wxButton(this,
                                   idDISMISS_BUTTON,
                                   _("Cancel"),
                                   wxDefaultPosition,
                                   wxDefaultSize);
#endif
    m_dismissButton->SetDefault();
    m_treeSplitter = new wxThinSplitterWindow(m_scrolledWindow,
                                              idSPLITTER_WINDOW,
                                              wxDefaultPosition,
                                              wxDefaultSize,
                                              wxSP_3DBORDER | wxCLIP_CHILDREN);
    m_treeSplitter->SetSashSize(2);

    /* Note the wxTR_ROW_LINES style: draws horizontal lines between items */
#if wxCHECK_VERSION(2, 3, 0)
    m_treeControl = new wxLuaStackTree(m_treeSplitter,
                                  idSTACKTREE_CTRL,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxNO_BORDER | wxTR_ROW_LINES );
#else
    m_treeControl = new wxLuaStackTree(m_treeSplitter,
                                  idSTACKTREE_CTRL,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxNO_BORDER);
#endif
    m_valueSplitter = new wxThinSplitterWindow(m_treeSplitter,
                                               idVALUE_SPLITTER_WINDOW,
                                               wxDefaultPosition,
                                               wxDefaultSize,
                                               wxSP_NOBORDER | wxCLIP_CHILDREN);
    m_valueSplitter->SetSashSize(2);

    m_firstValueWindow = new wxLuaStackDataWindow(m_valueSplitter,
                                             idFIRST_VALUE_WINDOW,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxNO_BORDER);
    m_secondValueWindow = new wxLuaStackDataWindow(m_valueSplitter,
                                             idSECOND_VALUE_WINDOW,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxNO_BORDER,
                                             false);

    m_treeSplitter->SetSashPosition(25);
    m_treeSplitter->SetMinimumPaneSize(10);
    m_treeSplitter->SplitVertically(m_treeControl, m_valueSplitter);

    m_valueSplitter->SetSashPosition(125);
    m_valueSplitter->SetMinimumPaneSize(10);
    m_valueSplitter->SplitVertically(m_firstValueWindow, m_secondValueWindow);

    m_scrolledWindow->SetTargetWindow(m_treeControl);
    m_scrolledWindow->EnableScrolling(FALSE, FALSE);

    // Let the two controls know about each other
    m_firstValueWindow->SetTreeCtrl(m_treeControl);
    m_secondValueWindow->SetTreeCtrl(m_treeControl);
    m_treeControl->SetCompanionWindows(m_firstValueWindow, m_secondValueWindow);

    // set the frame icon
#if wxCHECK_VERSION(2, 3, 0)
    SetIcon(wxICON(LUA));
#endif

    int nWidth, nHeight;
    GetClientSize(&nWidth, &nHeight);
    SizeWindow(nWidth, nHeight);

    if (m_pServer != NULL)
        m_pServer->EnumerateStack();
    else
    {
        wxLuaDebugData *pSortedList = m_luaInterface->BuildStack();
        if (pSortedList != NULL)
        {
            FillStackCombobox(pSortedList);
            delete pSortedList;
        }
    }
}

void wxLuaStackFrame::FillStackCombobox(const wxLuaDebugData *pSortedList)
{
    size_t idx;

    m_stackComboBox->Clear();

    for (idx = 0; idx < pSortedList->Count(); ++idx)
    {
        const wxLuaDebugDataItem *item = pSortedList->Item(idx);
#if defined(__WXMAC__)
		// Mac is dying on the two parameter version...don't know how to fix.
		m_stackComboBox->Append(item->GetName());
#else
		m_stackComboBox->Append(item->GetName(), (void *) item->GetReference());        
#endif
    }

    if (pSortedList->Count() > 0)
    {
        m_stackComboBox->SetSelection(0);
        SelectStack(0);
    }
}

BEGIN_EVENT_TABLE(wxLuaStackFrame, wxDialog)
    EVT_SIZE(wxLuaStackFrame::OnSizeWindow)
    EVT_COMBOBOX(idSTACK_COMBOBOX, wxLuaStackFrame::OnSelectStack)
    EVT_TREE_ITEM_EXPANDING(idSTACKTREE_CTRL, wxLuaStackFrame::OnItemExpanding)
    EVT_BUTTON(idDISMISS_BUTTON, wxLuaStackFrame::OnCloseDialog)
END_EVENT_TABLE()

void wxLuaStackFrame::OnSizeWindow(wxSizeEvent &event)
{
    wxSize size = event.GetSize();
    SizeWindow(size.x, size.y);
}

void wxLuaStackFrame::SizeWindow(int nWidth, int nHeight)
{
    int nComboWidth, nComboHeight, nButtonWidth, nButtonHeight;
    m_stackComboBox->GetSize(&nComboWidth, &nComboHeight);
    m_dismissButton->GetSize(&nButtonWidth, &nButtonHeight);
    nComboHeight += 2; // makes it look a little better
    m_stackComboBox->SetSize(0, 0, nWidth, -1);
    m_scrolledWindow->SetSize(0,
                              nComboHeight,
                              nWidth,
                              nHeight - 15 - nComboHeight - nButtonHeight);
    m_dismissButton->SetSize((nWidth / 2) - (nButtonWidth / 2),
                             nHeight - 5 - nComboHeight,
                             nButtonWidth,
                             nButtonHeight);
}

void wxLuaStackFrame::OnSelectStack(wxCommandEvent &event)
{
    SelectStack(event.GetSelection());
}

void wxLuaStackFrame::SelectStack(int nCurrentSel)
{
    m_nCurrentSel = nCurrentSel;
    int nEntry = (int) m_stackComboBox->GetClientData(nCurrentSel);

    if (m_pServer != NULL)
    {
        m_pServer->EnumerateStackEntry(nEntry);
    }
    else
    {
        wxLuaDebugData *pSortedList = m_luaInterface->BuildStackList(nEntry);
        if (pSortedList != NULL)
        {
            FillStackEntry(nEntry, pSortedList);
            delete pSortedList;
        }
    }
}

void wxLuaStackFrame::FillStackEntry(int /*nEntry*/, const wxLuaDebugData *pSortedList)
{
    m_treeControl->DeleteAllItems();
    wxTreeItemId rootItem = m_treeControl->AddRoot(_("Locals"), -1, -1);
    m_treeControl->AdjustRemoteScrollbars();

    size_t idx;
    for (idx = 0; idx < pSortedList->Count(); ++idx)
    {
        const wxLuaDebugDataItem *item = pSortedList->Item(idx);

        int nOffset = 1;
        if (item->GetReference() != LUA_NOREF)
            nOffset = 0;

        wxTreeItemId treeNode = m_treeControl->AppendItem(rootItem,
                                                          item->GetName(),
                                                          nOffset,
                                                          nOffset,
                                                          new wxLuaDebugDataItem(*item));

        if (item->GetReference() != LUA_NOREF)
            m_treeControl->SetItemHasChildren(treeNode);
    }

//  If at global scope, process globals
    if (m_nCurrentSel == m_stackComboBox->GetCount() - 1)
    {
        if (m_pServer != NULL)
            m_pServer->EnumerateTable(-1, -1, rootItem);
        else
        {
            wxLuaDebugData *pSortedList = m_luaInterface->GetGlobalData();
            const wxLuaDebugDataItem *item = pSortedList->Item(0);

            wxTreeItemId treeNode = m_treeControl->AppendItem( rootItem,
                                                               item->GetName(),
                                                               0,
                                                               0,
                                                               new wxLuaDebugDataItem(*item));

            if (item->GetReference() != LUA_NOREF)
                m_treeControl->SetItemHasChildren(treeNode);

            GetDerivedAndTrackedItems(m_treeControl, rootItem);

            delete pSortedList;
        }
    }

    m_treeControl->Expand(rootItem);
}

void wxLuaStackFrame::GetDerivedAndTrackedItems(wxRemotelyScrolledTreeCtrl *treeControl, wxTreeItemId rootItem)
{
    GET_LUASTATEVARS_RET(m_luaInterface->GetLuaState())

    int derivedCount = 0,
        trackedCount = 0,
        appHandlerCount = 0,
        windowCount = 0;

    if (stateVars->m_pDerivedList)
        derivedCount = stateVars->m_pDerivedList->GetCount();
    if (stateVars->m_pTrackedList)
        trackedCount = stateVars->m_pTrackedList->GetCount();
    if (stateVars->m_pAppHandlerList)
        appHandlerCount = stateVars->m_pAppHandlerList->GetCount();
    if (stateVars->m_pWindowList)
        windowCount = stateVars->m_pWindowList->GetCount();

    treeControl->AppendItem (rootItem, wxT("Derived List"), 1, 1, new wxLuaDebugDataItem(_("Derived List"),  wxT("wxLuaData"), wxT(""), wxString::Format(wxT("%d"), derivedCount), LUA_NOREF, 0));
    treeControl->AppendItem (rootItem, wxT("Tracked List"), 1, 1, new wxLuaDebugDataItem(_("Tracked List"), wxT("wxLuaData"), wxT(""), wxString::Format(wxT("%d"), trackedCount), LUA_NOREF, 0));
    treeControl->AppendItem (rootItem, wxT("AppHandler List"), 1, 1, new wxLuaDebugDataItem(_("AppHandler List"), wxT("wxLuaData"), wxT(""), wxString::Format(wxT("%d"), appHandlerCount), LUA_NOREF, 0));
    treeControl->AppendItem (rootItem, wxT("wxWindow List"), 1, 1, new wxLuaDebugDataItem(_("wxWindow List"), wxT("wxLuaData"), wxT(""), wxString::Format(wxT("%d"), windowCount), LUA_NOREF, 0));
}

void wxLuaStackFrame::OnItemExpanding(wxTreeEvent &event)
{
    ItemExpanding(event.GetItem());
}

void wxLuaStackFrame::ItemExpanding(const wxTreeItemId &itemNode)
{
    if (itemNode)
    {
        wxLuaDebugDataItem *pDebugData = (wxLuaDebugDataItem *)m_treeControl->GetItemData(itemNode);
        if (pDebugData != NULL)
        {
            int nRef = pDebugData->GetReference();
            if (nRef != LUA_NOREF)
            {
                int nIndex = pDebugData->GetIndex() + 1;
                if (!pDebugData->IsExpanded())
                {
                    pDebugData->SetExpanded(true);

                    if (m_pServer != NULL)
                    {
                        m_pServer->EnumerateTable(nRef, nIndex, itemNode);
                    }
                    else
                    {
                        // insert items
                        wxLuaDebugData *pSortedList = m_luaInterface->BuildTableList(nRef, nIndex);
                        if (pSortedList != NULL)
                        {
                            FillTableEntry(itemNode, pSortedList);
                            delete pSortedList;
                        }
                    }
                }
            }
        }
    }
}

void wxLuaStackFrame::FillTableEntry(wxTreeItemId itemNode, const wxLuaDebugData *pSortedList)
{
    if (pSortedList->Count() == 0)
        m_treeControl->SetItemHasChildren(itemNode, false);
    else
    {
        size_t count;
        for (count = 0; count < pSortedList->Count(); ++count)
        {
            const wxLuaDebugDataItem *item = pSortedList->Item(count);

            int nOffset = 1;
            if (item->GetReference() != LUA_NOREF)
                nOffset = 0;

            wxTreeItemId treeNode = m_treeControl->AppendItem(itemNode,
                                                              item->GetName(),
                                                              nOffset,
                                                              nOffset,
                                                              new wxLuaDebugDataItem(*item));
            if (item->GetReference() != LUA_NOREF)
                m_treeControl->SetItemHasChildren(treeNode);
        }
    }
    m_treeControl->Expand(itemNode);
}

void wxLuaStackFrame::OnCloseDialog(wxCommandEvent & /*event*/)
{
    EndModal(0);
}

// ----------------------------------------------------------------------------
// wxLuaStackTree
// ----------------------------------------------------------------------------

wxLuaStackTree::wxLuaStackTree( wxWindow    *parent,
                                wxWindowID     id,
                                const wxPoint &pt,
                                const wxSize  &sz,
                                long           style)
               : wxRemotelyScrolledTreeCtrl(parent, id, pt, sz, style)
{
    m_imageList = new wxImageList(16, 16, TRUE);
#if !defined(__WXMSW__) // || wxUSE_XPM_IN_MSW
    m_imageList->Add(wxIcon(icon1_xpm));
    m_imageList->Add(wxIcon(icon2_xpm));
#elif defined(__WXMSW__)
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_CLOSED_FOLDER"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_FILE"),          wxBITMAP_TYPE_ICO_RESOURCE));
#else
#error "Sorry, we don't have icons available for this platforms."
#endif
    SetImageList(m_imageList);
}

IMPLEMENT_CLASS(wxLuaStackTree, wxRemotelyScrolledTreeCtrl)

BEGIN_EVENT_TABLE(wxLuaStackTree, wxRemotelyScrolledTreeCtrl)
END_EVENT_TABLE()

wxLuaStackTree::~wxLuaStackTree()
{
    SetImageList(NULL);
    delete m_imageList;
}

// ----------------------------------------------------------------------------
// wxLuaStackDataWindow
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wxLuaStackDataWindow, wxTreeCompanionWindow)
END_EVENT_TABLE()

wxLuaStackDataWindow::wxLuaStackDataWindow(wxWindow      *parent,
                                           wxWindowID     id,
                                           const wxPoint &pos,
                                           const wxSize  &sz,
                                           long           style,
                                           bool           fFirst):
      wxTreeCompanionWindow(parent, id, pos, sz, style),
      m_fFirst(fFirst)
{
    SetBackgroundColour(*wxWHITE);
}

void wxLuaStackDataWindow::DrawItem(wxDC &dc, wxTreeItemId id, const wxRect &rect)
{
    if (m_treeControl)
    {
        wxLuaDebugDataItem *itemData = (wxLuaDebugDataItem *) m_treeControl->GetItemData(id);

        dc.SetTextForeground(*wxBLACK);
        dc.SetBackgroundMode(wxTRANSPARENT);

        int textWidth, textHeight = 0;
        if (itemData != NULL)
        {
            if (m_fFirst)
                dc.GetTextExtent(itemData->GetType(), &textWidth, &textHeight);
            else
                dc.GetTextExtent(itemData->GetValue(), &textWidth, &textHeight);
        }

        int x = 5;
        int y = rect.GetY() + wxMax(0, (rect.GetHeight() - textHeight) / 2);

        if (itemData != NULL)
        {
            if (m_fFirst)
                dc.DrawText(itemData->GetType(), x, y);
            else
                dc.DrawText(itemData->GetValue(), x, y);
        }
    }
}
