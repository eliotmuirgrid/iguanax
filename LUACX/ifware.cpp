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

COLostream& operator<<(COLostream& Stream, TStringx* pLuaString)
{
   Stream << getstr(pLuaString);
   return Stream;
}

COLostream& operator<<(COLostream& Stream, luaxLexState* pState)
{
   Stream << "Lex state (source=" << pState->source 
          << ",ch = " << (char)pState->current 
          << ",l = " << pState->linenumber
          << ", ll = " << pState->lastline 
          << " remain=" << pState->z->n
          << " buffer= \"" << (char)pState->current;
   
   Stream.sink()->write(pState->z->p, pState->z->n);
   

   
   Stream << "\") ";
   return Stream;
}


COLostream& operator<<(COLostream& Stream, Ziox* pLuaStream)
{
   Stream << "Ziox (remain=" << pLuaStream->n << " buff=\"";
   Stream.sink()->write(pLuaStream->p, pLuaStream->n);
   //luax_Reader reader;
   //void* data;         /* additional data */
   //luax_State *L;         /* Lua state (for reader) */
   return Stream;
};


COLstring IFluaxxToken(int xToken)
{
   if (xToken >= FIRST_xRESERVED && xToken <= FIRST_xRESERVED + TKX_EOS)
   {
      return luaxX_tokens[xToken - FIRST_xRESERVED];
   }
   else return COLstring(1, (char)xToken);
}

COLostream& operator<<(COLostream& Stream, const TXValue* pValue)
{
   switch((pValue)->tt)
   {
   case LUAX_TSTRING:        Stream << '\'' << rawtsvalue(pValue) << '\''; break;
   case LUAX_TTABLE :        Stream << "xTable";           break;
   case LUAX_TNUMBER :       Stream << nvalue(pValue);    break;
   case LUAX_TNIL     :       Stream << "(nil)";           break;
   case LUAX_TBOOLEAN   :      Stream << bvalue(pValue);    break;
   case LUAX_TLIGHTUSERDATA: Stream << "light UD";        break;
   case LUAX_TFUNCTION:      Stream << "(function)";      break;
   case LUAX_TUSERDATA:       Stream << "(userdata)";      break;
   case LUAX_TTHREAD:          Stream << "(thread)";        break;
   default:
      // probably invalid!  Don't crash. See #15141.
      break;
   }
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const TXValue& pValue)
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


COLostream& operator<<(COLostream& Stream, TMSx Event)
{
   Stream << IFware_eventname[Event];
   return Stream;
}

