/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxStyledTextCtrl from contribs
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxStyledTextCtrl

%include "wx/stc/stc.h"

%define wxSTC_INVALID_POSITION
%define wxSTC_START
%define wxSTC_OPTIONAL_START
%define wxSTC_LEXER_START
%define wxSTC_CMD_REDO
%define wxSTC_CMD_SELECTALL
%define wxSTC_WS_INVISIBLE
%define wxSTC_WS_VISIBLEALWAYS
%define wxSTC_WS_VISIBLEAFTERINDENT
%define wxSTC_EOL_CRLF
%define wxSTC_EOL_CR
%define wxSTC_EOL_LF
%define wxSTC_CP_UTF8
%define wxSTC_MARKER_MAX
%define wxSTC_MARK_CIRCLE
%define wxSTC_MARK_ROUNDRECT
%define wxSTC_MARK_ARROW
%define wxSTC_MARK_SMALLRECT
%define wxSTC_MARK_SHORTARROW
%define wxSTC_MARK_EMPTY
%define wxSTC_MARK_ARROWDOWN
%define wxSTC_MARK_MINUS
%define wxSTC_MARK_PLUS
%define wxSTC_MARK_VLINE
%define wxSTC_MARK_LCORNER
%define wxSTC_MARK_TCORNER
%define wxSTC_MARK_BOXPLUS
%define wxSTC_MARK_BOXPLUSCONNECTED
%define wxSTC_MARK_BOXMINUS
%define wxSTC_MARK_BOXMINUSCONNECTED
%define wxSTC_MARK_LCORNERCURVE
%define wxSTC_MARK_TCORNERCURVE
%define wxSTC_MARK_CIRCLEPLUS
%define wxSTC_MARK_CIRCLEPLUSCONNECTED
%define wxSTC_MARK_CIRCLEMINUS
%define wxSTC_MARK_CIRCLEMINUSCONNECTED
%define wxSTC_MARK_BACKGROUND
%define wxSTC_MARK_DOTDOTDOT
%define wxSTC_MARK_ARROWS
%define wxSTC_MARK_CHARACTER
%define wxSTC_MARKNUM_FOLDEREND
%define wxSTC_MARKNUM_FOLDEROPENMID
%define wxSTC_MARKNUM_FOLDERMIDTAIL
%define wxSTC_MARKNUM_FOLDERTAIL
%define wxSTC_MARKNUM_FOLDERSUB
%define wxSTC_MARKNUM_FOLDER
%define wxSTC_MARKNUM_FOLDEROPEN
%define wxSTC_MASK_FOLDERS
%define wxSTC_MARGIN_SYMBOL
%define wxSTC_MARGIN_NUMBER
%define wxSTC_STYLE_DEFAULT
%define wxSTC_STYLE_LINENUMBER
%define wxSTC_STYLE_BRACELIGHT
%define wxSTC_STYLE_BRACEBAD
%define wxSTC_STYLE_CONTROLCHAR
%define wxSTC_STYLE_INDENTGUIDE
%define wxSTC_STYLE_LASTPREDEFINED
%define wxSTC_STYLE_MAX
%define wxSTC_CHARSET_ANSI
%define wxSTC_CHARSET_DEFAULT
%define wxSTC_CHARSET_BALTIC
%define wxSTC_CHARSET_CHINESEBIG5
%define wxSTC_CHARSET_EASTEUROPE
%define wxSTC_CHARSET_GB2312
%define wxSTC_CHARSET_GREEK
%define wxSTC_CHARSET_HANGUL
%define wxSTC_CHARSET_MAC
%define wxSTC_CHARSET_OEM
%define wxSTC_CHARSET_RUSSIAN
%define wxSTC_CHARSET_SHIFTJIS
%define wxSTC_CHARSET_SYMBOL
%define wxSTC_CHARSET_TURKISH
%define wxSTC_CHARSET_JOHAB
%define wxSTC_CHARSET_HEBREW
%define wxSTC_CHARSET_ARABIC
%define wxSTC_CHARSET_VIETNAMESE
%define wxSTC_CHARSET_THAI
%define wxSTC_CASE_MIXED
%define wxSTC_CASE_UPPER
%define wxSTC_CASE_LOWER
%define wxSTC_INDIC_MAX
%define wxSTC_INDIC_PLAIN
%define wxSTC_INDIC_SQUIGGLE
%define wxSTC_INDIC_TT
%define wxSTC_INDIC_DIAGONAL
%define wxSTC_INDIC_STRIKE
%define wxSTC_INDIC0_MASK
%define wxSTC_INDIC1_MASK
%define wxSTC_INDIC2_MASK
%define wxSTC_INDICS_MASK
%define wxSTC_PRINT_NORMAL
%define wxSTC_PRINT_INVERTLIGHT
%define wxSTC_PRINT_BLACKONWHITE
%define wxSTC_PRINT_COLOURONWHITE
%define wxSTC_PRINT_COLOURONWHITEDEFAULTBG
%define wxSTC_FIND_WHOLEWORD
%define wxSTC_FIND_MATCHCASE
%define wxSTC_FIND_WORDSTART
%define wxSTC_FIND_REGEXP
%define wxSTC_CMD_UNDO
%define wxSTC_CMD_CUT
%define wxSTC_CMD_COPY
%define wxSTC_CMD_PASTE
%define wxSTC_FOLDLEVELBASE
%define wxSTC_FOLDLEVELWHITEFLAG
%define wxSTC_FOLDLEVELHEADERFLAG
%define wxSTC_FOLDLEVELNUMBERMASK
%define wxSTC_TIME_FOREVER
%define wxSTC_WRAP_NONE
%define wxSTC_WRAP_WORD
%define wxSTC_CACHE_NONE
%define wxSTC_CACHE_CARET
%define wxSTC_CACHE_PAGE
%define wxSTC_CACHE_DOCUMENT
%define wxSTC_CMD_LINEDOWN
%define wxSTC_CMD_LINEDOWNEXTEND
%define wxSTC_CMD_LINEUP
%define wxSTC_CMD_LINEUPEXTEND
%define wxSTC_CMD_CHARLEFT
%define wxSTC_CMD_CHARLEFTEXTEND
%define wxSTC_CMD_CHARRIGHT
%define wxSTC_CMD_CHARRIGHTEXTEND
%define wxSTC_CMD_WORDLEFT
%define wxSTC_CMD_WORDLEFTEXTEND
%define wxSTC_CMD_WORDRIGHT
%define wxSTC_CMD_WORDRIGHTEXTEND
%define wxSTC_CMD_HOME
%define wxSTC_CMD_HOMEEXTEND
%define wxSTC_CMD_LINEEND
%define wxSTC_CMD_LINEENDEXTEND
%define wxSTC_CMD_DOCUMENTSTART
%define wxSTC_CMD_DOCUMENTSTARTEXTEND
%define wxSTC_CMD_DOCUMENTEND
%define wxSTC_CMD_DOCUMENTENDEXTEND
%define wxSTC_CMD_PAGEUP
%define wxSTC_CMD_PAGEUPEXTEND
%define wxSTC_CMD_PAGEDOWN
%define wxSTC_CMD_PAGEDOWNEXTEND
%define wxSTC_CMD_EDITTOGGLEOVERTYPE
%define wxSTC_CMD_CANCEL
%define wxSTC_CMD_DELETEBACK
%define wxSTC_CMD_TAB
%define wxSTC_CMD_BACKTAB
%define wxSTC_CMD_NEWLINE
%define wxSTC_CMD_FORMFEED
%define wxSTC_CMD_VCHOME
%define wxSTC_CMD_VCHOMEEXTEND
%define wxSTC_CMD_ZOOMIN
%define wxSTC_CMD_ZOOMOUT
%define wxSTC_CMD_DELWORDLEFT
%define wxSTC_CMD_DELWORDRIGHT
%define wxSTC_CMD_LINECUT
%define wxSTC_CMD_LINEDELETE
%define wxSTC_CMD_LINETRANSPOSE
%define wxSTC_CMD_LOWERCASE
%define wxSTC_CMD_UPPERCASE
%define wxSTC_CMD_LINESCROLLDOWN
%define wxSTC_CMD_LINESCROLLUP
%define wxSTC_CMD_DELETEBACKNOTLINE
%define wxSTC_CMD_HOMEDISPLAY
%define wxSTC_CMD_HOMEDISPLAYEXTEND
%define wxSTC_CMD_LINEENDDISPLAY
%define wxSTC_CMD_LINEENDDISPLAYEXTEND
%define wxSTC_EDGE_NONE
%define wxSTC_EDGE_LINE
%define wxSTC_EDGE_BACKGROUND
%define wxSTC_CURSORNORMAL
%define wxSTC_CURSORWAIT
%define wxSTC_VISIBLE_SLOP
%define wxSTC_VISIBLE_STRICT
%define wxSTC_CARET_SLOP
%define wxSTC_CARET_STRICT
%define wxSTC_CARET_JUMPS
%define wxSTC_CARET_EVEN
%define wxSTC_MOD_INSERTTEXT
%define wxSTC_MOD_DELETETEXT
%define wxSTC_MOD_CHANGESTYLE
%define wxSTC_MOD_CHANGEFOLD
%define wxSTC_PERFORMED_USER
%define wxSTC_PERFORMED_UNDO
%define wxSTC_PERFORMED_REDO
%define wxSTC_LASTSTEPINUNDOREDO
%define wxSTC_MOD_CHANGEMARKER
%define wxSTC_MOD_BEFOREINSERT
%define wxSTC_MOD_BEFOREDELETE
%define wxSTC_MODEVENTMASKALL
%define wxSTC_KEY_DOWN
%define wxSTC_KEY_UP
%define wxSTC_KEY_LEFT
%define wxSTC_KEY_RIGHT
%define wxSTC_KEY_HOME
%define wxSTC_KEY_END
%define wxSTC_KEY_PRIOR
%define wxSTC_KEY_NEXT
%define wxSTC_KEY_DELETE
%define wxSTC_KEY_INSERT
%define wxSTC_KEY_ESCAPE
%define wxSTC_KEY_BACK
%define wxSTC_KEY_TAB
%define wxSTC_KEY_RETURN
%define wxSTC_KEY_ADD
%define wxSTC_KEY_SUBTRACT
%define wxSTC_KEY_DIVIDE
%define wxSTC_SCMOD_SHIFT
%define wxSTC_SCMOD_CTRL
%define wxSTC_SCMOD_ALT
%define wxSTC_LEX_CONTAINER
%define wxSTC_LEX_NULL
%define wxSTC_LEX_PYTHON
%define wxSTC_LEX_CPP
%define wxSTC_LEX_HTML
%define wxSTC_LEX_XML
%define wxSTC_LEX_PERL
%define wxSTC_LEX_SQL
%define wxSTC_LEX_VB
%define wxSTC_LEX_PROPERTIES
%define wxSTC_LEX_ERRORLIST
%define wxSTC_LEX_MAKEFILE
%define wxSTC_LEX_BATCH
%define wxSTC_LEX_XCODE
%define wxSTC_LEX_LATEX
%define wxSTC_LEX_LUA
%define wxSTC_LEX_DIFF
%define wxSTC_LEX_CONF
%define wxSTC_LEX_PASCAL
%define wxSTC_LEX_AVE
%define wxSTC_LEX_ADA
%define wxSTC_LEX_LISP
%define wxSTC_LEX_RUBY
%define wxSTC_LEX_EIFFEL
%define wxSTC_LEX_EIFFELKW
%define wxSTC_LEX_TCL
%define wxSTC_LEX_NNCRONTAB
%define wxSTC_LEX_BULLANT
%define wxSTC_LEX_VBSCRIPT
%define wxSTC_LEX_ASP
%define wxSTC_LEX_PHP
%define wxSTC_LEX_BAAN
%define wxSTC_LEX_MATLAB
%define wxSTC_LEX_SCRIPTOL
%define wxSTC_LEX_AUTOMATIC
%define wxSTC_P_DEFAULT
%define wxSTC_P_COMMENTLINE
%define wxSTC_P_NUMBER
%define wxSTC_P_STRING
%define wxSTC_P_CHARACTER
%define wxSTC_P_WORD
%define wxSTC_P_TRIPLE
%define wxSTC_P_TRIPLEDOUBLE
%define wxSTC_P_CLASSNAME
%define wxSTC_P_DEFNAME
%define wxSTC_P_OPERATOR
%define wxSTC_P_IDENTIFIER
%define wxSTC_P_COMMENTBLOCK
%define wxSTC_P_STRINGEOL
%define wxSTC_C_DEFAULT
%define wxSTC_C_COMMENT
%define wxSTC_C_COMMENTLINE
%define wxSTC_C_COMMENTDOC
%define wxSTC_C_NUMBER
%define wxSTC_C_WORD
%define wxSTC_C_STRING
%define wxSTC_C_CHARACTER
%define wxSTC_C_UUID
%define wxSTC_C_PREPROCESSOR
%define wxSTC_C_OPERATOR
%define wxSTC_C_IDENTIFIER
%define wxSTC_C_STRINGEOL
%define wxSTC_C_VERBATIM
%define wxSTC_C_REGEX
%define wxSTC_C_COMMENTLINEDOC
%define wxSTC_C_WORD2
%define wxSTC_C_COMMENTDOCKEYWORD
%define wxSTC_C_COMMENTDOCKEYWORDERROR
%define wxSTC_H_DEFAULT
%define wxSTC_H_TAG
%define wxSTC_H_TAGUNKNOWN
%define wxSTC_H_ATTRIBUTE
%define wxSTC_H_ATTRIBUTEUNKNOWN
%define wxSTC_H_NUMBER
%define wxSTC_H_DOUBLESTRING
%define wxSTC_H_SINGLESTRING
%define wxSTC_H_OTHER
%define wxSTC_H_COMMENT
%define wxSTC_H_ENTITY
%define wxSTC_H_TAGEND
%define wxSTC_H_XMLSTART
%define wxSTC_H_XMLEND
%define wxSTC_H_SCRIPT
%define wxSTC_H_ASP
%define wxSTC_H_ASPAT
%define wxSTC_H_CDATA
%define wxSTC_H_QUESTION
%define wxSTC_H_VALUE
%define wxSTC_H_XCCOMMENT
%define wxSTC_H_SGML_DEFAULT
%define wxSTC_H_SGML_COMMAND
%define wxSTC_H_SGML_1ST_PARAM
%define wxSTC_H_SGML_DOUBLESTRING
%define wxSTC_H_SGML_SIMPLESTRING
%define wxSTC_H_SGML_ERROR
%define wxSTC_H_SGML_SPECIAL
%define wxSTC_H_SGML_ENTITY
%define wxSTC_H_SGML_COMMENT
%define wxSTC_H_SGML_1ST_PARAM_COMMENT
%define wxSTC_H_SGML_BLOCK_DEFAULT
%define wxSTC_HJ_START
%define wxSTC_HJ_DEFAULT
%define wxSTC_HJ_COMMENT
%define wxSTC_HJ_COMMENTLINE
%define wxSTC_HJ_COMMENTDOC
%define wxSTC_HJ_NUMBER
%define wxSTC_HJ_WORD
%define wxSTC_HJ_KEYWORD
%define wxSTC_HJ_DOUBLESTRING
%define wxSTC_HJ_SINGLESTRING
%define wxSTC_HJ_SYMBOLS
%define wxSTC_HJ_STRINGEOL
%define wxSTC_HJ_REGEX
%define wxSTC_HJA_START
%define wxSTC_HJA_DEFAULT
%define wxSTC_HJA_COMMENT
%define wxSTC_HJA_COMMENTLINE
%define wxSTC_HJA_COMMENTDOC
%define wxSTC_HJA_NUMBER
%define wxSTC_HJA_WORD
%define wxSTC_HJA_KEYWORD
%define wxSTC_HJA_DOUBLESTRING
%define wxSTC_HJA_SINGLESTRING
%define wxSTC_HJA_SYMBOLS
%define wxSTC_HJA_STRINGEOL
%define wxSTC_HJA_REGEX
%define wxSTC_HB_START
%define wxSTC_HB_DEFAULT
%define wxSTC_HB_COMMENTLINE
%define wxSTC_HB_NUMBER
%define wxSTC_HB_WORD
%define wxSTC_HB_STRING
%define wxSTC_HB_IDENTIFIER
%define wxSTC_HB_STRINGEOL
%define wxSTC_HBA_START
%define wxSTC_HBA_DEFAULT
%define wxSTC_HBA_COMMENTLINE
%define wxSTC_HBA_NUMBER
%define wxSTC_HBA_WORD
%define wxSTC_HBA_STRING
%define wxSTC_HBA_IDENTIFIER
%define wxSTC_HBA_STRINGEOL
%define wxSTC_HP_START
%define wxSTC_HP_DEFAULT
%define wxSTC_HP_COMMENTLINE
%define wxSTC_HP_NUMBER
%define wxSTC_HP_STRING
%define wxSTC_HP_CHARACTER
%define wxSTC_HP_WORD
%define wxSTC_HP_TRIPLE
%define wxSTC_HP_TRIPLEDOUBLE
%define wxSTC_HP_CLASSNAME
%define wxSTC_HP_DEFNAME
%define wxSTC_HP_OPERATOR
%define wxSTC_HP_IDENTIFIER
%define wxSTC_HPA_START
%define wxSTC_HPA_DEFAULT
%define wxSTC_HPA_COMMENTLINE
%define wxSTC_HPA_NUMBER
%define wxSTC_HPA_STRING
%define wxSTC_HPA_CHARACTER
%define wxSTC_HPA_WORD
%define wxSTC_HPA_TRIPLE
%define wxSTC_HPA_TRIPLEDOUBLE
%define wxSTC_HPA_CLASSNAME
%define wxSTC_HPA_DEFNAME
%define wxSTC_HPA_OPERATOR
%define wxSTC_HPA_IDENTIFIER
%define wxSTC_HPHP_DEFAULT
%define wxSTC_HPHP_HSTRING
%define wxSTC_HPHP_SIMPLESTRING
%define wxSTC_HPHP_WORD
%define wxSTC_HPHP_NUMBER
%define wxSTC_HPHP_VARIABLE
%define wxSTC_HPHP_COMMENT
%define wxSTC_HPHP_COMMENTLINE
%define wxSTC_HPHP_HSTRING_VARIABLE
%define wxSTC_HPHP_OPERATOR
%define wxSTC_PL_DEFAULT
%define wxSTC_PL_ERROR
%define wxSTC_PL_COMMENTLINE
%define wxSTC_PL_POD
%define wxSTC_PL_NUMBER
%define wxSTC_PL_WORD
%define wxSTC_PL_STRING
%define wxSTC_PL_CHARACTER
%define wxSTC_PL_PUNCTUATION
%define wxSTC_PL_PREPROCESSOR
%define wxSTC_PL_OPERATOR
%define wxSTC_PL_IDENTIFIER
%define wxSTC_PL_SCALAR
%define wxSTC_PL_ARRAY
%define wxSTC_PL_HASH
%define wxSTC_PL_SYMBOLTABLE
%define wxSTC_PL_REGEX
%define wxSTC_PL_REGSUBST
%define wxSTC_PL_LONGQUOTE
%define wxSTC_PL_BACKTICKS
%define wxSTC_PL_DATASECTION
%define wxSTC_PL_HERE_DELIM
%define wxSTC_PL_HERE_Q
%define wxSTC_PL_HERE_QQ
%define wxSTC_PL_HERE_QX
%define wxSTC_PL_STRING_Q
%define wxSTC_PL_STRING_QQ
%define wxSTC_PL_STRING_QX
%define wxSTC_PL_STRING_QR
%define wxSTC_PL_STRING_QW
%define wxSTC_B_DEFAULT
%define wxSTC_B_COMMENT
%define wxSTC_B_NUMBER
%define wxSTC_B_KEYWORD
%define wxSTC_B_STRING
%define wxSTC_B_PREPROCESSOR
%define wxSTC_B_OPERATOR
%define wxSTC_B_IDENTIFIER
%define wxSTC_B_DATE
%define wxSTC_PROPS_DEFAULT
%define wxSTC_PROPS_COMMENT
%define wxSTC_PROPS_SECTION
%define wxSTC_PROPS_ASSIGNMENT
%define wxSTC_PROPS_DEFVAL
%define wxSTC_L_DEFAULT
%define wxSTC_L_COMMAND
%define wxSTC_L_TAG
%define wxSTC_L_MATH
%define wxSTC_L_COMMENT
%define wxSTC_LUA_DEFAULT
%define wxSTC_LUA_COMMENT
%define wxSTC_LUA_COMMENTLINE
%define wxSTC_LUA_COMMENTDOC
%define wxSTC_LUA_NUMBER
%define wxSTC_LUA_WORD
%define wxSTC_LUA_STRING
%define wxSTC_LUA_CHARACTER
%define wxSTC_LUA_LITERALSTRING
%define wxSTC_LUA_PREPROCESSOR
%define wxSTC_LUA_OPERATOR
%define wxSTC_LUA_IDENTIFIER
%define wxSTC_LUA_STRINGEOL
%define wxSTC_LUA_WORD2
%define wxSTC_LUA_WORD3
%define wxSTC_LUA_WORD4
%define wxSTC_LUA_WORD5
%define wxSTC_LUA_WORD6
%define wxSTC_ERR_DEFAULT
%define wxSTC_ERR_PYTHON
%define wxSTC_ERR_GCC
%define wxSTC_ERR_MS
%define wxSTC_ERR_CMD
%define wxSTC_ERR_BORLAND
%define wxSTC_ERR_PERL
%define wxSTC_ERR_NET
%define wxSTC_ERR_LUA
%define wxSTC_ERR_CTAG
%define wxSTC_ERR_DIFF_CHANGED
%define wxSTC_ERR_DIFF_ADDITION
%define wxSTC_ERR_DIFF_DELETION
%define wxSTC_ERR_DIFF_MESSAGE
%define wxSTC_BAT_DEFAULT
%define wxSTC_BAT_COMMENT
%define wxSTC_BAT_WORD
%define wxSTC_BAT_LABEL
%define wxSTC_BAT_HIDE
%define wxSTC_BAT_COMMAND
%define wxSTC_BAT_IDENTIFIER
%define wxSTC_BAT_OPERATOR
%define wxSTC_MAKE_DEFAULT
%define wxSTC_MAKE_COMMENT
%define wxSTC_MAKE_PREPROCESSOR
%define wxSTC_MAKE_IDENTIFIER
%define wxSTC_MAKE_OPERATOR
%define wxSTC_MAKE_TARGET
%define wxSTC_MAKE_IDEOL
%define wxSTC_DIFF_DEFAULT
%define wxSTC_DIFF_COMMENT
%define wxSTC_DIFF_COMMAND
%define wxSTC_DIFF_HEADER
%define wxSTC_DIFF_POSITION
%define wxSTC_DIFF_DELETED
%define wxSTC_DIFF_ADDED
%define wxSTC_CONF_DEFAULT
%define wxSTC_CONF_COMMENT
%define wxSTC_CONF_NUMBER
%define wxSTC_CONF_IDENTIFIER
%define wxSTC_CONF_EXTENSION
%define wxSTC_CONF_PARAMETER
%define wxSTC_CONF_STRING
%define wxSTC_CONF_OPERATOR
%define wxSTC_CONF_IP
%define wxSTC_CONF_DIRECTIVE
%define wxSTC_AVE_DEFAULT
%define wxSTC_AVE_COMMENT
%define wxSTC_AVE_NUMBER
%define wxSTC_AVE_WORD
%define wxSTC_AVE_STRING
%define wxSTC_AVE_ENUM
%define wxSTC_AVE_STRINGEOL
%define wxSTC_AVE_IDENTIFIER
%define wxSTC_AVE_OPERATOR
%define wxSTC_ADA_DEFAULT
%define wxSTC_ADA_NUMBER
%define wxSTC_ADA_WORD
%define wxSTC_ADA_STRING
%define wxSTC_ADA_CHARACTER
%define wxSTC_ADA_IDENTIFIER
%define wxSTC_ADA_STRINGEOL
%define wxSTC_BAAN_DEFAULT
%define wxSTC_BAAN_COMMENT
%define wxSTC_BAAN_COMMENTDOC
%define wxSTC_BAAN_NUMBER
%define wxSTC_BAAN_WORD
%define wxSTC_BAAN_STRING
%define wxSTC_BAAN_PREPROCESSOR
%define wxSTC_BAAN_OPERATOR
%define wxSTC_BAAN_IDENTIFIER
%define wxSTC_BAAN_STRINGEOL
%define wxSTC_BAAN_WORD2
%define wxSTC_LISP_DEFAULT
%define wxSTC_LISP_COMMENT
%define wxSTC_LISP_NUMBER
%define wxSTC_LISP_KEYWORD
%define wxSTC_LISP_STRING
%define wxSTC_LISP_STRINGEOL
%define wxSTC_LISP_IDENTIFIER
%define wxSTC_LISP_OPERATOR
%define wxSTC_EIFFEL_DEFAULT
%define wxSTC_EIFFEL_COMMENTLINE
%define wxSTC_EIFFEL_NUMBER
%define wxSTC_EIFFEL_WORD
%define wxSTC_EIFFEL_STRING
%define wxSTC_EIFFEL_CHARACTER
%define wxSTC_EIFFEL_OPERATOR
%define wxSTC_EIFFEL_IDENTIFIER
%define wxSTC_EIFFEL_STRINGEOL
%define wxSTC_NNCRONTAB_DEFAULT
%define wxSTC_NNCRONTAB_COMMENT
%define wxSTC_NNCRONTAB_TASK
%define wxSTC_NNCRONTAB_SECTION
%define wxSTC_NNCRONTAB_KEYWORD
%define wxSTC_NNCRONTAB_MODIFIER
%define wxSTC_NNCRONTAB_ASTERISK
%define wxSTC_NNCRONTAB_NUMBER
%define wxSTC_NNCRONTAB_STRING
%define wxSTC_NNCRONTAB_ENVIRONMENT
%define wxSTC_NNCRONTAB_IDENTIFIER
%define wxSTC_MATLAB_DEFAULT
%define wxSTC_MATLAB_COMMENT
%define wxSTC_MATLAB_COMMAND
%define wxSTC_MATLAB_NUMBER
%define wxSTC_MATLAB_KEYWORD
%define wxSTC_MATLAB_STRING
%define wxSTC_MATLAB_OPERATOR
%define wxSTC_MATLAB_IDENTIFIER
//%define wxSTC_SCRIPTOL_DEFAULT   // FIXME - update for 2.5
//%define wxSTC_SCRIPTOL_COMMENT
//%define wxSTC_SCRIPTOL_COMMENTLINE
//%define wxSTC_SCRIPTOL_COMMENTDOC
//%define wxSTC_SCRIPTOL_NUMBER
//%define wxSTC_SCRIPTOL_WORD
//%define wxSTC_SCRIPTOL_STRING
//%define wxSTC_SCRIPTOL_CHARACTER
//%define wxSTC_SCRIPTOL_UUID
//%define wxSTC_SCRIPTOL_PREPROCESSOR
//%define wxSTC_SCRIPTOL_OPERATOR
//%define wxSTC_SCRIPTOL_IDENTIFIER
//%define wxSTC_SCRIPTOL_STRINGEOL
//%define wxSTC_SCRIPTOL_VERBATIM
//%define wxSTC_SCRIPTOL_REGEX
//%define wxSTC_SCRIPTOL_COMMENTLINEDOC
//%define wxSTC_SCRIPTOL_WORD2
//%define wxSTC_SCRIPTOL_COMMENTDOCKEYWORD
//%define wxSTC_SCRIPTOL_COMMENTDOCKEYWORDERROR
//%define wxSTC_SCRIPTOL_COMMENTBASIC

///////////////////////////////////////////////////////////////////////////////
// wxStyledTextCtrl

%class %noclassinfo wxStyledTextCtrlDocument

%class wxStyledTextCtrl, wxControl
    wxStyledTextCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString &name = "styledtext")
    void AddText(const wxString& text)
    //void AddStyledText(const wxMemoryBuffer& data)
    void InsertText(int pos, const wxString& text)
    void ClearAll()
    void ClearDocumentStyle()
    int GetLength()
    %property=Length, read
    int GetCharAt(int pos)
    %property=CharAt, read
    int GetCurrentPos()
    int GetAnchor()
    int GetStyleAt(int pos)
    void Redo()
    void SetUndoCollection(bool collectUndo)
    void SelectAll()
    void SetSavePoint()
    //wxMemoryBuffer GetStyledText(int startPos, int endPos)
    bool CanRedo()
    int MarkerLineFromHandle(int handle)
    void MarkerDeleteHandle(int handle)
    bool GetUndoCollection()
    int GetViewWhiteSpace()
    void SetViewWhiteSpace(int viewWS)
    %property=ViewWhiteSpace, read, write
    int PositionFromPoint(wxPoint &pt)
    int PositionFromPointClose(int x, int y)
    void GotoLine(int line)
    void GotoPos(int pos)
    void SetAnchor(int posAnchor)
    %property=Anchor, read, write
    wxString GetCurLine(int* linePos = NULL)
    int GetEndStyled()
    %property=EndStyled, read
    void ConvertEOLs(int eolMode)
    int GetEOLMode()
    void SetEOLMode(int eolMode)
    %property=EOLMode, read, write
    void StartStyling(int pos, int mask)
    void SetStyling(int length, int style)
    bool GetBufferedDraw()
    void SetBufferedDraw(bool buffered)
    %property=BufferedDraw, read, write
    void SetTabWidth(int tabWidth)
    int GetTabWidth()
    %property=TabWidth, read, write
    void SetCodePage(int codePage)
    %property=CodePage, read, write
    void MarkerDefine(int markerNumber, int markerSymbol, const wxColour& foreground = wxNullColour, const wxColour& background = wxNullColour)
    void MarkerSetForeground(int markerNumber, const wxColour& fore)
    void MarkerSetBackground(int markerNumber, const wxColour& back)
    int MarkerAdd(int line, int markerNumber)
    void MarkerDelete(int line, int markerNumber)
    void MarkerDeleteAll(int markerNumber)
    int MarkerGet(int line)
    int MarkerNext(int lineStart, int markerMask)
    int MarkerPrevious(int lineStart, int markerMask)
    void SetMarginType(int margin, int marginType)
    int GetMarginType(int margin)
    void SetMarginWidth(int margin, int pixelWidth)
    int GetMarginWidth(int margin)
    void SetMarginMask(int margin, int mask)
    int GetMarginMask(int margin)
    void SetMarginSensitive(int margin, bool sensitive)
    bool GetMarginSensitive(int margin)
    void StyleClearAll()
    void StyleSetForeground(int style, const wxColour& fore)
    void StyleSetBackground(int style, const wxColour& back)
    void StyleSetBold(int style, bool bold)
    void StyleSetItalic(int style, bool italic)
    void StyleSetSize(int style, int sizePoints)
    void StyleSetFaceName(int style, const wxString& fontName)
    void StyleSetEOLFilled(int style, bool filled)
    void StyleResetDefault()
    void StyleSetUnderline(int style, bool underline)
    void StyleSetCase(int style, int caseForce)
    void StyleSetCharacterSet(int style, int characterSet)
    void SetSelForeground(bool useSetting, const wxColour& fore)
    void SetSelBackground(bool useSetting, const wxColour& back)
    void SetCaretForeground(const wxColour& fore)
    void CmdKeyAssign(int key, int modifiers, int cmd)
    void CmdKeyClear(int key, int modifiers)
    void CmdKeyClearAll()
    void SetStyleBytes(int length, char* styleBytes)
    void StyleSetVisible(int style, bool visible)
    int GetCaretPeriod()
    void SetCaretPeriod(int periodMilliseconds)
    %property=CaretPeriod, read, write
    void SetWordChars(const wxString& characters)
    void BeginUndoAction()
    void EndUndoAction()
    void IndicatorSetStyle(int indic, int style)
    int IndicatorGetStyle(int indic)
    void IndicatorSetForeground(int indic, const wxColour& fore)
    wxColour IndicatorGetForeground(int indic)
    void SetWhitespaceForeground(bool useSetting, const wxColour& fore)
    void SetWhitespaceBackground(bool useSetting, const wxColour& back)
    void SetStyleBits(int bits)
    int GetStyleBits()
    %property=StyleBits, read, write
    void SetLineState(int line, int state)
    int GetLineState(int line)
    int GetMaxLineState()
    %property=MaxLineState, read
    bool GetCaretLineVisible()
    void SetCaretLineVisible(bool show)
    %property=CaretLineVisible, read, write
    wxColour GetCaretLineBack()
    void SetCaretLineBack(const wxColour& back)
    %property=CaretLineBack, read, write
    void StyleSetChangeable(int style, bool changeable)
    void AutoCompShow(int lenEntered, const wxString& itemList)
    void AutoCompCancel()
    bool AutoCompActive()
    int AutoCompPosStart()
    void AutoCompComplete()
    void AutoCompStops(const wxString& characterSet)
    void AutoCompSetSeparator(int separatorCharacter)
    int AutoCompGetSeparator()
    void AutoCompSelect(const wxString& text)
    void AutoCompSetCancelAtStart(bool cancel)
    bool AutoCompGetCancelAtStart()
    void AutoCompSetFillUps(const wxString& characterSet)
    void AutoCompSetChooseSingle(bool chooseSingle)
    bool AutoCompGetChooseSingle()
    void AutoCompSetIgnoreCase(bool ignoreCase)
    bool AutoCompGetIgnoreCase()
    void UserListShow(int listType, const wxString& itemList)
    void AutoCompSetAutoHide(bool autoHide)
    bool AutoCompGetAutoHide()
    void AutoCompSetDropRestOfWord(bool dropRestOfWord)
    bool AutoCompGetDropRestOfWord()
    void SetIndent(int indentSize)
    int GetIndent()
    %property=Indent, read, write
    void SetUseTabs(bool useTabs)
    bool GetUseTabs()
    %property=UseTabs, read, write
    void SetLineIndentation(int line, int indentSize)
    int GetLineIndentation(int line)
    int GetLineIndentPosition(int line)
    int GetColumn(int pos)
    void SetUseHorizontalScrollBar(bool show)
    bool GetUseHorizontalScrollBar()
    %property=UseHorizontalScrollBar, read, write
    void SetIndentationGuides(bool show)
    bool GetIndentationGuides()
    %property=IndentationGuides, read, write
    void SetHighlightGuide(int column)
    int GetHighlightGuide()
    %property=HighlightGuide, read, write
    int GetLineEndPosition(int line)
    int GetCodePage()
    wxColour GetCaretForeground()
    bool GetReadOnly()
    void SetCurrentPos(int pos)
    %property=CurrentPos, read, write
    void SetSelectionStart(int pos)
    int GetSelectionStart()
    %property=SelectionStart, read, write
    void SetSelectionEnd(int pos)
    int GetSelectionEnd()
    %property=SelectionEnd, read, write
    void SetPrintMagnification(int magnification)
    int GetPrintMagnification()
    %property=PrintMagnification, read, write
    void SetPrintColourMode(int mode)
    int GetPrintColourMode()
    %property=PrintColourMode, read, write
    int FindText(int minPos, int maxPos, const wxString& text, int flags=0)
    int FormatRange(bool doDraw, int startPos, int endPos, wxDC *draw, wxDC *target, wxRect &renderRect, wxRect &pageRect)
    int GetFirstVisibleLine()
    %property=FirstVisibleLine, read
    wxString GetLine(int line)
    int GetLineCount()
    %property=LineCount, read
    void SetMarginLeft(int pixelWidth)
    int GetMarginLeft()
    %property=MarginLeft, read, write
    void SetMarginRight(int pixelWidth)
    int GetMarginRight()
    %property=MarginRight, read, write
    bool GetModify()
    %property=Modify, read
    void SetSelection(int start, int end)
    wxString GetSelectedText()
    %property=SelectedText, read
    wxString GetTextRange(int startPos, int endPos)
    void HideSelection(bool normal)
    int LineFromPosition(int pos)
    int PositionFromLine(int line)
    void LineScroll(int columns, int lines)
    void EnsureCaretVisible()
    void ReplaceSelection(const wxString& text)
    void SetReadOnly(bool readOnly)
    %property=ReadOnly, read, write
    bool CanPaste()
    bool CanUndo()
    void EmptyUndoBuffer()
    void Undo()
    void Cut()
    void Copy()
    void Paste()
    void Clear()
    void SetText(const wxString& text)
    wxString GetText()
    %property=Text, read, write
    int GetTextLength()
    %property=TextLength, read
    void SetOvertype(bool overtype)
    bool GetOvertype()
    %property=Overtype, read, write
    void SetCaretWidth(int pixelWidth)
    int GetCaretWidth()
    %property=CaretWidth, read, write
    void SetTargetStart(int pos)
    int GetTargetStart()
    %property=TargetStart, read, write
    void SetTargetEnd(int pos)
    int GetTargetEnd()
    %property=TargetEnd, read, write
    int ReplaceTarget(const wxString& text)
    int ReplaceTargetRE(const wxString& text)
    int SearchInTarget(const wxString& text)
    void SetSearchFlags(int flags)
    int GetSearchFlags()
    %property=SearchFlags, read, write
    void CallTipShow(int pos, const wxString& definition)
    void CallTipCancel()
    bool CallTipActive()
    int CallTipPosAtStart()
    void CallTipSetHighlight(int start, int end)
    void CallTipSetBackground(const wxColour& back)
    int VisibleFromDocLine(int line)
    int DocLineFromVisible(int lineDisplay)
    void SetFoldLevel(int line, int level)
    int GetFoldLevel(int line)
    int GetLastChild(int line, int level)
    int GetFoldParent(int line)
    void ShowLines(int lineStart, int lineEnd)
    void HideLines(int lineStart, int lineEnd)
    bool GetLineVisible(int line)
    void SetFoldExpanded(int line, bool expanded)
    bool GetFoldExpanded(int line)
    void ToggleFold(int line)
    void EnsureVisible(int line)
    void SetFoldFlags(int flags)
    void EnsureVisibleEnforcePolicy(int line)
    void SetTabIndents(bool tabIndents)
    bool GetTabIndents()
    %property=TabIndents, read, write
    void SetBackSpaceUnIndents(bool bsUnIndents)
    bool GetBackSpaceUnIndents()
    %property=BackSpaceUnIndents, read, write
    void SetMouseDwellTime(int periodMilliseconds)
    int GetMouseDwellTime()
    %property=MouseDwellTime, read, write
    int WordStartPosition(int pos, bool onlyWordCharacters)
    int WordEndPosition(int pos, bool onlyWordCharacters)
    void SetWrapMode(int mode)
    int GetWrapMode()
    %property=WrapMode, read, write
    void SetLayoutCache(int mode)
    int GetLayoutCache()
    %property=LayoutCache, read, write
    void SetScrollWidth(int pixelWidth)
    int GetScrollWidth()
    %property=ScrollWidth, read, write
    int TextWidth(int style, const wxString& text)
    void SetEndAtLastLine(bool endAtLastLine)
    int GetEndAtLastLine()
    %property=EndAtLastLine, read, write
    int TextHeight(int line)
    void HomeDisplay()
    void HomeDisplayExtend()
    void LineEndDisplay()
    void LineEndDisplayExtend()
    void MoveCaretInsideView()
    int LineLength(int line)
    void BraceHighlight(int pos1, int pos2)
    void BraceBadLight(int pos)
    int BraceMatch(int pos)
    bool GetViewEOL()
    void SetViewEOL(bool visible)
    %property=ViewEOL, read, write
    void* GetDocPointer()
    void SetDocPointer(void* docPointer)
    %property=DocPointer, read, write
    void SetModEventMask(int mask)
    int GetEdgeColumn()
    void SetEdgeColumn(int column)
    %property=EdgeColumn, read, write
    int GetEdgeMode()
    void SetEdgeMode(int mode)
    %property=EdgeMode, read, write
    wxColour GetEdgeColour()
    void SetEdgeColour(const wxColour& edgeColour)
    %property=EdgeColour, read, write
    void SearchAnchor()
    int SearchNext(int flags, const wxString& text)
    int SearchPrev(int flags, const wxString& text)
    int LinesOnScreen()
    void UsePopUp(bool allowPopUp)
    bool SelectionIsRectangle()
    void SetZoom(int zoom)
    int GetZoom()
    %property=Zoom, read, write
    void* CreateDocument()
    void AddRefDocument(void* docPointer)
    void ReleaseDocument(void* docPointer)
    int GetModEventMask()
    %property=ModEventMask, read, write
    void SetSTCFocus(bool focus)
    bool GetSTCFocus()
    %property=STCFocus, read, write
    void SetStatus(int statusCode)
    int GetStatus()
    %property=Status, read, write
    void SetMouseDownCaptures(bool captures)
    bool GetMouseDownCaptures()
    %property=MouseDownCaptures, read, write
    <wxver24 void SetCursor(int cursorType)
    <wxver24 int GetCursor()
    <wxver24 %property=Cursor, read, write
    ~<wxver24 void SetSTCCursor(int cursorType)
    ~<wxver24 int GetSTCCursor()
    ~<wxver24 %property=STCCursor, read, write
    void SetControlCharSymbol(int symbol)
    int GetControlCharSymbol()
    %property=ControlCharSymbol, read, write
    void WordPartLeft()
    void WordPartLeftExtend()
    void WordPartRight()
    void WordPartRightExtend()
    void SetVisiblePolicy(int visiblePolicy, int visibleSlop)
    void DelLineLeft()
    void DelLineRight()
    void SetXOffset(int newOffset)
    int GetXOffset()
    %property=XOffset, read, write
    void SetXCaretPolicy(int caretPolicy, int caretSlop)
    void SetYCaretPolicy(int caretPolicy, int caretSlop)
    void StartRecord()
    void StopRecord()
    void SetLexer(int lexer)
    int GetLexer()
    %property=Lexer, read, write
    void Colourise(int start, int end)
    void SetProperty(const wxString& key, const wxString& value)
    void SetKeyWords(int keywordSet, const wxString& keyWords)
    void SetLexerLanguage(const wxString& language)
    int GetCurrentLine()
    %property=CurrentLine, read
    void StyleSetSpec(int styleNum, const wxString& spec)
    void StyleSetFont(int styleNum, wxFont& font)
    void StyleSetFontAttr(int styleNum, int size, const wxString& faceName, bool bold, bool italic, bool underline)
    void CmdKeyExecute(int cmd)
    void SetMargins(int left, int right)
    void GetSelection(int *startPos, int *endPos)
    wxPoint PointFromPosition(int pos)
    void ScrollToLine(int line)
    void ScrollToColumn(int column)
    long SendMsg(int msg, long wp = 0, long lp = 0)
    void SetVScrollBar(wxScrollBar *bar)
    void SetHScrollBar(wxScrollBar *bar)
    bool GetLastKeydownProcessed()
    void SetLastKeydownProcessed(bool val)
    %property=LastKeydownProcessed, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxStyledTextEvent

%class %delete wxStyledTextEvent, wxCommandEvent
    %define %event wxEVT_STC_CHANGE
    %define %event wxEVT_STC_STYLENEEDED
    %define %event wxEVT_STC_CHARADDED
    %define %event wxEVT_STC_SAVEPOINTREACHED
    %define %event wxEVT_STC_SAVEPOINTLEFT
    %define %event wxEVT_STC_ROMODIFYATTEMPT
    %define %event wxEVT_STC_KEY
    %define %event wxEVT_STC_DOUBLECLICK
    %define %event wxEVT_STC_UPDATEUI
    %define %event wxEVT_STC_MODIFIED
    %define %event wxEVT_STC_MACRORECORD
    %define %event wxEVT_STC_MARGINCLICK
    %define %event wxEVT_STC_NEEDSHOWN
//  %define %event wxEVT_STC_POSCHANGED
    %define %event wxEVT_STC_PAINTED
    %define %event wxEVT_STC_USERLISTSELECTION
    %define %event wxEVT_STC_URIDROPPED
    %define %event wxEVT_STC_DWELLSTART
    %define %event wxEVT_STC_DWELLEND
    %define %event wxEVT_STC_START_DRAG
    %define %event wxEVT_STC_DRAG_OVER
    %define %event wxEVT_STC_DO_DROP
    %define %event wxEVT_STC_ZOOM

    wxStyledTextEvent(wxEventType commandType = 0, int id = 0)
    void SetPosition(int pos)
    void SetKey(int k)
    void SetModifiers(int m)
    void SetModificationType(int t)
    void SetText(const wxString& t)
    void SetLength(int len)
    void SetLinesAdded(int num)
    void SetLine(int val)
    void SetFoldLevelNow(int val)
    void SetFoldLevelPrev(int val)
    void SetMargin(int val)
    void SetMessage(int val)
    void SetWParam(int val)
    void SetLParam(int val)
    void SetListType(int val)
    void SetX(int val)
    void SetY(int val)
    void SetDragText(const wxString& val)
    void SetDragAllowMove(bool val)
    void SetDragResult(wxDragResult val)
    int  GetPosition() const
    %property=Position, read, write
    int  GetKey()  const
    %property=Key, read, write
    int  GetModifiers() const
    %property=Modifiers, read, write
    int  GetModificationType() const
    %property=ModificationType, read, write
    wxString GetText() const
    %property=Text, read, write
    int  GetLength() const
    %property=Length, read, write
    int  GetLinesAdded() const
    %property=LinesAdded, read, write
    int  GetLine() const
    %property=Line, read, write
    int  GetFoldLevelNow() const
    %property=FoldLevelNow, read, write
    int  GetFoldLevelPrev() const
    %property=FoldLevelPrev, read, write
    int  GetMargin() const
    %property=Margin, read, write
    int  GetMessage() const
    %property=Message, read, write
    int  GetWParam() const
    %property=WParam, read, write
    int  GetLParam() const
    %property=LParam, read, write
    int  GetListType() const
    %property=ListType, read, write
    int  GetX() const
    %property=X, read, write
    int  GetY() const
    %property=Y, read, write
    wxString GetDragText()
    %property=DragText, read, write
    bool GetDragAllowMove()
    %property=DragAllowMove, read, write
    wxDragResult GetDragResult()
    %property=DragResult, read, write
    bool GetShift() const
    %property=Shift, read
    bool GetControl() const
    %property=Control, read
    bool GetAlt() const
    %property=Alt, read
%endclass

%endif wxLUA_USE_wxStyledTextCtrl
