// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7parse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 26 October 2022 - 02:02PM
// ---------------------------------------------------------------------------
#include <LHL7/LHL7parse.h>
#include <LHL7/LHL7node.h>

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIhl7.h>
#include <EDI/EDIparse.h>

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

static void LHL7pushWarnings(lua_State* L, const COLlist<COLstring>& MissingNodes, const COLlist<COLstring>& SkippedNodes) {
   COL_FUNCTION(LHL7pushWarnings);
   COLvar Warnings;
   for (const auto & i : MissingNodes) {
      Warnings["missing"].push_back(i);
   }
   for (const auto & i : SkippedNodes) {
      Warnings["skipped"].push_back(i);
   }
   Warnings.toLua(L);
}

int LHL7parseMessage(lua_State* L){
   COL_FUNCTION(LHL7parseMessage);
   COLstring Name = LUAoptionalStringParam(L, 1, "name");
   COLstring Data = LUAexpectedStringParam(L, 1, "data");
   COLstring Vmd  = LEDIresolveVmdFile(L) ;
   COL_VAR2(Name, Vmd);
   if(!FILfileExists(Vmd)) { LUAraiseLuaError(L, "Unable to find " + Vmd); }

   bool LegacyWarning = false;
   COLstring ConversionWarning;
   EDIcollection* pCollection = LEDIcacheGetEdiGrammar(L, Vmd, &LegacyWarning, &ConversionWarning);

   COLstring Error;
   if(!EDIparseMsh(*pCollection, &Data, &Name, &Error) || !Error.is_null()) {  // if error not null parsing was successful but cannot match name
      LUAraiseLuaError(L, Error);
      return 0;
   }

   try {
      // If we get an exception this means that EDIdata should be okay
      COLref<EDIdata> pRoot = new EDIdata(*pCollection, Name);
      COLref<NODplace> pMessage = new LHL7node(pRoot.get());  // Important that this happens directly after creation of pRoot so we don't leak memory.
      // Obtain a map of valid segments
      COLhashmap<COLstring, bool> MessageSegments;
      EDIgetSegmentsInMessage(pCollection, pCollection->Nodes[Name], &MessageSegments);
      COL_VAR(MessageSegments.size());
      COL_TRC_DUMP("Message:", Data.c_str(), Data.size());
      // create list
      COLlist<COLstring> SkippedNodes;
      COLarray<COLstring> Here;
      Here.push_back(pRoot->name());
      int Consumed = EDIparseSegmentCollection(pRoot.get(), Data.c_str(), Data.size(),MessageSegments, SkippedNodes, &Here);
      Here.pop_back();
      COL_VAR(Consumed);
      EDIpostProcess(pRoot.get());
      COLlist<COLstring> MissingNodes;
      COLvar Missing;
      EDIfindMissingSegments(pRoot.get(), pCollection->Nodes[Name], MissingNodes, Missing);
      COL_VAR(MissingNodes.size());
      LNDpushNode(L, pMessage.get());
      COL_VAR(pMessage->name());
      LUApushString(L, pMessage->name());
      Missing.toLua(L);
      // LHL7pushWarnings(L, MissingNodes, SkippedNodes);
   } catch (COLerror& Error) {
      COL_TRC("Error thrown: " << Error.description());
      LUAraiseLuaError(L, Error.description());
   }

   if(!LegacyWarning) { return 3; }
   COLstring Warning = "⚠️Warning: Legacy vmd file format detected. It is recommended to use the importer and convert the file to the new format for future use.";
   COL_TRC("Returned the table node and the legacy vmd file warning.");
   if(!ConversionWarning.is_null()) {
      Warning = Warning + FIL_NEWLINE_CHARACTER + "⚠️Warning: " + ConversionWarning;
   }
   LUApushString(L, Warning);
   return 4; // node + warning
}