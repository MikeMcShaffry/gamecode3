/////////////////////////////////////////////////////////////////////////////
// Purpose:     Internals of wxLua
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_LUA_INTERNALS_H
#define WX_LUA_INTERNALS_H

#ifdef __GNUG__
    #pragma interface "wxLuaInternals.h"
#endif

#include "wx/treectrl.h"
#include "wxLua.h"
#include "wxLuaInterpreter.h"

extern "C"
{
    #include "lualib.h"
    #include "lauxlib.h"
}

// change the name of some defines to avoid clashes
// with standard lua functions.

#define wxFileRead        read
#define wxFileWrite       write
#define wxFileReadWrite   read_write
#define wxFileWriteAppend write_append

#define wxDataObjectGet  wxDataObject::Get
#define wxDataObjectSet  wxDataObject::Set

#if defined(__WXGTK__)
    #define wxPlatformGTK     wxVERSION_NUMBER
#elif  defined(__WXMSW__)
    #define wxPlatformWindows wxVERSION_NUMBER
#elif  defined(__WXMAC__)
    #define wxPlatformMac     wxVERSION_NUMBER
#endif

// ----------------------------------------------------------------------------
// wxLua type enums and structs
// ----------------------------------------------------------------------------

enum   LuaType                      // The type of a Lua method
{
    LuaDelete        = 1,           // gc routine
    LuaConstructor   = 2,           // constructor
    LuaDestructor    = 4,           // destructor (not used)
    LuaMethod        = 8,           // class method
    LuaGlobal        = 16,          // global method (not really related to the class)
    LuaGetProp       = 32,
    LuaSetProp       = 64
};

struct WXLUAMETHOD                  // defines a LUA method or property
{
    LuaType        type;            // type (one of the above enumeration values)
    const char    *name;            // name of the method or property
    lua_CFunction  func;            // function that implements the method or property
    int            params;          // number of params to the method (required + optional)
    int            required;        // number of required params
};

struct WXLUACLASS                   // defines a LUA class interface
{
    const char    *name;            // name of the class
    WXLUAMETHOD   *methods;         // pointer to methods for this class
    int            num_methods;     // number of methods
    int            baseclassindex;  // index in the class list to the base class
    wxClassInfo   *pClassInfo;      // pointer to the wxClassInfo associated with this class
    int           *class_tag;       // tag for user data allocated by ourselves
                                    // and therefore needs gc.
};

struct WXLUADEFINE                  // defines a wxWidgets define for wxLua
{
    const char    *name;            // name
    double         value;           // numeric value
};

struct WXLUASTRING                  // defines a wxWidgets define for wxLua
{
    const char    *name;            // name
    const wxChar  *strValue;        // string value
};

struct WXLUAEVENT                   // defines a wxWidgets Event for wxLua
{
    const int  *eventType;          // new wxWidgets event type
    const char *eventName;          // name of the event
    int        *eventClassTag;      // lua class tag
};

struct WXLUAOBJECT                  // Defines a wxWidgets object or pointer for wxLua
{
    const void  *objPtr;            // a pointer to the object or pointer
    const void **pObjPtr;           // a pointer to the object or pointer
    const char  *objName;           // the name of the object or pointer
    int         *objClassTag;       // the class tag of the object or pointer.
};

// Functions defined in wxLuaWrap.cpp
extern WXLUACLASS  *GetClassList(size_t &count);
extern WXLUADEFINE *GetDefineList(size_t &count);
extern WXLUASTRING *GetStringList(size_t &count);
extern WXLUAEVENT  *GetEventList(size_t &count);
extern WXLUAOBJECT *GetObjectList(size_t &count);

// ----------------------------------------------------------------------------
// wxLuaStateVariables - state variables for a wxLua instance
// ----------------------------------------------------------------------------

class wxLuaStateVariables
{
public:
    wxLuaStateVariables();
    ~wxLuaStateVariables();
    
    void ClearCallbacks();

    bool         m_typesRegistered; // wxWidgets classes are registed to lua
    int          m_startTag;        // The first wxLua allocated lua tag
    int          m_lastTag;         // The last wxLua lua tag of registered classes
    int          m_functionTag;     // The lua tag for function objects.
    int          m_wxLuaTable;      // The lua tag for the wxLua private tables
    int          m_wxNull;          // The lua tag for wxNull (for NULL pointers)
    int          m_wxDeleteTable;   // The lua tag for the wxWindow destroy tracking table

    wxEventType  m_inEventType;     // wxEventType set when in event, else -1

    wxHashTable *m_pDerivedList;    // The list of derived objects.
    wxHashTable *m_pTrackedList;    // The list of tracked objects.
    wxList      *m_pAppHandlerList; // The list of event objects associated with the application object
    wxList      *m_pDestroyHandlerList; // The list of wxLuaDestroyCallbacks installed 
    wxList      *m_pWindowList;     // The list of all wxWindow objects, wxWidgets will delete these
                                    //    but for an embedded program they must be deleted before
                                    //    deleting shutting down the interpreter, else they dangle
                                    //    used in ~wxLuaInterpreter
};

// GET_LUASTATEVARS_XXX returns an instance of the wxLuaStateVariables for the
//    corresponding lua_State

extern wxLuaStateVariables *wxFindLuaStateVariables(lua_State *L);
extern wxLuaInterpreter    *wxFindLuaInterpreter(lua_State *L);

// these follow wxCHECK_MSG/RET semantics
#define GET_LUASTATEVARS_MSG(L, return_val) \
    wxLuaStateVariables *stateVars = wxFindLuaStateVariables(L); \
    wxCHECK_MSG(stateVars, return_val, wxT("invalid wxLuaInterpreter"));

#define GET_LUASTATEVARS_RET(L) \
    wxLuaStateVariables *stateVars = wxFindLuaStateVariables(L); \
    wxCHECK_RET(stateVars, wxT("invalid wxLuaInterpreter"));

// ----------------------------------------------------------------------------
// SmartStringArray - A class to wrap an array of wxStrings with an
//                    automatic destructor for making binding easier
// ----------------------------------------------------------------------------
class SmartStringArray
{
public:
    SmartStringArray() : m_choices(NULL) { }
    ~SmartStringArray() { if (m_choices != NULL) delete[] m_choices; }

    void operator =(wxString *choices) { m_choices = choices; }

private:
    wxString *m_choices;
};

// ----------------------------------------------------------------------------
// SmartIntArray - A class to wrap an array of ints with an
//                 automatic destructor for making binding easier
// ----------------------------------------------------------------------------
class SmartIntArray
{
public:
    SmartIntArray() : m_choices(NULL) { }
    ~SmartIntArray() { if (m_choices != NULL) delete[] m_choices; }

    void operator =(int *choices) { m_choices = choices; }

private:
    int *m_choices;
};

// ----------------------------------------------------------------------------
// DECLARE_ENCAPSULATION and IMPLEMENT_ENCAPSULATION
//     Declare the macros used to define and implement classes that
//     wrap non-wxObject derived pointers used by wxLua.
// ----------------------------------------------------------------------------

#define DECLARE_ENCAPSULATION(theObject) \
class theObject; \
void LUACALL addToTrackedMemoryList(lua_State *, theObject *); \
class theObject##Object : public wxObject \
{ \
public: \
    theObject##Object(theObject *p_##theObject); \
    ~theObject##Object(); \
private: \
    theObject *m_p##theObject; \
};

#define IMPLEMENT_ENCAPSULATION(theObject) \
theObject##Object::theObject##Object(theObject *p##theObject) : \
    m_p##theObject(p##theObject) \
{ \
} \
theObject##Object::~theObject##Object() \
{ \
    if (m_p##theObject != NULL) \
        delete m_p##theObject; \
} \
void LUACALL addToTrackedMemoryList(lua_State *L, theObject *p##theObject) \
{ \
    GET_LUASTATEVARS_RET(L) \
    if (stateVars->m_pTrackedList == NULL) \
        stateVars->m_pTrackedList = new wxHashTable(wxKEY_INTEGER); \
    if (stateVars->m_pTrackedList != NULL) \
        stateVars->m_pTrackedList->Put((int)p##theObject, new theObject##Object(p##theObject)); \
}

// Add a wxWidgets object to the list of tracked objects for
// garbage collection purposes.
void LUACALL addToTrackedMemoryList(lua_State *L, wxObject *);
bool LUACALL removeTrackedMemory(lua_State *L, void *pObject, bool fDelete = true);
void LUACALL addToTrackedWindowList(lua_State *L, wxWindow *);

// ----------------------------------------------------------------------------
// wxLuaObject  - wraps a reference to a Lua object inside a wxObject-derived
//                class so that it can be used for user data, etc.
//                Extended to support the wxValidator classes.
// ----------------------------------------------------------------------------

class wxLuaObject : public wxObject
{
public:
    wxLuaObject();
    wxLuaObject(lua_State *, int iParam, int iRef = 1);
    ~wxLuaObject();
    bool GetObject();
    void SetObject(int iParam);
    void SetObject(lua_State *, int iParam);

    bool       *GetBoolPtr();
    int        *GetIntPtr();
    wxString   *GetStringPtr();
    wxArrayInt *GetArrayPtr();

    int         GetAllocationFlags() const;
    void        ModifyAllocationFlags(int iValue);

    lua_State  *GetLuaState() const { return m_luaState; }

private:
    lua_State *m_luaState;
    int        m_iReference;
    bool       m_allocatedBool;
    bool       m_bool;
    bool       m_allocatedInt;
    int        m_int;
    bool       m_allocatedString;
    wxString   m_string;
    bool       m_allocatedArray;
    wxArrayInt m_arrayInt;
    DECLARE_DYNAMIC_CLASS(wxLuaObject)
};

class wxLuaTreeItemData : public wxTreeItemData
{
private:
	double m_value;

public:
	wxLuaTreeItemData(double value = 0) : m_value(value) {}
	double GetValue() const { return m_value; }
	void SetValue(double value) { m_value = value; }
};

// ----------------------------------------------------------------------------
// C lua helper functions
// ----------------------------------------------------------------------------

// get a user data type (either allocated by us if the tag is iTagS otherwise
// system allocated iTagP. if the data type does not have the correct tag,
// or if the parameter iParam is nil, NULL is returned.
// otherwise an error occurs
void * LUACALL getuserdatatype(lua_State *, int iParam, int iTag);
// set a user data type
void LUACALL pushuserdatatype(lua_State *, int iTag, const void *data);

// helper functions to get numbers, booleans and strings safer
const char * LUACALL getstringtype(lua_State *, int iParam);
bool LUACALL getbooleantype(lua_State *, int iParam);
long LUACALL getenumerationtype(lua_State *, int iParam);
double LUACALL getnumbertype(lua_State *, int iParam);

// convert a parameter which is presumably a table into a string array.
// returns the number of strings in the array in count.
// the return value may need to be deleted.
wxString * LUACALL tostringarray(lua_State *, int iParam, int &count);
// convert a parameter which is presumably a table into a character pointer array.
// returns the number of character strings in the array in count.
// the return value may need to be deleted.
const char ** LUACALL tochararray(lua_State *, int iParam, int &count);
// convert a parameter which is presumably a wxArrayString reference a string array.
// returns the table containing the strings on the Lua stack.
// returns the number of items in the table
int LUACALL tostringtable(lua_State *, wxArrayString &files);
int LUACALL tointtable(lua_State *L, const wxArrayInt &table);
// convert a parameter which is presumably a table into a integer array.
// returns the number of ints in the array in count.
// the return value may need to be deleted.
int * LUACALL tointarray(lua_State *, int iParam, int &count);
int   LUACALL toarrayint(lua_State *L, int iParam, wxArrayInt &intArray);
 // register all the generated classes
void LUACALL registerGeneratedClasses(lua_State *L, int tableOffset, bool registerTypes);

 // register non-generated functions and types
void LUACALL registerOthers(lua_State *L, bool registerTypes);

// free memory associated with the lua call class.
int LUACALL functionGarbageCollect(lua_State *L);

// memory deallocation function
int LUACALL garbageCollect(lua_State *L);
// lua 'set table' tag method handler
int LUACALL setTableFunc(lua_State *L);
// lua 'get table' tag method handler
int LUACALL getTableFunc(lua_State *L);
// call a Lua function
int LUACALL callFunction(lua_State *L);

// global function in Lua for wiring
int LUACALL ConnectEvent(lua_State *L);
int LUACALL DisconnectEvent(lua_State *L);

// get the method or 'get' property for the class at cpIndex
WXLUAMETHOD * LUACALL getLuaMethod(WXLUACLASS *pClass, const char *cpIndex, bool &isProperty);
// get the property for the class at cpIndex
WXLUAMETHOD * LUACALL getLuaProperty(WXLUACLASS *pClass, const char *cpIndex, bool isSet);

// get the derived method for the class cpIndex
lua_State * LUACALL getDerivedMethod(void *pObject, const char *method, lua_State *L = NULL);

int LUACALL LuaCall(lua_State *L, int narg, int clear);
int LUACALL LuaDoFile(lua_State *L, const wxString &file);
int LUACALL LuaDoBuffer(lua_State *L, const char *buffer, size_t len, const char *name);

// Check the return value of LuaDoFile, LuaDoBuffer, lua_dostring, lua_dofile
//   returns TRUE for no error, if error fills msg with a useful message if !NULL
bool wxLuaCheckRunError(int ret_val, wxString *msg);

// create a reference to the object at index iParam in the Lua index
int LUACALL tinsert(lua_State *luaState, int iParam);
// push onto the top of the stack the object referenced by iReference
bool LUACALL tget(lua_State *luaState, int iReference);
// remove a Lua reference
bool LUACALL tremove(lua_State *luaState, int iReference);
// Get the number of items in the reference table
int  LUACALL tgetn(lua_State *luaState);
// Display an error message
void LUACALL terror(lua_State *luaState, const char *errorMsg);
// push onto the top of the stack an userdata object containing u using tag to set the metatable
void LUACALL tpushusertag   (lua_State *L, const void *u, int tag);
// get the tag of the object at index
int  LUACALL ttag (lua_State *L, int index);
// return the user data in the userdata object at index, if reset is true clear the user data
void * LUACALL ttouserdata (lua_State *L, int index, bool reset = false);
// allocate a new metatable return its reference
int   LUACALL tnewtag(lua_State *L);
int   LUACALL tnewweaktag(lua_State *L, bool fWeakKey, bool fWeakData);
// set the metatable of the object at top of stack from the reference tag
void LUACALL tsettag(lua_State *L, int tag);
// set a metamethod for the metatable referenced by tag with the supplied name and function.
// if pClass is non-null set the upvalue of the function to the supplied class
bool LUACALL tsettagmethod(lua_State *L, int tag, const char *method, lua_CFunction func, void *pClass = NULL);

#define tpushliteralstring(L, n) \
    lua_pushlstring(L, n, sizeof(n)-1)

#define TLUA_NOTAG 0

class wxLuaCheckStack
{
public:
    wxLuaCheckStack(lua_State *lua_state, const wxString &msg);
    ~wxLuaCheckStack();
    void TestStack(const wxString &msg);

    static void DumpGlobals(lua_State *lua_state);
    static void DumpTable(lua_State *lua_state, const wxString &tableName);
    static void DumpTable(lua_State *lua_state, int stackIndex);

private:
    static void     DumpType(lua_State *lua_state, const wxString &name, int index, wxString &outputbuffer);
    static wxString GetTableInfo(lua_State *lua_state, int index);
    static wxString MakeNumber(double dnum);
    static wxString GetUserDataInfo(lua_State *lua_state, int index);

    lua_State *m_luastate;
    wxString   m_msg;
    int        m_top;
};

// ----------------------------------------------------------------------------
// wxLuaFunction - a proxy class to store a reference to a Lua function.
// ----------------------------------------------------------------------------

class wxLuaFunction
{
public:
    wxLuaFunction(WXLUAMETHOD *pMethod, WXLUACLASS *pClass, void *pObject)
        : m_pMethod(pMethod),
          m_pClass(pClass),
          m_pObject(pObject)
    {
    }

    ~wxLuaFunction() {}

    int CallMethod(lua_State *L)
    {
        lua_remove(L, 1);               // remove this line to restore calling
                                        // methods using the dot notation
                                        // otherwise the colon notation *must* be used.
        return (*m_pMethod->func)(L);
    }

    void *GetObject() const { return m_pObject; }

private:
    WXLUAMETHOD *m_pMethod;
    WXLUACLASS  *m_pClass;
    void        *m_pObject;
};

// ----------------------------------------------------------------------------
// String functions - convert between Lua (ansi string) and wxString (encoded)
// ----------------------------------------------------------------------------

// Convert a 8-bit Lua String into wxString
inline wxString lua2wx(const char * luastr)
{
    if (luastr == NULL)
        return wxEmptyString;
    
    return wxConvertMB2WX(luastr);
}

// Convert a wxString to 8-bit Lua String
inline const wxCharBuffer wx2lua(const wxString& wxstr)
{
    wxCharBuffer buffer(wxConvertWX2MB(wxstr.c_str()));
    return buffer;
}

// Convert a wxString to 8-bit Lua Buffer and store it
class LuaCharBuffer
{
public:
    LuaCharBuffer(const wxString &wxstr)
        : m_buffer((const char *)NULL)
    {
        wxCharBuffer charBuffer = wxConvertWX2MB(wxstr.c_str());
        m_buffer = charBuffer;
    }

    ~LuaCharBuffer() {}

    size_t Length() const { return strlen((const char*)m_buffer); }
    const char *GetData() const { return (const char*)m_buffer; }

    wxCharBuffer m_buffer;
};

#endif // WX_LUA_INTERNALS_H
