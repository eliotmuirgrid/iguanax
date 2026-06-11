//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNextractErrorString
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, November 26th, 2010 @ 12:57:05 PM
//---------------------------------------------------------------------------
#include "TRNextractErrorString.h"
#include "TRNannotationState.h"
#include "TRNfile.h"
#include "TRNconstant.h"

#include <REX/REXmatcher.h>

#include <LUAL/LUALprint.h>
#include <LUA/LUAutils.h>

#include <FMT/FMTbinaryDump.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNextractErrorString(lua_State* L){
   COL_FUNCTION(TRNextractErrorString);
   // All non-string errors thrown should have __tostring meta method
   // for pretty printing in the GUI.
   if (lua_getmetatable(L, -1)) {
      lua_getfield(L, -1, "__tostring");
      if (lua_isfunction (L, -1)) {
         lua_remove(L, -2);     // Remove the metatable
         lua_insert(L, -2);     // Swap function and the table
         lua_pcall(L, 1, 1, 0); // Pops function and argument, and leaves __tostring result on top.
      } else {
         lua_pop (L, 2); // Pop the non-function and the metatable
      }
   } else if (!lua_isstring(L, -1)){
      COL_TRC("NOT A STRING!");
      COLstring SerializedError;
      COLostream SerializedErrorStream(SerializedError);
      LUALprintItem(L, lua_gettop(L), SerializedErrorStream);
      lua_pop(L, 1); // Pop error item...
      LUApushString(L, SerializedError); // ...and replace it with a string version.
      COL_VAR(SerializedError);
   }
}
