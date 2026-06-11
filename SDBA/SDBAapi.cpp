// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 05 May 2023 - 03:46PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SDBA/SDBAapi.h>
#include <SDBA/SDBAdownload.h>
#include <SDBA/SDBAlogClear.h>
#include <SDBA/SDBAlogComponentUsage.h>
#include <SDBA/SDBAlogDetail.h>
#include <SDBA/SDBAlogDirectory.h>
#include <SDBA/SDBAlogExportSample.h>
#include <SDBA/SDBAlogQueueConsumers.h>
#include <SDBA/SDBAlogQueuePosition.h>
#include <SDBA/SDBAlogQueueSources.h>
#include <SDBA/SDBAoldest.h>
#include <SDBA/SDBApurgeNow.h>
#include <SDBA/SDBApurgeRules.h>
#include <SDBA/SDBApurgeTest.h>
#include <SDBA/SDBApurgeTestOther.h>
#include <SDBA/SDBArelatedUpstream.h>
#include <SDBA/SDBAresubmit.h>
#include <SDBA/SDBAresubmitCustom.h>
COL_LOG_MODULE;

void SDBAblua(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap,
              BLUAinstanceManager* pBluaManager, SDBsystem* pSystem) {
	COL_FUNCTION(SDBAblua);
   ApiMap.add("/log/queue/position", COLnewClosure1(&SDBAlogQueuePosition, pMap, pBluaManager, pSystem));

   ApiMap.add("/logs/resubmit",        COLnewClosure1(&SDBAresubmitStart,  pMap, pBluaManager, pLoop));
   ApiMap.add("/logs/resubmit/results",COLnewClosure1(&SDBAresubmitPoll));
   ApiMap.add("/logs/resubmit/stop",   COLnewClosure1(&SDBAresubmitStop));
   ApiMap.add("/logs/resubmit/custom", COLnewClosure1(&SDBAresubmitCustom, pMap, pBluaManager, pLoop));
}

void SDBAlog(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SDBsystem* pSystem, SCKloop* pLoop) {
   COL_FUNCTION(SDBAlog);
   ApiMap.add("/log/purge_rules",            COLnewClosure1(&SDBApurgeRulesGet, pSystem));
   ApiMap.add("/log/purge_rules/alt_dir",    COLnewClosure1(&SDBApurgeRulesAltDir));
   ApiMap.add("/log/purge_rules/set",        COLnewClosure1(&SDBApurgeRulesSet, pSystem, pLoop));
   ApiMap.add("/log/purge_rules/test",       COLnewClosure1(&SDBApurgeTest,     pSystem));
   ApiMap.add("/log/purge_rules/test/other", COLnewClosure1(&SDBApurgeTestOther,pSystem));
   ApiMap.add("/log/purge_rules/purge", COLnewClosure1(&SDBApurgeNow,     pSystem));
   ApiMap.add("/log/component/clear",   COLnewClosure1(&SDBAlogClear,      pSystem));
}

void SDBAworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(SDBAworker);
   ApiMap.add("/log/queue/oldest",    &SDBAoldest);
   ApiMap.add("/log/queue/consumers", &SDBAlogQueueConsumers);
   ApiMap.add("/log/queue/sources",   &SDBAlogQueueSources);
   ApiMap.add("/related/upstream",    &SDBArelatedUpstream);
   ApiMap.add("/log/detail",          &SDBAlogDetail);
   ApiMap.add("/log/download",        &SDBAdownload);
   ApiMap.add("/log/export/sample",   &SDBAlogExportSample);
   ApiMap.add("/log/directory",       &SDBAlogDirectory);
   ApiMap.add("/log/component/usage", &SDBAlogComponentUsage);
}