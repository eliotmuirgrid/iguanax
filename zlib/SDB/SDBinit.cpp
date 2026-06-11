// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBinit
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcache.h>
#include <SDB/SDBflush.h>
#include <SDB/SDBinit.h>
#include <SDB/SDBjournalSwap.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBpurgeAuto.h>
#include <SDB/SDBrecovery.h>
#include <SDB/SDBserviceLog.h>
#include <SDB/SDBsystem.h>

#include <SCK/SCKloop.h>

#include <FIL/FILutils.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void SDBpurgeConfigsLoaded(COLarray<SDBpurgeRule>* pRuleMap, SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBpurgeConfigsLoaded);
   pSystem->LogPurgeRules = *pRuleMap;
   delete pRuleMap;
   // add Iguana-Service to component map for log purging
   pSystem->ComponentMap[SDB_SERVICE_COMPONENT_ID].LastJournaledId.MessageId.ComponentId = SDB_SERVICE_COMPONENT_ID;
   SDBstartLogPurging(pSystem);  // start log purging cycle
}

static void SDBgetPositionMap(SDBsystem* pSystem, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap) {
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& PositionMap = *pPositionMap;
   const auto& ComponentMap = pSystem->ComponentMap;
   for (const auto& Queue : ComponentMap) {
      const auto& Consumers = Queue.second.Consumers;
      for (const auto& Consumer : Consumers) {
         COL_VAR(Consumer.second.Position);
         PositionMap[Queue.first][Consumer.first] = Consumer.second.Position;
      }
   }
}

static COLstring SDBtracePosMap(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, const COLstring& Name) {
   COL_FUNCTION(SDBtracePosMap);
   COLstring Trace;
   COLostream Stream(Trace);
   Stream << Name << " Queue Positions:\n";
   if (pPositionMap->size() == 0) { Stream << "  - No positions found.\n"; }
   for (auto Queue : *pPositionMap) {
      Stream << "  - Queue '" << Queue->first << "'\n";
      for (auto Consumer : Queue->second) {
         const SDBid& Position = Consumer->second;
         Stream << "    * Consumer '" << Consumer->first << "' @ " << Position.MessageId << " SID=" << Position.SequenceId << " QID=" << Position.QueueId << "\n";
      }
   }
   COL_TRC(Trace);
   // COLcout << Trace;
   return Trace;
}

#define PRINT_SDB_ID(ID)   ID.MessageId << " | " << ID.SequenceId << " | " << ID.QueueId

static COLstring SDBtraceComponentMap(SDBsystem* pSystem) {
   COL_FUNCTION(SDBtracePosMap);
   static COLstring Padding(10, ' ');
   COLstring Trace;
   COLostream Stream(Trace);
   Stream << "Component States:\n";
   const auto& ComponentMap = pSystem->ComponentMap;
   for (const auto& Entry : ComponentMap) {
      const auto& ComponentId = Entry.first;
      const auto& Component = Entry->second;
      Stream << "  - Component '" << ComponentId << "'" << Padding.substr(0, 25 - ComponentId.size()) << " LastDataId = [" << PRINT_SDB_ID(Component.LastJournaledId) << "] LastLogId = [" << PRINT_SDB_ID(Component.LastLogId) << "] BOQid = " << Component.BOQqueueId << "\n";
      const auto& Consumers = Component.Consumers;
      for (const auto& Pair : Consumers) {
         const auto& ConsumerId = Pair.first;
         const auto& Position = Pair.second.Position;
         Stream << "      * Consumer '" << ConsumerId << "'" << Padding.substr(0, 25 - ConsumerId.size()) << " @ " << PRINT_SDB_ID(Position) << "\n";
      }
   }
   return Trace;
}

static void SDBjournalFileInitialized(SDBsystem* pSystem, COLclosure0* pDone) {
   SDBcleanUpCache(pSystem);     // start cache cleanup cycle
   SDBstartStats(&pSystem->StatsMap, pSystem->pLoop, 1000);    // start stats poll cycle
   SDBstartFlushCycle(pSystem, pDone);  // start flush cycle, which will run the callback
   COLstring Success = "Log system initialization completed.";
   SDBlogService(Success, "#startup");
   COLcout << Success << newline;
}

static void SDBinitializeJournalFile(COLhashmap<COLstring, COLhashmap<COLstring, SDBid>> PositionMap, SDBsystem* pSystem, COLclosure0* pDone) {
   COLstring LogRootDir = SDBroot();
   SDBsetUpJournalFile(&PositionMap, pSystem->LogEncryptionKey, LogRootDir);
   int Code = FILremove(SDBoldJournalFile(LogRootDir));
   if (Code) {
      SDBlogService("Failed to remove journal.old.log. Code = " + COLintToString(Code), "#warning #startup");
   }
   pSystem->pLoop->post(COLnewClosure0(&SDBjournalFileInitialized, pSystem, pDone));
}

static void SDBcomponentsInitialized(COLhashmap<COLstring, SDBcomponent>* pComponentMap, SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBcomponentsInitialized);
   // TODO
   // pSystem->pComponentMap = pComponentMap;
   for (const auto& Component : *pComponentMap) {
      pSystem->ComponentMap[Component.first] = Component.second;
   }
   delete pComponentMap;
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>> PositionMap;
   SDBgetPositionMap(pSystem, &PositionMap);
   SDBlogService(SDBtracePosMap(&PositionMap, "Log"), "#startup");
   SDBlogService(SDBtraceComponentMap(pSystem), "#startup");
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBinitializeJournalFile, PositionMap, pSystem, pDone));
   SDBlogService("Loading purge rules...", "#startup");
   SDBloadPurgeConfigs(pSystem->pLoop, COLnewClosure1(&SDBpurgeConfigsLoaded, pSystem, pDone));
}

void SDBlogRecovered(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBlogRecovered);
   if (Code) {
      if (SDB_RECOVER_NO_SPACE == Code) {
         COLstring Error = "Unable to initialize the log system due to insufficient disk space.";
         // SDMlogErrorEvent(SDMserviceName(), Error);
         SDBlogService(Error, "#error #startup");
         COLcout << Error << newline;
         COL_ERR("TODO: No way to report serial init error. hard stop iguana.");
         exit(0);
      }
      if (SDB_RECOVER_NO_JOURNAL == Code) {
         SDBlogService("Journal file not found.", "#startup");
      } else {
         SDBlogService("Encountered an error while processing the journal file. Code = " + COLintToString(Code), "#startup");
      }
   }
   SDBlogService(SDBtracePosMap(pPositionMap, "Journal"), "#startup");
   delete pPositionMap;    // can't always trust position map from journal. use logs instead. IX-4136
   COL_TRC("Initializing component queues and consumers");
   SDBinitializeComponents(pSystem->ConfigMap, pSystem->LogEncryptionKey, pSystem->pLoop, COLnewClosure1(&SDBcomponentsInitialized, pSystem, pDone));
}

void SDBstartSystem(SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBstartSystem);
   SDBlogService("Initializing the log system...", "#startup");
   COLcout << "Initializing the log system..." << newline;
   SDBrecover(pSystem->pLoop, &pSystem->ConfigMap, pSystem->LogEncryptionKey, COLnewClosure2(&SDBlogRecovered, pSystem, pDone));
}

void SDBstopSystem(SDBsystem* pSystem, COLclosure0* pDone) {
   COL_FUNCTION(SDBstopSystem);
   SDBlogService("Stopping the log system...", "#shutdown");
   COLcout << "Stopping the log system..." << newline;
   SDBstopFlushCycle(pSystem, pDone);
   SDBstopCacheCleanupCycle(pSystem);
   SDBstopStats(&pSystem->StatsMap, pSystem->pLoop);
   SDBstopLogPurging(pSystem);
}
