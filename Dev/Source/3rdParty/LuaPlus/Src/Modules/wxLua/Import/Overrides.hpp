
%override wxPoint_GetX
// int GetX()
static int LUACALL wxPoint_GetX(lua_State *L)
{
    // get this
    wxPoint *self = ( wxPoint * ) getuserdatatype(L, 1, s_wxPoint );
    // push the result number
    lua_pushnumber(L, self->x);
    // return the number of parameters
    return 1;
}
%end

%override wxPoint_GetY
// int GetY()
static int LUACALL wxPoint_GetY(lua_State *L)
{
    // get this
    wxPoint *self = ( wxPoint * ) getuserdatatype(L, 1, s_wxPoint );
    // push the result number
    lua_pushnumber(L, self->y);
    // return the number of parameters
    return 1;
}
%end

%override wxPoint_GetXY
// int GetXY()
static int LUACALL wxPoint_GetXY(lua_State *L)
{
    // get this
    wxPoint *self = ( wxPoint * ) getuserdatatype(L, 1, s_wxPoint );
    // push the result number
    lua_pushnumber(L, self->x);
    lua_pushnumber(L, self->y);
    // return the number of parameters
    return 2;
}
%end

%override wxPoint_SetX
// void SetX(int x)
static int LUACALL wxPoint_SetX(lua_State *L)
{
    // int x
    int x = (int) getnumbertype(L, 2);
    // get this
    wxPoint *self = ( wxPoint * ) getuserdatatype(L, 1, s_wxPoint );
    self->x = x;
    // return the number of parameters
    return 0;
}
%end

%override wxPoint_SetY
// void SetY(int y)
static int LUACALL wxPoint_SetY(lua_State *L)
{
    // int y
    int y = (int) getnumbertype(L, 2);
    // get this
    wxPoint *self = ( wxPoint * ) getuserdatatype(L, 1, s_wxPoint );
    self->y = y;
    // return the number of parameters
    return 0;
}
%end

%override wxPoint_Set
// void Set(int x, int y)
static int LUACALL wxPoint_Set(lua_State *L)
{
    // int y
    int y = (int) getnumbertype(L, 3);
    // int x
    int x = (int) getnumbertype(L, 2);
    // get this
    wxPoint *self = ( wxPoint * ) getuserdatatype(L, 1, s_wxPoint );
    self->x = x;
    self->y = y;
    // return the number of parameters
    return 0;
}
%end

%override wxChoice_constructor
// wxChoice(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxChoiceNameStr)
int LUACALL wxChoice_constructor(lua_State *L)
{
    wxChoice *returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = wxChoiceNameStr
    wxString name = (argCount >= 8 ? lua2wx(lua_tostring(L, 8)) : wxString(wxChoiceNameStr));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 7 ? (wxValidator *) getuserdatatype(L, 7, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 6 ? (long) lua_tonumber(L, 6) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 5 ? tostringarray(L, 5, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 4 ? (wxSize *) getuserdatatype(L, 4, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 3 ? (wxPoint *) getuserdatatype(L, 3, s_wxPoint) : &wxDefaultPosition);
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 2);
    // wxWindow *parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 1, s_wxWindow);
    // call constructor
    returns = new wxChoice(parent, id, *pos, *size, count, choices, style, *validator, name);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxChoice, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxChoice_Create
// bool     Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxChoiceNameStr)
int LUACALL wxChoice_Create(lua_State *L)
{
    bool  returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = wxChoiceNameStr
    wxString name = (argCount >= 9 ? lua2wx(lua_tostring(L, 9)) : wxString(wxChoiceNameStr));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 8 ? (wxValidator *) getuserdatatype(L, 8, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 7 ? (long) lua_tonumber(L, 7) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 6 ? tostringarray(L, 6, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 5 ? (wxSize *) getuserdatatype(L, 5, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 4 ? (wxPoint *) getuserdatatype(L, 4, s_wxPoint) : &wxDefaultPosition);
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 3);
    // wxWindow *parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 2, s_wxWindow);
    // get this
    wxChoice *self = ( wxChoice * ) getuserdatatype(L, 1, s_wxChoice );
    // call Create
    returns = self->Create(parent, id, *pos, *size, count, choices, style, *validator, name);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxComboBox_constructor
// wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")
int LUACALL wxComboBox_constructor(lua_State *L)
{
    wxComboBox *returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = "comboBox"
    wxString name = (argCount >= 9 ? lua2wx(lua_tostring(L, 9)) : wxT("comboBox"));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 8 ? (wxValidator *) getuserdatatype(L, 8, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 7 ? (long) lua_tonumber(L, 7) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 6 ? tostringarray(L, 6, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 5 ? (wxSize *) getuserdatatype(L, 5, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 4 ? (wxPoint *) getuserdatatype(L, 4, s_wxPoint) : &wxDefaultPosition);
    // const wxString& value = ""
    wxString value = (argCount >= 3 ? lua2wx(lua_tostring(L, 3)) : wxT(""));
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 2);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 1, s_wxWindow);
    // call constructor
    returns = new wxComboBox(parent, id, value, *pos, *size, count, choices, style, *validator, name);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxComboBox, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxComboBox_Create
// bool     Create(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")
int LUACALL wxComboBox_Create(lua_State *L)
{
    bool  returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = "comboBox"
    wxString name = (argCount >= 10 ? lua2wx(lua_tostring(L, 10)) : wxT("comboBox"));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 9 ? (wxValidator *) getuserdatatype(L, 9, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 8 ? (long) lua_tonumber(L, 8) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 7 ? tostringarray(L, 7, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 6 ? (wxSize *) getuserdatatype(L, 6, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 5 ? (wxPoint *) getuserdatatype(L, 5, s_wxPoint) : &wxDefaultPosition);
    // const wxString& value = ""
    wxString value = (argCount >= 4 ? lua2wx(lua_tostring(L, 4)) : wxT(""));
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 3);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 2, s_wxWindow);
    // get this
    wxComboBox *self = ( wxComboBox * ) getuserdatatype(L, 1, s_wxComboBox );
    // call Create
    returns = self->Create(parent, id, value, *pos, *size, count, choices, style, *validator, name);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxListBox_constructor
// wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
int LUACALL wxListBox_constructor(lua_State *L)
{
    wxListBox *returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = "listBox"
    wxString name = (argCount >= 8 ? lua2wx(lua_tostring(L, 8)) : wxT("listBox"));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 7 ? (wxValidator *) getuserdatatype(L, 7, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 6 ? (long) lua_tonumber(L, 6) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 5 ? tostringarray(L, 5, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 4 ? (wxSize *) getuserdatatype(L, 4, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 3 ? (wxPoint *) getuserdatatype(L, 3, s_wxPoint) : &wxDefaultPosition);
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 2);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 1, s_wxWindow);
    // call constructor
    returns = new wxListBox(parent, id, *pos, *size, count, choices, style, *validator, name);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxListBox, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxListBox_Create
// bool     Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
int LUACALL wxListBox_Create(lua_State *L)
{
    bool  returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = "listBox"
    wxString name = (argCount >= 9 ? lua2wx(lua_tostring(L, 9)) : wxT("listBox"));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 8 ? (wxValidator *) getuserdatatype(L, 8, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 7 ? (long) lua_tonumber(L, 7) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 6 ? tostringarray(L, 6, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 5 ? (wxSize *) getuserdatatype(L, 5, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 4 ? (wxPoint *) getuserdatatype(L, 4, s_wxPoint) : &wxDefaultPosition);
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 3);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 2, s_wxWindow);
    // get this
    wxListBox *self = ( wxListBox * ) getuserdatatype(L, 1, s_wxListBox );
    // call Create
    returns = self->Create(parent, id, *pos, *size, count, choices, style, *validator, name);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxCheckListBox_constructor
// wxCheckListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
int LUACALL wxCheckListBox_constructor(lua_State *L)
{
    wxCheckListBox *returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = "listBox"
    wxString name = (argCount >= 8 ? lua2wx(lua_tostring(L, 8)) : wxT("listBox"));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 7 ? (wxValidator *) getuserdatatype(L, 7, s_wxValidator) : &wxDefaultValidator);
    // long style = 0
    long style = (argCount >= 6 ? (long) lua_tonumber(L, 6) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 5 ? tostringarray(L, 5, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 4 ? (wxSize *) getuserdatatype(L, 4, s_wxSize) : &wxDefaultSize);
    // const wxPoint& pos = wxDefaultPosition
    const wxPoint *pos = (argCount >= 3 ? (wxPoint *) getuserdatatype(L, 3, s_wxPoint) : &wxDefaultPosition);
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 2);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 1, s_wxWindow);
    // call constructor
    returns = new wxCheckListBox(parent, id, *pos, *size, count, choices, style, *validator, name);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxCheckListBox, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxRadioBox_constructor
// wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")
int LUACALL wxRadioBox_constructor(lua_State *L)
{
    wxRadioBox *returns;
    // get number of arguments
    int   argCount = lua_gettop(L);
    // const wxString& name = "radioBox"
    wxString name = (argCount >= 10 ? lua2wx(lua_tostring(L, 10)) : wxT("radioBox"));
    // const wxValidator& validator = wxDefaultValidator
    const wxValidator *validator = (argCount >= 9 ? (wxValidator *) getuserdatatype(L, 9, s_wxValidator) : &wxDefaultValidator);
    // long style = wxRA_SPECIFY_COLS
    long style = (argCount >= 8 ? (long) lua_tonumber(L, 8) : wxRA_SPECIFY_COLS);
    // int majorDimension = 0
    int majorDimension = (argCount >= 7 ? (int) lua_tonumber(L, 7) : 0);
    // const wxString choices[] = NULL
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = (argCount >= 6 ? tostringarray(L, 6, count) : NULL);
    // const wxSize& size = wxDefaultSize
    const wxSize *size = (argCount >= 5 ? (wxSize *) getuserdatatype(L, 5, s_wxSize) : &wxDefaultSize);
    // const wxPoint& point = wxDefaultPosition
    const wxPoint *point = (argCount >= 4 ? (wxPoint *) getuserdatatype(L, 4, s_wxPoint) : &wxDefaultPosition);
    // const wxString& label
    wxString label = lua2wx(lua_tostring(L, 3));
    // wxWindowID id
    wxWindowID id = (wxWindowID) lua_tonumber(L, 2);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 1, s_wxWindow);
    // call constructor
    returns = new wxRadioBox(parent, id, label, *point, *size, count, choices, majorDimension, style, *validator, name);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxRadioBox, returns );
    // return the number of parameters
    return 1;
}
%end

%override builtin_wxDisplaySize
// %builtin void wxDisplaySize(int *width, int *height)
int LUACALL builtin_wxDisplaySize(lua_State *L)
{
    int height, width;
    // call wxDisplaySize
    wxDisplaySize(&width, &height);
    // return the number of parameters
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    return 2;
}
%end

%override builtin_wxDisplaySizeMM
// %builtin void wxDisplaySizeMM(int *width, int *height)
static int LUACALL builtin_wxDisplaySizeMM(lua_State *L)
{
    int height, width;
    // call wxDisplaySizeMM
    wxDisplaySizeMM(&width, &height);
    // return the number of parameters
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    return 2;
}
%end

%override wxWindow_ClientToScreen
// virtual void ClientToScreen(int* x, int* y) const
int LUACALL wxWindow_ClientToScreen(lua_State *L)
{
    int y = (int) lua_tonumber(L,  3);
    int x = (int) lua_tonumber(L,  2);
    // get this
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call ClientToScreen
    self->ClientToScreen(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override builtin_wxClientDisplayRect
// %builtin void wxClientDisplayRect(int *x, int *y,int *width, int *height)
static int LUACALL builtin_wxClientDisplayRect(lua_State *L)
{
    int height;
    int width;
    int y;
    int x;
    // call wxClientDisplayRect
    wxClientDisplayRect(&x, &y, &width, &height);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 4;
}
%end

%override wxDC_GetSize
// void GetSize(wxCoord *width, wxCoord *height)
int LUACALL wxDC_GetSize(lua_State *L)
{
    wxCoord width;
    wxCoord height;
    // get this
    wxDC *self = ( wxDC * ) getuserdatatype(L, 1, s_wxDC );
    // call ClientToScreen
    self->GetSize(&width, &height);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 2;
}
%end

%override wxDC_GetUserScale
// void GetUserScale(double *x, double *y)
static int LUACALL wxDC_GetUserScale(lua_State *L)
{
    double y;
    double x;
    // get this
    wxDC *self = ( wxDC * ) getuserdatatype(L, 1, s_wxDC );
    // call GetUserScale
    self->GetUserScale(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxWindow_GetSize
// void GetSize(int *width, int *height)
int LUACALL wxWindow_GetSize(lua_State *L)
{
    int width;
    int height;
    // get this
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call ClientToScreen
    self->GetSize(&width, &height);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 2;
}
%end

%override wxWindow_GetPosition
// virtual void GetPosition(int* x, int* y) const
static int LUACALL wxWindow_GetPosition(lua_State *L)
{
    int y;
    int x;
    // get this
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call GetPosition
    self->GetPosition(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxWindow_GetClientSize
// virtual wxSize GetClientSize() const
static int LUACALL wxWindow_GetClientSize(lua_State *L)
{
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call GetClientSize
    wxSize size = self->GetClientSize();
    lua_pushnumber(L, size.x);
    lua_pushnumber(L, size.y);
    // return the number of parameters
    return 2;
}
%end

%override wxMDIParentFrame_GetClientSize
// virtual void GetClientSize(int* width, int* height) const
static int LUACALL wxMDIParentFrame_GetClientSize(lua_State *L)
{
    int height;
    int width;
    wxMDIParentFrame *self = ( wxMDIParentFrame * ) getuserdatatype(L, 1, s_wxMDIParentFrame );
    // call GetClientSize
    self->GetClientSize(&width, &height);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 2;
}
%end

%override wxScrolledWindow_GetScrollPixelsPerUnit
// void GetScrollPixelsPerUnit(int* xUnit, int* yUnit) const
static int LUACALL wxScrolledWindow_GetScrollPixelsPerUnit(lua_State *L)
{
    int yUnit;
    int xUnit;
    // get this
    wxScrolledWindow *self = ( wxScrolledWindow * ) getuserdatatype(L, 1, s_wxScrolledWindow );
    // call GetScrollPixelsPerUnit
    self->GetScrollPixelsPerUnit(&xUnit, &yUnit);
    lua_pushnumber(L, xUnit);
    lua_pushnumber(L, yUnit);
    // return the number of parameters
    return 2;
}
%end

%override wxScrolledWindow_GetVirtualSize
// void GetVirtualSize(int* x, int* y) const
static int LUACALL wxScrolledWindow_GetVirtualSize(lua_State *L)
{
    int y;
    int x;
    // get this
    wxScrolledWindow *self = ( wxScrolledWindow * ) getuserdatatype(L, 1, s_wxScrolledWindow );
    // call GetVirtualSize
    self->GetVirtualSize(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxScrolledWindow_GetViewStart
// void GetViewStart(int* x, int* y) const
static int LUACALL wxScrolledWindow_GetViewStart(lua_State *L)
{
    int y;
    int x;
    // get this
    wxScrolledWindow *self = ( wxScrolledWindow * ) getuserdatatype(L, 1, s_wxScrolledWindow );
    // call GetViewStart
    self->GetViewStart(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxTextCtrl_GetSelection
// virtual void GetSelection(long* from, long* to)
static int LUACALL wxTextCtrl_GetSelection(lua_State *L)
{
    long to;
    long from;
    // get this
    wxTextCtrl *self = ( wxTextCtrl * ) getuserdatatype(L, 1, s_wxTextCtrl );
    // call GetSelection
    self->GetSelection(&from, &to);
    lua_pushnumber(L, from);
    lua_pushnumber(L, to);
    // return the number of parameters
    return 2;
}
%end

%override wxDC_GetTextExtent
// void GetTextExtent(const wxString& string, wxCoord *w, wxCoord *h, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL)
static int LUACALL wxDC_GetTextExtent(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxFont *font = NULL
    wxFont *font = (argCount >= 3 ? (wxFont *) getuserdatatype(L, 3, s_wxFont) : NULL);

    wxCoord externalLeading;
    wxCoord descent;
    wxCoord h;
    wxCoord w;

    wxString string = lua2wx(lua_tostring(L, 2));
    // get this
    wxDC *self = ( wxDC * ) getuserdatatype(L, 1, s_wxDC );
    // call GetTextExtent
    self->GetTextExtent(string, &w, &h, &descent, &externalLeading, font);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    lua_pushnumber(L, descent);
    lua_pushnumber(L, externalLeading);
    // return the number of parameters
    return 4;
}
%end

%override wxDC_GetClippingBox
// void GetClippingBox(wxCoord *x, wxCoord *y, wxCoord *width, wxCoord *height)
static int LUACALL wxDC_GetClippingBox(lua_State *L)
{
    wxCoord height;
    wxCoord width;
    wxCoord y;
    wxCoord x;
    // get this
    wxDC *self = ( wxDC * ) getuserdatatype(L, 1, s_wxDC );
    // call GetClippingBox
    self->GetClippingBox(&x, &y, &width, &height);
    // push results
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 4;
}
%end

%override wxKeyEvent_GetPositionXY
static int LUACALL wxKeyEvent_GetPositionXY(lua_State *L)
{
    wxCoord y;
    wxCoord x;
    // get this
    wxKeyEvent *self = ( wxKeyEvent * ) getuserdatatype(L, 1, s_wxKeyEvent );
    // call GetPositionXY
    self->GetPosition(&x, &y);
    // push results
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxMouseEvent_GetPositionXY
// void GetPositionXY(wxCoord* x, wxCoord* y) const
static int LUACALL wxMouseEvent_GetPositionXY(lua_State *L)
{
    wxCoord y;
    wxCoord x;
    // get this
    wxMouseEvent *self = ( wxMouseEvent * ) getuserdatatype(L, 1, s_wxMouseEvent );
    // call GetPosition
    self->GetPosition(&x, &y);
    // push results
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxEvtHandler_ConnectEvent
// void ConnectEvent()
// body is in wxLuaInternals.cpp
extern int LUACALL wxEvtHandler_ConnectEvent(lua_State *L);

%end

%override wxObject_DynamicCast
// void *DynamicCast(const char *class)
// body is in wxLuaInternals.cpp
extern int LUACALL wxObject_DynamicCast(lua_State *L);

%end

%override wxRegion_GetBoxCoords
// %rename GetBoxCoords void GetBox(int &x, int &y, int &width, int &height)
static int LUACALL wxRegion_GetBoxCoords(lua_State *L)
{
    int height;
    int width;
    int y;
    int x;
    // get this
    wxRegion *self = ( wxRegion * ) getuserdatatype(L, 1, s_wxRegion );
    // call GetBox
    self->GetBox(x, y, width, height);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 4;
}
%end

%override wxStyledTextEvent_SetDragResult
// void SetDragResult(wxDragResult val)
static int LUACALL wxStyledTextEvent_SetDragResult(lua_State *L)
{
    // wxDragResult val
    wxDragResult val = (wxDragResult)(int) lua_tonumber(L, 2);
    // get this
    wxStyledTextEvent *self = ( wxStyledTextEvent * ) getuserdatatype(L, 1, s_wxStyledTextEvent );
    // call SetDragResult
    self->SetDragResult(val);
    // return the number of parameters
    return 0;
}
%end


%override wxGrid_GetRowLabelAlignment
// void    GetRowLabelAlignment( int *horz, int *vert )
static int LUACALL wxGrid_GetRowLabelAlignment(lua_State *L)
{
    int vert;
    int horz;
    // get this
    wxGrid *self = ( wxGrid * ) getuserdatatype(L, 1, s_wxGrid );
    // call GetRowLabelAlignment
    self->GetRowLabelAlignment(&horz, &vert);
    // push results
    lua_pushnumber(L, horz);
    lua_pushnumber(L, vert);
    // return the number of parameters
    return 2;
}
%end

%override wxGrid_GetColLabelAlignment
// void    GetColLabelAlignment( int *horz, int *vert )
static int LUACALL wxGrid_GetColLabelAlignment(lua_State *L)
{
    int vert;
    int horz;
    // get this
    wxGrid *self = ( wxGrid * ) getuserdatatype(L, 1, s_wxGrid );
    // call GetColLabelAlignment
    self->GetColLabelAlignment(&horz, &vert);
    // push results
    lua_pushnumber(L, horz);
    lua_pushnumber(L, vert);
    // return the number of parameters
    return 2;
}
%end

%override wxGrid_GetDefaultCellAlignment
// void    GetDefaultCellAlignment( int *horiz, int *vert )
static int LUACALL wxGrid_GetDefaultCellAlignment(lua_State *L)
{
    int vert;
    int horiz;
    // get this
    wxGrid *self = ( wxGrid * ) getuserdatatype(L, 1, s_wxGrid );
    // call GetDefaultCellAlignment
    self->GetDefaultCellAlignment(&horiz, &vert);
    // push results
    lua_pushnumber(L, horiz);
    lua_pushnumber(L, vert);
    // return the number of parameters
    return 2;
}
%end

%override wxGrid_GetCellAlignment
// void    GetCellAlignment( int row, int col, int *horiz, int *vert )
static int LUACALL wxGrid_GetCellAlignment(lua_State *L)
{
    int vert;
    int horiz;
    // int col
    int col = (int) lua_tonumber(L, 3);
    // int row
    int row = (int) lua_tonumber(L, 2);
    // get this
    wxGrid *self = ( wxGrid * ) getuserdatatype(L, 1, s_wxGrid );
    // call GetCellAlignment
    self->GetCellAlignment(row, col, &horiz, &vert);
    // push results
    lua_pushnumber(L, horiz);
    lua_pushnumber(L, vert);
    // return the number of parameters
    return 2;
}
%end

%override wxStyledTextCtrl_GetCurLine
// wxString GetCurLine(int* linePos=NULL)
static int LUACALL wxStyledTextCtrl_GetCurLine(lua_State *L)
{
    wxString returns;
    int linePos;
    // get this
    wxStyledTextCtrl *self = ( wxStyledTextCtrl * ) getuserdatatype(L, 1, s_wxStyledTextCtrl );
    // call GetCurLine
    returns = self->GetCurLine(&linePos);
    // push the result string
    lua_pushstring(L, wx2lua(returns) );
    lua_pushnumber(L, linePos);
    // return the number of parameters
    return 2;
}
%end

%override wxStyledTextCtrl_GetSelection
// void GetSelection(int* startPos, int* endPos)
static int LUACALL wxStyledTextCtrl_GetSelection(lua_State *L)
{
    int endPos;
    int startPos;
    // get this
    wxStyledTextCtrl *self = ( wxStyledTextCtrl * ) getuserdatatype(L, 1, s_wxStyledTextCtrl );
    // call GetSelection
    self->GetSelection(&startPos, &endPos);
    // push results
    lua_pushnumber(L, startPos);
    lua_pushnumber(L, endPos);
    // return the number of parameters
    return 2;
}
%end

%override wxStyledTextCtrl_SetStyleBytes
// void    SetStyleBytes(int length, const wxString &styleBytes)
static int LUACALL wxStyledTextCtrl_SetStyleBytes(lua_State *L)
{
    // const wxString &styleBytes
    wxCharBuffer styleBytes(lua_tostring(L, 3));
    // int length
    int length = (int) lua_tonumber(L, 2);
    // get this
    wxStyledTextCtrl *self = ( wxStyledTextCtrl * ) getuserdatatype(L, 1, s_wxStyledTextCtrl );
    // call SetStyleBytes
    self->SetStyleBytes(length, styleBytes.data());
    // push result
    lua_pushstring(L, styleBytes);
    // return the number of parameters
    return 1;
}
%end

%override wxStyledTextEvent_GetDragResult
// wxDragResult GetDragResult()
static int LUACALL wxStyledTextEvent_GetDragResult(lua_State *L)
{
    wxDragResult returns;
    // get this
    wxStyledTextEvent *self = ( wxStyledTextEvent * ) getuserdatatype(L, 1, s_wxStyledTextEvent );
    // call GetDragResult
    returns = self->GetDragResult();
    // push the result datatype
    lua_pushnumber(L, (long) returns );
    // return the number of parameters
    return 1;
}
%end

%override wxFrame_SetStatusWidths
// virtual void SetStatusWidths(int n, int *widths)
static int LUACALL wxFrame_SetStatusWidths(lua_State *L)
{
    int count = 0; SmartIntArray ptr; int *choices; ptr = choices = tointarray(L, 2, count);
    // get this
    wxFrame *self = ( wxFrame * ) getuserdatatype(L, 1, s_wxFrame );
    // call SetStatusWidths
    self->SetStatusWidths(count, choices);
    // return the number of parameters
    return 0;
}
%end

%override wxStatusBar_SetStatusWidths
// virtual void SetStatusWidths(int n, int *widths)
static int LUACALL wxStatusBar_SetStatusWidths(lua_State *L)
{
    int count = 0; SmartIntArray ptr; int *choices; ptr = choices = tointarray(L, 2, count);
    // get this
    wxStatusBar *self = ( wxStatusBar * ) getuserdatatype(L, 1, s_wxStatusBar );
    // call SetStatusWidths
    self->SetStatusWidths(count, choices);
    // return the number of parameters
    return 0;
}
%end

%override wxLuaObject_constructor
// wxLuaObject(void *object)
static int LUACALL wxLuaObject_constructor(lua_State *L)
{
    wxLuaObject *returns;
    // call constructor
    returns = new wxLuaObject(L, 1);
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxLuaObject, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxLuaObject_SetObject
// void SetObject(void *object)
static int LUACALL wxLuaObject_SetObject(lua_State *L)
{
    // get this
    wxLuaObject *self = ( wxLuaObject * ) getuserdatatype(L, 1, s_wxLuaObject );
    // call SetObject
    self->SetObject(1);
    // return the number of parameters
    return 0;
}
%end

%override wxLuaObject_GetObject
// void *GetObject() const
static int LUACALL wxLuaObject_GetObject(lua_State *L)
{
    // get this
    wxLuaObject *self = ( wxLuaObject * ) getuserdatatype(L, 1, s_wxLuaObject );
    // call GetObject
    if (self->GetObject())
        return 1;
    return 0;
}
%end

// wxObject *GetClientData(int n) const
%override wxListBox_GetClientData
static int LUACALL wxListBox_GetClientData(lua_State *L)
{
    wxObject *returns;
    // int n
    int n = (int) getnumbertype(L, 2);
    // get this
    wxListBox *self = ( wxListBox * ) getuserdatatype(L, 1, s_wxListBox );
    // call GetClientData
    returns = (wxObject *) self->GetClientData(n);
    // push the result datatype
    pushuserdatatype(L, s_wxObject, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxCommandEvent_GetClientData
// wxObject* GetClientData()
static int LUACALL wxCommandEvent_GetClientData(lua_State *L)
{
    wxObject *returns;
    // get this
    wxCommandEvent *self = ( wxCommandEvent * ) getuserdatatype(L, 1, s_wxCommandEvent );
    // call GetClientData
    returns = (wxObject *) self->GetClientData();
    // push the result datatype
    pushuserdatatype(L, s_wxObject, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxSocketEvent_GetClientData
// wxObject * GetClientData()
static int LUACALL wxSocketEvent_GetClientData(lua_State *L)
{
    wxObject *returns;
    // get this
    wxSocketEvent *self = ( wxSocketEvent * ) getuserdatatype(L, 1, s_wxSocketEvent );
    // call GetClientData
    returns = (wxObject *) self->GetClientData();
    // push the result datatype
    pushuserdatatype(L, s_wxObject, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxTextValidator_constructor

class wxLuaTextValidator : public wxTextValidator
{
    public:
        wxLuaTextValidator(long style, const wxString& valPtr)
            : wxTextValidator(style, &valString),
	      valString(valPtr)
        {
        }

        wxLuaTextValidator(long style)
            : wxTextValidator(style)
        {
        }

    private:
        wxString valString;
};

// wxTextValidator(long style = wxFILTER_NONE, wxString *valPtr = NULL)
static int LUACALL wxTextValidator_constructor(lua_State *L)
{
    wxTextValidator *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // long style = wxFILTER_NONE
    long style = (argCount >= 1 ? (long) getnumbertype(L, 1) : wxFILTER_NONE);
    // call constructor
    if (argCount >= 2)
        returns = new wxLuaTextValidator(style, lua2wx(getstringtype(L, 2)));
    else
        returns = new wxLuaTextValidator(style);

    // push the constructed class pointer
    pushuserdatatype(L, s_wxTextValidator, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxTextValidator_GetExcludeList
// wxStringList& GetExcludeList() const
static int LUACALL wxTextValidator_GetExcludeList(lua_State *L)
{
    wxStringList *returns;
    // get this
    wxTextValidator *self = ( wxTextValidator * ) getuserdatatype(L, 1, s_wxTextValidator );
    // call GetExcludeList
    returns = &self->GetExcludeList();
    // push the result datatype
    pushuserdatatype(L, s_wxStringList, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxTextValidator_GetIncludeList
// wxStringList& GetIncludeList() const
static int LUACALL wxTextValidator_GetIncludeList(lua_State *L)
{
    wxStringList *returns;
    // get this
    wxTextValidator *self = ( wxTextValidator * ) getuserdatatype(L, 1, s_wxTextValidator );
    // call GetIncludeList
    returns = &self->GetIncludeList();
    // push the result datatype
    pushuserdatatype(L, s_wxStringList, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxTextValidator_SetExcludeList
// void SetExcludeList(const wxStringList& stringList)
static int LUACALL wxTextValidator_SetExcludeList(lua_State *L)
{
    // const wxStringList& stringList
    const wxStringList *stringList = (wxStringList *) getuserdatatype(L, 2, s_wxStringList);
    // get this
    wxTextValidator *self = ( wxTextValidator * ) getuserdatatype(L, 1, s_wxTextValidator );
    // call SetExcludeList
    self->SetExcludeList(*stringList);
    // return the number of parameters
    return 0;
}
%end

%override wxTextValidator_SetIncludeList
// void SetIncludeList(const wxStringList& stringList)
static int LUACALL wxTextValidator_SetIncludeList(lua_State *L)
{
    // const wxStringList& stringList
    const wxStringList *stringList = (wxStringList *) getuserdatatype(L, 2, s_wxStringList);
    // get this
    wxTextValidator *self = ( wxTextValidator * ) getuserdatatype(L, 1, s_wxTextValidator );
    // call SetIncludeList
    self->SetIncludeList(*stringList);
    // return the number of parameters
    return 0;
}
%end

%override wxGenericValidatorBool_constructor
// %constructor wxGenericValidatorBool(wxLuaObject *boolPtr)
static int LUACALL wxGenericValidatorBool_constructor(lua_State *L)
{
    wxGenericValidator *returns;
    // wxLuaObject *boolPtr
    wxLuaObject *boolPtr = (wxLuaObject *) getuserdatatype(L, 1, s_wxLuaObject);
    // call constructor
    returns = new wxGenericValidator(boolPtr->GetBoolPtr());
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxGenericValidator, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxGenericValidatorString_constructor
// %constructor wxGenericValidatorString(wxLuaObject *valPtr)
static int LUACALL wxGenericValidatorString_constructor(lua_State *L)
{
    wxGenericValidator *returns;
    // wxLuaObject *valPtr
    wxLuaObject *valPtr = (wxLuaObject *) getuserdatatype(L, 1, s_wxLuaObject);
    // call constructor
    returns = new wxGenericValidator(valPtr->GetStringPtr());
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxGenericValidator, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxGenericValidatorInt_constructor
// %constructor wxGenericValidatorInt(wxLuaObject *valPtr)
static int LUACALL wxGenericValidatorInt_constructor(lua_State *L)
{
    wxGenericValidator *returns;
    // wxLuaObject *valPtr
    wxLuaObject *valPtr = (wxLuaObject *) getuserdatatype(L, 1, s_wxLuaObject);
    // call constructor
    returns = new wxGenericValidator(valPtr->GetIntPtr());
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxGenericValidator, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxGenericValidatorArray_constructor
// %constructor wxGenericValidatorArray(wxLuaObject *valPtr)
static int LUACALL wxGenericValidatorArray_constructor(lua_State *L)
{
    wxGenericValidator *returns;
    // wxLuaObject *valPtr
    wxLuaObject *valPtr = (wxLuaObject *) getuserdatatype(L, 1, s_wxLuaObject);
    // call constructor
    returns = new wxGenericValidator(valPtr->GetArrayPtr());
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxGenericValidator, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxSocketBase_Peek
// void Peek(void * buffer, unsigned long nbytes)
static int LUACALL wxSocketBase_Peek(lua_State *L)
{
    // unsigned long nbytes
    unsigned long nbytes = (unsigned long) getnumbertype(L, 2);
    // void * buffer
    void *buffer = malloc(nbytes);
    if (buffer != NULL)
    {
        // get this
        wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
        // call Peek
        self->Peek(buffer, nbytes);
        // return the number of parameters
        lua_pushlstring(L, (const char *)buffer, nbytes);
        free(buffer);
        return 1;
    }
    return 0;
}
%end

%override wxSocketBase_Read
// void Read(void * buffer, unsigned long nbytes)
static int LUACALL wxSocketBase_Read(lua_State *L)
{
    // unsigned long nbytes
    unsigned long nbytes = (unsigned long) getnumbertype(L, 2);
    // void * buffer
    void *buffer = malloc(nbytes);
    if (buffer != NULL)
    {
        // get this
        wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
        // call Peek
        self->Read(buffer, nbytes);
        // return the number of parameters
        lua_pushlstring(L, (const char *)buffer, nbytes);
        free(buffer);
        return 1;
    }
    return 0;
}
%end

%override wxSocketBase_ReadMsg
// void ReadMsg(void * buffer, unsigned long nbytes)
static int LUACALL wxSocketBase_ReadMsg(lua_State *L)
{
    // unsigned long nbytes
    unsigned long nbytes = (unsigned long) getnumbertype(L, 2);
    // void * buffer
    void *buffer = malloc(nbytes);
    if (buffer != NULL)
    {
        // get this
        wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
        // call Peek
        self->ReadMsg(buffer, nbytes);
        // return the number of parameters
        lua_pushlstring(L, (const char *)buffer, nbytes);
        free(buffer);
        return 1;
    }
    return 0;
}
%end

%override wxSocketBase_Unread
// void Unread(const void * buffer, unsigned long nbytes)
static int LUACALL wxSocketBase_Unread(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // const void * buffer
    const char *buffer = lua_tostring(L, 2);
    // unsigned long nbytes
    unsigned long nbytes = (argCount >= 3 ? (unsigned long) getnumbertype(L, 3) : lua_strlen(L, 2));
    // get this
    wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
    // call Unread
    self->Unread(buffer, nbytes);
    // return the number of parameters
    return 0;
}
%end

%override wxSocketBase_Write
// void Write(const void * buffer, unsigned long nbytes)
static int LUACALL wxSocketBase_Write(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // const void * buffer
    const char *buffer = lua_tostring(L, 2);
    // unsigned long nbytes
    unsigned long nbytes = (argCount >= 3 ? (unsigned long) getnumbertype(L, 3) : lua_strlen(L, 2));
    // get this
    wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
    // call Write
    self->Write(buffer, nbytes);
    // return the number of parameters
    return 0;
}
%end

%override wxSocketBase_WriteMsg
// void WriteMsg(const void * buffer, wxUint32 nbytes)
static int LUACALL wxSocketBase_WriteMsg(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // const void * buffer
    const char *buffer = lua_tostring(L, 2);
    // unsigned long nbytes
    unsigned long nbytes = (argCount >= 3 ? (unsigned long) getnumbertype(L, 3) : lua_strlen(L, 2));
    // get this
    wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
    // call WriteMsg
    self->WriteMsg(buffer, nbytes);
    // return the number of parameters
    return 0;
}
%end

%override wxSocketBase_GetClientData
// wxObject * GetClientData() const
static int LUACALL wxSocketBase_GetClientData(lua_State *L)
{
    wxObject *returns;
    // get this
    wxSocketBase *self = ( wxSocketBase * ) getuserdatatype(L, 1, s_wxSocketBase );
    // call GetClientData
    returns = (wxObject *) self->GetClientData();
    // push the result datatype
    pushuserdatatype(L, s_wxObject, returns );
    // return the number of parameters
    return 1;
}
%end

%override builtin_wxGetOsVersion
// %builtin int wxGetOsVersion(int *major = NULL, int *minor = NULL)
static int LUACALL builtin_wxGetOsVersion(lua_State *L)
{
    int returns;
    // int *minor = NULL
    int minor;
    // int *major = NULL
    int major;
    // call wxGetOsVersion
    returns = wxGetOsVersion(&major, &minor);
    // push the result number
    lua_pushnumber(L, returns);
    lua_pushnumber(L, major);
    lua_pushnumber(L, minor);
    // return the number of parameters
    return 3;
}
%end

%override wxPalette_Create
// bool Create(int n, const unsigned char* red, const unsigned char* green, const unsigned char* blue)
static int LUACALL wxPalette_Create(lua_State *L)
{
    bool returns;
    // const unsigned char* blue
    const unsigned char *blue = (unsigned char *) lua_tostring(L, 5);
    // const unsigned char* green
    const unsigned char *green = (unsigned char *) lua_tostring(L, 4);
    // const unsigned char* red
    const unsigned char *red = (unsigned char *) lua_tostring(L, 3);
    // int n
    int n = (int) getnumbertype(L, 2);
    // get this
    wxPalette *self = ( wxPalette * ) getuserdatatype(L, 1, s_wxPalette );
    // call Create
    returns = self->Create(n, red, green, blue);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxBitmap_constructor
// %win wxBitmap(void* data, int type, int width, int height, int depth = -1)
#ifdef __WXMSW__
static int LUACALL wxBitmap_constructor(lua_State *L)
{
    wxBitmap *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // int depth = -1
    int depth = (argCount >= 5 ? (int) getnumbertype(L, 5) : -1);
    // int height
    int height = (int) getnumbertype(L, 4);
    // int width
    int width = (int) getnumbertype(L, 3);
    // int type
    int type = (int) getnumbertype(L, 2);
    // void* data
    void *data = (void *) lua_tostring(L, 1);
    // call constructor
    returns = new wxBitmap(data, type, width, height, depth);
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxBitmap, returns );
    // return the number of parameters
    return 1;
}
#endif
%end

%override wxImageFromData_constructor
// %constructor wxImageFromData(int width, int height, unsigned char* data, bool static_data = FALSE)
static int LUACALL wxImageFromData_constructor(lua_State *L)
{
    wxImage *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // bool static_data = FALSE
    bool static_data = (argCount >= 4 ? getbooleantype(L, 4) : FALSE);
    // unsigned char* data
    unsigned char *data = (unsigned char *) lua_tostring(L, 3);
    // int height
    int height = (int) getnumbertype(L, 2);
    // int width
    int width = (int) getnumbertype(L, 1);
    // call constructor
    returns = new wxImage(width, height, data, static_data);
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxImage, returns );
    // return the number of parameters
    return 1;
}
%end


%override wxImageFromBitmap_constructor
// %constructor wxImageFromBitmap(const wxBitmap& bitmap)
static int LUACALL wxImageFromBitmap_constructor(lua_State *L)
{
    wxImage *returns;
    // const wxBitmap& bitmap
    const wxBitmap *bitmap = (wxBitmap *) getuserdatatype(L, 1, s_wxBitmap);
    // call constructor
    returns = new wxImage(bitmap->ConvertToImage());
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxImage, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxImage_SetData
// void SetData(unsigned char *data)
static int LUACALL wxImage_SetData(lua_State *L)
{
    // unsigned char *data
    unsigned char *data = (unsigned char *) lua_tostring(L, 2);
    // get this
    wxImage *self = ( wxImage * ) getuserdatatype(L, 1, s_wxImage );
    // call SetData
    self->SetData(data);
    // return the number of parameters
    return 0;
}
%end

%override wxCaret_GetPositionXY
// %rename GetPositionXY void GetPosition(int *x, int *y)
static int LUACALL wxCaret_GetPositionXY(lua_State *L)
{
    int x;
    int y;
    // get this
    wxCaret *self = ( wxCaret * ) getuserdatatype(L, 1, s_wxCaret );
    // call GetPosition
    self->GetPosition(&x, &y);
    // return the number of parameters
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxCaret_GetSizeXY
// %rename GetSizeXY void GetSize(int *x, int *y)
static int LUACALL wxCaret_GetSizeXY(lua_State *L)
{
    int x;
    int y;
    // get this
    wxCaret *self = ( wxCaret * ) getuserdatatype(L, 1, s_wxCaret );
    // call GetSize
    self->GetSize(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxWindow_GetTextExtent
// virtual void GetTextExtent(const wxString& string, int* x, int* y, int* descent = NULL, int* externalLeading = NULL, const wxFont* font = NULL ) const
static int LUACALL wxWindow_GetTextExtent(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // const wxFont* font = NULL
    const wxFont *font = (argCount >= 3 ? (wxFont *) getuserdatatype(L, 3, s_wxFont) : NULL);

    int externalLeading;
    int descent;
    int w;
    int h;
    // const wxString& string
    wxString string = lua2wx(getstringtype(L, 2));
    // get this
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call GetTextExtent
    self->GetTextExtent(string, &w, &h, &descent, &externalLeading, font);
    // return the number of parameters
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    lua_pushnumber(L, descent);
    lua_pushnumber(L, externalLeading);
    return 4;
}
%end

%override wxWindow_ScreenToClientXY
// %rename ScreenToClientXY virtual void ScreenToClient(int* x, int* y) const
static int LUACALL wxWindow_ScreenToClientXY(lua_State *L)
{
    int y;
    int x;
    // get this
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call ScreenToClient
    self->ScreenToClient(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 2;
}
%end

%override wxScrolledWindow_CalcScrolledPosition
// void CalcScrolledPosition( int x, int y, int *xx, int *yy) const
static int LUACALL wxScrolledWindow_CalcScrolledPosition(lua_State *L)
{
    int yy;
    int xx;
    // int y
    int y = (int) getnumbertype(L, 3);
    // int x
    int x = (int) getnumbertype(L, 2);
    // get this
    wxScrolledWindow *self = ( wxScrolledWindow * ) getuserdatatype(L, 1, s_wxScrolledWindow );
    // call CalcScrolledPosition
    self->CalcScrolledPosition(x, y, &xx, &yy);
    lua_pushnumber(L, xx);
    lua_pushnumber(L, yy);
    // return the number of parameters
    return 2;
}
%end

%override wxScrolledWindow_CalcUnscrolledPosition
// void CalcUnscrolledPosition( int x, int y, int *xx, int *yy) const
static int LUACALL wxScrolledWindow_CalcUnscrolledPosition(lua_State *L)
{
    int yy;
    int xx;
    // int y
    int y = (int) getnumbertype(L, 3);
    // int x
    int x = (int) getnumbertype(L, 2);
    // get this
    wxScrolledWindow *self = ( wxScrolledWindow * ) getuserdatatype(L, 1, s_wxScrolledWindow );
    // call CalcUnscrolledPosition
    self->CalcUnscrolledPosition(x, y, &xx, &yy);
    lua_pushnumber(L, xx);
    lua_pushnumber(L, yy);
    // return the number of parameters
    return 2;
}
%end

%override wxStatusBar_SetFieldsCount
// virtual void SetFieldsCount(int number = 1, int* widths = NULL)
static int LUACALL wxStatusBar_SetFieldsCount(lua_State *L)
{
    int  count  = 0;
    int *widths = NULL;

    SmartIntArray ptr;

    if (lua_istable(L, 2))
        ptr = widths = tointarray(L, 2, count);
    else
        count = (int) lua_tonumber(L, 2);

    // get this
    wxStatusBar *self = ( wxStatusBar * ) getuserdatatype(L, 1, s_wxStatusBar );
    // call SetFieldsCount
    self->SetFieldsCount(count, widths);
    // return the number of parameters
    return 0;
}
%end

%override wxImageList_GetSize
// void    GetSize(int index, int& width, int& height)
static int LUACALL wxImageList_GetSize(lua_State *L)
{
    // int& height
    int height;
    // int& width
    int width;
    // int index
    int index = (int) getnumbertype(L, 2);
    // get this
    wxImageList *self = ( wxImageList * ) getuserdatatype(L, 1, s_wxImageList );
    // call GetSize
    self->GetSize(index, width, height);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 2;
}
%end

%override wxListCtrl_HitTest
// long     HitTest(const wxPoint& point, int& flags)
static int LUACALL wxListCtrl_HitTest(lua_State *L)
{
    long returns;
    // int& flags
    int flags;
    // const wxPoint& point
    const wxPoint *point = (wxPoint *) getuserdatatype(L, 2, s_wxPoint);
    // get this
    wxListCtrl *self = ( wxListCtrl * ) getuserdatatype(L, 1, s_wxListCtrl );
    // call HitTest
    returns = self->HitTest(*point, flags);
    // push the result number
    lua_pushnumber(L, returns);
    // push the result flags
    lua_pushnumber(L, flags);
    // return the number of parameters
    return 2;
}
%end

%override wxTabCtrl_HitTest
// int HitTest(const wxPoint& pt, long& flags)
static int LUACALL wxTabCtrl_HitTest(lua_State *L)
{
    int returns;
    // long& flags
    long flags;
    // const wxPoint& pt
    const wxPoint *pt = (wxPoint *) getuserdatatype(L, 2, s_wxPoint);
    // get this
    wxTabCtrl *self = ( wxTabCtrl * ) getuserdatatype(L, 1, s_wxTabCtrl );
    // call HitTest
    returns = self->HitTest(*pt, flags);
    // push the result number
    lua_pushnumber(L, returns);
    // push the result flags
    lua_pushnumber(L, flags);
    // return the number of parameters
    return 2;
}
%end


%override wxTextCtrl_PositionToXY
// bool PositionToXY(long pos, long *x, long *y) const
static int LUACALL wxTextCtrl_PositionToXY(lua_State *L)
{
    bool returns;
    long y;
    long x;
    // long pos
    long pos = (long) getnumbertype(L, 2);
    // get this
    wxTextCtrl *self = ( wxTextCtrl * ) getuserdatatype(L, 1, s_wxTextCtrl );
    // call PositionToXY
    returns = self->PositionToXY(pos, &x, &y);
    // push the result number
    lua_pushboolean(L, returns);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    // return the number of parameters
    return 3;
}
%end

%override <wxver25|wxcompat24 wxTreeCtrl_GetFirstChild
// wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const
static int LUACALL wxTreeCtrl_GetFirstChild(lua_State *L)
{
    wxTreeItemId *returns;
    // long& cookie
    long cookie = 0;
    // const wxTreeItemId& item
    const wxTreeItemId *item = (wxTreeItemId *) getuserdatatype(L, 2, s_wxTreeItemId);
    // get this
    wxTreeCtrl *self = ( wxTreeCtrl * ) getuserdatatype(L, 1, s_wxTreeCtrl );
    // call GetFirstChild
    // allocate a new object using the copy constructor
    returns = new wxTreeItemId(self->GetFirstChild(*item, cookie) );
    // add the new object to the tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the result datatype
    pushuserdatatype(L, s_wxTreeItemId, returns );
    // push the cookie
    lua_pushnumber(L, cookie);
    // return the number of parameters
    return 2;
}
%end

%override <wxver25|wxcompat24 wxTreeCtrl_GetNextChild
// wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const
static int LUACALL wxTreeCtrl_GetNextChild(lua_State *L)
{
    wxTreeItemId *returns;
    // long& cookie
    long cookie = (long) lua_tonumber(L, 3);
    // const wxTreeItemId& item
    const wxTreeItemId *item = (wxTreeItemId *) getuserdatatype(L, 2, s_wxTreeItemId);
    // get this
    wxTreeCtrl *self = ( wxTreeCtrl * ) getuserdatatype(L, 1, s_wxTreeCtrl );
    // call GetNextChild
    // allocate a new object using the copy constructor
    returns = new wxTreeItemId(self->GetNextChild(*item, cookie) );
    // add the new object to the tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the result datatype
    pushuserdatatype(L, s_wxTreeItemId, returns );
    // push the cookie
    lua_pushnumber(L, cookie);
    // return the number of parameters
    return 2;
}
%end

%override wxTreeCtrl_HitTest
// wxTreeItemId HitTest(const wxPoint& point, int& flags)
static int LUACALL wxTreeCtrl_HitTest(lua_State *L)
{
    wxTreeItemId *returns;
    // int& flags
    int flags = 0;
    // const wxPoint& point
    const wxPoint *point = (wxPoint *) getuserdatatype(L, 2, s_wxPoint);
    // get this
    wxTreeCtrl *self = ( wxTreeCtrl * ) getuserdatatype(L, 1, s_wxTreeCtrl );
    // call HitTest
    // allocate a new object using the copy constructor
    returns = new wxTreeItemId(self->HitTest(*point, flags) );
    // add the new object to the tracked memory list
    addToTrackedMemoryList(L, returns);

    // push the result datatype
    pushuserdatatype(L, s_wxTreeItemId, returns );
    lua_pushnumber(L, flags);
    // return the number of parameters
    return 2;
}
%end

%override builtin_wxResourceParseString
// %builtin bool wxResourceParseString(char *string, wxResourceTable *table = NULL)
static int LUACALL builtin_wxResourceParseString(lua_State *L)
{
    bool returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxResourceTable *table = NULL
    wxResourceTable *table = (argCount >= 2 ? (wxResourceTable *) getuserdatatype(L, 2, s_wxResourceTable) : NULL);
    // char *string
    const char *string = getstringtype(L, 1);
    // call wxResourceParseString
    returns = wxResourceParseString((char *) string, table);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxGridCellAttr_GetAlignment
// void GetAlignment(int *horz, int *vert) const
static int LUACALL wxGridCellAttr_GetAlignment(lua_State *L)
{
    int horz;
    int vert;

    // get this
    wxGridCellAttr *self = ( wxGridCellAttr * ) getuserdatatype(L, 1, s_wxGridCellAttr );
    // call GetAlignment
    self->GetAlignment(&horz, &vert);

    lua_pushnumber(L, horz);
    lua_pushnumber(L, vert);
    // return the number of parameters
    return 2;
}
%end

%override wxGrid_GetTextBoxSize
// void    GetTextBoxSize( wxDC& dc, wxArrayString& lines, long * width, long * height)
static int LUACALL wxGrid_GetTextBoxSize(lua_State *L)
{
    long height;
    long width;
    // wxArrayString& lines
    wxArrayString *lines = (wxArrayString *) getuserdatatype(L, 3, s_wxArrayString);
    // wxDC& dc
    wxDC *dc = (wxDC *) getuserdatatype(L, 2, s_wxDC);
    // get this
    wxGrid *self = ( wxGrid * ) getuserdatatype(L, 1, s_wxGrid );
    // call GetTextBoxSize
    self->GetTextBoxSize(*dc, *lines, &width, &height);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    // return the number of parameters
    return 2;
}
%end

%override wxTabCtrl_GetItemData
// wxObject * GetItemData(int item) const
static int LUACALL wxTabCtrl_GetItemData(lua_State *L)
{
    wxObject *returns;
    // int item
    int item = (int) getnumbertype(L, 2);
    // get this
    wxTabCtrl *self = ( wxTabCtrl * ) getuserdatatype(L, 1, s_wxTabCtrl );
    // call GetItemData
    returns = (wxObject *) self->GetItemData(item);
    // push the result datatype
    pushuserdatatype(L, s_wxObject, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxDir_GetFirst
// bool GetFirst(wxString * filename, const wxString& filespec = "", int flags = wxDIR_DEFAULT) const
static int LUACALL wxDir_GetFirst(lua_State *L)
{
    bool returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // int flags = wxDIR_DEFAULT
    int flags = (argCount >= 3 ? (int) getnumbertype(L, 3) : wxDIR_DEFAULT);
    // const wxString& filespec = ""
    wxString filespec = (argCount >= 2 ? lua2wx(getstringtype(L, 2)) : wxT(""));
    // wxString * filename
    wxString filename;
    // get this
    wxDir *self = ( wxDir * ) getuserdatatype(L, 1, s_wxDir );
    // call GetFirst
    returns = self->GetFirst(&filename, filespec, flags);
    lua_pushboolean(L, returns);
    // push the result number
    lua_pushstring(L, wx2lua(filename));
    // return the number of parameters
    return 2;
}
%end

%override wxDir_GetNext
// bool GetNext(wxString * filename) const
static int LUACALL wxDir_GetNext(lua_State *L)
{
    bool returns;
    // wxString * filename
    wxString filename;
    // get this
    wxDir *self = ( wxDir * ) getuserdatatype(L, 1, s_wxDir );
    // call GetNext
    returns = self->GetNext(&filename);
    lua_pushboolean(L, returns);
    // push the result number
    lua_pushstring(L, wx2lua(filename));
    // return the number of parameters
    return 2;
}
%end

%override builtin_wxDos2UnixFilename
// %builtin wxString wxDos2UnixFilename(const wxString& s)
static int LUACALL builtin_wxDos2UnixFilename(lua_State *L)
{
    wxString str = lua2wx(lua_tostring(L, 1));
    if (!str.IsEmpty())
    {
        // call wxDos2UnixFilename
        wxDos2UnixFilename((wxChar*)str.GetData());
        // push the result string
        lua_pushstring(L, wx2lua(str));

        return 1;
    }
    return 0;
}
%end

%override builtin_wxUnix2DosFilename
// %builtin wxString wxUnix2DosFilename(const wxString& s)
static int LUACALL builtin_wxUnix2DosFilename(lua_State *L)
{
    wxString str = lua2wx(lua_tostring(L, 1));
    if (!str.IsEmpty())
    {
        // call wxUnix2DosFilename
        wxUnix2DosFilename((wxChar*)str.GetData());
        // push the result string
        lua_pushstring(L, wx2lua(str));

        return 1;
    }
    return 0;
}
%end

%override builtin_wxGetTempFileName
// %builtin char* wxGetTempFileName(const wxString& prefix, char* buf=NULL)
static int LUACALL builtin_wxGetTempFileName(lua_State *L)
{
    wxString returns;
    // const wxString& prefix
    wxString prefix = lua2wx(getstringtype(L, 1));
    // call wxGetTempFileName
    returns = wxGetTempFileName(prefix, NULL);
    // push the result number
    lua_pushstring(L, wx2lua(returns));
    // return the number of parameters
    return 1;
}
%end

%override wxDir_GetAllFiles
// static unsigned int GetAllFiles(const wxString& dirname, wxArrayString *files, const wxString& filespec = "", int flags = wxDIR_DEFAULT)
static int LUACALL wxDir_GetAllFiles(lua_State *L)
{
    unsigned int returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // int flags = wxDIR_DEFAULT
    int flags = (argCount >= 4 ? (int) getnumbertype(L, 4) : wxDIR_DEFAULT);
    // const wxString& filespec = ""
    wxString filespec = (argCount >= 3 ? lua2wx(getstringtype(L, 3)) : wxT(""));
    // wxArrayString *files
    wxArrayString files;
    // const wxString& dirname
    wxString dirname = lua2wx(getstringtype(L, 2));
    // call GetAllFiles
    returns = wxDir::GetAllFiles(dirname, &files, filespec, flags);
    // push the result number
    lua_pushnumber(L, returns);
    tostringtable(L, files);
    // return the number of parameters
    return 2;
}
%end

%override wxFile_Read
// unsigned int Read(void * buffer,  unsigned int count)
static int LUACALL wxFile_Read(lua_State *L)
{
    unsigned int returns;
    // unsigned int count
    unsigned int count = (unsigned int) getnumbertype(L, 2);
    // void * buffer
    void *buffer = malloc(count);
    if (buffer != NULL)
    {
        // get this
        wxFile *self = ( wxFile * ) getuserdatatype(L, 1, s_wxFile );
        // call Read
        returns = self->Read(buffer, count);
        // push the result number
        lua_pushnumber(L, returns);
        lua_pushlstring(L, (const char *) buffer, count);
        free(buffer);
        // return the number of parameters
        return 2;
    }
    return 0;
}
%end

%override wxFile_Write
// unsigned int Write(const void * buffer, unsigned int nbytes)
static int LUACALL wxFile_Write(lua_State *L)
{
    unsigned int returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // unsigned long nbytes
    unsigned long nbytes = (argCount >= 3 ? (unsigned long) getnumbertype(L, 3) : lua_strlen(L, 2));
    // const void * buffer
    const void *buffer = (const void *) lua_tostring(L, 2);
    // get this
    wxFile *self = ( wxFile * ) getuserdatatype(L, 1, s_wxFile );
    // call Write
    returns = self->Write(buffer, nbytes);
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxDropFilesEvent_GetFiles
// wxString* GetFiles() const
static int LUACALL wxDropFilesEvent_GetFiles(lua_State *L)
{
    // get this
    wxDropFilesEvent *self = ( wxDropFilesEvent * ) getuserdatatype(L, 1, s_wxDropFilesEvent );
    // call GetFiles
    int       numFiles = self->GetNumberOfFiles();
    wxString *files    = self->GetFiles();

    lua_newtable(L);

    int idx;
    for (idx = 0; idx < numFiles; ++idx)
    {
        lua_pushstring(L, wx2lua(files[idx]));
        lua_rawseti(L, -2, idx + 1);
    }
    // return the number of parameters
    return 1;
}
%end

%override wxCreateMenu_constructor
// %constructor wxCreateMenu(int table, const wxString& title = "", long style = 0)
static int LUACALL wxCreateMenu_constructor(lua_State *L)
{
    wxMenu *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // long style = 0
    long style = (argCount >= 3 ? (long) getnumbertype(L, 3) : 0);
    // const wxString& title = ""
    wxString title = (argCount >= 2 ? lua2wx(getstringtype(L, 2)) : wxT(""));
    // int table
    if (lua_istable(L, 1))
    {
        // call constructor
        returns = new wxMenu(title, style);

        int idx, count = luaL_getn(L, 1);

        for (idx = 1; idx <= count; ++idx)
        {
            lua_pushnumber(L, idx);
            lua_gettable(L, -2);

            if (lua_istable(L, -1))
            {
                lua_pushnumber(L, 1);
                lua_gettable(L, -2);
                if (lua_isnil(L, -1))
                {
                    returns->AppendSeparator();
                    lua_pop(L, 1);
                }
                else
                {
                    wxString helpText;
                    wxString menuText;

                    int iValue = (int) lua_tonumber(L, -1);
                    lua_pop(L, 1);

                    lua_pushnumber(L, 2);
                    lua_gettable(L, -2);
                    menuText = lua2wx(lua_tostring(L, -1));
                    lua_pop(L, 1);

                    lua_pushnumber(L, 3);
                    lua_gettable(L, -2);
                    if (lua_isstring(L, -1))
                        helpText = lua2wx(lua_tostring(L, -1));
                    lua_pop(L, 1);

                    returns->Append(iValue, menuText, helpText);
                }
            }
            lua_pop(L, 1);
        }

        // push the constructed class pointer
        pushuserdatatype(L, s_wxMenu, returns );
        // return the number of parameters
        return 1;
    }
    return 0;
}
%end

%override wxAcceleratorTable_constructor
// wxAcceleratorTable(int n, wxAcceleratorEntry* entries)
static int LUACALL wxAcceleratorTable_constructor(lua_State *L)
{
    wxAcceleratorEntry *pItems  = NULL;
    int                 nItems  = 0;
    if (lua_istable(L, 1))
    {
        nItems = luaL_getn(L, 1);
        if (nItems > 0)
            pItems = new wxAcceleratorEntry[nItems];

        if (pItems != NULL)
        {
            int idx, idxMax = nItems;
            nItems = 0;
            for (idx = 1; idx <= idxMax; ++idx)
            {
                lua_pushnumber(L, idx);
                lua_gettable(L, -2);

                int  iFlags  = 0;
                int  keyCode = 0;
                int  cmd     = 0;
                bool fValid  = false;

                if (lua_istable(L, -1))
                {
                    lua_pushnumber(L, 1);
                    lua_gettable(L, -2);
                    iFlags = (int) lua_tonumber(L, -1);
                    lua_pop(L, 1);

                    lua_pushnumber(L, 2);
                    lua_gettable(L, -2);
                    keyCode = (int) lua_tonumber(L, -1);
                    lua_pop(L, 1);

                    lua_pushnumber(L, 3);
                    lua_gettable(L, -2);
                    cmd = (int) lua_tonumber(L, -1);
                    lua_pop(L, 1);

                    fValid = true;
                }
                else
                if (lua_isuserdata(L, -1))
                {
                    wxAcceleratorEntry *pEntry = (wxAcceleratorEntry *) getuserdatatype(L, -1, s_wxAcceleratorEntry);
                    if (pEntry != NULL)
                    {
                        iFlags  = pEntry->GetFlags();
                        keyCode = pEntry->GetKeyCode();
                        cmd     = pEntry->GetCommand();

                        fValid = true;
                    }
                }
                lua_pop(L, 1);

                if (fValid)
                {
                    pItems[nItems].Set(iFlags, keyCode, cmd);
                    ++nItems;
                }
            }
        }
    }

    // call constructor
    if (pItems != NULL)
    {
        wxAcceleratorTable *returns = NULL;
        if (nItems > 0)
            returns = new wxAcceleratorTable(nItems, pItems);

        delete[] pItems;

        if (returns != NULL)
        {
		    addToTrackedMemoryList(L, returns);
            // push the constructed class pointer
            pushuserdatatype(L, s_wxAcceleratorTable, returns );
            // return the number of parameters
            return 1;
        }
    }
    return 0;
}
%end

%override wxLuaDebugServerCompile_constructor
// %constructor wxLuaDebugServerCompile(const wxString &buffer, const wxString &fileName)
static int LUACALL wxLuaDebugServerCompile_constructor(lua_State *L)
{
    // const wxString &fileName
    wxString fileName = lua2wx(getstringtype(L, 2));
    // const wxString &buffer
    wxString buffer = lua2wx(getstringtype(L, 1));
    // compile the file
    bool fResult = wxLuaDebugServer::Compile(buffer, fileName);
    // push result
    lua_pushboolean(L, fResult);
    // return the number of parameters
    return 1;
}
%end

%override wxConfigBase_ReadFloat
// %rename ReadFloat double Read(const wxString& key, double defaultVal = 0) const
static int LUACALL wxConfigBase_ReadFloat(lua_State *L)
{
    double returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // double defaultVal = 0
    long defaultVal = (argCount >= 3 ? (long) getnumbertype(L, 3) : 0);
    // const wxString& key
    wxString key = lua2wx(getstringtype(L, 2));
    // get this
    wxConfigBase *self = ( wxConfigBase * ) getuserdatatype(L, 1, s_wxConfigBase );
    // call Read
    returns = self->Read(key, defaultVal);
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxRegConfig_ReadFloat
// %rename ReadFloat double Read(const wxString& key, double defaultVal = 0) const
static int LUACALL wxRegConfig_ReadFloat(lua_State *L)
{
    double returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // double defaultVal = 0
    double defaultVal = (argCount >= 3 ? (double) getnumbertype(L, 3) : 0);
    // const wxString& key
    wxString key = lua2wx(getstringtype(L, 2));
    // get this
    wxRegConfig *self = ( wxRegConfig * ) getuserdatatype(L, 1, s_wxRegConfig );
    // call Read
    self->Read(key, &returns, defaultVal);
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxConfig_GetFirstGroup
// bool GetFirstGroup(wxString& str, long& index) const
static int LUACALL wxConfig_GetFirstGroup(lua_State *L)
{
    bool     returns;
    long     index = 0;
    wxString str;
    // get this
    wxConfig *self = ( wxConfig * ) getuserdatatype(L, 1, s_wxConfig );
    // call GetFirstGroup
    returns = self->GetFirstGroup(str, index);
    // push the result number
    lua_pushboolean(L, returns);
    // push the result string
    lua_pushstring(L, wx2lua(str));
    // push the next index
    lua_pushnumber(L, index);
    // return the number of parameters
    return 3;
}
%end

%override wxConfig_GetFirstEntry
// bool GetFirstEntry(wxString& str, long& index) const
static int LUACALL wxConfig_GetFirstEntry(lua_State *L)
{
    bool     returns;
    long     index = 0;
    wxString str;
    // get this
    wxConfig *self = ( wxConfig * ) getuserdatatype(L, 1, s_wxConfig );
    // call GetFirstEntry
    returns = self->GetFirstEntry(str, index);
    // push the result number
    lua_pushboolean(L, returns);
    // push the next string
    lua_pushstring(L, wx2lua(str));
    // push the next index
    lua_pushnumber(L, index);
    // return the number of parameters
    return 3;
}
%end

%override wxConfig_GetNextGroup
// bool GetNextGroup(wxString& str, long& index) const
static int LUACALL wxConfig_GetNextGroup(lua_State *L)
{
    bool     returns;
    long     index = (long) lua_tonumber(L, 2);
    wxString str;
    // get this
    wxConfig *self = ( wxConfig * ) getuserdatatype(L, 1, s_wxConfig );
    // call GetNextGroup
    returns = self->GetNextGroup(str, index);
    // push the result number
    lua_pushboolean(L, returns);
    // push the next result string
    lua_pushstring(L, wx2lua(str));
    // push the next index
    lua_pushnumber(L, index);
    // return the number of parameters
    return 3;
}
%end

%override wxConfig_GetNextEntry
// bool GetNextEntry(wxString& str, long& index) const
static int LUACALL wxConfig_GetNextEntry(lua_State *L)
{
    bool returns;
    long index = (long) lua_tonumber(L, 2);
    wxString str;
    // get this
    wxConfig *self = ( wxConfig * ) getuserdatatype(L, 1, s_wxConfig );
    // call GetNextEntry
    returns = self->GetNextEntry(str, index);
    // push the result number
    lua_pushboolean(L, returns);
    // push the result string
    lua_pushstring(L, wx2lua(str));
    // push the next index
    lua_pushnumber(L, index);
    // return the number of parameters
    return 3;
}
%end

%override wxHtmlWindow_SetFonts
// void SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
static int LUACALL wxHtmlWindow_SetFonts(lua_State *L)
{
    // const int *sizes
#ifdef __WXMSW__
    int sizes[7] = { 7,8,10,12,16,22,30 };
#else
    int sizes[7] = { 10,12,14,16,19,24,32 };
#endif

    const int arraySize = sizeof(sizes)/sizeof(sizes[0]);

    int argCount = lua_gettop(L);

    if (argCount >= 4)
    {
        if (lua_istable(L, 4))
        {
            int count = 0;

            int *sizeArray = tointarray(L, 4, count);

            if (count > arraySize)
                count = arraySize;

            memcpy(sizes, sizeArray, count * sizeof(int));

            delete[] sizeArray;
        }
        else
        {
            int idx;

            if (argCount > 4 + arraySize)
                argCount = 4 + arraySize;

            for (idx = 4; idx < argCount; ++idx)
                sizes[idx - 4] = (int) lua_tonumber(L, idx);
        }
    }

    // wxString fixed_face
    wxString fixed_face = lua2wx(getstringtype(L, 3));
    // wxString normal_face
    wxString normal_face = lua2wx(getstringtype(L, 2));
    // get this
    wxHtmlWindow *self = ( wxHtmlWindow * ) getuserdatatype(L, 1, s_wxHtmlWindow );
    // call SetFonts
    self->SetFonts(normal_face, fixed_face, sizes);

    // return the number of parameters
    return 0;
}
%end

%override wxHtmlWinParser_SetFonts
// void SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
static int LUACALL wxHtmlWinParser_SetFonts(lua_State *L)
{
    // const int *sizes
#ifdef __WXMSW__
    int sizes[7] = { 7,8,10,12,16,22,30 };
#else
    int sizes[7] = { 10,12,14,16,19,24,32 };
#endif

    const int arraySize = sizeof(sizes)/sizeof(sizes[0]);

    int argCount = lua_gettop(L);

    if (argCount >= 4)
    {
        if (lua_istable(L, 4))
        {
            int count = 0;

            int *sizeArray = tointarray(L, 4, count);

            if (count > arraySize)
                count = arraySize;

            memcpy(sizes, sizeArray, count * sizeof(int));

            delete[] sizeArray;
        }
        else
        {
            int idx;

            if (argCount > 4 + arraySize)
                argCount = 4 + arraySize;

            for (idx = 4; idx < argCount; ++idx)
                sizes[idx - 4] = (int) lua_tonumber(L, idx);
        }
    }

    // wxString fixed_face
    wxString fixed_face = lua2wx(getstringtype(L, 3));
    // wxString normal_face
    wxString normal_face = lua2wx(getstringtype(L, 2));
    // get this
    wxHtmlWinParser *self = ( wxHtmlWinParser * ) getuserdatatype(L, 1, s_wxHtmlWinParser );
    // call SetFonts
    self->SetFonts(normal_face, fixed_face, sizes);
    // return the number of parameters
    return 0;
}
%end

%override wxHtmlTag_GetParamAsColour
// %alias GetParamAsColor bool GetParamAsColour(const wxString& par, wxColour *clr) const
static int LUACALL wxHtmlTag_GetParamAsColour(lua_State *L)
{
    bool returns;
    wxColour *retColour = new wxColour;
    // const wxString& par
    wxString par = lua2wx(getstringtype(L, 2));
    // get this
    wxHtmlTag *self = ( wxHtmlTag * ) getuserdatatype(L, 1, s_wxHtmlTag );
    // call GetParamAsColour
    returns = self->GetParamAsColour(par, retColour);
    //
    // push the result number
    lua_pushboolean(L, returns);
    pushuserdatatype(L, s_wxColour, retColour );
    // return the number of parameters
    return 2;
}
%end

%override wxHtmlTag_GetParamAsInt
// bool GetParamAsInt(const wxString& par, int *value) const
static int LUACALL wxHtmlTag_GetParamAsInt(lua_State *L)
{
    bool returns;
    int value;
    // const wxString& par
    wxString par = lua2wx(getstringtype(L, 2));
    // get this
    wxHtmlTag *self = ( wxHtmlTag * ) getuserdatatype(L, 1, s_wxHtmlTag );
    // call GetParamAsInt
    returns = self->GetParamAsInt(par, &value);
    // push the result number
    lua_pushboolean(L, returns);
    //
    lua_pushnumber(L, value);
    // return the number of parameters
    return 2;
}
%end

%override wxFontMapper_GetAltForEncoding
// bool GetAltForEncoding(wxFontEncoding encoding, wxFontEncoding *altEncoding, const wxString &faceName = wxEmptyString, bool interactive = TRUE)
static int LUACALL wxFontMapper_GetAltForEncoding(lua_State *L)
{
    bool returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // bool interactive = TRUE
    bool interactive = (argCount >= 5 ? getbooleantype(L, 4) : TRUE);
    // const wxString &faceName = wxEmptyString
    wxString faceName = (argCount >= 4 ? lua2wx(getstringtype(L, 3)) : wxT(""));
    // wxFontEncoding *altEncoding
    wxFontEncoding altEncoding;
    // wxFontEncoding encoding
    wxFontEncoding encoding = (wxFontEncoding) getenumerationtype(L, 2);
    // get this
    wxFontMapper *self = ( wxFontMapper * ) getuserdatatype(L, 1, s_wxFontMapper );
    // call GetAltForEncoding
    returns = self->GetAltForEncoding(encoding, &altEncoding, faceName, interactive);
    // push the result number
    lua_pushboolean(L, returns);
    // push the result encoding
    lua_pushnumber(L, altEncoding);
    // return the number of parameters
    return 2;
}
%end

%override wxHtmlCell_AdjustPagebreak
// virtual bool AdjustPagebreak(int * pagebreak)
static int LUACALL wxHtmlCell_AdjustPagebreak(lua_State *L)
{
    bool returns;
    // int * pagebreak
    int pagebreak  = (int) getnumbertype(L, 2);
    // get this
    wxHtmlCell *self = ( wxHtmlCell * ) getuserdatatype(L, 1, s_wxHtmlCell );
    // call AdjustPagebreak
    returns = self->AdjustPagebreak(&pagebreak);
    // push the result number
    lua_pushboolean(L, returns);
    //
    lua_pushnumber(L, pagebreak);
    // return the number of parameters
    return 2;
}
%end

%override wxHtmlCell_Find
// virtual const wxHtmlCell* Find(int condition, void *param = 0)
static int LUACALL wxHtmlCell_Find(lua_State *L)
{
    const wxHtmlCell *returns = NULL;
    // int condition
    int condition = (int) getnumbertype(L, 2);
    // get this
    wxHtmlCell *self = ( wxHtmlCell * ) getuserdatatype(L, 1, s_wxHtmlCell );
    // call Find
    switch(lua_type(L, 3))
    {
        case LUA_TNIL:
            returns = self->Find(condition, NULL);
            break;

        case LUA_TSTRING:
            {
                wxString param = lua2wx(getstringtype(L, 3));
                returns = self->Find(condition, &param);
            }
            break;

        case LUA_TNUMBER:
            {
                int param = (int) getnumbertype(L, 3);
                returns = self->Find(condition, &param);
            }
            break;

        default:
            terror(L, "wxLua: Expected simple type for parameter 3.");
            break;
    }

    // push the result datatype
    pushuserdatatype(L, s_wxHtmlCell, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxStringList_constructor
// wxStringList()
static int LUACALL wxStringList_constructor(lua_State *L)
{
    wxStringList *returns;
    // call constructor
    returns = new wxStringList();

    int argCount = lua_gettop(L);
    if (argCount > 0)
    {
        int idx;
        if (argCount == 1 && lua_istable(L, 1))
        {
            int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = tostringarray(L, 1, count);
            for (idx = 0; idx < count; ++idx)
                returns->Add(choices[idx]);
        }
        else
        {
            for (idx = 1; idx < argCount; ++idx)
                returns->Add(lua2wx(lua_tostring(L, idx)));
        }
    }

    // push the constructed class pointer
    pushuserdatatype(L, s_wxStringList, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxHelpController_GetFrameParameters
// virtual wxFrame * GetFrameParameters(wxSize * size = NULL, wxPoint * pos = NULL, bool *newFrameEachTime = NULL)
static int LUACALL wxHelpController_GetFrameParameters(lua_State *L)
{
    wxFrame *returns;

    bool    newFrameEachTime = false;
    wxPoint *pos = new wxPoint();
    wxSize  *size = new wxSize();
    // get this
    wxHelpController *self = ( wxHelpController * ) getuserdatatype(L, 1, s_wxHelpController );
    // call GetFrameParameters
    returns = self->GetFrameParameters(size, pos, &newFrameEachTime);
    // push the result datatype

    pushuserdatatype(L, s_wxFrame, returns );
    pushuserdatatype(L, s_wxSize, size );
    pushuserdatatype(L, s_wxPoint, pos );
    lua_pushboolean(L, newFrameEachTime);
    // return the number of parameters
    return 4;
}
%end

%override wxPrintout_GetPageInfo
// void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
static int LUACALL wxPrintout_GetPageInfo(lua_State *L)
{
    int pageTo;
    int pageFrom;
    int maxPage;
    int minPage;
    // get this
    wxPrintout *self = ( wxPrintout * ) getuserdatatype(L, 1, s_wxPrintout );
    // call GetPageInfo
    self->GetPageInfo(&minPage, &maxPage, &pageFrom, &pageTo);
    lua_pushnumber(L, minPage);
    lua_pushnumber(L, maxPage);
    lua_pushnumber(L, pageFrom);
    lua_pushnumber(L, pageTo);
    // return the number of parameters
    return 4;
}
%end

%override wxPrintout_GetPageSizeMM
// void GetPageSizeMM(int *w, int *h)
static int LUACALL wxPrintout_GetPageSizeMM(lua_State *L)
{
    int h;
    int w;
    // get this
    wxPrintout *self = ( wxPrintout * ) getuserdatatype(L, 1, s_wxPrintout );
    // call GetPageSizeMM
    self->GetPageSizeMM(&w, &h);
    // return the number of parameters
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}
%end

%override wxPrintout_GetPageSizePixels
// void GetPageSizePixels(int *w, int *h)
static int LUACALL wxPrintout_GetPageSizePixels(lua_State *L)
{
    int h;
    int w;
    // get this
    wxPrintout *self = ( wxPrintout * ) getuserdatatype(L, 1, s_wxPrintout );
    // call GetPageSizePixels
    self->GetPageSizePixels(&w, &h);
    // return the number of parameters
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}
%end

%override wxPrintout_GetPPIPrinter
// void GetPPIPrinter(int *w, int *h)
static int LUACALL wxPrintout_GetPPIPrinter(lua_State *L)
{
    int h;
    int w;
    // get this
    wxPrintout *self = ( wxPrintout * ) getuserdatatype(L, 1, s_wxPrintout );
    // call GetPPIPrinter
    self->GetPPIPrinter(&w, &h);
    // return the number of parameters
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}
%end

%override wxPrintout_GetPPIScreen
// void GetPPIScreen(int *w, int *h)
static int LUACALL wxPrintout_GetPPIScreen(lua_State *L)
{
    int h;
    int w;
    // get this
    wxPrintout *self = ( wxPrintout * ) getuserdatatype(L, 1, s_wxPrintout );
    // call GetPPIScreen
    self->GetPPIScreen(&w, &h);
    // return the number of parameters
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}
%end

%override wxPrintData_Copy
// wxPrintData *Copy()
static int LUACALL wxPrintData_Copy(lua_State *L)
{
    wxPrintData *self    = ( wxPrintData * ) getuserdatatype(L, 1, s_wxPrintData );
    wxPrintData *returns = new wxPrintData;
    // Copy data over
    *returns = *self;
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxPrintData, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxPageSetupDialogData_Copy
// wxPrintData *Copy()
static int LUACALL wxPageSetupDialogData_Copy(lua_State *L)
{
    wxPageSetupDialogData *self    = ( wxPageSetupDialogData * ) getuserdatatype(L, 1, s_wxPageSetupDialogData );
    wxPageSetupDialogData *returns = new wxPageSetupDialogData;
    // Copy data over
    *returns = *self;
    // add to tracked memory list
    addToTrackedMemoryList(L, returns);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxPageSetupDialogData, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxPrintPreview_constructor
// wxPrintPreview(wxLuaPrintout* printout, wxLuaPrintout* printoutForPrinting, wxPrintData* data=NULL)
static int LUACALL wxPrintPreview_constructor(lua_State *L)
{
    wxPrintPreview *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxPrintData* data=NULL
    wxPrintData *data = (argCount >= 3 ? (wxPrintData *) getuserdatatype(L, 3, s_wxPrintData) : NULL);
    // wxLuaPrintout* printoutForPrinting
    wxLuaPrintout *printoutForPrinting = (argCount >= 2 ? (wxLuaPrintout *) getuserdatatype(L, 2, s_wxLuaPrintout) : NULL);
    // wxLuaPrintout* printout
    wxLuaPrintout *printout = (wxLuaPrintout *) getuserdatatype(L, 1, s_wxLuaPrintout);

    // when the wxPrintPreview constructor is called, the
    // object takes over ownership of the wxPrintout objects, therefore
    // we must disconnect them from our tracking list
    if (printoutForPrinting != NULL)
        removeTrackedMemory(L, printoutForPrinting, false);
    removeTrackedMemory(L, printout, false);

    // call constructor
    returns = new wxPrintPreview(printout, printoutForPrinting, data);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxPrintPreview, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxPrintout_OnBeginDocument
// bool OnBeginDocument(int startPage, int endPage);
static int LUACALL wxPrintout_OnBeginDocument(lua_State *L)
{
    bool returns;
    // int endPage
    int endPage = (int) getnumbertype(L, 3);
    // int startPagest
    int startPage = (int) getnumbertype(L, 2);
    // get this
    wxPrintout *self = ( wxPrintout * ) getuserdatatype(L, 1, s_wxPrintout );
    // call OnBeginDocument
    returns = self->wxPrintout::OnBeginDocument(startPage, endPage);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxDataObject_SetData
// virtual bool SetData( const wxDataFormat& format, int len, const void *buf )
static int LUACALL wxDataObject_SetData(lua_State *L)
{
    bool returns;
    // const void *buf
    const void *buf = lua_tostring(L, 3);
    // int len
    int len = lua_strlen(L, 3);
    // const wxDataFormat& format
    const wxDataFormat *format = (wxDataFormat *) getuserdatatype(L, 2, s_wxDataFormat);
    // get this
    wxDataObject *self = ( wxDataObject * ) getuserdatatype(L, 1, s_wxDataObject );
    // call SetData
    returns = self->SetData(*format, len, buf);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxDataObjectSimple_SetData
// virtual bool SetData(size_t len, const void *buf)
static int LUACALL wxDataObjectSimple_SetData(lua_State *L)
{
    bool returns;
    // const void *buf
    const void *buf = lua_tostring(L, 2);
    // size_t len
    size_t len = (size_t) lua_strlen(L, 2);
    // get this
    wxDataObjectSimple *self = ( wxDataObjectSimple * ) getuserdatatype(L, 1, s_wxDataObjectSimple );
    // call SetData
    returns = self->SetData(len, buf);
    // push the result number
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxDataObject_GetDataHere
// virtual bool GetDataHere(const wxDataFormat& format, void *buf ) const
static int LUACALL wxDataObject_GetDataHere(lua_State *L)
{
    bool returns;
    // const wxDataFormat& format
    const wxDataFormat *format = (wxDataFormat *) getuserdatatype(L, 2, s_wxDataFormat);
    // get this
    wxDataObject *self = ( wxDataObject * ) getuserdatatype(L, 1, s_wxDataObject );

    size_t size = self->GetDataSize(*format);

    void *buf = malloc(size);

    if (buf != NULL)
    {
        // call GetDataHere
        returns = self->GetDataHere(*format, buf);

        // push the result number
        lua_pushboolean(L, returns);
        // push the result string
        lua_pushlstring(L, (const char *) buf, size);

        free(buf);

        // return the number of parameters
        return 2;
    }
    return 0;
}
%end

%override wxDataObjectSimple_GetDataHere
// virtual bool GetDataHere(void *buf) const
static int LUACALL wxDataObjectSimple_GetDataHere(lua_State *L)
{
    bool returns;

    // get this
    wxDataObjectSimple *self = ( wxDataObjectSimple * ) getuserdatatype(L, 1, s_wxDataObjectSimple );

    size_t size = self->GetDataSize();

    void *buf = malloc(size);
    if (buf != NULL)
    {
        // call GetDataHere
        returns = self->GetDataHere(buf);

        // push the result number
        lua_pushboolean(L, returns);
        // push the result string
        lua_pushlstring(L, (const char *) buf, size);

        free(buf);

        // return the number of parameters
        return 2;
    }
    return 0;
}
%end

%override wxDataObject_GetAllFormats
// virtual void GetAllFormats( wxDataFormat *formats, wxDataObject::Direction dir = wxDataObject::Get) const
static int LUACALL wxDataObject_GetAllFormats(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxDataObject::Direction dir = wxDataObject::Get
    wxDataObject::Direction dir = (argCount >= 2 ? (wxDataObject::Direction)(int) getnumbertype(L, 2) : wxDataObject::Get);
    // get this
    wxDataObject *self = ( wxDataObject * ) getuserdatatype(L, 1, s_wxDataObject );
    // wxDataFormat *formats
    int idx, formatCount = self->GetFormatCount(dir);

    if (formatCount > 0)
    {
        wxDataFormat *formats = new wxDataFormat[formatCount];
        if (formats != NULL)
        {
            // call GetAllFormats
            self->GetAllFormats(formats, dir);
            // return the number of parameters

            lua_newtable(L);
            for (idx = 0; idx < formatCount; ++idx)
            {
                wxDataFormat *pFormat = new wxDataFormat(formats[idx]);
                pushuserdatatype(L, s_wxDataFormat, pFormat);
                lua_rawseti(L, -2, idx + 1);
            }
            delete[] formats;
            return 1;
        }
    }
    return 0;
}
%end

%override wxDataFormat_GetType
// wxDataFormatId GetType() const
static int LUACALL wxDataFormat_GetType(lua_State *L)
{
    wxDataFormatId returns;
    // get this
    wxDataFormat *self = ( wxDataFormat * ) getuserdatatype(L, 1, s_wxDataFormat );
    // call GetType
    returns = (wxDataFormatId) self->GetType();
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxDocManager_GetDocuments
// wxList& GetDocuments()
static int LUACALL wxDocManager_GetDocuments(lua_State *L)
{
    wxList *returns;
    // get this
    wxDocManager *self = ( wxDocManager * ) getuserdatatype(L, 1, s_wxDocManager );
    // call GetDocuments
    wxList &docs = self->GetDocuments();
    returns = new wxList(docs);
    // push the result datatype
    pushuserdatatype(L, s_wxList, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxDocument_GetViews
// wxList& GetViews() const
static int LUACALL wxDocument_GetViews(lua_State *L)
{
    wxList *returns;
    // get this
    wxDocument *self = ( wxDocument * ) getuserdatatype(L, 1, s_wxDocument );
    // call GetViews
    wxList &views = self->GetViews();
    returns = new wxList(views);
    // push the result datatype
    pushuserdatatype(L, s_wxList, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxCommandProcessor_GetCommands
// wxList& GetCommands() const
static int LUACALL wxCommandProcessor_GetCommands(lua_State *L)
{
    wxList *returns;
    // get this
    wxCommandProcessor *self = ( wxCommandProcessor * ) getuserdatatype(L, 1, s_wxCommandProcessor );
    // call GetCommands
    wxList &commands = self->GetCommands();
    returns = new wxList(commands);
    // push the result datatype
    pushuserdatatype(L, s_wxList, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxClassInfo_constructor
// wxClassInfo(const wxString &name)
static int LUACALL wxClassInfo_constructor(lua_State *L)
{
    wxClassInfo *returns;
    // const wxString &name
    wxString name = lua2wx(getstringtype(L, 1));
    // call constructor
    returns = wxClassInfo::FindClass(name);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxClassInfo, returns);
    // return the number of parameters
    return 1;
}
%end


%override wxFrameLayout_GetUpdatesManager
// cbUpdatesManagerBase &GetUpdatesManager()
static int LUACALL wxFrameLayout_GetUpdatesManager(lua_State *L)
{
    // get this
    wxFrameLayout *self = ( wxFrameLayout * ) getuserdatatype(L, 1, s_wxFrameLayout );
    // call GetUpdatesManager
    cbUpdatesManagerBase *returns = self->mpUpdatesMgr;
    // push the result datatype
    pushuserdatatype(L, s_cbUpdatesManagerBase, returns );
    // return the number of parameters
    return 1;
}
%end

%override cbDockPane_GetRowList
// wxBarIterator* GetRowList();
static int LUACALL cbDockPane_GetRowList(lua_State *L)
{
    wxBarIterator *returns;
    // get this
    cbDockPane *self = ( cbDockPane * ) getuserdatatype(L, 1, s_cbDockPane );
    // call GetRowList
    returns = new wxBarIterator(self->GetRowList());
    // push the result datatype
    pushuserdatatype(L, s_wxBarIterator, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxFrameLayout_GetNthPane
// cbDockPane* GetNthPane(int nth)
static int LUACALL wxFrameLayout_GetNthPane(lua_State *L)
{
    cbDockPane *returns;
    // int nth
    int nth = (int) getnumbertype(L, 2);
    // get this
    wxFrameLayout *self = ( wxFrameLayout * ) getuserdatatype(L, 1, s_wxFrameLayout );
    // call GetNthPane
    returns = self->GetPanesArray()[(nth)];
    // push the result datatype
    pushuserdatatype(L, s_cbDockPane, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxFrameLayout_GetNumBars
// int GetNumBars()
static int LUACALL wxFrameLayout_GetNumBars(lua_State *L)
{
    int returns;
    // get this
    wxFrameLayout *self = ( wxFrameLayout * ) getuserdatatype(L, 1, s_wxFrameLayout );
    // call GetNumBars
    returns = self->GetBars().Count();
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxFrameLayout_GetNthBar
// cbBarInfo *GetNthBar(int nth)
static int LUACALL wxFrameLayout_GetNthBar(lua_State *L)
{
    cbBarInfo *returns;
    // int nth
    int nth = (int) getnumbertype(L, 2);
    // get this
    wxFrameLayout *self = ( wxFrameLayout * ) getuserdatatype(L, 1, s_wxFrameLayout );
    // call GetNthBar
    returns = self->GetBars().Item(nth);
    // push the result datatype
    pushuserdatatype(L, s_cbBarInfo, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxDynamicToolBar_GetPreferredDim
// virtual void GetPreferredDim( const wxSize& givenDim, wxSize& prefDim )
static int LUACALL wxDynamicToolBar_GetPreferredDim(lua_State *L)
{
    // wxSize& prefDim
    wxSize *prefDim = new wxSize();
    // const wxSize& givenDim
    const wxSize *givenDim = (wxSize *) getuserdatatype(L, 2, s_wxSize);
    // get this
    wxDynamicToolBar *self = ( wxDynamicToolBar * ) getuserdatatype(L, 1, s_wxDynamicToolBar );
    // call GetPreferredDim
    self->GetPreferredDim(*givenDim, *prefDim);
    // return the number of parameters
    addToTrackedMemoryList(L, prefDim);
    pushuserdatatype(L, s_wxSize, prefDim);
    return 1;
}
%end

%override wxConfigGet_constructor
// %constructor wxConfigGet(bool CreateOnDemand = TRUE)
static int LUACALL wxConfigGet_constructor(lua_State *L)
{
    wxConfigBase *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // bool CreateOnDemand = TRUE
    bool CreateOnDemand = (argCount >= 1 ? getbooleantype(L, 1) : TRUE);
    // call constructor
    returns = wxConfigBase::Get(CreateOnDemand);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxConfig, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxConfigCreate_constructor
// %constructor wxConfigCreate()
static int LUACALL wxConfigCreate_constructor(lua_State *L)
{
    wxConfigBase *returns;
    // call constructor
    returns = wxConfigBase::Create();
    // push the constructed class pointer
    pushuserdatatype(L, s_wxConfig, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxConfigBase_Destroy
// void Destroy()
static int LUACALL wxConfigBase_Destroy(lua_State *L)
{
    // get this
    wxConfigBase *self = ( wxConfigBase * ) getuserdatatype(L, 1, s_wxConfigBase );
    // call Destroy
    delete self;
    // return the number of parameters
    return 0;
}
%end

%override wxXmlResourceGetDefault_constructor
// %constructor wxXmlResourceGetDefault()
static int LUACALL wxXmlResourceGetDefault_constructor(lua_State *L)
{
    wxXmlResource *returns;
    // call constructor
    returns = wxXmlResource::Get();
    // push the constructed class pointer
    pushuserdatatype(L, s_wxXmlResource, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxXmlResource_CreateFrame
// wxFrame* CreateFrame(wxWindow* parent, const wxString& name)
static int LUACALL wxXmlResource_CreateFrame(lua_State *L)
{
    wxFrame *returns = NULL;
    // create an empty frame
    returns = new wxFrame;
    // const wxString& name
    wxString name = lua2wx(getstringtype(L, 3));
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 2, s_wxWindow);
    // get this
    wxXmlResource *self = ( wxXmlResource * ) getuserdatatype(L, 1, s_wxXmlResource );
    // call LoadFrame
    if (self->LoadFrame(returns, parent, name))
    {
        // push the result datatype
        pushuserdatatype(L, s_wxFrame, returns );
        // return the number of parameters
        return 1;
    }
    return 0;
}
%end

%override wxXmlResource_CreateDialog
// wxFrame* CreateFrame(wxWindow* parent, const wxString& name)
static int LUACALL wxXmlResource_CreateDialog(lua_State *L)
{
    wxDialog *returns = NULL;
    // create an empty frame
    returns = new wxDialog;
    // const wxString& name
    wxString name = lua2wx(getstringtype(L, 3));
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 2, s_wxWindow);
    // get this
    wxXmlResource *self = (wxXmlResource *) getuserdatatype(L, 1, s_wxXmlResource );
    // call LoadFrame
    if (self->LoadDialog(returns, parent, name))
    {
        // push the result datatype
        pushuserdatatype(L, s_wxDialog, returns );
        // return the number of parameters
        return 1;
    }
    return 0;
}
%end

%override wxBitmapFromXPMData_constructor
// %constructor wxXmlResourceGetDefault()
static int LUACALL wxBitmapFromXPMData_constructor(lua_State *L)
{
    wxBitmap *returns;
    if (lua_istable(L, 1))
    {
        int count = 0;

        const char **sizeArray = tochararray(L, 1, count);
        if (sizeArray != NULL)
        {
            // call constructor
            returns = new wxBitmap(sizeArray);
            // add to tracked memory list
            addToTrackedMemoryList(L, returns);
            // push the constructed class pointer
            pushuserdatatype(L, s_wxBitmap, returns );
            // return the number of parameters
            delete [] sizeArray;
            return 1;
        }
    }
    return 0;
}
%end

%override wxRegEx_GetMatchIndexes
// %rename GetMatchPointer bool GetMatch(size_t* start, size_t* len, size_t index = 0) const
static int LUACALL wxRegEx_GetMatchIndexes(lua_State *L)
{
    bool returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // size_t index = 0
    size_t index = (argCount >= 2 ? (size_t) getnumbertype(L, 2) : 0);
    // size_t* len
    size_t len = 0;
    // size_t* start
    size_t start = 0;
    // get this
    wxRegEx *self = ( wxRegEx * ) getuserdatatype(L, 1, s_wxRegEx );
    // call GetMatch
    returns = self->GetMatch(&start, &len, index);
    // push the result number
    lua_pushboolean(L, returns);
    // push the match start and length indexes
    lua_pushnumber(L, start);
    lua_pushnumber(L, len);
    // return the number of parameters
    return 3;
}
%end

%override wxRegEx_Replace
// int Replace(wxString* text, const wxString& replacement, size_t maxMatches = 0) const
static int LUACALL wxRegEx_Replace(lua_State *L)
{
    int returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // size_t maxMatches = 0
    size_t maxMatches = (argCount >= 4 ? (size_t) getnumbertype(L, 4) : 0);
    // const wxString& replacement
    wxString replacement = lua2wx(getstringtype(L, 3));
    // wxString* text
    wxString text = lua2wx(getstringtype(L, 2));
    // get this
    wxRegEx *self = ( wxRegEx * ) getuserdatatype(L, 1, s_wxRegEx );
    // call Replace
    returns = self->Replace(&text, replacement, maxMatches);
    // push the result number
    lua_pushnumber(L, returns);
    // push the result text
    lua_pushstring(L, wx2lua(text));
    // return the number of parameters
    return 2;
}
%end

%override wxRegEx_ReplaceAll
// int ReplaceAll(wxString* text, const wxString& replacement) const
static int LUACALL wxRegEx_ReplaceAll(lua_State *L)
{
    int returns;
    // const wxString& replacement
    wxString replacement = lua2wx(getstringtype(L, 3));
    // wxString* text
    wxString text = lua2wx(getstringtype(L, 2));
    // get this
    wxRegEx *self = ( wxRegEx * ) getuserdatatype(L, 1, s_wxRegEx );
    // call ReplaceAll
    returns = self->ReplaceAll(&text, replacement);
    // push the result number
    lua_pushnumber(L, returns);
    // push the result text
    lua_pushstring(L, wx2lua(text));
    // return the number of parameters
    return 2;
}
%end

%override wxRegEx_ReplaceFirst
// int ReplaceFirst(wxString* text, const wxString& replacement) const
static int LUACALL wxRegEx_ReplaceFirst(lua_State *L)
{
    int returns;
    // const wxString& replacement
    wxString replacement = lua2wx(getstringtype(L, 3));
    // wxString* text
    wxString text = lua2wx(getstringtype(L, 2));
    // get this
    wxRegEx *self = ( wxRegEx * ) getuserdatatype(L, 1, s_wxRegEx );
    // call ReplaceFirst
    returns = self->ReplaceFirst(&text, replacement);
    // push the result number
    lua_pushnumber(L, returns);
    // push the result text
    lua_pushstring(L, wx2lua(text));
    // return the number of parameters
    return 2;
}
%end

%override wxXmlResource_Get
// wxXmlResource* Get()
static int LUACALL wxXmlResource_Get(lua_State *L)
{
    wxXmlResource *returns;
    // call Get
    returns = wxXmlResource::Get();
    // push the result datatype
    pushuserdatatype(L, s_wxXmlResource, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxXmlResource_GetXRCID
// int GetXRCID(const wxString &stringID)
static int LUACALL wxXmlResource_GetXRCID(lua_State *L)
{
    int returns;
    // const wxString &stringID
    wxString stringID = lua2wx(getstringtype(L, 2));
    // call GetXRCID
    returns = wxXmlResource::GetXRCID(stringID);
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxXmlResource_Set
// wxXmlResource* Set(wxXmlResource *res)
static int LUACALL wxXmlResource_Set(lua_State *L)
{
    wxXmlResource *returns;
    // wxXmlResource *res
    wxXmlResource *res = (wxXmlResource *) getuserdatatype(L, 2, s_wxXmlResource);
    // call Set
    returns = wxXmlResource::Set(res);
    // push the result datatype
    pushuserdatatype(L, s_wxXmlResource, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxXmlNode_GetPropValPtr
// %rename GetPropValPtr bool GetPropVal(const wxString& propName, wxString *value) const;
static int LUACALL wxXmlNode_GetPropValPtr(lua_State *L)
{
    bool returns;
    // wxString *value
    wxString value;
    // const wxString& propName
    wxString propName = lua2wx(getstringtype(L, 2));
    // get this
    wxXmlNode *self = ( wxXmlNode * ) getuserdatatype(L, 1, s_wxXmlNode );
    // call GetPropVal
    returns = self->GetPropVal(propName, &value);
    // push the result number
    lua_pushboolean(L, returns);
    // push the result string
    lua_pushstring(L, wx2lua(value));
    // return the number of parameters
    return 2;
}
%end

%override wxFileName_SplitPath
// static void SplitPath(const wxString& fullpath, wxString* volume, wxString* path, wxString* name, wxString* ext, wxPathFormat format = wxPATH_NATIVE)
static int LUACALL wxFileName_SplitPath(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxPathFormat format = wxPATH_NATIVE
    wxPathFormat format = (argCount >= 2 ? (wxPathFormat) getenumerationtype(L, 3) : wxPATH_NATIVE);
    wxString ext;
    wxString name;
    wxString path;
    // const wxString& fullpath
    wxString fullpath = lua2wx(getstringtype(L, 2));
    // call SplitPath
    wxFileName::SplitPath(fullpath, &path, &name, &ext, format);
    // push the result strings
    lua_pushstring(L, wx2lua(path));
    lua_pushstring(L, wx2lua(name));
    lua_pushstring(L, wx2lua(ext));
    // return the number of parameters
    return 3;
}
%end

%override wxFileName_SplitVolumePath
// static void SplitPath(const wxString& fullpath, wxString* volume, wxString* path, wxString* name, wxString* ext, wxPathFormat format = wxPATH_NATIVE)
static int LUACALL wxFileName_SplitVolumePath(lua_State *L)
{
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxPathFormat format = wxPATH_NATIVE
    wxPathFormat format = (argCount >= 2 ? (wxPathFormat) getenumerationtype(L, 3) : wxPATH_NATIVE);
    wxString ext;
    wxString name;
    wxString path;
    wxString volume;
    // const wxString& fullpath
    wxString fullpath = lua2wx(getstringtype(L, 2));
    // call SplitPath
    wxFileName::SplitPath(fullpath, &volume, &path, &name, &ext, format);
    // push the result strings
    lua_pushstring(L, wx2lua(volume));
    lua_pushstring(L, wx2lua(path));
    lua_pushstring(L, wx2lua(name));
    lua_pushstring(L, wx2lua(ext));
    // return the number of parameters
    return 4;
}
%end

%override wxFileName_GetTimes
// bool GetTimes(wxDateTime* dtAccess, wxDateTime* dtMod, wxDateTime* dtCreate) const
static int LUACALL wxFileName_GetTimes(lua_State *L)
{
    bool returns;
    wxDateTime *dtCreate = new wxDateTime();
    wxDateTime *dtMod = new wxDateTime();
    wxDateTime *dtAccess= new wxDateTime();
    // get this
    wxFileName *self = ( wxFileName * ) getuserdatatype(L, 1, s_wxFileName );
    // call GetTimes
    returns = self->GetTimes(dtAccess, dtMod, dtCreate);
    // add to tracked memory list
    addToTrackedMemoryList(L, dtAccess);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxDateTime, dtAccess );
    // add to tracked memory list
    addToTrackedMemoryList(L, dtMod);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxDateTime, dtMod);
    // add to tracked memory list
    addToTrackedMemoryList(L, dtCreate);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxDateTime, dtCreate);
    // push the result flag
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxQueryLayoutInfoEvent_SetAlignment
// void SetAlignment(wxLayoutAlignment alignment)
static int LUACALL wxQueryLayoutInfoEvent_SetAlignment(lua_State *L)
{
    // wxLayoutAlignment alignment
    wxLayoutAlignment alignment = (wxLayoutAlignment) getenumerationtype(L, 2);
    // get this
    wxQueryLayoutInfoEvent *self = ( wxQueryLayoutInfoEvent * ) getuserdatatype(L, 1, s_wxQueryLayoutInfoEvent );
    // call SetAlignment
    self->SetAlignment(alignment);
    // return the number of parameters
    return 0;
}
%end

%override wxQueryLayoutInfoEvent_SetOrientation
// void SetOrientation(wxLayoutOrientation orientation)
static int LUACALL wxQueryLayoutInfoEvent_SetOrientation(lua_State *L)
{
    // wxLayoutOrientation orientation
    wxLayoutOrientation orientation = (wxLayoutOrientation) getenumerationtype(L, 2);
    // get this
    wxQueryLayoutInfoEvent *self = ( wxQueryLayoutInfoEvent * ) getuserdatatype(L, 1, s_wxQueryLayoutInfoEvent );
    // call SetOrientation
    self->SetOrientation(orientation);
    // return the number of parameters
    return 0;
}
%end

%override builtin_wxGetBaseApp
// %builtin wxApp* wxGetBaseApp()
static int LUACALL builtin_wxGetBaseApp(lua_State *L)
{
    wxApp *returns;
    // call wxGetBaseApp
    returns = wxTheApp;
    // push the result datatype
    pushuserdatatype(L, s_wxApp, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxClassInfo_FindClass
// static wxClassInfo * FindClass(const wxString &name)
static int LUACALL wxClassInfo_FindClass(lua_State *L)
{
    wxClassInfo *returns;
    // const wxString &name
    wxString name = lua2wx(getstringtype(L, 2));
    // call FindClass
    returns = wxClassInfo::FindClass((wxChar *)name.c_str());
    // push the result datatype
    pushuserdatatype(L, s_wxClassInfo, returns );
    // return the number of parameters
    return 1;
}
%end

%override builtin_wxGetSingleChoice
// %builtin int wxGetSingleChoice(const wxString& message, const wxString& caption, int n, const wxString& choices[], wxWindow *parent = NULL, int x = -1, int y = -1, bool centre = TRUE, int width=150, int height=200)
static int LUACALL builtin_wxGetSingleChoice(lua_State *L)
{
    int returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // int height=200
    int height = (argCount >= 9 ? (int) getnumbertype(L, 9) : 200);
    // int width=150
    int width = (argCount >= 8 ? (int) getnumbertype(L, 8) : 150);
    // bool centre = TRUE
    bool centre = (argCount >= 7 ? getbooleantype(L, 7) : TRUE);
    // int y = -1
    int y = (argCount >= 6 ? (int) getnumbertype(L, 6) : -1);
    // int x = -1
    int x = (argCount >= 5 ? (int) getnumbertype(L, 5) : -1);
    // wxWindow *parent = NULL
    wxWindow *parent = (argCount >= 5 ? (wxWindow *) getuserdatatype(L, 4, s_wxWindow) : NULL);
    // const wxString& choices[]
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = tostringarray(L, 3, count);
    // const wxString& caption
    wxString caption = lua2wx(getstringtype(L, 2));
    // const wxString& message
    wxString message = lua2wx(getstringtype(L, 1));
    // call wxGetSingleChoice
    returns = wxGetSingleChoiceIndex(message, caption, count, choices, parent, x, y, centre, width, height);
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override builtin_wxGetMultipleChoices
// %builtin size_t wxGetMultipleChoices(wxArrayInt& selections,const wxString& message,const wxString& caption,int n, const wxString *choices,wxWindow *parent = (wxWindow *) NULL,int x = -1, int y = -1, bool centre = TRUE, int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);
static int LUACALL builtin_wxGetMultipleChoices(lua_State *L)
{
    size_t returns = 0;
    // get number of arguments
    int argCount = lua_gettop(L);
    // int height = wxCHOICE_HEIGHT
    int height = (argCount >= 10 ? (int) getnumbertype(L, 10) : wxCHOICE_HEIGHT);
    // int width = wxCHOICE_WIDTH
    int width = (argCount >= 9 ? (int) getnumbertype(L, 9) : wxCHOICE_WIDTH);
    // bool centre = TRUE
    bool centre = (argCount >= 8 ? getbooleantype(L, 8) : TRUE);
    // int y = -1
    int y = (argCount >= 7 ? (int) getnumbertype(L, 7) : -1);
    // int x = -1
    int x = (argCount >= 6 ? (int) getnumbertype(L, 6) : -1);
    // wxWindow *parent = (wxWindow *) NULL
    wxWindow *parent = (argCount >= 5 ? (wxWindow *) getuserdatatype(L, 5, s_wxWindow) : (wxWindow *) NULL);
    // const wxString& choices[]
    int count = 0; SmartStringArray ptr; wxString *choices; ptr = choices = tostringarray(L, 4, count);
    // const wxString& caption
    wxString caption = lua2wx(getstringtype(L, 3));
    // const wxString& message
    wxString message = lua2wx(getstringtype(L, 2));
    // wxArrayInt& selections
    wxArrayInt selections;
    toarrayint(L, 1, selections);

    // call wxGetMultipleChoices
    returns = wxGetMultipleChoices(selections, message, caption, count, choices, parent, x, y, centre, width, height);

    tointtable(L, selections);

    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 2;
}
%end

%override wxWindow_GetHandle
// void *GetHandle() const
static int LUACALL wxWindow_GetHandle(lua_State *L)
{
    // get this
    wxWindow *self = ( wxWindow * ) getuserdatatype(L, 1, s_wxWindow );
    // call GetHandle
    void *handle = (void *)self->GetHandle();
    // push handle
    lua_pushlightuserdata(L, handle);
    // return the number of parameters
    return 1;
}
%end

%override %wxver22 wxFontMapper_Get
// static wxFontMapper *Get()
static int LUACALL wxFontMapper_Get(lua_State *L)
{
    wxFontMapper *returns;
    // call Get
    returns = wxTheFontMapper;
    // push the result datatype
    pushuserdatatype(L, s_wxFontMapper, returns );
    // return the number of parameters

    return 1;
}
%end

%override %wxver22 wxFontMapper_Set
// static wxFontMapper *Set(wxFontMapper *mapper)
static int LUACALL wxFontMapper_Set(lua_State *L)
{
    wxFontMapper *returns;
    // wxFontMapper *mapper
    wxTheFontMapper = (wxFontMapper *) getuserdatatype(L, 2, s_wxFontMapper);
    // call Set
    returns = wxTheFontMapper;
    // push the result datatype
    pushuserdatatype(L, s_wxFontMapper, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxMenuItem_constructor
// wxMenuItem(wxMenu *parentMenu = NULL, int id = wxID_SEPARATOR, const wxString& text = wxEmptyString, const wxString& help = wxEmptyString, wxItemKind = 0, wxMenu *subMenu = NULL)
static int LUACALL wxMenuItem_constructor(lua_State *L)
{
    wxMenuItem *returns;
    // get number of arguments
    int argCount = lua_gettop(L);
    // wxMenu *subMenu = NULL
    wxMenu *subMenu = (argCount >= 6 ? (wxMenu *) getuserdatatype(L, 6, s_wxMenu) : NULL);
    // bool isCheckable = FALSE
    // This used to be a bool instead of a wxItemKind
    wxItemKind itemkind = (wxItemKind)(argCount >= 5 ? getenumerationtype(L, 5) : wxITEM_NORMAL);
    // const wxString& help = wxEmptyString
    wxString help = (argCount >= 4 ? lua2wx(getstringtype(L, 4)) : wxString(wxEmptyString));
    // const wxString& text = wxEmptyString
    wxString text = (argCount >= 3 ? lua2wx(getstringtype(L, 3)) : wxString(wxEmptyString));
    // int id = wxID_SEPARATOR
    int id = (argCount >= 2 ? (int) getnumbertype(L, 2) : wxID_SEPARATOR);
    // wxMenu *parentMenu = NULL
    wxMenu *parentMenu = (argCount >= 1 ? (wxMenu *) getuserdatatype(L, 1, s_wxMenu) : NULL);
    // call constructor
    returns = new wxMenuItem(parentMenu, id, text, help, itemkind, subMenu);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxMenuItem, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxInputStream_Read
// wxInputStream& Read(void *buffer, size_t size)
static int LUACALL wxInputStream_Read(lua_State *L)
{    
    wxInputStream *returns;    
    // size_t size
    size_t size = (size_t) getnumbertype(L, 2);
    // void *buffer
    void *buffer = malloc(size);
    // get this
    wxInputStream *self = ( wxInputStream * ) getuserdatatype(L, 1, s_wxInputStream );
    if (buffer != NULL)
    {
        // call Read
        returns = &self->Read(buffer, size);
        // push the result datatype
        pushuserdatatype(L, s_wxInputStream, returns );
        lua_pushlstring(L, (const char *)buffer, size);
        free(buffer);
        return 2;
    }
    return 0;
}
%end

%override wxInputStream_ReadStream
// %rename ReadStream wxInputStream& Read(wxOutputStream& stream_in)
static int LUACALL wxInputStream_ReadStream(lua_State *L)
{    
    wxInputStream *returns;    
    // wxOutputStream& stream_in
    wxOutputStream *stream_in = (wxOutputStream *) getuserdatatype(L, 2, s_wxOutputStream);
    // get this
    wxInputStream *self = ( wxInputStream * ) getuserdatatype(L, 1, s_wxInputStream );
    // call Read
    returns = &self->Read(*stream_in);
    // push the result datatype
    pushuserdatatype(L, s_wxInputStream, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxOutputStream_Write
// wxOutputStream& Write(const void *buffer, size_t size)
static int LUACALL wxOutputStream_Write(lua_State *L)
{    
    wxOutputStream *returns;    
    // size_t size
    size_t size = (size_t) getnumbertype(L, 3);
    // const void *buffer
    const void *buffer = (void *) lua_tostring(L, 2);
    // get this
    wxOutputStream *self = ( wxOutputStream * ) getuserdatatype(L, 1, s_wxOutputStream );
    // call Write
    returns = &self->Write(buffer, size);
    // push the result datatype
    pushuserdatatype(L, s_wxOutputStream, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxOutputStream_WriteStream
// %rename WriteStream wxOutputStream& Write(wxInputStream& stream_in)
static int LUACALL wxOutputStream_WriteStream(lua_State *L)
{    
    wxOutputStream *returns;    
    // wxInputStream& stream_in
    wxInputStream *stream_in = (wxInputStream *) getuserdatatype(L, 2, s_wxInputStream);
    // get this
    wxOutputStream *self = ( wxOutputStream * ) getuserdatatype(L, 1, s_wxOutputStream );
    // call Write
    returns = &self->Write(*stream_in);
    // push the result datatype
    pushuserdatatype(L, s_wxOutputStream, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxFTP_SetTransferMode
// bool SetTransferMode(wxFTP::TransferMode mode)
static int LUACALL wxFTP_SetTransferMode(lua_State *L)
{    
    bool returns;    
    // wxFTP::TransferMode mode
    wxFTP::TransferMode mode = (wxFTP::TransferMode)(int) getnumbertype(L, 2);
    // get this
    wxFTP *self = ( wxFTP * ) getuserdatatype(L, 1, s_wxFTP );
    // call SetTransferMode
    returns = self->SetTransferMode(mode);
    // push the result flag
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxConfigBase_GetFirstGroup
// bool GetFirstGroup(wxString& str, long& index) const
static int LUACALL wxConfigBase_GetFirstGroup(lua_State *L)
{    
    bool returns;    
    // long& index
    long index = (long) getnumbertype(L, 3);
    // wxString& str
    wxString str = lua2wx(getstringtype(L, 2));
    // get this
    wxConfigBase *self = ( wxConfigBase * ) getuserdatatype(L, 1, s_wxConfigBase );
    // call GetFirstGroup
    returns = self->GetFirstGroup(str, index);
    // push the result flag
    lua_pushnumber(L, index);
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 2;
}
%end

%override wxConfigBase_GetFirstEntry
// bool GetFirstEntry(wxString& str, long& index) const
static int LUACALL wxConfigBase_GetFirstEntry(lua_State *L)
{    
    bool returns;    
    // long& index
    long index = (long) getnumbertype(L, 3);
    // wxString& str
    wxString str = lua2wx(getstringtype(L, 2));
    // get this
    wxConfigBase *self = ( wxConfigBase * ) getuserdatatype(L, 1, s_wxConfigBase );
    // call GetFirstEntry
    returns = self->GetFirstEntry(str, index);
    // push the result flag
    lua_pushnumber(L, index);
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 2;
}
%end

%override wxConfigBase_GetNextGroup
// bool GetNextGroup(wxString& str, long& index) const
static int LUACALL wxConfigBase_GetNextGroup(lua_State *L)
{    
    bool returns;    
    // long& index
    long index = (long) getnumbertype(L, 3);
    // wxString& str
    wxString str = lua2wx(getstringtype(L, 2));
    // get this
    wxConfigBase *self = ( wxConfigBase * ) getuserdatatype(L, 1, s_wxConfigBase );
    // call GetNextGroup
    returns = self->GetNextGroup(str, index);
    // push the result flag
    lua_pushnumber(L, index);
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 2;
}
%end

%override wxConfigBase_GetNextEntry
// bool GetNextEntry(wxString& str, long& index) const
static int LUACALL wxConfigBase_GetNextEntry(lua_State *L)
{    
    bool returns;    
    // long& index
    long index = (long) getnumbertype(L, 3);
    // wxString& str
    wxString str = lua2wx(getstringtype(L, 2));
    // get this
    wxConfigBase *self = ( wxConfigBase * ) getuserdatatype(L, 1, s_wxConfigBase );
    // call GetNextEntry
    returns = self->GetNextEntry(str, index);
    // push the result flag
    lua_pushnumber(L, index);
    lua_pushboolean(L, returns);
    // return the number of parameters
    return 2;
}
%end

%override wxInputStream_Unget
// %rename Unget size_t Ungetch(const char* buffer, size_t size)
static int LUACALL wxInputStream_Unget(lua_State *L)
{    
    size_t returns;    
    // size_t size
    size_t size = (size_t) getnumbertype(L, 3);
    // const char* buffer
    const char *buffer = (const char *) lua_tostring(L, 2);
    // get this
    wxInputStream *self = ( wxInputStream * ) getuserdatatype(L, 1, s_wxInputStream );
    // call Ungetch
    returns = self->Ungetch(buffer, size);
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxGLCanvas_constructor
// wxGLCanvas(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos, const wxSize& size, long style=0, const wxString& name="GLCanvas", int* attribList = 0, const wxPalette& palette = wxNullPalette)
static int LUACALL wxGLCanvas_constructor(lua_State *L)
{    
    SmartIntArray ptr; 
    int *attribList = NULL; 
    int count = 0; 
    wxGLCanvas *returns;
    // get number of arguments
    int argCount = lua_gettop(L);    
    // const wxPalette& palette = wxNullPalette
    const wxPalette *palette = (argCount >= 8 ? (wxPalette *) getuserdatatype(L, 8, s_wxPalette) : &wxNullPalette);
    // int* attribList = 0
    if (argCount >= 7)
    {
        ptr = attribList = tointarray(L, 7, count);
    }
    // const wxString& name="GLCanvas"
    wxString name = (argCount >= 6 ? lua2wx(getstringtype(L, 6)) : wxString(wxT("GLCanvas")));
    // long style=0
    long style = (argCount >= 5 ? (long) getnumbertype(L, 5) : 0);
    // const wxSize& size
    const wxSize *size = (wxSize *) getuserdatatype(L, 4, s_wxSize);
    // const wxPoint& pos
    const wxPoint *pos = (wxPoint *) getuserdatatype(L, 3, s_wxPoint);
    // wxWindowID id = -1
    wxWindowID id = (argCount >= 2 ? (wxWindowID) getnumbertype(L, 2) : -1);
    // wxWindow* parent
    wxWindow *parent = (wxWindow *) getuserdatatype(L, 1, s_wxWindow);
    // call constructor
    returns = new wxGLCanvas(parent, id, *pos, *size, style, name, attribList, *palette);
    // push the constructed class pointer
    pushuserdatatype(L, s_wxGLCanvas, returns );
    // return the number of parameters
    return 1;
}
%end

%override wxTreeItemId_GetValue
// double  GetValue() const;
static int LUACALL wxTreeItemId_GetValue(lua_State *L)
{    
    double returns;
    // get this
    wxTreeItemId *self = ( wxTreeItemId * ) getuserdatatype(L, 1, s_wxTreeItemId );
    // call GetValue
    returns = (long)self->m_pItem;
    // push the result number
    lua_pushnumber(L, returns);
    // return the number of parameters
    return 1;
}
%end

%override wxTreeItemId_SetValue
// void SetValue(double value);
static int LUACALL wxTreeItemId_SetValue(lua_State *L)
{        
    // double value
    long value = (long) getnumbertype(L, 2);
    // get this
    wxTreeItemId *self = ( wxTreeItemId * ) getuserdatatype(L, 1, s_wxTreeItemId );
    // call SetValue
    self->m_pItem = (void *) value;
    // return the number of parameters
    return 0;
}
%end

%override wxFrameLayout_GetPaneProperties
class cbLuaCommonPaneProperties : public cbCommonPaneProperties
{
public:
    cbLuaCommonPaneProperties()
       : cbCommonPaneProperties()
    {
    }

    void SetRealTimeUpdates(bool fOn = true)
    {
        mRealTimeUpdatesOn = fOn;
    }

    void SetOutOfPaneDrag(bool fOn = true)
    {
        mOutOfPaneDragOn = fOn;
    }

    void SetExactDockPrediction(bool fOn = true)
    {
        mExactDockPredictionOn = fOn;
    }

    void SetNonDestructFriction(bool fOn = true)
    {
        mNonDestructFrictionOn = fOn;
    }

    void SetShow3DPaneBorder(bool fOn = true)
    {
        mShow3DPaneBorderOn = fOn;
    }
};

// void GetPaneProperties(cbCommonPaneProperties& props, int alignment = FL_ALIGN_TOP)
static int LUACALL wxFrameLayout_GetPaneProperties(lua_State *L)
{
    // cbCommonPaneProperties& props
    cbLuaCommonPaneProperties *returns = new cbLuaCommonPaneProperties;
    // get number of arguments
    int argCount = lua_gettop(L);
    // int alignment = FL_ALIGN_TOP
    int alignment = (argCount >= 2 ? (int) getnumbertype(L, 2) : FL_ALIGN_TOP);
    // get this
    wxFrameLayout *self = ( wxFrameLayout * ) getuserdatatype(L, 1, s_wxFrameLayout );
    // call GetPaneProperties
    self->GetPaneProperties(*returns, alignment);
    //
    addToTrackedMemoryList(L, returns);
    pushuserdatatype(L, s_cbLuaCommonPaneProperties, returns);
    // return the number of parameters
    return 1;
}
%end

%override cbDockPane_HitTestPaneItems
// int HitTestPaneItems(const wxPoint& pos, cbRowInfo** ppRow, cbBarInfo** ppBar)
static int LUACALL cbDockPane_HitTestPaneItems(lua_State *L)
{
    int returns;
    // cbBarInfo** ppBar
    cbBarInfo *ppBar = NULL;
    // cbRowInfo** ppRow
    cbRowInfo *ppRow = NULL;
    // const wxPoint& pos
    const wxPoint *pos = (wxPoint *) getuserdatatype(L, 2, s_wxPoint);
    // get this
    cbDockPane *self = ( cbDockPane * ) getuserdatatype(L, 1, s_cbDockPane );
    // call HitTestPaneItems
    returns = self->HitTestPaneItems(*pos, &ppRow, &ppBar);
    // push the result number
    lua_pushnumber(L, returns);
    if (ppRow != NULL)
        pushuserdatatype(L, s_cbRowInfo, ppRow);
    else
        lua_pushnil(L);
    if (ppBar != NULL)
        pushuserdatatype(L, s_cbBarInfo, ppBar);
    else
        lua_pushnil(L);
    // return the number of parameters
    return 3;
}
%end

%override cbDockPane_GetBarResizeRange
// void GetBarResizeRange(cbBarInfo* pBar, int* from, int *till, bool forLeftHandle)
static int LUACALL cbDockPane_GetBarResizeRange(lua_State *L)
{
    // bool forLeftHandle
    bool forLeftHandle = getbooleantype(L, 3);
    // int *till
    int till;
    // int* from
    int from;
    // cbBarInfo* pBar
    cbBarInfo *pBar = (cbBarInfo *) getuserdatatype(L, 2, s_cbBarInfo);
    // get this
    cbDockPane *self = ( cbDockPane * ) getuserdatatype(L, 1, s_cbDockPane );
    // call GetBarResizeRange
    self->GetBarResizeRange(pBar, &from, &till, forLeftHandle);

    lua_pushnumber(L, from);
    lua_pushnumber(L, till);
    // return the number of parameters
    return 2;
}
%end

%override cbDockPane_GetRowResizeRange
// void GetRowResizeRange(cbRowInfo* pRow, int* from, int* till, bool forUpperHandle)
static int LUACALL cbDockPane_GetRowResizeRange(lua_State *L)
{
    // bool forUpperHandle
    bool forUpperHandle = getbooleantype(L, 3);
    // int* till
    int till;
    // int* from
    int from;
    // cbRowInfo* pRow
    cbRowInfo *pRow = (cbRowInfo *) getuserdatatype(L, 2, s_cbRowInfo);
    // get this
    cbDockPane *self = ( cbDockPane * ) getuserdatatype(L, 1, s_cbDockPane );
    // call GetRowResizeRange
    self->GetRowResizeRange(pRow, &from, &till, forUpperHandle);

    lua_pushnumber(L, from);
    lua_pushnumber(L, till);
    // return the number of parameters
    return 2;
}
%end
