/*
** Tag methods
** See Copyright Notice in lua.h
*/


#include <string.h>

#define ltm_c
#define LUAX_CORE

#include "lua.h"

#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


#include "ifware.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;


const char *const luaxT_typenames[] = {
  "nil", "boolean", "userdata", "number",
  "string", "table", "function", "userdata", "thread",
  "proto", "upval"
};


void luaxT_init (luax_State *L) {
  static const char *const luaxT_eventname[] = {  /* ORDER TM */
    "__index", "__newindex",
    "__gc", "__mode", "__eq",
    "__add", "__sub", "__mul", "__div", "__mod",
    "__pow", "__unm", "__len", "__lt", "__le",
    "__concat", "__call"
  };
  int i;
  for (i=0; i<TM_N; i++) {
    G(L)->tmname[i] = luaxS_new(L, luaxT_eventname[i]);
    luaxS_fix(G(L)->tmname[i]);  /* never collect these names */
  }
}


/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
const TXValue *luaxT_gettm (xTable *events, TMSx event, TStringx *ename) {
  const TXValue *tm = luaxH_getstr(events, ename);
  lua_assert(event <= TM_EQ);
  if (ttisnil(tm)) {  /* no tag method? */
    events->flags |= cast_byte(1u<<event);  /* cache this fact */
    return NULL;
  }
  else return tm;
}


const TXValue *luaxT_gettmbyobj (luax_State *L, const TXValue *o, TMSx event) {
  COL_FUNCTION(luaxT_gettmbyobj);
  COL_VAR2(o, event);
  xTable *mt;
  switch (txtype(o)) {
    case LUAX_TTABLE:
      mt = hvalue(o)->metatable;
      break;
    case LUAX_TUSERDATA:
      mt = uvalue(o)->metatable;
      break;
    default:
      mt = G(L)->mt[txtype(o)];
  }
  return (mt ? luaxH_getstr(mt, G(L)->tmname[event]) : luaxO_nilobject);
}

