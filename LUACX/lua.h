/*
** Lua - An Extensible Extension Language
** Lua.org, PUC-Rio, Brazil (http://www.lua.org)
** See Copyright Notice at the end of this file
*/

#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


#include "luaconf.h"



/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUAX_MULTRET   (-1)


/*
** pseudo-indices
*/
#define LUAX_REGISTRYINDEX   (-10000)
#define LUAX_ENVIRONINDEX   (-10001)
#define LUAX_GLOBALSINDEX   (-10002)
#define lua_upvalueindex(i)   (LUAX_GLOBALSINDEX-(i))


/* thread status; 0 is OK */
#define LUAX_YIELD   1
#define LUAX_ERRRUN   2
#define LUAX_ERRSYNTAX   3
#define LUAX_ERRMEM   4
#define LUAX_ERRERR   5


typedef struct luax_State luax_State;

typedef int (*lua_CFunction) (luax_State *L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*luax_Reader) (luax_State *L, void *ud, size_t *sz);

typedef int (*lua_Writer) (luax_State *L, const void* p, size_t sz, void* ud);


/*
** prototype for memory-allocation functions
*/
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);


/*
** basic types
*/
#define LUAX_TNONE      (-1)

#define LUAX_TNIL      0
#define LUAX_TBOOLEAN      1
#define LUAX_TLIGHTUSERDATA   2
#define LUAX_TNUMBER      3
#define LUAX_TSTRING      4
#define LUAX_TTABLE      5
#define LUAX_TFUNCTION      6
#define LUAX_TUSERDATA      7
#define LUAX_TTHREAD      8



/* minimum Lua stack available to a C function */
#define LUAX_MINSTACK   20


/*
** generic extra include file
*/
#if defined(LUAX_USER_H)
#include LUAX_USER_H
#endif


/* type of numbers in Lua */
typedef LUAX_NUMBER lua_Number;


/* type for integer functions */
typedef LUAX_INTEGER lua_Integer;



/*
** state manipulation
*/
LUAX_API luax_State *(luax_newstate) (lua_Alloc f, void *ud);
LUAX_API void       (lua_close) (luax_State *L);
LUAX_API luax_State *(lua_newthread) (luax_State *L);

LUAX_API lua_CFunction (lua_atpanic) (luax_State *L, lua_CFunction panicf);


/*
** basic stack manipulation
*/
LUAX_API int   (lua_gettop) (luax_State *L);
LUAX_API void  (lua_settop) (luax_State *L, int idx);
LUAX_API void  (lua_pushvalue) (luax_State *L, int idx);
LUAX_API void  (lua_remove) (luax_State *L, int idx);
LUAX_API void  (lua_insert) (luax_State *L, int idx);
LUAX_API void  (lua_replace) (luax_State *L, int idx);
LUAX_API int   (lua_checkstack) (luax_State *L, int sz);

LUAX_API void  (lua_xmove) (luax_State *from, luax_State *to, int n);


/*
** access functions (stack -> C)
*/

LUAX_API int             (lua_isnumber) (luax_State *L, int idx);
LUAX_API int             (lua_isstring) (luax_State *L, int idx);
LUAX_API int             (lua_iscfunction) (luax_State *L, int idx);
LUAX_API int             (lua_isuserdata) (luax_State *L, int idx);
LUAX_API int             (lua_type) (luax_State *L, int idx);
LUAX_API const char     *(lua_typename) (luax_State *L, int tp);

LUAX_API int            (lua_equal) (luax_State *L, int idx1, int idx2);
LUAX_API int            (lua_rawequal) (luax_State *L, int idx1, int idx2);
LUAX_API int            (lua_lessthan) (luax_State *L, int idx1, int idx2);

LUAX_API lua_Number      (lua_toxnumber) (luax_State *L, int idx);
LUAX_API lua_Integer     (lua_tointeger) (luax_State *L, int idx);
LUAX_API int             (lua_toboolean) (luax_State *L, int idx);
LUAX_API const char     *(lua_tolstring) (luax_State *L, int idx, size_t *len);
LUAX_API size_t          (lua_objlen) (luax_State *L, int idx);
LUAX_API lua_CFunction   (lua_tocfunction) (luax_State *L, int idx);
LUAX_API void          *(lua_touserdata) (luax_State *L, int idx);
LUAX_API luax_State      *(lua_tothread) (luax_State *L, int idx);
LUAX_API const void     *(lua_topointer) (luax_State *L, int idx);


/*
** push functions (C -> stack)
*/
LUAX_API void  (lua_pushnil) (luax_State *L);
LUAX_API void  (lua_pushnumber) (luax_State *L, lua_Number n);
LUAX_API void  (lua_pushinteger) (luax_State *L, lua_Integer n);
LUAX_API void  (lua_pushlstring) (luax_State *L, const char *s, size_t l);
LUAX_API void  (lua_pushstring) (luax_State *L, const char *s);
LUAX_API const char *(lua_pushvfstring) (luax_State *L, const char *fmt,
                                                      va_list argp);
LUAX_API const char *(lua_pushfstring) (luax_State *L, const char *fmt, ...);
LUAX_API void  (lua_pushcclosure) (luax_State *L, lua_CFunction fn, int n);
LUAX_API void  (lua_pushboolean) (luax_State *L, int b);
LUAX_API void  (lua_pushlightuserdata) (luax_State *L, void *p);
LUAX_API int   (lua_pushthread) (luax_State *L);


/*
** get functions (Lua -> stack)
*/
LUAX_API void  (lua_gettable) (luax_State *L, int idx);
LUAX_API void  (lua_getfield) (luax_State *L, int idx, const char *k);
LUAX_API void  (lua_rawget) (luax_State *L, int idx);
LUAX_API void  (lua_rawgeti) (luax_State *L, int idx, int n);
LUAX_API void  (lua_createtable) (luax_State *L, int narr, int nrec);
LUAX_API void *(lua_newuserdata) (luax_State *L, size_t sz);
LUAX_API int   (lua_getmetatable) (luax_State *L, int objindex);
LUAX_API void  (lua_getfenv) (luax_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
LUAX_API void  (lua_settable) (luax_State *L, int idx);
LUAX_API void  (lua_setfield) (luax_State *L, int idx, const char *k);
LUAX_API void  (lua_rawset) (luax_State *L, int idx);
LUAX_API void  (lua_rawseti) (luax_State *L, int idx, int n);
LUAX_API int   (lua_setmetatable) (luax_State *L, int objindex);
LUAX_API int   (lua_setfenv) (luax_State *L, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
LUAX_API void  (lua_call) (luax_State *L, int nargs, int nresults);
LUAX_API int   (lua_pcall) (luax_State *L, int nargs, int nresults, int errfunc);
LUAX_API int   (lua_cpcall) (luax_State *L, lua_CFunction func, void *ud);
LUAX_API int   (lua_load) (luax_State *L, luax_Reader reader, void *dt,
                                        const char *chunkname);

LUAX_API int (lua_dump) (luax_State *L, lua_Writer writer, void *data);


/*
** coroutine functions
*/
LUAX_API int  (lua_yield) (luax_State *L, int nresults);
LUAX_API int  (lua_resume) (luax_State *L, int narg);
LUAX_API int  (lua_status) (luax_State *L);

/*
** garbage-collection function and options
*/

#define LUAX_GCSTOP      0
#define LUAX_GCRESTART      1
#define LUAX_GCCOLLECT      2
#define LUAX_GCCOUNT      3
#define LUAX_GCCOUNTB      4
#define LUAX_GCSTEP      5
#define LUAX_GCSETPAUSE      6
#define LUAX_GCSETSTEPMUL   7

LUAX_API int (lua_gc) (luax_State *L, int what, int data);


/*
** miscellaneous functions
*/

LUAX_API int   (lua_error) (luax_State *L);

LUAX_API int   (lua_next) (luax_State *L, int idx);

LUAX_API void  (lua_concat) (luax_State *L, int n);

LUAX_API lua_Alloc (lua_getallocf) (luax_State *L, void **ud);
LUAX_API void lua_setallocf (luax_State *L, lua_Alloc f, void *ud);



/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)      lua_settop(L, -(n)-1)

#define lua_newtable(L)      lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)   lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)      lua_objlen(L, (i))

#define lua_isfunction(L,n)   (lua_type(L, (n)) == LUAX_TFUNCTION)
#define lua_istable(L,n)   (lua_type(L, (n)) == LUAX_TTABLE)
#define lua_islightuserdata(L,n)   (lua_type(L, (n)) == LUAX_TLIGHTUSERDATA)
#define lua_isnil(L,n)      (lua_type(L, (n)) == LUAX_TNIL)
#define lua_isboolean(L,n)   (lua_type(L, (n)) == LUAX_TBOOLEAN)
#define lua_isthread(L,n)   (lua_type(L, (n)) == LUAX_TTHREAD)
#define lua_isnone(L,n)      (lua_type(L, (n)) == LUAX_TNONE)
#define lua_isnoneornil(L, n)   (lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)   \
   lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_setglobal(L,s)   lua_setfield(L, LUAX_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)   lua_getfield(L, LUAX_GLOBALSINDEX, (s))

#define lua_toxstring(L,i)   lua_tolstring(L, (i), NULL)



/*
** compatibility macros and functions
*/

#define lua_open()   luaxL_newstate()

#define lua_getregistry(L)   lua_pushvalue(L, LUAX_REGISTRYINDEX)

#define lua_getgccount(L)   lua_gc(L, LUAX_GCCOUNT, 0)

#define lua_Chunkreader      luax_Reader
#define lua_Chunkwriter      lua_Writer


/* hack */
LUAX_API void lua_setlevel   (luax_State *from, luax_State *to);


/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define LUAX_HOOKCALL   0
#define LUAX_HOOKRET   1
#define LUAX_HOOKLINE   2
#define LUAX_HOOKCOUNT   3
#define LUAX_HOOKTAILRET 4


/*
** Event masks
*/
#define LUAX_MASKCALL   (1 << LUAX_HOOKCALL)
#define LUAX_MASKRET   (1 << LUAX_HOOKRET)
#define LUAX_MASKLINE   (1 << LUAX_HOOKLINE)
#define LUAX_MASKCOUNT   (1 << LUAX_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */


/* Functions to be called by the debuger in specific events */
typedef void (*lua_Hook) (luax_State *L, lua_Debug *ar);


struct lua_Debug {
  int event;
  const char *name;   /* (n) */
  const char *namewhat;   /* (n) `global', `local', `field', `method' */
  const char *what;   /* (S) `Lua', `C', `main', `tail' */
  const char *source;   /* (S) */
  int currentline;   /* (l) */
  int nups;      /* (u) number of upvalues */
  int linedefined;   /* (S) */
  int lastlinedefined;   /* (S) */
  char short_src[LUAX_IDSIZE]; /* (S) */
  /* private part */
  int i_ci;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2008 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


#endif
