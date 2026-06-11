//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testRetrieveSync.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-27, 2:49p.m.
//  ---------------------------------------------------------------------------
#include "testRetrieveSync.h"

#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLutils.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBsystem.h>
#include <SDBS/SDBSretrieveState.h>
#include <SDBS/SDBSretrieveSync.h>
#include <SDBU/SDBUparams.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

const COLstring PAST_TIME       = "1756328181567";  // some random time in the past
const COLstring AFTER_PAST_TIME = "1756328184567";  // shortly after past time

struct syncParameters {
   syncParameters(const SDBUparams& Params, std::shared_ptr<SDBSretrieveItem> Report, const int Expected)
       : m_Params(Params), m_Report(Report), m_Expected(Expected) {}
   SDBUparams                m_Params;
   std::shared_ptr<SDBSretrieveItem> m_Report;
   int                               m_Expected;
};

static COLarray<COLstring> generateTemplates() {
   static COLarray<COLstring> Templates;
   Templates.push_back("Processing patient ID: 12345 with status: ACTIVE");
   Templates.push_back("Order ORD789 completed at timestamp");
   Templates.push_back("Error in module MOD_5: Error code 42");
   Templates.push_back("User user25 performed action: LOGIN");
   Templates.push_back("System health check: SYS_3 - OK");
   return Templates;
}

static COLvar createTestRequestData(const COLstring& StartTime, const COLstring& StartId, const COLstring& EndTime,
                                    const COLstring& EndId, const COLstring& Search, const COLstring& TagSearch,
                                    const bool Forward, const bool Exclusive, const bool Exact,
                                    const bool IncludeNonMatches, const int Limit = 1000) {
   COLvar RequestData;
   RequestData["start"]               = StartTime;
   RequestData["start_id"]            = StartId;
   RequestData["end"]                 = EndTime;
   RequestData["end_id"]              = EndId;
   RequestData["search"]              = Search;
   RequestData["tags"]                = TagSearch;
   RequestData["forward"]             = Forward;
   RequestData["exclusive"]           = Exclusive;
   RequestData["include_non_matches"] = IncludeNonMatches;
   RequestData["exact"]               = Exact;
   RequestData["limit"]               = Limit;
   return RequestData;
}

static void SyncCheckResults(std::shared_ptr<int> pRemaining, std::shared_ptr<SDBSretrieveItem> pReport,
                           const int Expected, SCKloop* pLoop) {
   COL_FUNCTION(SyncCheckResults);
   (*pRemaining)--;
   UNIT_ASSERT_EQUALS(Expected, pReport->m_Results.size())
   if(*pRemaining == 0) {
      COL_TRC("Reached end of sync tests");
      pLoop->shutdown();
   }
}

static void addTestCase(const COLstring& Component, const COLvar& RequestData, const int Expected,
                                              COLarray<syncParameters>* pParameters) {
   const SDBUparams Params(Component, RequestData);
   const auto               pReport   = std::make_shared<SDBSretrieveItem>();
   pParameters->push_back(syncParameters(Params, pReport, Expected));
}

static COLarray<syncParameters> setupSyncTests() {
   COLarray<syncParameters> Cases;
   
   // Test basic retrieval in both directions - should return all 450 logs
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "", "", true, false, false, false), 450, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "", "", false, false, false, false), 450, &Cases);

   // Test exact match parameter -- 90 logs should have error in it (case insensitive)
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "error", "", false, false, true, false), 90, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "error", "", false, false, true, true), 450, &Cases);

   // Test time-based filtering with early end time - should return no logs (end time before any logs)
   addTestCase("Component_1", createTestRequestData("0", "0", "200", "0", "", "", true, false, false, false), 0, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "0", "200", "0", "", "#test", true, false, false, false), 0, &Cases);
   
   // Test time-based filtering with past time boundary - should return only the 50 "#old" logs
   addTestCase("Component_1", createTestRequestData("0", "0", PAST_TIME, "0", "", "", true, false, false, false), 50, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "0", PAST_TIME, "49", "", "", true, false, false, false), 49, &Cases);
   
   // Test backward retrieval with time start and limit constraints
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "0", "0", "0", "", "", false, false, false, false, 25), 25, &Cases);
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "0", "0", "0", "", "", false, false, false, false, 3), 3, &Cases);
   
   // Test time range filtering with exclusive boundaries
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "25", AFTER_PAST_TIME, "0", "", "", true, true, false, false), 25, &Cases);
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "25", AFTER_PAST_TIME, "123", "", "", true, false, false, false), 26, &Cases);
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "25", AFTER_PAST_TIME, "123", "", "", true, true, false, true), 25, &Cases);
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "25", AFTER_PAST_TIME, "0", "", "", false, false, false, false), 0, &Cases);
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "25", AFTER_PAST_TIME, "0", "", "#old", false, false, false, false), 0, &Cases);

   // Test tag-based filtering
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "", "#test", true, false, false, false), 400, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "", "#old", true, false, false, false), 50, &Cases);
   // Test text search combined with tag filtering - should return no results (search term doesn't exist)
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "iDontExist", "#old", true, false, false, false), 0, &Cases);

   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "iDontExist", "#old", true, false, false, false), 0, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "0", "0", "0", "iDontExist", "#old", true, false, false, true), 450, &Cases);
   
   // Forward search with missing start_id
   addTestCase("Component_1", createTestRequestData("0", "", "0", "0", "", "", true, false, false, false), 450, &Cases);
   
   // Forward search with missing end_id - end=infinity
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "0", "0", "", "", "", true, false, false, false), 450, &Cases);

   // Backward search with missing start_id - start=infinity
   addTestCase("Component_1", createTestRequestData("0", "", "0", "0", "", "", false, false, false, false), 450, &Cases);
   
   // Backward search with missing end_id - should search from end backwards to specified start time
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "0", "0", "", "", "", false, false, false, false, 25), 25, &Cases);
   
   // Both start_id and end_id missing with forward search
   addTestCase("Component_1", createTestRequestData("0", "", "0", "", "", "", true, false, false, false), 450, &Cases);
   
   // Both start_id and end_id missing with backward search
   addTestCase("Component_1", createTestRequestData("0", "", "0", "", "", "", false, false, false, false), 450, &Cases);
   
   // Missing start_id with time range - forward search from beginning to specific end time
   addTestCase("Component_1", createTestRequestData("0", "", PAST_TIME, "49", "", "", true, false, false, false), 49, &Cases);
   
   // Missing end_id with time range - backward search from start time to end of logs
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "10", "0", "", "", "", false, false, false, false), 10, &Cases);
   
   // Missing start_id with exclusive bounds - should exclude the boundary at end
   addTestCase("Component_1", createTestRequestData("0", "", PAST_TIME, "30", "", "", true, true, false, false), 30, &Cases);
   
   // Missing end_id with exclusive bounds - should exclude the boundary at start
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "20", "0", "", "", "", false, true, false, false, 30), 19, &Cases);
   
   // Missing IDs with tag filtering - should apply tag filter across entire range
   addTestCase("Component_1", createTestRequestData("0", "", "0", "", "", "#old", true, false, false, false), 50, &Cases);
   addTestCase("Component_1", createTestRequestData("0", "", "0", "", "", "#test", false, false, false, false), 400, &Cases);
   
   // Missing IDs with text search - should search entire log range
   addTestCase("Component_1", createTestRequestData("0", "", "0", "", "error", "", true, false, true, false), 90, &Cases);
   
   // Missing IDs with combined filters - text search + tag filtering across all logs
   addTestCase("Component_1", createTestRequestData("0", "", "0", "", "Processing", "#test", true, false, false, false), 80, &Cases);
   
   // Start time specified but start_id missing, end time also specified but end_id missing
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "", AFTER_PAST_TIME, "", "", "", true, false, false, false), 50, &Cases);
   addTestCase("Component_1", createTestRequestData(PAST_TIME, "", AFTER_PAST_TIME, "", "", "", false, false, false, false), 0, &Cases);

   return Cases;
}

static void runTests(SCKloop* pLoop) {
   COL_FUNCTION(runTests);
   const COLarray<syncParameters> SyncCases = setupSyncTests();

   const auto Remaining = std::make_shared<int>(SyncCases.size());
   for(const auto& it : SyncCases) {
      COLclosure0* pCallback = COLnewClosure0(&SyncCheckResults, Remaining, it.m_Report, it.m_Expected, pLoop);
      SDBSretrieveSyncImpl(it.m_Params, it.m_Report, pCallback);
   }
}

static void generateLogs(const COLarray<COLstring>& Templates, const COLstring& Component, const COLstring& Tags,
                         const int Amount, SDBsystem* pSystem, const COLuint64 Time = 0) {
   COL_FUNCTION(generateLogs);
   for(int i = 0; i < Amount; i++) {
      COLstring  Data     = Templates[i % Templates.size()];
      const auto pMessage = Time ? std::make_shared<SDBmessage>(Component, Data, Time, i + 1)
                                 : std::make_shared<SDBmessage>(Component, Data);
      pMessage->Tags      = Tags;
      SDBlogMessage(pSystem, pMessage);
   }
}

static void testApi(const COLarray<COLstring> Templates, COLlist<COLstring>* pComponentList, SDBsystem* pSystem,
                              SCKloop* pLoop) {
   COL_FUNCTION(testApi);
   static int Count = 0;
   if(Count++ > 4) { return runTests(pLoop); }
   if(Count == 1) {
      // created 50 messages with time=PAST_TIME
      generateLogs(Templates, pComponentList->front(), "#old", 50, pSystem, atoll(PAST_TIME.c_str()));
   } else {
      // after all iterations we have 400 logs at varying times
      generateLogs(Templates, pComponentList->front(), "#test", 100, pSystem);
   }
   pSystem->JournalBuffer.clear();
   COLclosure0* pCallback = COLnewClosure0(&testApi, Templates, pComponentList, pSystem, pLoop);
   SDBwriteLogs(pSystem, pCallback, *pComponentList);
}

static void testSync() {
   COL_FUNCTION(testSync);
   COLstring TestDir = "logt";
   COLstring Error;
   FILremoveFullDirNewSafe(TestDir, &Error);

   COLthreadPool Pool;
   Pool.start();
   SCKloop   Loop(&Pool);
   CFGmap    Map;
   SDBsystem System(&Loop, Map);

   COLlist<COLstring> ComponentList;
   COLstring ComponentId = "Component_1";
   SDBregisterComponent(&System, ComponentId);
   ComponentList.push_back(ComponentId);

   SDBstartLogThread(&System, NULL, "", TestDir);
   COLarray<COLstring> Templates = generateTemplates();
   Loop.addTimer(COLnewClosure0(&testApi, Templates, &ComponentList, &System, &Loop));
   Loop.start();

   SDBstopLogThread(&System, NULL);
   COLsleep(100);  // wait for Log thread to stop completely before task queue get deleted
   FILremoveFullDirNewSafe(TestDir, &Error);
}

void testRetrieveSync(UNITapp* pApp) {
   COL_FUNCTION(testRetrieveSync);
   pApp->add("sync", &testSync);
}