// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurge
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBsize.h>
#include <SDB/SDBsystem.h>
COL_LOG_MODULE;

// WORKER
static void SDBupdateLogStats(SCKloop* pLoop, COLarray<COLstring> ComponentList, COLclosure2<SDBlogStats, SDBlogStats>* pDone) {
   COL_FUNCTION(SDBupdateLogStats);
   SDBlogStats Stats;
   Stats.Disk = INFOdiskStats();
   for (int i = 0; i < ComponentList.size(); ++i) {
      const COLstring& ComponentId = ComponentList[i];
      COLstring ComponentName;
      if (ComponentId == SDB_SERVICE_COMPONENT_ID) { ComponentName = SDB_SERVICE_COMPONENT_ID; }
      else                                 { ComponentName = SDBInameFromGuid(ComponentId); }
      Stats.ComponentLogSizes[ComponentId].Name = ComponentName;
      Stats.ComponentLogSizes[ComponentId].Size = SDBsize(SDBdir(ComponentId, SDBroot()));
      Stats.TotalSize += Stats.ComponentLogSizes[ComponentId].Size;
   }
   SDBlogStats Stats2;
   if (!SDBroot2().is_null()) {
      Stats2.Disk = INFOdiskStats();
      for (int i = 0; i < ComponentList.size(); ++i) {
         const COLstring& ComponentId = ComponentList[i];
         COLstring ComponentName;
         if (ComponentId == SDB_SERVICE_COMPONENT_ID) { ComponentName = SDB_SERVICE_COMPONENT_ID; }
         else                                 { ComponentName = SDBInameFromGuid(ComponentId); }
         Stats2.ComponentLogSizes[ComponentId].Name = ComponentName;
         Stats2.ComponentLogSizes[ComponentId].Size = SDBsize(SDBdir(ComponentId, SDBroot2()));
         Stats2.TotalSize += Stats2.ComponentLogSizes[ComponentId].Size;
      }
   }
   pLoop->post(COLnewClosure0(pDone, &COLclosure2<SDBlogStats, SDBlogStats>::runAndDelete, Stats, Stats2));
}

void SDBstatsLogSpace(SDBsystem* pSystem, COLclosure2<SDBlogStats, SDBlogStats>* pCallback) {
   COL_FUNCTION(SDBstatsLogSpace);
   COLarray<COLstring> ComponentList;
   for (auto Component = pSystem->ComponentMap.begin(); Component != pSystem->ComponentMap.end(); Component++) {
      ComponentList.push_back(Component.key());
   }
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBupdateLogStats, pSystem->pLoop, ComponentList, pCallback));
}

static void SDBinitDefaultPurgeRules(COLarray<SDBpurgeRule>* pRuleMap) {
   COL_FUNCTION(SDBinitDefaultPurgeRules);
   COLarray<SDBpurgeRule>& RuleMap = *pRuleMap;
   pRuleMap->push_back(SDBpurgeRule("*", SDB_PURGE_AGE_DEFAULT));
}

static bool SDBpurgeRuleFromVar(SDBpurgeRule* pRule, const COLvar& Input) {
   COL_FUNCTION(SDBpurgeRuleFromVar);
   if (!Input.exists("hash") || Input["hash"].asString().isWhitespace()) { return false; }
   const COLstring& PurgeId = Input["hash"];
   // if ('#' != PurgeId[0]) { return false; }
   if (!Input.exists("days_to_keep")) { return false; }
   int Keep = atoi(Input["days_to_keep"].asString().c_str());
   if (Keep < 1) { return false; }
   (*pRule).Hash       = PurgeId;
   (*pRule).DaysToKeep = Keep;
   COL_TRC("Found purge rule : " << PurgeId << " -> " << Keep << " days.");
   return true;
}

static void SDBpurgeRuleToVar(const SDBpurgeRule& Rule, COLvar* pOut) {
   COL_FUNCTION(SDBpurgeRuleToVar);
   COLvar Out;
   Out["hash"]         = Rule.Hash;
   Out["days_to_keep"] = Rule.DaysToKeep;
   pOut->push_back(Out);
}

void SDBpurgeRulesFromVarOld(COLarray<SDBpurgeRule>* pRuleMap, const COLvar& Input) {
   COL_FUNCTION(SDBpurgeRulesFromVar);
   COLarray<SDBpurgeRule>& RuleMap = *pRuleMap;
   if(Input.isNull() || !Input.isMap()) { return; }
   for (auto R = Input.map().cbegin(); R != Input.map().cend(); R++) {
      SDBpurgeRule Rule;
      if (SDBpurgeRuleFromVar(&Rule, R.value())) {
         RuleMap.push_back(Rule);
      }
   }
}

void SDBpurgeRulesFromVar(COLarray<SDBpurgeRule>* pRuleMap, const COLvar& Input) {
   COL_FUNCTION(SDBpurgeRulesFromVar);
   if(Input.isNull()) { return; }
   COLarray<SDBpurgeRule>& RuleMap = *pRuleMap;
   if (!Input.isArray()){ return SDBpurgeRulesFromVarOld(pRuleMap, Input); }
   for (auto R = Input.array().begin(); R != Input.array().end(); R++) {
      SDBpurgeRule Rule;
      if (SDBpurgeRuleFromVar(&Rule, *R)) {
         RuleMap.push_back(Rule);
      }
   }
}

void SDBpurgeRulesToVar(const COLarray<SDBpurgeRule>& RuleMap, COLvar* pOut) {
   COL_FUNCTION(SDBpurgeRulesToVar);
   for (auto Rule = RuleMap.begin(); Rule != RuleMap.end(); Rule++) {
      SDBpurgeRuleToVar(*Rule, pOut);
   }
}

void SDBpurgeRulesLoad(COLarray<SDBpurgeRule>* pRuleMap){
   COL_FUNCTION(SDBpurgeRulesLoad);
   const COLstring LoggingFile = DIRfileConfigLogging();
   if(!FILfileExists(LoggingFile)) {
      SDBinitDefaultPurgeRules(pRuleMap);
      return;
   }
   COLvar PurgeRuleMapVar;
   if (FILserializeLoadConfig(&PurgeRuleMapVar, LoggingFile)) {
      SDBpurgeRulesFromVar(pRuleMap, PurgeRuleMapVar);
      if (pRuleMap->size()) {
         COL_TRC("Successfully loaded purge rules");
         return;
      }
   }
   COL_TRC("Failed to load purge rules.");
}


// Worker
void SDBreadPurgeConfigFile(SCKloop* pLoop, COLclosure1<COLarray<SDBpurgeRule>*>* pDone) {
   COL_FUNCTION(SDBreadPurgeConfigFile);
   COL_TRC("Loading log purge rules on a worker thread");
   COLarray<SDBpurgeRule>* pRuleMap = new COLarray<SDBpurgeRule>();
   SDBpurgeRulesLoad(pRuleMap);
   pLoop->post(COLnewClosure0(pDone, &COLclosure1<COLarray<SDBpurgeRule>*>::runAndDelete, pRuleMap));
}

void SDBloadPurgeConfigs(SCKloop* pLoop, COLclosure1<COLarray<SDBpurgeRule>*>* pCallback) {
   COL_FUNCTION(SDBloadPurgeConfigs);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(SDBreadPurgeConfigFile, pLoop, pCallback));
}
