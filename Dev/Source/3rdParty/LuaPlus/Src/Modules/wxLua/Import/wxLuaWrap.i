/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaWrap.i
// Purpose:     definition of interface data, functions and classes
// Author:      J Winwood
// Modified by: J Winwood. Automatic generation of event table.
//                         More code generation overrides.
//                         Support for wxWindows 2.3 and wxWindows 2.4
// Created:     3/11/2001
// Copyrights:
// Original file from wxBasic (Wrap.i) (c) 2001 David Cuny
// Modifications for wxLua             (c) 2001, 2002, 2003 J. Winwood
// Portions are extracted from the wxWindows documentation
// and headers therefore
// Portions (c) 2001 wxWindows development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%define wxBACKINGSTORE
%define wxBACKWARD
%define wxBOTH
%define wxBOTTOM
%define wxCANCEL
%define wxCENTER
%define wxCENTER_FRAME
%define wxCENTER_ON_SCREEN
%define wxCENTRE
%define wxCENTRE_ON_SCREEN
%define wxCOLOURED
//%define wxED_BUTTONS_BOTTOM  // for wxExtDialog? not used?
//%define wxED_BUTTONS_RIGHT
//%define wxED_CLIENT_MARGIN
//%define wxED_STATIC_LINE
%define wxEQUIV
%define wxFIXED_LENGTH
%define wxFORWARD
%define wxGTK
%define wxHELP
%define wxHORIZONTAL
%define wxINVERT
%define wxMORE
%define wxMOTIF_X
%define wxNO
%define wxNO_BORDER
%define wxNO_DEFAULT
%define wxOK
%define wxPASSWORD
%define wxPROCESS_ENTER
%define wxRESET
%define wxRESIZE_BOX
%define wxRETAINED
%define wxRIGHT
%define wxSET
%define wxSETUP
%define wxSIZE_ALLOW_MINUS_ONE
%define wxSIZE_AUTO
%define wxSIZE_AUTO_HEIGHT
%define wxSIZE_AUTO_WIDTH
%define wxSIZE_NO_ADJUSTMENTS
%define wxSIZE_USE_EXISTING
%define wxST_NO_AUTORESIZE
%define wxTOP
%define wxUNKNOWN_PLATFORM
//%define wxUSER_COLOURS depricated use wxNO_3D
%define wxVERTICAL
%define wxWINDOWS
%define wxWS_EX_VALIDATE_RECURSIVELY
%define wxXVIEW_X
%define wxYES
%define wxYES_DEFAULT
%define wxYES_NO

%win %define wxPlatformWindows
%gtk %define wxPlatformGTK
%mac %define wxPlatformMac
%mgl %define wxPlatformMgl
%motif %define wxPlatformMotif
%os2 %define wxPlatformOS2
%univ %define wxPlatformUniv
%x11 %define wxPlatformX11
%cocoa %define wxPlatformCocoa

%include "wx/wx.h"
%include "wxLuaInternals.h"

%includefile Overrides.hpp

// classes
%class %noclassinfo wxLog
%endclass
%class %noclassinfo wxProcess
%endclass

//======================================================================================

// These are out of order to ensure proper checking for base classes
%includefile data.i
%includefile event.i
%includefile windows.i
%includefile gdi.i
%includefile appframe.i
%includefile dialogs.i
%includefile print.i
%includefile clipdrag.i
%includefile config.i
%includefile controls.i
%includefile datetime.i
%includefile defsutil.i
%includefile file.i
%includefile grid.i
%includefile help.i
%includefile html.i
%includefile image.i
%includefile mdi.i
%includefile menutool.i
%includefile plot.i
%includefile sizer.i
%includefile socket.i
%includefile geometry.i
%includefile wave.i
%includefile regex.i
%includefile wxlua.i

%includefile fl.i
%includefile stc.i
%includefile xml.i

