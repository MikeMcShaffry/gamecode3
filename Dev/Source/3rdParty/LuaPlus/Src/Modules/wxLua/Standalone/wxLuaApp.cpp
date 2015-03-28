/////////////////////////////////////////////////////////////////////////////
// Purpose:     Standalone wxLua application
// Author:      Francis Irving
// Created:     16/01/2002
// Modifications: J. Winwood. Bug fixes to original wxLua code.
// RCS-ID:      $Id: wxLuaApp.cpp,v 1.5 2004/03/31 00:41:21 john Exp $
// Copyright:   (c) 2002 Creature Labs. All rights reserved.
// Copyright:   (c) 2001-2004 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaApp.cpp"
    #pragma interface "wxLuaApp.cpp"
#endif

#include "wx/wxprec.h"

#include "LuaPlus/LuaPlus.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/module.h"

#include "wx/splitter.h"
#include "wx/cmdline.h"
#include "wx/fs_mem.h"
#include "../Library/wxLua.h"
#include "wxLuaApp.h"

#if wxCHECK_VERSION(2, 3, 0)
    #include "../Library/wxLuaSplitTree.h"
    #include "../Library/wxLuaStackTree.h"
#endif

extern "C"
{
    #include "lualib.h"
}

extern wxString g_strProgramName;
extern wxString g_strNetworkName;

// ---------------------------------------------------------------------------
// LuaHandler - routes commands from the wxLuaHandler to the app
// ---------------------------------------------------------------------------

class LuaHandler : public wxLuaHandler
{
public:
    LuaHandler(wxLuaStandaloneApp *app) : m_app(app) {}

    virtual void AddPendingEvent(wxEvent& event) { m_app->AddPendingEvent(event); }
    virtual bool ProcessEvent(wxEvent& event) { return m_app->ProcessEvent(event); }
    virtual void SetStackFrame(wxLuaStackFrame *pStackFrame) { m_app->SetStackFrame(pStackFrame); }
    virtual void DisplayError(const wxString &strError) const { m_app->DisplayError(strError); }

public:
    static wxLuaHandler* sm_luahandler;
    wxLuaStandaloneApp *m_app;
};

// ---------------------------------------------------------------------------
// wxLuaException - exception handler for linux
// ---------------------------------------------------------------------------

#ifndef __WXMSW__
class wxLuaException
{
public:
    // constructors
    wxLuaException(int nReturnCode) :
    m_nReturnCode(nReturnCode)
    {
    }

    wxLuaException(const wxLuaException &e) :
    m_nReturnCode(e.m_nReturnCode)
    {
    }

private:
    wxLuaException& operator=(const wxLuaException& e);

private:
    int m_nReturnCode;
};
#endif

// ---------------------------------------------------------------------------
// wxLuaStandaloneApp
// ---------------------------------------------------------------------------

IMPLEMENT_APP_NO_MAIN(wxLuaStandaloneApp)

HANDLE g_hModule;

BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID WXUNUSED(lpReserved))
{
	g_hModule = hModule;

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			wxEntryCleanup();
			break;
	}

	return TRUE;
}

lua_State* globalState;


extern "C" int __declspec(dllexport) luaopen_wxLua(lua_State *L)
{
	globalState = L;
	// remember the parameters Windows gave us
	wxSetInstance((HINSTANCE)g_hModule);
//	wxApp::m_nCmdShow = nCmdShow;

	if ( !wxTheApp )
	{
		wxAppInitializerFunction fnCreate = wxApp::GetInitializerFunction();

		if ( fnCreate )
		{
			// he did, try to create the custom wxApp object
			wxApp::SetInstance((*fnCreate)());
		}
	}

	int argc = 0;
	if ( !wxTheApp->Initialize(argc, NULL) )
	{
		return false;
	}

	if ( !wxTheApp->OnInitGui() )
		return false;

	wxTheApp->SetExitOnFrameDelete(TRUE);

	wxModule::RegisterModules();
	wxModule::InitializeModules();

	if ( !wxTheApp->CallOnInit() )
	{
		// don't call OnExit() if OnInit() failed
		return -1;
	}

/*	wxTheApp->OnRun();
	wxTheApp->OnExit();

	wxEntryCleanup();
*/
	return 0;
}

BEGIN_EVENT_TABLE(wxLuaStandaloneApp, wxApp)
    EVT_LUA_CONSOLE     (-1, wxLuaStandaloneApp::OnLua)
    EVT_LUA_WARNING     (-1, wxLuaStandaloneApp::OnLua)
    EVT_LUA_ERROR       (-1, wxLuaStandaloneApp::OnLua)
    EVT_LUA_DEBUG_HOOK  (-1, wxLuaStandaloneApp::OnLua)
    EVT_LUA_SHUTDOWN    (-1, wxLuaStandaloneApp::OnLua)
    EVT_DEBUG_STACK_ENUM(wxLuaStandaloneApp::OnDebugStackEnum)
    EVT_DEBUG_STACK_ENTRY_ENUM(wxLuaStandaloneApp::OnDebugStackEntryEnum)
    EVT_DEBUG_TABLE_ENUM(wxLuaStandaloneApp::OnDebugTableEnum)
END_EVENT_TABLE()

bool wxLuaStandaloneApp::OnInit()
{
    wxString errorStr;
    int      argCount = 0;
    bool     fOk      = false;
    m_pStackFrame = NULL;

#if defined(__WXMSW__) && wxCHECK_VERSION(2, 3, 3)
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSAStartup(wVersionRequested, &wsaData);
#endif

    g_strProgramName = "wxLua"; //argv[0];
    g_strNetworkName = wxGetHostName();

    m_fMemoryBitmapAdded = false;
#if wxCHECK_VERSION(2, 3, 0)
    m_pDebugTarget = NULL;
#endif
#ifdef __WXMSW__
    m_pConsole = NULL;
#endif

    // Initialize lua
    m_luaInterpreter = new wxLuaInterpreter(globalState, this, -1);
    m_luaState = m_luaInterpreter->GetLuaState();
    m_luaHandler = new LuaHandler(this);

    if (m_luaInterpreter->Ok())
    {
        RegisterClasses(m_luaState);

        if (argc == 1)
        {
			CreateSimpleArgs();

            int rc = RunEditor();

            fOk = wxLuaCheckRunError(rc, &errorStr);
            if (!fOk)
                DisplayError(errorStr);
        }
        else
        // If there is only one command line argument
#if wxCHECK_VERSION(2, 3, 0) || defined(__WXMSW__)
        if (argc == 2 && *argv[1] == wxT('-'))
        {
#if wxCHECK_VERSION(2, 3, 0)
            // Check if we are launched as a debuggee
            if (wxStrlen(argv[1]) > 2 && argv[1][1] == wxT('d'))
            {
                // Set up for debugging

                wxChar *serverPtr = argv[1] + 2;
                wxChar *portPtr   = wxStrrchr(serverPtr, wxT(':'));

                if (portPtr != NULL)
                {
                    *portPtr = 0;
                    int portNumber = wxAtoi(portPtr + 1);

                    m_pDebugTarget = new LuaDebugTarget(m_luaState,
                                                        serverPtr,
                                                        portNumber);
                    if (m_pDebugTarget != NULL)
                        return m_pDebugTarget->Run();
                }
                return FALSE;
            }
#endif
#ifdef __WXMSW__
            // Check if we are to display the console
            if ((wxStrlen(argv[1]) == 2) && (argv[1][1] == wxT('c')))
            {
                wxLuaConsole *pConsole = new wxLuaConsole(this, m_luaState);
                if (pConsole != NULL)
                    pConsole->Show(TRUE);

				CreateSimpleArgs();

                int rc = RunEditor();

                fOk = wxLuaCheckRunError(rc, &errorStr);
                if (!fOk)
                    DisplayError(errorStr);
            }
            else
#endif
            // See if a file has been specified that we are to edit
            if ((wxStrlen(argv[1]) > 2) && (argv[1][1] == wxT('e')))
            {
                wxString fileName = argv[1] + 2;

                lua_newtable(m_luaState);
                lua_pushstring(m_luaState, wx2lua(fileName));
                lua_rawseti(m_luaState, -2, 1);
                lua_pushstring(m_luaState, wx2lua(g_strProgramName));
                lua_rawseti(m_luaState, -2, 0);
                lua_setglobal(m_luaState, "args");

                int rc = RunEditor();

                fOk = wxLuaCheckRunError(rc, &errorStr);
                if (!fOk)
                    DisplayError(errorStr);
            }
            else
            // Otherwise just run the file
            {				
				CreateSimpleArgs();
            	
                if (m_luaInterpreter->RunFile(argv[1]) == 0)
                    fOk = TRUE;
            }
        }
        else
#endif
        // execute the program using argc, argv;
        for (int idx = 1; idx < argc; ++idx)
        {
#ifdef __WXMSW__
            if ((m_pConsole == NULL) && (strlen(argv[idx]) == 2) && (argv[idx][0] == wxT('-')) && (argv[idx][1] == wxT('c')))
            {
                wxLuaConsole *pConsole = new wxLuaConsole(this, m_luaState);
                if (pConsole != NULL)
                    pConsole->Show(TRUE);
            }
            else
#endif
            // See if we are to run the specified file with command-line arguments.
            if (wxStrlen(argv[idx]) > 2 && argv[idx][0] == wxT('-') && argv[idx][1] == wxT('f'))
            {
                wxString fileName = argv[idx] + 2;

                lua_newtable(m_luaState);

                // accumulate the arguments into the 'args' table
                for (++idx; idx < argc; ++idx)
                {
                    lua_pushstring(m_luaState, wx2lua(argv[idx]));
                    lua_rawseti(m_luaState, -2, ++argCount);
                }

                lua_pushstring(m_luaState, wx2lua(fileName));
                lua_rawseti(m_luaState, -2, 0);

                lua_setglobal(m_luaState, "args");

                if (m_luaInterpreter->RunFile(fileName) == 0)
                    fOk = TRUE;

                break;
            }
            else
            // See if a file has been specified that we are to edit
            if (wxStrlen(argv[idx]) > 2 && argv[idx][0] == wxT('-') && argv[idx][1] == wxT('e'))
            {
                wxString fileName = argv[idx] + 2;

                lua_newtable(m_luaState);
                lua_pushstring(m_luaState, wx2lua(fileName));
                lua_rawseti(m_luaState, -2, 1);
                lua_pushstring(m_luaState, wx2lua(g_strProgramName));
                lua_rawseti(m_luaState, -2, 0);
                lua_setglobal(m_luaState, "args");

                int rc = RunEditor();

                fOk = wxLuaCheckRunError(rc, &errorStr);
                if (!fOk)
                    DisplayError(errorStr);

                break;
            }
            else
            {
				CreateSimpleArgs();			
            	
                if (m_luaInterpreter->RunFile(argv[idx]) == 0)
                    fOk = TRUE;
            }
        }
    }
#ifdef __WXMSW__
//    return (m_pConsole != NULL) || fOk;
	return TRUE;
#else
    return fOk;
#endif
}

int wxLuaStandaloneApp::OnExit()
{
#if wxCHECK_VERSION(2, 3, 0)
    // If acting as a debuggee, we have done so disconnect
    // from the debugger.
    if (m_pDebugTarget != NULL)
    {
        m_pDebugTarget->Stop();
        delete m_pDebugTarget;
        m_pDebugTarget = NULL;
    }
#endif

    if (m_fMemoryBitmapAdded)
        wxMemoryFSHandler::RemoveFile(wxT("wxLua"));

    delete m_luaInterpreter;
    delete m_luaHandler;
    m_luaState = NULL;

    wxSafeYield();  // make sure windows get destroyed

    wxApp::OnExit();

#if defined(__WXMSW__) && wxCHECK_VERSION(2, 3, 3)
    WSACleanup ();
#endif

    return 0;
}

void wxLuaStandaloneApp::OnLua( wxLuaEvent &event )
{
    if (event.GetEventType() == wxEVT_LUA_CONSOLE)
    {
#ifdef __WXMSW__
        if (m_pConsole != NULL)
            m_pConsole->DisplayText(event.GetString());
        else
            wxMessageBox(event.GetString(), wxT("wxLua"));
#else
        wxPrintf(wxT("%s\n"), event.GetString().c_str());
#endif
    }
    else if (event.GetEventType() == wxEVT_LUA_ERROR)
    {
        DisplayError(event.GetString());
    }
}

int LUACALL wxLuaStackDialog(lua_State *L)
{
#if wxCHECK_VERSION(2, 3, 0)
    wxLuaInterface li(L);
        wxLuaStackFrame *pStackFrame = new wxLuaStackFrame(NULL,
                                                           wxT("Stack frame"),
                                                           wxDefaultPosition,
                                                           wxSize(500, 250),
                                                           &li);
    pStackFrame->ShowModal();
    pStackFrame->Destroy();
#endif
    return 0;
}

void wxLuaStandaloneApp::RegisterClasses(lua_State *L)
{
    lua_pushcfunction(L, wxLuaStackDialog);
    lua_setglobal(L, "wxLuaStackDialog");
}

int  wxLuaStandaloneApp::RunEditor()
{
    #include "Editor.h"

    wxFileSystem::AddHandler(new wxMemoryFSHandler);

#if defined(__WXGTK__) || (__WXMAC__)
    wxImage::AddHandler(new wxXPMHandler);
    #include "../Examples/bitmaps/wxLuaSmall.xpm"
    wxMemoryFSHandler::AddFile(wxT("wxLua"), wxBITMAP(wxLuaSmall), wxBITMAP_TYPE_XPM);
#else
    wxMemoryFSHandler::AddFile(wxT("wxLua"), wxBITMAP(logo), wxBITMAP_TYPE_BMP);
#endif
    m_fMemoryBitmapAdded = true;

    lua_pushcfunction(m_luaState, CreateStandaloneBitmaps);
    lua_setglobal(m_luaState, "standaloneLoadBitmaps");

    return m_luaInterpreter->RunBuffer(wxLuaEditor, sizeof(wxLuaEditor), g_strProgramName);
}

void wxLuaStandaloneApp::DisplayError(const wxString &errorStr) const
{
    if (m_pDebugTarget != NULL)
        m_pDebugTarget->DisplayError(errorStr);
    else
    {
#ifdef __WXMSW__
        if (m_pConsole != NULL)
            m_pConsole->DisplayText(errorStr, true);
        else
            wxMessageBox(errorStr, wxT("wxLua"));
#else
        fprintf(stderr, wx2lua(errorStr));
#endif
    }
}

void wxLuaStandaloneApp::CreateSimpleArgs()
{
    lua_newtable(m_luaState);
    lua_pushstring(m_luaState, wx2lua(g_strProgramName));
    lua_rawseti(m_luaState, -2, 0);
    lua_setglobal(m_luaState, "args");
}

void wxLuaStandaloneApp::OnDebugStackEnum(wxLuaDebugEvent &event)
{
#if wxCHECK_VERSION(2, 3, 0)
    if (m_pStackFrame != NULL)
        m_pStackFrame->FillStackCombobox(event.GetDebugData());
    else
#endif
        event.Skip();
}

void wxLuaStandaloneApp::OnDebugStackEntryEnum(wxLuaDebugEvent &event)
{
#if wxCHECK_VERSION(2, 3, 0)
    if (m_pStackFrame != NULL)
        m_pStackFrame->FillStackEntry(event.GetReference(), event.GetDebugData());
    else
#endif
        event.Skip();
}

void wxLuaStandaloneApp::OnDebugTableEnum(wxLuaDebugEvent &event)
{
#if wxCHECK_VERSION(2, 3, 0)
    if (m_pStackFrame != NULL)
        m_pStackFrame->FillTableEntry(wxTreeItemId((wxTreeItemIdValue)event.GetReference()), event.GetDebugData());
    else
#endif
        event.Skip();
}
