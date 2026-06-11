//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     Wednesday, April 28th, 2010 @ 10:14:19 AM
//
//---------------------------------------------------------------------------


#include "ifware.h"
#include "lstate.h"

#include <COL/COLostream.h>
#include <COL/COLerror.h>
#include <COL/COLsink.h>


COLostream& operator<<(COLostream& Stream, TString* pLuaString)
{
   Stream << getstr(pLuaString);
   return Stream;
}

COLostream& operator<<(COLostream& Stream, LexState* pState)
{
   Stream << "Lex state (source=" << pState->source 
          << ",ch = " << (char)pState->current 
          << ",l = " << pState->linenumber
          << ", ll = " << pState->lastline 
          << " remain=" << pState->z->n
          << " buffer= \"" << (char)pState->current;
   
   Stream.sink()->write(pState->z->p, pState->z->n);
   

   
   Stream << "\") ";
     

//  Token t;  /* current token */
//  Token lookahead;  /* look ahead token */
//  struct FuncState *fs;  /* `FuncState' is private to the parser */
//  struct lua_State *L;
//  ZIO *z;  /* input stream */
//  Mbuffer *buff;  /* buffer for tokens */
//  char decpoint;  /* locale decimal point */


   return Stream;
}


COLostream& operator<<(COLostream& Stream, Zio* pLuaStream)
{
   Stream << "Zio (remain=" << pLuaStream->n << " buff=\"";
   Stream.sink()->write(pLuaStream->p, pLuaStream->n);
   //lua_Reader reader;
   //void* data;         /* additional data */
   //lua_State *L;         /* Lua state (for reader) */
   return Stream;
};


COLstring IFluaToken(int Token)
{
   if (Token >= FIRST_RESERVED && Token <= FIRST_RESERVED + TK_EOS)
   {
      return luaX_tokens[Token - FIRST_RESERVED];
   }
   else return COLstring(1, (char)Token);
}

COLostream& operator<<(COLostream& Stream, const TValue* pValue)
{
   switch((pValue)->tt)
   {
   case LUA_TSTRING:        Stream << '\'' << rawtsvalue(pValue) << '\''; break;
   case LUA_TTABLE :        Stream << "Table";           break;
   case LUA_TNUMBER :       Stream << nvalue(pValue);    break;
   case LUA_TNIL	  :       Stream << "(nil)";           break;
   case LUA_TBOOLEAN	:      Stream << bvalue(pValue);    break;
   case LUA_TLIGHTUSERDATA: Stream << "light UD";        break;
   case LUA_TFUNCTION:      Stream << "(function)";      break;
   case LUA_TUSERDATA:	    Stream << "(userdata)";      break;
   case LUA_TTHREAD:	       Stream << "(thread)";        break;
   default:
      // probably invalid!  Don't crash. See #15141.
      break;
   }
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const TValue& pValue)
{
   Stream << &pValue;
   return Stream;
}

static const char* const IFware_eventname[] = 
{ 
   "__index", "__newindex",
   "__gc", "__mode", "__eq",
   "__add", "__sub", "__mul", "__div", "__mod",
   "__pow", "__unm", "__len", "__lt", "__le",
   "__concat", "__call"
};


COLostream& operator<<(COLostream& Stream, TMS Event)
{
   Stream << IFware_eventname[Event];
   return Stream;
}

