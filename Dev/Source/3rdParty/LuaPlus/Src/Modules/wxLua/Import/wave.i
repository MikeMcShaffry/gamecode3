///////////////////////////////////////////////////////////////////////////////
//  wxWave

%if wxLUA_USE_wxWave

%win %define	wxSOUND_SYNC
%win %define	wxSOUND_ASYNC
%win %define	wxSOUND_LOOP

%gtk %skip 
%mac %skip 
%class %delete %noclassinfo wxWave, wxObject
    %include "wx/wave.h"
    wxWave(const wxString& fileName, bool isResource = FALSE)
    %constructor wxWaveDefault()
    bool Create(const wxString& fileName, bool isResource = FALSE)
    bool IsOk() const
    bool Play(unsigned flags) const
%endclass

%endif wxLUA_USE_wxWave

