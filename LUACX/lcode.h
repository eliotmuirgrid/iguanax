/*
** Code generator for Lua
** See Copyright Notice in lua.h
*/

#ifndef lcode_h
#define lcode_h

#include "llex.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"


/*
** Marks the end of a patch list. It is an invalid value both as an absolute
** address, and as a list link (would link an element to itself).
*/
#define NO_JUMP (-1)


/*
** grep "ORDER OPR" if you change these enums
*/
typedef enum BinOprx {
  OPRX_ADD, OPRX_SUB, OPRX_MUL, OPRX_DIV, OPRX_MOD, OPRX_POW,
  OPRX_CONCAT,
  OPRX_NE, OPRX_EQ,
  OPRX_LT, OPRX_LE, OPRX_GT, OPRX_GE,
  OPRX_AND, OPRX_OR,
  OPRX_NOBINOPR
} BinOprx;


typedef enum UnOprx { OPRX_MINUS, OPRX_NOT, OPRX_LEN, OPRX_NOUNOPR } UnOprx;


#define getcode(fs,e)   ((fs)->f->code[(e)->u.s.info])

#define luaxK_codeAsBx(fs,o,A,sBx)   luaxK_codeABx(fs,o,A,(sBx)+MAXARG_sBx)

#define luaxK_setmultret(fs,e)   luaxK_setreturns(fs, e, LUAX_MULTRET)

LUAXI_FUNC int luaxK_codeABx (xFuncState *fs, OpxCode o, int A, unsigned int Bx);
LUAXI_FUNC int luaxK_codeABC (xFuncState *fs, OpxCode o, int A, int B, int C);
LUAXI_FUNC void luaxK_fixline (xFuncState *fs, int line);
LUAXI_FUNC void luaxK_nil (xFuncState *fs, int from, int n);
LUAXI_FUNC void luaxK_reserveregs (xFuncState *fs, int n);
LUAXI_FUNC void luaxK_checkstack (xFuncState *fs, int n);
LUAXI_FUNC int luaxK_stringK (xFuncState *fs, TStringx *s);
LUAXI_FUNC int luaxK_numberK (xFuncState *fs, lua_Number r);
LUAXI_FUNC void luaxK_dischargevars (xFuncState *fs, expxdesc *e);
LUAXI_FUNC int luaxK_exp2anyreg (xFuncState *fs, expxdesc *e);
LUAXI_FUNC void luaxK_exp2nextreg (xFuncState *fs, expxdesc *e);
LUAXI_FUNC void luaxK_exp2val (xFuncState *fs, expxdesc *e);
LUAXI_FUNC int luaxK_exp2RK (xFuncState *fs, expxdesc *e);
LUAXI_FUNC void luaxK_self (xFuncState *fs, expxdesc *e, expxdesc *key);
LUAXI_FUNC void luaxK_indexed (xFuncState *fs, expxdesc *t, expxdesc *k);
LUAXI_FUNC void luaxK_goiftrue (xFuncState *fs, expxdesc *e);
LUAXI_FUNC void luaxK_storevar (xFuncState *fs, expxdesc *var, expxdesc *e);
LUAXI_FUNC void luaxK_setreturns (xFuncState *fs, expxdesc *e, int nresults);
LUAXI_FUNC void luaxK_setoneret (xFuncState *fs, expxdesc *e);
LUAXI_FUNC int luaxK_jump (xFuncState *fs);
LUAXI_FUNC void luaxK_ret (xFuncState *fs, int first, int nret);
LUAXI_FUNC void luaxK_patchlist (xFuncState *fs, int list, int target);
LUAXI_FUNC void luaxK_patchtohere (xFuncState *fs, int list);
LUAXI_FUNC void luaxK_concat (xFuncState *fs, int *l1, int l2);
LUAXI_FUNC int luaxK_getlabel (xFuncState *fs);
LUAXI_FUNC void luaxK_prefix (xFuncState *fs, UnOprx op, expxdesc *v);
LUAXI_FUNC void luaxK_infix (xFuncState *fs, BinOprx op, expxdesc *v);
LUAXI_FUNC void luaxK_posfix (xFuncState *fs, BinOprx op, expxdesc *v1, expxdesc *v2);
LUAXI_FUNC void luaxK_setlist (xFuncState *fs, int base, int nelems, int tostore);


#endif
