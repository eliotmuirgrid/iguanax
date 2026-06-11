// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeAuto
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue 15 Oct 2024 16:56:26 EDT
// ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <MATCH/MATCHsearch.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBpurgeAuto.h>
#include <SDB/SDBpurgeComponent.h>
#include <SDB/SDBserviceLog.h>
#include <SDB/SDBsystem.h>
#include <time.h>
COL_LOG_MODULE;


SDBmessageId SDBdetermineStopId(const SDBcomponent& Component) {
   COL_FUNCTION(SDBdetermineStopId);
   SDBmessageId StopId;
   const auto& Consumers = Component.Consumers;
   if (Consumers.size()) { StopId = Component.LastJournaledId.MessageId; }
   for (auto Consumer = Consumers.cbegin(); Consumer != Consumers.cend(); Consumer++) {
      if (Consumer.value().Position.MessageId < StopId) {
         StopId = Consumer.value().Position.MessageId;
      }
   }
   return StopId;
}

void SDBpopulatePurgeInfo(COLhashmap<COLstring, SDBpurgeData>* pPurgeInfo, const SDBsystem* pSystem){
   COL_FUNCTION(SDBpopulatePurgeInfo);
   for (auto Queue = pSystem->ComponentMap.begin(); Queue != pSystem->ComponentMap.end(); Queue++) {
      const SDBcomponent& Component = Queue.value();
      if (Component.LastJournaledId.MessageId.isNull()) { continue; } // Skip these - these are invalid components. May the journal file has old stuff lying around? See: IX-2429
      if (Queue.key() == SDB_SERVICE_COMPONENT_ID){
         COL_TRC("Hit iguana service");
         (*pPurgeInfo)[Queue.key()].ComponentName = SDB_SERVICE_COMPONENT_ID;
      } else {
         (*pPurgeInfo)[Queue.key()].Tags = pSystem->ConfigMap.ConfigMap[Queue.key()].CoreConfig.Tags;
         (*pPurgeInfo)[Queue.key()].ComponentName = pSystem->ConfigMap.ConfigMap[Queue.key()].CoreConfig.Name;
      }
      SDBmessageId BeforePosition = SDBdetermineStopId(Component);
      (*pPurgeInfo)[Queue.key()].StopId = BeforePosition;
   }
}

void SDBpopulateAllComponentMap(COLhashmap<COLstring, bool>* pAllComponents, const SDBsystem* pSystem){
   COL_FUNCTION(SDBpopulateAllComponentMap);
   for (auto Queue = pSystem->ComponentMap.begin(); Queue != pSystem->ComponentMap.end(); Queue++) {
      (*pAllComponents)[Queue.key()] = true;
   }
}

SDBpurgeRule SDBgetPurgeLimitFromRules(const SDBpurgeData& PurgeCriteria, const COLarray<SDBpurgeRule>& RuleMap) {
   COL_FUNCTION(SDBgetPurgeLimitFromRules);
   COL_VAR(PurgeCriteria.ComponentName);
   SDBpurgeRule Match;
   for(const auto& Rule : RuleMap) {
      if(Rule.DaysToKeep < Match.DaysToKeep && MATCHsearch(Rule.Hash, PurgeCriteria.ComponentName, true)) {
         COL_TRC("Found rule that matched name with smaller days to keep -- " << Rule.DaysToKeep);
         Match = Rule;
         continue;
      }
      COL_VAR2(Rule.Hash, PurgeCriteria.Tags.size());
      for(const auto& it : PurgeCriteria.Tags) {
         if(Rule.DaysToKeep < Match.DaysToKeep && MATCHsearch(Rule.Hash, it, true)) {
            COL_TRC("Found rule with smaller days to keep -- " << Rule.DaysToKeep);
            Match = Rule;
         }
      }
   }
   return Match;
}

bool SDBpurgeIsJournalFile(const COLstring& File){
   COL_FUNCTION(SDBpurgeIsJournalFile);
   // Don't delete journal.log, journal.old.log, journal.log.corrupted.<epoch_timestamp>, journal.old.log.corrupted.<epoch_timestamp>, etc.
   COLstring Name = FILpathName(File);
   return COLstringContains(Name, "journal") && COLstringContains(Name, "log");
}

static void SDBpurgeOtherDirectory(const COLstring& Dir, const COLstring& Destination){
   COL_FUNCTION(SDBpurgeOtherDirectory);
   try {
      FILmakeFullDir(Destination, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILcopyDirectoryContents(Dir, Destination, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILremoveFullDirNew(Dir);
   } catch(const COLerror& Error) {  // Important to catch so we don't kill the worker thread and crash.
      COL_TRC("Failed to remove directory " + Dir);
      SDBIlogService("Failed to remove directory " + Dir + ". Please remove this directory manually.", "#error");
   }
}

static void SDBpurgeOtherFile(const COLstring& File, const COLstring& Destination){
   COL_FUNCTION(SDBpurgeOtherFile);
   if (SDBpurgeIsJournalFile(File)) { return; }
   try {
      FILcreateDirectoryTreeToFile(Destination, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILcopyFile(File, Destination);
      FILremove(File);
   } catch(const COLerror& Error) {
      COL_TRC("Failed to remove file " + File);
      SDBIlogService("Failed to remove file " + File + ". Please remove this file manually.", "#error");
   }
}

static void SDBpurgeOther(const COLhashmap<COLstring, bool>& AllComponents){
   COL_FUNCTION(SDBpurgeOther);
   COLstring BaseDir = SDBroot() + FIL_DIR_SEPARATOR;
   FILdirEnumerator Enum(BaseDir + "*", false, true);
   COLstring File;
   while(Enum.getFile(File)) {
      COL_VAR(File);
      if (AllComponents.count(File) != 0) {
         COL_TRC("Valid component, skip");
         continue;
      }
      const COLstring Path = FILpathAppend(BaseDir, File);
      if(FILfileExists(Path)) {
         COL_TRC("Found a file/directory that's not in our component map, move to archive");
         COL_VAR2(BaseDir, Path);
         const COLstring Destination = FILpathAppend(DIRarchiveDir(), Path);
         COL_VAR(Destination);
         if (FILisDirectory(Path)) { SDBpurgeOtherDirectory(Path, Destination); }
         else                      { SDBpurgeOtherFile(Path, Destination);      }
      }
   }
   COL_TRC("Done deleting other directories and files");
}

// WORKER
void SDBpurgeLogsWorker(SCKloop* pLoop, COLarray<SDBpurgeRule> RuleMap, COLhashmap<COLstring, SDBpurgeData> PurgeInfo, COLhashmap<COLstring, bool> AllComponents, COLclosure0* pDone) { // exposed as its also called by purge now
   COL_FUNCTION(SDBpurgeLogsWorker);
   for (auto Component = PurgeInfo.begin(); Component != PurgeInfo.end(); Component++) {
      const COLstring& ComponentId = Component.key();
      const SDBpurgeData& PurgeCriteria = Component.value();
      SDBpurgeRule PurgeRule = SDBgetPurgeLimitFromRules(PurgeCriteria, RuleMap);
      COL_VAR2(PurgeCriteria.StopId, PurgeRule.DaysToKeep);
      SDBpurgeComponentLogs(ComponentId, PurgeCriteria.StopId, PurgeRule.DaysToKeep, SDBroot());
      if (!SDBroot2().is_null()) {
         SDBpurgeComponentLogs(ComponentId, PurgeCriteria.StopId, PurgeRule.DaysToKeep, SDBroot2());
      }
   }
   COL_TRC("Also purge other directories, typically old components which are no longer in the component map");
   SDBpurgeOther(AllComponents);
   pLoop->post(pDone);
}

static void SDBpurgeLogsNow(SDBsystem* pSystem) {
   COL_FUNCTION(SDBpurgeLogsNow);
   pSystem->LogPurgerTimer = 0;
   COLhashmap<COLstring, SDBpurgeData> PurgeInfo;
   COLhashmap<COLstring, bool> AllComponents; // keeps track of all components in the system so that we can purge other files/log generator components later.
   SDBpopulatePurgeInfo(&PurgeInfo, pSystem);
   SDBpopulateAllComponentMap(&AllComponents, pSystem);
   COLclosure0* pDone = COLnewClosure0(&SDBstartLogPurging, pSystem);
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(SDBpurgeLogsWorker, pSystem->pLoop, pSystem->LogPurgeRules, PurgeInfo, AllComponents, pDone));
}

static const char* SDBordinalSuffix(int day) {
   switch(day) {
      case 1: case 21: case 31: return "st";
      case 2: case 22: return "nd";
      case 3: case 23: return "rd";
      default: return "th";
   }
}

static COLstring SDBformatPurgeTime(time_t X) {
   COL_FUNCTION(SDBformatPurgeTime);
   struct tm* timeinfo = localtime(&X);
   int day = timeinfo->tm_mday;
   char buffer[80];
   strftime(buffer, sizeof(buffer), "%A %d", timeinfo);
   COLstring Result = buffer;
   Result += SDBordinalSuffix(day);
   strftime(buffer, sizeof(buffer), " %B %H:%M", timeinfo);
   Result += buffer;
   if (day < 10) { Result.remove(Result.find('0'), 1); } // Remove leading zero for single-digit days
   COL_VAR(Result);
   return Result;
}

static time_t SDBsecondsUntilMidnight() {
   COL_FUNCTION(SDBsecondsUntilMidnight);
   time_t Now = time(NULL);
   struct tm *tm_now = localtime(&Now);
   // Build a struct tm for *tomorrow at 00:00:00*
   struct tm tm_midnight = *tm_now;
   tm_midnight.tm_hour = 0;
   tm_midnight.tm_min = 0;
   tm_midnight.tm_sec = 0;
   tm_midnight.tm_mday += 1;
   time_t Midnight = mktime(&tm_midnight);
   return difftime(Midnight, Now);
}

void SDBstartLogPurging(SDBsystem* pSystem) {
   COL_FUNCTION(SDBstartLogPurging);
   int PurgeTimeSchedule = SDBsecondsUntilMidnight();
   const char* pValue = ::getenv("SDB_PURGE_INTERVAL");
   if (pValue) {
      int Interval = ::atoi(pValue);
      if (Interval > 0) { PurgeTimeSchedule = Interval; }
   }
   // Should we do a purge on startup?
   time_t Then = time(NULL) + PurgeTimeSchedule;
   SDBIlogService("Next log purge set up for " +SDBformatPurgeTime(Then), "#purge");
   pSystem->LogPurgerTimer = pSystem->pLoop->addTimer(COLnewClosure0(&SDBpurgeLogsNow, pSystem), PurgeTimeSchedule*1000, false, "Automatic purge timer");
}

void SDBstopLogPurging(SDBsystem* pSystem) {
   COL_FUNCTION(SDBstopLogPurging);
   if (pSystem->LogPurgerTimer) {
      pSystem->pLoop->cancelTimer(pSystem->LogPurgerTimer);
      pSystem->LogPurgerTimer = 0;
   }
}
