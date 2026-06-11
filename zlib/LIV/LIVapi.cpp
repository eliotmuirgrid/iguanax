//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIViconv
//
// Description:
//
// Implementation
//
// Author: Akshay Ganeshen
// Date:   Mon 24 Nov 2014 09:49:37 EST
//
//---------------------------------------------------------------------------

#include "LIVapi.h"

#include <LSFI/LSFItextFilters.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAvar.h>

#include <LUAL/LUALutils.h>
#include <LUAL/LUALtextFilters.h> // For TNDL_LUA_CATCH
#include <LUA/LUAerror.h>
#include <LUA/LUAcheck.h>


#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <TXT/TXTencoding.h>
#include <TXT/TXTencodingFilter.h>

int LIViconvListEncodings(lua_State* L) {
   COL_FUNCTION(LIViconvListEncodings);

   LUAcheckArgCount(L, 0, -1);

   const COLvar& EncodingTable = TXTgetAvailableEncodings();

   LUApushVar(L, EncodingTable);
   return 1;
}

int LIViconvListAliases(lua_State* L) {
   COL_FUNCTION(LIViconvListAliases);

   LUAcheckArgCount(L, 1, -1);
   COLstring EncodingRequest;
   if (!LUAextractString(L, 1, &EncodingRequest)) {
      LUA_ERROR_STREAM("Argument must be of type 'string'.");
   }
   COL_VAR(EncodingRequest);

   try {
      const COLvar& Aliases = TXTgetAliasEncodings(EncodingRequest); // can throw
      LUApushVar(L, Aliases);
      return 1;
   } LUA_CATCH();
   return 0;
}

int LIViconvIsSupported(lua_State* L) {
   COL_FUNCTION(LIViconvIsSupported);

   LUAcheckArgCount(L, 1, -1);
   COLstring EncodingRequest;
   if (!LUAextractString(L, 1, &EncodingRequest)) {
      LUA_ERROR_STREAM("Argument must be of type 'string'.");
   }
   COL_VAR(EncodingRequest);

   TXTencoding Encoding = TXTencoding(EncodingRequest);
   lua_pushboolean(L, Encoding.isSupported());
   return 1;
}

int LIViconvConvert(lua_State* L) {
   COL_FUNCTION(LIViconvConvert);
   LUAcheckArgCount(L, 3, -1);

   // No need to do any checks on the data - the wrapper below does that
   COLstring FromEncoding, ToEncoding;
   if (!lua_isstring(L, 2) || !lua_isstring(L, 3)) {
      LUA_ERROR_STREAM("Second and third arguments must be strings.");
   }

   FromEncoding = LUAtoString(L, 2);
   ToEncoding = LUAtoString(L, 3);

   // Apply the TNDLtextFilter wrapper around an encoding filter
   return LSFIapplyTextFilter(L, 1, new TXTencodingFilter(NULL, FromEncoding, ToEncoding));
}

void LIVaddIconvNamespace(lua_State* L) {
   COL_FUNCTION(LIVaddIconvNamespace);
   
   lua_newtable(L); // iconv
   {
      LUAaddMethod(L, "supported", &LIViconvIsSupported);
      LUAaddMethod(L, "convert", &LIViconvConvert);
      LUAaddMethod(L, "list", &LIViconvListEncodings);
      LUAaddMethod(L, "aliases", &LIViconvListAliases);
   }
   lua_setglobal(L, "iconv");
}
