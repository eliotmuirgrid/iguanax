//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LX12parse.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/8/23 1:56 PM
//  ---------------------------------------------------------------------------
#include <LX12/LX12parse.h>
#include <LX12/LX12node.h>

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIparse.h>
#include <EDI/EDIx12.h>
#include <FIL/FILutils.h>
#include <LEDI/LEDIcache.h>
#include <LEDI/LEDIutils.h>
#include <LND/LNDutils.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

static void LX12pushWarnings(lua_State* L, const COLlist<COLstring>& MissingNodes, const COLlist<COLstring>& SkippedNodes) {
   COL_FUNCTION(LX12pushWarnings);
   COLvar Warnings;
   for (const auto & i : MissingNodes) {
      Warnings["missing"].push_back(i);
   }
   for (const auto & i : SkippedNodes) {
      Warnings["skipped"].push_back(i);
   }
   Warnings.toLua(L);
}

int LX12parseMessage(lua_State* L) {
   COL_FUNCTION(LX12parseMessage);
   COLstring Data = LUAexpectedStringParam(L, 1, "data");
   COLstring Vmd  = LEDIresolveVmdFile(L);
   COL_VAR(Vmd);
   if(!FILfileExists(Vmd)) { LUAraiseLuaError(L, "Unable to find " + Vmd); }

   bool LegacyWarning = false;
   COLstring ConversionWarning;
   EDIcollection* pCollection = LEDIcacheGetEdiGrammar(L, Vmd, &LegacyWarning, &ConversionWarning);
   int MessageCount = 0;
   for(const auto& it : pCollection->Nodes) {
      if(it->second->Type == EDItype::EDI_MESSAGE && !it->first.equals("ignore")) { MessageCount++; }
   }
   COLstring Message = MessageCount > 1 ? LUAexpectedStringParam(L, 1, "name") : "";

   COLstring Error;
   if(!EDIx12Parse(*pCollection, &Data, &Message, &Error) || !Error.is_null()) {  // if error not null parsing was successful multiple messages
      LUAraiseLuaError(L, Error);
      return 0;
   }

   try {
      COLref<EDIdata> pRoot = new EDIdata(*pCollection, Message);
      COLref<NODplace> pMessage = new LX12node(pRoot.get());  // Important that this happens directly after creation of pRoot so we don't leak memory.
      // Obtain a map of valid segments
      COLhashmap<COLstring, bool> MessageSegments;
      EDIgetSegmentsInMessage(pCollection, pCollection->Nodes[Message], &MessageSegments);
      COL_VAR(MessageSegments.size());
      COL_TRC_DUMP("Message:", Data.c_str(), Data.size());
      COLlist<COLstring> SkippedNodes;
      COLarray<COLstring> Here;
      Here.push_back(pRoot->name());
      int Consumed = EDIparseSegmentCollection(pRoot.get(), Data.c_str(), Data.size(),MessageSegments, SkippedNodes, &Here);
      Here.pop_back();
      COL_VAR(Consumed);
      COLlist<COLstring> MissingNodes;
      COLvar Missing;
      EDIfindMissingSegments(pRoot.get(), pCollection->Nodes[Message], MissingNodes, Missing);
      COL_VAR(MissingNodes.size());
      LNDpushNode(L, pMessage.get());
      LUApushString(L, pMessage->name());
      Missing.toLua(L);
      // LX12pushWarnings(L, MissingNodes, SkippedNodes);
   } catch (COLerror& Error) {
      COL_TRC("Error thrown: " << Error.description());
      LUAraiseLuaError(L, Error.description());
   }
   return 3;
}