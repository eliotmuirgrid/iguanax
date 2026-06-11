/*
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"

#include "lobject.h"
#include "ltm.h"
#include "lzio.h"



struct lua_longjmp;  /* defined in ldo.c */


/* table of globals */
#define gt(L)   (&L->l_gt)

/* registry */
#define registry(L)   (&G(L)->l_registry)


/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5


#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUAX_MINSTACK)



typedef struct stringtable {
  GCObjectx **hash;
  lu_int32 nuse;  /* number of elements */
  int size;
} stringtable;


/*
** informations about a call
*/
typedef struct xCallInfo {
  StkIdx base;  /* base for this function */
  StkIdx func;  /* function index in the stack */
  StkIdx   top;  /* top for this function */
  const Instruction *savedpc;
  int nresults;  /* expected number of results from this function */
  int tailcalls;  /* number of tail calls lost under this entry */
} xCallInfo;



#define curr_func(L)   (clvalue(L->ci->func))
#define ci_func(ci)   (clvalue((ci)->func))
#define f_isLua(ci)   (!ci_func(ci)->c.isC)
#define isLua(ci)   (ttisfunction((ci)->func) && f_isLua(ci))


/*
** `global state', shared by all threads of this state
*/
typedef struct global_xState {
  stringtable strt;  /* hash table for strings */
  lua_Alloc frealloc;  /* function to reallocate memory */
  void *ud;         /* auxiliary data to `frealloc' */
  lux_byte currentwhite;
  lux_byte gcstate;  /* state of garbage collector */
  int sweepstrgc;  /* position of sweep in `strt' */
  GCObjectx *rootgc;  /* list of all collectable objects */
  GCObjectx **sweepgc;  /* position of sweep in `rootgc' */
  GCObjectx *gray;  /* list of gray objects */
  GCObjectx *grayagain;  /* list of objects to be traversed atomically */
  GCObjectx *weak;  /* list of weak tables (to be cleared) */
  GCObjectx *tmudata;  /* last element of list of userdata to be GC */
  Mxbuffer buff;  /* temporary buffer for string concatentation */
  lu_mem GCthreshold;
  lu_mem totalbytes;  /* number of bytes currently allocated */
  lu_mem estimate;  /* an estimate of number of bytes actually in use */
  lu_mem gcdept;  /* how much GC is `behind schedule' */
  int gcpause;  /* size of pause between successive GCs */
  int gcstepmul;  /* GC `granularity' */
  lua_CFunction panic;  /* to be called in unprotected errors */
  TXValue l_registry;
  struct luax_State *mainthread;
  xUpVal uvhead;  /* head of double-linked list of all open upvalues */
  struct xTable *mt[NUM_TAGS];  /* metatables for basic types */
  TStringx *tmname[TM_N];  /* array with tag-method names */
} global_xState;


/*
** `per thread' state
*/
struct luax_State {
  CommonHeader;
  lux_byte status;
  StkIdx top;  /* first free slot in the stack */
  StkIdx base;  /* base of current function */
  global_xState *l_G;
  xCallInfo *ci;  /* call info for current function */
  const Instruction *savedpc;  /* `savedpc' of current function */
  StkIdx stack_last;  /* last free slot in the stack */
  StkIdx stack;  /* stack base */
  xCallInfo *end_ci;  /* points after end of ci array*/
  xCallInfo *base_ci;  /* array of xCallInfo's */
  int stacksize;
  int size_ci;  /* size of array `base_ci' */
  unsigned short nCcalls;  /* number of nested C calls */
  unsigned short baseCcalls;  /* nested C calls when resuming coroutine */
  lux_byte hookmask;
  lux_byte allowhook;
  int basehookcount;
  int hookcount;
  TXValue l_gt;  /* table of globals */
  TXValue env;  /* temporary place for environments */
  GCObjectx *openupval;  /* list of open upvalues in this stack */
  GCObjectx *gclist;
  struct lua_longjmp *errorJmp;  /* current error recover point */
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
};


#define G(L)   (L->l_G)


/*
** Union of all collectable objects
*/
union GCObjectx {
  GCheaderx gch;
  union TStringx ts;
  union Udata u;
  union xClosure cl;
  struct xTable h;
  struct xProto p;
  struct xUpVal uv;
  struct luax_State th;  /* thread */
};


/* macros to convert a GCObjectx into a specific value */
#define rawgco2ts(o)   check_exp((o)->gch.tt == LUAX_TSTRING, &((o)->ts))
#define gco2ts(o)   (&rawgco2ts(o)->tsv)
#define rawgco2u(o)   check_exp((o)->gch.tt == LUAX_TUSERDATA, &((o)->u))
#define gco2u(o)   (&rawgco2u(o)->uv)
#define gco2cl(o)   check_exp((o)->gch.tt == LUAX_TFUNCTION, &((o)->cl))
#define gco2h(o)   check_exp((o)->gch.tt == LUAX_TTABLE, &((o)->h))
#define gco2p(o)   check_exp((o)->gch.tt == LUAX_TPROTO, &((o)->p))
#define gco2uv(o)   check_exp((o)->gch.tt == LUAX_TUPVAL, &((o)->uv))
#define ngcotouv(o) \
   check_exp((o) == NULL || (o)->gch.tt == LUAX_TUPVAL, &((o)->uv))
#define gco2th(o)   check_exp((o)->gch.tt == LUAX_TTHREAD, &((o)->th))

/* macro to convert any Lua object into a GCObjectx */
#define obj2gco(v)   (cast(GCObjectx *, (v)))



#endif

