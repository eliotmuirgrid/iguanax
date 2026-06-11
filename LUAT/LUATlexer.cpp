//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUATlexer
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, June 9th, 2010 @ 02:05:05 PM
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include "LUATlexer.h"

#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>

#include <UNIT/UNITapp.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;
#include <LUACX/lua.h>
#include <LUACX/lauxlib.h>
#include <LUACX/lzio.h>
#include <LUACX/llex.h>
#include <LUACX/lstring.h>
#include <LUACX/lparser.h>
#include <LUACX/lfunc.h>
#include <LUACX/lcode.h>
#include <LUACX/ltable.h>
#include <LUACX/ldo.h>
#include <LUACX/ifware.h>
#include <LUACX/LUAXgrabOpenExpression.h>

#include <exception>    // std::exception so we can wrap a potential crash

COLstring LUATtoString(luax_State* L, int i) {
   return COLstring(lua_toxstring(L, i), lua_strlen(L, i));
}

struct LUAbufferStream 
{
   const char *s;
   size_t size;
};

struct LUAparser {  /* data to `f_parser' */
   ZIOx* z;
   Mxbuffer buff;  /* buffer to be used by the scanner */
   const char *name;
};

const char* LUAreadFromBuffer(luax_State* L, void* ud, size_t* size) {
   LUAbufferStream* ls = (LUAbufferStream*)ud;
 
   if (ls->size == 0) return NULL;
   *size = ls->size;
   ls->size = 0;
   return ls->s;
}

static void LUAopenFunc(luaxLexState *ls, xFuncState *fs){
   COL_FUNCTION(LUAopenFunc);
   luax_State *L = ls->L;
   xProto *f = luaxF_newproto(L);
   fs->f = f;
   fs->prev = ls->fs;  /* linked list of funcstates */
   fs->ls = ls;
   fs->L = L;
   ls->fs = fs;
   fs->pc = 0;
   fs->lasttarget = -1;
   fs->jpc = NO_JUMP;
   fs->freereg = 0;
   fs->nk = 0;
   fs->np = 0;
   fs->nlocvars = 0;
   fs->nactvar = 0;
   fs->bl = NULL;
   f->source = ls->source;
   f->maxstacksize = 2;  /* registers 0/1 are always valid */
   fs->h = luaxH_new(L, 0, 0);
   /* anchor table of constants and prototype (to avoid being collected) */
   sethvalue2s(L, L->top, fs->h);
   incr_top(L);
   setptvalue2s(L, L->top, f);
   incr_top(L);
}

int LUATlexBuffer(luax_State* L, const char* Buffer, size_t Size, const char* pChunkName) {
   COL_FUNCTION(luaL_loadbuffer);
   LUAbufferStream ls;
   ls.s = Buffer;
   ls.size = Size;

   // LUAreadFromBuffer is our reader function
   // &ls is our void* to the data.

   ZIOx z;
   int status=0;
   //lua_lock(L);
   if (!pChunkName) pChunkName = "?";
   luaxZ_init(L, &z, LUAreadFromBuffer, &ls);
   //status = luaD_protectedparser(L, &z, chunkname);
   //lua_unlock(L);

   luaxLexState MyLexState;
   LUAparser Parser;

   luaxZ_initbuffer(L, &Parser.buff);

   MyLexState.buff = &Parser.buff;
   
   luaxX_setinput(L, &MyLexState, &z, luaxS_new(L, pChunkName)); 

   struct xFuncState MyFuncState;
   LUAopenFunc(&MyLexState, &MyFuncState);

   MyFuncState.f->is_vararg = VARARG_ISVARARG;  /* main func. is always vararg */

   COLcout << newline << "[1] ";
   do {
      //COLcout << '(' << MyLexState.current << ')';
      luaxX_next(&MyLexState);
      if (MyLexState.lastline != MyLexState.linenumber)
      {
         COLcout << newline << '[' << MyLexState.linenumber << "] ";
      }
      switch(MyLexState.t.token)
      {
      case TKX_NAME:
         COLcout << MyLexState.t.seminfo.ts << " (n)";         
         break;
      case TKX_STRING:
         COLcout << MyLexState.t.seminfo.ts << " (s)"; 
         break;
      case TKX_EQ:
         COLcout << "==";
         break;
      case ',':
          COLcout << "comma";
          break;
      case '[':
      case ']':
      case '.':
      case '#':
      case '}':
      case '{':
      case '=':
      case '-':
      case ')':
      case '(':
         COLcout << (char)MyLexState.t.token;
         break;
      default:  
         if (MyLexState.t.token > FIRST_xRESERVED)
         {
            COLcout << IFluaxxToken(MyLexState.t.token) << " (k)";   
         }
         else
         {
            COLcout << "Unknown!";
         }
      }
      COLcout << ',';
   } while(MyLexState.t.token != TKX_EOS);
   COLcout << newline;

   luaxZ_resizebuffer(L, MyLexState.buff, 0); // free MyLexState.buff

   return status;
}

void LUATlexCode(const COLstring& LuaCode){
   COL_FUNCTION(LUATlexCode);
   luax_State* L = lua_open(); 
   //LUAopenBaseLibraries(L);
   COLcout << "Parsing: " << newline << LuaCode;
   LUATlexBuffer(L, LuaCode.c_str(), LuaCode.size(), "Lex Hacked Code");

   lua_close(L);
}


TSTtestable* LUATlexer()
{
   TST_START_COLLECTION("LUATlexer")
     
   TST_END_FIXTURE
};

