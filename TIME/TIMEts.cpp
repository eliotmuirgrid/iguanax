//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TIMEts
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Tuesday, August 14th, 2012 @ 09:20:40 AM
//
//---------------------------------------------------------------------------
#include "TIMEprecomp.h"
#pragma hdrstop

#include "TIMEts.h"
#include "TIMEdate.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const int TIME_SECONDS_IN_DAY = 3600*24;
static const int TIME_SECONDS_IN_HOUR = 3600;
static const int TIME_SECONDS_IN_MINUTE = 60;

// compute a timestamp from a DateTime in UTC
COLint64 TIMEtsFromDateTimeUtc(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
   // returns the number of seconds since Unix Epoch, with 1970-01-01 = 0

   COLint64 TotalDays = TIMEunixDay(Year, Month, Day);

   COLPRECONDITION(Hour >= 0 && Hour < 24);
   COLPRECONDITION(Minute >=0 && Minute < 60);
   COLPRECONDITION(Second >=0 && Second < 60);
   COLint64 TotalHours = TotalDays * 24 + Hour;
   COLint64 TotalMinutes = TotalHours * 60 + Minute;
   COLint64 TotalSeconds = TotalMinutes * 60 + Second;

   return TotalSeconds;
};

TIMEtime TIMEdateTimeUtcFromTs(COLint64 Ts)
{
   COLint64 UnixDay = Ts / TIME_SECONDS_IN_DAY;
   COLint64 DaySeconds = Ts % TIME_SECONDS_IN_DAY;
   if(DaySeconds<0)
   {
      DaySeconds += TIME_SECONDS_IN_DAY;
      UnixDay --;
   }
   TIMEdate Date = TIMEdateFromUnixDay(UnixDay);
   TIMEtime Time;
   Time.Year = Date.year();
   Time.Month = Date.month();
   Time.Day = Date.day();
   Time.Hour = DaySeconds / TIME_SECONDS_IN_HOUR;
   Time.Minute = (DaySeconds % TIME_SECONDS_IN_HOUR) / TIME_SECONDS_IN_MINUTE;
   Time.Second = DaySeconds % TIME_SECONDS_IN_MINUTE;
   return Time;
};

bool operator==(const TIMEtime& Time1, const TIMEtime& Time2)
{
   return true
      && Time1.Year   == Time2.Year
      && Time1.Month  == Time2.Month
      && Time1.Day    == Time2.Day
      && Time1.Hour   == Time2.Hour
      && Time1.Minute == Time2.Minute
      && Time1.Second == Time2.Second
      ;
};

bool operator!=(const TIMEtime& TIme1, const TIMEtime& Time2)
{
   return !(TIme1==Time2);
}
