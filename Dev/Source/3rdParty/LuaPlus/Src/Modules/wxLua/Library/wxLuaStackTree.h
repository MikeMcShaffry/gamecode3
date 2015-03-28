/////////////////////////////////////////////////////////////////////////////
// Name:         wxLuaStackTree.h
// Purpose:      Interface to display the lua stack in wxLua
// Author:       J. Winwood
// Created:      April 2002
// Copyright:    (c) 2002. Lomtick Software. All rights reserved.
// Licence:      wxWidgets license.
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LUA_STACKTREE_H_
#define _WX_LUA_STACKTREE_H_

#ifdef __GNUG__
    #pragma interface "wxLuaStackTree.h"
#endif

#include "wxLuaSplitTree.h"

/*
extern "C"
{
    typedef struct lua_State lua_State;
    typedef struct lua_Debug lua_Debug;
}
*/

class wxLuaStackDataWindow;
class wxLuaDebugServer;
class wxLuaInterface;
class wxLuaDebugData;

// ----------------------------------------------------------------------------
// wxLuaStackFrame
// ----------------------------------------------------------------------------

class wxLuaStackFrame : public wxDialog
{
public:
    wxLuaStackFrame(wxWindow         *parent,
                    const wxString   &title,
                    const wxPoint    &pos,
                    const wxSize     &size,
                    wxLuaDebugServer *pServer);

    wxLuaStackFrame(wxWindow         *parent,
                    const wxString   &title,
                    const wxPoint    &pos,
                    const wxSize     &size,
                    wxLuaInterface   *luaInterface);

    ~wxLuaStackFrame();

    void OnSizeWindow(wxSizeEvent& event);
    void OnSelectStack(wxCommandEvent &event);
    void OnItemExpanding(wxTreeEvent &event);
    void OnCloseDialog(wxCommandEvent &event);

    void SelectStack(int nCurrentSel);
    void ItemExpanding(const wxTreeItemId &itemId);
    void SizeWindow(int nWidth, int nHeight);

    void FillStackCombobox(const wxLuaDebugData *pDebugData);
    void FillStackEntry(int nEntry, const wxLuaDebugData *pSortedList);
    void FillTableEntry(wxTreeItemId itemNode, const wxLuaDebugData *pSortedList);

    void GetDerivedAndTrackedItems(wxRemotelyScrolledTreeCtrl *treeControl, wxTreeItemId rootItem);

private:
    void Construct();

protected:
    wxComboBox                 *m_stackComboBox;
    wxRemotelyScrolledTreeCtrl *m_treeControl;
    wxThinSplitterWindow       *m_treeSplitter;
    wxThinSplitterWindow       *m_valueSplitter;
    wxSplitterScrolledWindow   *m_scrolledWindow;
    wxButton                   *m_dismissButton;
    wxLuaStackDataWindow       *m_firstValueWindow;
    wxLuaStackDataWindow       *m_secondValueWindow;
    wxLuaInterface             *m_luaInterface;
    wxLuaDebugServer           *m_pServer;
    int                         m_nCurrentSel;

    DECLARE_EVENT_TABLE()

private:

    friend class wxLuaStackDataWindow;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define idSTACK_FRAME             2000
#define idSTACK_COMBOBOX          2001
#define idSCROLLED_WINDOW         2002
#define idSPLITTER_WINDOW         2003
#define idSTACKTREE_CTRL          2004
#define idVALUE_SPLITTER_WINDOW   2005
#define idFIRST_VALUE_WINDOW      2006
#define idSECOND_VALUE_WINDOW     2007
#define idDISMISS_BUTTON          2008

// ----------------------------------------------------------------------------
// wxLuaStackTree
// ----------------------------------------------------------------------------

class wxLuaStackTree: public wxRemotelyScrolledTreeCtrl
{
    DECLARE_CLASS(wxLuaStackTree)
public:
    wxLuaStackTree( wxWindow      *parent,
                    wxWindowID     id,
                    const wxPoint &pt    = wxDefaultPosition,
                    const wxSize  &sz    = wxDefaultSize,
                    long           style = wxTR_HAS_BUTTONS);
    ~wxLuaStackTree();

    void DeleteAllItems();

    DECLARE_EVENT_TABLE()

private:
    wxImageList*    m_imageList;
};

// ----------------------------------------------------------------------------
// wxLuaStackDataWindow
// ----------------------------------------------------------------------------
class wxLuaStackDataWindow : public wxTreeCompanionWindow
{
public:
    wxLuaStackDataWindow( wxWindow      *parent,
                          wxWindowID     id     = -1,
                          const wxPoint &pos    = wxDefaultPosition,
                          const wxSize  &sz     = wxDefaultSize,
                          long           style  = 0,
                          bool           fFirst = true);

// Overrides
    virtual void DrawItem(wxDC &dc, wxTreeItemId id, const wxRect &rect);

private:
    bool    m_fFirst;
    DECLARE_EVENT_TABLE()
};

#endif //_WX_LUA_STACKTREE_H_
