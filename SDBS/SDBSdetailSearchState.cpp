//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAlogDetailSearchState.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 1:14p.m.
//  ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <CHNK/CHNKrenderChunks.h>
#include <CHNK/CHNKsplitIntoChunks.h>
#include <CHNK/CHNKviewString.h>
#include <CHNK/CHNKwriteChunk.h>
#include <CHNK/CHNKwriteStringVar.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <MATCH/MATCHhighlight.h>
#include <MATCH/MATCHsearch.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBmessage.h>
#include <SDBS/SDBSdetailSearchState.h>
COL_LOG_MODULE;

static void SDBSdetailReportOnMain(const bool Success, std::shared_ptr<COLvar> pReport,
                                   std::shared_ptr<SDBSdetailSearchArgs> pArgs, SDBSdetailSearchCallback pCallback) {
   pCallback->run(Success, pReport, pArgs);
}

static void SDBSfilterChunksAndHighlight(const COLstring& Search, int Remaining, const CHNKviewMode& ViewMode,
                                         COLvar* pReport) {
   COL_FUNCTION(SDBSfilterChunksAndHighlight);
   if(!pReport->exists("Chunks") || !(*pReport)["Chunks"].isArray()) { return; }
   const bool RenderUrls = ViewMode == CHNK_VIEW_PLAIN_TEXT;
   int        Count      = 0;
   auto&      Chunks     = (*pReport)["Chunks"].array();
   for(auto it = Chunks.begin(); it != Chunks.end();) {
      if(Remaining <= 0) {
         it = Chunks.erase(it);
         continue;
      }
      MATCHpositions Positions;
      MATCHsearch(Search, (*it)["Value"].asString(), false, &Positions, Remaining);
      COL_VAR(Positions.size());
      if(Positions.size() > 0) {
         Remaining -= Positions.size();
         Count += Positions.size();
         (*it)["Value"] = MATCHgenerateMarkup((*it)["Value"].asString(), Positions, RenderUrls, false);
         ++it;
      } else {
         it = Chunks.erase(it);
      }
   }
   (*pReport)["count"] = Count;
}

static void SDBSgetChunkIncrements(const COLstring& Message, const COLuint64 ChunkId, const int Increment,
                                   const CHNKviewMode& ViewMode, COLvar* pReport) {
   COL_FUNCTION(SDBSgetChunkIncrements);
   COLstring                     ResultString;
   COLarray<CHNKstringArgChunk> Chunks;
   CHNKrenderViewString(Message, ViewMode, &ResultString);
   CHNKsplitIntoChunks(ResultString, &Chunks);
   COLarray<CHNKrenderedChunk> RenderedChunks;
   for(int i = ChunkId, count = 0; i < Chunks.size() && count < Increment; i++, count++) {
      COL_VAR("ChunkId=" << ChunkId);
      CHNKrenderChunks(Chunks, i, 0, RenderedChunks);
   }
   COL_VAR(RenderedChunks.size());
   CHNKwriteChunkVar(RenderedChunks, Chunks, pReport);
}

void SDBSdetailSearchWorker(std::shared_ptr<SDBSdetailSearchArgs> pArgs, SDBSdetailSearchCallback pCallback, SCKloop* pLoop) {
   COL_FUNCTION(SDBSdetailSearchWorker);
   COLstring Err;
   auto        pReport  = std::make_shared<COLvar>();
   const auto* pMessage = SDBUextractMessage(pArgs->m_Component, pArgs->m_Index, pArgs->m_Time, &Err);
   const bool  Success  = pMessage != nullptr;
   if(Success) {
      SDBUcontentToVar(*pMessage, pReport.get());
      SDBSgetChunkIncrements(pMessage->Data, pArgs->m_ChunkId, pArgs->m_Increment, pArgs->m_ViewMode, pReport.get());
      SDBSfilterChunksAndHighlight(pArgs->m_Search, pArgs->m_Remaining, pArgs->m_ViewMode, pReport.get());
      delete pMessage;
   } else {
      COL_TRC("Error getting the message: " << Err);
      (*pReport)["error"] = Err;
   }
   COL_TRC("Report back to main");
   pLoop->post(COLnewClosure0(&SDBSdetailReportOnMain, Success, pReport, pArgs, pCallback));
}