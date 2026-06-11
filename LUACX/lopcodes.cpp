/*
** See Copyright Notice in lua.h
*/


#define lopcodes_c
#define LUAX_CORE


#include "lopcodes.h"


/* ORDER OP */

const char *const luaxP_opnames[NUM_OPCODES+1] = {
  "MOVE",
  "LOADK",
  "LOADBOOL",
  "LOADNIL",
  "GETUPVAL",
  "GETGLOBAL",
  "GETTABLE",
  "SETGLOBAL",
  "SETUPVAL",
  "SETTABLE",
  "NEWTABLE",
  "SELF",
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "MOD",
  "POW",
  "UNM",
  "NOT",
  "LEN",
  "CONCAT",
  "JMP",
  "EQ",
  "LT",
  "LE",
  "TEST",
  "TESTSET",
  "CALL",
  "TAILCALL",
  "RETURN",
  "FORLOOP",
  "FORPREP",
  "TFORLOOP",
  "SETLIST",
  "CLOSE",
  "CLOSURE",
  "VARARG",
  NULL
};


#define opmode(t,a,b,c,m) (((t)<<7) | ((a)<<6) | ((b)<<4) | ((c)<<2) | (m))

const lux_byte luaxP_opmodes[NUM_OPCODES] = {
/*       T  A    B       C     mode         opcode   */
  opmode(0, 1, OpArgR, OpArgN, iABC)       /* OPX_MOVE */
 ,opmode(0, 1, OpArgK, OpArgN, iABx)      /* OPX_LOADK */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)      /* OPX_LOADBOOL */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)      /* OPX_LOADNIL */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)      /* OPX_GETUPVAL */
 ,opmode(0, 1, OpArgK, OpArgN, iABx)      /* OPX_GETGLOBAL */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)      /* OPX_GETTABLE */
 ,opmode(0, 0, OpArgK, OpArgN, iABx)      /* OPX_SETGLOBAL */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)      /* OPX_SETUPVAL */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)      /* OPX_SETTABLE */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)      /* OPX_NEWTABLE */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)      /* OPX_SELF */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)      /* OPX_ADD */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)      /* OPX_SUB */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)      /* OPX_MUL */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)      /* OPX_DIV */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)      /* OPX_MOD */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)      /* OPX_POW */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)      /* OPX_UNM */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)      /* OPX_NOT */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)      /* OPX_LEN */
 ,opmode(0, 1, OpArgR, OpArgR, iABC)      /* OPX_CONCAT */
 ,opmode(0, 0, OpArgR, OpArgN, iAsBx)      /* OPX_JMP */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)      /* OPX_EQ */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)      /* OPX_LT */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)      /* OPX_LE */
 ,opmode(1, 1, OpArgR, OpArgU, iABC)      /* OPX_TEST */
 ,opmode(1, 1, OpArgR, OpArgU, iABC)      /* OPX_TESTSET */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)      /* OPX_CALL */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)      /* OPX_TAILCALL */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)      /* OPX_RETURN */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)      /* OPX_FORLOOP */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)      /* OPX_FORPREP */
 ,opmode(1, 0, OpArgN, OpArgU, iABC)      /* OPX_TFORLOOP */
 ,opmode(0, 0, OpArgU, OpArgU, iABC)      /* OPX_SETLIST */
 ,opmode(0, 0, OpArgN, OpArgN, iABC)      /* OPX_CLOSE */
 ,opmode(0, 1, OpArgU, OpArgN, iABx)      /* OPX_CLOSURE */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)      /* OPX_VARARG */
};

