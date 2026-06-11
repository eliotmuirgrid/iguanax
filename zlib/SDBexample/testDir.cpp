// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testDir.h"
#include <UNIT/UNITapp.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBepochTime.h>

#include <SDBT/SDBTdir.h>

#include <FIL/FILutils.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testNextEpoch() {
   COL_FUNCTION(testNextEpoch);
   COLstring TestDir = "dir.log.epoch.next.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   time_t Now = time(NULL);

   if (FILfileExists(TestDir)) {
      FILremoveFullDirNewSafe(TestDir, &RemoveError);
   }

   // one file, there is no next log file
   COLstring FileA = SDBlogFile(ComponentId, Now*1000, TestDir);
   COL_VAR(FileA);
   SDBcheckLogDir(FileA);
   FILwriteFile(FileA, "");
   UNIT_ASSERT_EQUALS("", SDBfindNextLogFile(FileA, TestDir));

   // two files in same dir
   COLstring FileB = SDBlogFile(ComponentId, (Now+1)*1000, TestDir);
   COL_VAR(FileB);
   SDBcheckLogDir(FileA);
   FILwriteFile(FileB, "");
   UNIT_ASSERT_EQUALS(FileB, SDBfindNextLogFile(FileA, TestDir));

   // put next file in the sibling of parent dir
   COLstring FileC = SDBlogFile(ComponentId, (Now+1+0xFFFF)*1000, TestDir);
   COL_VAR(FileC);
   SDBcheckLogDir(FileC);
   FILwriteFile(FileC, "");
   UNIT_ASSERT_EQUALS(FileC, SDBfindNextLogFile(FileB, TestDir));

   // put next file in the sibling of grandparent dir
   COLstring FileD = SDBlogFile(ComponentId, (Now+1+0xFFFF+0xFFFFFF)*1000, TestDir);
   COL_VAR(FileD);
   SDBcheckLogDir(FileD);
   FILwriteFile(FileD, "");
   UNIT_ASSERT_EQUALS(FileD, SDBfindNextLogFile(FileC, TestDir));

   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void testThisEpoch() {
   COL_FUNCTION(testThisEpoch);
   COLstring TestDir = "dir.log.epoch.this.test";
   COLstring RemoveError;
   if (FILfileExists(TestDir)) { FILremoveFullDirNewSafe(TestDir, &RemoveError); }
   COLstring ComponentId = "Component_0";
   time_t Now = 0x6447FFFF;
   COLstring FileA = SDBlogFile(ComponentId, Now*1000,          TestDir);
   COLstring FileB = SDBlogFile(ComponentId, (Now+0xFFFFFF)*1000, TestDir);
   COL_VAR2(FileA, FileB);
   SDBcheckLogDir(FileA);
   SDBcheckLogDir(FileB);
   FILwriteFile(FileA, "");
   FILwriteFile(FileB, "");

   for (int i = 0; i < 60; ++i) {
      UNIT_ASSERT_EQUALS(FileA, SDBfindLogFile(ComponentId, (Now+i)*1000, TestDir));
   }
   for (int i = 0; i < 60; ++i) {
      UNIT_ASSERT_EQUALS(FileB, SDBfindLogFile(ComponentId, (Now+0xFFFFFF+i)*1000, TestDir));
   }
   FILremoveFullDirNewSafe(TestDir, &RemoveError);
}

static void writeTestFile(const COLstring& Base, time_t Time){
   COL_FUNCTION(writeTestFile);
   COLstring FilePath = Base + SDBepochTimePath(Time);
   COL_VAR(FilePath);
   FILcreateDirectoryTreeToFile(FilePath, FIL_USER_RWX);
   FILwriteFile(FilePath, "123");
}

static void testFindOldest(){
   COL_FUNCTION(testFindOldest);
   COLstring Base = COL_T("find") + FIL_DIR_SEPARATOR;
   writeTestFile(Base, 10000000);
   writeTestFile(Base, 10004000);
   writeTestFile(Base, 20004000);
   writeTestFile(Base, 10020004000);
   writeTestFile(Base, 10020004010);

   COLstring Old = SDBfindOldest(Base);
   Old.replace(FIL_DIR_SEPARATOR, FIL_POSIX_DIR_SEPARATOR);
   COL_VAR(Old);
   COLstring New = SDBfindNewest(Base);
   New.replace(FIL_DIR_SEPARATOR, FIL_POSIX_DIR_SEPARATOR);
   COL_VAR(New);
   UNIT_ASSERT_EQUALS("find/00000000/00/98/9680.log", Old);
   UNIT_ASSERT_EQUALS("find/00000002/55/3D/20AA.log", New);
   COLstring RemoveError;
   FILremoveFullDirNewSafe("find", &RemoveError);
}

void testDir(UNITapp* pApp){
   COL_FUNCTION(testDir);
   pApp->add("dir/next/epoch", &testNextEpoch);
   pApp->add("dir/this/epoch", &testThisEpoch);
   pApp->add("dir/find/earliest", &testFindOldest);
}
