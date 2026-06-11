// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGtime
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 03 November 2025 - 01:26PM
// ---------------------------------------------------------------------------
#include <LIG/LIGtime.h>

#include <LIG/LIGdate.h>
#include <LIG/LIGdifftime.h>
#include <LIG/LIGdateTimeUtils.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>

#include <COL/COLdateTime.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool LIGgetFieldNumberOptional(lua_State *L, const char* FieldName, lua_Number* pOutNumber){
   COL_FUNCTION(LIGgetFieldNumberOptional);
   lua_getfield(L, -1, FieldName);
   if(lua_isnoneornil(L, -1)){
      lua_pop(L, 1);
      return false;
   }
   if(lua_isnumber(L, -1)){
      if(pOutNumber)*pOutNumber = lua_tonumber(L, -1);
      lua_pop(L, 1);
      return true;
   }
   luaL_error(L, "field '%s' must be a number", FieldName);
   // if(pOutNumber)*pOutNumber = 0;
   // lua_pop(L, 1);
   return false;
}

static int LIGgetBoolField(lua_State *L, const char *key){
   COL_FUNCTION(LIGgetBoolField);
   int res;
   lua_getfield(L, -1, key);
   res = lua_isnil(L, -1) ? -1 : lua_toboolean(L, -1);
   lua_pop(L, 1);
   return res;
}

static int LIGdisplayTime(lua_State* L) {
   COL_FUNCTION(LIGdisplayTime);
   lua_pushliteral(L, "%Y-%m-%d %H:%M:%S");
   lua_insert(L, 1);  // Format goes first.
   lua_settop(L, 2);  // Strip off extra args, if any.
   return LIGdate(L);  // os.date(fmt, t)
}

static int LIGtimeEq(lua_State* L) {
   COL_FUNCTION(LIGtimeEq);
   return LIGdifftime(L) == 1 && lua_tonumber(L,-1) == 0;
}

static int LIGtimeLt(lua_State* L) {
   COL_FUNCTION(LIGtimeLt);
   return LIGdifftime(L) == 1 && lua_tonumber(L,-1) < 0;
}

static void LIGpushDateTime(lua_State* L, const COLdateTime& DateTime) {
   COL_FUNCTION(LIGpushDateTime);
   LUApushObject<COLdateTime>(L, DateTime, LIG6_TIME_META_NAME);
   if(lua_getmetatable(L,-1)) {
#define M(N,F) lua_pushliteral(L,N); lua_pushcfunction(L,F); lua_settable(L,-3)
      M("__tostring", LIGdisplayTime);
      M("__eq",       LIGtimeEq);
      M("__lt",       LIGtimeLt);
#undef M
      lua_pop(L,1);
   }
}

static COLdateTime LIGmakeTime(struct tm *pTm) {
   COL_FUNCTION(LIGmakeTime);
   COLdateTime Out;
   struct tm Tm = *pTm;
   COLdateTime DateTime(Tm.tm_year + LIG6_TM_YEAR_ZERO, Tm.tm_mon + 1, Tm.tm_mday,
                        Tm.tm_hour, Tm.tm_min, Tm.tm_sec);
   if(DateTime.status() == COLdateTime::valid) {
      Out = DateTime;
   }
   return Out;
}

int LIGtime(lua_State* L){
   COL_FUNCTION(LIGtime);
   try {
      COLdateTime Out;
      if (lua_isnoneornil(L, 1)) { /* called without args? */
         Out = COLdateTime::currentTime();
      } else {
         struct tm ts;
         luaL_checktype(L, 1, LUA_TTABLE);
         lua_settop(L, 1);  /* make sure table is at the top */
         if(LIGgetFieldNumberOptional(L, "ole", 0)) {
            lua_Number Ole=0;
            COLPRECONDITION(LIGgetFieldNumberOptional(L, "ole", &Ole));
            Out = (DATE)Ole;
         } else {
            ts.tm_sec  = LIGdateTimeGetField(L, "sec", 0);
            ts.tm_min  = LIGdateTimeGetField(L, "min", 0);
            ts.tm_hour = LIGdateTimeGetField(L, "hour", 12);
            ts.tm_mday = LIGdateTimeGetField(L, "day", -1);
            ts.tm_mon  = LIGdateTimeGetField(L, "month", -1) - 1;
            ts.tm_year = LIGdateTimeGetField(L, "year", -1)  - LIG6_TM_YEAR_ZERO;
            ts.tm_isdst = LIGgetBoolField(L, "isdst");
            Out = LIGmakeTime(&ts);
         }
      }

      if (Out.status() == COLdateTime::valid) { LIGpushDateTime(L, Out); } 
      else                                    { lua_pushnil(L); }
   } catch(const COLerror& e) {
      return luaL_error(L, "%s", e.description().c_str());
   }
   return 1;
}