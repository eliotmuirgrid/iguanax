/*
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lfunc_c
#define LUAX_CORE

#include "lua.h"

#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

xClosure *luaxF_newCclosure (luax_State *L, int nelems, xTable *e) {
  xClosure *c = cast(xClosure *, luaxM_malloc(L, sizeCclosure(nelems)));
  luaxC_link(L, obj2gco(c), LUAX_TFUNCTION);
  c->c.isC = 1;
  c->c.env = e;
  c->c.nupvalues = cast_byte(nelems);
  return c;
}


xClosure *luaxF_newLclosure (luax_State *L, int nelems, xTable *e) {
  xClosure *c = cast(xClosure *, luaxM_malloc(L, sizeLclosure(nelems)));
  luaxC_link(L, obj2gco(c), LUAX_TFUNCTION);
  c->l.isC = 0;
  c->l.env = e;
  c->l.nupvalues = cast_byte(nelems);
  while (nelems--) c->l.upvals[nelems] = NULL;
  return c;
}


xUpVal *luaxF_newupval (luax_State *L) {
  xUpVal *uv = luaxM_new(L, xUpVal);
  luaxC_link(L, obj2gco(uv), LUAX_TUPVAL);
  uv->v = &uv->u.value;
  setnilvalue(uv->v);
  return uv;
}


xUpVal *luaxF_findupval (luax_State *L, StkIdx level) {
  global_xState *g = G(L);
  GCObjectx **pp = &L->openupval;
  xUpVal *p;
  xUpVal *uv;
  while (*pp != NULL && (p = ngcotouv(*pp))->v >= level) {
    lua_assert(p->v != &p->u.value);
    if (p->v == level) {  /* found a corresponding upvalue? */
      if (isdead(g, obj2gco(p)))  /* is it dead? */
        changewhite(obj2gco(p));  /* ressurect it */
      return p;
    }
    pp = &p->next;
  }
  uv = luaxM_new(L, xUpVal);  /* not found: create a new one */
  uv->tt = LUAX_TUPVAL;
  uv->marked = luaxC_white(g);
  uv->v = level;  /* current value lives in the stack */
  uv->next = *pp;  /* chain it in the proper position */
  *pp = obj2gco(uv);
  uv->u.l.prev = &g->uvhead;  /* double link it in `uvhead' list */
  uv->u.l.next = g->uvhead.u.l.next;
  uv->u.l.next->u.l.prev = uv;
  g->uvhead.u.l.next = uv;
  lua_assert(uv->u.l.next->u.l.prev == uv && uv->u.l.prev->u.l.next == uv);
  return uv;
}


static void unlinkupval (xUpVal *uv) {
  lua_assert(uv->u.l.next->u.l.prev == uv && uv->u.l.prev->u.l.next == uv);
  uv->u.l.next->u.l.prev = uv->u.l.prev;  /* remove from `uvhead' list */
  uv->u.l.prev->u.l.next = uv->u.l.next;
}


void luaxF_freeupval (luax_State *L, xUpVal *uv) {
  if (uv->v != &uv->u.value)  /* is it open? */
    unlinkupval(uv);  /* remove from open list */
  luaxM_free(L, uv);  /* free upvalue */
}


void luaxF_close (luax_State *L, StkIdx level) {
  xUpVal *uv;
  global_xState *g = G(L);
  while (L->openupval != NULL && (uv = ngcotouv(L->openupval))->v >= level) {
    GCObjectx *o = obj2gco(uv);
    lua_assert(!isblack(o) && uv->v != &uv->u.value);
    L->openupval = uv->next;  /* remove from `open' list */
    if (isdead(g, o))
      luaxF_freeupval(L, uv);  /* free upvalue */
    else {
      unlinkupval(uv);
      setobj(L, &uv->u.value, uv->v);
      uv->v = &uv->u.value;  /* now current value lives here */
      luaxC_linkupval(L, uv);  /* link upvalue into `gcroot' list */
    }
  }
}


xProto *luaxF_newproto (luax_State *L) {
  COL_FUNCTION(luaxF_newproto);
  xProto *f = luaxM_new(L, xProto);
  luaxC_link(L, obj2gco(f), LUAX_TPROTO);
  f->k = NULL;
  f->sizek = 0;
  f->p = NULL;
  f->sizep = 0;
  f->code = NULL;
  f->sizecode = 0;
  f->sizelineinfo = 0;
  f->sizeupvalues = 0;
  f->nups = 0;
  f->upvalues = NULL;
  f->numparams = 0;
  f->is_vararg = 0;
  f->maxstacksize = 0;
  f->lineinfo = NULL;
  f->sizelocvars = 0;
  f->locvars = NULL;
  f->linedefined = 0;
  f->lastlinedefined = 0;
  f->source = NULL;
  return f;
}


void luaxF_freeproto (luax_State *L, xProto *f) {
  luaxM_freearray(L, f->code, f->sizecode, Instruction);
  luaxM_freearray(L, f->p, f->sizep, xProto *);
  luaxM_freearray(L, f->k, f->sizek, TXValue);
  luaxM_freearray(L, f->lineinfo, f->sizelineinfo, int);
  luaxM_freearray(L, f->locvars, f->sizelocvars, struct LocVar);
  luaxM_freearray(L, f->upvalues, f->sizeupvalues, TStringx *);
  luaxM_free(L, f);
}


void luaxF_freeclosure (luax_State *L, xClosure *c) {
  int size = (c->c.isC) ? sizeCclosure(c->c.nupvalues) :
                          sizeLclosure(c->l.nupvalues);
  luaxM_freemem(L, c, size);
}


/*
** Look for n-th local variable at line `line' in function `func'.
** Returns NULL if not found.
*/
const char *luaxF_getlocalname (const xProto *f, int local_number, int pc) {
  int i;
  for (i = 0; i<f->sizelocvars && f->locvars[i].startpc <= pc; i++) {
    if (pc < f->locvars[i].endpc) {  /* is variable active? */
      local_number--;
      if (local_number == 0)
        return getstr(f->locvars[i].varname);
    }
  }
  return NULL;  /* not found */
}

