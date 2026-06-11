/*
** Tag methods
** See Copyright Notice in lua.h
*/

#ifndef ltm_h
#define ltm_h


#include "lobject.h"


/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM"
*/
typedef enum {
  TM_INDEX,
  TM_NEWINDEX,
  TM_GC,
  TM_MODE,
  TM_EQ,  /* last tag method with `fast' access */
  TM_ADD,
  TM_SUB,
  TM_MUL,
  TM_DIV,
  TM_MOD,
  TM_POW,
  TM_UNM,
  TM_LEN,
  TM_LT,
  TM_LE,
  TM_CONCAT,
  TM_CALL,
  TM_N      /* number of elements in the enum */
} TMSx;



#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
  ((et)->flags & (1u<<(e))) ? NULL : luaxT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)   gfasttm(G(l), et, e)

LUAI_DATA const char *const luaxT_typenames[];


LUAXI_FUNC const TXValue *luaxT_gettm (xTable *events, TMSx event, TStringx *ename);
LUAXI_FUNC const TXValue *luaxT_gettmbyobj (luax_State *L, const TXValue *o,
                                                       TMSx event);
LUAXI_FUNC void luaxT_init (luax_State *L);

#endif
