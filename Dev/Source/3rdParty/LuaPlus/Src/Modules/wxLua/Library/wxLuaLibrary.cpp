/////////////////////////////////////////////////////////////////////////////
// Purpose:     Implements wxLuaApp and the wxGetBaseApp function
// Author:      J Winwood
// Created:     14/11/2001
// Modifications: Thanks to Francis Irving for separating out into
//                Library and Standalone modules.
// Copyright:   (c) 2002 Creature Labs. All rights reserved.
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaLibrary.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxLuaLibrary.h"
#include "wxLuaInternals.h"
#include "wxLuaDebugServer.h"
// #include "wxLuaStackTree.h"

// ----------------------------------------------------------------------------
// wxLuaHandler - Allow for Custom Lua Handler (an alternative to wxLuaApp)
// ----------------------------------------------------------------------------

wxLuaHandler* wxLuaHandler::sm_luahandler = NULL;

wxLuaHandler::wxLuaHandler()
{
    wxASSERT(!sm_luahandler);

    sm_luahandler = this;
}

wxLuaHandler::~wxLuaHandler()
{
    sm_luahandler = NULL;
}

wxLuaHandler& wxGetLuaHandler()
{
    return *wxLuaHandler::sm_luahandler;
}

