// WatchCtl.cpp : Implementation of CWatchCtl
#include "stdafx.h"
#include "WatchCtl.h"

bool g_swapEndian = true;


inline unsigned short& SwapEndian(unsigned short& val)
{
	if (g_swapEndian)
		val = (WORD)((val >> 8) | (val << 8));

	return val;
}

inline short& SwapEndian(short& val)
{
	SwapEndian((WORD&)val);

	return val;
}

inline unsigned int& SwapEndian(unsigned int& val)
{
	if (g_swapEndian)
	{
		BYTE dest[8];
		BYTE* src = (BYTE*)&val;

		dest[0] = src[3];
		dest[1] = src[2];
		dest[2] = src[1];
		dest[3] = src[0];

		val = *(unsigned long*)dest;
	//	val = (val >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | (val << 24);
	}

	return val;
}

inline unsigned long& SwapEndian(unsigned long& val)
{
	SwapEndian((unsigned int&)val);
	return val;
}

template <typename T>
inline T*& SwapEndian(T*& val)
{
	SwapEndian((unsigned int&)val);
	return val;
}

inline int& SwapEndian(int& val)
{
	SwapEndian((unsigned int&)val);

	return val;
}

inline float& SwapEndian(float& val)
{
	SwapEndian((unsigned int&)val);

	return val;
}

inline double& SwapEndian(double& val)
{
	if (g_swapEndian)
	{
		BYTE dest[8];
		BYTE* src = (BYTE*)&val;

		dest[0] = src[7];
		dest[1] = src[6];
		dest[2] = src[5];
		dest[3] = src[4];

		dest[4] = src[3];
		dest[5] = src[2];
		dest[6] = src[1];
		dest[7] = src[0];

		val = *(double*)dest;
	}

	return val;
}

inline unsigned __int64& SwapEndian(unsigned __int64& val)
{
	if (g_swapEndian)
	{
		BYTE dest[8];
		BYTE* src = (BYTE*)&val;

		dest[0] = src[3];
		dest[1] = src[2];
		dest[2] = src[1];
		dest[3] = src[0];

		dest[4] = src[7];
		dest[5] = src[6];
		dest[6] = src[5];
		dest[7] = src[4];

		val = *(unsigned __int64*)dest;
	}

	return val;
}



CWatchCtl* g_watchCtl;
HANDLE g_processHandle;

lua_State theState;

class DebuggerEventsSink : public IDispEventImpl<1, DebuggerEventsSink, &__uuidof(EnvDTE::_dispDebuggerEvents), &EnvDTE::LIBID_EnvDTE, 7, 0>
{
public:
	BEGIN_SINK_MAP(DebuggerEventsSink)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispDebuggerEvents), 1, OnEnterRunMode)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispDebuggerEvents), 1, OnEnterDesignMode)
	END_SINK_MAP()

	HRESULT __stdcall OnEnterRunMode(EnvDTE::dbgEventReason Reason)
	{
		return S_OK;
	}

	HRESULT __stdcall OnEnterDesignMode(EnvDTE::dbgEventReason Reason)
	{
		return S_OK;
	}
};


DebuggerEventsSink m_debuggerEventsSink;
CComPtr<EnvDTE::_DebuggerEvents> m_debuggerEvents;


STDMETHODIMP CWatchCtl::OnConnect(IDispatch* pApplication)
{
	g_watchCtl = this;
	pApplication->QueryInterface(__uuidof(EnvDTE::_DTE), (LPVOID*)&m_pDTE);

	// Retrieve the list of events.
	CComPtr<EnvDTE::Events> pEvents;
	HRESULT hr = m_pDTE->get_Events( &pEvents );
	if ( FAILED( hr ) )
		return hr;

	// If there are events registered and we got here, then VStudio is
	// really messing with us...
	if (m_debuggerEvents)
	{
		m_debuggerEventsSink.DispEventUnadvise((IUnknown*)m_debuggerEvents.p);
		m_debuggerEvents = NULL;
	}

	// Register the event systems.
	if(SUCCEEDED(pEvents->get_DebuggerEvents((EnvDTE::_DebuggerEvents**)&m_debuggerEvents)))
	{
		m_debuggerEventsSink.DispEventAdvise((IUnknown*)m_debuggerEvents.p);
	}

	return S_OK;
}


STDMETHODIMP CWatchCtl::OnDisconnect()
{
	// If there are events registered and we got here, then VStudio is
	// really messing with us...
	if (m_debuggerEvents)
	{
		m_debuggerEventsSink.DispEventUnadvise((IUnknown*)m_debuggerEvents.p);
		m_debuggerEvents = NULL;
	}

	m_pDTE = NULL;

	return S_OK;
}


LRESULT CWatchCtl::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CWatchCtl* pT = static_cast<CWatchCtl*>(this);
	RECT r;
	bHandled = false;
	pT->GetClientRect(&r);
	_minClientSize.cx = (r.right-r.left);
	_minClientSize.cy = (r.bottom-r.top);
	_prevClientSize = _minClientSize;
	pT->GetWindowRect(&r);
	_minWindowSize.cx = (r.right-r.left);
	_minWindowSize.cy = (r.bottom-r.top);
	pT->DefineLayout();
	pT->SetScrollSize(_minClientSize);
	bHandled = false;

	m_luaStateNameWnd.Attach(GetDlgItem(IDC_VD_LUASTATEVARIABLENAME));
	m_evaluateSymbolRadio.Attach(GetDlgItem(IDC_EVALUATESYMBOL));
	m_globalsRadio.Attach(GetDlgItem(IDC_GLOBALS));
	m_registryRadio.Attach(GetDlgItem(IDC_REGISTRY));
	m_localsRadio.Attach(GetDlgItem(IDC_LOCALS));
	m_xbox360Check.Attach(GetDlgItem(IDC_XBOX360));
	m_symbolWnd.Attach(GetDlgItem(IDC_SYMBOL));
	m_tableTree.Attach(GetDlgItem(IDC_TABLES));
	m_keyValueList.Attach(GetDlgItem(IDC_KEYVALUES));

	m_keyValueList.InsertColumn(0, _T("Key"), LVCFMT_LEFT, 100, -1);
	m_keyValueList.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 200, -1);
	m_keyValueList.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 50, -1);

	m_globalsRadio.SetCheck(1);

	AttachForm(IDC_VD_ACCEPTLUASTATENAME, ATTACH_RIGHT);
	AttachForm(IDC_VD_ACCEPTLUASTATENAME, ATTACH_TOP);
	AttachForm(IDC_KEYVALUES, ATTACH_LEFT);
	AttachForm(IDC_KEYVALUES, ATTACH_TOP);
	AttachForm(IDC_KEYVALUES, ATTACH_RIGHT);
	AttachForm(IDC_KEYVALUES, ATTACH_BOTTOM);
	AttachForm(IDC_TABLES, ATTACH_TOP);
	AttachForm(IDC_TABLES, ATTACH_BOTTOM);
/*	AttachForm(IDC_IMG, ATTACH_HCENTER);
	AttachForm(IDC_IMG, ATTACH_VCENTER);
	AttachForm(IDC_LABEL, ATTACH_HCENTER);
	AttachForm(IDC_LABEL, ATTACH_VCENTER);
*/
/*	CRect rcVert;
	GetClientRect(&rcVert);

	// create the vertical splitter
	m_splitterWnd.Create(m_hWnd, rcVert, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_splitterWnd.SetSplitterPane(SPLIT_PANE_LEFT, m_tableTree.m_hWnd);
	m_splitterWnd.SetSplitterPane(SPLIT_PANE_RIGHT, m_keyValueList.m_hWnd);
*/
	return 0;
}


void CWatchCtl::Initialize()
{
	m_LuaStackObjectPtr = NULL;
	m_LuaObjectPtr = NULL;
	m_LuaStateOwnerPtr = NULL;
	m_LuaStatePtr = NULL;
	m_luaStatePtr = NULL;
	m_lookupObject = NULL;
}


















bool m_isXbox360;

// memory
typedef HRESULT (*fnDmGetMemory)(LPCVOID lpbAddr, DWORD cb, LPVOID lpbBuf,
    LPDWORD pcbRet);

fnDmGetMemory DmGetMemory;
HMODULE m_hModule;

	
void ReadMem(const void* fromPtr, void* toPtr, size_t size)
{
	if (m_isXbox360)
	{
		if (!DmGetMemory)
		{
			CRegKey devKey;
			if (devKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Classes\\CLSID\\{a5eb45d8-f3b6-49b9-984a-0d313ab60342}\\InprocServer32")) != ERROR_SUCCESS)
				throw -1;

			ULONG nChars = 1000;
			TCHAR buffer[1000];
			if (devKey.QueryStringValue(_T(""), buffer, &nChars) != ERROR_SUCCESS)
				throw -1;

			CString key = buffer;
			int slashPos = key.ReverseFind('\\');
			if (slashPos == -1)
				throw -1;

			key = key.Left(slashPos + 1);

			m_hModule = LoadLibrary(key + _T("xbdm.dll"));
			if (m_hModule == NULL)
				throw -1;

			DmGetMemory = (fnDmGetMemory)GetProcAddress(m_hModule, "_DmGetMemory@16");
			if (!DmGetMemory)
				throw -1;
		}

		SIZE_T numRead;
		HRESULT hr = DmGetMemory(fromPtr, size, toPtr, &numRead);
		if (FAILED(hr))
		{
			throw -1;
		}

		if (size != numRead)
			throw -1;
	}
	else
	{
		SIZE_T numRead;
		if (!ReadProcessMemory(g_processHandle, fromPtr, toPtr,
				size, &numRead))
		{
			throw -1;
		}

		if (size != numRead)
			throw -1;
	}
}


//////////////////////////////////////////////////////////////////////////
// ltable.h
//////////////////////////////////////////////////////////////////////////
#define node(t,i)	(&(t)->node[i])
#define key(n)		(&(n)->i_key)
#define val(n)		(&(n)->i_val)

//////////////////////////////////////////////////////////////////////////
// ltable.c
//////////////////////////////////////////////////////////////////////////
#define hashnum(t,n)	\
           (node(t, lmod(cast(lu_hash, cast(ls_hash, n)), sizenode(t))))

#define _offsetof(s,m)  (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))

void ReadMem_Table(void* fromIn, Table& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from, &to, sizeof(Table));
	SwapEndian(to.next);
	SwapEndian(to.prev);
	SwapEndian(to.ref);
	SwapEndian(to.metatable);
	SwapEndian(to.array);
	SwapEndian(to.node);
	SwapEndian(to.firstfree);
	SwapEndian(to.gclist);
	SwapEndian(to.sizearray);
}


void ReadMem_TObject(void* fromIn, TObject& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from, &to, sizeof(to));
	SwapEndian(to.tt);
	switch (to.tt)
	{
		case LUA_TBOOLEAN:			SwapEndian(to.value.b);		break;
		case LUA_TLIGHTUSERDATA:	SwapEndian(to.value.p);		break;
		case LUA_TNUMBER:			SwapEndian(to.value.n);		break;
		case LUA_TSTRING:			SwapEndian(to.value.gc);	break;
		case LUA_TTABLE:			SwapEndian(to.value.gc);	break;
		case LUA_TFUNCTION:			SwapEndian(to.value.gc);	break;
		case LUA_TUSERDATA:			SwapEndian(to.value.gc);	break;
		case LUA_TTHREAD:			SwapEndian(to.value.gc);	break;
		case LUA_TWSTRING:			SwapEndian(to.value.gc);	break;
	}
}


void ReadMem_LuaObject(void* fromIn, LuaObject& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from + _offsetof(LuaObject, m_next), &to.m_next, sizeof(to.m_next)); 
	ReadMem(from + _offsetof(LuaObject, m_prev), &to.m_prev, sizeof(to.m_prev)); 
	ReadMem(from + _offsetof(LuaObject, m_state), &to.m_state, sizeof(to.m_state)); 
	ReadMem_TObject(from + _offsetof(LuaObject, m_object), to.m_object);
	SwapEndian(to.m_next);
	SwapEndian(to.m_prev);
	SwapEndian(to.m_state);
}


void ReadMem_LuaStackObject(void* fromIn, LuaStackObject& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from, &to, sizeof(to));
	SwapEndian(to.m_state);
	SwapEndian(to.m_stackIndex);
}


void ReadMem_Node(void* fromIn, Node& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem_TObject(from, to.i_key);
	ReadMem_TObject(from + _offsetof(Node, i_val), to.i_val);
	ReadMem(from + _offsetof(Node, next), &to.next, sizeof(to.next));
	SwapEndian(to.next);
}


void ReadMem_TString(void* fromIn, TString& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from, &to, sizeof(to));
	SwapEndian(to.tsv.next);
	SwapEndian(to.tsv.prev);
	SwapEndian(to.tsv.ref);
	SwapEndian(to.tsv.hash);
	SwapEndian(to.tsv.len);
}


void ReadMem_GCObject(void* fromIn, GCObject& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from, &to, sizeof(to));
	SwapEndian(to.gch.next);
	SwapEndian(to.gch.prev);
	SwapEndian(to.gch.ref);
}


/*
** search function for integers
*/
const TObject *luaH_getnum (Table *t, int key)
{
	Table tBuf;
	ReadMem_Table(t, tBuf);

	if (1 <= key && key <= tBuf.sizearray)
	{
		return &tBuf.array[key-1];
	}
	else
	{
//#define node(t,i)	(&(t)->node[i])
//#define hashnum(t,n)	\
//           (node(t, lmod(cast(lu_hash, cast(ls_hash, n)), sizenode(t))))
		Node *n = hashnum(&tBuf, key);
		Node nBuf;
		ReadMem_Node(n, nBuf);

		do {  /* check whether `key' is somewhere in the chain */
			if (ttype(key(&nBuf)) == LUA_TNUMBER && nvalue(key(&nBuf)) == (lua_Number)key)
				return val(&nBuf);  /* that's it */
			else
			{
				n = nBuf.next;
				if (n)
					ReadMem_Node(n, nBuf);
			}
		} while (n);

		return &luaO_nilobject;
	}
}


CStringA ReadTStringToCString(TString* str)
{
	TString data;
	ReadMem_TString(str, data);

	CStringA out;
	LPSTR buf = out.GetBufferSetLength((int)data.tsv.len);
	ReadMem(str + 1, buf, data.tsv.len);
	out.ReleaseBuffer();

	return out;
}


TString* ReadTString(TString* str)
{
	TString data;
	ReadMem_TString(str, data);

	TString* outStr = (TString*)new BYTE[sizeof(TString) + data.tsv.len + 1];
	memcpy(outStr, &data, sizeof(TString));
	ReadMem(str + 1, outStr + 1, data.tsv.len);
	char* buf = (char*)(outStr + 1);
	buf[data.tsv.len] = 0;

	return outStr;
}


/*
** search function for strings
*/
const TObject* luaH_getstr (Table *t, TString *key, bool localString)
{
	Table tBuf;
	ReadMem_Table(t, tBuf);
	t = &tBuf;

	bool mustDeleteKey = false;
	if (!localString)
	{
		key = ReadTString(key);
		mustDeleteKey = true;
	}

	Node *n = hashstr(t, key);
	Node nBuf;
	ReadMem_Node(n, nBuf);

	do
	{  /* check whether `key' is somewhere in the chain */
		if (ttype(key(&nBuf)) == LUA_TSTRING)
		{
			CStringA destKeyStr = ReadTStringToCString(tsvalue(key(&nBuf)));
			if (destKeyStr == (char*)(key + 1))
				return val(n); /* that's it */
			goto DoNext;
		}
		else
		{
DoNext:			
			n = nBuf.next;
			if (n)
				ReadMem_Node(n, nBuf);
		}
	} while (n);
	return &luaO_nilobject;
}

/*
** main search function
*/
const TObject* luaH_get (Table *t, const TObject *key, bool localString) {
  switch (key->tt)
  {
    case LUA_TSTRING: return luaH_getstr(t, tsvalue(key), localString);
    case LUA_TWSTRING: return luaH_getstr(t, tsvalue(key), localString);
    case LUA_TNUMBER: {
      int k;
      lua_number2int(k, (nvalue(key)));
      if (cast(lua_Number, k) == nvalue(key))  /* is an integer index? */
        return luaH_getnum(t, k);  /* use specialized version */
      /* else go through */
    }
//    default: return luaH_getany(t, key);
  }

  //jj What to do about this?
  return &luaO_nilobject;  // Josh added this.
}

#if 0
static const TObject *luaV_index (lua_State *L, const TObject *t,
                                  TObject *key, int loop) {
  const TObject *tm = fasttm(L, hvalue(t)->metatable, TM_INDEX);
  if (tm == NULL) return &luaO_nilobject;  /* no TM */
  if (ttisfunction(tm)) {
    callTMres(L, tm, t, key);
    return L->top;
  }
  else return luaV_gettable(L, tm, key, loop);
}

static const TObject *luaV_getnotable (lua_State *L, const TObject *t,
                                       TObject *key, int loop) {
  TObject foundObj;
  const TObject *tm = luaT_gettmbyobj(L, t, TM_GETTABLE);
  if (ttisnil(tm))
  {
	foundObj.value.h = luaT_getmetatable(L, t);
	foundObj.tt = LUA_TTABLE;
	tm = &foundObj;

//    luaG_typeerror(L, t, "index");
//    return luaV_gettable(L, tm, key, loop);
  }
  if (ttisfunction(tm)) {
    callTMres(L, tm, t, key);
    return L->top;
  }
  else return luaV_gettable(L, tm, key, loop);
}
#endif 0

/* limit for table tag-method chains (to avoid loops) */
#define MAXTAGLOOP	100

// t has already been retrieved locally.
const TObject* luaV_gettable (lua_State *L, const TObject *t, TObject *key,
							  int loop, bool localKey)
{
	if (loop > MAXTAGLOOP)
		throw -1;
//		luaG_runerror(L, "loop in gettable");
//	TObject tBuf;
//	ReadMem(t, &tBuf, sizeof(TObject));
//	t = &tBuf;
	if (ttistable(t)) {  /* `t' is a table? */
		Table *h = hvalue(t);
		const TObject* v = luaH_get(h, key, localKey);  /* do a primitive get */
//		if (!ttisnil(&v)) return v;
		return v;
//jj		else return luaV_index(L, t, key, loop+1);
	}
//jj	else return luaV_getnotable(L, t, key, loop+1);
	return &luaO_nilobject;  // Josh added this.
}

Table* getmetatable(const TObject *o)
{
	global_State gs;
	GCObject gcobj;
	ReadMem_GCObject(o->value.gc, gcobj);
	ReadMem(theState.l_G, &gs, sizeof(global_State));
	switch (ttype(o))
	{
		case LUA_TTABLE:
		{
			SwapEndian(gcobj.h.metatable);
//			SwapEndian(gs._defaultmetatypes[LUA_TTABLE].value.gc->h);
			if (&gs._defaultmetatypes[LUA_TTABLE].value.gc->h == gcobj.h.metatable)
				return NULL;
			return gcobj.h.metatable;
		}
		case LUA_TUSERDATA:
		{
			SwapEndian(gcobj.u.uv.metatable);
//			SwapEndian(gs._defaultmetatypes[LUA_TUSERDATA].value.gc->h);
			if (&gs._defaultmetatypes[LUA_TUSERDATA].value.gc->h == gcobj.u.uv.metatable)
				return NULL;
			return gcobj.u.uv.metatable;
		}
		default: 
			return NULL;
	}
}

void acquiremetatable(Table* in, TObject* out)
{
	out->tt = LUA_TTABLE;
	out->value.p = in;
}

#define LUA_IDSIZE	60

struct lua_Debug {
  const char *event;     /* `call', `return' */
  const char *name;      /* (n) */
  const char *namewhat;  /* (n) `global', `tag method', `local', `field' */
  const char *what;      /* (S) `Lua' function, `C' function, Lua `main' */
  const char *source;    /* (S) */
  int currentline;       /* (l) */
  int nups;              /* (u) number of upvalues */
  int linedefined;       /* (S) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  int _ci;  /* active function */
};


int lua_getstack (lua_State *L, int level, lua_Debug *ar) {
  int status;
  if (L->ci - L->base_ci <= level) status = 0;  /* there is no such level */
  else {
    ar->_ci = (L->ci - L->base_ci) - level;
    status = 1;
  }
  return status;
}



static void infoLproto (lua_Debug *ar, Proto *f) {
//jj  ar->source = getstr(f->source);
  ar->source = "Unsupported";
  ar->linedefined = f->lineDefined;
  ar->what = "Lua";
}


void ReadMem_Closure(void* fromIn, Closure& to)
{
	BYTE* from = (BYTE*)fromIn;
	ReadMem(from, &to, sizeof(to));
}



static void funcinfo (lua_State *L, lua_Debug *ar, StkId func) {
  Closure clBuf;
  Closure *cl;
  TObject funcBuf;
  ReadMem_TObject(func, funcBuf);

  if (ttype(&funcBuf) == LUA_TFUNCTION)
    cl = clvalue(&funcBuf);
  else {
//jj    luaD_error(L, "value for `lua_getinfo' is not a function");
    cl = NULL;  /* to avoid warnings */
  }

  ReadMem_Closure(cl, clBuf);
  cl = &clBuf;				// Reassign.

  if (cl->c.isC) {
    ar->source = "=C";
    ar->linedefined = -1;
    ar->what = "C";
  }
//jj  else
//jj    infoLproto(ar, cl->l.p);
//jj  luaO_chunkid(ar->short_src, ar->source, LUA_IDSIZE);
  if (ar->linedefined == 0)
    ar->what = "main";
}


#define ci_func(ci)	(clvalue((ci)->base - 1))

static int isLmark (CallInfo *ci)
{
//	return (ttype(ci->base - 1) == LUA_TFUNCTION && !ci_func(ci)->c.isC);
	CallInfo ciBuf;
	ReadMem(ci, &ciBuf, sizeof(CallInfo));

	TObject objBuf;
	ReadMem(ciBuf.base - 1, &objBuf, sizeof(TObject));

	if (ttype(&objBuf) != LUA_TFUNCTION)
		return 0;
	
	Closure clBuf;
	ReadMem(clvalue(&objBuf), &clBuf, sizeof(Closure));

	return (!clBuf.c.isC);
}


#define isLua(ci)	(!((ci)->state & CI_C))
#define pcRel(pc, p)	(cast(int, (pc) - (p)->code) - 1)

static int currentpc (lua_State *L, CallInfo *ci) {
//  if (!isLua(ci)) return -1;  /* function is not a Lua function? */
  CallInfo ciBuf;
  ReadMem(ci, &ciBuf, sizeof(CallInfo));
  ci = &ciBuf;
  if (!isLua(&ciBuf)) return -1;  /* function is not a Lua function? */
//  if (ciBuf.pc == NULL) return 0;  /* function is not active */
  if (ci->state & CI_HASFRAME)  /* function has a frame? */
    ci->u.l.savedpc = *ci->u.l.pc;  /* use `pc' from there */
  /* function's pc is saved */
  return pcRel(ci->u.l.savedpc, ci_func(ci)->l.p);

//  if (ci == L->ci || ci->pc != (ci+1)->pc)  /* no other function using `pc'? */
//    return (*ci->pc - ci_func(ci)->l.p->code) - 1;
//  else  /* function's pc is saved */
//    return (ci->savedpc - ci_func(ci)->l.p->code) - 1;
#if 0
  CallInfo ci1Buf;
  ReadMem(ci + 1, &ci1Buf, sizeof(CallInfo));

  // ci_func(ci)->l.p
  TObject toCiBase_1Buf;
  ReadMem(ciBuf.base - 1, &toCiBase_1Buf, sizeof(TObject));

  Closure clBuf;
  ReadMem(clvalue(&toCiBase_1Buf), &clBuf, sizeof(Closure));
  
  Proto protoBuf;
  ReadMem(clBuf.l.p, &protoBuf, sizeof(Proto));

  if (ci == L->ci || ciBuf.pc != ci1Buf.pc)  /* no other function using `pc'? */
  {
	  Instruction* ciBufpc;
	  ReadMem(ciBuf.pc, &ciBufpc, sizeof(Instruction*));

	  return (ciBufpc - protoBuf.code) - 1;
  }
  else  /* function's pc is saved */
    return (ciBuf.savedpc - protoBuf.code) - 1;
#endif 0
}


static int currentline (CallInfo *ci) {
#if 0
  int pc = currentpc(ci);
  if (pc < 0)
    return -1;  /* only active lua functions have current-line information */
  else
    return getline(ci_func(ci)->l.p, pc);
#endif 
  return 0;
}


#if 0
static int currentline (lua_State *L, CallInfo *ci)
{
  if (!isLmark(ci))
    return -1;  /* only active lua functions have current-line information */
  else {
	CallInfo ciBuf;
	ReadMem(ci, &ciBuf, sizeof(CallInfo));

	TObject obj;
	ReadMem(ciBuf.base - 1, &obj, sizeof(TObject));

//    int *lineinfo = ci_func(&ciBuf)->l.p->lineinfo;
	return 0;
//jj    return luaG_getline(lineinfo, currentpc(L, ci), 1, NULL);
  }
}
#endif 0

#if 0
static void funcinfo (lua_Debug *ar, StkId func) {
  Closure *cl = clvalue(func);
  Closure clBuf;
  ReadMem(cl, &clBuf, sizeof(Closure));
  cl = &clBuf;

  if (cl->c.isC) {
    ar->source = "=[C]";
    ar->linedefined = -1;
    ar->what = "C";
  }
  else {
    ar->source = getstr(cl->l.p->source);
    ar->linedefined = cl->l.p->lineDefined;
    ar->what = (ar->linedefined == 0) ? "main" : "Lua";
  }
  luaO_chunkid(ar->short_src, ar->source, LUA_IDSIZE);
}


static int auxgetinfo (lua_State *L, const char *what, lua_Debug *ar,
                    StkId f, CallInfo *ci) {
  int status = 1;
  for (; *what; what++) {
    switch (*what) {
      case 'S': {
        funcinfo(ar, f);
        break;
      }
      case 'l': {
        ar->currentline = (ci) ? currentline(ci) : -1;
        break;
      }
      case 'u': {
        ar->nups = clvalue(f)->c.nupvalues;
        break;
      }
      case 'n': {
        ar->namewhat = (ci) ? getfuncname(ci, &ar->name) : NULL;
        if (ar->namewhat == NULL) {
          /* try to find a global name */
          if ((ar->name = travglobals(L, f)) != NULL)
            ar->namewhat = "global";
          else ar->namewhat = "";  /* not found */
        }
        break;
      }
      case 'f': {
        setobj2s(L->top, f);
        break;
      }
      default: status = 0;  /* invalid option */
    }
  }
  return status;
}


int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar) {
  int status = 1;

  lua_Debug arBuf;
  ReadMem(ar, &arBuf, sizeof(lua_Debug));
  ar = &arBuf;

  if (*what == '>') {
    StkId f = L->top - 1;

	TObject obj;
	ReadMem(f, &obj, sizeof(TObject));
	f = &obj;

    if (!ttisfunction(f))
	  throw -1;

    status = auxgetinfo(L, what + 1, ar, f, NULL);
    L->top--;  /* pop function */
  }
  else if (ar->i_ci != 0) {  /* no tail call? */
    CallInfo *ci = L->base_ci + ar->i_ci;
    lua_assert(ttisfunction(ci->base - 1));
    status = auxgetinfo(L, what, ar, ci->base - 1, ci);
  }
  else
    info_tailcall(L, ar);
  if (strchr(what, 'f')) incr_top(L);
  return status;
}

#endif 0


static Proto *getluaproto (CallInfo *ci) {
	CallInfo ciBuf;
	ReadMem(ci, &ciBuf, sizeof(CallInfo));

	TObject objBuf;
	ReadMem(ciBuf.base - 1, &objBuf, sizeof(TObject));

	Closure clBuf;
	ReadMem(clvalue(&objBuf), &clBuf, sizeof(Closure));

  return (isLmark(ci) ? clBuf.l.p : NULL);
}


#define getstr(ts)	cast(const char *, (ts) + 1)


/*
** Look for n-th local variable at line `line' in function `func'.
** Returns NULL if not found.
*/
bool luaF_getlocalname(const Proto *f, int local_number, int pc, CStringA& name)
{
	int i;
	Proto fBuf;
	ReadMem(f, &fBuf, sizeof(Proto));
	f = &fBuf;			// Reassign

	LocVar locVarBuf;

	for (i = 0; i < f->sizelocvars; i++)
	{
		ReadMem(&f->locvars[i], &locVarBuf, sizeof(LocVar));
		if (locVarBuf.startpc > pc)
			break;
		if (pc < locVarBuf.endpc) {  /* is variable active? */
			local_number--;
			if (local_number == 0)
			{
				TString tStr;
				ReadMem(locVarBuf.varname, &tStr, sizeof(TString));

				LPSTR buf = name.GetBufferSetLength(tStr.tsv.len);
				ReadMem(locVarBuf.varname + 1, buf, tStr.tsv.len);
				name.ReleaseBuffer();
				return true;
			}
		}
	}
	return false;  /* not found */
}

#if 0
static Proto *getluaproto (CallInfo *ci) {
  return (isLua(ci) ? ci_func(ci)->l.p : NULL);
}


const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n, TObject& obj) {
  const char *name;
  CallInfo *ci;
  Proto *fp;

  lua_Debug arBuf;
  ReadMem(ar, &arBuf, sizeof(lua_Debug));
  ar = &arBuf;

  name = NULL;
  ci = L->base_ci + ar->i_ci;
  fp = getluaproto(ci);
  if (fp) {  /* is a Lua function? */
    name = luaF_getlocalname(fp, n, currentpc(ci));
    if (name)
      ReadMem(ci->base+(n-1), &obj, sizeof(TObject));
  }
  return name;
}
#endif 0

int GetTableCount(TObject& obj)
{
//	StkId t;
//	const TObject *value;
	int n;
	Node *nd;
	Table *a = hvalue(&obj);
	Table aBuf;
	ReadMem_Table(a, aBuf);
	lua_Number max = 0;
	int i;
	i = sizearray(&aBuf);
	while (i--)
	{
		TObject obj;
		ReadMem_TObject(&aBuf.array[i], obj);
		if (ttype(&obj) != LUA_TNIL)
			break;
	}
	max = i+1;
	i = sizenode(&aBuf);

	Node ndBuf;
	nd = aBuf.node;
	ReadMem_Node(nd, ndBuf);
	while (i--) {
		if (ttype(key(&ndBuf)) != LUA_TNIL)
			max++;
		nd++;
		ReadMem_Node(nd, ndBuf);
	}
	n = cast(int, max);
	return n;
}


CStringW FormatObject(TObject& obj, CString& type)
{
	CStringW out;

	switch (ttype(&obj))
	{
		case LUA_TNONE:
			out = _T("NONE");
			type = _T("None");
			break;

		case LUA_TNIL:
			out = _T("NIL");
			type = _T("Nil");
			break;

		case LUA_TBOOLEAN:
			out = bvalue(&obj) ? _T("true") : _T("false");
			type = "Boolean";
			break;

		case LUA_TLIGHTUSERDATA:
			out.Format(L"0x%08x", pvalue(&obj));
			type = "LightUserData";
			break;

		case LUA_TNUMBER:
			out.Format(L"%.14g", nvalue(&obj));
			type = "Number";
			break;

		case LUA_TSTRING:
		{
			TString data;
			ReadMem_TString(tsvalue(&obj), data);

			CStringA temp;
			LPSTR buf = temp.GetBufferSetLength(data.tsv.len);
			ReadMem((TString*)(tsvalue(&obj)) + 1, buf, data.tsv.len);
			temp.ReleaseBuffer(data.tsv.len);

			out = L"\"" + CStringW(temp) + L"\""; 
			type = "String";
			break;
		}

		case LUA_TTABLE:
		{
			int tableCount = GetTableCount(obj);
			out.Format(L"count=%d", tableCount);
			type = "Table";
			break;
		}

		case LUA_TFUNCTION:
		{
			Closure cl;
			ReadMem_Closure(gcvalue(&obj), cl);

			out = cl.c.isC ? "CFUNCTION" : "FUNCTION";
			type = "Function";
			break;
		}

		case LUA_TUSERDATA:
		{
			Udata data;
			ReadMem(gcvalue(&obj), &data, sizeof(Udata));
			SwapEndian(data.uv.len);

			void* ptr;
			ptr = ((Udata*)obj.value.gc) + 1;
			if ( data.uv.len & 4 )
			{
				// The user data pointer is embedded.
				ReadMem( ptr, &ptr, sizeof(void*) );
			}

			out.Format(L"0x%08x", ptr);
			type = "Userdata";
			break;
		}

		case LUA_TTHREAD:
			out = L"[THREAD]";
			break;

		case LUA_TWSTRING:
		{
			TString data;

			ReadMem_TString(obj.value.gc, data);

			LPWSTR buf = out.GetBufferSetLength(data.tsv.len);
			ReadMem((void*)( ((TString*)obj.value.gc) + 1 ), buf, data.tsv.len * 2);
			if (g_swapEndian)
			{
				for (size_t i = 0; i < data.tsv.len; ++i)
					SwapEndian(buf[i]);
			}
			out.ReleaseBuffer(data.tsv.len);

			break;

/*			TString tStr;
			ReadMem(tsvalue(&obj), &tStr, sizeof(TString));

			LPSTR buf = out.GetBufferSetLength(tStr.tsv.len);
			ReadMem(tsvalue(&obj) + 1, buf, tStr.tsv.len);
			out.ReleaseBuffer();

			out = "STRING: \"" + out + "\""; 

			int size = min(data.tsv.len, maxOutBuf / 2);
			if (pHelper->ReadDebuggeeMemory(pHelper, (DWORDLONG)(obj.value.ts + 1), size * 2, &outBuf, &nGot)!=S_OK)
				return E_FAIL;
			if (nGot!=size * 2)
				return E_FAIL;
			outBuf[size * 2] = 0;
			outBuf[size * 2 + 1] = 0;

			sprintf(pResult, "USTRING: \"%S\"", (wchar_t*)outBuf);*/
			type = "WString";
			break;
		}
	}

	return out;
}


CStringW FormatObject(TObject* o, CString& type)
{
	TObject obj;
	ReadMem_TObject(o, obj);
	return FormatObject(obj, type);
}



bool lua_getlocal(lua_State *L, const lua_Debug *ar, int n, CStringA& name, TObject*& value)
{
	CallInfo *ci;
	Proto *fp;
	CallInfo ciBuf;

	name.Empty();

	ci = L->base_ci + ar->_ci;
	ReadMem(ci, &ciBuf, sizeof(CallInfo));

	fp = getluaproto(ci);
	if (fp)
	{  /* is a Lua function? */
		bool ret = luaF_getlocalname(fp, n, currentpc(L, ci), name);
		if (ret)
		{
//			value = FormatObject(ciBuf.base + (n - 1), type);
			value = ciBuf.base + (n - 1);
		}
		//    if (name)
		//      luaA_pushobject(L, ci->base+(n-1));  /* push value */
		return ret;
	}

	return false;
}


static TString *newlstr (lua_State *L, const char *str, size_t l, lu_hash h) {
  TString *ts;
  unsigned int stringbytes = sizestring(l);
  ts = cast(TString *, malloc(stringbytes));
  ts->tsv.len = l;
  ts->tsv.hash = h;
  ts->tsv.marked = 0;
  ts->tsv.tt = LUA_TSTRING;
  ts->tsv.reserved = 0;
  memcpy(ts+1, str, l*sizeof(char));
  ((char *)(ts+1))[l] = '\0';  /* ending 0 */
  return ts;
}


TString *luaS_newlstr (lua_State *L, const char *str, size_t l) {
  lu_hash h = (lu_hash)l;  /* seed */
  size_t step = (l>>5)+1;  /* if string is too long, don't hash all its chars */
  size_t l1;
  for (l1=l; l1>=step; l1-=step)  /* compute hash */
    h = h ^ ((h<<5)+(h>>2)+(unsigned char)(str[l1-1]));
  return newlstr(L, str, l, h);  /* not found */
}


/*
** max size of array part is 2^MAXBITS
*/
#define BITS_INT	32
#if BITS_INT > 26
#define MAXBITS		24
#else
#define MAXBITS		(BITS_INT-2)
#endif

/* check whether `x' < 2^MAXBITS */
#define toobig(x)	((((x)-1) >> MAXBITS) != 0)

/*
** returns the index for `key' if `key' is an appropriate key to live in
** the array part of the table, -1 otherwise.
*/
static int arrayindex (const TObject *key)
{
  if (ttype(key) == LUA_TNUMBER)
  {
    int k = cast(int, nvalue(key));
    if (cast(lua_Number, k) == nvalue(key) && k >= 1 && !toobig(k))
      return k;
  }
  return -1;  /* `key' did not match some condition */
}


/*
** returns the index of a `key' for table traversals. First goes all
** elements in the array part, then elements in the hash part. The
** beginning and end of a traversal are signalled by -1.
*/
static int luaH_index (lua_State *L, Table *t, TObject *key,
						bool localKey, bool localString) {
	Table tBuf;
	ReadMem_Table(t, tBuf);

	if (!localKey)
	{
		ReadMem_TObject(key, *key);
	}

	int i;
	if (ttype(key) == LUA_TNIL) return -1;  /* first iteration */
	i = arrayindex(key);
	if (0 <= i && i <= tBuf.sizearray) {  /* is `key' inside array part? */
		return i-1;  /* yes; that's the index (corrected to C) */
	}
	else
	{
		const TObject* v = luaH_get(t, key, localString);
		TObject obj;
		ReadMem_TObject((void*)v, obj);
		if (obj.tt == luaO_nilobject.tt)
		{
//jj			luaD_error(L, "invalid key for `next'");
			int hi = 5;
		}
		i = cast(int, (cast(const lu_byte *, v) -
			cast(const lu_byte *, val(node(&tBuf, 0)))) / sizeof(Node));
		return i + tBuf.sizearray;  /* hash elements are numbered after array ones */
	}
}


int luaH_next (lua_State *L, Table *t, TObject *key, bool localKey,
			   TObject& outValue)
{
	int i = luaH_index(L, t, key, localKey, false);  /* find original element */
	Table tBuf;
	ReadMem_Table(t, tBuf);
	for (i++; i < tBuf.sizearray; i++) {  /* try first array part */
		TObject arrayObj;
		ReadMem_TObject(&tBuf.array[i], arrayObj);
		if (ttype(&arrayObj) != LUA_TNIL) {  /* a non-nil value? */
			setnvalue(key, i+1);
			setobj2s(&outValue, &arrayObj);
			return 1;
		}
	}
	for (i -= tBuf.sizearray; i < sizenode(&tBuf); i++)
	{  /* then hash part */
		Node nodeBuf;
		ReadMem_Node(&tBuf.node[i], nodeBuf);
		if (ttype(val(&nodeBuf)) != LUA_TNIL) {  /* a non-nil value? */
			setobj(key, key(&nodeBuf));
			setobj(&outValue, val(&nodeBuf));
			return 1;
		}
	}
	return 0;  /* no more elements */
}


void CWatchCtl::OnPlusExpand(CTreeItem treeItem)
{
	// Let's see if it's already filled in.
	CTreeItem childItem = treeItem.GetChild();
	if (childItem)
	{
		CString childText;
		childItem.GetText(childText);
		if (childText != _T("***Expanding***"))
			return;
	}
	
	childItem.Delete();
	
	TObject key2Obj;
	key2Obj.tt = LUA_TNIL;
	bool isLocalKey = true;

	CTreeItem curTableItem = treeItem;

	TObject table = *(TObject*)treeItem.GetData();
    bool checkedmeta = false;
	while (true)
	{
		TObject outValue;
		if (!checkedmeta && !luaH_next(&theState, hvalue(&table), &key2Obj, isLocalKey, outValue))
        {
            if (!getmetatable(&table) || getmetatable(&table) == (void*) 0xfeeefeee)// || getmetatable((&tableObj) == tableObj.value.gc->h))
                break;

            checkedmeta = true;

            acquiremetatable(getmetatable(&table), &outValue);
        }
        else if (checkedmeta)
            break;      

		if (outValue.tt == LUA_TTABLE)
		{
			CString type;
			CStringW keyStr;
            if (!checkedmeta)
                keyStr = FormatObject(key2Obj, type);
            else
                keyStr = L"metatable";

			CTreeItem treeItem = curTableItem.AddHead(keyStr, -1);
			TObject* newValue = new TObject;
			*newValue = outValue;
			treeItem.SetData((DWORD)newValue);
			treeItem.AddHead(_T("***Expanding***"), -1);
		}
	}
	treeItem.SortChildren();
}


void CWatchCtl::FillInKeyValues(void* inTableObj, CTreeItem* treeItem)
{
	TObject tableObj = *(TObject*)inTableObj;
	m_keyValueList.DeleteAllItems();

	CStringW value;
	CString type;
	if (tableObj.tt != LUA_TTABLE)
	{
		value = FormatObject(tableObj, type);
		int item = m_keyValueList.InsertItem(m_keyValueList.GetItemCount() + 1, _T(""));
		m_keyValueList.SetItemText(item, 1, value);
		m_keyValueList.SetItemText(item, 2, type);
	}
	else
	{
		TObject key2Obj;
		key2Obj.tt = LUA_TNIL;
		bool isLocalKey = true;

		CTreeItem curTableItem;
		if (treeItem)
			curTableItem = *treeItem;

        bool checkedmeta = false;

		while (true)
		{
			TObject outValue;
			if (!checkedmeta && !luaH_next(&theState, hvalue(&tableObj), &key2Obj, isLocalKey, outValue))
            {
                if (!getmetatable(&tableObj) || getmetatable(&tableObj) == (void*) 0xfeeefeee)// || getmetatable((&tableObj) == tableObj.value.gc->h))
                    break;

                checkedmeta = true;
        
                acquiremetatable(getmetatable(&tableObj), &outValue);
            }
            else if (checkedmeta)
                break;      

			CStringW keyStr;
            if (!checkedmeta)
                keyStr = FormatObject(key2Obj, type);
            else
                keyStr = L"metatable";

			if (outValue.tt == LUA_TTABLE)
			{
				if (treeItem)
				{
					CTreeItem tableTreeItem = curTableItem.AddHead(keyStr, -1);
					TObject* newValue = new TObject;
					*newValue = outValue;
					tableTreeItem.SetData((DWORD)newValue);
					tableTreeItem.AddHead(_T("***Expanding***"), -1);
				}
			}

			CStringW value = FormatObject(outValue, type);

			int item = m_keyValueList.InsertItem(m_keyValueList.GetItemCount() + 1, keyStr);
			m_keyValueList.SetItemText(item, 1, value);
			m_keyValueList.SetItemText(item, 2, type);
		}

		if (treeItem)
			curTableItem.SortChildren();
	}


}


void CWatchCtl::Refresh()
{
	try
	{
		CComPtr<EnvDTE::Debugger> pDebugger;
		if (FAILED(m_pDTE->get_Debugger(&pDebugger)))
		{
			return;
		}

		CComPtr<EnvDTE::Process> pProcess;
		if (FAILED(pDebugger->get_CurrentProcess(&pProcess)))
		{
			return;
		}

		if (!pProcess)
		{
			return;
		}

		long processID;
		if (FAILED(pProcess->get_ProcessID(&processID)))
		{
			return;
		}

		m_pDebugger = pDebugger;
		m_processID = processID;

		m_isXbox360 = m_xbox360Check.GetCheck() != 0;
		g_swapEndian = m_isXbox360 ? true : false;

		ReadAddresses();

		lua_State* L = &theState;

		TObject m_lookupObject;
		m_lookupObject = *gt(L);

		m_tableTree.DeleteAllItems();
		m_keyValueList.DeleteAllItems();

		if (m_evaluateSymbolRadio.GetCheck() != 0)
		{
			if (m_LuaObjectPtr)
			{
				LuaObject luaObject;
				ReadMem_LuaObject(m_LuaObjectPtr, luaObject);
				m_lookupObject = luaObject.m_object;
			}

			if (!m_symbol.IsEmpty())
			{
				m_lookupObject = LookupSymbol(m_lookupObject, m_symbol);
			}
		}
		else if (m_globalsRadio.GetCheck() != 0)
		{
			StkId globals = gt(L);
			m_lookupObject = *globals;
		}
		else if (m_registryRadio.GetCheck() != 0)
		{
            global_State gs;
            ReadMem(L->l_G, &gs, sizeof(global_State));
//			SwapEndian(gs._registry);
			StkId registryPtr = &gs._registry;
			m_lookupObject = *registryPtr;
		}
		else if (m_localsRadio.GetCheck() != 0)
		{
#if 0
			// Read locals.
			lua_Debug debugInfo;
			if (lua_getstack(L, 0, &debugInfo) != 0)
			{
				lua_getinfo(L, "Sl", &debugInfo);

				// Wander the list of local variables.
				int i = 1;
				CString localName;
				CTreeItem curTableItem = m_tableTree.GetRootItem();
				TObject* localValue;
				while (lua_getlocal(L, &debugInfo, i++, localName, localValue))
				{
					TObject localValueObj;
					ReadMem(localValue, &localValueObj, sizeof(TObject));
					FillInKeyValues(&localValueObj, &curTableItem);
	//				lua_pop(L, 1);  // remove variable value
				}
			}
//			StkId registryPtr = registry(L);
//			m_lookupObject = *registryPtr;
#endif 0
			return;
		}

		if (!ttisnil(&m_lookupObject))
		{
			CTreeItem curTableItem = m_tableTree.GetRootItem().AddHead(_T("Object"), -1);
			TObject* newObj = new TObject;
			*newObj = m_lookupObject;
			curTableItem.SetData((DWORD)newObj);

			FillInKeyValues(&m_lookupObject, &curTableItem);
		}
	}
	catch (...)
	{
	}
}


#include "Lex.h"

TObject CWatchCtl::LookupSymbol(TObject obj, CString symbol)
{
	m_tableTree.DeleteAllItems();
	m_keyValueList.DeleteAllItems();

	TObject nilObj;
	nilObj.tt = LUA_TNIL;

	Lex lex;
	lex.Init(symbol);

	while (!lex.Eof())
	{
		if (obj.tt != LUA_TTABLE)
			return nilObj;

		LexToken token;
		lex.GetToken(token);
		if (token.IsInteger())
		{
			// Lookup number
			const TObject* v = luaH_getnum (hvalue(&obj), token.GetInteger());
			if (!v)
				return nilObj;

			ReadMem_TObject((void*)v, obj);

			// Continue trying.
		}
		else if (token.IsString()  ||  token.IsIdent())
		{
			CStringA str = CStringA(token.IsString() ? token.GetString() : token.GetIdent());
			// Lookup string
			TString* strObj = luaS_newlstr(&theState, str, str.GetLength());
//			TObject keyObj;
//			keyObj.tt = LUA_TSTRING;
//			keyObj.value.ts = strObj;

			const TObject* v = luaH_getstr(hvalue(&obj), strObj, true);
			free(strObj);

			if (!v)
				return nilObj;

			ReadMem_TObject((void*)v, obj);
		}
		else
		{
			return nilObj;
			assert(0);
		}

		lex.GetToken(token);
		if (token.GetType() != '.')
			break;
	}

	return obj;
}

	
void CWatchCtl::UpdateLuaObjectAddress(void)
{
	Refresh();

	lua_State* L = &theState;

/*	StkId globals = gt(L);

	CStringA varName = "Table";
	TString* strObj = luaS_newlstr(&state, "Table", 5);
	TObject keyObj;
	keyObj.tt = LUA_TSTRING;
	keyObj.value.ts = strObj;
//	keyObj.tt = LUA_TNUMBER;
//	keyObj.value.n = 1;

	TObject resultObj;
	resultObj = luaV_gettable(&state, globals, &keyObj, 0, true);

//	TObject resultObjBuf;
//	ReadMem(resultObj, &resultObjBuf, sizeof(TObject));
*/

//	CWindow editField = GetDlgItem(IDC_OUTPUT);
//	editField.SetWindowText(str);
//	OutputDebugString(str);


#if 0
	// Read locals.
	lua_Debug debugInfo;
	if (lua_getstack(&state, 0, &debugInfo) != 0)
	{
		lua_getinfo(&state, "Sl", &debugInfo);

		// Wander the list of local variables.
		int i = 1;
		CStringA localName;
		CStringA localValue;
		while (lua_getlocal(&state, &debugInfo, i++, localName, localValue))
		{
			OutputDebugString(localName);
			OutputDebugString("\t\t");
			OutputDebugString(localValue);
			OutputDebugString("\n");
/*			// If the name matches the local variable, then choose it.
			if (findName == localName)
			{
				break;
			}

			lua_pop(L, 1);  // remove variable value
*/
		}
	}
#endif 0
}



void CWatchCtl::UpdateLuaStateAddress(void)
{
#if 0
	lua_State* L = &state;
	StkId globals = gt(L);
#if 0
	CStringA varName = "Table";
	TString* strObj = luaS_newlstr(&state, "Table", 5);
	TObject keyObj;
	keyObj.tt = LUA_TSTRING;
	keyObj.value.ts = strObj;
//	keyObj.tt = LUA_TNUMBER;
//	keyObj.value.n = 1;

	TObject resultObj;
	resultObj = luaV_gettable(&state, globals, &keyObj, 0, true);

//	TObject resultObjBuf;
//	ReadMem(resultObj, &resultObjBuf, sizeof(TObject));

	TObject key2Obj;
	key2Obj.tt = LUA_TNIL;
	bool isLocalKey = true;

	while (true)
	{
		TObject outValue;
		if (!luaH_next(&state, hvalue(&resultObj), &key2Obj, isLocalKey, outValue))
			break;

		isLocalKey = false;

		OutputDebugString(FormatObject(outValue));
		OutputDebugString("\n");
	}

	free(keyObj.value.ts);
#endif 0


#if 0
	// Read locals.
	lua_Debug debugInfo;
	if (lua_getstack(&state, 0, &debugInfo) != 0)
	{
		lua_getinfo(&state, "Sl", &debugInfo);

		// Wander the list of local variables.
		int i = 1;
		CStringA localName;
		CStringA localValue;
		while (lua_getlocal(&state, &debugInfo, i++, localName, localValue))
		{
			OutputDebugString(localName);
			OutputDebugString("\t\t");
			OutputDebugString(localValue);
			OutputDebugString("\n");
/*			// If the name matches the local variable, then choose it.
			if (findName == localName)
			{
				break;
			}

			lua_pop(L, 1);  // remove variable value
*/
		}
	}
#endif 0

#endif 0
}

bool m_isHex;

void CWatchCtl::ReadAddresses()
{
	m_LuaStackObjectPtr = NULL;
	m_LuaObjectPtr = NULL;
	m_LuaStateOwnerPtr = NULL;
	m_LuaStatePtr = NULL;
	m_luaStatePtr = NULL;

	m_symbolWnd.GetWindowText(m_symbol);

	CString varName;
	m_luaStateNameWnd.GetWindowText(varName);
	if (varName.IsEmpty())
	{
		m_luaStatePtr = NULL;
	}
	else
	{
		if (varName.Left(2) == "0x")
		{
			_tscanf(varName, "0x%x", &m_luaStatePtr);
			m_LuaStatePtr = NULL;
		}
		else
		{
			CComPtr<EnvDTE::Expression> pExpression;
			if (SUCCEEDED(m_pDebugger->GetExpression(CComBSTR(varName), VARIANT_FALSE, -1, &pExpression)))
			{
				VARIANT_BOOL isValidValue;
				pExpression->get_IsValidValue(&isValidValue);

				if (isValidValue == VARIANT_TRUE)
				{
					CComBSTR bstrType;
					pExpression->get_Type(&bstrType);
					CStringA strType(bstrType);

					if (strType[strType.GetLength() - 1] != '*'  &&  strType[strType.GetLength() - 1] != '&')
					{
						varName = _T("&") + varName;
					}
				}
			}

			m_isHex = true;
			CComPtr<EnvDTE::Expression> pExpression2;
			if (SUCCEEDED(m_pDebugger->GetExpression(CComBSTR(varName), VARIANT_FALSE, -1, &pExpression2)))
			{
				VARIANT_BOOL isValidValue;
				pExpression2->get_IsValidValue(&isValidValue);

				CStringA strType;

				if (isValidValue == VARIANT_TRUE)
				{
					CComBSTR bstrType;
					pExpression2->get_Type(&bstrType);
					strType = bstrType;

					if (strType.Find("ManagedLuaPlus.LuaObject") != -1)
					{
						pExpression2 = NULL;
						m_isHex = false;

						if (SUCCEEDED(m_pDebugger->GetExpression(CComBSTR(_T("*") + varName + _T(".m_object")), VARIANT_FALSE, -1, &pExpression2)))
						{
							pExpression2->get_IsValidValue(&isValidValue);

							strType = "LuaPlus::LuaObject";
						}
						else
						{
							isValidValue = VARIANT_FALSE;
						}
					}
					else if (strType.Find("ManagedLuaPlus.LuaState") != -1)
					{
						pExpression2 = NULL;
						m_isHex = false;

						if (SUCCEEDED(m_pDebugger->GetExpression(CComBSTR(_T("*") + varName + _T(".m_state")), VARIANT_FALSE, -1, &pExpression2)))
						{
							pExpression2->get_IsValidValue(&isValidValue);
							strType = "LuaPlus::LuaState";
						}
						else
						{
							isValidValue = VARIANT_FALSE;
						}
					}
				}

				if (isValidValue == VARIANT_TRUE)
				{
					CComBSTR bstrValue;
					pExpression2->get_Value(&bstrValue);

					void* address;
					CStringA strValue(bstrValue);
					if (m_isHex)
						sscanf(strValue, "0x%x", &address);
					else
						sscanf(strValue, "%u", &address);

					if (strType.Find("LuaPlus::LuaStateOwner") != -1)
					{
						m_LuaStateOwnerPtr = address;
					}
					else if (strType.Find("LuaPlus::LuaState") != -1)
					{
						m_LuaStateOwnerPtr = NULL;
						m_LuaStatePtr = address;
					}
					else if (strType.Find("LuaPlus::LuaObject") != -1)
					{
						m_LuaObjectPtr = address;
						m_evaluateSymbolRadio.SetCheck(1);
					}
					else if (strType.Find("LuaPlus::LuaStackObject") != -1)
					{
						m_LuaStackObjectPtr = address;
						m_evaluateSymbolRadio.SetCheck(1);
					}
				}
			}
		}
	}

	if (m_processHandle)
		CloseHandle(m_processHandle);
	m_processHandle = OpenProcess(PROCESS_VM_READ, FALSE, m_processID);
	g_processHandle = m_processHandle;

	if (m_LuaStackObjectPtr)
	{
		LuaStackObject luaStackObject;
		ReadMem_LuaStackObject( m_LuaStackObjectPtr, luaStackObject );
		m_LuaStatePtr = luaStackObject.m_state;
	}

	if (m_LuaObjectPtr)
	{
		LuaObject luaObject;
		ReadMem_LuaObject( m_LuaObjectPtr, luaObject );
		m_LuaStatePtr = luaObject.m_state;
	}
	
	if (m_LuaStateOwnerPtr)
	{
		ReadMem( m_LuaStateOwnerPtr, &m_LuaStatePtr, sizeof( void* ) );
		SwapEndian(m_LuaStatePtr);
	}
	
	if (m_LuaStatePtr)
	{
		LuaState luaState;
		ReadMem( m_LuaStatePtr, &luaState, sizeof( LuaState ) );
		SwapEndian(luaState.m_state);
		m_luaStatePtr = luaState.m_state;
	}

	ReadMem(m_luaStatePtr, &theState, sizeof(lua_State));
	BYTE* from = (BYTE*)m_luaStatePtr;
	ReadMem(from + _offsetof(lua_State, l_G), &theState.l_G, sizeof(theState.l_G));
	SwapEndian(theState.l_G);
	ReadMem_TObject(from + _offsetof(lua_State, _gt), theState._gt);
}


LRESULT CWatchCtl::OnTvnItemexpandingTables(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CTreeItem treeItem(pNMTreeView->itemNew.hItem, &m_tableTree);
	OnPlusExpand(treeItem);

	return 0;
}

LRESULT CWatchCtl::OnTvnSelchangedTables(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CTreeItem treeItem(pNMTreeView->itemNew.hItem, &m_tableTree);
	TObject table = *(TObject*)treeItem.GetData();
	FillInKeyValues(&table, NULL);

	return 0;
}

LRESULT CWatchCtl::OnBnClickedGlobals(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Refresh();

	return 0;
}

LRESULT CWatchCtl::OnBnClickedRegistry(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Refresh();

	return 0;
}


LRESULT CWatchCtl::OnBnClickedLocals(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Refresh();

	return 0;
}


LRESULT CWatchCtl::OnBnClickedEvaluatesymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Refresh();

	return 0;
}

LRESULT CWatchCtl::OnEnChangeSymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_evaluateSymbolRadio.SetCheck(1);

	return 0;
}

LRESULT CWatchCtl::OnBnClickedAccept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Refresh();

	return 0;
}
