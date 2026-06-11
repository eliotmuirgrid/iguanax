// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentResponse
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 01 May 2023 - 01:19PM
// ---------------------------------------------------------------------------

#include <TRN/TRNargumentResponse.h>
#include <THTM/THTMprinter.h>
#include "TRNrenderNodTree.h"
#include <CHNK/CHNKrenderChunks.h>
#include "TRNwriteStringVar.h"

#include <CHNK/CHNKsplitIntoChunks.h>
#include <CHNK/CHNKviewString.h>
#include <SFI/SFIhtmlEscape.h>
#include <DIS/DISbinaryDisplay.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNargumentPopulateResponse(TRNarg& Arg, TRNargRequest& Request){
   COL_FUNCTION(TRNargumentPopulateResponse);
   if (Arg.m_ViewMode != Request.m_ViewMode){
      Arg.m_ViewMode = Request.m_ViewMode;
      CHNKrenderViewString(Arg.m_OriginalString, Arg.m_ViewMode, &Arg.m_ViewString);
      CHNKsplitIntoChunks(Arg.m_ViewString, &Arg.m_Chunks);
   }
   if (TRNargIsTree(Arg)){
      COL_TRC("Tree type request.");
      COLPRECONDITION(Arg.m_pTree.get());
      TRNrenderNodTree(*Arg.m_pTree, Arg, Request.m_RootAddressJson, Request.m_ExpansionTreeJson, &Request.m_VarResult);
   } else {
      COL_TRC("Scalar type request.");
      COLarray<CHNKrenderedChunk> RenderedChunks;
      CHNKrenderChunks(Arg.m_Chunks, Request.m_FirstChunk, Request.m_SecondChunk, RenderedChunks); // new lib
      TRNwriteStringVar(RenderedChunks, Arg, &Request.m_VarResult);                      // new lib
   }
}

void TRNargumentWriteResponses(const COLarray<TRNargRequest>& FullArgumentRequests, COLvar* pResponse){
   COL_FUNCTION(TRNargumentWriteResponses);
   COLvar FullArgResp;
   COL_VAR(FullArgumentRequests.size());
   for (int i = 0, n = FullArgumentRequests.size(); i < n; i++){
      const TRNargRequest& Request = FullArgumentRequests[i];
      if (Request.m_VarResult.isNull()) {
         COL_TRC("Result was null for request " << Request.m_Id);
         // We must not have reached the desired point of the script.
         COLvar ErrorVar;
         ErrorVar["ErrorMessage"] = "Unknown error.";
         FullArgResp[Request.m_Id] = ErrorVar;
      } else {
         COL_TRC("Successfully got result for request " << Request.m_Id);
         FullArgResp[Request.m_Id] = Request.m_VarResult;
      }
   }
   (*pResponse)["FullArgumentResponses"].swap(&FullArgResp);
}
