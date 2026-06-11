//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteLuaStringKey
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:58:44 AM
//---------------------------------------------------------------------------
#include "TRNisValidLuaName.h"

#include <LUA/LUAescape.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h> // for isdigit

void TRNwriteLuaStringKey(const COLstring& Key, COLostream& Stream, bool UseColon){
   COL_FUNCTION(TRNwriteLuaStringKey);
   if (!Key.is_null() && TRNisValidLuaName(Key) && !isdigit(Key[0])){
      COL_TRC((UseColon ? ':' : '.') << Key);
      Stream << (UseColon ? ':' : '.') << Key;
   } else {
      // Key name contains non-alphanumeric characters, or starts with a digit.
      Stream << COL_T("[\"") << LUAstringEscape(Key.c_str(), Key.size()) << COL_T("\"]");
   }
}
