// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGdate
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 03 November 2025 - 04:12PM
// ---------------------------------------------------------------------------
#include <LIG/LIGdate.h>
#include <LIG/LIGdateTimeUtils.h>

#include <LUA/LUAutils.h>

#include <COL/COLdateTime.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void LIGdateTable(lua_State* L, const COLdateTime& LuaDateTime){
   COL_FUNCTION(LIGdateTable);
   struct tm Tm = {0};
   COLPRECONDITION(LIGdateTimeUserDataToTm(LuaDateTime, &Tm));
   lua_createtable(L, 0, 9);  // 9 = number of fields 
   LIGdateTimeSetField(L, "sec",   Tm.tm_sec);
   LIGdateTimeSetField(L, "min",   Tm.tm_min);
   LIGdateTimeSetField(L, "hour",  Tm.tm_hour);
   LIGdateTimeSetField(L, "day",   Tm.tm_mday);
   LIGdateTimeSetField(L, "month", Tm.tm_mon  + 1);
   LIGdateTimeSetField(L, "year",  Tm.tm_year + LIG6_TM_YEAR_ZERO);
   LIGdateTimeSetField(L, "wday",  Tm.tm_wday + 1);
   LIGdateTimeSetField(L, "yday",  Tm.tm_yday + 1);
   // setboolfield(L, "isdst", Tm.tm_isdst);
}

int LIGdate(lua_State* L){
   COL_FUNCTION(LIGdate);
   try {
      const char *FormatString = luaL_optstring(L, 1, "%c");
      COLdateTime LuaDateTime;
      if (lua_isnoneornil(L, 2)) { LuaDateTime = COLdateTime::currentTime(); } 
      else                       { LuaDateTime= *(COLdateTime*)LUAcheckUdata(L, 2, LIG6_TIME_META_NAME, LIG6_INVALID_TM); }

      if (*FormatString == '!') {  // UTC?
         FormatString++;  // skip '!'
      }

      if (LuaDateTime.status() != COLdateTime::valid) {
         COL_TRC("Invalid datetime - return nil");
         lua_pushnil(L);
      } else if (strcmp(FormatString,"*ole") == 0) {
         COL_TRC("datetime converted to an OLE Automation date - return OLE date");
         lua_pushnumber(L, (DATE)LuaDateTime);
      } else if (strcmp(FormatString, "*t") == 0) {
         COL_TRC("format string equals *t - returning Lua table containing the date-time components");
         LIGdateTable(L, LuaDateTime);
      } else {
         COL_TRC("returning formatted timestamp");
         LUApushString(L, LuaDateTime.format(FormatString));         
      }
   } catch(const COLerror& Error) {
      return luaL_error(L, "%s", Error.description().c_str());
   }
   return 1;
}