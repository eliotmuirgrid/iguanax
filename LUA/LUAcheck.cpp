// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAcheck
//
// Description:
//
// Implementation
//
// Author: Eliot Muir (probably Eric's code originally)
// Date:   Tuesday 16 May 2023 - 04:37PM
// ---------------------------------------------------------------------------

#include <LUA/LUAcheck.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void LUAcheckParams(lua_State* L, int i, const char *const aOptions[]) {
   COL_FUNCTION(LUAcheckParams);
   int Top = lua_gettop(L);
   if(i<0) i+=Top+1;  // Table position needs to be absolute.

   COL_VAR2(lua_istable(L,i), lua_typename(L, lua_type(L,i)));
   LUAparamIsTable(L, i);
   for(lua_pushnil(L); lua_next(L,i); lua_pop(L,1)) {
      if(lua_type(L,-2) != LUA_TSTRING) {
         LUA_ERROR_STREAM("Use named argument form e.g. {data=Data}.");
      }
      const char* pKey = lua_tostring(L,-2);
      for(const char *const *opt=aOptions; *opt; opt++) {
         if(strcmp(pKey, *opt) == 0) goto known_option;
      }
      LUA_ERROR_STREAM("Unknown argument " << pKey << " or not using named argument form e.g. {data=Data}");
known_option:;
   }
   lua_settop(L, Top);
}

int LUAcheckArgCount(lua_State* L, int Low, int High) {
   int Top = lua_gettop(L);
   // High < 0 means args must be exactly Low
   if (High < 0 && Top != Low) {
      luaL_error(L, "%d argument%s expected; got %d", Low, (Low == 1 ? "" : "s"), Top);
   } else if(High >= 0 && (Top < Low || Top > High)) {
      luaL_error(L, "%d to %d arguments expected; got %d", Low, High, Top);
   }
   return Top;
}

bool LUAparamsAreKnown(const COLvar& VarMap, const char* const KnownParameters[], COLstring* OffendingKey) {
   COL_FUNCTION(LUAparamsAreKnown);
   if (!VarMap.isMap()) {
      COL_ERROR_STREAM("Expected a table.", COLerror::PreCondition);
   }

   for (COLvar::COLvarMap::const_iterator it = VarMap.map().cbegin(); it != VarMap.map().cend(); it++) {
      for (const char* const* KnownParam = KnownParameters; *KnownParam; KnownParam++) {
         COL_VAR2(*KnownParam, it->first);
         if ((*it)->first.equals(*KnownParam)) {
            // This param is in the known list, so move on and check the next one
            goto NextParam;
         }
      }
      // Getting here means that the continue didn't trigger => Param is not within the list
      if (OffendingKey) {
         *OffendingKey = (*it)->first;
      }
      COL_TRC("Parameter " << (*it)->first << " is unknown");
      return false;
NextParam:;
   }
   return true;
}