/*
** Debug Interface
** See Copyright Notice in lua.h
*/


#include <stdarg.h>
#include <stddef.h>
#include <string.h>


#define ldebug_c
#define LUAX_CORE

#include "lua.h"

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
//#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"



//static const char *getfuncname (luax_State *L, xCallInfo *ci, const char **name);


static int currentpc (luax_State *L, xCallInfo *ci) {
  if (!isLua(ci)) return -1;  /* function is not a Lua function? */
  if (ci == L->ci)
    ci->savedpc = L->savedpc;
  return pcRel(ci->savedpc, ci_func(ci)->l.p);
}


static int currentline (luax_State *L, xCallInfo *ci) {
  int pc = currentpc(L, ci);
  if (pc < 0)
    return -1;  /* only active lua functions have current-line information */
  else
    return getline(ci_func(ci)->l.p, pc);
}



LUAX_API int lua_getstack (luax_State *L, int level, lua_Debug *ar) {
  int status;
  xCallInfo *ci;
  lua_lock(L);
  for (ci = L->ci; level > 0 && ci > L->base_ci; ci--) {
    level--;
    if (f_isLua(ci))  /* Lua function? */
      level -= ci->tailcalls;  /* skip lost tail calls */
  }
  if (level == 0 && ci > L->base_ci) {  /* level found? */
    status = 1;
    ar->i_ci = cast_int(ci - L->base_ci);
  }
  else if (level < 0) {  /* level is of a lost tail call? */
    status = 1;
    ar->i_ci = 0;
  }
  else status = 0;  /* no such level */
  lua_unlock(L);
  return status;
}


static xProto *getluaproto (xCallInfo *ci) {
  return (isLua(ci) ? ci_func(ci)->l.p : NULL);
}


static const char *findlocal (luax_State *L, xCallInfo *ci, int n) {
  const char *name;
  xProto *fp = getluaproto(ci);
  if (fp && (name = luaxF_getlocalname(fp, n, currentpc(L, ci))) != NULL)
    return name;  /* is a local variable in a Lua function */
  else {
    StkIdx limit = (ci == L->ci) ? L->top : (ci+1)->func;
    if (limit - ci->base >= n && n > 0)  /* is 'n' inside 'ci' stack? */
      return "(*temporary)";
    else
      return NULL;
  }
}


LUAX_API const char *lua_getlocal (luax_State *L, const lua_Debug *ar, int n) {
  xCallInfo *ci = L->base_ci + ar->i_ci;
  const char *name = findlocal(L, ci, n);
  lua_lock(L);
  if (name)
      luaA_pushobject(L, ci->base + (n - 1));
  lua_unlock(L);
  return name;
}


LUAX_API const char *lua_setlocal (luax_State *L, const lua_Debug *ar, int n) {
  xCallInfo *ci = L->base_ci + ar->i_ci;
  const char *name = findlocal(L, ci, n);
  lua_lock(L);
  if (name)
      setobjs2s(L, ci->base + (n - 1), L->top - 1);
  L->top--;  /* pop value */
  lua_unlock(L);
  return name;
}


static void funcinfo (lua_Debug *ar, xClosure *cl) {
  if (cl->c.isC) {
    ar->source = "=[C]";
    ar->linedefined = -1;
    ar->lastlinedefined = -1;
    ar->what = "C";
  }
  else {
    ar->source = getstr(cl->l.p->source);
    ar->linedefined = cl->l.p->linedefined;
    ar->lastlinedefined = cl->l.p->lastlinedefined;
    ar->what = (ar->linedefined == 0) ? "main" : "Lua";
  }
  luaxO_chunkid(ar->short_src, ar->source, LUAX_IDSIZE);
}


static void info_tailcall (lua_Debug *ar) {
  ar->name = ar->namewhat = "";
  ar->what = "tail";
  ar->lastlinedefined = ar->linedefined = ar->currentline = -1;
  ar->source = "=(tail call)";
  luaxO_chunkid(ar->short_src, ar->source, LUAX_IDSIZE);
  ar->nups = 0;
}


static void collectvalidlines (luax_State *L, xClosure *f) {
  if (f == NULL || f->c.isC) {
    setnilvalue(L->top);
  }
  else {
    xTable *t = luaxH_new(L, 0, 0);
    int *lineinfo = f->l.p->lineinfo;
    int i;
    for (i=0; i<f->l.p->sizelineinfo; i++)
      setbvalue(luaxH_setnum(L, t, lineinfo[i]), 1);
    sethvalue(L, L->top, t); 
  }
  incr_top(L);
}




/*
** {======================================================
** Symbolic Execution and code checker
** =======================================================
*/

#define check(x)      if (!(x)) return 0;

#define checkjump(pt,pc)   check(0 <= pc && pc < pt->sizecode)

#define checkreg(pt,reg)   check((reg) < (pt)->maxstacksize)




#define checkopenop(pt,pc)   luaxG_checkopenop((pt)->code[(pc)+1])

/* }====================================================== */




static const char *getobjname (luax_State *L, xCallInfo *ci, int stackpos,
                               const char **name) {

  return NULL;  /* no useful name found */
}


static const char *getfuncname (luax_State *L, xCallInfo *ci, const char **name) {

    return NULL;  /* no useful name can be found */
}




void luaxG_typeerror (luax_State *L, const TXValue *o, const char *op) {

}


void luaxG_concaterror (luax_State *L, StkIdx p1, StkIdx p2) {
  if (ttisstring(p1) || ttisnumber(p1)) p1 = p2;
  lua_assert(!ttisstring(p1) && !ttisnumber(p1));
  luaxG_typeerror(L, p1, "concatenate");
}




static void addinfo (luax_State *L, const char *msg) {
  xCallInfo *ci = L->ci;
  if (isLua(ci)) {  /* is Lua code? */
    char buff[LUAX_IDSIZE];  /* add file:line information */
    int line = currentline(L, ci);
    luaxO_chunkid(buff, getstr(getluaproto(ci)->source), LUAX_IDSIZE);
    luaxO_pushfstring(L, "%s:%d: %s", buff, line, msg);
  }
}


void luaxG_errormsg (luax_State *L) {
  if (L->errfunc != 0) {  /* is there an error handling function? */
    StkIdx errfunc = restorestack(L, L->errfunc);
    if (!ttisfunction(errfunc)) luaxD_throw(L, LUAX_ERRERR);
    setobjs2s(L, L->top, L->top - 1);  /* move argument */
    setobjs2s(L, L->top - 1, errfunc);  /* push function */
    incr_top(L);
    luaxD_call(L, L->top - 2, 1);  /* call it */
  }
  luaxD_throw(L, LUAX_ERRRUN);
}


void luaxG_runerror (luax_State *L, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  addinfo(L, luaxO_pushvfstring(L, fmt, argp));
  va_end(argp);
  luaxG_errormsg(L);
}

