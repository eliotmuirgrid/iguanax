//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXlexCode
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, May 17th, 2011 @ 03:11:12 PM
//---------------------------------------------------------------------------
#include "LUAXlexCode.h"

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "lzio.h"
#include "llex.h"
#include "lfunc.h"
#include "lparser.h"
#include "lcode.h"
#include "ltable.h"
#include "lstate.h"
#include "ldo.h"
#include "lstring.h"

static const char* LUAX_ALPHANUMERIC_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

struct LUAXbufferStream{
   const char *s;
   size_t size;
};

struct LUAXparser{  /* data to `f_parser' */
   ZIOx* z;
   Mxbuffer buff;  /* buffer to be used by the scanner */
   const char *name;
};

static COLstring LUAXlexToString(luax_State* L, int i){
   return COLstring(lua_toxstring(L, i), lua_strlen(L, i));
}

static const char* LUAXreadFromBuffer(luax_State* L, void* ud, size_t* size){
   LUAXbufferStream* ls = (LUAXbufferStream*)ud;

   if (ls->size == 0) return NULL;
   *size = ls->size;
   ls->size = 0;
   return ls->s;
}

static void LUAXlexNext(luax_State* L, void* arg){
   COL_FUNCTION(LUAXlexNext);
   luaxX_next((luaxLexState*)arg);
}

static void LUAXopenFunc(luaxLexState *ls, xFuncState *fs){
   COL_FUNCTION(LUAXopenFunc);
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


// Not very efficient, but should be fine for small strings (which is what we're
// using it for).
void LUAXunescape(COLstring& EscapedString)
{
   EscapedString.replace("\\\\", "\\");
   EscapedString.replace("\\\'", "\'");
   EscapedString.replace("\\\"", "\"");
   EscapedString.replace("\\b", "\b");
   EscapedString.replace("\\f", "\f");
   EscapedString.replace("\\n", "\n");
   EscapedString.replace("\\r", "\r");
   EscapedString.replace("\\t", "\t");
   EscapedString.replace("\\v", "\v");
}

// While it seems like this should be done by the lexer, in practice it's very
// hard (due to the possibility of unfinished strings.
static void LUAXextractIncompleteKey(const COLstring& ChoppedExpression, COLstring& IncompleteString, bool& IncompleteKeyIsStringLiteral){
   COL_FUNCTION(LUAXextractIncompleteKey);
   const char* pStringEnd = NULL;
   size_t StringValueStart = strcspn(ChoppedExpression.c_str(), "\'\"[");

   if (StringValueStart+1 < ChoppedExpression.size()){
      switch (ChoppedExpression[StringValueStart]){
      case '\'': pStringEnd = "\'"; StringValueStart++; break;
      case '\"': pStringEnd = "\""; StringValueStart++; break;
      case '[':
         if (ChoppedExpression[StringValueStart + 1] == '['){
            pStringEnd = "]]";
            StringValueStart += 2;
         }
         break;
      }
   }
   if (!pStringEnd){
      IncompleteKeyIsStringLiteral = false;
      const char* pString = ChoppedExpression.c_str();
      size_t WordStart = strcspn(pString, LUAX_ALPHANUMERIC_CHARACTERS);
      size_t WordSize  = strspn(pString + WordStart, LUAX_ALPHANUMERIC_CHARACTERS);
      IncompleteString = ChoppedExpression.substr(WordStart, WordSize);
   } else {
      IncompleteKeyIsStringLiteral = true;
      bool IsLongString = (strlen(pStringEnd) == 2);
      size_t StringValueEnd = ChoppedExpression.rfind(pStringEnd);
      if (StringValueEnd == npos || StringValueEnd == StringValueStart-1 || (!IsLongString && ChoppedExpression[StringValueEnd-1] == '\\')){
         // There is no ending quote, or it is escaped - that means the string is unfinished.
         IncompleteString = ChoppedExpression.substr(StringValueStart, ChoppedExpression.size() - StringValueStart);
      } else{
         IncompleteString = ChoppedExpression.substr(StringValueStart, StringValueEnd - StringValueStart);
      }
      if (!IsLongString){
         LUAXunescape(IncompleteString);
      }
   }
   COL_VAR2(IncompleteString, IncompleteKeyIsStringLiteral);
}

COLauto<LUAXtoken> LUAXlexCode(luax_State* L, const COLstring& Code, const char* pChunkName, size_t StopPosition, bool Reverse, COLstring& IncompleteKey, bool& IncompleteKeyIsStringLiteral){
   COL_FUNCTION(LUAXlexCode);
   COL_DUMP("Lex", Code.c_str(),StopPosition);
   LUAXbufferStream ls;
   ls.s = Code.c_str();
   ls.size = Code.size();
   // LUAXreadFromBuffer is our reader function
   // &ls is our void* to the data.
   ZIOx z;
   int status=0;
   if (!pChunkName) pChunkName = "?";
   luaxZ_init(L, &z, LUAXreadFromBuffer, &ls);
   luaxLexState MyLexState;
   LUAXparser Parser;
   luaxZ_initbuffer(L, &Parser.buff);
   MyLexState.buff = &Parser.buff;
   luaxX_setinput(L, &MyLexState, &z, luaxS_new(L, pChunkName)); 

   struct xFuncState MyFuncState;
   LUAXopenFunc(&MyLexState, &MyFuncState);
   MyFuncState.f->is_vararg = VARARG_ISVARARG;  /* main func. is always vararg */
   size_t CurrentPosition = npos; // the end of the current token
   size_t CurrentTokenSize = npos; // the size of the current token
   size_t LastDotOrBracketPosition = npos; // the position of the last '.','[','{','(' or ',' character
   bool LexError = false; // true if we've encountered a lex error
   COLauto<LUAXtoken> pHead = NULL;
   LUAXtoken* pCurrent = NULL;
   do{
      if (luaxD_pcall(L, LUAXlexNext, &MyLexState, savestack(L, L->top), L->errfunc)){
         LexError = true;
         COL_ERR(LUAXlexToString(L, -1));
      }
      CurrentPosition = Code.size() - MyLexState.z->n - 1;
      if (!LexError && CurrentPosition <= StopPosition){
         switch(MyLexState.t.token){
         case TKX_NAME: CurrentTokenSize = MyLexState.t.seminfo.ts->tsv.len;break;
         case '.':
         case ':':
         case '[':
         case '(':
         case '{':
         case ',':
            LastDotOrBracketPosition = CurrentPosition;
         default:
            CurrentTokenSize = npos; // not applicable/required
            break;
         }
         if (Reverse){
            pHead = new LUAXtoken(MyLexState.t.token, CurrentPosition, CurrentTokenSize, MyLexState.linenumber, pHead.release());
            COL_DUMP("R Token:", Code.c_str()+ CurrentPosition, CurrentTokenSize);
         } else {
            LUAXtoken* pNew = new LUAXtoken(MyLexState.t.token, CurrentPosition, CurrentTokenSize, MyLexState.linenumber, NULL);
            COL_DUMP("R Token:", Code.c_str()+ CurrentPosition, CurrentTokenSize);
            if (pCurrent){
               pCurrent->pNext = pNew;
            } else {
               pHead = pNew;
            }
            pCurrent = pNew;
         }
      }
      if (CurrentPosition >= StopPosition && LastDotOrBracketPosition != npos) {
         COLstring AfterStop = COLstring(Code.c_str() + LastDotOrBracketPosition, StopPosition - LastDotOrBracketPosition);
         LUAXextractIncompleteKey(AfterStop, IncompleteKey, IncompleteKeyIsStringLiteral);
         break;
      }
   }
   while (MyLexState.t.token != TKX_EOS && !LexError);
   luaxZ_freebuffer(L, &Parser.buff);
   return pHead.release();
}
