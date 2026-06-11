//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TIMETutil
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Tuesday, October 5th, 2010 @ 06:41:17 PM
//
//---------------------------------------------------------------------------
#include "TIMETprecomp.h"
#pragma hdrstop

#include "TIMETdate.h"
#include <TIME/TIMEdate.h>

#include <COL/COLhashmap.h>
#include <COL/COLdateTime.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static int TIMET_LEAP_YEAR_LIST[] = {
1804, 1808, 1812, 1816, 1820, 1824, 1828, 1832, 1836, 1840, 1844, 1848, 1852, 1856, 1860, 1864, 1868, 1872, 1876, 1880, 1884, 1888, 1892, 1896, 
1904, 1908, 1912, 1916, 1920, 1924, 1928, 1932, 1936, 1940, 1944, 1948, 1952, 1956, 1960, 1964, 1968, 1972, 1976, 1980, 1984, 1988, 1992, 1996,
2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036, 2040, 2044, 2048, 2052, 2056, 2060, 2064, 2068, 2072, 2076, 2080, 2084, 2088, 2092, 2096,
2104, 2108, 2112, 2116, 2120, 2124, 2128, 2132, 2136, 2140, 2144, 2148, 2152, 2156, 2160, 2164, 2168, 2172, 2176, 2180, 2184, 2188, 2192, 2196,
2204, 2208, 2212, 2216, 2220, 2224, 2228, 2232, 2236, 2240, 2244, 2248, 2252, 2256, 2260, 2264, 2268, 2272, 2276, 2280, 2284, 2288, 2292, 2296,
0
};


void testLeapYear()
{
   COLhashmap<int, bool> LeapYearFlagMap;
   for(COLindex LeapYearIndex=0; TIMET_LEAP_YEAR_LIST[LeapYearIndex]; LeapYearIndex++)
   {
      LeapYearFlagMap[TIMET_LEAP_YEAR_LIST[LeapYearIndex]]=true;
   }
   TST_ASSERT( !TIMEisLeapYear(1969) );
   TST_ASSERT( !TIMEisLeapYear(1970) );
   TST_ASSERT( !TIMEisLeapYear(1971) );
   TST_ASSERT( TIMEisLeapYear(1972) );
   TST_ASSERT( !TIMEisLeapYear(1973) );
   TST_ASSERT( !TIMEisLeapYear(1974) );
   TST_ASSERT( !TIMEisLeapYear(1975) );
   TST_ASSERT( TIMEisLeapYear(1976) );
   for(COLindex TestYear=1804; TestYear<=2296; TestYear++)
   {
      bool IsLeapYear = LeapYearFlagMap[TestYear]?true:false;
      TST_ASSERT_EQUALS(IsLeapYear, (TIMEisLeapYear(TestYear)?true:false) );
      TST_ASSERT_EQUALS( (IsLeapYear?366:365), TIMEdaysInYear(TestYear) );
      TST_ASSERT_EQUALS( (IsLeapYear?29:28), TIMEdaysInMonth(TestYear,2) );
   }
}

void testUnixDay()
{
   TST_ASSERT_EQUALS(-365, TIMEunixDay(1969, 1, 1) );
   TST_ASSERT_EQUALS(0, TIMEunixDay(1970, 1, 1) );
   TST_ASSERT_EQUALS(1, TIMEunixDay(1970, 1, 2) );
   TST_ASSERT_EQUALS(30, TIMEunixDay(1970, 1, 31) );
   TST_ASSERT_EQUALS(31, TIMEunixDay(1970, 2, 1) );
   TST_ASSERT_EQUALS(364, TIMEunixDay(1970, 12, 31) );
   TST_ASSERT_EQUALS(365, TIMEunixDay(1971, 1, 1) );
   TST_ASSERT_EQUALS(365+365, TIMEunixDay(1972, 1, 1) );
   TST_ASSERT_EQUALS(365+365+366, TIMEunixDay(1973, 1, 1) ); // 1972 is leap
   TST_ASSERT_EQUALS(365+365+366+365, TIMEunixDay(1974, 1, 1) );
   TST_ASSERT_EQUALS(365+365+366+365+365, TIMEunixDay(1975, 1, 1) );
}

void testDateFromUnixDay()
{
   TST_ASSERT_EQUALS( COLstring("1900-01-01"), TIMEdateString(TIMEdateFromUnixDay(-25567)) );
   TST_ASSERT_EQUALS( COLstring("1968-01-01"), TIMEdateString(TIMEdateFromUnixDay(-731)) );
   TST_ASSERT_EQUALS( COLstring("1969-01-01"), TIMEdateString(TIMEdateFromUnixDay(-365)) );
   TST_ASSERT_EQUALS( COLstring("1970-01-01"), TIMEdateString(TIMEdateFromUnixDay(0)) );
   TST_ASSERT_EQUALS( COLstring("1971-01-01"), TIMEdateString(TIMEdateFromUnixDay(365)) );
   TST_ASSERT_EQUALS( COLstring("1972-01-01"), TIMEdateString(TIMEdateFromUnixDay(730)) );
   TST_ASSERT_EQUALS( COLstring("2010-11-04"), TIMEdateString(TIMEdateFromUnixDay(14917)) );
   TST_ASSERT_EQUALS( COLstring("2012-01-01"), TIMEdateString(TIMEdateFromUnixDay(15340)) );
   TST_ASSERT_EQUALS( COLstring("2012-09-30"), TIMEdateString(TIMEdateFromUnixDay(15613)) );
   TST_ASSERT_EQUALS( COLstring("2012-10-01"), TIMEdateString(TIMEdateFromUnixDay(15614)) );
   TST_ASSERT_EQUALS( COLstring("2012-10-02"), TIMEdateString(TIMEdateFromUnixDay(15615)) );
   TST_ASSERT_EQUALS( COLstring("2099-01-01"), TIMEdateString(TIMEdateFromUnixDay(47117)) );


   COLdateTime TodayDateTime = COLdateTime::currentTime();
   COLstring TodayDateString = TodayDateTime.format("%Y-%m-%d");
   TIMEdate TodayDate(TodayDateTime.year(), TodayDateTime.month(), TodayDateTime.day());
   int TodayUnixDay = TIMEunixDay(TodayDate);

   COL_TRC("Today - " << TodayDateString << " = " << TodayUnixDay);

   TST_ASSERT_EQUALS( TodayDateString, TIMEdateString(TIMEdateFromUnixDay(TodayUnixDay)) );
}

void testDayOfWeekMonday0()
{
   TST_ASSERT_EQUALS( 3, TIMEdayOfWeekMonday0(1970,1,1) ); // thu
   TST_ASSERT_EQUALS( 6, TIMEdayOfWeekMonday0(2010,10,31) ); // sun
   TST_ASSERT_EQUALS( 0, TIMEdayOfWeekMonday0(2010,11,1) ); // mon
   TST_ASSERT_EQUALS( 1, TIMEdayOfWeekMonday0(2010,11,2) ); // tue
   TST_ASSERT_EQUALS( 2, TIMEdayOfWeekMonday0(2010,11,3) ); // wed
   TST_ASSERT_EQUALS( 3, TIMEdayOfWeekMonday0(2010,11,4) ); // thu
   TST_ASSERT_EQUALS( 4, TIMEdayOfWeekMonday0(2010,11,5) ); // fri
   TST_ASSERT_EQUALS( 5, TIMEdayOfWeekMonday0(2010,11,6) ); // sat
   TST_ASSERT_EQUALS( 6, TIMEdayOfWeekMonday0(2010,11,7) ); // sun
   TST_ASSERT_EQUALS( 2, TIMEdayOfWeekMonday0(2010,11,10) ); // wed
}

void testDayOfWeekSunday0()
{
   TST_ASSERT_EQUALS( 3, TIMEdayOfWeekSunday0(1969,1,1) ); // wed
   TST_ASSERT_EQUALS( 4, TIMEdayOfWeekSunday0(1970,1,1) ); // thu
   TST_ASSERT_EQUALS( 0, TIMEdayOfWeekSunday0(2010,10,31) ); // sun
   TST_ASSERT_EQUALS( 1, TIMEdayOfWeekSunday0(2010,11,1) ); // mon
   TST_ASSERT_EQUALS( 2, TIMEdayOfWeekSunday0(2010,11,2) ); // tue
   TST_ASSERT_EQUALS( 3, TIMEdayOfWeekSunday0(2010,11,3) ); // wed
   TST_ASSERT_EQUALS( 4, TIMEdayOfWeekSunday0(2010,11,4) ); // thu
   TST_ASSERT_EQUALS( 5, TIMEdayOfWeekSunday0(2010,11,5) ); // fri
   TST_ASSERT_EQUALS( 6, TIMEdayOfWeekSunday0(2010,11,6) ); // sat
   TST_ASSERT_EQUALS( 0, TIMEdayOfWeekSunday0(2010,11,7) ); // sun
   TST_ASSERT_EQUALS( 3, TIMEdayOfWeekSunday0(2010,11,10) ); // wed
   TST_ASSERT_EQUALS( 2, TIMEdayOfWeekSunday0(2012,8,14) ); // tue
}

void testDayOfYear()
{
   TST_ASSERT_EQUALS( 0, TIMEdayOfYearBase0(1970,1,1) );
   TST_ASSERT_EQUALS( 1, TIMEdayOfYearBase0(1970,1,2) );
   TST_ASSERT_EQUALS( 30, TIMEdayOfYearBase0(1970,1,31) );
   TST_ASSERT_EQUALS( 31, TIMEdayOfYearBase0(1970,2,1) );
   TST_ASSERT_EQUALS( 59, TIMEdayOfYearBase0(1970,3,1) );
   TST_ASSERT_EQUALS( 363, TIMEdayOfYearBase0(1970,12,30) );
   TST_ASSERT_EQUALS( 364, TIMEdayOfYearBase0(1970,12,31) );
   TST_ASSERT_EQUALS( 0, TIMEdayOfYearBase0(1971,1,1) );
   TST_ASSERT_EQUALS( 363, TIMEdayOfYearBase0(1971,12,30) );
   TST_ASSERT_EQUALS( 364, TIMEdayOfYearBase0(1971,12,31) );
   TST_ASSERT_EQUALS( 226, TIMEdayOfYearBase0(2012,8, 14) );
}

TSTtestable* TIMETdateTests()
{
   TST_START_COLLECTION("date")
      TST_ADD_TEST_FUNCTION(testLeapYear);
      TST_ADD_TEST_FUNCTION(testUnixDay);
      TST_ADD_TEST_FUNCTION(testDateFromUnixDay);
      TST_ADD_TEST_FUNCTION(testDayOfWeekMonday0);
      TST_ADD_TEST_FUNCTION(testDayOfWeekSunday0);
      TST_ADD_TEST_FUNCTION(testDayOfYear);
   TST_END_FIXTURE
};

