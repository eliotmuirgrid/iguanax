// Very hacked form of Lua interpreter just to get the lexer out



#include "lua.h"

#include "lapi.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"

#include "ifware.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;



#define api_checknelems(L, n)   api_check(L, (n) <= (L->top - L->base))

#define api_checkvalidindex(L, i)   api_check(L, (i) != luaxO_nilobject)

#define api_incr_top(L)   {api_check(L, L->top < L->ci->top); L->top++;}



static TXValue *index2adr (luax_State *L, int idx) {
  COL_FUNCTION(index2adr);
  if (idx > 0) {
    COL_DBG("Looking for " << idx);
    TXValue *o = L->base + (idx - 1);
    api_check(L, idx <= L->ci->top - L->base);
    if (o >= L->top){
       COL_TRC("Returning nil object");
       return cast(TXValue *, luaxO_nilobject);
    }
    else return o;
  }
  else if (idx > LUAX_REGISTRYINDEX) {
    COL_DBG("idx " <<idx);
    api_check(L, idx != 0 && -idx <= L->top - L->base);
    return L->top + idx;
  }
  else switch (idx) {  /* pseudo-indices */
    case LUAX_REGISTRYINDEX: {
      COL_DBG("Is registry."); 
      return registry(L);
    }
    case LUAX_ENVIRONINDEX: {
      COL_DBG("Is environment index.");
      xClosure *func = curr_func(L);
      sethvalue(L, &L->env, func->c.env);
      return &L->env;
    }
    case LUAX_GLOBALSINDEX:{ 
      COL_DBG("Globals index.");
      return gt(L);
    }
    default: {
      xClosure *func = curr_func(L);
      idx = LUAX_GLOBALSINDEX - idx;
      return (idx <= func->c.nupvalues)
                ? &func->c.upvalue[idx-1]
                : cast(TXValue *, luaxO_nilobject);
    }
  }
}

void luaA_pushobject (luax_State *L, const TXValue *o) {
  COL_FUNCTION(luaA_pushobject);
  setobj2s(L, L->top, o);
  api_incr_top(L);
}



LUAX_API lua_CFunction lua_atpanic (luax_State *L, lua_CFunction panicf) {
  COL_FUNCTION(lua_setlevel);
  lua_CFunction old;
  lua_lock(L);
  old = G(L)->panic;
  G(L)->panic = panicf;
  lua_unlock(L);
  return old;
}



LUAX_API int lua_gettop (luax_State *L) {
  COL_FUNCTION(lua_gettop);
  return cast_int(L->top - L->base);
}


LUAX_API const char *lua_tolstring (luax_State *L, int idx, size_t *len) {
  COL_FUNCTION(lua_tolstring);
  StkIdx o = index2adr(L, idx);
  if (!ttisstring(o)) {
    lua_lock(L);  /* `luaxV_toxstring' may create a new string */
    if (!luaxV_toxstring(L, o)) {  /* conversion failed? */
      if (len != NULL) *len = 0;
      lua_unlock(L);
      return NULL;
    }
    luaxC_checkGC(L);
    o = index2adr(L, idx);  /* previous call may reallocate the stack */
    lua_unlock(L);
  }
  if (len != NULL) *len = tsvalue(o)->len;
  return svalue(o);
}


LUAX_API size_t lua_objlen (luax_State *L, int idx) {
  COL_FUNCTION(lua_objlen);
  StkIdx o = index2adr(L, idx);
  switch (txtype(o)) {
    case LUAX_TSTRING: return tsvalue(o)->len;
    case LUAX_TUSERDATA: return uvalue(o)->len;
    case LUAX_TTABLE: return luaxH_getn(hvalue(o));
    case LUAX_TNUMBER: {
      size_t l;
      lua_lock(L);  /* `luaxV_toxstring' may create a new string */
      l = (luaxV_toxstring(L, o) ? tsvalue(o)->len : 0);
      lua_unlock(L);
      return l;
    }
    default: return 0;
  }
}







/*
** `load' and `call' functions (run Lua code)
*/


#define adjustresults(L,nres) \
    { if (nres == LUAX_MULTRET && L->top >= L->ci->top) L->ci->top = L->top; }


#define checkresults(L,na,nr) \
     api_check(L, (nr) == LUAX_MULTRET || (L->ci->top - L->top >= (nr) - (na)))
  

/*
** Execute a protected call.
*/
struct CallS {  /* data to `f_call' */
  StkIdx func;
  int nresults;
};


static void f_call (luax_State *L, void *ud) {
  COL_FUNCTION(f_call); 
  struct CallS *c = cast(struct CallS *, ud);
  luaxD_call(L, c->func, c->nresults);
}



LUAX_API int lua_pcall (luax_State *L, int nargs, int nresults, int errfunc) {
  COL_FUNCTION(lua_pcall);
  COL_VAR3(nargs, nresults, errfunc);
  struct CallS c;
  int status;
  ptrdiff_t func;
  lua_lock(L);
  api_checknelems(L, nargs+1);
  checkresults(L, nargs, nresults);
  if (errfunc == 0){
    COL_TRC("No error function defined.");
    func = 0;
  }
  else {
    StkIdx o = index2adr(L, errfunc);
    api_checkvalidindex(L, o);
    func = savestack(L, o);
  }
  c.func = L->top - (nargs+1);  /* function to be called */
  c.nresults = nresults;
  status = luaxD_pcall(L, f_call, &c, savestack(L, c.func), func);
  adjustresults(L, nresults);
  lua_unlock(L);
  return status;
}

