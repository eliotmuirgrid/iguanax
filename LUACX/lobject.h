/*
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/


#ifndef lobject_h
#define lobject_h


#include <stdarg.h>


#include "llimits.h"
#include "lua.h"


/* tags for values visible from Lua */
#define LAST_TAG   LUAX_TTHREAD

#define NUM_TAGS   (LAST_TAG+1)


/*
** Extra tags for non-values
*/
#define LUAX_TPROTO   (LAST_TAG+1)
#define LUAX_TUPVAL   (LAST_TAG+2)
#define LUAX_TDEADKEY   (LAST_TAG+3)


/*
** Union of all collectable objects
*/
typedef union GCObjectx GCObjectx;


/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
*/
#define CommonHeader   GCObjectx *next; lux_byte tt; lux_byte marked


/*
** Common header in struct form
*/
typedef struct GCheaderx {
  CommonHeader;
} GCheaderx;




/*
** Union of all Lua values
*/
typedef union {
  GCObjectx *gc;
  void *p;
  lua_Number n;
  int b;
} Value;


/*
** Tagged Values
*/

#define TXValuefields   Value value; int tt

typedef struct lua_TXValue {
  TXValuefields;
} TXValue;


/* Macros to test type */
#define ttisnil(o)   (txtype(o) == LUAX_TNIL)
#define ttisnumber(o)   (txtype(o) == LUAX_TNUMBER)
#define ttisstring(o)   (txtype(o) == LUAX_TSTRING)
#define ttistable(o)   (txtype(o) == LUAX_TTABLE)
#define ttisfunction(o)   (txtype(o) == LUAX_TFUNCTION)
#define ttisboolean(o)   (txtype(o) == LUAX_TBOOLEAN)
#define ttisuserdata(o)   (txtype(o) == LUAX_TUSERDATA)
#define ttisthread(o)   (txtype(o) == LUAX_TTHREAD)
#define ttislightuserdata(o)   (txtype(o) == LUAX_TLIGHTUSERDATA)

/* Macros to access values */
#define txtype(o)   ((o)->tt)
#define gcvalue(o)   check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o)   check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o)   check_exp(ttisnumber(o), (o)->value.n)
#define rawtsvalue(o)   check_exp(ttisstring(o), &(o)->value.gc->ts)
#define tsvalue(o)   (&rawtsvalue(o)->tsv)
#define rawuvalue(o)   check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define uvalue(o)   (&rawuvalue(o)->uv)
#define clvalue(o)   check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define hvalue(o)   check_exp(ttistable(o), &(o)->value.gc->h)
#define bvalue(o)   check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o)   check_exp(ttisthread(o), &(o)->value.gc->th)

#define l_isfalse(o)   (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

/*
** for internal debug only
*/
#define checkconsistency(obj) \
  lua_assert(!iscollectable(obj) || (txtype(obj) == (obj)->value.gc->gch.tt))

#define checkliveness(g,obj) \
  lua_assert(!iscollectable(obj) || \
  ((txtype(obj) == (obj)->value.gc->gch.tt) && !isdead(g, (obj)->value.gc)))


/* Macros to set values */
#define setnilvalue(obj) ((obj)->tt=LUAX_TNIL)

#define setnvalue(obj,x) \
  { TXValue *i_o=(obj); i_o->value.n=(x); i_o->tt=LUAX_TNUMBER; }

#define setpvalue(obj,x) \
  { TXValue *i_o=(obj); i_o->value.p=(x); i_o->tt=LUAX_TLIGHTUSERDATA; }

#define setbvalue(obj,x) \
  { TXValue *i_o=(obj); i_o->value.b=(x); i_o->tt=LUAX_TBOOLEAN; }

#define setsvalue(L,obj,x) \
  { TXValue *i_o=(obj); \
    i_o->value.gc=cast(GCObjectx *, (x)); i_o->tt=LUAX_TSTRING; \
    checkliveness(G(L),i_o); }

#define setuvalue(L,obj,x) \
  { TXValue *i_o=(obj); \
    i_o->value.gc=cast(GCObjectx *, (x)); i_o->tt=LUAX_TUSERDATA; \
    checkliveness(G(L),i_o); }

#define setthvalue(L,obj,x) \
  { TXValue *i_o=(obj); \
    i_o->value.gc=cast(GCObjectx *, (x)); i_o->tt=LUAX_TTHREAD; \
    checkliveness(G(L),i_o); }

#define setclvalue(L,obj,x) \
  { TXValue *i_o=(obj); \
    i_o->value.gc=cast(GCObjectx *, (x)); i_o->tt=LUAX_TFUNCTION; \
    checkliveness(G(L),i_o); }

#define sethvalue(L,obj,x) \
  { TXValue *i_o=(obj); \
    i_o->value.gc=cast(GCObjectx *, (x)); i_o->tt=LUAX_TTABLE; \
    checkliveness(G(L),i_o); }

#define setptvalue(L,obj,x) \
  { TXValue *i_o=(obj); \
    i_o->value.gc=cast(GCObjectx *, (x)); i_o->tt=LUAX_TPROTO; \
    checkliveness(G(L),i_o); }




#define setobj(L,obj1,obj2) \
  { const TXValue *o2=(obj2); TXValue *o1=(obj1); \
    o1->value = o2->value; o1->tt=o2->tt; \
    checkliveness(G(L),o1); }


/*
** different types of sets, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s   setobj
/* to stack (not from same stack) */
#define setobj2s   setobj
#define setsvalue2s   setsvalue
#define sethvalue2s   sethvalue
#define setptvalue2s   setptvalue
/* from table to same table */
#define setobjt2t   setobj
/* to table */
#define setobj2t   setobj
/* to new object */
#define setobj2n   setobj
#define setsvalue2n   setsvalue

#define settxtype(obj, tt) (txtype(obj) = (tt))


#define iscollectable(o)   (txtype(o) >= LUAX_TSTRING)



typedef TXValue *StkIdx;  /* index to stack elements */


/*
** String headers for string table
*/
typedef union TStringx {
  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  struct {
    CommonHeader;
    lux_byte reserved;
    unsigned int hash;
    size_t len;
  } tsv;
} TStringx;


#define getstr(ts)   cast(const char *, (ts) + 1)
#define svalue(o)       getstr(rawtsvalue(o))



typedef union Udata {
  L_Umaxalign dummy;  /* ensures maximum alignment for `local' udata */
  struct {
    CommonHeader;
    struct xTable *metatable;
    struct xTable *env;
    size_t len;
  } uv;
} Udata;




/*
** Function xPrototypes
*/
typedef struct xProto {
  CommonHeader;
  TXValue *k;  /* constants used by the function */
  Instruction *code;
  struct xProto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines */
  struct LocVar *locvars;  /* information about local variables */
  TStringx **upvalues;  /* upvalue names */
  TStringx  *source;
  int sizeupvalues;
  int sizek;  /* size of `k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of `p' */
  int sizelocvars;
  int linedefined;
  int lastlinedefined;
  GCObjectx *gclist;
  lux_byte nups;  /* number of upvalues */
  lux_byte numparams;
  lux_byte is_vararg;
  lux_byte maxstacksize;
} xProto;


/* masks for new-style vararg */
#define VARARG_HASARG      1
#define VARARG_ISVARARG      2
#define VARARG_NEEDSARG      4


typedef struct LocVar {
  TStringx *varname;
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
} LocVar;



/*
** Upvalues
*/

typedef struct xUpVal {
  CommonHeader;
  TXValue *v;  /* points to stack or to its own value */
  union {
    TXValue value;  /* the value (when closed) */
    struct {  /* double linked list (when open) */
      struct xUpVal *prev;
      struct xUpVal *next;
    } l;
  } u;
} xUpVal;


/*
** xClosures
*/

#define xClosureHeader \
   CommonHeader; lux_byte isC; lux_byte nupvalues; GCObjectx *gclist; \
   struct xTable *env

typedef struct CxClosure {
  xClosureHeader;
  lua_CFunction f;
  TXValue upvalue[1];
} CxClosure;


typedef struct LxClosure {
  xClosureHeader;
  struct xProto *p;
  xUpVal *upvals[1];
} LxClosure;


typedef union xClosure {
  CxClosure c;
  LxClosure l;
} xClosure;


#define iscfunction(o)   (txtype(o) == LUAX_TFUNCTION && clvalue(o)->c.isC)
#define isLfunction(o)   (txtype(o) == LUAX_TFUNCTION && !clvalue(o)->c.isC)


/*
** xTables
*/

typedef union TKey {
  struct {
    TXValuefields;
    struct Node *next;  /* for chaining */
  } nk;
  TXValue tvk;
} TKey;


typedef struct Node {
  TXValue i_val;
  TKey i_key;
} Node;


typedef struct xTable {
  CommonHeader;
  lux_byte flags;  /* 1<<p means tagmethod(p) is not present */ 
  lux_byte lsizenode;  /* log2 of size of `node' array */
  struct xTable *metatable;
  TXValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  GCObjectx *gclist;
  int sizearray;  /* size of `array' array */
} xTable;



/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
   (check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))


#define twoto(x)   (1<<(x))
#define sizenode(t)   (twoto((t)->lsizenode))


#define luaxO_nilobject      (&luaxO_nilobject_)

LUAI_DATA const TXValue luaxO_nilobject_;

#define ceillog2(x)   (luaxO_log2((x)-1) + 1)

LUAXI_FUNC int luaxO_log2 (unsigned int x);
LUAXI_FUNC int luaxO_int2fb (unsigned int x);
LUAXI_FUNC int luaxO_fb2int (int x);
LUAXI_FUNC int luaxO_rawequalObj (const TXValue *t1, const TXValue *t2);
LUAXI_FUNC int luaxO_str2d (const char *s, lua_Number *result);
LUAXI_FUNC const char *luaxO_pushvfstring (luax_State *L, const char *fmt,
                                                       va_list argp);
LUAXI_FUNC const char *luaxO_pushfstring (luax_State *L, const char *fmt, ...);
LUAXI_FUNC void luaxO_chunkid (char *out, const char *source, size_t len);


#endif

