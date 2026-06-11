#ifndef __TIME_DATE_H__
#define __TIME_DATE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TIMEutil
//
// Description:
//
// TIMEutil class.
//
// Author: Dmitri Klimenko
// Date:   Tuesday, October 5th, 2010 @ 06:15:58 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class TIMEdate
{
   // POD
public:
   TIMEdate(int Year, int Month, int Day):m_Year(Year), m_Month(Month), m_Day(Day){};
   TIMEdate():m_Year(-1), m_Month(-1), m_Day(-1){};

   int year() const { return m_Year; };
   void setYear(int Year){ m_Year = Year; };

   int month() const { return m_Month; };
   void setMonth(int Month){ m_Month = Month; };

   int day() const { return m_Day; };
   void setDay(int Day){ m_Day = Day; };

protected:
   int m_Year;
   int m_Month;
   int m_Day;
};

bool TIMEisLeapYear(int Year);

int TIMEdaysInMonth(int Year, int Month);

int TIMEdaysInYear(int Year);

int TIMEunixDay(int Year, int Month, int Day);

int TIMEunixDay(const TIMEdate& Value);

TIMEdate TIMEdateFromUnixDay(int UnixDay);

COLstring TIMEdateString(const TIMEdate& Value, char Separator='-');

int TIMEdayOfWeekMonday0(int Year, int Month, int Day);

int TIMEdayOfWeekMonday0(const TIMEdate& Value);

// sunday=0 - same format as in struct tm
int TIMEdayOfWeekSunday0(int Year, int Month, int Day);

// 0..364/365 - same format as in struct tm
int TIMEdayOfYearBase0(int Year, int Month, int Day);

#endif // end of defensive include
