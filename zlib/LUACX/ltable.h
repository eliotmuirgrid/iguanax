/*
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define gnode(t,i)   (&(t)->node[i])
#define gkey(n)      (&(n)->i_key.nk)
#define gval(n)      (&(n)->i_val)
#define gnext(n)   ((n)->i_key.nk.next)

#define key2tval(n)   (&(n)->i_key.tvk)


LUAXI_FUNC const TXValue *luaxH_getnum (xTable *t, int key);
LUAXI_FUNC TXValue *luaxH_setnum (luax_State *L, xTable *t, int key);
LUAXI_FUNC const TXValue *luaxH_getstr (xTable *t, TStringx *key);
LUAXI_FUNC TXValue *luaxH_setstr (luax_State *L, xTable *t, TStringx *key);
LUAXI_FUNC const TXValue *luaxH_get (xTable *t, const TXValue *key);
LUAXI_FUNC TXValue *luaxH_set (luax_State *L, xTable *t, const TXValue *key);
LUAXI_FUNC xTable *luaxH_new (luax_State *L, int narray, int lnhash);
LUAXI_FUNC void luaxH_resizearray (luax_State *L, xTable *t, int nasize);
LUAXI_FUNC void luaxH_free (luax_State *L, xTable *t);
LUAXI_FUNC int luaxH_next (luax_State *L, xTable *t, StkIdx key);
LUAXI_FUNC int luaxH_getn (xTable *t);


#if defined(LUAX_DEBUG)
LUAXI_FUNC Node *luaxH_mainposition (const xTable *t, const TXValue *key);
LUAXI_FUNC int luaxH_isdummy (Node *n);
#endif


#endif
