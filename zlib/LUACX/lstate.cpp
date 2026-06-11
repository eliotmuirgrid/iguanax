/*
** Global State
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lstate_c
#define LUAX_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


#define state_size(x)   (sizeof(x) + LUAI_EXTRASPACE)
#define fromstate(l)   (cast(lux_byte *, (l)) - LUAI_EXTRASPACE)
#define tostate(l)   (cast(luax_State *, cast(lux_byte *, l) + LUAI_EXTRASPACE))


/*
** Main thread combines a thread state and the global state
*/
typedef struct LG {
  luax_State l;
  global_xState g;
} LG;
  


static void stack_init (luax_State *L1, luax_State *L) {
  /* initialize xCallInfo array */
  L1->base_ci = luaxM_newvector(L, BASIC_CI_SIZE, xCallInfo);
  L1->ci = L1->base_ci;
  L1->size_ci = BASIC_CI_SIZE;
  L1->end_ci = L1->base_ci + L1->size_ci - 1;
  /* initialize stack array */
  L1->stack = luaxM_newvector(L, BASIC_STACK_SIZE + EXTRA_STACK, TXValue);
  L1->stacksize = BASIC_STACK_SIZE + EXTRA_STACK;
  L1->top = L1->stack;
  L1->stack_last = L1->stack+(L1->stacksize - EXTRA_STACK)-1;
  /* initialize first ci */
  L1->ci->func = L1->top;
  setnilvalue(L1->top++);  /* `function' entry for this `ci' */
  L1->base = L1->ci->base = L1->top;
  L1->ci->top = L1->top + LUAX_MINSTACK;
}


static void freestack (luax_State *L, luax_State *L1) {
  luaxM_freearray(L, L1->base_ci, L1->size_ci, xCallInfo);
  luaxM_freearray(L, L1->stack, L1->stacksize, TXValue);
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (luax_State *L, void *ud) {
  global_xState *g = G(L);
  UNUSED(ud);
  stack_init(L, L);  /* init stack */
  sethvalue(L, gt(L), luaxH_new(L, 0, 2));  /* table of globals */
  sethvalue(L, registry(L), luaxH_new(L, 0, 2));  /* registry */
  luaxS_resize(L, MINSTRTABSIZE);  /* initial size of string table */
  luaxT_init(L);
  luaxX_init(L);
  luaxS_fix(luaxS_newliteral(L, MEMERRMSG));
  g->GCthreshold = 4*g->totalbytes;
}


static void preinit_state (luax_State *L, global_xState *g) {
  G(L) = g;
  L->stack = NULL;
  L->stacksize = 0;
  L->errorJmp = NULL;
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  resethookcount(L);
  L->openupval = NULL;
  L->size_ci = 0;
  L->nCcalls = L->baseCcalls = 0;
  L->status = 0;
  L->base_ci = L->ci = NULL;
  L->savedpc = NULL;
  L->errfunc = 0;
  setnilvalue(gt(L));
}


static void close_state (luax_State *L) {
  global_xState *g = G(L);
  luaxF_close(L, L->stack);  /* close all upvalues for this thread */
  luaxC_freeall(L);  /* collect all objects */
  lua_assert(g->rootgc == obj2gco(L));
  lua_assert(g->strt.nuse == 0);
  luaxM_freearray(L, G(L)->strt.hash, G(L)->strt.size, TStringx *);
  luaxZ_freebuffer(L, &g->buff);
  freestack(L, L);
  lua_assert(g->totalbytes == sizeof(LG));
  (*g->frealloc)(g->ud, fromstate(L), state_size(LG), 0);
}



LUAX_API luax_State *luax_newstate (lua_Alloc f, void *ud) {
  int i;
  luax_State *L;
  global_xState *g;
  void *l = (*f)(ud, NULL, 0, state_size(LG));
  if (l == NULL) return NULL;
  L = tostate(l);
  g = &((LG *)L)->g;
  L->next = NULL;
  L->tt = LUAX_TTHREAD;
  g->currentwhite = bit2mask(WHITE0BIT, FIXEDBIT);
  L->marked = luaxC_white(g);
  set2bits(L->marked, FIXEDBIT, SFIXEDBIT);
  preinit_state(L, g);
  g->frealloc = f;
  g->ud = ud;
  g->mainthread = L;
  g->uvhead.u.l.prev = &g->uvhead;
  g->uvhead.u.l.next = &g->uvhead;
  g->GCthreshold = 0;  /* mark it as unfinished state */
  g->strt.size = 0;
  g->strt.nuse = 0;
  g->strt.hash = NULL;
  setnilvalue(registry(L));
  luaxZ_initbuffer(L, &g->buff);
  g->panic = NULL;
  g->gcstate = GCSpause;
  g->rootgc = obj2gco(L);
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gray = NULL;
  g->grayagain = NULL;
  g->weak = NULL;
  g->tmudata = NULL;
  g->totalbytes = sizeof(LG);
  g->gcpause = LUAI_GCPAUSE;
  g->gcstepmul = LUAI_GCMUL;
  g->gcdept = 0;
  for (i=0; i<NUM_TAGS; i++) g->mt[i] = NULL;
  if (luaxD_rawrunprotected(L, f_luaopen, NULL) != 0) {
    /* memory allocation error: free partial state */
    close_state(L);
    L = NULL;
  }
  else
    luai_userstateopen(L);
  return L;
}


static void callallgcTM (luax_State *L, void *ud) {
  UNUSED(ud);
  luaxC_callGCTM(L);  /* call GC metamethods for all udata */
}


LUAX_API void lua_close (luax_State *L) {
  L = G(L)->mainthread;  /* only the main thread can be closed */
  lua_lock(L);
  luaxF_close(L, L->stack);  /* close all upvalues for this thread */
  luaxC_separateudata(L, 1);  /* separate udata that have GC metamethods */
  L->errfunc = 0;  /* no error function during GC metamethods */
  do {  /* repeat until no more errors */
    L->ci = L->base_ci;
    L->base = L->top = L->ci->base;
    L->nCcalls = L->baseCcalls = 0;
  } while (luaxD_rawrunprotected(L, callallgcTM, NULL) != 0);
  lua_assert(G(L)->tmudata == NULL);
  luai_userstateclose(L);
  close_state(L);
}

