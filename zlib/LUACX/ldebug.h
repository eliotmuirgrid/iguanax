/*
** Auxiliary functions from Debug Interface module
** See Copyright Notice in lua.h
*/

#ifndef ldebug_h
#define ldebug_h


#include "lstate.h"


#define pcRel(pc, p)   (cast(int, (pc) - (p)->code) - 1)

#define getline(f,pc)   (((f)->lineinfo) ? (f)->lineinfo[pc] : 0)

#define resethookcount(L)   (L->hookcount = L->basehookcount)


LUAXI_FUNC void luaxG_typeerror (luax_State *L, const TXValue *o,
                                             const char *opname);
LUAXI_FUNC void luaxG_concaterror (luax_State *L, StkIdx p1, StkIdx p2);
LUAXI_FUNC void luaxG_aritherror (luax_State *L, const TXValue *p1,
                                              const TXValue *p2);
LUAXI_FUNC int luaxG_ordererror (luax_State *L, const TXValue *p1,
                                             const TXValue *p2);
LUAXI_FUNC void luaxG_runerror (luax_State *L, const char *fmt, ...);
LUAXI_FUNC void luaxG_errormsg (luax_State *L);
LUAXI_FUNC int luaxG_checkcode (const xProto *pt);
LUAXI_FUNC int luaxG_checkopenop (Instruction i);

#endif
