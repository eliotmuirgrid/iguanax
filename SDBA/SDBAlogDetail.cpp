//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAlogDetail.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/10/23 11:15 AM
//  ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <CHNK/CHNKrenderChunks.h>
#include <CHNK/CHNKsplitIntoChunks.h>
#include <CHNK/CHNKviewString.h>
#include <CHNK/CHNKwriteChunk.h>
#include <CHNK/CHNKwriteStringVar.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <MATCH/MATCHhighlight.h>
#include <PRM/PRMcallbacks.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBepochTime.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBserviceLog.h>
#include <SDBA/SDBAlogDetail.h>
COL_LOG_MODULE;

static void SDBAchunkStringVar(const COLstring& OriginalMessage, const CHNKviewMode& ViewMode, const int ChunkId,
                               const int ChunkIdExtra, COLvar* pResult) {
   COL_FUNCTION(SDBAchunkStringVar);
   COL_VAR(OriginalMessage);
   COLstring                     ResultString;
   COLarray<CHNKstringArgChunk> Chunks;
   CHNKrenderViewString(OriginalMessage, ViewMode, &ResultString);
   CHNKsplitIntoChunks(ResultString, &Chunks);
   COLarray<CHNKrenderedChunk> RenderedChunks;
   CHNKrenderChunks(Chunks, ChunkId, ChunkIdExtra, RenderedChunks);
   CHNKwriteChunkVar(RenderedChunks, Chunks, pResult);
   if(ViewMode != CHNK_VIEW_PLAIN_TEXT || !pResult->exists("Chunks") || !(*pResult)["Chunks"].isArray()) { return; }
   for(auto& it : (*pResult)["Chunks"].array()) {
      COLstring Out;
      MATCHsearchAndCreateMarkup("", it["Value"].asString(), false, &Out, true, -1, -1, false);
      it["Value"] = Out;
   }
}

static bool SDBAlogFileExists(const COLstring& Component, COLuint64 Time, COLstring* pLogFile) {
   *pLogFile = SDBfindLogFile(Component, Time, SDBroot());
   COL_VAR(*pLogFile);
   if (SDBroot2().is_null()) { return ! pLogFile->is_null(); }
   if (!pLogFile->is_null()) { return true; }
   *pLogFile = SDBfindLogFile(Component, Time, SDBroot2());
   COL_VAR(*pLogFile);
   return ! pLogFile->is_null();
}

static void SDBAlogViewImpl(const int Index, const COLstring& Component, const COLstring& Filter,
                            const COLuint64 Time, const COLuint64 ChunkId, const COLuint64 ChunkIdExtra,
                            const CHNKviewMode& ViewMode, const bool WantRaw, const COLaddress& Address) {
   COL_FUNCTION(SDBAlogViewImpl);
   COLstring LogFile;
   if (!SDBAlogFileExists(Component, Time, &LogFile)) {
      return COLrespondError(Address, "Log file not found");
   }
   COLvar Result;
   Result["file"]      = LogFile;
   Result["component"] = Component;
   COLstring   Err;
   const auto* pMessage = SDBUextractMessage(Component, Index, Time, &Err);
   const bool  Success  = pMessage != nullptr;
   if(Success) {
      COL_VAR("Found the message");
      SDBUcontentToVar(*pMessage, &Result);
      if(WantRaw) {
         Result["content"] = pMessage->Data;
      } else {
         SDBAchunkStringVar(pMessage->Data, ViewMode, ChunkId, ChunkIdExtra, &Result);
      }
      delete pMessage;
   } else {
      COL_TRC("Error getting the message: " << Err);
      return COLrespondError(Address, "Couldn't find this message.");
   }
   COLrespondSuccess(Address, Result);
}

// /log/detail
void SDBAlogDetail(const COLwebRequest& Request) {
   COL_FUNCTION(SDBAlogDetail);
   COL_VAR(Request.Data);
   const COLvar& Data  = Request.Data;
   COLstring ComponentId, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &ComponentId, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   if (ComponentId != SDB_SERVICE_COMPONENT_ID &&!PRMcheckRoleAccess(false, Request.User, ComponentId, ROLid::VIEW_LOGS, &Error)){
      COL_VAR(Error);
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_LOGS));
   }
   COLstring    Filter           = COLvarGetString(Data, "filter", "");
   COLstring    ViewModeArg      = COLvarGetString(Data, "view_mode", "");
   CHNKviewMode ViewMode         = CHNKviewModeArg(ViewModeArg);
   COLstring    ChunkString      = COLvarGetString(Data, "chunk", "0");
   COLstring    ChunkStringExtra = COLvarGetString(Data, "chunk_extra", "0");
   COLuint64    ChunkId          = atoll(ChunkString.c_str());
   COLuint64    ChunkIdExtra     = atoll(ChunkStringExtra.c_str());
   COLstring    TimeString       = COLvarGetString(Data, "time", "0");
   COLuint64    Time             = atoll(TimeString.c_str());
   if(ChunkIdExtra < 0) return COLrespondError(Request.Address, "Chunk_extra parameter must be positive");
   if(ChunkId < 0) return COLrespondError(Request.Address, "Chunk parameter must be positive");
   if(Time    < 0) return COLrespondError(Request.Address, "Time parameter must be positive");
   int Index = COLvarGet(Data, "id",   0);
   bool WantRaw = COLvarGetBool(Data, "raw", false); // IX-1801 flag to not trim and escape the data so we can get the complete thing.
   COL_VAR3(Index, ComponentId, Time);
   SDBAlogViewImpl(Index, ComponentId, Filter, Time, ChunkId, ChunkIdExtra, ViewMode, WantRaw, Request.Address);
}
