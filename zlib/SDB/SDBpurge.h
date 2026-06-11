#ifndef __SDB_PURGE_H__
#define __SDB_PURGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurge
//
// Description:
//
// functions for purging log files
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <CFG/CFGconfig.h>
#include <INFO/INFOusage.h>
#include <SDB/SDBmessageId.h>

class COLstring;
class SDBsystem;
class SCKloop;

#define SDB_PURGE_AGE_DEFAULT 3

class SDBpurgeRule {
public:
   SDBpurgeRule() {}
   SDBpurgeRule(const COLstring& PurgeId, int Days) : Hash(PurgeId), DaysToKeep(Days) {}
   COLstring Hash;
   int DaysToKeep = COL_INT_32_MAX;
};

struct SDBpurgeData {
	COLstring ComponentName;
   SDBmessageId StopId;
   COLarray<COLstring> Tags;
};

class SDBcomponentLogSize{
public:
   COLstring Name;
   COLuint64 Size;
};
class SDBlogStats {
public:
   INFOdisk Disk;
   COLuint64 TotalSize = 0;
   COLhashmap<COLstring, SDBcomponentLogSize> ComponentLogSizes;
};

void SDBstatsLogSpace(SDBsystem* pSystem, COLclosure2<SDBlogStats, SDBlogStats>* pCallback);

void SDBpurgeRulesLoad(COLarray<SDBpurgeRule>* pRuleMap);
void SDBpurgeRulesFromVar(COLarray<SDBpurgeRule>* pRuleMap, const COLvar& Input);
void SDBpurgeRulesToVar(const COLarray<SDBpurgeRule>& RuleMap, COLvar* pOut);

void SDBloadPurgeConfigs(SCKloop* pLoop, COLclosure1<COLarray<SDBpurgeRule>*>* pCallback);

#endif // end of defensive include
