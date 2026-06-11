//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLdateTime.cpp
//
// Description:
//
// Implementation
//
// Author: Michael Vi-Nguyen Truong
// Date:   Monday, February 14, 2005 @ 11:07 AM EST
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLdateTime.h>
#include "testCOLdateTime.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void testFormatting()
{
   COLdateTime Valentine2005(2005, 2, 14, 0, 0, 0);
   COLstring Valentine2005String;
   COLostream Valentine2005Stream(Valentine2005String);

   COLdateTime ChineseNewYear(2005, 2, 9, 23, 2, 3);
   COLstring ChineseNewYearString;
   COLostream ChineseNewYearStream(ChineseNewYearString);

   COLdateTime Tenth(1010, 10, 10, 10, 10, 10);
   COLstring TenthString;
   COLostream TenthStream(TenthString);

   COLuint32 Iterator;

   COLarray<COLstring> ExpectedResults;

   Valentine2005Stream << Valentine2005;
   ChineseNewYearStream << ChineseNewYear;
   TenthStream << Tenth;

   TST_ASSERT_EQUALS(  "Monday, February 14th, 2005 @ 12:00:00 AM", Valentine2005String);
   TST_ASSERT_EQUALS("Wednesday, February 9th, 2005 @ 11:02:03 PM", ChineseNewYearString);
   TST_ASSERT_EQUALS("Wednesday, October 10th, 1010 @ 10:10:10 AM", TenthString);
   
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("1st");
   ExpectedResults.push_back("2nd");
   ExpectedResults.push_back("3rd");
   ExpectedResults.push_back("4th");
   ExpectedResults.push_back("5th");
   ExpectedResults.push_back("6th");
   ExpectedResults.push_back("7th");
   ExpectedResults.push_back("8th");
   ExpectedResults.push_back("9th");
   ExpectedResults.push_back("10th");
   ExpectedResults.push_back("11th");
   ExpectedResults.push_back("12th");
   ExpectedResults.push_back("13th");
   ExpectedResults.push_back("14th");
   ExpectedResults.push_back("15th");
   ExpectedResults.push_back("16th");
   ExpectedResults.push_back("17th");
   ExpectedResults.push_back("18th");
   ExpectedResults.push_back("19th");
   ExpectedResults.push_back("20th");
   ExpectedResults.push_back("21st");
   ExpectedResults.push_back("22nd");
   ExpectedResults.push_back("23rd");
   ExpectedResults.push_back("24th");
   ExpectedResults.push_back("25th");
   ExpectedResults.push_back("26th");
   ExpectedResults.push_back("27th");
   ExpectedResults.push_back("28th");
   ExpectedResults.push_back("29th");
   ExpectedResults.push_back("30th");
   ExpectedResults.push_back("31st");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
   
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      TST_ASSERT_EQUALS(ExpectedResults[Iterator], COLdateTime::dayOfMonthString(Iterator - 1));
   }

   ExpectedResults.clear();

   ExpectedResults.push_back("");   
   ExpectedResults.push_back("");
   ExpectedResults.push_back("Sunday");
   ExpectedResults.push_back("Monday");
   ExpectedResults.push_back("Tuesday");
   ExpectedResults.push_back("Wednesday");
   ExpectedResults.push_back("Thursday");
   ExpectedResults.push_back("Friday");
   ExpectedResults.push_back("Saturday");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
   
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      TST_ASSERT_MESSAGE(COLdateTime::dayOfWeekString(Iterator - 1, true) == ExpectedResults[Iterator], "COLdateTime - Day of Week String Long failed.");
   }

   ExpectedResults.clear();

   ExpectedResults.push_back("");   
   ExpectedResults.push_back("");
   ExpectedResults.push_back("Sun");
   ExpectedResults.push_back("Mon");
   ExpectedResults.push_back("Tue");
   ExpectedResults.push_back("Wed");
   ExpectedResults.push_back("Thu");
   ExpectedResults.push_back("Fri");
   ExpectedResults.push_back("Sat");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
   
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      COL_TRC(newline << ExpectedResults[Iterator] << " ?= " << COLdateTime::dayOfWeekString(Iterator - 1, false));
      TST_ASSERT_MESSAGE(COLdateTime::dayOfWeekString(Iterator - 1, false) == ExpectedResults[Iterator], "COLdateTime - Day of Week String failed.");
   }

   ExpectedResults.clear();

   ExpectedResults.push_back("");   
   ExpectedResults.push_back("");
   ExpectedResults.push_back("January");
   ExpectedResults.push_back("February");
   ExpectedResults.push_back("March");
   ExpectedResults.push_back("April");
   ExpectedResults.push_back("May");
   ExpectedResults.push_back("June");
   ExpectedResults.push_back("July");
   ExpectedResults.push_back("August");
   ExpectedResults.push_back("September");
   ExpectedResults.push_back("October");
   ExpectedResults.push_back("November");
   ExpectedResults.push_back("December");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
   
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      TST_ASSERT_MESSAGE(COLdateTime::monthNumberToString(Iterator - 1, true) == ExpectedResults[Iterator], "COLdateTime - Month of Year String Long failed.");
   }

   ExpectedResults.clear();

   ExpectedResults.push_back("");   
   ExpectedResults.push_back("");
   ExpectedResults.push_back("Jan");
   ExpectedResults.push_back("Feb");
   ExpectedResults.push_back("Mar");
   ExpectedResults.push_back("Apr");
   ExpectedResults.push_back("May");
   ExpectedResults.push_back("Jun");
   ExpectedResults.push_back("Jul");
   ExpectedResults.push_back("Aug");
   ExpectedResults.push_back("Sep");
   ExpectedResults.push_back("Oct");
   ExpectedResults.push_back("Nov");
   ExpectedResults.push_back("Dec");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
   
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      TST_ASSERT_MESSAGE(COLdateTime::monthNumberToString(Iterator - 1, false) == ExpectedResults[Iterator], "COLdateTime - Month of Year String failed.")
   }
   
   ExpectedResults.clear();

   ExpectedResults.push_back("");   
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   ExpectedResults.push_back("a.m.");
   
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("p.m.");
   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
      
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      COL_TRC(newline << ExpectedResults[Iterator] << " ?= " << COLdateTime::meridianString(Iterator - 1, true));
      TST_ASSERT_MESSAGE(COLdateTime::meridianString(Iterator - 1, true) == ExpectedResults[Iterator], "COLdateTime - Meridian String Formal failed.")
   }
   
   ExpectedResults.clear();

   ExpectedResults.push_back("");   
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");
   ExpectedResults.push_back("AM");

   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");
   ExpectedResults.push_back("PM");

   ExpectedResults.push_back("");
   ExpectedResults.push_back("");
      
   for(Iterator = 0; Iterator < ExpectedResults.size(); Iterator++)
   {
      COL_TRC(newline << ExpectedResults[Iterator] << " ?= " << COLdateTime::meridianString(Iterator - 1, false));
      TST_ASSERT_MESSAGE(COLdateTime::meridianString(Iterator - 1, false) == ExpectedResults[Iterator], "COLdateTime - Meridian String failed.")
   }

   // Check the month conversion.
   for (COLuint32 MonthIndex = 1; MonthIndex <= 12; MonthIndex++)
   {
      COL_TRC(MonthIndex << " == " << COLdateTime::shortNameFromMonth(MonthIndex) << newline);
      TST_ASSERT(!COLdateTime::shortNameFromMonth(MonthIndex).is_null());
   }

   // Range check.
   TST_ASSERT(COLdateTime::shortNameFromMonth(0).is_null());
   TST_ASSERT(COLdateTime::shortNameFromMonth(-2).is_null());
   TST_ASSERT(COLdateTime::shortNameFromMonth(13).is_null());
   TST_ASSERT(COLdateTime::shortNameFromMonth(99).is_null());
}

void testFormatTimezone()
{
   // this is about whether it coredumps or not - #5444
   COLdateTime DateTime = COLdateTime::currentTime();

   // a DateTime starts out with an undefined offset
   TST_ASSERT_EQUALS( COLstring(""), DateTime.format("%Z") );

   // a UTC (zero but defined) offset
   DateTime.setOffsetInMinutes(0, true);
   TST_ASSERT_EQUALS( COLstring("+0000"), DateTime.format("%Z") );

   DateTime.setOffsetInMinutes(60*3 /* 3 hours */, true);
   TST_ASSERT_EQUALS( COLstring("+0300"), DateTime.format("%Z") );

   DateTime.setOffsetInMinutes(-60*3 /* -3 hours */, true);
   TST_ASSERT_EQUALS( COLstring("-0300"), DateTime.format("%Z") );

   DateTime.setOffsetInMinutes(60*11+30 /* 3 hours */, true);
   TST_ASSERT_EQUALS( COLstring("+1130"), DateTime.format("%Z") );

   DateTime.setOffsetInMinutes(-(60*11+30) /* 3 hours */, true);
   TST_ASSERT_EQUALS( COLstring("-1130"), DateTime.format("%Z") );

   /*
   // testing this Zone against using strftime() directly
   //// is not being done because the local timezone has nothing to do with anything,
   //// and isn't being recorded as such, unless set specifically in terms of offset minutes
   //// so we don't care about this - see #11476
   //// --Dmitri

   time_t Time = time(0);
   struct tm Tm = *localtime(&Time);
   char ZoneFromStrftime[80]="";
   strftime(ZoneFromStrftime, sizeof(ZoneFromStrftime), "%Z", &Tm);
   */

}

void testFormatInvalidDateTime()
{
   COLdateTime DateTime(1, 1, 1, 0, 0, 0);
   COLstring FormattedString = DateTime.format("%Y %m %d");

   TST_ASSERT_EQUALS("Invalid Date", FormattedString);
}

void testFormatEarliestDate()
{
   // this is to force mktime to return -1
   COLdateTime DateTime((DATE)0);
   COLstring FormattedString = DateTime.format("%Y %m %d");

   TST_ASSERT_EQUALS("1899 12 30", FormattedString);
}

#undef ABSOLUTE_DELTA
#define ABSOLUTE_DELTA(X,Y) (((X-Y) < 0)? (Y-X) : (X-Y))

// Note: this test will fail twice per year during the daylight saving time change hour.
// At least one of testToFromTimeT[123] will be in daylight savings time.
void testToFromTimeT1()
{
   COL_FUNCTION(testToFromTimeT1);

   time_t ApproxTimeT = time(NULL);
   COL_TRC("ApproxTimeT = " << ApproxTimeT << newline);

   COLdateTime DateTimeOrig = COLdateTime::currentTime();
   COL_TRC("DateTimeOrig = " << DateTimeOrig << newline);

   time_t AsTimeT = DateTimeOrig.asTimeT();
   COL_TRC("AsTimeT = " << AsTimeT << newline);
   COL_TRC("absolute delta = " << (ABSOLUTE_DELTA(AsTimeT, ApproxTimeT)) << newline);

   // We shouldn't be off by more than a few seconds, and we shouldn't have moved
   // backwards in time.
   TST_ASSERT(AsTimeT >= ApproxTimeT);
   TST_ASSERT(ABSOLUTE_DELTA(AsTimeT, ApproxTimeT) <= 5);

   COLdateTime DateTimeRebuilt(AsTimeT);
   COL_TRC("DateTimeRebuilt = " << DateTimeRebuilt << newline);
   TST_ASSERT_EQUALS(DateTimeOrig, DateTimeRebuilt);
}

// Note: this test will fail twice per year during the daylight saving time change hour.
// At least one of testToFromTimeT[123] will be in daylight savings time.
void testToFromTimeT2()
{
   COL_FUNCTION(testToFromTimeT2);

   const int sixMonthsOfSeconds = 15724800;
   time_t ApproxTimeT = time(NULL) - sixMonthsOfSeconds;
   COL_TRC("ApproxTimeT = " << ApproxTimeT << newline);

   COLdateTime DateTimeOrig = COLdateTime(ApproxTimeT);
   COL_TRC("DateTimeOrig = " << DateTimeOrig << newline);

   time_t AsTimeT = DateTimeOrig.asTimeT();
   COL_TRC("AsTimeT = " << AsTimeT << newline);
   COL_TRC("absolute delta = " << (ABSOLUTE_DELTA(AsTimeT, ApproxTimeT)) << newline);
  
   // We shouldn't be off by more than a few seconds
   TST_ASSERT(ABSOLUTE_DELTA(AsTimeT, ApproxTimeT) <= 5);

   COLdateTime DateTimeRebuilt(AsTimeT);
   COL_TRC("DateTimeRebuilt = " << DateTimeRebuilt << newline);
   TST_ASSERT_EQUALS(DateTimeOrig, DateTimeRebuilt);
}

// Note: this test will fail twice per year during the daylight saving time change hour.
// At least one of testToFromTimeT[123] will be in daylight savings time.
void testToFromTimeT3()
{
   COL_FUNCTION(testToFromTimeT3);

   const int nineMonthsOfSeconds = 15724800 * 3 / 2;

   time_t ApproxTimeT = time(NULL) - nineMonthsOfSeconds;
   COL_TRC("ApproxTimeT = " << ApproxTimeT << newline);

   COLdateTime DateTimeOrig = COLdateTime(ApproxTimeT);
   COL_TRC("DateTimeOrig = " << DateTimeOrig << newline);

   time_t AsTimeT = DateTimeOrig.asTimeT();
   COL_TRC("AsTimeT = " << AsTimeT << newline);
   COL_TRC("absolute delta = " << (ABSOLUTE_DELTA(AsTimeT, ApproxTimeT)) << newline);
  
   // We shouldn't be off by more than a few seconds
   TST_ASSERT(ABSOLUTE_DELTA(AsTimeT, ApproxTimeT) <= 5);

   COLdateTime DateTimeRebuilt(AsTimeT);
   COL_TRC("DateTimeRebuilt = " << DateTimeRebuilt << newline);
   TST_ASSERT_EQUALS(DateTimeOrig, DateTimeRebuilt);
}

// Helper function used by COLcompileTimeStamp(void).
COLdateTime COLtimeFromString(const COLstring& String);

static void CompareCompileTimeStampBasic(const COLstring& TestString, const COLdateTime& Expected)
{
   COLdateTime Result = COLtimeFromString(TestString);
   COL_TRC("Result:   " << Result   << newline);
   COL_TRC("Expected: " << Expected << newline);
   TST_ASSERT(Expected == Result);
}

static void testCOLcompileTimeStampBasic()
{
   // Verify that the current compile time gets parsed into a valid date and
   // print it.
   COLdateTime CurrentCompileTime = COLcompileTimeStamp();
   TST_ASSERT(CurrentCompileTime.status() == COLdateTime::valid);
   COL_VAR(CurrentCompileTime);

   //now try some conversions
   CompareCompileTimeStampBasic("Jan 01 1000 00:00:00", COLdateTime(1000, 1, 1, 0, 0, 0));
   CompareCompileTimeStampBasic("Jul 14 1999 12:34:56", COLdateTime(1999, 7, 14, 12, 34, 56));
   CompareCompileTimeStampBasic("May 12 2005 04:19:42", COLdateTime(2005, 5, 12, 4, 19, 42));
   CompareCompileTimeStampBasic("Dec 31 8999 23:59:59", COLdateTime(8999, 12, 31, 23, 59, 59));
}

static void CompareCompileTimeStampString(const COLstring& TestString, const COLdateTime& Expected)
{
   COLdateTime Result = COLcompileTimeStamp(TestString);
   COL_TRC("Result:   " << Result   << newline);
   COL_TRC("Expected: " << Expected << newline);
   TST_ASSERT(Expected == Result);
}

static void testCOLcompileTimeStampString()
{
   CompareCompileTimeStampString("10000101000000", COLdateTime(1000, 1, 1, 0, 0, 0));
   CompareCompileTimeStampString("19990714123456", COLdateTime(1999, 7, 14, 12, 34, 56));
   CompareCompileTimeStampString("20050512041942", COLdateTime(2005, 5, 12, 4, 19, 42));
   CompareCompileTimeStampString("89991231235959", COLdateTime(8999, 12, 31, 23, 59, 59));
}

static void testAddMilliseconds(){
   COLtimeStamp time1;
   COLcurrentTimeStamp(&time1);
   int Seconds = time1.tv_sec;
   int Microseconds = time1.tv_usec;
   time1.addMilliseconds(1000);
   TST_ASSERT_EQUALS(Seconds + 1, time1.tv_sec);
   TST_ASSERT_EQUALS(Microseconds, time1.tv_usec);
}

TSTtestable* testCOLdateTime::COLdateTimeTests()
{
   TST_START_FIXTURE(testCOLdateTime)
      TST_ADD_TEST_FUNCTION(testFormatting);
      TST_ADD_TEST_FUNCTION(testFormatTimezone);
      TST_ADD_TEST_FUNCTION(testFormatInvalidDateTime);
      TST_ADD_TEST_FUNCTION(testFormatEarliestDate);
      TST_ADD_TEST_FUNCTION(testToFromTimeT1);
      TST_ADD_TEST_FUNCTION(testToFromTimeT2);
      TST_ADD_TEST_FUNCTION(testToFromTimeT3);
      TST_ADD_TEST_FUNCTION(testCOLcompileTimeStampBasic);
      TST_ADD_TEST_FUNCTION(testCOLcompileTimeStampString);
      TST_ADD_TEST_FUNCTION(testAddMilliseconds);
   TST_END_FIXTURE
}
