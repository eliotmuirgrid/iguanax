// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSDBepochTime
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir
// Date:   Sunday 02 April 2023 - 09:48AM
// ---------------------------------------------------------------------------
#include "testSDBepochTime.h"

#include <SDB/SDBepochTime.h>
#include <SDB/SDBdateTime.h>

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void showTime(time_t Time, const COLstring& Description){
   COLstring File = SDBepochTimePath(Time);
   COL_VAR2(Description, File);
}

static void testEpochPath(){
   COL_FUNCTION(testEpochPath);
   time_t Now = time(NULL);
   showTime(Now, "Now");
   time_t Year = 365.25*24*60*60;
   // My 'years' are a little off.
   showTime(0,                 "In 1970");
   showTime(24*60*60,          "In 1970 + 1 day");
   showTime(Year,              "In 1971");
   showTime(Year*10,           "In 1980");
   showTime(Year*53,           "In 2023");
   showTime(Now + Year * 10,   "In 2033");
   showTime(Now + Year * 100,  "In 2123");
   showTime(Now + Year * 1000, "In 3023");
}

static void testEpochTime(){
   COL_FUNCTION(testEpochTime);
   time_t Now = time(NULL);
   COLstring Path = SDBepochTimePath(Now);
   UNIT_ASSERT_EQUALS(Now, SDBepochTime(Path));
}

static void testShowDay(){
   COL_FUNCTION(testShowDay);
   time_t Now = time(NULL);
   time_t Hour = 60 * 60;
   showTime(Now + Hour,    "In one hour");
   showTime(Now + Hour*2,  "In two hours");
   showTime(Now + Hour*4,  "In four hour");
   showTime(Now + Hour*24, "In twenty four hours");
}

static void testShowHuman(){
   COL_FUNCTION(testShowHuman);
   time_t Time;
   Time = 0x64200000; showTime(Time, SDBdateTime(Time));
   Time = 0x642F0000; showTime(Time, SDBdateTime(Time));
   Time = 0x64290000; showTime(Time, SDBdateTime(Time));
   Time = 0x642A0000; showTime(Time, SDBdateTime(Time));
   Time = 0xFFFF0000; showTime(Time, SDBdateTime(Time));
}

static void testRolloverRequired(){
   COL_FUNCTION(testRollover);
   COL_TRC("Calculating when we need to roll over is very simple with unix epoch hex encoded paths.");
   UNIT_ASSERT(!SDBepochRollover(0x6422F000, 0x6422D000));
   UNIT_ASSERT( SDBepochRollover(0x64230001, 0x6422D000));
   UNIT_ASSERT( SDBepochRollover(0x64230001, 0x6422DF45));
   UNIT_ASSERT(!SDBepochRollover(0x64230001, 0x64230000));
}

void testSDBepochTime(UNITapp* pApp) {
   pApp->add("epoch/simple", &testEpochPath);
   pApp->add("epoch/time",   &testEpochTime);
   pApp->add("epoch/day",    &testShowDay);
   pApp->add("epoch/human",  &testShowHuman);
   pApp->add("epoch/rollover", &testRolloverRequired);
}
