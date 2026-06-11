#ifndef __SDB_STATS_H__
#define __SDB_STATS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBstats
//
// Description:
//
// Stats - messages per second
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLhashmap.h>

#include <time.h>

class SCKloop;

// TODO: keep TPS history
class SDBstats {
public:
   int StatsTimer = 0;
   time_t LastActivity = 0;
   COLhashmap<COLstring, float> InboundMpsMap;
   COLhashmap<COLstring, float> OutboundMpsMap;
   COLhashmap<COLstring, int> MessageInCountMap;
   COLhashmap<COLstring, int> MessageOutCountMap;
};

void SDBstartStats(SDBstats* pStats, SCKloop* pLoop, int PollIntervalMs = 1000);
void SDBstopStats (SDBstats* pStats, SCKloop* pLoop);

// Increments the transaction counter of the specified component.
// should be call each time a message is consumed
// should be call with Inbound = false each time a message is pushed to the component queue
void SDBstatsIncrementCount(SDBstats* pStats, const COLstring& ComponentId, bool Inbound = true);

// Returns transactions per second for the specified component.
// 0 is returned if the component doesn't exist
float SDBstatsMessagesPerSecond(SDBstats* pStats, const COLstring& ComponentId, bool Inbound = true);

#endif // end of defensive include
