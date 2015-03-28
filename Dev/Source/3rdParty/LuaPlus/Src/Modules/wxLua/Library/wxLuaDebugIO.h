/////////////////////////////////////////////////////////////////////////////
// Purpose:     Implements the debugger end of wxLua debugging session
// Author:      J. Winwood
// Created:     April 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_DEBUG_IO_H
#define WX_LUA_DEBUG_IO_H

#ifdef __GNUG__
    #pragma interface "wxLuaDebugIO.h"
#endif

class wxLuaDebugData;
class wxLuaSocket;

// ----------------------------------------------------------------------------
// wxLuaDebugIO
// ----------------------------------------------------------------------------

class wxLuaDebugIO
{
protected:

    static unsigned char  ReadByte(wxLuaSocket *);
    static short ReadWord(wxLuaSocket *);
    static unsigned short ReadUWord(wxLuaSocket *);
    static int ReadInt(wxLuaSocket *);
    static unsigned int ReadUInt(wxLuaSocket *);
    static long ReadLong(wxLuaSocket *);
    static unsigned long  ReadULong(wxLuaSocket *);
    static wxString ReadString(wxLuaSocket *);
    static wxLuaDebugData *ReadDebugData(wxLuaSocket *);

    static bool WriteByte(wxLuaSocket *, unsigned char value);
    static bool WriteWord(wxLuaSocket *, short value);
    static bool WriteUWord(wxLuaSocket *, unsigned short value);
    static bool WriteInt(wxLuaSocket *, int value);
    static bool WriteUInt(wxLuaSocket *, unsigned int value);
    static bool WriteLong(wxLuaSocket *, long value);
    static bool WriteULong(wxLuaSocket *, unsigned long value);
    static bool WriteString(wxLuaSocket *, const wxString &value);
    static bool WriteDebugData(wxLuaSocket *, const wxLuaDebugData *pData);
};

#endif // WX_LUA_DEBUG_IO_H
