/*
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/

#ifndef ldo_h
#define ldo_h


#include "lobject.h"
#include "lstate.h"
#include "lzio.h"


#define luaxD_checkstack(L,n)   \
  if ((char *)L->stack_last - (char *)L->top <= (n)*(int)sizeof(TXValue)) \
    luaxD_growstack(L, n); \
  else condhardstacktests(luaxD_reallocstack(L, L->stacksize - EXTRA_STACK - 1));


#define incr_top(L) {luaxD_checkstack(L,1); L->top++;}

#define savestack(L,p)      ((char *)(p) - (char *)L->stack)
#define restorestack(L,n)   ((TXValue *)((char *)L->stack + (n)))

#define saveci(L,p)      ((char *)(p) - (char *)L->base_ci)
#define restoreci(L,n)      ((xCallInfo *)((char *)L->base_ci + (n)))


/* results from luaxD_precall */
#define PCRLUA      0   /* initiated a call to a Lua function */
#define PCRC      1   /* did a call to a C function */
#define PCRYIELD   2   /* C funtion yielded */


/* type of protected functions, to be ran by `runprotected' */
typedef void (*Pfunc) (luax_State *L, void *ud);

LUAXI_FUNC int luaxD_protectedparser (luax_State *L, ZIOx *z, const char *name);
LUAXI_FUNC void luaxD_callhook (luax_State *L, int event, int line);
LUAXI_FUNC int luaxD_precall (luax_State *L, StkIdx func, int nresults);
LUAXI_FUNC void luaxD_call (luax_State *L, StkIdx func, int nResults);
LUAXI_FUNC int luaxD_pcall (luax_State *L, Pfunc func, void *u,
                                        ptrdiff_t oldtop, ptrdiff_t ef);
LUAXI_FUNC int luaxD_poscall (luax_State *L, StkIdx firstResult);
LUAXI_FUNC void luaxD_reallocCI (luax_State *L, int newsize);
LUAXI_FUNC void luaxD_reallocstack (luax_State *L, int newsize);
LUAXI_FUNC void luaxD_growstack (luax_State *L, int n);

LUAXI_FUNC void luaxD_throw (luax_State *L, int errcode);
LUAXI_FUNC int luaxD_rawrunprotected (luax_State *L, Pfunc f, void *ud);

LUAXI_FUNC void luaxD_seterrorobj (luax_State *L, int errcode, StkIdx oldtop);

#endif

