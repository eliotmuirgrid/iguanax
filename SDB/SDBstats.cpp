// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBstats
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------

#include <SDB/SDBstats.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void SDBcalculateStats(SDBstats* pStats, SCKloop* pLoop, int PollIntervalMs) {
   COL_FUNCTION(SDBcalculateStats);
   time_t Now = time(NULL);
   for (auto Component = pStats->MessageInCountMap.begin(); Component != pStats->MessageInCountMap.end(); Component++) {
      float MPS = Component.value() * 1.0 / (Now - pStats->LastActivity);
      COL_VAR2(Component.key(), MPS);
      pStats->InboundMpsMap[Component.key()] = MPS;
      Component.value() = 0;
   }
   for (auto Component = pStats->MessageOutCountMap.begin(); Component != pStats->MessageOutCountMap.end(); Component++) {
      float MPS = Component.value() * 1.0 / (Now - pStats->LastActivity);
      COL_VAR2(Component.key(), MPS);
      pStats->OutboundMpsMap[Component.key()] = MPS;
      Component.value() = 0;
   }
   pStats->LastActivity = Now;
   pStats->StatsTimer = pLoop->addTimer(COLnewClosure0(&SDBcalculateStats, pStats, pLoop, PollIntervalMs), PollIntervalMs, false, "SDBlogStats");
}

void SDBstartStats(SDBstats* pStats, SCKloop* pLoop, int PollIntervalMs) {
   COL_FUNCTION(SDBstartStats);
   SDBcalculateStats(pStats, pLoop, PollIntervalMs);
}

void SDBstopStats(SDBstats* pStats, SCKloop* pLoop) {
   COL_FUNCTION(SDBstopStats);
   if (pStats->StatsTimer) {
      pLoop->cancelTimer(pStats->StatsTimer);
      pStats->StatsTimer = 0;
   }
}

void SDBstatsIncrementCount(SDBstats* pStats, const COLstring& ComponentId, bool Inbound) {
   // COL_FUNCTION(SDBstatsIncrementCount);
   COLhashmap<COLstring, int>* pCountMap = Inbound ? &pStats->MessageInCountMap : &pStats->MessageOutCountMap;
   if (pCountMap->count(ComponentId)) {
      (*pCountMap)[ComponentId]++;
   } else {
      (*pCountMap)[ComponentId] = 1;
   }
}

float SDBstatsMessagesPerSecond(SDBstats* pStats, const COLstring& ComponentId, bool Inbound) {
   // COL_FUNCTION(SDBstatsMessagesPerSecond);
   COLhashmap<COLstring, float>* pCountMap = Inbound ? &pStats->InboundMpsMap : &pStats->OutboundMpsMap;
   if (pCountMap->count(ComponentId) == 0) {
      return 0;
   }
   return (*pCountMap)[ComponentId];
}
