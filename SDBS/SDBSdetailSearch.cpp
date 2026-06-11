//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSdetailSearch.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 1:14p.m.
//  ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <CHNK/CHNKviewString.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GUID/GUIDid.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBserviceLog.h>
#include <SDBS/SDBSdetailSearch.h>
#include <SDBS/SDBSdetailSearchState.h>
COL_LOG_MODULE;

static void SDBSdetailSearchReport(bool Success, std::shared_ptr<COLvar> pReport,
                                   std::shared_ptr<SDBSdetailSearchArgs> pArgs, COLstring User,
                                   COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap, SCKloop* pLoop);

static void SDBSdetailSearchStopAndClear(SDBSdetailSearchItem* pItem) {
   COL_FUNCTION(SDBSdetailSearchStopAndClear);
   pItem->m_TaskGuid = "";
   pItem->m_Count    = -1;
   pItem->m_Running  = false;
   pItem->m_Result.reset();
}

static void SDBSdetailSearchSetupItem(SDBSdetailSearchItem* pItem, const COLstring& TaskGuid) {
   COL_FUNCTION(SDBSdetailSearchSetupItem);
   pItem->m_TaskGuid = TaskGuid;
   pItem->m_Count    = 0;
   pItem->m_Running  = true;
   pItem->m_Result.setArrayType();
}

static void SDBScreateSearchWorker(std::shared_ptr<SDBSdetailSearchArgs> pArgs, const COLstring& User,
                                   COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap, SCKloop* pLoop) {
   SDBSdetailSearchCallback pReportCallback = COLnewClosure3(&SDBSdetailSearchReport, User, pSearchMap, pLoop);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBSdetailSearchWorker, pArgs, pReportCallback, pLoop));
}

static void SDBSdetailSearchReport(const bool Success, std::shared_ptr<COLvar> pReport,
                                   std::shared_ptr<SDBSdetailSearchArgs> pArgs, COLstring User,
                                   COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap, SCKloop* pLoop) {
   COL_FUNCTION(SDBSdetailSearchReport);
   COL_VAR(pReport->json(1));
   SDBSdetailSearchItem& Item        = (*pSearchMap)[User];
   const int             TotalChunks = (*pReport)["TotalChunks"].asInt();
   const COLvar&         Chunks      = (*pReport)["Chunks"];
   const int             Count       = (*pReport)["count"].asInt();
   Item.m_Count += Count;
   pArgs->m_Remaining -= Count;
   COL_VAR3(pArgs->m_ChunkId, TotalChunks, Item.m_Count);

   if(Item.m_TaskGuid != pArgs->m_TaskGuid) {
      COL_TRC("Cleanup due to wrong task");
      Item.m_Result.reset();
      return;
   }

   if(Chunks.size() > 0) {
      COL_TRC("Appending " << Chunks.size() << " new results");
      for(int i = 0; i < Chunks.size(); i++) { Item.m_Result.push_back(Chunks[i]); }
   }

   Item.m_Running = Success && pArgs->m_ChunkId < TotalChunks && pArgs->m_Remaining > 0;
   if(Item.m_Running) {
      pArgs->increment();
      COL_TRC("More chunks to go through, currently on " << pArgs->m_ChunkId);
      SDBScreateSearchWorker(pArgs, User, pSearchMap, pLoop);
   }
}

// /log/detail/search/start
void SDBSdetailSearchStart(const COLwebRequest& Request, COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap,
                              SCKloop* pLoop) {
   COL_FUNCTION(SDBSdetailSearchStart);
   COL_VAR(Request.Data);
   COLstring ComponentId, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &ComponentId, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   if(ComponentId != SDB_SERVICE_COMPONENT_ID && !PRMcheckRoleAccess(false, Request.User, ComponentId, VIEW_LOGS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(VIEW_LOGS));
   }
   const COLstring Search      = COLvarGetString(Request.Data, "search", "");
   const int       Index       = COLvarGet(Request.Data, "id", 0);
   const COLuint64 Time        = atoll(COLvarGetString(Request.Data, "time", "0").c_str());
   COLuint64       ChunkId     = atoll(COLvarGetString(Request.Data, "chunk", "0").c_str());
   CHNKviewMode    ViewMode    = CHNKviewModeArg(COLvarGetString(Request.Data, "view_mode", ""));
   if(pSearchMap->count(Request.User)) {
      COL_TRC("User " << Request.User << " started a new search. Clean up old data.");
      SDBSdetailSearchStopAndClear(&(*pSearchMap)[Request.User]);
   }
   if(ViewMode != CHNK_VIEW_PLAIN_TEXT && ViewMode != CHNK_VIEW_ESCAPED_TEXT) {
      return COLrespondError(Request.Address, "Searching is only supported in plain or escaped text view");
   }
   COL_VAR3(Index, ComponentId, Time);
   COL_TRC("Setting up search state");
   const COLstring TaskGuid = GUIDnewShortId();
   SDBSdetailSearchSetupItem(&(*pSearchMap)[Request.User], TaskGuid);
   auto pArgs = std::make_shared<SDBSdetailSearchArgs>(ComponentId, Index, TaskGuid, Time, Search, ChunkId, ViewMode);
   SDBScreateSearchWorker(pArgs, Request.User, pSearchMap, pLoop);
   COL_TRC("Search job started - poll for results");
   COLrespondSuccess(Request.Address);
}

// /log/detail/search/poll
void SDBSdetailSearchPoll(const COLwebRequest& Request, COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap) {
   COL_FUNCTION(SDBSdetailSearchPoll);
   SDBSdetailSearchItem& Item = (*pSearchMap)[Request.User];
   COLvar                Result;
   Result["search"] = Item.m_Result;
   Result["count"]  = Item.m_Count == -1 ? "" : COLint32ToString(Item.m_Count);
   Result["done"]   = !Item.m_Running;
   Item.m_Result.reset();
   Item.m_Result.setArrayType();
   COLrespondSuccess(Request.Address, Result);
}
