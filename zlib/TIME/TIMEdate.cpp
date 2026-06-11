//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TIMEdate
//
// Description: basic calendar logic functions
//
// Author: Dmitri Klimenko
// Date:   Tuesday, October 5th, 2010 @ 06:15:58 PM
//
//---------------------------------------------------------------------------
#include "TIMEprecomp.h"
#pragma hdrstop

#include "TIMEdate.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TIMEisLeapYear(int Year)
{
   return (Year%4 == 0 && (Year%100 != 0 || Year % 400 == 0)); // from mail_webserver/web_docs/date_chooser.js
};

// non-leap - February = 28
static int TIME_DAYS_PER_MONTH_NONLEAP[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// leap - February = 29
static int TIME_DAYS_PER_MONTH_LEAP[] =    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int TIMEdaysInMonth(int Year, int Month)
{
   COLPRECONDITION( Month >= 1 && Month <= 12 );
   if(TIMEisLeapYear(Year))
   {
      return TIME_DAYS_PER_MONTH_LEAP[Month-1];
   }
   return TIME_DAYS_PER_MONTH_NONLEAP[Month-1];
}

int TIMEdaysInYear(int Year)
{
   if(TIMEisLeapYear(Year))
   {
      return 366;
   }
   return 365;
}

int TIMEunixDay(int Year, int Month, int Day)
{
   // returns the number of days since Unix Epoch, with 1970-01-01 = 0

   COLPRECONDITION( Month >=1 && Month <= 12 );
   COLPRECONDITION( Day >= 1 && Day <= TIMEdaysInMonth(Year, Month) );

   int TotalDays = 0;
   if(Year < 1970)
   {
      // just in case
      for(int YearBefore=Year; YearBefore < 1970; YearBefore++)
      {
         TotalDays -= TIMEdaysInYear(YearBefore);
      }
   }
   else
   {
      for(int YearBefore=1970; YearBefore < Year; YearBefore++)
      {
         TotalDays += TIMEdaysInYear(YearBefore);
      }
   }
   for(int MonthBefore = 1; MonthBefore < Month; MonthBefore++)
   {
      TotalDays += TIMEdaysInMonth(Year, MonthBefore);
   };
   TotalDays += (Day - 1);
   return TotalDays;
};

int TIMEunixDay(const TIMEdate& Value)
{
   return TIMEunixDay(Value.year(), Value.month(), Value.day());
}

TIMEdate TIMEdateFromUnixDay(int UnixDay)
{
   int Year = 1970;
   int Month = 1;
   int Day = 1;

   int YearDay = UnixDay;

   if(YearDay >= 0)
   {
      while(YearDay >= TIMEdaysInYear(Year))
      {
         YearDay -= TIMEdaysInYear(Year);
         Year++;
      }
   }
   else
   {
      while(YearDay < 0)
      {
         YearDay += TIMEdaysInYear(Year-1);
         Year --;
      }
   }
   COLPRECONDITION(YearDay >= 0 && YearDay < TIMEdaysInYear(Year) );

   int MonthDay = YearDay;
   while(MonthDay >= TIMEdaysInMonth(Year, Month))
   {
      MonthDay -= TIMEdaysInMonth(Year, Month);
      Month++;
   }

   COLPRECONDITION(MonthDay >= 0 && MonthDay < TIMEdaysInMonth(Year,Month));
   Day += MonthDay;

   return TIMEdate(Year,Month,Day);
}

COLstring TIMEdateString(const TIMEdate& Value, char Separator)
{
   char Buffer[64];
   snprintf(Buffer, 64, "%04d%c%02d%c%02d", Value.year(), Separator, Value.month(), Separator, Value.day() );
   return COLstring(Buffer);
};

int TIMEdayOfWeekMonday0(int Year, int Month, int Day)
{
   int UnixDay = TIMEunixDay(Year, Month, Day);
   int DayOfWeek = (UnixDay+3)%7;
   if(DayOfWeek<0)
   {
      DayOfWeek += 7;
   }
   COLPRECONDITION(DayOfWeek >= 0 && DayOfWeek <= 6);
   return DayOfWeek;
}

int TIMEdayOfWeekSunday0(int Year, int Month, int Day)
{
   int UnixDay = TIMEunixDay(Year, Month, Day);
   int DayOfWeek = (UnixDay+4)%7;
   if(DayOfWeek<0)
   {
      DayOfWeek += 7;
   }
   COLPRECONDITION(DayOfWeek >= 0 && DayOfWeek <= 6);
   return DayOfWeek;
}

int TIMEdayOfYearBase0(int Year, int Month, int Day)
{
   return TIMEunixDay(Year, Month, Day)-TIMEunixDay(Year,1,1);
}
