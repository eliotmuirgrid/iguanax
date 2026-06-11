// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testServiceLog
//
// Description:
//
// Implementation of tests
//
// Author: John Qi
// Date:   Friday 28 April 2023 - 11:44AM
// ---------------------------------------------------------------------------
#include "testServiceLog.h"

#include <UNIT/UNITapp.h>

#include <SDB/SDBserviceLog.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBfileRead.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBcontentDelete.h>

// #include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLutils.h>
// #include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testServiceErrorLog(){
   COL_FUNCTION(testServiceErrorLog);
   COLthreadPool Pool;
   Pool.start();
   SDBstartServiceLog(&Pool, "");

   COLstring Startup  = "#startup Main iguana service has started.";
   COLstring GitLoad  = "#git Refresh git cache.";
   COLstring Shutdown = "#shutdown Main iguana service has shut down.";
   SDBlogService(Startup);
   SDBlogService(GitLoad);
   SDBlogService(Shutdown);
   COLsleep(200);    // give the Service log writer some time to write the message to disk
   // Message should be available in log file immediately

   COLstring Data;
   SDBfileRead("ServiceErrorLog.txt", &Data);

   // only #startup and #shutdown messages are saved to the ServiceErrorLog.txt
   UNIT_ASSERT(Data.find(Startup)  != npos);
   UNIT_ASSERT(Data.find(Shutdown) != npos);
   UNIT_ASSERT(Data.find(GitLoad)  == npos);

   // clean up
   SDBstopServiceLog();
   FILremove("ServiceErrorLog.txt");
   COLstring RemoveError;
   FILremoveFullDirNewSafe(SDBroot(), &RemoveError);
}

static void testServiceLogSpeed(){
   COL_FUNCTION(testServiceLogSpeed);
   for (int i = 0; i < 1000; ++i) {
      COLstring Msg = "98uYHF9*YHeSGH*(YH(*89y(HY(*HY^$)))).";
      SDBlogService(Msg);
   }
}

void testServiceLog(UNITapp* pApp) {
   // pApp->add("service/error/log", &testServiceErrorLog);
   // Performance test - interferes with service/log, run separately
   // pApp->add("service/log/speed", &testServiceLogSpeed);
}
