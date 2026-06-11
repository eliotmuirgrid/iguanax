// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeTest
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 28 September 2023 - 01:32PM
// ---------------------------------------------------------------------------

#include <SDB/SDBpurgeTest.h>
#include <SDB/SDBpurgeAuto.h>
#include <SDB/SDBpurgeComponent.h>
#include <SDB/SDBsystem.h>
#include <SDB/SDBcapi.h>

#include <SCK/SCKloop.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// This function is basically SDBpurgeLogFiles but with the delete calls swapped for FILfileSize
static COLuint64 SDBcalculatePurgeSize(const COLstring& ComponentId, COLuint64 PurgeLimitTime, const COLstring& LogDir = SDBroot()) {
   COL_FUNCTION(SDBcalculatePurgeSize);
   COL_VAR2(ComponentId, PurgeLimitTime);
   COLuint64 TotalPurgeSize = 0;
   COLstring KeepFile = SDBfindLogFile(ComponentId, PurgeLimitTime, LogDir);
   if (KeepFile.is_null()) {
      COL_TRC("No file found for component " << ComponentId << " within PurgeLimitTime. Skipping.");
      return TotalPurgeSize;
   }
   COL_TRC("Calculating size of log files before " << KeepFile);
   COLstring CurrentFile = SDBfindOldest(SDBdir(ComponentId, LogDir));
   COL_VAR2(KeepFile, CurrentFile);
   while (!CurrentFile.is_null() && CurrentFile != KeepFile) {
      TotalPurgeSize += FILfileSize(CurrentFile);
      CurrentFile = SDBfindNextLogFile(CurrentFile, ComponentId);
   }
   return TotalPurgeSize;
}

static COLstring SDBpurgeAfterRuleString(int DaysToKeep){
   COL_FUNCTION(SDBpurgeAfterRuleString);
   COLstring Out = "Purge after " + COLintToString(DaysToKeep) + " day";
	return DaysToKeep == 1 ? Out : Out + "s";
}

static void SDBpurgeTestPopulateReturnVar(const COLstring& Name, SDBpurgeRule Rule, COLuint64 AmountFreed, COLuint64 AltAmountFreed, COLvar* pOut){
   COL_FUNCTION(SDBpurgeTestPopulateReturnVar);
   COLvar& Out = *pOut;
   Out["name"]         = Name;
   Out["would_purge"]  = (AmountFreed > 0) ? "Yes" : "No";
   Out["amount_freed"] = COLuint64ToString(AmountFreed);
   if (AltAmountFreed) {
      Out["alt_amount_freed"] = COLuint64ToString(AltAmountFreed);
   }
	Out["rule"]         = SDBpurgeAfterRuleString(Rule.DaysToKeep);
}

static void SDBshouldPurgeComponentLog(const COLstring& ComponentId, const SDBpurgeData& PurgeData, const COLarray<SDBpurgeRule>& RuleMap, COLvar* pResult){
   COL_FUNCTION(SDBshouldPurgeComponentLog);
	COLuint64 SizeOfLogsPurged = 0;
	COLuint64 SizeOfLogsPurged2 = 0;
   SDBpurgeRule Rule = SDBgetPurgeLimitFromRules(PurgeData, RuleMap);
	if(Rule.DaysToKeep != COL_INT_32_MAX) {
		SizeOfLogsPurged = SDBcalculatePurgeSize(ComponentId, SDBgetPurgeLimit(PurgeData.StopId, Rule.DaysToKeep), SDBroot());
      if (!SDBroot2().is_null()) {
         SizeOfLogsPurged2 = SDBcalculatePurgeSize(ComponentId, SDBgetPurgeLimit(PurgeData.StopId, Rule.DaysToKeep), SDBroot2());
      }
	}
	if(SizeOfLogsPurged > 0) {
		COLvar PurgeTestComponentInfo;
		// Have to determine whether we have anything old enough to purge, as well as total size of what would be purged
		COL_VAR4(ComponentId, PurgeData.StopId, Rule.DaysToKeep, SizeOfLogsPurged);
		SDBpurgeTestPopulateReturnVar(SDBInameFromGuid(ComponentId), Rule, SizeOfLogsPurged, SizeOfLogsPurged2, &PurgeTestComponentInfo);
		(*pResult)[ComponentId] = PurgeTestComponentInfo;
	}
}

static void SDBpurgeTestOtherWorker(COLhashmap<COLstring, bool> AllComponents, COLclosure1<COLvar>* pCallback){
   COL_FUNCTION(SDBpurgeTestOtherWorker);
   COLvar Result;
   Result.setArrayType();
   COLstring BaseDir = SDBroot() + FIL_DIR_SEPARATOR;
   FILdirEnumerator Enum(BaseDir + "*", false, true);
   COLstring File;
   while(Enum.getFile(File)) {
      if (AllComponents.count(File) != 0) {
         COL_TRC("Valid component, skip");
         continue;
      }
      const COLstring Path = FILpathAppend(BaseDir, File);
      if(FILfileExists(Path) && !SDBpurgeIsJournalFile(Path)) {
         Result.push_back(File);
      }
   }
   pCallback->runAndDelete(Result);
}

static void SDBpurgeTestWorker(COLhashmap<COLstring, SDBpurgeData> PurgeInfo, COLarray<SDBpurgeRule> RuleMap, COLclosure1<COLvar>* pCallback){
   COL_FUNCTION(SDBpurgeTestWorker);
   COLvar Result;
   for (auto Component = PurgeInfo.begin(); Component != PurgeInfo.end(); Component++) {
      SDBshouldPurgeComponentLog(Component.key(), Component.value(), RuleMap, &Result);
   }
   pCallback->runAndDelete(Result);
}

void SDBpurgeTest(SDBsystem* pSystem, COLclosure1<COLvar>* pCallback){
   COL_FUNCTION(SDBpurgeTest);
   COLhashmap<COLstring, SDBpurgeData> PurgeInfo;
   SDBpopulatePurgeInfo(&PurgeInfo, pSystem);
   // Calculating log purge size is going to require file io, so pass to worker thread
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBpurgeTestWorker, PurgeInfo, pSystem->LogPurgeRules, pCallback));
}

void SDBpurgeTestOther(SDBsystem* pSystem, COLclosure1<COLvar>* pCallback){
   COL_FUNCTION(SDBpurgeTestOther);
   COLhashmap<COLstring, bool> AllComponents;
   SDBpopulateAllComponentMap(&AllComponents, pSystem);
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBpurgeTestOtherWorker, AllComponents, pCallback));
}
