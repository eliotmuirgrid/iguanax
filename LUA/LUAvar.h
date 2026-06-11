#ifndef __LUA_VAR_H__
#define __LUA_VAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAvar
//
// Description:
//
// Translate COLvar to/from Lua.
// See COL/COLvar.h for the method declarations.
//
// Author: Eliot Muir
// Date:   Thu  6 Mar 2014 12:50:06 EST
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

template <class TValue> class COLlist;
class COLvar;
class COLstring;
template <class TValue> class COLarray;

void LUApushVar(lua_State* L, const COLvar& Var);

struct LUA{
   enum LUAconversionResult {
      CONVERSION_SUCCESSFUL = 0,
      ERROR_TABLE_MIXED_KEYS = 1,
      ERROR_ARRAY_INVALID_INDEX,
      ERROR_TABLE_INVALID_KEY,
      ERROR_UD_TO_STRING,
      ERROR_LUA_CYCLIC_TABLE,
      ERROR_LUA_VALUE_UNKNOWN
   };
};


// COLvar::fromLua will simply throw an error if it cannot parse a valid COLvar from L.
// LUAtoVar will return why it failed in the return value. pTraversalStack provides where it failed.
LUA::LUAconversionResult LUAtoVar(
      lua_State*  L,
      int         StackPos,
      COLvar*     pVar,
      COLlist<COLstring>* pTraversalStack = NULL,
      COLarray<const void*>* pSeenTables = NULL
);

#endif // end of defensive include

