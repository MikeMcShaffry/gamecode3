///////////////////////////////////////////////////////////////////////////////
//  wxRegEx - Regular expression support

%if wxLUA_USE_wxRegEx

%include "wx/regex.h"

%class %delete %noclassinfo %encapsulate wxRegEx
    %enum
        wxRE_EXTENDED
        wxRE_BASIC
        wxRE_ICASE
        wxRE_NOSUB
        wxRE_NEWLINE
        wxRE_DEFAULT
    %end

    %enum
        wxRE_NOTBOL
        wxRE_NOTEOL
    %end

    %constructor wxRegExDefault()
    wxRegEx(const wxString& expr, int flags = wxRE_DEFAULT)
    bool Compile(const wxString& pattern, int flags = wxRE_DEFAULT)
    bool IsValid() const
    wxString GetMatch(const wxString& text, size_t index = 0) const
    bool Matches(const wxString &text, int flags = 0) const
    %rename GetMatchIndexes bool GetMatch(size_t* start, size_t* len, size_t index = 0) const
    int Replace(wxString* text, const wxString& replacement, size_t maxMatches = 0) const
    int ReplaceAll(wxString* text, const wxString& replacement) const
    int ReplaceFirst(wxString* text, const wxString& replacement) const
%endclass

%endif wxLUA_USE_wxRegEx

