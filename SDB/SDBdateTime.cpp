// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdateTime
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Wednesday 01 March 2023 - 08:43PM
// ---------------------------------------------------------------------------

#include <SDB/SDBdateTime.h>

#include <cstdlib>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static struct tm SDBconvertToStruct(time_t EpochTime, bool UTC) {
   return UTC ? *gmtime(&EpochTime) : *localtime(&EpochTime);
}

COLstring SDBdateMs(unsigned long long DateTime, const char* Format, bool UTC) {
   time_t EpochTime = DateTime / 1000;
   return SDBdateTime(EpochTime, Format, UTC);
}

COLstring SDBdateTimeMs(unsigned long long DateTime, const char* Format, bool UTC) {
   time_t EpochTime = DateTime / 1000;
   int Millisecond = DateTime % 1000;
   COLstring Timestamp = SDBdateTime(EpochTime, Format, UTC);
   if (Millisecond) {
      Timestamp += "." + COLintToString(Millisecond);
   }
   return Timestamp;
}

COLstring SDBdate(time_t DateTime, const char* Format, bool UTC) {
   return SDBdateTime(DateTime, Format, UTC);
}

COLstring SDBdateTime(time_t DateTime, const char* Format, bool UTC) {
   struct tm Components = SDBconvertToStruct(DateTime, UTC);
   return SDBdateTime(Components, Format);
}

COLstring SDBdate(struct tm DateTime, const char* Format) {
   return SDBdateTime(DateTime, Format);
}

COLstring SDBdateTime(struct tm DateTime, const char* Format) {
   COLstring DateString;
   DateString.setCapacity(128);
   int Amount = strftime(DateString.get_buffer(), 128, Format, &DateTime);
   DateString.setSize(Amount);
   return DateString;
}


// POSIX only
//struct tm SDBdateTimeComponentsTimeZone(time_t EpochTime, const char* TimeZone) {
//   char Buffer[32] = "TZ=";
//   strncat(Buffer, TimeZone, 32);
//   putenv(Buffer);
//   return *localtime(&EpochTime);
//}