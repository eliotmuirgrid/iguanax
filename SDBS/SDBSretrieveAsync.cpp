//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieveAsync.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 1:39p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <GUID/GUIDid.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcontentDelete.h>
#include <SDBS/SDBSprocessLogs.h>
#include <SDBS/SDBSretrieveAsync.h>
#include <SDBS/SDBSretrieveState.h>
#include <SDBU/SDBUlogFile.h>
#include <SDBU/SDBUparams.h>

#include <memory>
COL_LOG_MODULE;

using SDBSretrieveCallback = COLauto<COLclosure3<bool, std::shared_ptr<SDBSretrieveItem>, std::shared_ptr<SDBSretrieveArgs>>>;

static void SDBSretrieveWorker(std::shared_ptr<SDBSretrieveArgs> pArgs, SDBSretrieveCallback pCallback, SCKloop* pLoop);

static void SDBSreportOnMain(SDBSretrieveCallback pCallback, const bool Success, std::shared_ptr<SDBSretrieveItem> pReport,
                             std::shared_ptr<SDBSretrieveArgs> pArgs) {
   COL_FUNCTION(SDBSreportOnMain);
   pCallback->run(Success, pReport, pArgs);
}

static SDBfile SDBSfindFirstLog(const SDBfile& LogFile, const COLstring& ComponentId) {
   COL_FUNCTION(SDBSfindFirstLog);
   SDBfile File;
   File.LogFile1 = SDBfindOldest(SDBdir(ComponentId, SDBroot()));
   File.BothDir  = LogFile.BothDir;
   if(File.BothDir) { File.LogFile2 = SDBfindOldest(SDBdir(ComponentId, SDBroot2())); }
   return File;
}

static void SDBSretrieveReport(const bool Success, std::shared_ptr<SDBSretrieveItem> pReport, std::shared_ptr<SDBSretrieveArgs> pArgs,
                             COLstring JobId, COLhashmap<COLstring, SDBSretrieveState>* pStateMap, SCKloop* pLoop) {
   COL_FUNCTION(SDBSretrieveReport);
   SDBSretrieveState& State = (*pStateMap)[JobId];
   if(State.m_TaskGuid != pReport->m_TaskGuid) {
      COL_TRC("Report guid does not match current task guid. Exiting." );
      return;
   }
   if (!Success){
      COL_TRC("Log file not found or no more log files to go through");
      State.m_EndOfLogs  = true;
      State.m_JobRunning = false;
      return;
   }

   State.m_NextFileTime = pReport->m_NextFile;
   for(const auto& it : pReport->m_Results) {
      if(it["match"].asBool()) {
         State.m_Result.push_back(it);
         pArgs->m_Limit--;
      }
   }

   if(pReport->m_Terminate) {
      COL_TRC(JobId << " completed.");
      State.m_JobRunning = false;
      if(pReport->m_EndOfLogs) { State.m_EndOfLogs = true; }
      if(pReport->m_MaxReached) { State.m_MaxReached = true; }
   } else {
      COL_TRC(pArgs->m_Limit << " remaining for " << JobId);
      SDBSretrieveCallback pReportCallback = COLnewClosure3(&SDBSretrieveReport, JobId, pStateMap, pLoop);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBSretrieveWorker, pArgs, pReportCallback, pLoop));
   }
   COL_VAR(State.m_Result.size());
}

static void SDBSretrieveWorker(std::shared_ptr<SDBSretrieveArgs> pArgs, SDBSretrieveCallback pCallback, SCKloop* pLoop) {
   COL_FUNCTION(SDBSretrieveWorker);
   auto pReport        = std::make_shared<SDBSretrieveItem>();
   pReport->m_TaskGuid = pArgs->m_TaskGuid;

   SDBfile File = pArgs->m_LogFile;
   if(pArgs->m_LogFile.is_null()) {
      File = SDBUfindStartingLogFile(pArgs->m_LogFilter.m_Start);
   } else {
      File = pArgs->m_LogFile;
   }

   if(!pArgs->m_LogFilter.m_RelatedMessage.isNull() && File.is_null()) {
      COL_TRC("Looking for related messages but couldn't find any earlier logs. Lets start from the first log file available");
      File = SDBSfindFirstLog(File, pArgs->m_Component);
   }

   COL_TRC("Starting reading from log file: " << File);
   if(File.is_null()) {
      COL_TRC("Could not find log file for time stamp");
      pLoop->post(COLnewClosure0(&SDBSreportOnMain, pCallback, false, pReport, pArgs));
      return;
   }

   COLlist<SDBcontent*> List;
   COL_TRC("Reading log file: " << File);
   SDBUloadFileToList(File, pArgs->m_LogFilter, &List);
   COL_TRC("List size is: " << List.size());
   bool MaxReached = !SDBSprocessList(List, pArgs->m_Limit, pArgs->m_LogFilter.m_Forward, pArgs->m_Search,
                                      pArgs->m_TagSearch, pArgs->m_LogFilter.m_Exact, pArgs->m_IncludeNonMatches,
                                      pArgs->m_Formatted, pArgs->m_Minify, pArgs->m_Relevant, pReport->m_Results);
   if(MaxReached) {
      COL_TRC("Reached limit");
      pReport->m_Terminate  = true;
      pReport->m_MaxReached = true;
   }

   COLuint64 CurrentFileTime = SDBUlogFileTime(File, pArgs->m_Component, pArgs->m_LogFilter.m_Forward);
   pReport->m_NextFile       = COLuint64ToString(CurrentFileTime);

   pArgs->m_LogFile = SDBUfindNextFileToRead(File, pArgs->m_Component, pArgs->m_LogFilter.m_Forward);
   COL_VAR2(File, pArgs->m_LogFile);
   if(pArgs->m_LogFile.is_null()) {
      COL_TRC("we have run out of files, terminating search");
      pReport->m_Terminate = true;
      if(!MaxReached) { pReport->m_EndOfLogs = true; }
   } else {
      COLuint64 FileTime  = SDBUlogFileTime(pArgs->m_LogFile, pArgs->m_Component, pArgs->m_LogFilter.m_Forward);
      pReport->m_NextFile = COLuint64ToString(FileTime);
      if(!pReport->m_Terminate) {
         COLuint64 EndTime = pArgs->m_LogFilter.m_End.DateTime;
         COL_VAR(FileTime);
         COL_VAR(EndTime);
         if(FileTime < EndTime) {
            pReport->m_Terminate   = true;
            pReport->m_EndOfLogs = true;
         }
      }
   }
   COL_TRC("Done this file - report back to main");
   SDBcontentDelete(&List);
   pLoop->post(COLnewClosure0(&SDBSreportOnMain, pCallback, true, pReport, pArgs));
}

static void SDBSsetupState(SDBSretrieveState* pState, const COLstring& Component, const COLstring& Guid) {
   COL_FUNCTION(SDBSsetupState);
   pState->m_JobRunning   = true;
   pState->m_EndOfLogs    = false;
   pState->m_MaxReached   = false;
   pState->m_NextFileTime = "0";
   pState->m_TaskGuid     = Guid;
   pState->m_Component    = Component;
   pState->m_Result.setArrayType();
}

static void SDBSstopJob(SDBSretrieveState* pState) {
   COL_FUNCTION(SDBSstopJob);
   if(pState->m_JobRunning) {
      COL_TRC("Aborting job in progress. Erasing the results");
      pState->m_JobRunning   = false;
      pState->m_NextFileTime = "";
      pState->m_EndOfLogs    = false;
      pState->m_MaxReached   = false;
      pState->m_Result.reset();
      pState->m_Result.setArrayType();
   }
}

void SDBSretrieveAsync(const SDBUparams& Params, COLhashmap<COLstring, SDBSretrieveState>* pRetrieveStateMap,
                       SCKloop* pLoop) {
   COL_FUNCTION(SDBSretrieveAsync);
   if (pRetrieveStateMap->count(Params.m_Job)){ SDBSstopJob(&(*pRetrieveStateMap)[Params.m_Job]); }
   COL_TRC("Setting up search state");
   const COLstring    TaskGuid = GUIDnewShortId();
   SDBSretrieveState& State    = (*pRetrieveStateMap)[Params.m_Job];
   SDBSsetupState(&State, Params.m_Component, TaskGuid);

   SDBSretrieveCallback pReportCallback = COLnewClosure3(&SDBSretrieveReport, Params.m_Job, pRetrieveStateMap, pLoop);
   auto pArgs = std::make_shared<SDBSretrieveArgs>(TaskGuid, Params.m_Component, Params.m_LogFilter, Params.m_Limit,
                                                   Params.m_Search, Params.m_TagSearch, Params.m_IncludeNonMatches,
                                                   Params.m_Formatted, Params.m_Minify, Params.m_Relevant);

   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBSretrieveWorker, pArgs, pReportCallback, pLoop));
   COL_TRC("Job started - poll for results");
}