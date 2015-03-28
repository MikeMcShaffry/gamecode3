#pragma once

//////////////////////////////////////////////////////////////////////////
// lua.h
//////////////////////////////////////////////////////////////////////////
/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_GLOBALSINDEX	(-10001)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/*
** basic types
*/
#define LUA_TNONE	(-1)

#define LUA_TNIL	0
#define LUA_TBOOLEAN	1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER	3
#define LUA_TSTRING	4
#define LUA_TTABLE	5
#define LUA_TFUNCTION	6
#define LUA_TUSERDATA	7
#define LUA_TTHREAD 8
#define LUA_TWSTRING	9
#define LUA_NTYPES 11


struct lua_State;

typedef int (*lua_CFunction) (lua_State *L);

typedef void* (*lua_ReallocFunction)(void* ptr, int oldsize, int size, void* data, const char* allocName, unsigned int flags);
typedef void (*lua_FreeFunction)(void* ptr, int oldsize, void* data);

typedef struct lua_Debug lua_Debug;  /* activation record */

typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);


//////////////////////////////////////////////////////////////////////////
// llimits.h
//////////////////////////////////////////////////////////////////////////
/* type of numbers in Lua */
#ifndef LUA_NUMBER
typedef double lua_Number;
#else
typedef LUA_NUMBER lua_Number;
#endif


/* an unsigned integer to hold hash values */
typedef unsigned int lu_hash;
/* its signed equivalent */
typedef int ls_hash;

/* an unsigned integer big enough to count the total memory used by Lua; */
/* it should be at least as large as size_t */
typedef unsigned long lu_mem;

/* an integer big enough to count the number of strings in use */
typedef long ls_nstr;

/* chars used as small naturals (so that `char' is reserved for characters) */
typedef unsigned char lu_byte;

/* type to ensure maximum alignment */
#ifndef LUSER_ALIGNMENT_T
typedef union { double u; void *s; long l; } L_Umaxalign;
#else
typedef LUSER_ALIGNMENT_T L_Umaxalign;
#endif


#ifndef check_exp
#define check_exp(c,e)	(e)
#endif


#ifndef cast
#define cast(t, exp)	((t)(exp))
#endif


/*
** type for virtual-machine instructions
** must be an unsigned with (at least) 4 bytes (see details in lopcodes.h)
*/
typedef unsigned long Instruction;


///////////////////////////////////////////////////////////////////////////////
// lobject.h
///////////////////////////////////////////////////////////////////////////////

/*
** Union of all collectable objects
*/
typedef union GCObject GCObject;


/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
*/
#define CommonHeader	GCObject *next; GCObject *prev; lu_byte tt; lu_byte marked; unsigned short ref;


/*
** Common header in struct form
*/
typedef struct GCheader {
  CommonHeader;
} GCheader;



/*
** Union of all Lua values
*/
typedef union {
  GCObject *gc;
  void *p;
  lua_Number n;
  int b;
} Value;


typedef struct lua_TObject {
  int tt;
  Value value;
} TObject;

/* Macros to test type */
#define ttisnil(o)	(ttype(o) == LUA_TNIL)
#define ttisnumber(o)	(ttype(o) == LUA_TNUMBER)
#define ttisstring(o)	(ttype(o) == LUA_TSTRING)
#define ttiswstring(o)	(ttype(o) == LUA_TWSTRING)
#define ttistable(o)	(ttype(o) == LUA_TTABLE)
#define ttisfunction(o)	(ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o)	(ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o)	(ttype(o) == LUA_TUSERDATA)
#define ttisthread(o)	(ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o)	(ttype(o) == LUA_TLIGHTUSERDATA)

/* Macros to access values */
#define ttype(o)	((o)->tt)
#define gcvalue(o)	check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o)	check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o)	check_exp(ttisnumber(o), (o)->value.n)
#define tsvalue(o)	check_exp(ttisstring(o), &(o)->value.gc->ts)
#define uvalue(o)	check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define clvalue(o)	check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define hvalue(o)	check_exp(ttistable(o), &(o)->value.gc->h)
#define bvalue(o)	check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o)	check_exp(ttisthread(o), &(o)->value.gc->th)
#define twsvalue(o)	check_exp(ttiswstring(o), &(o)->value.gc->ts)

#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

/* Macros to set values */
#define setnvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TNUMBER; i_o->value.n=(x); }

#define chgnvalue(obj,x) \
	check_exp(ttype(obj)==LUA_TNUMBER, (obj)->value.n=(x))

#define setpvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TLIGHTUSERDATA; i_o->value.p=(x); }

#define setbvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TBOOLEAN; i_o->value.b=(x); }

#define setsvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TSTRING; \
    i_o->value.gc=cast(GCObject *, (x)); \
    lua_assert(i_o->value.gc->gch.tt == LUA_TSTRING); }

#define setuvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TUSERDATA; \
    i_o->value.gc=cast(GCObject *, (x)); \
    lua_assert(i_o->value.gc->gch.tt == LUA_TUSERDATA); }

#define setthvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TTHREAD; \
    i_o->value.gc=cast(GCObject *, (x)); \
    lua_assert(i_o->value.gc->gch.tt == LUA_TTHREAD); }

#define setclvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TFUNCTION; \
    i_o->value.gc=cast(GCObject *, (x)); \
    lua_assert(i_o->value.gc->gch.tt == LUA_TFUNCTION); }

#define sethvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TTABLE; \
    i_o->value.gc=cast(GCObject *, (x)); \
    lua_assert(i_o->value.gc->gch.tt == LUA_TTABLE); }

#define setnilvalue(obj) ((obj)->tt=LUA_TNIL)

#define setwsvalue(obj,x) \
  { TObject *i_o=(obj); i_o->tt=LUA_TWSTRING; \
    i_o->value.gc=cast(GCObject *, (x)); \
    lua_assert(i_o->value.gc->gch.tt == LUA_TWSTRING); }


/*
** for internal debug only
*/
#define checkconsistency(obj)


#define setobj(obj1,obj2) \
  { const TObject *o2=(obj2); TObject *o1=(obj1); \
    checkconsistency(o2); \
    o1->tt=o2->tt; o1->value = o2->value; }


/*
** different types of sets, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s	setobj
/* to stack (not from same stack) */
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define setwsvalue2s	setwsvalue
/* from table to same table */
#define setobjt2t	setobj
/* to table */
#define setobj2t	setobj
/* to new object */
#define setobj2n	setobj
#define setsvalue2n	setsvalue
#define setwsvalue2n	setwsvalue

#define setttype(obj, tt) (ttype(obj) = (tt))

#define iscollectable(o)	(ttype(o) >= LUA_TSTRING)

typedef TObject *StkId;  /* index to stack elements */


/*
** String headers for string table
*/
typedef union TString {
  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  struct {
    CommonHeader;
    lu_byte reserved;
    lu_hash hash;
    size_t len;
  } tsv;
} TString;


#define getstr(ts)	cast(const char *, (ts) + 1)
#define svalue(o)       getstr(tsvalue(o))
#define getwstr(ts)	cast(const wchar_t *, (ts) + 1)
#define wsvalue(o)       getwstr(tsvalue(o))



typedef union Udata {
  L_Umaxalign dummy;  /* ensures maximum alignment for `local' udata */
  struct {
    CommonHeader;
    struct Table *metatable;
    size_t len;
  } uv;
} Udata;



/*
** Function Prototypes
*/
typedef struct Proto {
  CommonHeader;
  TObject *k;  /* constants used by the function */
  Instruction *code;
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines */
  struct LocVar *locvars;  /* information about local variables */
  TString  *source;
  int sizek;  /* size of `k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of `p' */
  int sizelocvars;
  int lineDefined;
  GCObject *gclist;
  lu_byte nupvalues;
  lu_byte numparams;
  lu_byte is_vararg;
  lu_byte maxstacksize;
} Proto;


typedef struct LocVar {
  TString *varname;
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
} LocVar;



/*
** Closures
*/

#define ClosureHeader \
	CommonHeader; lu_byte isC; lu_byte nupvalues; GCObject *gclist

typedef struct CClosure {
  ClosureHeader;
  int functiontype;
  lua_CFunction f;
/*  TObject upvalue[1];  Due to resizable closures, this can't be used anymore */
} CClosure;


// Differs from Lua's!
#define getcclosureupvalue(G, cl, n) \
	((TObject*)((unsigned char*)&cl+((G).sizeCClosure)+((n)*sizeof(TObject))))


typedef struct LClosure {
  ClosureHeader;
  struct Proto *p;
  TObject g;  /* global table for this closure */
#if 0
  UpVal *upvals[1];
#endif 0
} LClosure;


typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;



/*
** Tables
*/

typedef struct Node {
  TObject i_key;
  TObject i_val;
  struct Node *next;  /* for chaining */
} Node;


typedef struct Table {
  CommonHeader;
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */ 
  lu_byte lsizenode;  /* log2 of size of `node' array */
  struct Table *metatable;
  TObject *array;  /* array part */
  Node *node;
  Node *firstfree;  /* this position is free; all positions after it are full */
  GCObject *gclist;
  int sizearray;  /* size of `array' array */
} Table;


/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
	check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1))))


#define twoto(x)	(1<<(x))
#define sizenode(t)	(twoto((t)->lsizenode))
#define sizearray(t)	((t)->sizearray)


//////////////////////////////////////////////////////////////////////////
// lstring.h
//////////////////////////////////////////////////////////////////////////
#define sizestring(l)	(cast(lu_mem, sizeof(union TString))+ \
                         (cast(lu_mem, l)+1)*sizeof(char))
#define sizewstring(l)	(cast(lu_mem, sizeof(union TString))+ \
                         (cast(lu_mem, l)+1)*sizeof(wchar_t))


//////////////////////////////////////////////////////////////////////////
// lzio.h
//////////////////////////////////////////////////////////////////////////
typedef struct Mbuffer {
  char *buffer;
  size_t buffsize;
} Mbuffer;


//////////////////////////////////////////////////////////////////////////
// ltm.h
//////////////////////////////////////////////////////////////////////////
/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM"
*/
typedef enum {
  TM_INDEX,
  TM_NEWINDEX,
  TM_GC,
  TM_MODE,
  TM_EQ,  /* last tag method with `fast' access */
  TM_ADD,
  TM_SUB,
  TM_MUL,
  TM_DIV,
  TM_POW,
  TM_UNM,
  TM_LT,
  TM_LE,
  TM_CONCAT,
  TM_CALL,
  TM_N		/* number of elements in the enum */
} TMS;


//////////////////////////////////////////////////////////////////////////
// lstate.h
//////////////////////////////////////////////////////////////////////////
/* table of globals */
#define gt(L)	(&L->_gt)

/* registry */
#define registry(L)	(&G(L)->_registry)

typedef struct stringtable {
  GCObject **hash;
  ls_nstr nuse;  /* number of elements */
  int size;
} stringtable;


/*
** informations about a call
*/
typedef struct CallInfo {
  StkId base;  /* base for called function */
  StkId	top;  /* top for this function */
  int state;  /* bit fields; see below */
  union {
    struct {  /* for Lua functions */
      const Instruction *savedpc;
      const Instruction **pc;  /* points to `pc' variable in `luaV_execute' */
    } l;
    struct {  /* for C functions */
      int dummy;  /* just to avoid an empty struct */
    } c;
  } u;
} CallInfo;


/*
** bit fields for `CallInfo.state'
*/
#define CI_C		(1<<0)  /* 1 if function is a C function */
/* 1 if (Lua) function has an active `luaV_execute' running it */
#define CI_HASFRAME	(1<<1)
/* 1 if Lua function is calling another Lua function (and therefore its
   `pc' is being used by the other, and therefore CI_SAVEDPC is 1 too) */
#define CI_CALLING	(1<<2)
#define CI_SAVEDPC	(1<<3)  /* 1 if `savedpc' is updated */
#define CI_YIELD	(1<<4)  /* 1 if thread is suspended */


/*
** `global state', shared by all threads of this state
*/
typedef struct global_State {
  stringtable strt;  /* hash table for strings */
  GCObject *rootgc;  /* list of (almost) all collectable objects */
  GCObject *rootudata;   /* (separated) list of all userdata */
  GCObject *tmudata;  /* list of userdata to be GC */
  Mbuffer buff;  /* temporary buffer for string concatentation */
  lu_mem GCthreshold;
  lu_mem nblocks;  /* number of `bytes' currently allocated */
  lua_CFunction panic;  /* to be called in unprotected errors */
  TObject _registry;
  TObject _defaultmeta;
  struct lua_State *mainthread;
  Node dummynode[1];  /* common node array for all empty tables */
  TString *tmname[TM_N];  /* array with tag-method names */
  // Custom data starts here.
  TObject _defaultmetatypes[LUA_NTYPES];
  void (*fatalErrorFunc)();
  void* lockData;
  void (*lockFunc)(void*);
  void (*unlockFunc)(void*);
  void* memData;
  lua_ReallocFunction reallocFunc;
  lua_FreeFunction freeFunc;
  int minimumstrings;
  unsigned int sizeCClosure;
#if 0
  lua_CClosureManager cclosureManager;
  void* globalUserData;
  void (*userGCFunction)(struct GCState*);
#endif
} global_State;


/*
** `per thread' state
*/
struct lua_State {
  CommonHeader;
  StkId top;  /* first free slot in the stack */
  StkId base;  /* base of current function */
  global_State *l_G;
  CallInfo *ci;  /* call info for current function */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  int stacksize;
  CallInfo *end_ci;  /* points after end of ci array*/
  CallInfo *base_ci;  /* array of CallInfo's */
  unsigned short size_ci;  /* size of array `base_ci' */
  unsigned short nCcalls;  /* number of nested C calls */
  lu_byte hookmask;
  lu_byte allowhook;
  lu_byte hookinit;
  int basehookcount;
  int hookcount;
  lua_Hook hook;
  TObject _gt;  /* table of globals */
  GCObject *openupval;  /* list of open upvalues in this stack */
  GCObject *gclist;
  struct lua_longjmp *errorJmp;  /* current error recover point */
#if 0
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
  const char* allocName;
  unsigned int allocFlags;
  unsigned int mainFunctionAllocFlags;
  unsigned int origAllocFlags;
  void* stateUserData;
#endif 0
};


#define G(L)	(L->l_G)


/*
** Union of all collectable objects
*/
union GCObject {
  GCheader gch;
  union TString ts;
  union Udata u;
  union Closure cl;
  struct Table h;
#if 0
  struct Proto p;
  struct UpVal uv;
  struct lua_State th;  /* thread */
#endif 0
};


/* macros to convert a GCObject into a specific value */
#define gcotots(o)	check_exp((o)->gch.tt == LUA_TSTRING, &((o)->ts))
#define gcotou(o)	check_exp((o)->gch.tt == LUA_TUSERDATA, &((o)->u))
#define gcotocl(o)	check_exp((o)->gch.tt == LUA_TFUNCTION, &((o)->cl))
#define gcotoh(o)	check_exp((o)->gch.tt == LUA_TTABLE, &((o)->h))
#define gcotop(o)	check_exp((o)->gch.tt == LUA_TPROTO, &((o)->p))
#define gcotouv(o)	check_exp((o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define ngcotouv(o) \
	check_exp((o) == NULL || (o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define gcototh(o)	check_exp((o)->gch.tt == LUA_TTHREAD, &((o)->th))

/* macro to convert any value into a GCObject */
#define valtogco(v)	(cast(GCObject *, (v)))


struct LuaState
{
	lua_State* m_state;
};


typedef struct LuaStackObject
{
	LuaState* m_state;
	int m_stackIndex;
} LuaStackObject;

//////////////////////////////////////////////////////////////////////////
// ltable.h
//////////////////////////////////////////////////////////////////////////
#define node(t,i)	(&(t)->node[i])
#define key(n)		(&(n)->i_key)
#define val(n)		(&(n)->i_val)

//////////////////////////////////////////////////////////////////////////
// ltable.c
//////////////////////////////////////////////////////////////////////////
/* function to convert a lua_Number to int (with any rounding method) */
#ifndef lua_number2int
#define lua_number2int(i,n)	((i)=(int)(n))
#endif



#define hashnum(t,n)	\
           (node(t, lmod(cast(lu_hash, cast(ls_hash, n)), sizenode(t))))
#define hashstr(t,str)	 (node(t, lmod((str)->tsv.hash, sizenode(t))))

const TObject luaO_nilobject = {LUA_TNIL, {NULL}};


struct LuaObject
{
	LuaObject* m_next;		   // only valid when in free list
	LuaObject* m_prev;		   // only valid when in used list
	TObject m_object;
	LuaState* m_state;
	char filler[4];
};


