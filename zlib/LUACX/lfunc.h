/*
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

#ifndef lfunc_h
#define lfunc_h


#include "lobject.h"


#define sizeCclosure(n)   (cast(int, sizeof(CxClosure)) + \
                         cast(int, sizeof(TXValue)*((n)-1)))

#define sizeLclosure(n)   (cast(int, sizeof(LxClosure)) + \
                         cast(int, sizeof(TXValue *)*((n)-1)))


LUAXI_FUNC xProto *luaxF_newproto (luax_State *L);
LUAXI_FUNC xClosure *luaxF_newCclosure (luax_State *L, int nelems, xTable *e);
LUAXI_FUNC xClosure *luaxF_newLclosure (luax_State *L, int nelems, xTable *e);
LUAXI_FUNC xUpVal *luaxF_newupval (luax_State *L);
LUAXI_FUNC xUpVal *luaxF_findupval (luax_State *L, StkIdx level);
LUAXI_FUNC void luaxF_close (luax_State *L, StkIdx level);
LUAXI_FUNC void luaxF_freeproto (luax_State *L, xProto *f);
LUAXI_FUNC void luaxF_freeclosure (luax_State *L, xClosure *c);
LUAXI_FUNC void luaxF_freeupval (luax_State *L, xUpVal *uv);
LUAXI_FUNC const char *luaxF_getlocalname (const xProto *func, int local_number,
                                         int pc);


#endif
