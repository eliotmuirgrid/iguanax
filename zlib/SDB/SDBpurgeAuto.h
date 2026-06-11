#ifndef __SDB_PURGE_AUTO_H__
#define __SDB_PURGE_AUTO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeAuto
//
// Description:
//
// functions for automatically purging component log files according to rules
//
// Author: John Q
// Date:   Tue 15 Oct 2024 16:56:26 EDT
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>

#include <SDB/SDBmessageId.h>
#include <SDB/SDBpurge.h>

class SDBcomponent;
class SDBpurgeData;
class SDBsystem;

bool SDBpurgeIsJournalFile(const COLstring& File);
SDBmessageId SDBdetermineStopId(const SDBcomponent& Component);
void SDBpopulatePurgeInfo(COLhashmap<COLstring, SDBpurgeData>* pPurgeInfo, const SDBsystem* pSystem);
void SDBpopulateAllComponentMap(COLhashmap<COLstring, bool>* pAllComponents, const SDBsystem* pSystem);
SDBpurgeRule SDBgetPurgeLimitFromRules(const SDBpurgeData& PurgeCriteria, const COLarray<SDBpurgeRule>& RuleMap);
void SDBpurgeLogsWorker(SCKloop* pLoop, COLarray<SDBpurgeRule> RuleMap, COLhashmap<COLstring, SDBpurgeData> PurgeInfo, COLhashmap<COLstring, bool> AllComponents, COLclosure0* pDone);

void SDBstartLogPurging(SDBsystem* pSystem);
void SDBstopLogPurging(SDBsystem* pSystem);

#endif // end of defensive include
