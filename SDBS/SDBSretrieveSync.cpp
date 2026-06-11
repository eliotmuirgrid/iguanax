//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieveSync.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-21, 10:57a.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter.h>
#include <SDBS/SDBSprocessLogs.h>
#include <SDBS/SDBSretrieveState.h>
#include <SDBS/SDBSretrieveSync.h>
#include <SDBU/SDBUlogFile.h>
#include <SDBU/SDBUparams.h>

#include <memory>
COL_LOG_MODULE;

static void SDBSretrievePostToMain(std::shared_ptr<SDBSretrieveItem> pReport, const COLaddress Address) {
   COL_FUNCTION(SDBSretrievePostToMain);
   COLvar Result;
   Result.setArrayType();
   for(const auto& it : pReport->m_Results) { Result.push_back(it); }
   COL_VAR(Result.json(1));
   COLrespondSuccess(Address, Result);
}

static bool SDBSreadNextLogFile(SDBfile& LogFile, bool& LastFile, const COLstring& ComponentId,
                                const bool Forward, const COLuint64 StartTime, const COLuint64 EndTime) {
   COL_FUNCTION(SDBSreadNextLogFile);
   COL_VAR(Forward);
   LogFile = SDBUfindNextFileToRead(LogFile, ComponentId, Forward);
   if(LogFile.is_null()) { return false; }
   const COLuint64 FileTime = SDBUlogFileTime(LogFile, ComponentId, Forward);
   COL_VAR3(FileTime, EndTime, StartTime);
   if(!Forward && FileTime < EndTime) {
      if(LastFile) {
         COL_TRC("Reached end time");
         return false;
      }
      COL_TRC("one more file to check");
      // Edge case: need to fetch logs between t=100 and t=200. log file 50 contains logs
      // between t=50 and t=134. log file 135 contains logs between t=135 and t=348
      // The logic first fetches logs between t=135 and t=200 from file 135, and
      // "!DirectionNext && FileTime < EndTime" evaluates to true. We need to fetch logs
      // between t=100 and t=135 from file 50. IX-3780
      LastFile = true;
   }
   if(Forward && FileTime > EndTime) {
      COL_TRC("Reached start time");
      return false;
   }
   return true;
}

void SDBSretrieveSyncImpl(SDBUparams Params, std::shared_ptr<SDBSretrieveItem> pReport,
                          COLclosure0* pCallback) {
   COL_FUNCTION(SDBSretrieveSyncImpl);
   if(!pCallback) {
      COL_TRC("Callback is null -- should never happen?");
      return;
   }

   SDBfile File = SDBUfindStartingLogFile(Params.m_LogFilter.m_Start);
   if (Params.m_LogFilter.m_Forward && File.is_null()){
      COL_TRC("Doing a forward search and couldn't find a log file. Start time is probably earlier than the oldest log file. Start with the oldest logs available.");
      File = SDBUfindOldestLogFile(Params.m_Component);
   }

   COL_VAR(File);
   if(File.is_null()) {
      COL_TRC("Could not find log file for time stamp");
      return pCallback->runAndDelete();
   }
   COLlist<SDBcontent*> List;
   bool                 LastFile = false;
   while(Params.m_Limit > 0 && !File.is_null() && !SDBSnonCriticalJobTimedOut(Params)) {
      const size_t SizeBefore = pReport->m_Results.size();
      SDBUloadFileToList(File, Params.m_LogFilter, &List);
      COL_VAR4(SizeBefore, List.size(), Params.m_Limit, File);

      const size_t RemainingLimit = Params.m_Limit;
      if(!SDBSprocessList(List, RemainingLimit, Params.m_LogFilter.m_Forward, Params.m_Search, Params.m_TagSearch,
                          Params.m_LogFilter.m_Exact, Params.m_IncludeNonMatches, Params.m_Formatted, Params.m_Minify,
                          Params.m_Relevant, pReport->m_Results)) {
         COL_TRC("Reached max messages");
         break;
      }
      if(!SDBSreadNextLogFile(File, LastFile, Params.m_Component, Params.m_LogFilter.m_Forward,
                              Params.m_LogFilter.m_Start.DateTime, Params.m_LogFilter.m_End.DateTime)) {
         break;
      }
      COL_TRC("Opening log file next: " << File);
      if(pReport->m_Results.size() >= Params.m_Limit) { break; }
   }
   SDBcontentDelete(&List);
   pCallback->runAndDelete();
}

void SDBSretrieveSync(const SDBUparams& Params, SCKloop* pLoop, const COLaddress Address) {
   COL_FUNCTION(SDBSretrieveSync);
   const auto   pReport   = std::make_shared<SDBSretrieveItem>();
   COLclosure0* pCallback = COLnewClosure0(&SDBSretrievePostToMain, pReport, Address);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBSretrieveSyncImpl, Params, pReport, pCallback));
}