/*
** String table (keep all strings handled by Lua)
** See Copyright Notice in lua.h
*/

#ifndef lstring_h
#define lstring_h


#include "lgc.h"
#include "lobject.h"
#include "lstate.h"


#define sizestring(s)   (sizeof(union TStringx)+((s)->len+1)*sizeof(char))

#define sizeudata(u)   (sizeof(union Udata)+(u)->len)

#define luaxS_new(L, s)   (luaxS_newlstr(L, s, strlen(s)))
#define luaxS_newliteral(L, s)   (luaxS_newlstr(L, "" s, \
                                 (sizeof(s)/sizeof(char))-1))

#define luaxS_fix(s)   l_setbit((s)->tsv.marked, FIXEDBIT)

LUAXI_FUNC void luaxS_resize (luax_State *L, int newsize);
LUAXI_FUNC Udata *luaxS_newudata (luax_State *L, size_t s, xTable *e);
LUAXI_FUNC TStringx *luaxS_newlstr (luax_State *L, const char *str, size_t l);


#endif
