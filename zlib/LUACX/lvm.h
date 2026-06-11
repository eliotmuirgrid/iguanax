/*
** Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lvm_h
#define lvm_h


#include "ldo.h"
#include "ltm.h"

LUAXI_FUNC const TXValue *luaxV_toxnumber (const TXValue *obj, TXValue *n);
LUAXI_FUNC int luaxV_toxstring (luax_State *L, StkIdx obj);
LUAXI_FUNC void luaxV_concat (luax_State *L, int total, int last);

#endif
