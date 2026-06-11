// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testPurge
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 14 March 2023 - 03:14PM
// ---------------------------------------------------------------------------
#include "testPurge.h"

#include <time.h>

#include <UNIT/UNITapp.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBpurgeComponent.h>
#include <SDB/SDBmessageId.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void testPurgeLimit(){
   COL_FUNCTION(testPurgeLimit);
   SDBmessageId StopId;
   time_t Now = time(NULL);

   // TEST - when StopId is null, it should return Now - DaysToKeep in milliseconds
   UNIT_ASSERT_EQUALS((Now - 0 * SDB_DAY) * 1000, SDBgetPurgeLimit(StopId, 0));
   UNIT_ASSERT_EQUALS((Now - 5 * SDB_DAY) * 1000, SDBgetPurgeLimit(StopId, 5));

   StopId.ComponentId = "Component_0";
   StopId.DateTime  = (Now - 3 * SDB_DAY) * 1000;
   // TEST - when StopId.DateTime > Now - DaysToKeep, return StopId.DateTime
   UNIT_ASSERT_EQUALS((Now - 3 * SDB_DAY) * 1000, SDBgetPurgeLimit(StopId, 0));
   // TEST - when StopId.DateTime < Now - DaysToKeep, return Now - DaysToKeep
   UNIT_ASSERT_EQUALS((Now - 5 * SDB_DAY) * 1000, SDBgetPurgeLimit(StopId, 5));
}

static COLstring SDBTtouchLogFile(const COLstring& ComponentId, time_t EpochTime, const COLstring& LogDir) {
   COL_FUNCTION(SDBTtouchLogFile);
   COLstring FilePath = SDBlogFile(ComponentId, EpochTime * 1000, LogDir);
   FILmakeFullDir(FILparentDir(FilePath), FIL_USER_RWX);
   FILwriteFile(FilePath, "");
   return FilePath;
}

static void testPurgeLimitFile(){
   COL_FUNCTION(testPurgeLimitFile);
   COLstring TestDir     = "dir.limit.file.purge.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   time_t    Today       = time(NULL);
   time_t    Day2        = Today - SDB_DAY;
   time_t    Day1        = Today - SDB_DAY * 2;
   SDBmessageId StopId;

   // Component doesn't have any log files in TestDir
   // Test - return "" when component has no logs
   UNIT_ASSERT_EQUALS("", SDBgetPurgeLimitFile(ComponentId, StopId, 0, TestDir));

   COLstring File2 = SDBTtouchLogFile(ComponentId, Day2, TestDir);
   // Component only has one log file that contains messages between now - 24hr and now
   StopId.ComponentId = "Component_0";
   StopId.DateTime = (Today - 12 * 3600) * 1000;
   // StopId.DateTime set to 12 hr ago

   // Test - purge everything -> delete files older than the one that contains StopId (12hr),
   // which is File2, so File2 should be returned
   UNIT_ASSERT_EQUALS(File2, SDBgetPurgeLimitFile(ComponentId, StopId, 0, TestDir));
   // Test - keep 1 day -> no log files older than 1 day, should return File2
   UNIT_ASSERT_EQUALS(File2, SDBgetPurgeLimitFile(ComponentId, StopId, 1, TestDir));
   // Test - keep 3 day -> no log files older than 1 day, should return File2
   UNIT_ASSERT_EQUALS(File2, SDBgetPurgeLimitFile(ComponentId, StopId, 3, TestDir));

   COLstring File1 = SDBTtouchLogFile(ComponentId, Day1, TestDir);
   // add a 2nd log file that contain message between now - 48hr and now - 24hr
   StopId.DateTime = (Today - 36 * 3600) * 1000;
   // StopId.DateTime set to 36 hr ago

   // Test - purge everything -> delete files older than the one that contains StopId (36hr),
   // which is File1, so File1 should be returned
   UNIT_ASSERT_EQUALS(File1, SDBgetPurgeLimitFile(ComponentId, StopId, 0, TestDir));
   // Test - keep 1 day (File2) -> but StopId is File1, so should return File1
   UNIT_ASSERT_EQUALS(File1, SDBgetPurgeLimitFile(ComponentId, StopId, 1, TestDir));
   // Test - keep 3 day -> no log files older than 2 days, should return File1
   UNIT_ASSERT_EQUALS(File1, SDBgetPurgeLimitFile(ComponentId, StopId, 3, TestDir));

   StopId.DateTime = (Today - 12 * 3600) * 1000;
   // StopId.DateTime set back to 12 hr ago

   // Test - keep 1 day (File2) -> StopId also in File2, should return File2
   // this means File1 will be deleted by the purge logic
   UNIT_ASSERT_EQUALS(File2, SDBgetPurgeLimitFile(ComponentId, StopId, 1, TestDir));
   // Test - keep 3 day -> no log files older than 2 days, should return File1
   UNIT_ASSERT_EQUALS(File1, SDBgetPurgeLimitFile(ComponentId, StopId, 3, TestDir));

   // Clean up
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

void testPurge(UNITapp* pApp){
   COL_FUNCTION(testPurge);
   pApp->add("purge/limit",      &testPurgeLimit);
   pApp->add("purge/limit/file", &testPurgeLimitFile);
}
