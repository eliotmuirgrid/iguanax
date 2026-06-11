//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TIMETts
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Tuesday, August 14th, 2012 @ 09:37:21 AM
//
//---------------------------------------------------------------------------
#include "TIMETprecomp.h"
#pragma hdrstop

#include "TIMETts.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TIME/TIMEts.h>
#include <stdio.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const int TIMET_SID=3600*24; // seconds in day

COLostream& operator<<(COLostream& Stream, const TIMEtime& Time)
{
   char Buffer[200]={0};
   snprintf(Buffer, 200, "%04d-%02d-%02d %02d:%02d:%02d", Time.Year, Time.Month, Time.Day, Time.Hour, Time.Minute, Time.Second);
   Stream << Buffer;
   return Stream;
}

void testTsFromTime()
{
   TST_ASSERT_EQUALS((365+31+2)*TIMET_SID+4*3600+5*60+6, TIMEtsFromDateTimeUtc(1971, 2, 3, 4, 5, 6));

   TST_ASSERT_EQUALS(-2051222400-(COLint64)(366+365*4)*TIMET_SID, TIMEtsFromDateTimeUtc(1900, 1, 1, 0, 0, 0));
   TST_ASSERT_EQUALS(-2051222400, TIMEtsFromDateTimeUtc(1905, 1, 1, 0, 0, 0));
   TST_ASSERT_EQUALS(-731*TIMET_SID, TIMEtsFromDateTimeUtc(1968, 1, 1, 0, 0, 0));
   TST_ASSERT_EQUALS(-365*TIMET_SID, TIMEtsFromDateTimeUtc(1969, 1, 1, 0, 0, 0));
   TST_ASSERT_EQUALS(0, TIMEtsFromDateTimeUtc(1970, 1, 1, 0, 0, 0));
   TST_ASSERT_EQUALS(365*TIMET_SID, TIMEtsFromDateTimeUtc(1971, 1, 1, 0, 0, 0));
   TST_ASSERT_EQUALS(730*TIMET_SID, TIMEtsFromDateTimeUtc(1972, 1, 1, 0, 0, 0));

   TST_ASSERT_EQUALS(-(COLint64)7319587545ULL, TIMEtsFromDateTimeUtc(1738, 1, 19, 13, 14, 15));
   TST_ASSERT_EQUALS(-(COLint64)4163913945ULL, TIMEtsFromDateTimeUtc(1838, 1, 19, 13, 14, 15));
   TST_ASSERT_EQUALS(-(COLint64)1008240345ULL, TIMEtsFromDateTimeUtc(1938, 1, 19, 13, 14, 15));
   TST_ASSERT_EQUALS(2147519655ULL, TIMEtsFromDateTimeUtc(2038, 1, 19, 13, 14, 15));
   TST_ASSERT_EQUALS(5303193255ULL, TIMEtsFromDateTimeUtc(2138, 1, 19, 13, 14, 15));
   TST_ASSERT_EQUALS(8458866855ULL, TIMEtsFromDateTimeUtc(2238, 1, 19, 13, 14, 15));
}

void testTimeFromTs()
{
   TST_ASSERT_EQUALS(TIMEtime(1971, 2, 3, 4, 5, 6), TIMEdateTimeUtcFromTs((365+31+2)*TIMET_SID+4*3600+5*60+6));
   TST_ASSERT_EQUALS(TIMEtime(1900, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(-2051222400-(COLint64)(366+365*4)*TIMET_SID));
   TST_ASSERT_EQUALS(TIMEtime(1905, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(-2051222400));
   TST_ASSERT_EQUALS(TIMEtime(1968, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(-731*TIMET_SID));
   TST_ASSERT_EQUALS(TIMEtime(1969, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(-365*TIMET_SID));
   TST_ASSERT_EQUALS(TIMEtime(1969, 1, 1, 0, 0, 1), TIMEdateTimeUtcFromTs(-365*TIMET_SID+1));
   TST_ASSERT_EQUALS(TIMEtime(1970, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(0));
   TST_ASSERT_EQUALS(TIMEtime(1971, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(365*TIMET_SID));
   TST_ASSERT_EQUALS(TIMEtime(1972, 1, 1, 0, 0, 0), TIMEdateTimeUtcFromTs(730*TIMET_SID));
   TST_ASSERT_EQUALS(TIMEtime(2038, 1, 19, 13, 14, 15), TIMEdateTimeUtcFromTs(2147519655UL));
}

TSTtestable* TIMETtsTests()
{
   TST_START_COLLECTION("ts")
      TST_ADD_TEST_FUNCTION(testTsFromTime);
      TST_ADD_TEST_FUNCTION(testTimeFromTs);
   TST_END_FIXTURE
};
