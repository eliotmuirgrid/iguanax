/*
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/


#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#define ldo_c
#define LUAX_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
//#include "lundump.h"
#include "lvm.h"
#include "lzio.h"

#include "ifware.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;

/*
** {======================================================
** Error-recovery functions
** =======================================================
*/


/* chain list of long jump buffers */
struct lua_longjmp {
  struct lua_longjmp *previous;
  luai_jmpbuf b;
  volatile int status;  /* error code */
};


void luaxD_seterrorobj (luax_State *L, int errcode, StkIdx oldtop) {
  COL_FUNCTION(luaxD_seterrorobj);
  switch (errcode) {
    case LUAX_ERRMEM: {
      setsvalue2s(L, oldtop, luaxS_newliteral(L, MEMERRMSG));
      break;
    }
    case LUAX_ERRERR: {
      setsvalue2s(L, oldtop, luaxS_newliteral(L, "error in error handling"));
      break;
    }
    case LUAX_ERRSYNTAX:
    case LUAX_ERRRUN: {
      setobjs2s(L, oldtop, L->top - 1);  /* error message on current top */
      break;
    }
  }
  L->top = oldtop + 1;
}


static void restore_stack_limit (luax_State *L) {
  COL_FUNCTION(restore_stack_limit);
  lua_assert(L->stack_last - L->stack == L->stacksize - EXTRA_STACK - 1);
  if (L->size_ci > LUAI_MAXCALLS) {  /* there was an overflow? */
    int inuse = cast_int(L->ci - L->base_ci);
    if (inuse + 1 < LUAI_MAXCALLS)  /* can `undo' overflow? */
      luaxD_reallocCI(L, LUAI_MAXCALLS);
  }
}


static void resetstack (luax_State *L, int status) {
  COL_FUNCTION(resetstack);
  L->ci = L->base_ci;
  L->base = L->ci->base;
  luaxF_close(L, L->base);  /* close eventual pending closures */
  luaxD_seterrorobj(L, status, L->base);
  L->nCcalls = L->baseCcalls;
  L->allowhook = 1;
  restore_stack_limit(L);
  L->errfunc = 0;
  L->errorJmp = NULL;
}


void luaxD_throw (luax_State *L, int errcode) {
  COL_FUNCTION(luaxD_throw);
  if (L->errorJmp) {
    L->errorJmp->status = errcode;
    LUAI_THROW(L, L->errorJmp);
  }
  else {
    L->status = cast_byte(errcode);
    if (G(L)->panic) {
      resetstack(L, errcode);
      lua_unlock(L);
      G(L)->panic(L);
    }
    exit(EXIT_FAILURE);
  }
}


int luaxD_rawrunprotected (luax_State *L, Pfunc f, void *ud) {
  COL_FUNCTION(luaxD_rawrunprotected);
  struct lua_longjmp lj;
  lj.status = 0;
  lj.previous = L->errorJmp;  /* chain new error handler */
  L->errorJmp = &lj;
  LUAI_TRY(L, &lj,
    (*f)(L, ud);
  );
  L->errorJmp = lj.previous;  /* restore old error handler */
  return lj.status;
}

/* }====================================================== */


static void correctstack (luax_State *L, TXValue *oldstack) {
  COL_FUNCTION(correctstack);
  xCallInfo *ci;
  GCObjectx *up;
  L->top = (L->top - oldstack) + L->stack;
  for (up = L->openupval; up != NULL; up = up->gch.next)
    gco2uv(up)->v = (gco2uv(up)->v - oldstack) + L->stack;
  for (ci = L->base_ci; ci <= L->ci; ci++) {
    ci->top = (ci->top - oldstack) + L->stack;
    ci->base = (ci->base - oldstack) + L->stack;
    ci->func = (ci->func - oldstack) + L->stack;
  }
  L->base = (L->base - oldstack) + L->stack;
}


void luaxD_reallocstack (luax_State *L, int newsize) {
  COL_FUNCTION(luaxD_reallocstack);
  TXValue *oldstack = L->stack;
  int realsize = newsize + 1 + EXTRA_STACK;
  lua_assert(L->stack_last - L->stack == L->stacksize - EXTRA_STACK - 1);
  luaxM_reallocvector(L, L->stack, L->stacksize, realsize, TXValue);
  L->stacksize = realsize;
  L->stack_last = L->stack+newsize;
  correctstack(L, oldstack);
}


void luaxD_reallocCI (luax_State *L, int newsize) {
  COL_FUNCTION(luaxD_reallocCI);
  xCallInfo *oldci = L->base_ci;
  luaxM_reallocvector(L, L->base_ci, L->size_ci, newsize, xCallInfo);
  L->size_ci = newsize;
  L->ci = (L->ci - oldci) + L->base_ci;
  L->end_ci = L->base_ci + L->size_ci - 1;
}


void luaxD_growstack (luax_State *L, int n) {
  COL_FUNCTION(luaxD_growstack);
  if (n <= L->stacksize)  /* double size is enough? */
    luaxD_reallocstack(L, 2*L->stacksize);
  else
    luaxD_reallocstack(L, L->stacksize + n);
}


static xCallInfo *growCI (luax_State *L) {
  COL_FUNCTION(growCI);
  if (L->size_ci > LUAI_MAXCALLS)  /* overflow while handling overflow? */
    luaxD_throw(L, LUAX_ERRERR);
  else {
    luaxD_reallocCI(L, 2*L->size_ci);
    if (L->size_ci > LUAI_MAXCALLS)
      luaxG_runerror(L, "stack overflow");
  }
  return ++L->ci;
}





int luaxD_precall (luax_State *L, StkIdx func, int nresults) {
  COL_FUNCTION(luaxD_precall);
  COL_VAR2(func, nresults);

  return PCRC;
}




void luaxD_call (luax_State *L, StkIdx func, int nResults) {
  COL_FUNCTION(luaxD_call);
 
}




int luaxD_pcall (luax_State *L, Pfunc func, void *u,
                ptrdiff_t old_top, ptrdiff_t ef) {
  COL_FUNCTION(luaxD_pcall);
  int status;
  unsigned short oldnCcalls = L->nCcalls;
  ptrdiff_t old_ci = saveci(L, L->ci);
  lux_byte old_allowhooks = L->allowhook;
  ptrdiff_t old_errfunc = L->errfunc;
  L->errfunc = ef;
  status = luaxD_rawrunprotected(L, func, u);
  if (status != 0) {  /* an error occurred? */
    StkIdx oldtop = restorestack(L, old_top);
    luaxF_close(L, oldtop);  /* close eventual pending closures */
    luaxD_seterrorobj(L, status, oldtop);
    L->nCcalls = oldnCcalls;
    L->ci = restoreci(L, old_ci);
    L->base = L->ci->base;
    L->savedpc = L->ci->savedpc;
    L->allowhook = old_allowhooks;
    restore_stack_limit(L);
  }
  L->errfunc = old_errfunc;
  return status;
}




