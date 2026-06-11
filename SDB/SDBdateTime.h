#ifndef __SDB_DATE_TIME_H__
#define __SDB_DATE_TIME_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdateTime
//
// Description:
//
// UNIX time_t based date time functions.
//
// Author: John Qi
// Date:   Wednesday 01 March 2023 - 08:43PM
// ---------------------------------------------------------------------------

#include <time.h>

class COLstring;

COLstring SDBdateMs(unsigned long long DateTime, const char* Format = "%Y%m%d", bool UTC = false);
COLstring SDBdateTimeMs(unsigned long long DateTime, const char* Format = "%FT%T", bool UTC = false);

COLstring SDBdate(time_t DateTime = time(NULL), const char* Format = "%Y%m%d", bool UTC = false);
COLstring SDBdateTime(time_t DateTime = time(NULL), const char* Format = "%FT%T%z", bool UTC = false);

COLstring SDBdate(struct tm DateTime, const char* Format = "%Y%m%d");
COLstring SDBdateTime(struct tm DateTime, const char* Format = "%FT%T%z");

#endif // end of defensive include