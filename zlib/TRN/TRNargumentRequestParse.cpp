//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentRequestParse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//---------------------------------------------------------------------------
#include "TRNargumentRequestParse.h"
#include "TRNargumentRequest.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNargumentPopulate(const COLvar& ArgRequestData, TRNargRequest* pRequest){
   COL_FUNCTION(TRNargumentPopulate);
   TRNargRequest& Out = *pRequest;
   Out.m_Id = ArgRequestData["id"];
   TRNargumentRequestSetMembersFromId(Out);
   if (ArgRequestData.exists("view_mode"))     { TRNargumentRequestSetViewMode(Out, ArgRequestData["view_mode"]);}
   if (ArgRequestData.exists("chunk"))         { Out.m_FirstChunk        = ArgRequestData["chunk"]          ;}
   if (ArgRequestData.exists("extra_chunk"))   { Out.m_SecondChunk       = ArgRequestData["extra_chunk"]    ;}
   if (ArgRequestData.exists("expansion_tree")){ Out.m_ExpansionTreeJson = ArgRequestData["expansion_tree"].json(); } // The .json() seems really strange, but is needed because the new WEBserver implementation implicity parses all incoming requests and we need the stringified version
   if (ArgRequestData.exists("root_address"))  { Out.m_RootAddressJson   = ArgRequestData["root_address"]; }
}

void TRNargumentRequestParse(const COLvar& JsonArray, COLarray<TRNargRequest>* pFullArgumentRequests){
   COL_FUNCTION(TRNargumentRequestParse);
   if(!JsonArray.isArray()) { return; }
   COLarray<TRNargRequest>& FullArgumentRequests = *pFullArgumentRequests;
   COLvar::COLvarVec RequestArray = JsonArray.array();
   COL_VAR(RequestArray.size());
   int CountOfRequests = RequestArray.size();
   for (int i = 0; i < CountOfRequests; i++){
      TRNargRequest& Request = FullArgumentRequests.push_back();
      const COLvar& ArgRequest = RequestArray[i];
      TRNargumentPopulate(ArgRequest, &Request);
      COL_VAR2(i, Request);
   }
   COL_TRC("We have " << CountOfRequests << " full argument requests.");
}
