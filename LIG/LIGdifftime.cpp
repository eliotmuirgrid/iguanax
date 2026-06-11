// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGdifftime
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 03 November 2025 - 04:12PM
// ---------------------------------------------------------------------------
#include <LIG/LIGdifftime.h>
#include <LIG/LIGdateTimeUtils.h>

#include <LUA/LUAutils.h>

#include <cmath> // floor
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int LIGdifftime(lua_State* L){
   COL_FUNCTION(LIGdifftime);
   try {
      COLdateTime t1, t2;
      t1 = *(COLdateTime*)LUAcheckUdata(L, 1, LIG6_TIME_META_NAME, LIG6_INVALID_TM);

      if (lua_isnil(L, 2)) { t2 = COLdateTime((time_t) 0); } 
      else                 { t2 = *(COLdateTime*)LUAcheckUdata(L, 2, LIG6_TIME_META_NAME, LIG6_INVALID_TM); }

      COLdateTimeSpan Span = (t1 - t2);
      lua_pushnumber(L, floor(Span.totalSeconds()+0.5));
   } catch(const COLerror& Error) {
      return luaL_error(L, "%s", Error.description().c_str());
   }
   return 1;
}
