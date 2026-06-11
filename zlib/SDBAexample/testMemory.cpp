// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMemory
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testMemory.h"

#include <UNIT/UNITapp.h>

#include <SDBT/SDBTreader.h>

#include <SDBA/SDBAresubmit.h>

#include <SDB/SDBcapi.h>

#include <SCK/SCKloop.h>

#include <CFG/CFGmap.h>

#include <COL/COLweb.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
// #include <COL/COLclosure.h>

#include <BLUA/BLUAmanager.h>
COL_LOG_MODULE;

static void SDBATshutdown(SCKloop* pLoop) {
   COL_FUNCTION(SDBATshutdown);
   pLoop->shutdown();
}

// static void testResubmitLeak() {
//    COL_FUNCTION(testResubmitLeak);
//    COLstring TestDir     = "dir.leak.resubmit.test";
//    COLstring ComponentId = "Source";
//    time_t    StartDate   = time(NULL);
//    SDBTcreateTestLogFiles(StartDate, 1, TestDir, ComponentId, 3);
//    COLthreadPool Pool;
//    Pool.start();
//    SCKloop Loop(&Pool);
//    CFGmap Map;
//    Map.ConfigMap["Source"];
//    Map.ConfigMap["Destination"].CoreConfig.Sources.push_back("Source");
//    Map.ConfigMap["Destination"].CoreConfig.Guid = "Destination";
//    COLaddress Address;
//    Address.ConnectionId = -1;
//    BLUAinstanceManager Manager;
//    SDBITsetIsUnitTest();
//    Loop.addTimer(COLnewClosure0(&SDBATshutdown, &Loop), 200);
//    SDBAresubmitImpl("Destination", "data", "Source", SDBtime(), 1, &Loop, &Map, &Manager, "", Address, TestDir);
//    Loop.start();
// }

void testMemory(UNITapp* pApp){
   COL_FUNCTION(testMemory);
   // pApp->add("resubmit/mem", &testResubmitLeak);
}
