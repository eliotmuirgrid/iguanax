#ifndef __TIME_TS_H__
#define __TIME_TS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TIMEts
//
// Description:
//
// TIMEts class.
//
// Author: Dmitri Klimenko
// Date:   Tuesday, August 14th, 2012 @ 09:20:40 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

COLint64 TIMEtsFromDateTimeUtc(int Year, int Month, int Day, int Hour, int Minute, int Second);

struct TIMEtime
{
   int Year;
   int Month;
   int Day;
   int Hour;
   int Minute;
   int Second;

   TIMEtime(): Year(0), Month(0), Day(0), Hour(0), Minute(0), Second(0){};
   TIMEtime(int Year, int Month, int Day, int Hour, int Minute, int Second)
   {
      this->Year = Year;
      this->Month = Month;
      this->Day = Day;
      this->Hour = Hour;
      this->Minute = Minute;
      this->Second = Second;
   };
};

bool operator==(const TIMEtime& Time1, const TIMEtime& Time2);
bool operator!=(const TIMEtime& Time1, const TIMEtime& Time2);

TIMEtime TIMEdateTimeUtcFromTs(COLint64 Ts);

#endif // end of defensive include
