// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGdateTimeUtils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 03 November 2025 - 04:23PM
// ---------------------------------------------------------------------------
#include <LIG/LIGdateTimeUtils.h>

#include <LUA/LUAerror.h>
#include <LUAC/lualib.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int LIGdateTimeGetField(lua_State *L, const char *key, int d){
   COL_FUNCTION(LIGdateTimeGetField);
   int res;
   lua_getfield(L, -1, key);
   if (lua_isnumber(L, -1))
      res = (int)lua_tointeger(L, -1);
   else {
      if (d < 0)
         return luaL_error(L, "field '%s' missing in date table", key);
      res = d;
   }
   lua_pop(L, 1);
   return res;
}

void LIGdateTimeSetField(lua_State *L, const char *key, int value){
   COL_FUNCTION(LIGdateTimeSetField);
   lua_pushinteger(L, value);
   lua_setfield(L, -2, key);
}

struct tm* LIGdateTimeUserDataToTm(const COLdateTime& dt, struct tm *pOut){
   COL_FUNCTION(LIGdateTimeUserDataToTm);
   COLdateTime Time = dt;
   if(Time.status() == COLdateTime::invalid){ 
      COL_TRC("Invalid datetime - exit");
      return NULL; 
   }
   memset(pOut, 0, sizeof *pOut);
   pOut->tm_sec   = Time.second();
   pOut->tm_min   = Time.minute();
   pOut->tm_hour  = Time.hour();
   pOut->tm_mday  = Time.day();
   pOut->tm_mon   = Time.month() - 1;
   pOut->tm_year  = Time.year()  - LIG6_TM_YEAR_ZERO;
   pOut->tm_wday  = Time.dayOfWeek() - 1;
   pOut->tm_yday  = Time.dayOfYear() - 1;
   return pOut;
}