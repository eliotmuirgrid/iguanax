#ifndef __LIG_DATE_TIME_UTILS_H__
#define __LIG_DATE_TIME_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGdateTimeUtils
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Monday 03 November 2025 - 04:23PM
// ---------------------------------------------------------------------------
struct lua_State;
class COLdateTime;

#define LIG6_TM_YEAR_ZERO 1900
#define LIG6_TIME_META_NAME    "time_value_df"
#define LIG6_INVALID_TM "Invalid argument. This function requires its time parameters to be in the 'userdata' format returned by os.time().\n(The os.ts.* date and time functions take UNIX timestamps, and may be what you want instead.)"

void LIGdateTimeSetField(lua_State *L, const char *key, int value);
int LIGdateTimeGetField(lua_State *L, const char *key, int d);

struct tm* LIGdateTimeUserDataToTm(const COLdateTime& dt, struct tm *pOut);

#endif // end of defensive include
