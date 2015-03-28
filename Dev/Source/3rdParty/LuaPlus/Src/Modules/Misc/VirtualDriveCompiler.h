///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDriveCompiler.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDriveCompiler.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#ifndef VIRTUALDRIVECOMPILER_H
#define VIRTUALDRIVECOMPILER_H

#include "Misc.h"
#include "wx/list.h"

class File;

class VirtualDriveCompiler
{
public:
	VirtualDriveCompiler();
	~VirtualDriveCompiler();

	void SetCompression(bool compressFiles)							{  m_compressFiles = compressFiles;  }
	void SetRecurseDirectories(bool recurseDirectories)				{  m_recurseDirectories = recurseDirectories;  }
	void SetRecreate(bool recreate)									{  m_recreate = recreate;  }
	void SetPackOnChange(bool packOnChange)							{  m_packOnChange = packOnChange;  }
	void SetPackAlways(bool packAlways)								{  m_packAlways = packAlways;  }
	void SetDeleteNonExistentFiles(bool deleteNonExistantFiles)		{  m_deleteNonExistentFiles = deleteNonExistantFiles;  }
	
    void AddIgnoreExtension(wxString extensionToIgnore);
	void AddForceExtension(wxString extensionToForce);
	
    void ProcessFiles(wxString srcDir, wxString origSrcDir, wxString origDestDir);

	DWORD GetFileCRC(File& file, DWORD skipBytes);

protected:
    bool m_compressFiles:1;
    bool m_recurseDirectories:1;
    bool m_recreate:1;
    bool m_packOnChange:1;
    bool m_packAlways:1;
    bool m_deleteNonExistentFiles:1;

    wxArrayString m_ignoreExtensions;
    wxArrayString m_forceExtensions;
};


inline void VirtualDriveCompiler::AddIgnoreExtension(wxString extensionToIgnore)
{
	extensionToIgnore.MakeLower();
	m_ignoreExtensions.Add(extensionToIgnore);
}


inline void VirtualDriveCompiler::AddForceExtension(wxString extensionToForce)
{
	extensionToForce.MakeLower();
	m_forceExtensions.Add(extensionToForce);
}

#endif // VIRTUALDRIVECOMPILER_H
